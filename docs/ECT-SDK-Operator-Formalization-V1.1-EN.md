# ECT-SDK — Operator Formalization
## Version 1.1

### Formal Operator Contracts for the FXI–Δ–E Control Core

---

## 0. Document Purpose

This document defines **formal behavioral contracts** for the operators used in **ECT-SDK**:

- Embedding operator **F**
- Contraction operator **E**
- Inverse mapping operator **F⁻¹**
- Output operator **G**

The purpose of this document is to **explicitly constrain the admissible operator space**
in order to preserve the core invariants of ECT-SDK.

This document does **not** introduce new algorithms, control logic, or system-level behavior.

---

## 0.1 Scope

This document applies to:

- ECT-SDK **v1.0 and later**
- All current and future operator implementations used with the ECT controller
- Both built-in and user-defined operators

This document is **normative** for operator behavior and **advisory** for implementation techniques.

---

## 0.2 Non-Goals

This document explicitly does **not**:

- define a control system or application architecture
- specify scheduling, safety, or redundancy mechanisms
- introduce plant models or dynamic state
- modify the FXI–Δ–E execution pipeline
- mandate a specific programming language or implementation

---

## 0.3 Relationship to Other Documents

- **ECT-SDK Specification** — defines required external behavior
- **ECT-SDK Design Document** — defines architectural structure
- **ECT-SDK Implementation Document** — describes the reference implementation

This document **refines** those documents by constraining **operator admissibility**,
not controller behavior.

---

## 1. Operator Model and Terminology

### 1.1 Operator Definition

In the context of ECT-SDK, an **operator** is a deterministic, stateless function
that maps an input value to an output value within a single control update step.

Formally, an operator is defined as:

\[
\mathcal{O} : \mathbb{R}^n \rightarrow \mathbb{R}^m
\]

where the mapping:

- is evaluated synchronously,
- has no side effects,
- does not depend on historical inputs,
- does not retain internal state between evaluations.

An operator must be executable as a **pure computation**.

---

### 1.2 Operator Role in the FXI–Δ–E Pipeline

ECT-SDK defines four operator roles with fixed semantic positions:

1. **F — Embedding Operator**  
   Transforms the raw deviation into an embedded representation.

2. **E — Contraction Operator**  
   Applies stabilization by enforcing contraction in the embedded space.

3. **F⁻¹ — Inverse Mapping Operator**  
   Maps the contracted embedded value back to the deviation domain.

4. **G — Output Operator**  
   Produces the final bounded control command.

Each operator:

- is executed exactly once per update step,
- is invoked exclusively by the controller,
- must not invoke other operators directly.

The execution order is fixed and must not be altered.

---

### 1.3 Statelessness Requirement

All operators must be **stateless**.

Specifically, an operator must not:

- store previous input or output values,
- accumulate internal variables over time,
- implement integrators, differentiators, or filters with memory,
- rely on static or global mutable state.

Any operator whose output depends on past evaluations
is **non-admissible** within ECT-SDK.

---

### 1.4 Determinism Requirement

Operators must be deterministic.

For identical inputs and identical internal configuration,
an operator must always produce identical outputs.

Operators must not depend on:

- random number generation,
- timing information,
- execution order outside the FXI–Δ–E pipeline,
- external mutable state.

---

### 1.5 Bounded-Time Execution

Each operator must execute in **bounded time**.

Operators must not:

- contain unbounded loops,
- perform recursion without strict bounds,
- allocate dynamic memory during evaluation,
- block or wait on external resources.

Bounded execution time is a required property
for real-time compatibility of the ECT-SDK.

---

### 1.6 Operator Admissibility

An operator is considered **admissible** within ECT-SDK if and only if
it satisfies all requirements defined in this section.

Violation of any requirement renders the operator
**invalid**, regardless of mathematical intent or empirical performance.

---

## 2. Core Invariants and Preservation Requirements

### 2.1 Purpose of Invariants

ECT-SDK is defined by a set of **core invariants** that must hold for all valid
configurations and executions of the FXI–Δ–E pipeline.

These invariants are not implementation details.
They define the **identity and admissibility boundary** of ECT-SDK.

All operators used within the SDK must preserve these invariants.

---

### 2.2 Determinism Invariant

For a given deviation input Δ and a fixed operator configuration,
the control output u must be uniquely determined.

Formally:

If  
Δ₁ = Δ₂  
then  
u₁ = u₂

Operators must not introduce randomness, nondeterministic branching,
or dependence on external mutable state.

Preservation of determinism is mandatory for all operators.

---

### 2.3 Statelessness Invariant

ECT-SDK is stateless with respect to control logic.

Each control update step must depend only on:
- the current deviation input Δ
- the current operator configuration

Operators must not:
- accumulate internal memory,
- reference past inputs or outputs,
- implement time-dependent behavior.

Any operator that violates statelessness
breaks the ECT execution model and is non-admissible.

---

### 2.4 Boundedness Invariant

For any finite deviation input Δ, the resulting control output u
must be finite and bounded.

Boundedness must be enforced explicitly,
either by the output operator G
or by the combined behavior of preceding operators.

Operators must not:
- amplify unbounded inputs into unbounded outputs,
- silently propagate NaN or infinite values.

Bounded output is a required invariant of ECT-SDK.

---

### 2.5 Execution Order Invariant

The FXI–Δ–E execution order is fixed:

F → E → F⁻¹ → G

Operators must not:
- alter this execution order,
- bypass other operators,
- conditionally skip stages,
- introduce additional hidden stages.

All operator contracts are defined
with respect to this fixed execution order.

---

### 2.6 Bounded-Time Execution Invariant

Each control update step must complete in bounded time.

Operators must not:
- introduce unbounded computational complexity,
- perform dynamic allocation during evaluation,
- block on external resources.

Violation of bounded-time execution
invalidates real-time compatibility of the SDK.

---

### 2.7 Invariant Preservation Responsibility

Invariant preservation is the responsibility of:
- operator designers,
- integrators configuring custom operators.

ECT-SDK does not attempt to infer or repair
violations of these invariants at runtime.

An operator that violates any invariant defined in this section
is considered **invalid**, regardless of observed behavior.

---

## 3. Embedding Operator (F) Contract

### 3.1 Role of the Embedding Operator

The embedding operator **F** maps the raw engineering deviation Δ
into an embedded representation suitable for contraction.

The purpose of F is **not stabilization** and **not output shaping**,
but controlled transformation of the deviation domain
to enable effective contraction by operator E.

---

### 3.2 Determinism and Statelessness

The embedding operator F must satisfy:

- deterministic behavior for identical inputs,
- complete statelessness across evaluations,
- absence of side effects or external dependencies.

F must not:
- retain internal memory,
- depend on historical deviation values,
- introduce randomness or timing dependence.

---

### 3.3 Monotonicity Requirement

F must be **monotonic** with respect to the deviation input.

For scalar deviations, this implies:

If  
Δ₁ < Δ₂  
then  
F(Δ₁) ≤ F(Δ₂)

Monotonicity ensures that ordering of deviations
is preserved through the embedding stage.

Non-monotonic embedding operators are non-admissible,
as they may introduce ambiguity or instability
in subsequent contraction.

---

### 3.4 Continuity Near the Origin

F must be continuous in a neighborhood of Δ = 0.

Specifically:
- small changes in Δ near zero must result in small changes in F(Δ),
- discontinuities, dead zones, or jumps at or near zero are not permitted.

Continuity near the origin is required to preserve
predictable behavior under small deviations.

---

### 3.5 Local Invertibility Requirement

F must be **locally invertible** in the operational range
used by the inverse mapping operator F⁻¹.

Exact analytical invertibility is not required.
However, F must preserve sufficient information
to allow a well-defined inverse mapping
within the intended operating domain.

Embedding operators that collapse distinct inputs
into indistinguishable embedded values
are non-admissible.

---

### 3.6 Boundedness Behavior

F may be bounded or unbounded.

However, if F introduces saturation or bounding,
such behavior must be:
- explicit,
- symmetric where appropriate,
- compatible with the contraction behavior of E.

Hidden or implicit saturation inside F
is discouraged unless explicitly documented.

---

### 3.7 Admissibility Summary for F

An embedding operator F is admissible within ECT-SDK
if and only if it satisfies:

- determinism,
- statelessness,
- monotonicity,
- continuity near the origin,
- local invertibility within the operating range.

Violation of any requirement renders F non-admissible,
regardless of empirical performance or tuning.

---

## 4. Contraction Operator (E) Contract

### 4.1 Role of the Contraction Operator

The contraction operator **E** is the primary stabilizing element
of the ECT-SDK pipeline.

Its role is to reduce the magnitude of the embedded deviation
produced by the embedding operator F.

Stabilization in ECT-SDK is achieved through **contraction**,
not through integration, differentiation, or dynamic feedback.

---

### 4.2 Determinism and Statelessness

The contraction operator E must satisfy:

- deterministic behavior for identical inputs,
- complete statelessness across evaluations,
- absence of side effects or external dependencies.

E must not:
- accumulate internal state,
- depend on historical values,
- introduce time-dependent behavior.

---

### 4.3 Non-Expansiveness Requirement

E must be **non-expansive**.

Formally, for all admissible inputs x:

\[
|E(x)| \le |x|
\]

This property must hold over the entire operating range
of the contraction operator.

Any operator that amplifies its input magnitude
is non-admissible as a contraction operator.

---

### 4.4 Strict Contraction Near the Origin

In a neighborhood of x = 0,
E must exhibit **strict contraction**.

Formally, there exists a neighborhood ε > 0 such that
for all x with |x| < ε and x ≠ 0:

\[
|E(x)| < |x|
\]

Strict contraction near the origin ensures
monotonic convergence under repeated application
of the control loop.

---

### 4.5 Sign Preservation

E must preserve the sign of its input.

For scalar inputs:
- if x > 0, then E(x) ≥ 0
- if x < 0, then E(x) ≤ 0

Sign inversion inside the contraction operator
is not admissible, as it may introduce oscillatory behavior
or instability in subsequent stages.

---

### 4.6 Continuity Requirement

E must be continuous over its operating range.

Discontinuities, dead zones, or piecewise jumps
are not permitted unless explicitly bounded
and justified by the operator design.

Continuity is required to preserve
predictable contraction behavior.

---

### 4.7 Boundedness Behavior

E may be bounded or unbounded.

However, any saturation or bounding behavior
must preserve the non-expansive property
and must not introduce sign inversion.

Implicit or undocumented saturation
is non-admissible.

---

### 4.8 Admissibility Summary for E

A contraction operator E is admissible within ECT-SDK
if and only if it satisfies:

- determinism,
- statelessness,
- non-expansiveness,
- strict contraction near the origin,
- sign preservation,
- continuity.

Violation of any requirement renders E non-admissible,
regardless of empirical behavior or tuning.

---

## 5. Inverse Mapping Operator (F⁻¹) Contract

### 5.1 Role of the Inverse Mapping Operator

The inverse mapping operator **F⁻¹** maps the contracted embedded value
back into the deviation domain.

Its role is to restore a deviation-compatible representation
after contraction, without introducing amplification,
memory, or additional dynamics.

F⁻¹ is not required to be an exact analytical inverse of F,
but it must preserve a consistent correspondence
within the intended operating range.

---

### 5.2 Determinism and Statelessness

The inverse mapping operator F⁻¹ must satisfy:

- deterministic behavior for identical inputs,
- complete statelessness across evaluations,
- absence of side effects or external dependencies.

F⁻¹ must not:
- store internal state,
- depend on historical values,
- introduce time-dependent behavior.

---

### 5.3 Consistency with Embedding Operator

F⁻¹ must be **consistent** with the corresponding embedding operator F.

Specifically, within the operating range,
the composition F⁻¹(F(x)) must preserve
the ordering and sign of x.

Exact equality is not required.
However, F⁻¹ must not introduce distortion
that contradicts the intended embedding semantics.

Inverse mappings that are incompatible
with their corresponding embedding operators
are non-admissible.

---

### 5.4 Monotonicity Requirement

F⁻¹ must be monotonic.

For scalar inputs y₁ and y₂:

If  
y₁ < y₂  
then  
F⁻¹(y₁) ≤ F⁻¹(y₂)

Monotonicity ensures that contraction effects
are preserved through inverse mapping.

---

### 5.5 Boundedness and Amplification Control

F⁻¹ must not amplify its input in a manner
that negates the contraction applied by E.

Specifically:
- small embedded values must not map to large deviations,
- local amplification near zero is not admissible.

Any gain introduced by F⁻¹ must be compatible
with the contraction behavior of E.

---

### 5.6 Continuity Requirement

F⁻¹ must be continuous over its operating range.

Discontinuities, dead zones, or sharp jumps
are not admissible, particularly near the origin.

Continuity is required to preserve
smooth control output behavior.

---

### 5.7 Admissibility Summary for F⁻¹

An inverse mapping operator F⁻¹ is admissible within ECT-SDK
if and only if it satisfies:

- determinism,
- statelessness,
- consistency with the embedding operator F,
- monotonicity,
- bounded amplification behavior,
- continuity.

Violation of any requirement renders F⁻¹ non-admissible,
regardless of empirical behavior or tuning.

---

## 6. Output Operator (G) Contract

### 6.1 Role of the Output Operator

The output operator **G** produces the final control command u
from the mapped deviation value produced by F⁻¹.

Its role is to:
- scale the control signal to actuator-relevant units,
- enforce explicit output bounds,
- shape the final control command without introducing dynamics.

G is the **last stage** of the FXI–Δ–E pipeline
and is the only operator explicitly responsible
for output bounding.

---

### 6.2 Determinism and Statelessness

The output operator G must satisfy:

- deterministic behavior for identical inputs,
- complete statelessness across evaluations,
- absence of side effects or external dependencies.

G must not:
- store internal memory,
- depend on historical values,
- introduce time-dependent behavior.

---

### 6.3 Bounded Output Requirement

G must enforce **explicit boundedness** of the control output u.

For all admissible inputs x:

\[
u_{\min} \le G(x) \le u_{\max}
\]

The bounds must be:
- explicitly defined,
- finite,
- invariant across evaluations.

Implicit or undocumented output bounds are non-admissible.

---

### 6.4 Monotonicity Requirement

G must be monotonic with respect to its input.

For scalar inputs x₁ and x₂:

If  
x₁ < x₂  
then  
G(x₁) ≤ G(x₂)

Monotonicity ensures that the directionality
of the control command is preserved.

---

### 6.5 Sign Preservation

G must preserve the sign of its input
within the bounded operating range.

For scalar inputs:
- if x > 0, then G(x) ≥ 0
- if x < 0, then G(x) ≤ 0

Sign inversion is not admissible,
as it may introduce unintended actuation behavior.

---

### 6.6 Continuity Requirement

G must be continuous over its operating range.

Saturation behavior at output limits is permitted,
but must be:
- continuous,
- symmetric where appropriate,
- free of discontinuous jumps.

Continuity ensures predictable actuator behavior.

---

### 6.7 No Additional Dynamics

G must not introduce:
- filtering,
- rate limiting with memory,
- integration or differentiation,
- dynamic compensation.

Any such behavior constitutes dynamic state
and is non-admissible within ECT-SDK.

---

### 6.8 Admissibility Summary for G

An output operator G is admissible within ECT-SDK
if and only if it satisfies:

- determinism,
- statelessness,
- explicit boundedness,
- monotonicity,
- sign preservation,
- continuity,
- absence of additional dynamics.

Violation of any requirement renders G non-admissible,
regardless of empirical behavior or tuning.

---

## 7. Composite Operator Validity

### 7.1 Purpose of Composite Validity

While individual operators F, E, F⁻¹, and G may be admissible in isolation,
their **composition** must also preserve the core invariants of ECT-SDK.

An admissible ECT configuration is defined not only by valid individual operators,
but by the validity of the **entire FXI–Δ–E pipeline as a composite mapping**.

---

### 7.2 Composite Determinism

The composed mapping:

\[
u = G(F^{-1}(E(F(\Delta))))
\]

must be deterministic.

This property follows from:
- deterministic behavior of each operator,
- fixed execution order,
- absence of conditional branching that depends on external state.

If any operator introduces nondeterminism,
the composite mapping is non-admissible.

---

### 7.3 Composite Statelessness

The composite pipeline must be stateless.

No operator may:
- share hidden state with other operators,
- communicate via static or global mutable data,
- introduce cross-operator memory.

Statelessness must hold both **locally** (per operator)
and **globally** (across the pipeline).

---

### 7.4 Composite Boundedness

For any finite deviation input Δ,
the composite output u must remain finite and bounded.

Boundedness must be preserved across the full pipeline.

If individual operators rely on later stages
to enforce boundedness,
such reliance must be explicit and documented.

Implicit assumptions about downstream bounding
are non-admissible.

---

### 7.5 Contraction Preservation

The contraction effect introduced by operator E
must not be negated by subsequent operators.

Specifically:
- F⁻¹ must not amplify the contracted signal
  beyond its pre-contraction magnitude,
- G must not introduce gain
  that reverses the stabilizing effect of E.

Composite amplification that nullifies contraction
renders the configuration non-admissible.

---

### 7.6 Monotonicity Preservation

The composite mapping must preserve monotonicity.

For scalar deviations Δ₁ and Δ₂:

If  
Δ₁ < Δ₂  
then  
u₁ ≤ u₂

Violation of monotonicity at the composite level
indicates incompatibility between operators,
even if individual operators are monotonic.

---

### 7.7 Responsibility for Composite Validity

Composite validity is the responsibility of:
- the system integrator,
- the operator designer configuring the pipeline.

ECT-SDK does not automatically infer or correct
invalid composite behavior.

A configuration that violates composite validity
is considered **invalid**, regardless of individual operator admissibility.

---

## 8. Implementation Notes (Non-Normative)

### 8.1 Purpose of This Section

This section provides **implementation guidance** for developers and integrators.
It is **non-normative** and does not introduce additional admissibility requirements.

Failure to follow these notes does not automatically invalidate an operator,
provided all normative contracts defined in previous sections are satisfied.

---

### 8.2 Operator Configuration Immutability

Operators should be configured prior to use
and treated as immutable during control execution.

Recommended practices include:
- configuring operator parameters at construction time,
- avoiding runtime parameter mutation during control updates,
- treating operator instances as read-only after initialization.

Immutability simplifies reasoning about determinism and statelessness.

---

### 8.3 Debug-Time Validation

Implementations may include optional validation mechanisms
enabled in debug or development builds.

Such mechanisms may include:
- assertions for NaN or infinite values,
- checks for boundedness violations,
- verification of contraction properties,
- consistency checks between F and F⁻¹.

All validation mechanisms should be disabled
in production or real-time-critical builds.

---

### 8.4 Testing Considerations

Operator testing should focus on:
- determinism under repeated identical inputs,
- preservation of monotonicity,
- boundedness of outputs,
- continuity near the origin.

Testing should avoid embedding plant models,
closed-loop dynamics, or system-level logic,
which are outside the scope of ECT-SDK.

---

### 8.5 Language and Implementation Independence

The contracts defined in this document
are independent of programming language or platform.

While the reference implementation is provided in C++,
the operator contracts may be implemented
in other languages or environments,
provided all normative requirements are satisfied.

---

### 8.6 Documentation Responsibility

Each operator implementation should be accompanied by documentation
that explicitly states:
- which contracts it satisfies,
- its intended operating range,
- any assumptions about input scaling or bounds.

Clear documentation reduces the risk
of invalid composite configurations.

---

## 9. Summary of Operator Admissibility

### 9.1 Admissibility Principle

An operator is admissible within ECT-SDK
if and only if it satisfies **all applicable contracts**
defined in this document.

Admissibility is a **binary property**.
An operator is either admissible or non-admissible;
partial compliance is not sufficient.

Empirical performance or tuning results
do not override formal admissibility requirements.

---

### 9.2 Operator Contract Summary

**Embedding Operator (F)** must satisfy:
- determinism,
- statelessness,
- monotonicity,
- continuity near the origin,
- local invertibility within the operating range.

**Contraction Operator (E)** must satisfy:
- determinism,
- statelessness,
- non-expansiveness,
- strict contraction near the origin,
- sign preservation,
- continuity.

**Inverse Mapping Operator (F⁻¹)** must satisfy:
- determinism,
- statelessness,
- consistency with F,
- monotonicity,
- bounded amplification behavior,
- continuity.

**Output Operator (G)** must satisfy:
- determinism,
- statelessness,
- explicit boundedness,
- monotonicity,
- sign preservation,
- continuity,
- absence of additional dynamics.

---

### 9.3 Composite Configuration Requirements

An ECT-SDK configuration is admissible
only if the composed FXI–Δ–E pipeline satisfies:

- determinism,
- statelessness,
- bounded output,
- contraction preservation,
- monotonicity preservation,
- bounded-time execution.

Failure at the composite level
invalidates the configuration,
even if individual operators are admissible in isolation.

---

### 9.4 Responsibility and Enforcement

Responsibility for operator admissibility
rests with the operator designer and system integrator.

ECT-SDK does not enforce admissibility automatically
and does not attempt to correct invalid configurations.

Formal operator contracts exist
to enable explicit reasoning,
auditable design,
and predictable integration behavior.

---

### 9.5 Closing Statement

This document defines the **formal admissibility boundary**
of operators within ECT-SDK.

By constraining operator behavior explicitly,
ECT-SDK preserves its identity
as a deterministic, stateless control computation core.

Any extension or integration that violates these constraints
is outside the scope of ECT-SDK
and must be treated as a separate system-level design.
