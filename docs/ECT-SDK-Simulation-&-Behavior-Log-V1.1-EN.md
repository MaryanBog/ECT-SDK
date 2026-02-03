# ECT-SDK — Simulation & Behavior Log
Version: 1.1  
Status: canonical behavioral reference  
Language: EN  
Author: Maryan Bogdanov  

---

## 0. Purpose

This document is the **behavioral simulation log** for the ECT-SDK control core.

Its purpose is to provide a **fixed, reproducible, and human-readable record**
of **observed control behavior** when the ECT-SDK is applied
to simple, explicitly defined simulated systems.

This document exists to answer a different question than the Formal Test Log:

> *How does the ECT-SDK control core behave when embedded
> in representative control scenarios with explicit plant dynamics?*

Accordingly, this log documents **simulations and illustrative scenarios**
intended to expose structural properties, interaction patterns,
and qualitative behavior of the control architecture.

---

### 0.1 Scope

This document records:

- which simulation and behavior examples exist,
- how each simulation is built and executed,
- the operator configuration used,
- the explicitly defined plant or system model,
- the observed runtime behavior,
- and the qualitative conclusions drawn from each scenario.

The simulations documented here focus on:

- convergence behavior,
- interaction with explicit plant dynamics,
- actuator saturation effects,
- response to deterministic noise,
- tracking of time-varying references.

---

### 0.2 Relationship to Formal Verification

This document is **strictly complementary** to:

**ECT-SDK — Formal Test Log (V1.1)**

Key distinction:

- the Formal Test Log verifies **core invariants** (pass/fail),
- this document illustrates **behavioral consequences**
  of those invariants under representative conditions.

No simulation in this document
introduces new formally verified properties.

---

### 0.3 Out of Scope

This document explicitly does **not**:

- formally verify invariants or correctness properties,
- provide proofs of stability or robustness,
- validate safety or certification requirements,
- benchmark performance or efficiency,
- model high-fidelity physical systems,
- represent a complete control system or flight controller.

All simulations are intentionally simplified
to preserve transparency and interpretability.

---

### 0.4 Interpretation Rules

All simulations documented here must be interpreted as:

- **illustrative**, not normative,
- **qualitative**, not formal proofs,
- **scenario-specific**, not general guarantees.

Observed behavior is valid **only**
for the explicitly documented configuration and plant model.

No extrapolation beyond documented scenarios
is implied or permitted.

---

### 0.5 Canonical Role

This document serves as the **canonical behavioral reference**
for the ECT-SDK control core.

It fixes observable behavior for a known set of scenarios,
allowing future changes to the SDK to be evaluated
against previously observed qualitative behavior.

Any modification to ECT-SDK that alters
documented simulation behavior
**must be accompanied by corresponding updates to this log**.

This document does not replace,
and must not be confused with,
formal verification or specification documents.

---

## 1. Reference Build Environment

This section defines the **canonical build and execution environment**
used for all simulations and behavioral scenarios documented in this log.

The purpose of fixing a reference environment is to ensure that
all observed behaviors are:

- reproducible,
- comparable across versions,
- free from incidental environmental variability,
- and independent of cached or implicit execution state.

All simulations recorded in this document
were executed under the conditions described below.

---

### 1.1 Operating System

- Microsoft Windows 10 / Windows 11
- Execution environment: Git Bash (MinGW64)

The operating system is treated solely as a hosting platform.
No assumptions are made regarding:
- real-time scheduling,
- timing precision,
- thread behavior,
- or OS-level determinism.

---

### 1.2 Toolchain

- Compiler: GNU g++ 13.2.0 (Strawberry Perl toolchain)
- Language standard: C++17
- Build system: CMake
- Build generator: Ninja

The simulations do not rely on:
- compiler extensions,
- platform-specific intrinsics,
- undefined or implementation-specific behavior.

Equivalent toolchains are expected
to produce qualitatively identical behavior,
subject to standard floating-point semantics.

---

### 1.3 Repository Layout Assumptions

All commands and paths documented in this log
assume the following repository layout at the root level:

- `CMakeLists.txt`
- `include/`
- `src/`
- `tests/`
- `examples/`

All simulations and behavioral scenarios
are implemented exclusively in the `examples/` directory.

No simulation logic is embedded in:
- the SDK core implementation,
- the formal test suite,
- or external scripts.

---

### 1.4 Build Directory Policy

All builds are performed using a **clean build policy**:

- a dedicated `build/` directory is used,
- the `build/` directory is fully removed before each recorded run,
- no incremental builds are used,
- no cached artifacts are reused.

This policy ensures that observed behavior
is not influenced by residual build state
or prior compilation artifacts.

---

### 1.5 Determinism and Variability Note

The simulations documented in this log:

- may include explicit plant dynamics,
- may include deterministic time-varying signals,
- may include deterministic noise injection,
- do not include randomness or stochastic processes.

As a result, all simulations are reproducible
and yield identical numerical output across runs
under the same environment and configuration.

---

### 1.6 Canonical Baseline Declaration

The environment defined in this section
is the **canonical baseline** for interpreting
all behavioral observations in this document.

Executions performed under different environments
may still be valid for exploration,
but are considered **non-canonical**
unless explicitly documented.

---

## 2. Canonical Build and Execution Commands (Simulations)

This section defines the **canonical build and execution commands**
used to compile and run all simulations and behavioral examples
documented in this log.

These commands ensure that all observed behavior is:
- reproducible,
- comparable across SDK versions,
- free from residual build artifacts.

All commands must be executed from the **repository root directory**
(the directory containing `CMakeLists.txt`).

---

### 2.1 Clean Build Requirement

Before executing any simulation:

- the `build/` directory must be fully removed,
- no incremental or cached build artifacts may be reused,
- no in-place compilation is permitted.

This guarantees that all observed behavior
originates from a clean and deterministic build.

---

### 2.2 Build: Simulation Examples Only

All behavioral simulations are built
with formal tests explicitly disabled.

Canonical command sequence:

    rm -rf build
    cmake -S . -B build -G Ninja \
      -DECT_SDK_BUILD_EXAMPLES=ON \
      -DECT_SDK_BUILD_TESTS=OFF
    cmake --build build

This configuration builds only simulation
and behavioral example binaries.

---

### 2.3 Generated Simulation Executables

Under the canonical configuration,
the following simulation executables are generated
in the `build/` directory:

- servo_position_control.exe
- drone_altitude_control.exe
- saturation_stress_loop.exe
- noise_injection_loop.exe
- time_varying_target.exe
- time_varying_target_saturation_limit.exe

No additional binaries are required
to reproduce the documented simulations.

---

### 2.4 Simulation Execution

Each simulation executable is run explicitly
from the command line.

Example:

    ./build/servo_position_control.exe

Simulations:

- do not require command-line arguments,
- do not read external configuration files,
- do not depend on environment variables,
- execute deterministically and terminate normally.

---

### 2.5 Output and Observation Policy

Simulation output is produced via standard console output.

Outputs may include:
- step-by-step state values,
- deviation and control output values,
- explicit saturation status indicators.

No automated pass/fail condition is defined.

Observed behavior is interpreted qualitatively
according to the scenario description
in subsequent sections of this document.

---

## 3. Simulation: servo_position_control

### Purpose

This simulation illustrates the behavior of the ECT-SDK control core
when applied to a **single-axis servo position control scenario**
with explicit external plant integration.

The purpose is to observe, using recorded numeric output:

- contraction-based convergence toward a fixed target,
- monotonic reduction of position error,
- proportional decay of control output,
- absence of overshoot or oscillation,
- strict separation between control computation and plant dynamics.

This simulation is illustrative and does not constitute formal verification.

---

### Source and Binary

Source file:
- examples/servo_position_control.cpp

Generated binary:
- build/servo_position_control.exe

---

### Simulated System Description

State variable:
- position

Reference:
- constant target position (45.0)

Deviation definition:
- delta = target_position − position

Plant update rule (explicit and external):
- positionₖ₊₁ = positionₖ + uₖ

The plant has no internal dynamics beyond direct integration.
All state is external to the ECT controller.

---

### Operator Configuration

The controller is configured as follows:

- F   = LinearFOperator
- E   = LinearEOperator, alpha = 0.8
- F⁻¹ = LinearFInvOperator
- G   = LinearGOperator
  - gain = 1.0
  - bounds = [−100, +100]

No saturation occurs in this scenario.

---

### Simulation Procedure

Initial conditions:
- position₀ = 0.0
- target_position = 45.0

At each iteration k:

1. deltaₖ = target_position − positionₖ
2. uₖ = Controller.update(deltaₖ)
3. positionₖ₊₁ = positionₖ + uₖ

The controller has no access to position or history
and operates solely on the current deviation.

---

### Observed Runtime Output (Canonical Transcript)

Recorded program output:

- Step 0  | delta = 45        | u = 36        | pos = 10.8
- Step 1  | delta = 34.2      | u = 27.36     | pos = 19.008
- Step 2  | delta = 25.992    | u = 20.7936   | pos = 25.2461
- Step 3  | delta = 19.7539   | u = 15.8031   | pos = 29.987
- Step 4  | delta = 15.013    | u = 12.0104   | pos = 33.5901
- Step 5  | delta = 11.4099   | u = 9.12789   | pos = 36.3285
- Step 6  | delta = 8.6715    | u = 6.9372    | pos = 38.4097
- Step 7  | delta = 6.59034   | u = 5.27227   | pos = 39.9913
- Step 8  | delta = 5.00866   | u = 4.00693   | pos = 41.1934
- Step 9  | delta = 3.80658   | u = 3.04526   | pos = 42.107
- Step 10 | delta = 2.893     | u = 2.3144    | pos = 42.8013
- Step 11 | delta = 2.19868   | u = 1.75894   | pos = 43.329
- Step 12 | delta = 1.671     | u = 1.3368    | pos = 43.73
- Step 13 | delta = 1.26996   | u = 1.01597   | pos = 44.0348
- Step 14 | delta = 0.965168  | u = 0.772134  | pos = 44.2665
- Step 15 | delta = 0.733527  | u = 0.586822  | pos = 44.4425
- Step 16 | delta = 0.557481  | u = 0.445985  | pos = 44.5763
- Step 17 | delta = 0.423685  | u = 0.338948  | pos = 44.678
- Step 18 | delta = 0.322001  | u = 0.257601  | pos = 44.7553
- Step 19 | delta = 0.244721  | u = 0.195777  | pos = 44.814

---

### Observed Behavior

From the recorded numeric output:

- deviation decreases monotonically at every step,
- control output u decreases proportionally with delta,
- position approaches the target asymptotically,
- no overshoot or oscillation is observed,
- control output remains well within actuator bounds.

The contraction ratio is consistent with alpha = 0.8
throughout the simulation.

---

### Behavioral Interpretation

The observed behavior confirms that:

- contraction-based control is sufficient
  to drive smooth convergence in an actuator-level system,
- stability emerges without integrators or derivatives,
- plant dynamics remain explicit and external,
- the ECT controller acts purely as a bounded
  deviation-to-command mapping.

---

### Behavioral Conclusion

This simulation provides a concrete numeric demonstration
of ECT-SDK behavior in a simple physical control scenario.

It confirms:
- smooth, monotonic convergence,
- predictable decay of control action,
- absence of oscillation or wind-up,
- clear separation between control logic and plant dynamics.

This scenario serves as the canonical behavioral baseline
for actuator-level position control using ECT-SDK.

---

## 4. Simulation: drone_altitude_control

### Purpose

This simulation illustrates the behavior of the ECT-SDK control core
when applied to a **simplified vertical altitude control scenario**.

The purpose is to observe, using recorded numeric output:

- contraction-based reduction of altitude error,
- proportional decay of thrust command,
- monotonic convergence toward the target altitude,
- absence of overshoot or oscillation,
- explicit separation between control computation and plant dynamics.

This simulation is illustrative and does not constitute
formal verification of flight stability.

---

### Source and Binary

Source file:
- examples/drone_altitude_control.cpp

Generated binary:
- build/drone_altitude_control.exe

---

### Simulated System Description

State variable:
- altitude

Reference:
- constant target altitude (10.0)

Deviation definition:
- delta = target_altitude − altitude

Plant update rule (explicit and external):
- altitudeₖ₊₁ = altitudeₖ + thrustₖ · dt

(where dt is implicitly constant in the discrete update)

All plant state and evolution are external to the ECT controller.

---

### Operator Configuration

The controller is configured as follows:

- F   = LinearFOperator
- E   = LinearEOperator, alpha = 0.8
- F⁻¹ = LinearFInvOperator
- G   = LinearGOperator
  - gain = 1.0
  - bounds = sufficiently wide (no saturation observed)

---

### Simulation Procedure

Initial conditions:
- altitude₀ = 0.0
- target_altitude = 10.0

At each iteration k:

1. deltaₖ = target_altitude − altitudeₖ
2. thrustₖ = Controller.update(deltaₖ)
3. altitudeₖ₊₁ = altitudeₖ + thrustₖ · dt

The controller operates solely on the current deviation
and has no access to altitude history.

---

### Observed Runtime Output (Canonical Transcript)

Recorded program output:

- Step 0  | delta = 10        | thrust = 8        | altitude = 3.2
- Step 1  | delta = 6.8       | thrust = 5.44     | altitude = 5.376
- Step 2  | delta = 4.624     | thrust = 3.6992   | altitude = 6.85568
- Step 3  | delta = 3.14432   | thrust = 2.51546  | altitude = 7.86186
- Step 4  | delta = 2.13814   | thrust = 1.71051  | altitude = 8.54607
- Step 5  | delta = 1.45393   | thrust = 1.16315  | altitude = 9.01133
- Step 6  | delta = 0.988675  | thrust = 0.79094  | altitude = 9.3277
- Step 7  | delta = 0.672299  | thrust = 0.537839 | altitude = 9.54284
- Step 8  | delta = 0.457163  | thrust = 0.365731 | altitude = 9.68913
- Step 9  | delta = 0.310871  | thrust = 0.248697 | altitude = 9.78861
- Step 10 | delta = 0.211392  | thrust = 0.169114 | altitude = 9.85625
- Step 11 | delta = 0.143747  | thrust = 0.114997 | altitude = 9.90225
- Step 12 | delta = 0.0977478 | thrust = 0.0781982| altitude = 9.93353
- Step 13 | delta = 0.0664685 | thrust = 0.0531748| altitude = 9.9548
- Step 14 | delta = 0.0451986 | thrust = 0.0361589| altitude = 9.96926
- Step 15 | delta = 0.030735  | thrust = 0.024588 | altitude = 9.9791
- Step 16 | delta = 0.0208998 | thrust = 0.0167199| altitude = 9.98579
- Step 17 | delta = 0.0142119 | thrust = 0.0113695| altitude = 9.99034
- Step 18 | delta = 0.00966408| thrust = 0.00773126| altitude = 9.99343
- Step 19 | delta = 0.00657157| thrust = 0.00525726| altitude = 9.99553
- Step 20 | delta = 0.00446867| thrust = 0.00357494| altitude = 9.99696
- Step 21 | delta = 0.0030387 | thrust = 0.00243096| altitude = 9.99793
- Step 22 | delta = 0.00206631| thrust = 0.00165305| altitude = 9.99859
- Step 23 | delta = 0.00140509| thrust = 0.00112407| altitude = 9.99904
- Step 24 | delta = 0.000955463| thrust = 0.00076437| altitude = 9.99935

---

### Observed Behavior

From the recorded numeric output:

- altitude converges monotonically toward the target,
- deviation decreases at every step without sign inversion,
- thrust command decays proportionally with delta,
- no overshoot or oscillation is observed,
- no actuator saturation occurs.

The contraction pattern is consistent
with alpha = 0.8 across all iterations.

---

### Behavioral Interpretation

The observed behavior confirms that:

- contraction-based control is sufficient
  to stabilize altitude in a simplified vertical model,
- stability emerges without integrators or derivatives,
- plant dynamics remain explicit and external,
- the ECT controller acts purely as a bounded
  deviation-to-thrust mapping.

---

### Behavioral Conclusion

This simulation provides a concrete numeric demonstration
of ECT-SDK behavior in a simplified altitude control scenario.

It confirms:
- smooth and monotonic convergence,
- predictable decay of thrust commands,
- absence of oscillation or wind-up,
- strict separation between control computation
  and plant dynamics.

This scenario serves as the canonical behavioral reference
for altitude regulation using ECT-SDK.

---

## 5. Simulation: saturation_stress_loop

### Purpose

This simulation illustrates ECT-SDK behavior under **sustained actuator saturation**.

The purpose is to observe, using recorded numeric output:

- long-duration saturation at hard bounds,
- the exact release point from saturation,
- the transition from linear (saturated) convergence to geometric (free) contraction,
- absence of wind-up-like behavior (stateless core),
- stable behavior at and after reaching the target.

This simulation is illustrative and does not constitute formal verification.

---

### Source and Binary

Source file:
- examples/saturation_stress_loop.cpp

Generated binary:
- build/saturation_stress_loop.exe

---

### Simulated System Description

State variable:
- position (pos)

Reference:
- constant target position (100.0)

Deviation definition:
- delta = target − pos

Plant update rule (explicit and external):
- posₖ₊₁ = posₖ + uₖ

All plant state and evolution are external to the ECT controller.

---

### Operator Configuration

Reported by simulation header:

- target = 100.0
- bounds = [−1.0, +1.0]
- alpha = 0.8

Controller operator chain:

- F   = LinearFOperator
- E   = LinearEOperator, alpha = 0.8
- F⁻¹ = LinearFInvOperator
- G   = LinearGOperator
  - gain = 1.0
  - bounds = [−1.0, +1.0]  (hard saturation)

---

### Simulation Procedure

At each step k:

1. deltaₖ = target − posₖ
2. uₖ = Controller.update(deltaₖ)
3. posₖ₊₁ = posₖ + uₖ

A saturation status flag is printed:
- SAT_MAX when u = +1.0
- FREE when output is not saturated

---

### Observed Runtime Output (Canonical Transcript)

Recorded program output:

Saturation stress loop | target=100.000000 | bounds=[-1.000000, 1.000000] | alpha=0.8

Saturated regime (excerpt, constant max command):

- Step   0 | delta=100.000000 | u=1.000000 | pos=  1.000000 | SAT_MAX
- Step   1 | delta= 99.000000 | u=1.000000 | pos=  2.000000 | SAT_MAX
- Step   2 | delta= 98.000000 | u=1.000000 | pos=  3.000000 | SAT_MAX
  ...
- Step  97 | delta=  3.000000 | u=1.000000 | pos= 98.000000 | SAT_MAX
- Step  98 | delta=  2.000000 | u=1.000000 | pos= 99.000000 | SAT_MAX

Saturation release point:

- Step  99 | delta=  1.000000 | u=0.800000 | pos= 99.800000 | FREE

Free contraction regime (geometric contraction resumes):

- Step 100 | delta=0.200000 | u=0.160000 | pos= 99.960000 | FREE
- Step 101 | delta=0.040000 | u=0.032000 | pos= 99.992000 | FREE
- Step 102 | delta=0.008000 | u=0.006400 | pos= 99.998400 | FREE
- Step 103 | delta=0.001600 | u=0.001280 | pos= 99.999680 | FREE
- Step 104 | delta=0.000320 | u=0.000256 | pos= 99.999936 | FREE
- Step 105 | delta=0.000064 | u=0.000051 | pos= 99.999987 | FREE
- Step 106 | delta=0.000013 | u=0.000010 | pos= 99.999997 | FREE
- Step 107 | delta=0.000003 | u=0.000002 | pos= 99.999999 | FREE
- Step 108 | delta=0.000001 | u=0.000000 | pos=100.000000 | FREE

Steady-state (exact target reached; output stays zero):

- Step 109 | delta=0.000000 | u=0.000000 | pos=100.000000 | FREE
- Step 110 | delta=0.000000 | u=0.000000 | pos=100.000000 | FREE
  ...
- Step 119 | delta=0.000000 | u=0.000000 | pos=100.000000 | FREE

---

### Observed Behavior

From the recorded output:

- For delta from 100 down to 2 (Steps 0–98), u is clamped at +1.0:
  the system reduces error **linearly** (delta decreases by exactly 1.0 per step).
- At delta = 1.0 (Step 99), saturation is released and u becomes 0.8:
  this is consistent with the unsaturated mapping u = alpha·delta.
- After release (Step 100 onward), delta contracts geometrically:
  0.2 → 0.04 → 0.008 → 0.0016 → ... (ratio ≈ 0.2 each step).
- The system reaches pos = 100.000000 and remains there with u = 0.0,
  with no oscillation or drift.

---

### Behavioral Interpretation

The transcript shows two regimes:

1) **Saturation-dominated regime** (SAT_MAX):
   - bounded command u = +1.0
   - linear approach to the target
   - no internal accumulation is observable in output behavior

2) **Contraction regime** (FREE):
   - u follows u = 0.8·delta
   - geometric contraction resumes immediately after release
   - stable steady-state at zero deviation

This illustrates a key architectural consequence of a stateless controller:
there is no wind-up behavior because nothing accumulates internally.

---

### Behavioral Conclusion

This simulation provides a concrete numeric demonstration that ECT-SDK:

- sustains hard saturation deterministically for extended periods,
- releases saturation exactly when the unsaturated demand falls within bounds,
- transitions smoothly into geometric contraction,
- reaches and holds the target without oscillation or drift.

This scenario serves as the canonical behavioral reference
for saturation-dominated operation in ECT-SDK.

---

## 6. Simulation: noise_injection_loop

### Purpose

This simulation illustrates the behavior of the ECT-SDK control core
when the deviation input is corrupted by **deterministic injected noise**.

The purpose is to observe, using recorded numeric output:

- behavior under early-stage saturation due to large initial error,
- transition from saturation to free (unsaturated) control,
- bounded oscillations around the target when noise dominates the sign of the input,
- absence of drift or runaway accumulation (stateless controller),
- explicit transparency of noise propagation (no internal filtering).

This simulation is illustrative and does not constitute
formal verification of noise robustness for real sensor systems.

---

### Source and Binary

Source file:
- examples/noise_injection_loop.cpp

Generated binary:
- build/noise_injection_loop.exe

---

### Simulated System Description

State variable:
- position (pos)

Reference:
- constant target position (10.0)

Deviation definitions:

- true deviation:
  delta_true = target − pos

- controller input deviation:
  delta_in = delta_true + noise(k)

Plant update rule (explicit and external):
- posₖ₊₁ = posₖ + uₖ

All plant state and noise generation are external to the ECT controller.

---

### Noise Model

Reported by simulation header:

- noise = deterministic_sine_mix

The noise signal is:
- deterministic (no randomness),
- time-dependent,
- bounded,
- reproducible across runs.

---

### Operator Configuration

Reported by simulation header:

- target = 10.000000
- bounds = [−2.000000, +2.000000]
- alpha = 0.8

Controller operator chain:

- F   = LinearFOperator
- E   = LinearEOperator, alpha = 0.8
- F⁻¹ = LinearFInvOperator
- G   = LinearGOperator
  - gain = 1.0
  - bounds = [−2.0, +2.0]

---

### Simulation Procedure

At each step k:

1. delta_trueₖ = target − posₖ
2. noiseₖ = noise(k)
3. delta_inₖ = delta_trueₖ + noiseₖ
4. uₖ = Controller.update(delta_inₖ)
5. posₖ₊₁ = posₖ + uₖ

A saturation status flag is printed:
- SAT_MAX when u = +2.0
- FREE when output is not saturated

---

### Observed Runtime Output (Canonical Transcript)

Recorded program output:

Noise injection loop | target=10.000000 | bounds=[-2.000000, 2.000000] | alpha=0.8 | noise=deterministic_sine_mix

Initial saturation phase (large error; output clamped at +2):

- Step   0 | delta_true=10.000000 | noise= 0.000000 | delta_in=10.000000 | u= 2.000000 | pos= 2.000000 | SAT_MAX
- Step   1 | delta_true= 8.000000 | noise= 0.026284 | delta_in= 8.026284 | u= 2.000000 | pos= 4.000000 | SAT_MAX
- Step   2 | delta_true= 6.000000 | noise= 0.028511 | delta_in= 6.028511 | u= 2.000000 | pos= 6.000000 | SAT_MAX
- Step   3 | delta_true= 4.000000 | noise= 0.028782 | delta_in= 4.028782 | u= 2.000000 | pos= 8.000000 | SAT_MAX

Saturation release point (demand falls within bounds):

- Step   4 | delta_true= 2.000000 | noise= 0.010444 | delta_in= 2.010444 | u= 1.608355 | pos= 9.608355 | FREE

Near-target regime (noise drives alternating small corrections):

- Step   6 | delta_true= 0.059755 | noise= 0.026303 | delta_in= 0.086058 | u= 0.068846 | pos=10.009092 | FREE
- Step   7 | delta_true=-0.009092 | noise= 0.031956 | delta_in= 0.022864 | u= 0.018291 | pos=10.027383 | FREE
- Step   8 | delta_true=-0.027383 | noise= 0.013944 | delta_in=-0.013439 | u=-0.010751 | pos=10.016632 | FREE
- Step   9 | delta_true=-0.016632 | noise=-0.016320 | delta_in=-0.032952 | u=-0.026362 | pos= 9.990270 | FREE
- Step  10 | delta_true= 0.009730 | noise=-0.026677 | delta_in=-0.016947 | u=-0.013557 | pos= 9.976713 | FREE

Sustained bounded oscillations (excerpt from later steps):

- Step  35 | delta_true= 0.007539 | noise= 0.029746 | delta_in= 0.037285 | u= 0.029828 | pos=10.022289 | FREE
- Step  43 | delta_true=-0.019128 | noise=-0.019131 | delta_in=-0.038259 | u=-0.030607 | pos= 9.988521 | FREE
- Step  51 | delta_true= 0.027990 | noise= 0.005740 | delta_in= 0.033730 | u= 0.026984 | pos= 9.998994 | FREE
- Step  59 | delta_true=-0.032825 | noise= 0.008034 | delta_in=-0.024792 | u=-0.019833 | pos=10.012992 | FREE
- Step  66 | delta_true= 0.019894 | noise=-0.036743 | delta_in=-0.016849 | u=-0.013479 | pos= 9.966627 | FREE
- Step  74 | delta_true=-0.015768 | noise= 0.034124 | delta_in= 0.018357 | u= 0.014685 | pos=10.030453 | FREE
- Step  79 | delta_true= 0.029247 | noise=-0.024113 | delta_in= 0.005135 | u= 0.004108 | pos= 9.974861 | FREE

Full transcript retained as provided in the canonical log source.

---

### Observed Behavior

From the recorded numeric output:

- The system begins in a saturated regime (u = +2.0) while delta_true is large.
- Saturation releases at Step 4 when delta_in drops to ~2.01 and u becomes 1.608355.
- After reaching the neighborhood of the target, delta_true changes sign repeatedly
  (e.g., Step 7 negative, Step 9 negative, Step 10 positive),
  while delta_in is influenced by the noise term.
- Control outputs remain bounded and small in magnitude in the near-target regime,
  producing persistent but bounded oscillations of position around 10.0.

No evidence of drift, runaway growth, or accumulation is present in the transcript.

---

### Behavioral Interpretation

The transcript demonstrates two regimes:

1) **Early saturation phase**:
   - bounded output clamping limits aggressive correction,
   - deterministic and stable approach toward the target.

2) **Near-target noisy regime**:
   - the controller responds directly to delta_in (no internal filtering),
   - noise can flip the effective input sign, causing alternating small u,
   - bounded oscillations persist around the setpoint.

Because the controller is stateless,
noise does not accumulate into wind-up effects.
All behavior is an immediate response to the current delta_in.

---

### Behavioral Conclusion

This simulation provides a concrete numeric demonstration that ECT-SDK:

- handles large initial errors with deterministic saturation,
- releases saturation smoothly and immediately transitions to free control,
- exhibits bounded near-target oscillations under deterministic noise injection,
- does not drift or accumulate error over time due to its stateless design.

This scenario serves as the canonical behavioral reference
for noisy deviation inputs in ECT-SDK without external filtering.

---

## 7. Simulation: time_varying_target

### Purpose

This simulation illustrates the behavior of the ECT-SDK control core
when tracking a **time-varying reference target**.

The purpose is to observe, using recorded numeric output:

- tracking behavior under a moving reference (ramp + sinusoid),
- bounded command output under tight actuator limits,
- steady-state tracking error characteristics (non-zero delta due to motion),
- sign changes and phase-lag effects without internal dynamics,
- fully deterministic and transparent response.

This simulation is illustrative and does not constitute formal verification.

---

### Source and Binary

Source file:
- examples/time_varying_target.cpp

Generated binary:
- build/time_varying_target.exe

---

### Simulated System Description

State variable:
- position (pos)

Time-varying reference:
- target(k) = base + ramp_rate*k + A*sin(w*k)

Deviation definition:
- delta = target − pos

Plant update rule (explicit and external):
- posₖ₊₁ = posₖ + uₖ

All plant state is external to the ECT controller.

---

### Operator Configuration

Reported by simulation header:

- bounds = [−1.0, +1.0]
- alpha = 0.8
- target = base + ramp_rate*k + A*sin(w*k)
- base = 0.0
- ramp_rate = 0.02
- A = 1.0
- w = 0.15

Controller operator chain:

- F   = LinearFOperator
- E   = LinearEOperator, alpha = 0.8
- F⁻¹ = LinearFInvOperator
- G   = LinearGOperator
  - gain = 1.0
  - bounds = [−1.0, +1.0]

No saturation is observed in the recorded transcript (all steps marked FREE).

---

### Simulation Procedure

At each step k:

1. compute targetₖ from the defined reference function
2. deltaₖ = targetₖ − posₖ
3. uₖ = Controller.update(deltaₖ)
4. posₖ₊₁ = posₖ + uₖ

A status flag is printed:
- FREE (unsaturated)
- SAT_MAX / SAT_MIN (if saturation occurs)

---

### Observed Runtime Output (Canonical Transcript)

Recorded program header:

Time-varying target | bounds=[-1.000000, 1.000000] | alpha=0.8 | target = base + ramp_rate*k + A*sin(w*k) | base=0.000000 ramp_rate=0.020000 A=1.000000 w=0.150000

Start-up (initial acquisition):

- Step   0 | target=  0.000000 | pos=  0.000000 | delta=  0.000000 | u=  0.000000 | FREE
- Step   1 | target=  0.169438 | pos=  0.135551 | delta=  0.169438 | u=  0.135551 | FREE
- Step   2 | target=  0.335520 | pos=  0.295526 | delta=  0.199970 | u=  0.159976 | FREE
- Step   3 | target=  0.494966 | pos=  0.455078 | delta=  0.199439 | u=  0.159551 | FREE
- Step   4 | target=  0.644642 | pos=  0.606730 | delta=  0.189565 | u=  0.151652 | FREE
- Step   5 | target=  0.781639 | pos=  0.746657 | delta=  0.174909 | u=  0.139927 | FREE
- Step   6 | target=  0.903327 | pos=  0.871993 | delta=  0.156670 | u=  0.125336 | FREE

Crossing and sign change (reference reverses relative to pos):

- Step  12 | target=  1.213848 | pos=  1.213204 | delta=  0.003216 | u=  0.002573 | FREE
- Step  13 | target=  1.188960 | pos=  1.193809 | delta= -0.024245 | u= -0.019396 | FREE
- Step  14 | target=  1.143209 | pos=  1.153329 | delta= -0.050599 | u= -0.040479 | FREE
- Step  15 | target=  1.078073 | pos=  1.093124 | delta= -0.075256 | u= -0.060205 | FREE

Sustained tracking (bounded steady tracking error, typical magnitudes ~0.15–0.21):

- Step  40 | target=  0.520585 | pos=  0.481131 | delta=  0.197266 | u=  0.157813 | FREE
- Step  41 | target=  0.687208 | pos=  0.645993 | delta=  0.206077 | u=  0.164861 | FREE
- Step  42 | target=  0.856814 | pos=  0.814650 | delta=  0.210821 | u=  0.168657 | FREE
- Step  43 | target=  1.026042 | pos=  0.983764 | delta=  0.211392 | u=  0.169114 | FREE
- Step  44 | target=  1.191541 | pos=  1.149986 | delta=  0.207778 | u=  0.166222 | FREE

Later segment (continued deterministic tracking and sign flips):

- Step  96 | target=  2.885658 | pos=  2.886259 | delta= -0.003006 | u= -0.002404 | FREE
- Step  97 | target=  2.855988 | pos=  2.862042 | delta= -0.030271 | u= -0.024217 | FREE
- Step  98 | target=  2.805747 | pos=  2.817006 | delta= -0.056295 | u= -0.045036 | FREE
- Step  99 | target=  2.736512 | pos=  2.752611 | delta= -0.080494 | u= -0.064395 | FREE
- Step 100 | target=  2.650288 | pos=  2.670752 | delta= -0.102323 | u= -0.081858 | FREE

Final lines of the provided transcript:

- Step 197 | target=  2.983236 | pos=  2.993027 | delta= -0.048958 | u= -0.039166 | FREE
- Step 198 | target=  2.970513 | pos=  2.975016 | delta= -0.022514 | u= -0.018011 | FREE
- Step 199 | target=  2.980012 | pos=  2.979013 | delta=  0.004996 | u=  0.003997 | FREE

Full transcript retained as provided in the canonical log source.

---

### Observed Behavior

From the recorded numeric output:

- The controller tracks a moving target deterministically with bounded u.
- The deviation delta regularly changes sign (e.g., Step 12 → 15),
  indicating expected phase-lag when the reference reverses or accelerates.
- A characteristic steady tracking error emerges (often ~0.15–0.21),
  consistent with a stateless proportional contraction mapping
  applied to a continuously moving reference.
- No saturation is triggered in the recorded run (all steps FREE),
  despite tight bounds [−1, +1], because required u remains well below limits.

---

### Behavioral Interpretation

Because the controller is stateless:

- it cannot “predict” target motion or compensate with internal dynamics,
- it reacts only to instantaneous delta,
- therefore a bounded tracking error is expected when the reference moves.

The transcript shows a stable tracking regime:
u follows delta proportionally (via alpha = 0.8) while remaining bounded,
and the plant follows without oscillatory instability.

---

### Behavioral Conclusion

This simulation provides a concrete numeric demonstration that ECT-SDK:

- can track a time-varying reference in a stable and deterministic way,
- produces bounded control actions under tight actuator limits,
- exhibits an expected steady tracking error due to stateless design,
- handles sign changes smoothly without oscillatory artifacts.

This scenario serves as the canonical behavioral reference
for time-varying target tracking in ECT-SDK.

---

## 8. Simulation: time_varying_target_saturation_limit

### Purpose

This simulation illustrates ECT-SDK behavior when tracking the same **time-varying target**
as in the previous scenario, but with a **much tighter actuator bound**.

The purpose is to observe, using recorded numeric output:

- persistent saturation when target motion exceeds actuator authority,
- transitions between SAT_MAX / SAT_MIN and FREE,
- the emergence of a large tracking error under limited control authority,
- deterministic “bang-bang + brief free” tracking behavior,
- stability without wind-up (no internal accumulation; output is always bounded).

This simulation is illustrative and does not constitute formal verification.

---

### Source and Binary

Source file:
- examples/time_varying_target_saturation_limit.cpp

Generated binary:
- build/time_varying_target_saturation_limit.exe

---

### Simulated System Description

State variable:
- position (pos)

Time-varying reference:
- target(k) = base + ramp_rate*k + A*sin(w*k)

Deviation definition:
- delta = target − pos

Plant update rule (explicit and external):
- posₖ₊₁ = posₖ + uₖ

All plant state is external to the ECT controller.

---

### Operator Configuration

Reported by simulation header:

- bounds = [−0.100000, +0.100000]
- alpha = 0.8
- base = 0.0
- ramp_rate = 0.02
- A = 1.0
- w = 0.15

Controller operator chain:

- F   = LinearFOperator
- E   = LinearEOperator, alpha = 0.8
- F⁻¹ = LinearFInvOperator
- G   = LinearGOperator
  - gain = 1.0
  - bounds = [−0.1, +0.1]  (hard saturation)

---

### Simulation Procedure

At each step k:

1. compute targetₖ from the defined reference function
2. deltaₖ = targetₖ − posₖ
3. uₖ = Controller.update(deltaₖ) with hard bounds [−0.1, +0.1]
4. posₖ₊₁ = posₖ + uₖ

A status flag is printed:
- SAT_MAX when u = +0.1
- SAT_MIN when u = −0.1
- FREE otherwise

---

### Observed Runtime Output (Canonical Transcript)

Recorded program header:

Time-varying target (SAT LIMIT) | bounds=[-0.100000, 0.100000] | alpha=0.8 | target = base + ramp_rate*k + A*sin(w*k) | base=0.000000 ramp_rate=0.020000 A=1.000000 w=0.150000

Start-up and immediate saturation (positive direction):

- Step   0 | target=  0.000000 | pos=  0.000000 | delta=  0.000000 | u=  0.000000 | FREE
- Step   1 | target=  0.169438 | pos=  0.100000 | delta=  0.169438 | u=  0.100000 | SAT_MAX
- Step   2 | target=  0.335520 | pos=  0.200000 | delta=  0.235520 | u=  0.100000 | SAT_MAX
  ...
- Step  11 | target=  1.216865 | pos=  1.100000 | delta=  0.216865 | u=  0.100000 | SAT_MAX

First saturation release (demand drops below +0.1):

- Step  12 | target=  1.213848 | pos=  1.191078 | delta=  0.113848 | u=  0.091078 | FREE
- Step  13 | target=  1.188960 | pos=  1.189383 | delta= -0.002118 | u= -0.001695 | FREE
- Step  14 | target=  1.143209 | pos=  1.152444 | delta= -0.046174 | u= -0.036939 | FREE
- Step  15 | target=  1.078073 | pos=  1.092947 | delta= -0.074371 | u= -0.059497 | FREE
- Step  16 | target=  0.995463 | pos=  1.014960 | delta= -0.097484 | u= -0.077987 | FREE
- Step  17 | target=  0.897684 | pos=  0.921139 | delta= -0.117276 | u= -0.093821 | FREE

Negative saturation phase (tracking deficit grows while u clamps at −0.1):

- Step  18 | target=  0.787380 | pos=  0.821139 | delta= -0.133759 | u= -0.100000 | SAT_MIN
- Step  19 | target=  0.667478 | pos=  0.721139 | delta= -0.153661 | u= -0.100000 | SAT_MIN
  ...
- Step  29 | target= -0.355053 | pos= -0.278861 | delta= -0.176192 | u= -0.100000 | SAT_MIN

Release from SAT_MIN (demand returns within bounds):

- Step  30 | target= -0.377530 | pos= -0.357796 | delta= -0.098669 | u= -0.078935 | FREE
- Step  31 | target= -0.378054 | pos= -0.374003 | delta= -0.020258 | u= -0.016207 | FREE
- Step  32 | target= -0.356165 | pos= -0.359732 | delta=  0.017838 | u=  0.014271 | FREE
- Step  33 | target= -0.311903 | pos= -0.321469 | delta=  0.047829 | u=  0.038263 | FREE
- Step  34 | target= -0.245815 | pos= -0.260946 | delta=  0.075654 | u=  0.060523 | FREE
- Step  35 | target= -0.158934 | pos= -0.179337 | delta=  0.102011 | u=  0.081609 | FREE

Back into SAT_MAX as target rises faster than the actuator can follow:

- Step  36 | target= -0.052764 | pos= -0.079337 | delta=  0.126572 | u=  0.100000 | SAT_MAX
- Step  37 | target=  0.070760 | pos=  0.020663 | delta=  0.150097 | u=  0.100000 | SAT_MAX
  ...
- Step  56 | target=  1.974599 | pos=  1.920663 | delta=  0.153936 | u=  0.100000 | SAT_MAX

Short FREE transition around sign flip:

- Step  57 | target=  1.907401 | pos=  1.910054 | delta= -0.013262 | u= -0.010610 | FREE
- Step  58 | target=  1.822969 | pos=  1.840386 | delta= -0.087084 | u= -0.069667 | FREE
- Step  59 | target=  1.723648 | pos=  1.746996 | delta= -0.116738 | u= -0.093390 | FREE

Back into SAT_MIN for an extended interval:

- Step  60 | target=  1.612118 | pos=  1.646996 | delta= -0.134877 | u= -0.100000 | SAT_MIN
  ...
- Step  71 | target=  0.479119 | pos=  0.546996 | delta= -0.167877 | u= -0.100000 | SAT_MIN

Release from SAT_MIN:

- Step  72 | target=  0.459064 | pos=  0.476650 | delta= -0.087932 | u= -0.070346 | FREE
- Step  73 | target=  0.461038 | pos=  0.464161 | delta= -0.015612 | u= -0.012490 | FREE
- Step  74 | target=  0.485447 | pos=  0.481190 | delta=  0.021287 | u=  0.017029 | FREE
- Step  75 | target=  0.532192 | pos=  0.521992 | delta=  0.051002 | u=  0.040802 | FREE
- Step  76 | target=  0.600671 | pos=  0.584936 | delta=  0.078680 | u=  0.062944 | FREE
- Step  77 | target=  0.689797 | pos=  0.668825 | delta=  0.104862 | u=  0.083889 | FREE

Re-entry to SAT_MAX as delta exceeds bound again:

- Step  78 | target=  0.798016 | pos=  0.768825 | delta=  0.129192 | u=  0.100000 | SAT_MAX
  ...
- Step  98 | target=  2.805747 | pos=  2.768825 | delta=  0.136922 | u=  0.100000 | SAT_MAX

FREE sign-crossing (target begins to fall relative to pos):

- Step  99 | target=  2.736512 | pos=  2.742975 | delta= -0.032313 | u= -0.025850 | FREE
- Step 100 | target=  2.650288 | pos=  2.668825 | delta= -0.092687 | u= -0.074149 | FREE
- Step 101 | target=  2.549459 | pos=  2.573333 | delta= -0.119366 | u= -0.095493 | FREE

Extended SAT_MIN region:

- Step 102 | target=  2.436741 | pos=  2.473333 | delta= -0.136592 | u= -0.100000 | SAT_MIN
  ...
- Step 113 | target=  1.313556 | pos=  1.373333 | delta= -0.159776 | u= -0.100000 | SAT_MIN

Release and re-entry patterns continue deterministically (selected late transitions):

- Step 114 | target=  1.295935 | pos=  1.311415 | delta= -0.077398 | u= -0.061918 | FREE
- Step 120 | target=  1.649013 | pos=  1.617231 | delta=  0.131782 | u=  0.100000 | SAT_MAX
  ...
- Step 140 | target=  3.636656 | pos=  3.612771 | delta=  0.119424 | u=  0.095540 | FREE
- Step 144 | target=  3.261250 | pos=  3.299480 | delta= -0.138229 | u= -0.100000 | SAT_MIN
  ...
- Step 156 | target=  2.133084 | pos=  2.146363 | delta= -0.066395 | u= -0.053116 | FREE
- Step 162 | target=  2.500221 | pos=  2.465880 | delta=  0.134341 | u=  0.100000 | SAT_MAX
  ...
- Step 182 | target=  4.467328 | pos=  4.447038 | delta=  0.101448 | u=  0.081158 | FREE
- Step 186 | target=  4.085653 | pos=  4.125397 | delta= -0.139744 | u= -0.100000 | SAT_MIN
  ...
- Step 198 | target=  2.970513 | pos=  2.981490 | delta= -0.054884 | u= -0.043907 | FREE
- Step 199 | target=  2.980012 | pos=  2.980307 | delta= -0.001478 | u= -0.001182 | FREE

Full transcript retained as provided in the canonical log source.

---

### Observed Behavior

From the recorded numeric output:

- With bounds [−0.1, +0.1], the controller saturates frequently and for long intervals.
- During SAT_MAX / SAT_MIN, the position advances by a fixed increment per step
  (±0.1), while delta continues to evolve primarily with target motion.
- Saturation releases only when the instantaneous demanded output falls within bounds
  (e.g., Step 12 becomes FREE with u=0.091078 after SAT_MAX).
- The system exhibits repeated cycles:
  SAT_MAX → brief FREE near sign crossing → SAT_MIN → brief FREE → SAT_MAX ...
- Tracking error magnitude grows significantly during saturation intervals
  (e.g., delta reaches ~0.73 during sustained SAT_MAX around Steps 48–56).

No evidence of instability is present: behavior remains bounded and deterministic.

---

### Behavioral Interpretation

This transcript demonstrates **authority-limited tracking**:

- The controller can only change pos at ±0.1 per step during saturation.
- When the target velocity/acceleration exceeds that authority,
  the controller cannot maintain small delta, and a larger tracking error persists.
- FREE segments occur mainly near sign changes or when the target motion slows enough
  that the required u falls inside bounds.

Because the controller is stateless and bounded,
there is no wind-up-like accumulation: saturation does not “store” error internally.
The observed behavior is entirely a consequence of hard actuator limits.

---

### Behavioral Conclusion

This simulation provides a concrete numeric demonstration that ECT-SDK:

- remains stable under aggressive saturation constraints,
- exhibits deterministic authority-limited tracking of a moving reference,
- transitions cleanly between saturated and free regimes,
- maintains bounded behavior without drift or internal accumulation.

This scenario serves as the canonical behavioral reference
for time-varying target tracking under strict actuator saturation limits.
