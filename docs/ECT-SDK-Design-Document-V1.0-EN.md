# ECT-SDK Design Document V1.0

---

# 0. Architecture Overview and Design Goals

This document defines the **design and internal structure** of the ECT-SDK.

While the Specification defines what the system must do,
this Design Document describes **how the system is organized**
to satisfy those requirements.

---

## 0.1 Design Goals

The ECT-SDK is designed to achieve the following goals:

- clear separation between control logic and system integration,
- deterministic and stateless execution,
- minimal runtime overhead,
- suitability for real-time and embedded environments,
- modular replacement of FXI–Δ–E operators.

The design prioritizes simplicity, transparency,
and ease of integration over feature richness.

---

## 0.2 High-Level Architecture

At a high level, the ECT-SDK consists of:

- a **core controller** responsible for executing the FXI–Δ–E pipeline,
- a set of **operator interfaces** defining F, E, F⁻¹, and G,
- optional **default operator implementations**,
- a minimal **public API** for configuration and execution.

The SDK does not include scheduling, threading,
or platform-specific runtime logic.

---

## 0.3 Core Control Flow

The core control flow follows a fixed sequence:

1. receive deviation input \(\delta_k\),
2. apply embedding operator \(F\),
3. apply contraction operator \(E\),
4. apply inverse mapping operator \(F^{-1}\),
5. apply output operator \(G\),
6. return control output \(u_k\).

This sequence is executed atomically per update step
and must not be reordered or partially executed.

---

## 0.4 Module Boundaries

The SDK is organized into the following conceptual modules:

- **Controller Module**  
  Orchestrates execution of the FXI–Δ–E pipeline.

- **Operator Modules**  
  Encapsulate the behavior of F, E, F⁻¹, and G independently.

- **Configuration Interface**  
  Allows operator assignment and replacement.

Each module has a single responsibility
and does not depend on the internal state of other modules.

---

## 0.5 Data Flow and Ownership

All data passed through the control pipeline:

- is owned by the calling code,
- is provided explicitly as function arguments,
- is not retained between update calls.

The SDK does not allocate, cache,
or persist control-related data internally.

---

## 0.6 Design Constraints

The design explicitly avoids:

- dynamic memory allocation during updates,
- hidden internal state,
- implicit operator coupling,
- background execution threads.

These constraints ensure predictability
and real-time compatibility.

---

## 0.7 Summary

The ECT-SDK design defines a compact,
modular, and deterministic control architecture.

Its structure directly reflects the FXI–Δ–E control method
and enables safe, transparent integration
into engineering systems.

---

# 1. Module Structure and Responsibilities

This section describes the internal module structure of the ECT-SDK
and defines the responsibilities and boundaries of each module.

The design follows strict separation of concerns
to preserve determinism, clarity, and ease of integration.

---

## 1.1 Controller Module

The **Controller Module** is the central execution unit of the SDK.

Responsibilities:

- orchestrate execution of the FXI–Δ–E pipeline,
- enforce the fixed operator execution order,
- expose the primary update interface,
- return a single control output per update step.

The controller module must not:

- store historical deviation values,
- modify operator behavior,
- perform scheduling or timing.

---

## 1.2 Operator Modules

Each FXI–Δ–E operator is implemented as an independent module:

- **F Operator Module** — deviation embedding,
- **E Operator Module** — contraction,
- **F⁻¹ Operator Module** — inverse mapping,
- **G Operator Module** — output shaping.

Responsibilities:

- implement a deterministic, stateless function,
- operate only on provided input values,
- avoid side effects or shared state.

Operator modules must not depend on each other directly.

---

## 1.3 Operator Interface Layer

The operator interface layer defines a common contract
for all FXI–Δ–E operators.

This layer ensures that:

- operators are interchangeable,
- custom operator implementations are supported,
- the controller can invoke operators uniformly.

The interface layer does not impose
specific mathematical behavior beyond execution constraints.

---

## 1.4 Configuration Module

The **Configuration Module** allows the integrating system
to assign or replace operator implementations.

Responsibilities:

- register operator instances,
- validate operator presence,
- expose configuration functions.

The configuration module must not:

- alter operator execution order,
- modify operator outputs,
- introduce implicit defaults without explicit assignment.

---

## 1.5 Default Operator Implementations

The SDK may provide optional default operators
for basic functionality and testing.

Default operators:

- must conform to all operator interface requirements,
- must not assume specific plant dynamics,
- must be clearly documented as non-optimal baselines.

Use of default operators is optional
and not required for compliance.

---

## 1.6 Module Interaction Rules

Modules interact strictly through defined interfaces.

The design prohibits:

- direct access to internal module state,
- implicit data sharing,
- cross-module side effects.

All interactions are explicit and unidirectional.

---

## 1.7 Summary

The ECT-SDK module structure enforces:

- clear responsibility boundaries,
- strict operator isolation,
- deterministic execution flow.

This modular design enables safe customization
and predictable control behavior.

---

# 2. Public API Design

This section defines the design principles and structure
of the public API exposed by the ECT-SDK.

The API is designed to be minimal, explicit, and stable,
supporting deterministic use in real-time control loops.

---

## 2.1 API Design Principles

The public API adheres to the following principles:

- minimal surface area,
- explicit configuration,
- deterministic execution,
- no hidden side effects,
- compatibility with embedded and real-time systems.

The API must be usable without dynamic memory allocation
during control updates.

---

## 2.2 Controller Interface

The core API exposes a controller object
representing a single FXI–Δ–E control instance.

The controller interface must provide:

- a method to submit a deviation input,
- a method to retrieve the corresponding control output,
- configuration methods for assigning operators.

Each controller instance is independent
and does not share state with other instances.

---

## 2.3 Update Method Semantics

The update method:

- accepts a deviation value \(\delta_k\),
- executes the FXI–Δ–E pipeline exactly once,
- returns a single control output \(u_k\).

The update method must:

- be synchronous,
- be reentrant,
- complete in bounded time.

The method must not block, sleep,
or perform asynchronous operations.

---

## 2.4 Operator Configuration Interface

The API must expose explicit methods
to assign or replace FXI–Δ–E operators.

Configuration operations:

- must occur outside the control update loop,
- must be explicit and visible to the caller,
- must not implicitly modify other operators.

The API must not silently substitute missing operators.

---

## 2.5 Error Reporting Strategy

The API must provide a clear mechanism
for reporting configuration or execution errors.

Possible strategies include:

- return status codes,
- exceptions (where appropriate),
- explicit error callbacks.

The chosen strategy must be documented
and consistent across the API.

---

## 2.6 Threading and Concurrency Considerations

The SDK does not manage threading.

The public API must:

- be safe for single-threaded use by default,
- clearly document any thread-safety guarantees,
- avoid internal synchronization primitives.

Concurrent use of the same controller instance
must be explicitly documented as unsupported
unless stated otherwise.

---

## 2.7 API Stability and Versioning

The public API is versioned
and must remain stable within a major version.

Breaking changes require a major version increment.
Minor and patch versions may add functionality
without altering existing behavior.

---

## 2.8 Summary

The ECT-SDK public API is intentionally minimal
and designed for predictable, real-time use.

It exposes only what is necessary
to configure and execute the FXI–Δ–E control loop.

---

# 3. Data Types and Memory Model

This section defines the data model used by the ECT-SDK,
including expected numeric types, ownership rules,
and memory constraints required for real-time operation.

---

## 3.1 Numeric Representation

ECT-SDK operates on finite real-valued signals.

The SDK must support at least one floating-point representation
suitable for control applications (e.g., IEEE-754 `float` or `double`).

Implementations may additionally support fixed-point arithmetic,
but such support is optional and must be documented.

---

## 3.2 Scalar and Vector Deviations

The SDK may support:

- scalar deviations (\(n = 1\)),
- vector deviations (\(n > 1\)).

If vector deviations are supported, the SDK must define:

- vector dimension handling,
- component-wise operator semantics,
- norm conventions (if norms are used).

Where undefined, operators are interpreted component-wise.

---

## 3.3 Operator Input/Output Types

Each FXI–Δ–E operator (F, E, F⁻¹, G) must accept and return
the same numeric type as the deviation representation.

Operators must not:

- mutate input values,
- retain references to input buffers,
- allocate memory during evaluation.

---

## 3.4 Ownership and Lifetime Rules

All values passed into the SDK:

- are owned by the caller,
- are passed by value or by const reference,
- are not stored by the SDK after the update call completes.

The SDK must not maintain hidden copies of deviation histories.

---

## 3.5 Real-Time Memory Constraints

During control updates, the SDK must:

- avoid dynamic allocations,
- avoid unbounded growth of internal structures,
- avoid recursion or unbounded loops.

Any allocations required for configuration
must be performed outside the control loop.

---

## 3.6 Error Value Handling

The SDK must define behavior when encountering invalid numeric values,
including:

- NaN (Not-a-Number),
- infinities,
- overflowed or undefined intermediate results.

A compliant implementation must either:

- return a safe bounded output and signal an error, or
- reject the update step with an explicit error status.

Silent propagation of invalid values is not permitted.

---

## 3.7 Summary

ECT-SDK uses simple, explicit numeric data types
and a strict real-time memory model.

Ownership remains with the caller,
and all control updates execute without allocation,
history storage, or hidden state.

---

# 4. Default Operators and Customization Points

This section describes the default FXI–Δ–E operator implementations
provided by the ECT-SDK and the intended customization points
for application-specific control behavior.

---

## 4.1 Purpose of Default Operators

Default operators are provided to:

- enable immediate use of the SDK,
- serve as a safe baseline configuration,
- demonstrate correct operator semantics,
- support testing and validation.

Default operators are not intended
to provide optimal performance for all systems.

---

## 4.2 Default Embedding Operator (F)

The default embedding operator \(F\) implements
a simple, monotonic mapping of deviation values.

Typical characteristics:

- identity or smooth saturation behavior,
- continuity across the operating range,
- invertibility near the origin.

The default \(F\) does not assume
specific plant dynamics or constraints.

---

## 4.3 Default Contraction Operator (E)

The default contraction operator \(E\) implements
a linear contraction:

\[
E(x) = kx, \quad 0 < k < 1.
\]

This operator guarantees deviation reduction
under repeated application
and serves as a conservative stabilization baseline.

---

## 4.4 Default Inverse Mapping Operator (F⁻¹)

The default inverse mapping operator approximates
the inverse of the default embedding operator.

Exact inversion is not required;
monotonicity and boundedness take precedence.

The default inverse is designed
to remain well-behaved near the origin.

---

## 4.5 Default Output Operator (G)

The default output operator \(G\) applies
linear scaling and optional saturation
to produce actuator-compatible commands.

The default \(G\) ensures:

- bounded outputs,
- predictable scaling,
- compatibility with typical actuators.

---

## 4.6 Custom Operator Replacement

The SDK explicitly supports replacement
of any FXI–Δ–E operator.

Custom operators:

- must satisfy operator interface requirements,
- must be deterministic and stateless,
- must not rely on external mutable state.

Operator replacement must be explicit
and occur outside the control update loop.

---

## 4.7 Customization Boundaries

Customization is permitted only
at defined operator boundaries.

The SDK design prohibits:

- modification of control flow order,
- insertion of hidden logic between operators,
- coupling between operator implementations.

These boundaries preserve predictability
and compliance with the specification.

---

## 4.8 Summary

ECT-SDK provides conservative default operators
and well-defined customization points.

This design enables safe experimentation
while preserving deterministic control behavior.

---

# 5. Error Handling and Failure Modes

This section defines how the ECT-SDK handles errors,
invalid configurations, and runtime failure conditions.

The design goal is to ensure **predictable, contained behavior**
under all circumstances, including misconfiguration
and unexpected numeric conditions.

---

## 5.1 Configuration Errors

Configuration errors occur when required FXI–Δ–E operators
are missing, invalid, or incompatible.

The SDK must detect the following conditions:

- missing operator assignments,
- null or uninitialized operator references,
- incompatible operator signatures.

Upon detection, the SDK must:

- reject execution of the control update, and
- signal an explicit configuration error to the caller.

Silent fallback to implicit defaults is not permitted.

---

## 5.2 Runtime Execution Errors

Runtime errors may occur during operator evaluation,
including:

- numeric overflow,
- invalid intermediate values,
- operator exceptions or failures.

The SDK must ensure that such errors:

- do not corrupt internal state,
- do not affect subsequent update calls,
- are contained within the current update step.

---

## 5.3 Invalid Numeric Values

If an operator produces invalid numeric values
(e.g., NaN or infinity), the SDK must:

- prevent propagation of invalid values, and
- return either:
  - a safe bounded control output, or
  - an explicit execution error status.

The chosen behavior must be documented
and consistent across the implementation.

---

## 5.4 Error Reporting Mechanisms

The SDK must provide a clear and explicit mechanism
for reporting errors to the integrating system.

Acceptable mechanisms include:

- return status codes,
- exception signaling (where appropriate),
- explicit error callbacks.

Error reporting must not rely on logging alone.

---

## 5.5 Failure Containment

Failure in one control update step
must not compromise future updates.

Specifically:

- no partial state must be retained,
- operator failures must not cascade,
- recovery must be possible without reinitialization.

This property is essential for long-running systems.

---

## 5.6 Behavior Under Persistent Errors

If errors persist across multiple update steps,
the SDK must continue to behave deterministically.

The SDK must not:

- accumulate error-related state,
- escalate control output,
- enter undefined execution modes.

Persistent error handling strategies
are the responsibility of the integrating system.

---

## 5.7 Summary

ECT-SDK error handling is explicit, contained,
and deterministic.

Errors are detected early, reported clearly,
and prevented from propagating beyond
a single control update step.
