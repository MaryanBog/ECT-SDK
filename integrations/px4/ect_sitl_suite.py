#!/usr/bin/env python3
import csv
import math
import re
import subprocess
import time
from pathlib import Path

ROOT = Path.cwd()
OUT = ROOT / "Tools/ect_tests/results"
OUT.mkdir(parents=True, exist_ok=True)

RAW = OUT / "ect_sitl_suite_raw.csv"
SUMMARY = OUT / "ect_sitl_suite_summary.csv"
MODE_FILE = Path("/tmp/ect_test_mode")

LISTENER = ROOT / "build/px4_sitl_default/bin/px4-listener"

TARGET = (0.0, 0.0, -2.5)
ROWS = []
SUM = []

def sh(args):
    return subprocess.run(args, cwd=ROOT, text=True,
                          stdout=subprocess.PIPE,
                          stderr=subprocess.STDOUT).stdout

def set_mode(mode):
    MODE_FILE.write_text(str(mode))
    time.sleep(0.25)

def pos():
    out = sh([str(LISTENER), "vehicle_local_position", "-n", "1"])

    def val(name):
        m = re.search(rf"^\s*{name}:\s*([-+0-9.eE]+)", out, re.M)
        if not m:
            raise RuntimeError(f"missing {name}\n{out}")
        return float(m.group(1))

    m = re.search(r"^\s*timestamp:\s*(\d+)", out, re.M)
    if not m:
        raise RuntimeError(out)

    return {
        "timestamp": int(m.group(1)),
        "x": val("x"), "y": val("y"), "z": val("z"),
        "vx": val("vx"), "vy": val("vy"), "vz": val("vz"),
    }

def speed(p):
    return math.sqrt(p["vx"]**2 + p["vy"]**2 + p["vz"]**2)

def dist_xyz(p, q):
    return math.sqrt(
        (p["x"]-q[0])**2 +
        (p["y"]-q[1])**2 +
        (p["z"]-q[2])**2
    )

def noise(t):
    return (
        0.15*math.sin(7*t) + 0.07*math.sin(19*t),
        0.15*math.sin(8*t) + 0.07*math.sin(23*t),
        0.10*math.sin(6*t) + 0.05*math.sin(17*t),
    )

def dyn_target(t):
    return (
        math.sin(0.25*t),
        math.cos(0.25*t),
        -2.5 + 0.5*math.sin(0.18*t),
    )

def clamp(v):
    return max(-2.0, min(2.0, v))

def record(phase, mode, force=(0,0,0), baseline=None):
    p = pos()
    t = p["timestamp"] * 1e-6

    if mode == 1:
        n = noise(t)
        target = TARGET
    elif mode == 2:
        n = (0.0, 0.0, 0.0)
        target = dyn_target(t)
    else:
        n = (0.0, 0.0, 0.0)
        target = TARGET

    dx = target[0] - p["x"] + n[0]
    dy = target[1] - p["y"] + n[1]
    dz = target[2] - p["z"] + n[2]

    ex = clamp(1.2*dx)
    ey = clamp(1.2*dy)
    ez = clamp(1.2*dz)

    err = dist_xyz(p, target)
    disp = dist_xyz(p, baseline) if baseline else float("nan")

    row = {
        "px4_timestamp_us": p["timestamp"],
        "phase": phase,
        "mode": mode,
        "force_x": force[0],
        "force_y": force[1],
        "force_z": force[2],
        "target_x": target[0],
        "target_y": target[1],
        "target_z": target[2],
        "noise_x": n[0],
        "noise_y": n[1],
        "noise_z": n[2],
        "x": p["x"], "y": p["y"], "z": p["z"],
        "vx": p["vx"], "vy": p["vy"], "vz": p["vz"],
        "speed": speed(p),
        "target_error": err,
        "baseline_displacement": disp,
        "ect_x": ex, "ect_y": ey, "ect_z": ez,
        "sat_x": int(abs(ex) >= 1.999),
        "sat_y": int(abs(ey) >= 1.999),
        "sat_z": int(abs(ez) >= 1.999),
    }
    ROWS.append(row)
    return p, row

def apply_force(f):
    msg = (
        'entity: {name: "x500_0", type: MODEL} '
        f'wrench: {{force: {{x: {f[0]}, y: {f[1]}, z: {f[2]}}}}}'
    )
    sh(["gz", "topic", "-t", "/world/default/wrench/persistent",
        "-m", "gz.msgs.EntityWrench", "-p", msg])

def clear_force():
    sh(["gz", "topic", "-t", "/world/default/wrench/clear",
        "-m", "gz.msgs.Entity", "-p",
        'name: "x500_0", type: MODEL'])

def wait_stable(target=TARGET, timeout=60.0):
    start = time.monotonic()
    stable_since = None

    while time.monotonic() - start < timeout:
        p, _ = record("STABILIZE", 0)

        ok = dist_xyz(p, target) <= 0.15 and speed(p) <= 0.08

        if ok:
            if stable_since is None:
                stable_since = time.monotonic()
            elif time.monotonic() - stable_since >= 3.0:
                return p
        else:
            stable_since = None

        time.sleep(0.10)

    raise RuntimeError("stabilization timeout")

def wait_recovery(name, baseline, timeout=60.0):
    start = time.monotonic()
    stable_since = None

    while time.monotonic() - start < timeout:
        p, _ = record(name+"_RECOVERY", 0, baseline=baseline)

        ok = dist_xyz(p, baseline) <= 0.10 and speed(p) <= 0.08

        if ok:
            if stable_since is None:
                stable_since = time.monotonic()
            elif time.monotonic() - stable_since >= 2.0:
                return time.monotonic() - start
        else:
            stable_since = None

        time.sleep(0.10)

    raise RuntimeError(name+" recovery timeout")

def disturbance(name, force, duration):
    baseline_p = wait_stable()
    baseline = (baseline_p["x"], baseline_p["y"], baseline_p["z"])

    apply_force(force)
    end = time.monotonic() + duration
    peak_disp = 0.0
    peak_err = 0.0

    while time.monotonic() < end:
        _, r = record(name+"_FORCE", 0, force, baseline)
        peak_disp = max(peak_disp, r["baseline_displacement"])
        peak_err = max(peak_err, r["target_error"])
        time.sleep(0.10)

    clear_force()
    recovery = wait_recovery(name, baseline)

    SUM.append({
        "test": name,
        "peak_displacement_m": peak_disp,
        "peak_target_error_m": peak_err,
        "mean_error_m": "",
        "max_error_m": "",
        "recovery_s": recovery,
        "saturation_samples": "",
    })

    print(f"{name}: peak_disp={peak_disp:.3f} m  "
          f"peak_err={peak_err:.3f} m  recovery={recovery:.3f} s")

def sample_noise():
    set_mode(1)
    time.sleep(1.0)

    vals = []
    end = time.monotonic() + 15.0

    while time.monotonic() < end:
        _, r = record("NOISE", 1)
        vals.append(r["target_error"])
        time.sleep(0.10)

    set_mode(0)

    mean_e = sum(vals)/len(vals)
    max_e = max(vals)

    SUM.append({
        "test": "NOISE",
        "peak_displacement_m": "",
        "peak_target_error_m": "",
        "mean_error_m": mean_e,
        "max_error_m": max_e,
        "recovery_s": "",
        "saturation_samples": "",
    })

    print(f"NOISE: mean_error={mean_e:.3f} m  max_error={max_e:.3f} m")

def saturation_test():
    wait_stable()
    baseline_p = pos()
    baseline = (baseline_p["x"], baseline_p["y"], baseline_p["z"])

    force = (3.0, 3.0, 1.0)
    apply_force(force)

    end = time.monotonic() + 10.0
    peak_disp = 0.0
    peak_err = 0.0
    sat_samples = 0

    while time.monotonic() < end:
        _, r = record("SATURATION_FORCE", 0, force, baseline)
        peak_disp = max(peak_disp, r["baseline_displacement"])
        peak_err = max(peak_err, r["target_error"])
        sat_samples += int(r["sat_x"] or r["sat_y"] or r["sat_z"])
        time.sleep(0.10)

    clear_force()
    # Strong saturation can shift the post-disturbance equilibrium relative
    # to the pre-force baseline. Recovery is therefore measured to the commanded
    # fixed target, using the same position/speed criterion as stabilization.
    recovery_start = time.monotonic()
    wait_stable(TARGET, timeout=90.0)
    recovery = time.monotonic() - recovery_start

    SUM.append({
        "test": "SATURATION",
        "peak_displacement_m": peak_disp,
        "peak_target_error_m": peak_err,
        "mean_error_m": "",
        "max_error_m": "",
        "recovery_s": recovery,
        "saturation_samples": sat_samples,
    })

    print(f"SATURATION: peak_disp={peak_disp:.3f} m  "
          f"peak_err={peak_err:.3f} m  recovery={recovery:.3f} s  "
          f"sat_samples={sat_samples}")

def tracking_test():
    set_mode(2)
    time.sleep(1.0)

    vals = []
    end = time.monotonic() + 30.0

    while time.monotonic() < end:
        _, r = record("TRACKING", 2)
        vals.append(r["target_error"])
        time.sleep(0.10)

    set_mode(0)

    mean_e = sum(vals)/len(vals)
    max_e = max(vals)

    SUM.append({
        "test": "TRACKING",
        "peak_displacement_m": "",
        "peak_target_error_m": "",
        "mean_error_m": mean_e,
        "max_error_m": max_e,
        "recovery_s": "",
        "saturation_samples": "",
    })

    print(f"TRACKING: mean_error={mean_e:.3f} m  max_error={max_e:.3f} m")

def save():
    if ROWS:
        with RAW.open("w", newline="") as f:
            w = csv.DictWriter(f, fieldnames=ROWS[0].keys())
            w.writeheader()
            w.writerows(ROWS)

    if SUM:
        with SUMMARY.open("w", newline="") as f:
            w = csv.DictWriter(f, fieldnames=SUM[0].keys())
            w.writeheader()
            w.writerows(SUM)

def main():
    set_mode(0)
    clear_force()

    print("=== ECT SITL ONE-FLIGHT SUITE ===")

    disturbance("X_POS_0.5N", (0.0, 0.5, 0.0), 2.0)
    disturbance("Y_POS_0.5N", (0.5, 0.0, 0.0), 2.0)
    disturbance("Z_0.5N",     (0.0, 0.0, 0.5), 2.0)
    disturbance("XYZ_0.5N",   (0.5, 0.5, 0.5), 2.0)

    sample_noise()

    set_mode(0)
    wait_stable()

    saturation_test()

    set_mode(0)
    wait_stable()

    tracking_test()

    set_mode(0)
    clear_force()

    print(f"RAW: {RAW}")
    print(f"SUMMARY: {SUMMARY}")

if __name__ == "__main__":
    try:
        main()
    finally:
        try:
            set_mode(0)
            clear_force()
        except Exception:
            pass
        save()
