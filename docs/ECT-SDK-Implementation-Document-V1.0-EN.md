# ECT-SDK Implementation Document v1.0

---

# 0. Implementation Overview and Conventions

This document describes the **concrete implementation details**
of the ECT-SDK.

While the Specification defines required behavior
and the Design Document defines system structure,
this Implementation Document explains **how the SDK is implemented in code**
and which conventions govern that implementation.

---

## 0.1 Implementation Goals

The ECT-SDK implementation is designed to:

- faithfully implement the FXI–Δ–E control pipeline,
- satisfy all requirements of the Specification,
- reflect the modular structure defined in the Design Document,
- remain suitable for real-time and embedded use,
- be easy to read, audit, and extend.

Correctness, determinism, and clarity
take precedence over micro-optimizations.

---

## 0.2 Language and Environment Assumptions

The reference implementation of ECT-SDK is written in C++.

The implementation assumes:

- a standard-conforming C++ compiler,
- support for basic standard library facilities,
- no reliance on platform-specific extensions.

The SDK is designed to be portable
across desktop, embedded, and real-time environments.

---

## 0.3 Coding Conventions

The implementation follows these conventions:

- explicit and descriptive naming,
- minimal use of macros,
- avoidance of implicit conversions,
- clear separation between interface and implementation.

Control update paths avoid:

- dynamic memory allocation,
- exceptions (unless explicitly documented),
- blocking operations.

---

## 0.4 Deterministic Execution Model

Each call to the control update function:

- executes the FXI–Δ–E pipeline exactly once,
- uses only the current input values,
- produces a single control output,
- leaves no residual internal state.

Execution time is bounded
by the complexity of the configured operators.

---

## 0.5 Error Handling Strategy

Implementation-level error handling must:

- detect invalid configurations early,
- contain runtime errors within a single update call,
- propagate errors explicitly to the caller.

Error handling mechanisms are consistent
with those defined in the Specification
and described in the Design Document.

---

## 0.6 Implementation Boundaries

The implementation explicitly avoids:

- embedding scheduling or timing logic,
- managing threads or concurrency,
- enforcing safety or certification rules,
- incorporating plant-specific logic.

Such concerns belong to the integrating system.

---

## 0.7 Summary

This document provides the concrete implementation perspective
for the ECT-SDK.

It bridges the abstract requirements of the Specification
and the architectural structure of the Design Document
with real, auditable source code.

---

# 1. Controller Class Implementation

This section describes the concrete implementation
of the core controller class responsible for executing
the FXI–Δ–E control pipeline.

The controller class is the central runtime component
of the ECT-SDK.

---

## 1.1 Responsibilities of the Controller Class

The controller class is responsible for:

- storing references to configured FXI–Δ–E operators,
- enforcing the fixed execution order of operators,
- executing the control update for each deviation input,
- returning a single control output per update step.

The controller class must not:

- retain historical deviation values,
- modify operator behavior,
- perform scheduling or timing,
- allocate memory during updates.

---

## 1.2 Controller State and Members

The controller maintains only the following state:

- references or pointers to the F, E, F⁻¹, and G operators,
- configuration validity flags (if required).

No state related to previous control updates
or deviation history is stored.

---

## 1.3 Update Function Structure

The core update function follows a strict sequence:

1. validate operator configuration,
2. read the deviation input \(\delta_k\),
3. apply the embedding operator \(F\),
4. apply the contraction operator \(E\),
5. apply the inverse mapping operator \(F^{-1}\),
6. apply the output operator \(G\),
7. return the resulting control output \(u_k\).

This sequence must be executed atomically
and without reordering.

---

## 1.4 Deterministic Behavior

The update function must be deterministic.

For identical deviation inputs and identical operator configurations,
the returned control output must be identical.

The implementation must avoid:

- hidden randomness,
- timing-dependent behavior,
- reliance on external mutable state.

---

## 1.5 Error Handling in Update

If an error is detected during the update process,
the controller must:

- prevent propagation of invalid intermediate values,
- return a safe bounded output or explicit error status,
- leave the controller in a valid state for subsequent calls.

Partial execution results must not be retained.

---

## 1.6 Performance Considerations

The update function is expected to be called
inside real-time or near–real-time control loops.

The implementation must ensure:

- bounded execution time,
- no dynamic memory allocation,
- no blocking operations.

Any expensive validation or setup logic
must be performed outside the update path.

---

## 1.7 Summary

The controller class provides a minimal,
deterministic implementation of the FXI–Δ–E pipeline.

It enforces execution order, contains errors,
and produces predictable control outputs
suitable for real-time engineering systems.

---

# 2. Operator Interface and Implementations

This section describes the concrete implementation
of the FXI–Δ–E operator interfaces and their implementations.

Operators are the fundamental computational units
of the ECT-SDK and must obey strict execution rules.

---

## 2.1 Operator Interface Requirements

Each FXI–Δ–E operator (F, E, F⁻¹, G) implements
a common functional interface.

An operator must:

- accept a numeric input value,
- return a numeric output value,
- be deterministic and stateless,
- execute in bounded time.

Operators must not:

- allocate memory during execution,
- access global or static mutable state,
- depend on external timing or randomness.

---

## 2.2 Functional Operator Model

Operators are implemented as callable objects
or function-like entities.

The functional model ensures that:

- operator behavior is explicit,
- execution order is controlled by the controller,
- operators can be replaced independently.

Operators do not manage execution flow
and do not invoke other operators directly.

---

## 2.3 Embedding Operator Implementation (F)

The embedding operator implementation
maps the raw deviation into an embedded space.

Implementation guidelines:

- preserve sign where applicable,
- handle large input values gracefully,
- avoid discontinuities near the origin,
- remain invertible within the operating range.

The implementation must not introduce amplification
that could destabilize contraction.

---

## 2.4 Contraction Operator Implementation (E)

The contraction operator implementation
reduces the magnitude of its input.

Implementation guidelines:

- enforce non-expansive behavior,
- apply strict contraction near the origin,
- preserve sign when required.

Linear and nonlinear contraction implementations
must obey the same interface constraints.

---

## 2.5 Inverse Mapping Implementation (F⁻¹)

The inverse mapping implementation
transforms the contracted embedded value
back into the deviation domain.

Implementation guidelines:

- preserve monotonic correspondence with F,
- avoid numerical instability near zero,
- favor bounded approximations over exact inversion.

Exact analytical inversion is optional.

---

## 2.6 Output Operator Implementation (G)

The output operator implementation
produces the final actuator command.

Implementation guidelines:

- enforce output bounds and saturation,
- apply scaling and shaping explicitly,
- avoid introducing additional dynamics.

The output operator must not compensate
for plant dynamics or stability issues.

---

## 2.7 Error Handling within Operators

If an operator encounters invalid input values
or numerical failure conditions,
it must:

- return a well-defined failure signal, or
- produce a safe bounded output.

Operator-level failures must be detectable
by the controller and contained within the update step.

---

## 2.8 Summary

Operator implementations in ECT-SDK
are simple, deterministic, and isolated.

They implement the mathematical behavior
of the FXI–Δ–E control method
without introducing hidden state or side effects.

---

# 3. Control Update Algorithm

This section describes the concrete algorithm
executed during a single control update step.

The algorithm implements the FXI–Δ–E pipeline
exactly as specified, with no additional dynamics
or side effects.

---

## 3.1 Update Algorithm Overview

For each control update step \(k\),
the controller executes the following algorithm:

1. validate that all required operators are configured,
2. accept the deviation input \(\delta_k\),
3. apply the embedding operator \(F\),
4. apply the contraction operator \(E\),
5. apply the inverse mapping operator \(F^{-1}\),
6. apply the output operator \(G\),
7. return the control output \(u_k\).

This sequence is executed synchronously
and completes within a bounded time.

---

## 3.2 Pseudocode Representation

A conceptual pseudocode representation
of the update algorithm is shown below:

function update(delta):
assert operators_configured
x = F(delta)
xe = E(x)
d = F_inv(xe)
u = G(d)
return u


The pseudocode is illustrative and omits
error handling and boundary checks for clarity.

---

## 3.3 Atomicity of Execution

The control update algorithm must be executed atomically.

Specifically:

- no partial results may be externally visible,
- operator execution order must not be altered,
- failures must not result in partial updates.

Atomicity ensures predictable control behavior.

---

## 3.4 Error Detection and Handling

During execution, the controller must detect:

- missing or invalid operator configurations,
- invalid numeric values produced by operators,
- operator execution failures.

Upon detection, the algorithm must:

- abort the current update step,
- return a safe bounded output or error status,
- leave the controller in a valid state.

---

## 3.5 Determinism and Repeatability

The update algorithm must be deterministic.

Given identical deviation inputs
and identical operator configurations,
the algorithm must produce identical outputs.

The implementation must not depend on:

- timing variations,
- execution history,
- external mutable state.

---

## 3.6 Performance Characteristics

The computational complexity of the update algorithm
is linear in the number of operator evaluations.

The algorithm must:

- execute in bounded time,
- avoid dynamic memory allocation,
- be suitable for high-frequency update loops.

---

## 3.7 Summary

The control update algorithm implements
the FXI–Δ–E pipeline as a simple, atomic,
and deterministic computation.

It forms the core runtime behavior
of the ECT-SDK.

---

# 4. Integration Patterns

This section describes recommended integration patterns
for embedding the ECT-SDK into real-world systems.

The patterns are illustrative and non-prescriptive,
intended to guide correct and safe usage.

---

## 4.1 Generic Control Loop Integration

The most common integration pattern
is a synchronous control loop:

1. read sensor data,
2. compute deviation \(\delta_k\),
3. call the SDK update function,
4. apply the returned control output \(u_k\) to actuators.

The SDK must be invoked exactly once per control cycle.
Timing, scheduling, and synchronization
are managed by the integrating system.

---

## 4.2 Integration in Embedded Systems

In embedded environments:

- the SDK is typically called from a timer interrupt
  or real-time task,
- operator configuration is performed during initialization,
- control updates occur without dynamic allocation.

Care must be taken to ensure
that the update execution time
fits within the real-time budget.

---

## 4.3 PX4 Integration Pattern

When integrated with PX4-based systems,
the SDK is typically used inside a flight-control module
or work queue callback.

Recommended pattern:

- deviation computed from setpoint and state estimate,
- SDK update called once per control cycle,
- output mapped to actuator commands.

The SDK does not interact with PX4 scheduling,
uORB messaging, or parameter systems directly.

---

## 4.4 ROS2 Integration Pattern

In ROS2-based systems,
the SDK is typically invoked from:

- a timer callback, or
- a subscription callback.

Deviation is computed from incoming messages,
and the SDK output is published to an actuator or control topic.

The SDK itself remains independent of ROS2 APIs
and does not manage node lifecycle or threading.

---

## 4.5 Multi-Loop and Supervisory Integration

ECT-SDK may be used:

- as an inner-loop controller,
- within a supervisory or mode-based system,
- alongside other control methods.

In such cases, higher-level logic
selects when and how the SDK is invoked.

---

## 4.6 Integration Responsibilities

The integrating system is responsible for:

- correct scheduling and timing,
- sensor validation,
- actuator interfacing,
- safety and fault handling.

The SDK provides control computation only.

---

## 4.7 Summary

ECT-SDK integrates cleanly into a variety
of control environments.

It imposes minimal assumptions on the host system
and relies on explicit, synchronous invocation
for predictable behavior.

---

# 5. Performance, Testing, and Validation

This section describes performance expectations,
testing strategies, and validation principles
for the ECT-SDK implementation.

These considerations ensure that the SDK behaves
predictably under real-world conditions.

---

## 5.1 Performance Characteristics

The ECT-SDK is designed for use in
real-time and near–real-time control loops.

Performance characteristics include:

- bounded execution time per update,
- constant memory usage during updates,
- linear computational complexity
  with respect to operator evaluations.

Actual performance depends on the complexity
of configured operator implementations.

---

## 5.2 Real-Time Suitability

To remain suitable for real-time environments,
the implementation must:

- avoid dynamic memory allocation during updates,
- avoid blocking calls,
- avoid unbounded loops or recursion.

Operator configuration and validation
must occur outside the control loop.

---

## 5.3 Unit Testing

Unit tests should verify:

- correct execution order of FXI–Δ–E operators,
- deterministic behavior for identical inputs,
- bounded outputs for bounded inputs,
- correct handling of invalid numeric values.

Each operator should be testable in isolation.

---

## 5.4 Integration Testing

Integration tests should validate:

- correct interaction between controller and operators,
- correct behavior under realistic control loop timing,
- compatibility with target environments
  (e.g., embedded, PX4, ROS2).

Integration tests must not rely
on specific plant dynamics.

---

## 5.5 Stress and Boundary Testing

The implementation should be tested under:

- extreme deviation values,
- noisy inputs,
- saturation conditions,
- repeated error scenarios.

The SDK must remain deterministic
and must not degrade over time.

---

## 5.6 Validation Against Specification

Validation consists of confirming that:

- all requirements of the Specification are met,
- all design constraints are respected,
- all documented invariants hold in practice.

Validation does not imply safety certification
or system-level correctness.

---

## 5.7 Summary

ECT-SDK performance and validation focus on
predictability, determinism, and robustness.

Testing and validation ensure that the implementation
faithfully realizes the FXI–Δ–E control method
and remains suitable for engineering use.
