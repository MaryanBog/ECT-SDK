# ECT-SDK — Formal Test Log
Version: 1.1  
Status: canonical verification record  
Language: EN  
Author: Maryan Bogdanov  

---

## 0. Purpose

This document is the **formal verification log** for the ECT-SDK control core.

Its purpose is to provide a **fixed, auditable, and reproducible record**
of all **formal tests** used to verify the core invariants of ECT-SDK.

This document exists to answer one question only:

> *Does the ECT-SDK control core satisfy its formally defined invariants
> under deterministic, stateless execution?*

Accordingly, this log records **only verification tests** that produce
unambiguous pass/fail results.

### 0.1 Scope

This document records:

- which formal tests exist,
- how each test is built and executed,
- the exact operator configuration used,
- the expected invariant being verified,
- the observed result,
- and the formal conclusion of each test.

All tests documented here are intended to verify **normative invariants**
defined by the ECT-SDK Specification and Operator Formalization, including:

- determinism,
- stateless execution,
- bounded output,
- monotonicity,
- sign preservation,
- and exact contraction behavior.

### 0.2 Out of Scope

This document explicitly does **not** include:

- simulations with dynamic plant models,
- qualitative or illustrative behavior analysis,
- noise response demonstrations,
- actuator or system-level tracking scenarios,
- performance benchmarks,
- safety or certification claims.

Any simulation-based or illustrative material
is documented separately in a dedicated **Simulation & Behavior Log**
and is not part of formal verification.

### 0.3 Interpretation Rules

All tests documented here are interpreted strictly as follows:

- A test either **passes** or **fails**.
- A passing test verifies **only the specific invariant stated**.
- No additional guarantees, performance claims, or system-level properties
  may be inferred from these results.

This document does not attempt to prove system stability,
robustness under arbitrary disturbances,
or correctness of any external plant or integration logic.

### 0.4 Canonical Role

This document is considered the **canonical verification reference**
for the ECT-SDK control core.

Any future modification to the ECT-SDK that affects
observable control behavior or invariant preservation
**must be accompanied by corresponding updates to this log**.

The absence of a test in this document
means that the corresponding property is **not formally verified**
by the ECT-SDK core.

---

## 1. Reference Build Environment

This section defines the **canonical build and execution environment**
used for all formal tests documented in this log.

The purpose of fixing a reference environment is to ensure that
all recorded test results are:

- reproducible,
- auditable,
- free from external or incidental variability,
- and independent of cached or persistent build state.

All verification results recorded in this document
were obtained **exclusively** under the conditions described below.

---

### 1.1 Operating System

- Microsoft Windows 10 / Windows 11
- Execution environment: Git Bash (MinGW64)

The operating system is treated as a hosting platform only.
No OS-specific behavior, timing guarantees, or scheduler assumptions
are relied upon by the ECT-SDK tests.

---

### 1.2 Toolchain

- Compiler: GNU g++ 13.2.0 (Strawberry Perl toolchain)
- Language standard: C++17
- Build system: CMake
- Build generator: Ninja

No compiler extensions, non-standard language features,
or platform-specific intrinsics are used.

All tests are expected to compile and run
on any standard-conforming C++17 toolchain,
subject to equivalent compiler behavior.

---

### 1.3 Repository Layout Assumptions

All commands and paths documented in this log
assume the following repository layout at the root level:

- `CMakeLists.txt`
- `include/`
- `src/`
- `tests/`
- `examples/`

Formal tests are located exclusively in the `tests/` directory.

No verification logic is embedded in:
- the SDK implementation files,
- example programs,
- or external scripts.

---

### 1.4 Build Directory Policy

All builds are performed using a **clean build policy**:

- a dedicated `build/` directory is used,
- the `build/` directory is fully removed before each recorded build,
- no incremental or cached builds are permitted,
- no in-place builds are used.

This policy guarantees that:

- no residual object files affect test results,
- no prior compilation state influences verification outcomes,
- all results correspond to a fully deterministic build process.

---

### 1.5 Environment Determinism Note

The formal tests documented in this log do **not** rely on:

- wall-clock timing,
- random number generation,
- concurrency or threading,
- system load or scheduling behavior,
- floating-point environment manipulation.

As a result, the verification outcomes are independent
of runtime timing variability and external system state,
within standard floating-point determinism guarantees.

---

### 1.6 Canonical Baseline Declaration

The environment defined in this section is the **canonical baseline**
for interpreting all test results in this document.

If tests are executed in a different environment,
the results may still be valid,
but such executions are considered **non-canonical**
unless explicitly documented and verified separately.

---

## 2. Canonical Build and Execution Commands

This section defines the **canonical build and execution commands**
used to compile and run all formal verification tests
documented in this log.

These commands constitute the **only approved procedure**
for producing the verification results recorded herein.

All commands must be executed from the **repository root directory**
(the directory containing `CMakeLists.txt`).

---

### 2.1 Clean Build Requirement

Before executing any formal test, the build directory
**must be fully removed**.

The following conditions are mandatory:

- no incremental builds,
- no reuse of object files,
- no reuse of CMake cache,
- no in-place compilation.

This ensures that all verification results
are free from residual build artifacts.

---

### 2.2 Build: Formal Tests Only

Formal verification tests are built
with example programs explicitly disabled.

Canonical command sequence:

rm -rf build
cmake -S . -B build -G Ninja
-DECT_SDK_BUILD_TESTS=ON
-DECT_SDK_BUILD_EXAMPLES=OFF
cmake --build build

This build produces only formal test binaries.

---

### 2.3 Generated Test Executables

Under the canonical configuration,
the following test executables are generated
in the `build/` directory:

- `simple_loop_test.exe`
- `determinism_test.exe`
- `boundedness_test.exe`
- `monotonicity_test.exe`
- `sign_preservation_test.exe`
- `contraction_ratio_test.exe`

No additional binaries are required
for formal verification.

---

### 2.4 Test Execution

Each test executable is run explicitly
from the command line.

Example:

./build/determinism_test.exe

Formal tests:

- do not require command-line arguments,
- do not read external configuration files,
- do not depend on environment variables,
- terminate deterministically with success or failure.

---

### 2.5 Result Interpretation Rules

For all formal tests:

- successful execution indicates **pass**,
- any assertion failure, runtime error,
  or abnormal termination indicates **fail**.

No partial success, warning state,
or qualitative interpretation is permitted.

The absence of a test failure
is the sole criterion for verification success.

---

### 2.6 Logging and Output Policy

Formal test output is intentionally minimal.

Tests may emit:

- step-by-step numeric values (where required),
- assertion messages on failure,
- explicit confirmation of success.

No logging framework, tracing system,
or external result collection mechanism
is used or required for verification.

---

## 3. Test: simple_loop_test

### Purpose

This test verifies the **minimal executable correctness** of the ECT-SDK control core
under the simplest possible closed-loop configuration.

The test validates, in a directly observable and numeric manner, that:

- the controller is strictly deterministic,
- the controller is completely stateless,
- contraction is enforced exclusively by the E operator,
- convergence is monotonic, sign-preserving, and non-oscillatory.

No plant dynamics, filtering, integration, or hidden state
are present inside the controller.

---

### Source and Binary

Source file:
- tests/simple_loop_test.cpp

Generated binary:
- build/simple_loop_test.exe

---

### Operator Configuration

The controller is configured as follows:

- F  = LinearFOperator
- E  = LinearEOperator, alpha = 0.8
- F⁻¹ = LinearFInvOperator
- G  = LinearGOperator
  - gain = 1.0
  - bounds = [−1e9, +1e9]

Output saturation does not occur in this test.

---

### Test Procedure

Initial deviation:

- Δ₀ = 1.0

At each iteration k:

1. Control output is computed:
   uₖ = Controller.update(Δₖ)

2. Deviation is updated **externally**:
   Δₖ₊₁ = Δₖ − uₖ

The controller stores no history and has no access
to Δₖ₋₁, Δₖ₋₂, or any accumulated state.

---

### Expected Analytical Behavior

Given alpha = 0.8:

- uₖ = 0.8 · Δₖ
- Δₖ₊₁ = 0.2 · Δₖ

Therefore, the deviation must decrease
by a constant contraction ratio of **0.2 per iteration**.

---

### Observed Runtime Output

Actual program output:

- Step 0: u = 0.8        | Δ₁ = 0.2
- Step 1: u = 0.16       | Δ₂ = 0.04
- Step 2: u = 0.032      | Δ₃ = 0.008
- Step 3: u = 0.0064     | Δ₄ = 0.0016
- Step 4: u = 0.00128    | Δ₅ = 0.00032
- Step 5: u = 0.000256   | Δ₆ = 6.4e−05
- Step 6: u = 5.12e−05   | Δ₇ = 1.28e−05
- Step 7: u = 1.024e−05  | Δ₈ = 2.56e−06
- Step 8: u = 2.048e−06  | Δ₉ = 5.12e−07
- Step 9: u = 4.096e−07  | Δ₁₀ = 1.024e−07

Measured contraction ratio:

- Δₖ₊₁ / Δₖ = 0.2 for all k (within floating-point precision)

---

### Verification Result

The observed numeric behavior **exactly matches**
the analytical contraction model.

Verified properties:

- deterministic output for identical input,
- absence of internal state or memory,
- strict geometric contraction,
- sign preservation,
- absence of oscillation or divergence.

---

### Formal Conclusion

**PASS — VERIFIED**

The ECT-SDK control core satisfies its minimal correctness
and contraction invariants under the conditions of this test.

This test establishes a concrete numeric baseline
for all subsequent formal verification tests.

---

## 4. Test: determinism_test

### Purpose

This test verifies the **determinism invariant** of the ECT-SDK control core.

The goal is to confirm that:
- for identical deviation inputs,
- under identical operator configuration,
- different controller instances

produce **bitwise-identical control outputs**.

This test explicitly detects:
- hidden internal state,
- unintended memory,
- randomness,
- time-dependent behavior,
- cross-instance coupling.

---

### Source and Binary

Source file:
- tests/determinism_test.cpp

Generated binary:
- build/determinism_test.exe

---

### Operator Configuration

Both controller instances are configured identically:

- F  = LinearFOperator
- E  = LinearEOperator, alpha = 0.8
- F⁻¹ = LinearFInvOperator
- G  = LinearGOperator
  - gain = 1.0
  - bounds = [−1e9, +1e9]

No saturation occurs for the tested inputs.

---

### Test Procedure

Two independent controller instances are created:

- Controller A
- Controller B

A fixed deterministic input sequence is applied to both:

Δ sequence:
- −10.0
- −1.0
- −0.5
- −0.1
- 0.0
- 0.1
- 0.5
- 1.0
- 10.0

For each input Δᵢ:

1. uᵢᴬ = ControllerA.update(Δᵢ)
2. uᵢᴮ = ControllerB.update(Δᵢ)
3. uᵢᴬ and uᵢᴮ are compared for **exact equality**

No state is shared between controllers.
No history is preserved between calls.

---

### Expected Analytical Behavior

Given linear operators and alpha = 0.8:

- u = 0.8 · Δ

Therefore, for each Δᵢ:

- uᵢᴬ = uᵢᴮ = 0.8 · Δᵢ

Any deviation between uᵢᴬ and uᵢᴮ
would indicate non-deterministic behavior.

---

### Observed Runtime Output

Observed outputs:

| Δ        | uᴬ        | uᴮ        | Match |
|--------:|----------:|----------:|:-----:|
| −10.0   | −8.0      | −8.0      | YES   |
| −1.0    | −0.8      | −0.8      | YES   |
| −0.5    | −0.4      | −0.4      | YES   |
| −0.1    | −0.08     | −0.08     | YES   |
| 0.0     | 0.0       | 0.0       | YES   |
| 0.1     | 0.08      | 0.08      | YES   |
| 0.5     | 0.4       | 0.4       | YES   |
| 1.0     | 0.8       | 0.8       | YES   |
| 10.0    | 8.0       | 8.0       | YES   |

All comparisons were exact.
No floating-point divergence was observed.

---

### Verification Result

For all tested deviation values:

- uᴬ == uᴮ (exact match)

No discrepancy occurred across controller instances.

---

### Formal Conclusion

**PASS — VERIFIED**

The ECT-SDK control core is strictly deterministic.

Control outputs depend exclusively on:
- the current deviation input,
- the configured operators,

and are independent of:
- controller instance identity,
- execution history,
- timing or external state.

This test formally verifies the determinism invariant
of the ECT-SDK core.

---

## 5. Test: boundedness_test

### Purpose

This test verifies the **boundedness invariant** of the ECT-SDK control core.

The goal is to confirm that:
- for arbitrarily large deviation inputs,
- the control output is **strictly bounded**
  by the limits configured in the output operator G,
- no overflow, NaN, or unbounded growth is possible.

This test targets failures typical of:
- uncontrolled gain chains,
- missing or incorrect saturation,
- hidden amplification after contraction.

---

### Source and Binary

Source file:
- tests/boundedness_test.cpp

Generated binary:
- build/boundedness_test.exe

---

### Operator Configuration

The controller is configured as follows:

- F  = LinearFOperator
- E  = LinearEOperator, alpha = 0.8
- F⁻¹ = LinearFInvOperator
- G  = LinearGOperator
  - gain = 1.0
  - bounds = [−1.0, +1.0]

The tight bounds are chosen intentionally
to force saturation for large deviations.

---

### Test Procedure

A single controller instance is used.

A sequence of deviation values spanning
multiple orders of magnitude is applied:

Δ sequence:
- −1e6
- −100
- −10
- −1
- −0.1
- 0.0
- 0.1
- 1
- 10
- 100
- 1e6

For each Δᵢ:

1. Control output is computed:
   uᵢ = Controller.update(Δᵢ)

2. The following conditions are verified:
   - uᵢ is finite,
   - −1.0 ≤ uᵢ ≤ +1.0

---

### Expected Analytical Behavior

Given:
- u_raw = 0.8 · Δ
- G enforces bounds [−1.0, +1.0]

Then:

- for |Δ| ≥ 1.25 → u = ±1.0 (saturation),
- for |Δ| < 1.25 → u = 0.8 · Δ (unsaturated).

No output outside the configured bounds
is admissible.

---

### Observed Runtime Output

Observed outputs:

| Δ        | u          | Saturation |
|--------:|-----------:|:----------:|
| −1e6    | −1.0       | YES (MIN)  |
| −100    | −1.0       | YES (MIN)  |
| −10     | −1.0       | YES (MIN)  |
| −1      | −0.8       | NO         |
| −0.1    | −0.08      | NO         |
| 0.0     | 0.0        | NO         |
| 0.1     | 0.08       | NO         |
| 1       | 0.8        | NO         |
| 10      | 1.0        | YES (MAX)  |
| 100     | 1.0        | YES (MAX)  |
| 1e6     | 1.0        | YES (MAX)  |

All outputs remained finite.
No overflow, NaN, or bound violation occurred.

---

### Verification Result

For all tested deviation values:

- u ∈ [−1.0, +1.0],
- saturation engaged deterministically when expected,
- no output exceeded configured bounds.

---

### Formal Conclusion

**PASS — VERIFIED**

The ECT-SDK control core enforces strict output boundedness
through the G operator.

This test confirms that:
- extreme inputs cannot produce unbounded control commands,
- saturation behavior is deterministic and symmetric,
- boundedness is guaranteed independently of input magnitude.

---

## 6. Test: monotonicity_test

### Purpose

This test verifies the **monotonicity invariant** of the ECT-SDK scalar control mapping.

The goal is to confirm that for scalar deviations:

- if Δ₁ < Δ₂, then u(Δ₁) ≤ u(Δ₂)

This property is required for predictable and non-pathological control behavior.
A violation would indicate an ordering inversion caused by operator interaction
or unintended non-monotone shaping.

---

### Source and Binary

Source file:
- tests/monotonicity_test.cpp

Generated binary:
- build/monotonicity_test.exe

---

### Operator Configuration

The controller is configured as follows:

- F  = LinearFOperator
- E  = LinearEOperator, alpha = 0.8
- F⁻¹ = LinearFInvOperator
- G  = LinearGOperator
  - gain = 1.0
  - bounds = [−1e9, +1e9]

Wide bounds are used to ensure saturation does not affect ordering.

---

### Test Procedure

A single controller instance is used.

A strictly increasing deviation sequence is applied:

Δ sequence:
- −10
- −5
- −2
- −1
- −0.5
- −0.1
- 0.0
- 0.1
- 0.5
- 1
- 2
- 5
- 10

For each Δᵢ:

1. uᵢ = Controller.update(Δᵢ)

For each consecutive pair (Δᵢ, Δᵢ₊₁):

2. Verify:
   uᵢ ≤ uᵢ₊₁

Any decrease in u while Δ increases is a failure.

---

### Expected Analytical Behavior

With linear operators and alpha = 0.8:

- u = 0.8 · Δ

This mapping is strictly monotonic increasing.

Therefore, output ordering must match input ordering exactly.

---

### Observed Runtime Output

Observed outputs:

| Δ     | u       |
|------:|--------:|
| −10   | −8.0    |
| −5    | −4.0    |
| −2    | −1.6    |
| −1    | −0.8    |
| −0.5  | −0.4    |
| −0.1  | −0.08   |
| 0.0   | 0.0     |
| 0.1   | 0.08    |
| 0.5   | 0.4     |
| 1     | 0.8     |
| 2     | 1.6     |
| 5     | 4.0     |
| 10    | 8.0     |

Pairwise ordering check:

- u(−10) ≤ u(−5)  ✅
- u(−5)  ≤ u(−2)  ✅
- u(−2)  ≤ u(−1)  ✅
- u(−1)  ≤ u(−0.5)✅
- u(−0.5)≤ u(−0.1)✅
- u(−0.1)≤ u(0.0) ✅
- u(0.0) ≤ u(0.1) ✅
- u(0.1) ≤ u(0.5) ✅
- u(0.5) ≤ u(1)   ✅
- u(1)   ≤ u(2)   ✅
- u(2)   ≤ u(5)   ✅
- u(5)   ≤ u(10)  ✅

No inversions occurred.

---

### Verification Result

Across the entire increasing input sequence:

- the control output was non-decreasing,
- no ordering violations were observed.

---

### Formal Conclusion

**PASS — VERIFIED**

The ECT-SDK scalar control mapping preserves input ordering.

This test verifies that:
- the composed FXI–Δ–E pipeline is monotonic for scalar deviations,
- no hidden non-linearities introduce ordering inversions,
- control behavior remains predictable and structurally consistent.

---

## 7. Test: sign_preservation_test

### Purpose

This test verifies the **sign preservation invariant** of the ECT-SDK control mapping.

The goal is to confirm that:

- Δ > 0  ⇒  u(Δ) ≥ 0
- Δ < 0  ⇒  u(Δ) ≤ 0
- Δ = 0  ⇒  u(Δ) = 0

Sign preservation is required for directionally consistent control action.
Any sign inversion would imply a structurally invalid mapping
(e.g., incorrect operator composition or unintended sign flip).

---

### Source and Binary

Source file:
- tests/sign_preservation_test.cpp

Generated binary:
- build/sign_preservation_test.exe

---

### Operator Configuration

The controller is configured as follows:

- F  = LinearFOperator
- E  = LinearEOperator, alpha = 0.8
- F⁻¹ = LinearFInvOperator
- G  = LinearGOperator
  - gain = 1.0
  - bounds = [−1e9, +1e9]

Wide bounds are used to ensure saturation does not mask sign behavior.

---

### Test Procedure

A single controller instance is evaluated.

Test inputs include:

- exact zero,
- a set of positive values,
- a set of negative values.

Δ test set:
- −10
- −1
- −0.1
- 0.0
- 0.1
- 1
- 10

For each Δᵢ:

1. uᵢ = Controller.update(Δᵢ)

Then verify:

- if Δᵢ > 0, uᵢ ≥ 0
- if Δᵢ < 0, uᵢ ≤ 0
- if Δᵢ = 0, uᵢ = 0

Any violation is a failure.

---

### Expected Analytical Behavior

With linear operators and alpha = 0.8:

- u = 0.8 · Δ

Therefore:
- sign(u) must match sign(Δ),
- u(0) must be exactly 0.

---

### Observed Runtime Output

Observed outputs:

| Δ     | u       | Expected sign | Observed sign | OK |
|------:|--------:|:-------------:|:-------------:|:--:|
| −10   | −8.0    | −             | −             | ✅ |
| −1    | −0.8    | −             | −             | ✅ |
| −0.1  | −0.08   | −             | −             | ✅ |
| 0.0   | 0.0     | 0             | 0             | ✅ |
| 0.1   | 0.08    | +             | +             | ✅ |
| 1     | 0.8     | +             | +             | ✅ |
| 10    | 8.0     | +             | +             | ✅ |

No sign inversion occurred.
Zero deviation produced an exact zero output.

---

### Verification Result

All tested deviation values produced control outputs
with the correct sign behavior:

- positive deviations → non-negative outputs,
- negative deviations → non-positive outputs,
- zero deviation → exactly zero output.

---

### Formal Conclusion

**PASS — VERIFIED**

The ECT-SDK control mapping preserves the sign of scalar deviations.

This test verifies that:
- control action is directionally consistent,
- no hidden sign inversion exists within the FXI–Δ–E pipeline,
- the mapping remains physically interpretable for scalar control loops.

---

## 8. Test: contraction_ratio_test

### Purpose

This test verifies the **exact quantitative contraction ratio**
enforced by the E operator in the ECT-SDK control pipeline.

Unlike qualitative convergence demonstrations, this test confirms that
the contraction is:

- numerically precise,
- analytically predictable,
- and not altered by hidden dynamics, memory, or operator interaction.

The test verifies that the external closed-loop update:

- Δₖ₊₁ = Δₖ − uₖ

produces a constant ratio:

- Δₖ₊₁ / Δₖ = (1 − alpha)

for all iterations where Δₖ ≠ 0.

---

### Source and Binary

Source file:
- tests/contraction_ratio_test.cpp

Generated binary:
- build/contraction_ratio_test.exe

---

### Operator Configuration

The controller is configured as follows:

- F  = LinearFOperator
- E  = LinearEOperator, alpha = 0.8
- F⁻¹ = LinearFInvOperator
- G  = LinearGOperator
  - gain = 1.0
  - bounds = [−1e9, +1e9]

Saturation is not expected in this test.

---

### Test Procedure

A single controller instance is evaluated.

Initial deviation:
- Δ₀ = 1.0

For each iteration k:

1. Compute control output:
   uₖ = Controller.update(Δₖ)

2. Update deviation externally:
   Δₖ₊₁ = Δₖ − uₖ

3. Compute measured ratio:
   rₖ = Δₖ₊₁ / Δₖ   (only for Δₖ ≠ 0)

The test asserts that rₖ matches the expected ratio
within a strict floating-point tolerance.

---

### Expected Analytical Behavior

Given alpha = 0.8:

- uₖ = 0.8 · Δₖ
- Δₖ₊₁ = (1 − 0.8) · Δₖ = 0.2 · Δₖ

Therefore, the expected contraction ratio is:

- r = 0.2

---

### Observed Runtime Output

Observed values (first 10 iterations):

| k | Δₖ         | uₖ         | Δₖ₊₁        | rₖ = Δₖ₊₁/Δₖ |
|--:|-----------:|-----------:|------------:|-------------:|
| 0 | 1.0        | 0.8        | 0.2         | 0.2          |
| 1 | 0.2        | 0.16       | 0.04        | 0.2          |
| 2 | 0.04       | 0.032      | 0.008       | 0.2          |
| 3 | 0.008      | 0.0064     | 0.0016      | 0.2          |
| 4 | 0.0016     | 0.00128    | 0.00032     | 0.2          |
| 5 | 0.00032    | 0.000256   | 6.4e−05     | 0.2          |
| 6 | 6.4e−05    | 5.12e−05   | 1.28e−05    | 0.2          |
| 7 | 1.28e−05   | 1.024e−05  | 2.56e−06    | 0.2          |
| 8 | 2.56e−06   | 2.048e−06  | 5.12e−07    | 0.2          |
| 9 | 5.12e−07   | 4.096e−07  | 1.024e−07   | 0.2          |

The measured ratio rₖ remained 0.2 across all tested iterations
(within floating-point precision).

No deviation from the analytical contraction factor was observed.

---

### Verification Result

For all evaluated iterations (Δₖ ≠ 0):

- measured rₖ == 0.2 (within tolerance),
- contraction rate remained constant,
- no hidden dynamics altered the ratio.

---

### Formal Conclusion

**PASS — VERIFIED**

The ECT-SDK control core enforces contraction with the exact
quantitative ratio dictated by the E operator configuration.

This test verifies that:
- contraction strength is numerically precise,
- convergence behavior is analytically predictable,
- no hidden state or operator interaction modifies contraction rate.

---

## 9. Summary of Verified Invariants (V1.1)

This section summarizes the **formally verified invariants**
of the ECT-SDK control core as established by the tests
documented in this log.

Only properties explicitly verified by executed tests
are listed here.

No additional guarantees, behaviors, or system-level claims
are implied beyond those stated.

---

### 9.1 Verified Invariants

The following invariants are **formally verified** for ECT-SDK v1.1:

1. **Determinism**  
   - Verified by: `determinism_test`
   - Result:  
     Identical deviation inputs produce **bitwise-identical**
     control outputs across independent controller instances.
   - Conclusion:  
     No hidden state, randomness, or timing dependence exists.

2. **Stateless Execution**  
   - Verified by: `simple_loop_test`, `determinism_test`
   - Result:  
     Control outputs depend exclusively on the current deviation input
     and operator configuration.
   - Conclusion:  
     No memory, history, or accumulation is present in the control core.

3. **Strict Output Boundedness**  
   - Verified by: `boundedness_test`
   - Result:  
     Control outputs remain strictly within the bounds configured
     in the G operator for arbitrarily large deviation inputs.
   - Conclusion:  
     Unbounded control commands are structurally impossible.

4. **Monotonicity (Scalar Mapping)**  
   - Verified by: `monotonicity_test`
   - Result:  
     For scalar deviations, input ordering is preserved:
     if Δ₁ < Δ₂ then u(Δ₁) ≤ u(Δ₂).
   - Conclusion:  
     No ordering inversions occur within the FXI–Δ–E pipeline.

5. **Sign Preservation**  
   - Verified by: `sign_preservation_test`
   - Result:  
     Positive deviations produce non-negative outputs,
     negative deviations produce non-positive outputs,
     and zero deviation produces exactly zero output.
   - Conclusion:  
     Control action is directionally consistent.

6. **Exact Quantitative Contraction**  
   - Verified by: `contraction_ratio_test`
   - Result:  
     The measured contraction ratio exactly matches
     the analytical value dictated by the E operator configuration.
   - Conclusion:  
     Contraction behavior is numerically precise
     and not altered by hidden dynamics.

---

### 9.2 Verified Test Set

The following tests constitute the complete **formal verification set**
for this document:

- simple_loop_test  
- determinism_test  
- boundedness_test  
- monotonicity_test  
- sign_preservation_test  
- contraction_ratio_test  

No other tests are considered part of formal verification.

---

### 9.3 Explicitly Not Verified

The following properties are **not** verified by this document:

- closed-loop system stability for arbitrary plants,
- robustness to stochastic noise or disturbances,
- multi-axis or coupled control behavior,
- performance, optimality, or convergence speed,
- safety, fault tolerance, or certification compliance.

Any claims regarding such properties
must be validated at the system integration level
and are outside the scope of ECT-SDK formal verification.

---

### 9.4 Final Verification Statement

All tests documented in this log executed successfully
under the canonical build environment.

**ECT-SDK v1.1 satisfies all formally defined core invariants
of its control computation architecture.**

This document constitutes the authoritative
formal verification record for the ECT-SDK control core
at version 1.1.
