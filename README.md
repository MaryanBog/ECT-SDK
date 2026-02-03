# ECT-SDK — Engineering Control Core

ECT-SDK is a reference C++ implementation of the FXI–Δ–E deviation-based control architecture
defined in Engineering Control Theory: The FXI–Δ–E Control Architecture.

The SDK implements a deterministic, stateless control computation pipeline
based on deviation embedding, contraction, inverse mapping, and bounded output shaping.

NOTE:
ECT-SDK is provided as a reference implementation intended for verification,
reproducibility, and architectural evaluation.
It is not a production-ready control system and does not include
safety, scheduling, redundancy, or system-level integration logic.

## Overview

ECT-SDK implements a strict control computation core with the following properties:
- deterministic execution
- stateless control logic
- bounded output behavior
- fixed operator execution order
- no plant model or internal dynamics

The SDK is designed to demonstrate applicability to real-world engineering scenarios
while maintaining explicit architectural boundaries.

ECT-SDK is not a control system, framework, or flight controller.
It is a reusable control core intended to be integrated into larger systems
by experienced engineers.

## Control Architecture

The ECT control computation follows a fixed single-step pipeline:

u = G(F⁻¹(E(F(Δ))))

Where:
- Δ — engineering deviation input
- F — deviation embedding operator
- E — contraction (stabilization) operator
- F⁻¹ — inverse mapping operator
- G — bounded output operator

Each operator is deterministic, stateless,
and executed exactly once per update step.

No hidden feedback, integration, or memory is present in the pipeline.

## Project Structure

ECT-SDK/
include/
src/
examples/
tests/
docs/
CMakeLists.txt
README.md

## Build and Run

Build:
mkdir build
cd build
cmake ..
make

Run examples:
./simple_loop_test
./servo_position_control
./drone_altitude_control

The example outputs demonstrate monotonic deviation contraction,
bounded control output, and stable convergence behavior.

## Intended Use

ECT-SDK is intended for:
- architectural evaluation
- research and experimentation
- reproducibility of the FXI–Δ–E theory
- integration into custom control systems by experts

ECT-SDK is not intended for:
- direct deployment in safety-critical systems
- use as a complete control solution
- replacement of system-level controllers
- autonomous decision-making or learning control

## Operator Constraints

All operators used in ECT-SDK must satisfy strict behavioral contracts
defined in the document:

ECT-SDK — Operator Formalization v1.1

These contracts enforce determinism, statelessness, boundedness,
monotonicity, and contraction preservation.

Operators that violate these constraints are considered non-admissible
within the ECT architecture.

## Status

Architecture: frozen
SDK: stable reference implementation
Extensions: limited to formally defined versions (v1.1, v1.2)

The core design is intentionally minimal and closed to ad-hoc feature expansion.

## License

Apache License 2.0

See the LICENSE file for details.

## Citation

If you use ECT-SDK in academic or technical work, please cite:

Bogdanov, M.
ECT-SDK v1.0.0 — Reference Implementation of the FXI–Δ–E Control Architecture