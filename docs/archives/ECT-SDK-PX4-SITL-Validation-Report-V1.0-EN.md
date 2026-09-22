# ECT-SDK --- PX4 SITL Disturbance-Rejection Validation Report

**Version:** 1.0\
**Date:** 21 September 2026

## 1. Scope and Controller Configuration

This report documents the completed PX4/Gazebo SITL comparison between
the stock PX4 position controller and the ECT per-axis controller with
an explicit velocity-damping adapter.

The tested ECT integration uses three independent scalar ECT channels,
one for each local-position axis:

`Δ → F → E → F⁻¹ → G → ECT output`

The ECT output is combined with explicit velocity damping and supplied
to PX4 as an acceleration setpoint. PX4's lower-level
acceleration-to-thrust, attitude, rate, motor, and Gazebo plant dynamics
remain external to ECT.

Configuration:

-   **F:** identity
-   **E:** linear contraction, `α = 0.8`
-   **F⁻¹:** identity
-   **G:** gain `1.5`, saturation `[-2.0, +2.0]`
-   Unsaturated ECT position-error slope: `1.5 × 0.8 = 1.2`
-   Velocity damping: `kd_xy = 0.8`, `kd_z = 1.2`
-   Position target: `(0, 0, -2.5 m)` in PX4 NED
-   `MPC_THR_HOVER = 0.728`

## 2. Test Method

PX4 SITL was run with the Gazebo x500 model. Persistent external forces
of `0.5 N` were injected in single-axis and combined XYZ scenarios.

Each stage began from a stable baseline. Peak displacement was measured
relative to that stage's baseline.

Recovery was measured from `FORCE_CLEAR` until the vehicle remained:

-   within `0.10 m` of the stage baseline;
-   at or below `0.08 m/s`;
-   continuously for `2.0 s`.

Axis-mapping diagnostics showed that, with the simulated vehicle heading
near 96°, Gazebo world `+X` mapped approximately to PX4 `+Y`, while
Gazebo world `+Y` mapped approximately to PX4 `+X`. Test labels were
adjusted accordingly.

Actual force intervals recorded in ULog were approximately `4.6–4.8 s`
because Gazebo command overhead made them longer than the nominal sleep
interval.

## 3. Recorded Runs

  Controller   ULog                        Role
  ------------ --------------------------- ------------------------
  Stock PX4    `2026-09-21/07_40_39.ulg`   Clean reference run
  ECT G=1.5    `2026-09-21/07_55_54.ulg`   ECT run 1
  ECT G=1.5    `2026-09-21/08_00_48.ulg`   Independent ECT repeat

## 4. Primary Result --- 3D Peak Displacement

  Test     Stock PX4   ECT Run 1   ECT Run 2      ECT Mean   Mean vs Stock
  ------ ----------- ----------- ----------- ------------- ---------------
  X          0.147 m     0.099 m     0.129 m   **0.114 m**      **-22.4%**
  Y          0.167 m     0.131 m     0.128 m   **0.130 m**      **-22.5%**
  Z          0.055 m     0.055 m     0.056 m   **0.056 m**       **+0.9%**
  XYZ        0.221 m     0.190 m     0.185 m   **0.188 m**      **-15.2%**

Across the two ECT runs, mean 3D peak displacement was lower than the
stock reference for X, Y, and combined XYZ disturbances. Vertical Z
displacement was effectively unchanged.

## 5. Repeat ECT Run Details

ULog: `2026-09-21/08_00_48.ulg`

  Test     Duration   3D Peak        VMAX   Recovery     Final Oscillation
  ------ ---------- --------- ----------- ---------- --------- -------------
  Y          4.64 s   0.128 m   0.130 m/s     2.01 s   0.036 m NONE
  X          4.64 s   0.129 m   0.090 m/s     1.99 s   0.020 m NONE
  Z          4.60 s   0.056 m   0.059 m/s     1.98 s   0.055 m NONE
  XYZ        4.60 s   0.185 m   0.166 m/s     2.03 s   0.024 m NONE

The repeat reported `OSC=NONE` in all four scenarios.

## 6. Interpretation

The repeated `G=1.5` ECT test reproduced the same overall behavior as
the first `G=1.5` run.

Within this specific PX4/Gazebo configuration, the ECT integration
demonstrated bounded disturbance rejection and lower measured peak
displacement than the single stock PX4 reference run for X, Y, and
combined XYZ disturbances. Vertical peak displacement was essentially
the same.

These results are an engineering SITL observation, not a proof that ECT
generally outperforms PID or the PX4 controller. The stock result
consists of one clean reference run, while ECT `G=1.5` was repeated
twice. Both configurations also share PX4 lower-level control and Gazebo
dynamics.

## 7. Limitations

-   The experiment is SITL only; no physical-flight validation is
    claimed.
-   Force duration was not perfectly identical between runs because
    Gazebo command overhead affected the measured interval.
-   World-frame Gazebo force axes depend on vehicle heading; the present
    test used an empirically verified axis mapping.
-   The oscillation classification is a heuristic diagnostic, not a
    formal stability proof.
-   ECT contraction at the operator level does not by itself establish
    closed-loop physical-system stability.
-   The explicit velocity-damping adapter is part of the tested
    integration; the result must not be described as pure ECT alone.

## 8. Conclusion

The configuration `G=1.5`, `E α=0.8`, `kd_xy=0.8`, `kd_z=1.2` is
retained as the validated current PX4 SITL configuration.

Two independent ECT runs produced closely comparable
disturbance-response magnitudes. Against the recorded stock PX4
reference run, the ECT integration showed smaller 3D peak displacement
in the horizontal and combined-force scenarios and essentially equal
vertical peak displacement.

This establishes a reproducible SITL behavioral result for the tested
configuration and provides a basis for subsequent documentation or
broader validation.
