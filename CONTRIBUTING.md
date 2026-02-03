# Contributing to ECT-SDK

Thank you for your interest in contributing to **ECT-SDK**.

ECT-SDK is a **reference implementation** of the FXI–Δ–E control architecture.
Its primary goal is architectural correctness and formal clarity,
not feature growth or broad extensibility.

Please read this document carefully before submitting contributions.

---

## Scope of Contributions

Contributions are welcome **only if they preserve the core architecture**.

Acceptable contributions include:
- documentation improvements and clarifications
- bug fixes that preserve existing behavior
- refactoring without semantic changes
- additional examples or deterministic tests
- tooling improvements (build, CI, formatting)

Contributions **must not**:
- alter the FXI–Δ–E pipeline structure
- introduce state, memory, or dynamics
- add system-level logic (scheduling, safety, modes)
- change operator semantics outside documented versions
- turn ECT-SDK into a control system or framework

If a contribution changes *what ECT is*, it is out of scope.

---

## Architectural Constraints

All code contributions must comply with:
- deterministic execution
- stateless operator behavior
- bounded output guarantees
- fixed operator execution order

Operator implementations must conform to the contracts defined in:

**ECT-SDK — Operator Formalization v1.1**

Contributions that violate these constraints will not be accepted,
regardless of functionality or performance.

---

## How to Contribute

1. Open an issue describing the proposed change.
2. Clearly state:
   - what the change does
   - why it is needed
   - why it does not violate architectural constraints
3. Submit a pull request referencing the issue.

Pull requests without a clear architectural justification
may be closed without review.

---

## Review Process

All contributions are reviewed by the project maintainer.

Acceptance criteria are based on:
- architectural consistency
- formal correctness
- clarity and maintainability
- alignment with the project roadmap

There is no voting or committee process.

---

## Code Style and Quality

- Keep changes minimal and focused.
- Avoid introducing unnecessary abstractions.
- Prefer explicit, readable code over clever optimizations.
- Do not introduce dependencies without prior discussion.

---

## Final Note

ECT-SDK is intentionally minimal.

If your goal is to build:
- a full control system
- an adaptive or learning controller
- a production-ready framework

ECT-SDK is **not** the right place for that work.

Such projects are encouraged to live as separate implementations
built on top of the ECT principles.

Thank you for respecting the boundaries of the project.
