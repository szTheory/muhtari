# Phase 16 research synthesis

**Date:** 2026-11-10
**Confidence:** Medium for new toolchain integration; high for existing repository fixtures/oracles. No SDK acquired or implementation built during research.

Use a modularized `.mjs` factory with `.wasm`, a direct offline Node host, a narrow same-source native observation and required Linux artifact coverage. The two independent research reports supply official sources, immutable image digests, repository seams and concrete numeric oracles:

- [Toolchain research](16-TOOLCHAIN-RESEARCH.md): Emscripten 7.0.0, OCI index `sha256:6bd7aef93408b10bf7b36642cb7c91119b302f720b9ec2d20439f73cf07fe894`, per-platform leaves, loader/callback/memory contract and alternatives.
- [Harness research](16-HARNESS-RESEARCH.md): generated `input.a78`, independent SWCHA values, geometry/audio expectations, malformed-state refusal and restore/replay mechanisms.

No change cannot meet the requirements: existing WASI disables libretro. A static archive or standalone Wasm would shift loader/link work to integrators; the generated ES-module factory minimizes the new host surface. A complete JS port of native tests duplicates existing coverage. A native emsdk installation on every OS would require more checksum/download plumbing; the container costs about 800 MB compressed once and keeps its compiler/Node identity pinned. Prefer reuse of the existing Linux job and native outputs over duplicate job setup. These are design judgments, not measured CI improvements.

Adversarial synthesis: hash agreement alone can share a bug, so assert independent numeric expectations before parity; a native test proxy does not prove the artifact, so load emitted files directly; cached views become stale on memory growth, so reacquire views and copy synchronous callback data; an absent SDK must fail the dedicated gate rather than skip; container and native rates differ in environment, so report limits and claim no browser speed. Preserve all existing test/fault contracts and advisory distinctions.

First-build unknowns: exact generated imports/exports, adapter compilation under the pin, fixture transport, actual Node version, acquisition duration and architecture costs. Inspect and record actual outputs. Investigate failures before retry; compare the preceding stable toolchain only on concrete incompatibility, preserving integrity and evidence. No new resource budget is justified by an unmeasured first build.

Plan one vertical slice covering production artifact, first consumer, native/numeric parity, refusals, continuation, recurring integration and docs. Expected workload must be qualified before launch; stop at the standing about-one-hour local workload boundary if reached, without treating it as acceptance. Acceptance commands and receipts belong in execution summaries; no research build result is claimed.
