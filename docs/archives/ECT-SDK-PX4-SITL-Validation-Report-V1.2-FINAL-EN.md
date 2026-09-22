# ECT-SDK --- PX4 SITL Validation Report

**Version:** 1.2  
**Date:** 21 September 2026

## 1. Scope

This report documents PX4/Gazebo SITL validation of the ECT-SDK per-axis
controller integration with an explicit velocity-damping adapter.

Version 1.2 preserves the previously recorded disturbance-rejection comparison
between **Stock PX4** (the standard PX4 position controller, without ECT) and ECT, and extends the validation
with a single-flight ECT suite covering:

- fixed-target disturbance rejection;
- deterministic measurement/input-noise injection;
- strong-disturbance saturation and post-saturation recovery;
- dynamic three-axis trajectory tracking.

The report describes the behavior of the tested integration only. It does not
claim a general proof of closed-loop stability or general superiority over PID
or the PX4 controller.

## 2. Controller Configuration

The tested ECT integration uses three independent scalar ECT channels, one for
each PX4 local-position axis:

`Δ → F → E → F⁻¹ → G → ECT output`

The ECT output is combined with explicit velocity damping and supplied to PX4
as an acceleration setpoint. PX4's lower-level acceleration-to-thrust,
attitude, rate, motor, and Gazebo plant dynamics remain external to ECT.

Configuration is separated below between the **ECT core**, the **PX4 integration adapter**, and the **shared PX4 environment** so that adapter tuning is not attributed to ECT itself.

**ECT core parameters:**

- **F:** identity (no tunable parameter in this test)
- **E:** linear contraction, `α = 0.8`
- **F⁻¹:** identity (no tunable parameter in this test)
- **G:** gain `1.5`, saturation `[-2.0, +2.0]`
- Unsaturated ECT position-error slope: `1.5 × 0.8 = 1.2`

Thus, the tested ECT core had only three numerical tuning choices: contraction `α`, output gain, and output saturation.

**PX4 integration adapter (not part of the ECT core):**

- Velocity damping: `kd_xy = 0.8`, `kd_z = 1.2`

**Shared test environment / command settings (not ECT parameters):**

- Fixed position target: `(0, 0, -2.5 m)` in PX4 NED
- `MPC_THR_HOVER = 0.728`

For the dynamic-tracking stage, the commanded target was:

- `x_target = sin(0.25 t)`
- `y_target = cos(0.25 t)`
- `z_target = -2.5 + 0.5 sin(0.18 t)`

where `t` is PX4 monotonic time in seconds.

For the deterministic-noise stage, the ECT input deviations were perturbed by:

- `n_x = 0.15 sin(7t) + 0.07 sin(19t)`
- `n_y = 0.15 sin(8t) + 0.07 sin(23t)`
- `n_z = 0.10 sin(6t) + 0.05 sin(17t)`

### 2.1 Configuration and Tuning Complexity

A relevant engineering characteristic of this comparison is the difference in
controller configuration complexity. Stock PX4 is a mature flight-control stack
whose position-control behavior depends on an established, tuned set of PX4
control parameters and cascaded lower-level loops. In this report, **Stock PX4**
means that standard PX4 position controller operating without the ECT position
controller.

The tested **ECT core itself** used only three numerical tuning choices: `E α = 0.8`, `G gain = 1.5`, and output saturation `±2.0`; `F` and `F⁻¹` were identity operators. Separately, the **PX4 integration adapter**, which is not part of the ECT core, used velocity-damping coefficients `kd_xy = 0.8` and `kd_z = 1.2`. No plant model, gain schedule, integral term, or per-test ECT retuning
was introduced. The same ECT configuration was retained across the fixed-target,
noise, strong-disturbance, and dynamic-tracking stages.

This is an important result of the present validation: with a comparatively
compact position-control parameterization, the ECT integration reached the same
general performance range as Stock PX4 on several aligned fixed-target and noise
metrics. This should **not** be interpreted as proof that ECT requires no tuning,
or that the complete PX4 stack can be replaced without tuning. PX4 lower-level
loops remain part of both test paths, and the ECT velocity-damping adapter itself
contains tuned coefficients. The evidence supports a narrower claim: **the tested
ECT position-control layer achieved useful and often comparable fixed-target SITL
behavior with a small explicit set of ECT/adapter parameters and without extensive
per-scenario retuning.**

## 3. Test Environment and Method

PX4 SITL was run with the Gazebo x500 model.

Persistent Gazebo forces were used for disturbance testing. Axis-mapping
diagnostics established that, with the simulated vehicle heading near 96°,
Gazebo world `+X` mapped approximately to PX4 `+Y`, while Gazebo world `+Y`
mapped approximately to PX4 `+X`. Test labels were adjusted accordingly.

Two validation datasets are retained in this report.

### 3.1 Comparative Disturbance-Rejection Dataset

The earlier comparison used a stock PX4 reference run and two independent ECT
runs. Each disturbance stage began from a stable baseline.

Peak displacement was measured relative to the stage baseline. Recovery was
measured from force removal until the vehicle remained:

- within `0.10 m` of the stage baseline;
- at or below `0.08 m/s`;
- continuously for `2.0 s`.

Actual force intervals in those recorded ULogs were approximately `4.6–4.8 s`
because Gazebo command overhead made them longer than the nominal sleep
interval.

### 3.2 Extended Single-Flight ECT Validation Suite

The extended suite was executed in one ECT-controlled flight.

For fixed-target stabilization in this suite, the acceptance condition was:

- within `0.15 m` of the commanded fixed target;
- at or below `0.08 m/s`;
- continuously for `3.0 s`.

The normal disturbance stages applied nominal `0.5 N` forces for `2.0 s` and
recorded both displacement from the stage baseline and error from the commanded
target.

The noise stage injected the deterministic signals defined in Section 2 for
`15 s`.

The saturation stage applied a strong combined Gazebo force
`(3.0, 3.0, 1.0) N` for `10 s`. ECT saturation was identified when at least one
reconstructed ECT axis output reached the configured `±2.0` bound. Recovery
after this strong disturbance was measured back to the commanded fixed-target
stabilization condition.

The dynamic-tracking stage commanded the three-axis trajectory defined in
Section 2 for `30 s` and measured instantaneous 3D target error.

## 4. Recorded Data

### 4.1 Comparative Runs

| Controller | ULog | Role |
|---|---|---|
| Stock PX4 (standard PX4 position controller) | `2026-09-21/07_40_39.ulg` | Clean reference run |
| ECT G=1.5 | `2026-09-21/07_55_54.ulg` | ECT run 1 |
| ECT G=1.5 | `2026-09-21/08_00_48.ulg` | Independent ECT repeat |

### 4.2 Extended ECT Suite

The extended single-flight run is retained as:

- `ect_sitl_suite_2026-09-21.ulg` — complete PX4 ULog;
- `ect_sitl_suite_raw.csv` — sampled phase-by-phase test data;
- `ect_sitl_suite_summary.csv` — computed summary metrics.

The recorded ULog size was approximately `952 MB`.

## 5. Comparative Disturbance-Rejection Result

| Test | Stock PX4 | ECT Run 1 | ECT Run 2 | ECT Mean | Mean vs Stock |
|---|---:|---:|---:|---:|---:|
| X | 0.147 m | 0.099 m | 0.129 m | **0.114 m** | **-22.4%** |
| Y | 0.167 m | 0.131 m | 0.128 m | **0.130 m** | **-22.5%** |
| Z | 0.055 m | 0.055 m | 0.056 m | **0.056 m** | **+0.9%** |
| XYZ | 0.221 m | 0.190 m | 0.185 m | **0.188 m** | **-15.2%** |

Across the two ECT runs, mean 3D peak displacement was lower than the single
stock PX4 reference for X, Y, and combined XYZ disturbances. Vertical Z
displacement was effectively unchanged.

The independent ECT repeat produced:

| Test | Duration | 3D Peak | VMAX | Recovery | Final Oscillation |
|---|---:|---:|---:|---:|---|
| Y | 4.64 s | 0.128 m | 0.130 m/s | 2.01 s | 0.036 m / NONE |
| X | 4.64 s | 0.129 m | 0.090 m/s | 1.99 s | 0.020 m / NONE |
| Z | 4.60 s | 0.056 m | 0.059 m/s | 1.98 s | 0.055 m / NONE |
| XYZ | 4.60 s | 0.185 m | 0.166 m/s | 2.03 s | 0.024 m / NONE |

The repeat reported `OSC=NONE` in all four scenarios.

## 6. Extended Single-Flight ECT Results

### 6.1 Fixed-Target Disturbance Rejection

| Test | Peak Baseline Displacement | Peak Target Error | Recovery |
|---|---:|---:|---:|
| X +0.5 N | 0.285 m | 0.398 m | 5.786 s |
| Y +0.5 N | 0.223 m | 0.236 m | 4.820 s |
| Z 0.5 N | 0.131 m | 0.200 m | 3.446 s |
| XYZ 0.5 N | 0.362 m | 0.437 m | 5.966 s |

All four stages remained bounded and returned to the accepted fixed-target
state after force removal.

These values should not be numerically substituted for the comparative results
in Section 5: the extended suite used a different execution procedure,
different force timing, and a `0.15 m` fixed-target stabilization threshold.

### 6.2 Deterministic Noise Robustness

During the `15 s` deterministic-noise stage:

- **Mean 3D target error:** `0.135 m`
- **Maximum 3D target error:** `0.176 m`

The vehicle remained controlled throughout the noise interval. No divergent
position growth was observed in the recorded suite.

### 6.3 Saturation and Recovery

The strong combined-force stage produced:

- **Peak displacement from baseline:** `2.446 m`
- **Peak target error:** `2.500 m`
- **ECT saturation samples:** `16`
- **Recovery to fixed-target acceptance condition:** `15.175 s`

The test therefore exercised the configured ECT output bound rather than only
approaching it. After the strong disturbance was removed, the vehicle returned
to the commanded fixed-target acceptance region without an integral wind-up
mechanism in the ECT controller.

The saturation count is a sampled diagnostic, not a direct measurement of
continuous saturation duration.

### 6.4 Dynamic XYZ Tracking

During the `30 s` moving-target stage:

- **Mean 3D tracking error:** `0.260 m`
- **Maximum 3D tracking error:** `0.580 m`

The controller remained bounded while following simultaneous time-varying
commands in X, Y, and Z.

This result demonstrates trajectory-following behavior for the specified
trajectory and configuration; it is not a general tracking-performance bound.

## 7. Interpretation

The combined validation now supports four distinct behavioral observations for
the tested PX4/Gazebo configuration.

First, the earlier comparative dataset showed lower mean peak displacement for
the ECT integration than the single stock PX4 reference in the X, Y, and
combined XYZ disturbance cases, while Z was essentially unchanged.

Second, the extended suite showed bounded fixed-target response and recovery
after all nominal disturbance stages.

Third, the controller remained bounded under the specified deterministic noise
and recovered after a substantially stronger disturbance that drove the ECT
output to its configured saturation bound.

Fourth, the same ECT configuration tracked a simultaneous time-varying XYZ
trajectory with a recorded mean 3D error of `0.260 m`.

Taken together, these tests extend the empirical SITL evidence beyond the
original disturbance-rejection comparison. They exercise fixed-target
regulation, noisy input conditions, actuator-command limiting at the ECT
output, recovery after a strong disturbance, and dynamic reference tracking.

## 8. Limitations

- The experiments are SITL only; no physical-flight validation is claimed.
- The ECT integration includes explicit velocity damping. The tested system
  must therefore be described as **ECT per-axis control with a
  velocity-damping adapter**, not pure ECT alone.
- PX4 lower-level acceleration-to-thrust, attitude, rate, motor control, and
  Gazebo vehicle dynamics remain common external components.
- The stock PX4 comparison consists of one clean reference run, while the
  original ECT comparative configuration was repeated twice.
- The extended noise, saturation, and dynamic-tracking stages have not yet
  been repeated with the stock PX4 position controller under an identical
  suite. They therefore establish ECT behavior, not comparative superiority
  for those stages.
- The original comparison and the extended suite used different stabilization
  criteria and force timing; their absolute disturbance numbers must not be
  treated as directly interchangeable.
- World-frame Gazebo force axes depend on vehicle heading; the present
  disturbance labels use the empirically verified mapping.
- Saturation was reconstructed from the known ECT mapping and sampled test
  data; `16` is a sample count rather than continuous saturation duration.
- The oscillation classification used in the earlier comparison is a heuristic
  diagnostic, not a formal stability proof.
- ECT contraction at the operator level does not by itself establish
  closed-loop physical-system stability.
- No statistical generalization is claimed from the present number of runs.

## 9. Conclusion

The retained PX4 SITL configuration separates the ECT core from its integration adapter. The **ECT core** used `E α=0.8`, `G=1.5`, and output saturation `±2.0`. The **external PX4 velocity-damping adapter** used `kd_xy=0.8` and `kd_z=1.2`; these damping coefficients are not ECT-core parameters.

The original comparison established a reproducible disturbance-response result:
across two ECT runs, mean peak displacement was lower than the recorded stock
PX4 reference in the horizontal X/Y and combined XYZ cases, with essentially
equal vertical Z response.

The extended validation also includes a single-flight ECT validation suite. In
that suite, the controller recovered after all nominal disturbances, remained
bounded under deterministic noise, reached the configured ECT saturation bound
during a strong combined disturbance and subsequently recovered, and tracked a
time-varying XYZ trajectory.

Accordingly, the present evidence establishes a broader reproducible SITL
behavioral validation of the tested ECT integration. It does not establish a
general proof of physical closed-loop stability or a general claim that ECT
outperforms PID/PX4 across operating conditions.

## 10. Aligned Repeated Stock-vs-ECT Comparison (V1.2)

Version 1.2 adds a repeated comparison using aligned test definitions for the
stock PX4 position controller and the ECT integration. Three retained runs per
controller were evaluated for the normal disturbance, deterministic-noise, and
dynamic-tracking phases.

The aligned normal-disturbance recovery criterion was displacement within
`0.10 m` of the stage baseline, speed at or below `0.08 m/s`, held continuously
for `2.0 s`. The deterministic noise waveforms were identical. Noise error was
measured against the clean fixed target for both controllers. Dynamic target
functions used PX4 monotonic time for both test paths.

### 10.1 Three-Run Mean Results

| Metric | Stock PX4 Mean | ECT Mean |
|---|---:|---:|
| X peak baseline displacement | 0.145 m | 0.151 m |
| X recovery | 3.103 s | 3.308 s |
| Y peak baseline displacement | 0.115 m | 0.196 m |
| Y recovery | 4.662 s | 4.025 s |
| Z peak baseline displacement | 0.049 m | 0.057 m |
| Z recovery | 2.094 s | 2.089 s |
| XYZ peak baseline displacement | 0.138 m | 0.250 m |
| XYZ recovery | 3.972 s | 4.655 s |
| Noise mean 3D error | 0.030 m | 0.034 m |
| Noise maximum 3D error | 0.060 m | 0.057 m |
| Dynamic tracking mean 3D error | 0.280 m | 0.674 m |
| Dynamic tracking maximum 3D error | 0.645 m | 0.869 m |

The aligned repeated data show broadly similar X and Z fixed-target response
and similar deterministic-noise error. The Y result is mixed: ECT recovered
faster on average but experienced a larger peak displacement. For the combined
XYZ disturbance, stock PX4 had a smaller mean peak displacement and somewhat
faster recovery. For the tested moving trajectory, stock PX4 had substantially
lower mean and maximum tracking error than the present ECT integration.

These results supersede any comparative interpretation of the earlier
non-aligned noise measurements. In particular, the previously observed large
noise advantage for ECT was a measurement-method artifact: after both
controllers were evaluated against the same clean fixed target, their noise
errors were similar.

### 10.2 Strong-Force Phase

The strong-force phase is not included in the final three-run comparative
statistics. The third retained stock run reported only `0.048 m` peak baseline
displacement, whereas the first two aligned stock runs produced approximately
`1.1 m`. This indicates that the intended strong disturbance was not
comparably exercised in that third phase. Averaging the three values would
therefore produce a misleading comparison.

The earlier single-flight ECT saturation result in Section 6.3 remains valid
for that recorded run, but it is not used as evidence of repeated comparative
superiority.

## 11. V1.2 Interpretation

The aligned repeated comparison materially narrows the conclusions that should
be drawn from the SITL work. Under ordinary fixed-target regulation and the
specified deterministic-noise test, the present ECT integration performs in
the same general range as the stock PX4 controller on several metrics, rather
than demonstrating a general advantage.

At the same time, the current ECT configuration has a clear weakness on the
specified dynamic XYZ trajectory: its tracking error is substantially larger
than stock PX4. This identifies dynamic reference tracking as a concrete target
for further ECT integration and tuning work.

The V1.2 evidence therefore supports the following engineering conclusion: the
tested ECT per-axis controller with explicit velocity damping is a functional
PX4 SITL control integration capable of stable fixed-target regulation,
disturbance recovery, deterministic-noise operation, and moving-target
tracking. Its fixed-target behavior is often comparable in scale to Stock PX4
under the aligned tests, while the present configuration does not match Stock
PX4 dynamic-tracking accuracy.

A second engineering result concerns configuration complexity. The ECT
position-control layer obtained those fixed-target and noise results using the
same compact **ECT-core** parameterization throughout the suite: `α = 0.8`, `G = 1.5`, and `±2.0` output saturation. Two additional velocity-damping coefficients belonged to the external PX4 integration adapter, not to ECT itself. It was not
retuned for individual disturbance axes, noise, or tracking phases. Stock PX4,
by contrast, is the standard mature PX4 position-control implementation operating
within its established tuned flight-control stack. The comparison therefore
shows that the tested ECT layer can obtain practically comparable behavior on
several regulation metrics without a large ECT-specific tuning surface. This is
a configuration-simplicity observation, not proof that ECT is tuning-free; the
adapter coefficients and the common PX4 lower-level loops remain tuned elements.

No claim of general ECT superiority over PID/PX4 is supported by these tests.
The strongest supported distinction is more specific: **ECT demonstrated
competitive fixed-target/noise behavior with a compact explicit controller
configuration, while Stock PX4 retained a substantial advantage in the tested
dynamic-tracking task.**
