# Governance

## Purpose

This document defines the governance model for the **ECT-SDK** project.

ECT-SDK is an architectural reference implementation of the
FXI–Δ–E deviation-based control core.
Its primary value lies in architectural correctness and formal constraints,
not in feature velocity or broad extensibility.

The purpose of this governance model is to preserve
the architectural integrity of ECT-SDK.

---

## Maintainer

The project is maintained by a single maintainer:

**Maryan Bogdanov**  
Author and Architect of Engineering Control Theory (ECT)

The maintainer is responsible for:
- defining and approving the project roadmap
- accepting or rejecting contributions
- releasing official versions
- maintaining architectural consistency

---

## Architectural Stability

The core architecture of ECT-SDK is considered **stable and frozen**.

This includes, but is not limited to:
- the FXI–Δ–E control pipeline
- the stateless and deterministic execution model
- the role and ordering of operators (F, E, F⁻¹, G)

Changes that modify or extend the core architecture
are **out of scope** for regular contributions.

Architectural changes may only occur through
explicitly documented version milestones
(e.g., v1.1, v1.2) defined by the maintainer.

---

## Contributions

Contributions are welcome if they meet the following criteria:

- do not alter the core control architecture
- do not introduce state, dynamics, or system-level logic
- comply with all formal operator contracts
- improve clarity, correctness, or reproducibility

Examples of acceptable contributions include:
- documentation improvements
- bug fixes that preserve existing behavior
- additional examples or tests
- refactoring without semantic changes

The maintainer reserves the right to reject contributions
that conflict with the architectural principles of ECT-SDK,
regardless of functionality or performance improvements.

---

## Decision Process

All decisions regarding:
- pull requests
- issues
- roadmap changes
- releases

are made by the maintainer.

There is no voting process or committee structure.
This governance model is intentionally simple
to ensure architectural coherence.

---

## Scope of the Project

ECT-SDK is a **reference implementation**, not a production control system.

The project explicitly does not aim to:
- provide a complete control solution
- support all platforms or use cases
- incorporate safety, scheduling, or redundancy mechanisms
- evolve into a general-purpose control framework

Any work outside this scope is considered a separate project.

---

## Final Note

This governance model exists to ensure that ECT-SDK
remains a clear, minimal, and formally constrained control core.

Contributions that respect these boundaries are encouraged.
Contributions that attempt to expand or redefine them are not.
