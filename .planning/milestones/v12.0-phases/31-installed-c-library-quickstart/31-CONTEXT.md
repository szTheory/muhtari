# Phase 31: Installed C library quickstart - Context

**Gathered:** 2026-11-19
**Status:** Ready for planning

<domain>
## Phase Boundary

Deliver a complete, ROM-free C quickstart for a host that consumes Muhtari's installed release package, with accurate package instructions and recurring checks against the extracted archive. This phase changes the example, user documentation and package-consumer verification; it does not change the public API/ABI, emulator behavior, host integration, release format, or gameplay compatibility.

</domain>

<decisions>
## Implementation Decisions

### Example shape and behavior
- **D-01:** Add one small standalone C quickstart and use that same source in user documentation and package-consumer checks. Base it on the already tested no-content one-frame path rather than the larger multi-mode test consumer.
- **D-02:** The example will provide the required allocator callbacks, initialize size-tagged structs, query `muhtari_get_info`, size video/audio buffers from that result, run one frame, and check the successful status plus the known 800-sample result. It must not require a ROM, BIOS, save, or other media.

### Package proof and limits
- **D-03:** Build and run the shared source against the actual extracted release archive through CMake on all six existing native package lanes. Also compile and run it through pkg-config against that archive on one Ubuntu 26.04 lane, with a clear failure if pkg-config is missing; run one local Apple Silicon archive check. Keep Windows on its proven CMake route until a Windows pkg-config toolchain is demonstrated.
- **D-04:** Add a representative negative control that damages a copied package's exported CMake target and requires the expected consumer failure using the existing strict expected-failure helper. Preserve existing positive checks and fault coverage.
- **D-05:** Keep the README concise and point the C host quickstart to the example and the complete `docs/embedding.md` API guide. Explain the archive/tool requirements and that the result proves package discovery, linkage, instance setup, buffers and one no-content frame only; it does not establish game compatibility.

### the agent's Discretion
- Choose the exact example filename, output wording, and the least disruptive existing CMake/CTest integration points, provided the same source is exercised by both routes and the existing CI package coverage remains intact.
- Keep the pkg-config check in the existing Ubuntu lane identified by research; do not add a package dependency or create a Windows pkg-config promise without toolchain evidence.
- Autonomous recommendation adopted under the owner's standing `$gsd-autonomous` authorization. No direct owner selection was requested because these choices remain within the approved phase scope.

</decisions>

<canonical_refs>
## Canonical References

**Downstream agents MUST read these before planning or implementing.**

### Scope, requirements and project constraints
- `.planning/ROADMAP.md` §Phase 31 — goal and four acceptance criteria.
- `.planning/REQUIREMENTS.md` — EMBED-01 package-consumer requirement.
- `.planning/PROJECT.md` — three deliverables and deterministic, bounded core constraints.
- `.planning/INTENT.md` — current consumer jobs and autonomous-work preferences.
- `.planning/research/fan-out/prompt.txt` — autonomous recommendation and evidence rules.
- `.planning/preparation/README.md` — index for sourced hardware and engineering references.
- `.planning/preparation/DECISIONS.md` — settled package and compatibility decisions.
- `.planning/preparation/ENGINEERING.md` §11 — architecture and resource-bound review method.
- `.planning/LESSONS.md` — installed-consumer, package, and CI evidence lessons.
- `.planning/phases/31-installed-c-library-quickstart/31-RESEARCH.md` — package route evidence, runner inventory source, limits and revisit triggers.
- `.planning/phases/31-installed-c-library-quickstart/31-PATTERNS.md` — current consumer and negative-control patterns.

### Public API and user guidance
- `docs/embedding.md` — archive contents, CMake/pkg-config routes, allocator contract, buffers, frame call and behavior limits.
- `include/muhtari.h` — public status, allocator, info, frame and lifecycle declarations.
- `README.md` — current short user entry points and embedding-guide link.

### Package and consumer checks
- `cmake/muhtariConfig.cmake.in` — relocatable CMake imported target.
- `cmake/muhtari.pc.in` — relocatable pkg-config flags.
- `tests/consumer/main.c` — existing no-content one-frame result and allocator/buffer setup.
- `tests/consumer/CMakeLists.txt` — outside-consumer CMake package pattern.
- `tests/install/tests.cmake` — staged installed-package integration checks.
- `tests/release/ym_consumers.cmake` — extracted release archive and consumer pattern.
- `tests/release/tests.cmake` — release consumer registration.
- `tests/cmake/expect_fail.cmake` — exact exit and diagnostic matching for negative controls.
- `.github/workflows/ci.yml` — existing Ubuntu 26.04 native lane and package coverage.

### External environment evidence
- [Official Ubuntu 26.04 hosted runner image inventory](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2604-Readme.md) — research records image version `20261111.196.1` and pkg-config availability; the repository's hosted run must still establish runtime availability.

</canonical_refs>

<code_context>
## Existing Code Insights

### Reusable Assets
- `tests/consumer/main.c` already implements the no-content allocator, info-sized buffers, frame call, 800-sample check and recognizable success output.
- `tests/install/tests.cmake` already stages and consumes the library package outside the source tree.
- `tests/cmake/expect_fail.cmake` already provides strict expected-exit and diagnostic matching.

### Established Patterns
- Package checks should consume a staged or extracted prefix so they test installed metadata rather than the build tree.
- Keep positive and negative controls paired; the negative case must fail for the deliberately damaged package target, not for an unrelated setup error.
- Reuse the exact quickstart source for documentation and both package routes to prevent example/test drift.

### Integration Points
- Install/archive consumer checks are registered under the existing test surfaces and run within native CI package lanes.
- `README.md` links to `docs/embedding.md`, which is the complete current embedding/API reference.
- `muhtari.pc` is already installed in the release archive with paths relative to the `.pc` file.

</code_context>

<specifics>
## Specific Ideas

Use a minimal no-content first frame so developers can prove discovery, linkage and buffer setup before introducing a game image. The documented expected result is the tested 800 audio samples on that frame. Link this source as the quickstart and link the full embedding guide for the complete API contract.

</specifics>

<deferred>
## Deferred Ideas

No additional ideas were folded into Phase 31.

### Reviewed Todos (not folded)
- `2026-10-19-close-the-phase-2-review-seeds-on-the-nightly-filter-and-the-rom-tables.md` — concerns historical CI filtering and ROM tables; neither is needed to prove the installed C package route.
- `2026-10-16-promote-the-wasm32-lane-into-ci-required.md` — changes the broader required CI platform matrix, outside this native C package quickstart.
- `2026-10-29-keep-the-phase-11-first-review-info-findings.md` — preserves unrelated historical review findings; it does not affect this example or package contract.
- `2026-10-16-close-the-re-review-findings-on-the-dependabot-gate-and-the-publish-checks.md` — addresses dependency and publishing controls, not installed-consumer behavior.
- `2026-10-19-close-the-phase-2-re-review-findings-when-maria-and-the-run-loop-land.md` — concerns MARIA/run-loop correctness and is outside this documentation/package phase.

</deferred>

---

*Phase: 31-installed-c-library-quickstart*
*Context gathered: 2026-11-19*
