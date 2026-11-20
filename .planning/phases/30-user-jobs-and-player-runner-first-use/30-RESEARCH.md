# Phase 30: User jobs and player/runner first use - Research

**Researched:** 2026-11-19  
**Domain:** User documentation, libretro packaging, runner CLI, product evidence  
**Confidence:** HIGH for repository contracts; MEDIUM for persona needs and host-menu details

<user_constraints>
## User Constraints (from CONTEXT.md)

### Locked Decisions

<!-- DATA_7b19e4a2_START -->
### Player path in RetroArch
- Start from the current stable release package. Choose the native core archive for the operating system and the architecture of the running RetroArch process, then place the core module and its `.info` file in RetroArch's configured Cores and Core Info directories. The published v0.18.0 package is the current baseline; recheck the latest release when implementing.
- Keep the existing macOS Apple Silicon install command and its CI-consumed text in place. Follow it with the GUI path to load the Muhtari core and then a lawfully obtained `.a78` or headerless `.bin` through Load Content. Keep Start Core as the no-content test-card check.
- Describe the test card as the expected result of starting the core without content. For supported content, say the cartridge's own startup screen appears; do not promise one universal first screen or that every recognized cartridge is playable.
- Fast boot is the default and needs no BIOS. Link the existing BIOS reference for users choosing BIOS boot. Show the existing RetroPad mapping, including Start for console Pause and X for console Reset. Do not promise fixed keyboard keys because RetroArch keyboard bindings vary by installation.
- State compatibility limits as evidence limits: the pinned list has 201 unevaluated titles and 3 skipped titles; untested is not a failure report, and Muhtari makes no universal title-compatibility claim.

### Runner first use and refusal guidance
- Keep the built-in test-card command `bin/muhtari-run --frames 1 --hash-frame 1` and its pinned frame digest as the known first check.
- Give a separate own-image path: optionally run `bin/muhtari-run --probe "game.a78"`, then run a finite frame sample such as `bin/muhtari-run --frames 120 --hash-frame 120 --rom "game.a78"`. Show the package's Windows executable spelling where useful.
- Explain that `--probe` identifies an image but does not establish that its board, devices, or sound configuration can run. A successful run prints a content-dependent frame hash. Unsupported content or configurations return 77; malformed/unreadable input or another run failure returns 1; invalid command usage returns 2. Link the full runner reference for details.
- Keep Atari 2600 as an explicit refusal and sibling-ownership boundary. Replace the runner's legacy `unsupported: 2600 (planned)` text with a neutral unsupported message, and update its existing output assertion and documentation. This changes wording only; refusal status and emulation behavior stay the same.

### Evidence-backed user jobs
- Refresh the existing persona/JTBD section rather than creating a second persona document. Make the current jobs concrete and map each to a shipped deliverable: player → libretro core; runner/homebrew developer → `muhtari-run`; C host integrator → C library and installed package route.
- For each job, record the evidence date and class, source/revision, confidence, sibling ownership, and a revisit trigger. Mark runner and C-integrator need as inference from the shipped interfaces and existing research; do not imply interviews, adoption, or demand validation.
- Preserve the owner feedback as two title reports from one older local onboarding session, not two independent users: Choplifter has the more specific GUI-load and casual-play observation; Ninja Golf has only a brief positive report. Neither establishes general compatibility or representative demand.
- Keep future speedrunner, TAS, AI, and other frontend needs directional and evidence-labeled. Keep LibFuturo, Playstead, and `twentysixhundo` ownership explicit.

### Entry-document accuracy
- Remove or repair the stale Phase 25 paths in README and `docs/status.md`; point to the current release and valid v11.0 archive evidence where a release receipt is useful. Keep the status counts synchronized with the current compatibility ledger.
- Correct first-use claims surfaced in the installed core information and linked reference docs: distinguish supported Souper mapper behavior from unavailable BupChip audio, and qualify the BankSet refusal example so it does not imply the whole mapper is unsupported.
- Treat corrections to the packaged `.info` description or runner output as release-package changes. Use the normal patch-release route under the settled pre-1.0 version policy; no public C API, refusal status, or emulated behavior changes.
- Preserve the existing release install-line consumer, Emscripten link checks, compatibility disclaimer check, and document budgets. Keep the README short and use existing reference docs for detail; no general prose linter or repository-wide rewrite.
<!-- DATA_7b19e4a2_END -->

### the agent's Discretion

<!-- DATA_0d6ce5b9_START -->
Choose concise wording, headings, and link placement that satisfy these decisions and preserve the existing checks. Remove generic or redundant comments only in files touched by the journey; retain sourced hardware explanations and non-obvious intent.
<!-- DATA_0d6ce5b9_END -->

### Deferred Ideas (OUT OF SCOPE)

<!-- DATA_b64390c1_START -->
No new feature ideas were raised. Cross-platform physical input, broad game-compatibility validation, new host/UI work, and generalized comment or prose cleanup remain outside this phase.
<!-- DATA_b64390c1_END -->
</user_constraints>

<phase_requirements>
## Phase Requirements

| ID | Description | Research Support |
|----|-------------|------------------|
| PERSONA-01 | A maintainer can trace the primary player, runner/homebrew developer, and C host integrator jobs to current Muhtari deliverables using dated evidence, while distinguishing direct feedback, source-backed needs, and hypotheses. | Refresh the existing persona/JTBD section with source/date/confidence/owner/revisit fields; distinguish one owner's 2026-10-29 player observations from inferred runner and integrator jobs. |
| PLAYER-01 | A player can install a supported release core and load a lawfully obtained Atari 7800 image in RetroArch using documented prerequisites, steps, expected result, controls, optional BIOS behavior, and support limits. | Extend the README after its existing install and no-content test-card path; link to existing libretro/BIOS details and use official RetroArch docs for directory/menu wording. |
| RUNNER-01 | A runner user can run the built-in test card or probe/run their own cartridge using the current release or documented local build, understand expected output and refusal status, and find the full CLI reference. | Keep the test-card command/hash, add the finite own-image path, explain probe limits and stable exit classes, update neutral 2600 refusal wording, and link docs/runner.md. |
| DOCS-01 | Users can navigate current first-use and status documentation whose local links, release references, package/API claims, and commands match the shipped artifacts and source; touched comments explain intent without redundant narration and hardware claims remain sourced. | Repair stale Phase 25 links, align status with the generated ledger, correct .info claims, qualify BankSet example, preserve targeted CI/document checks. |
</phase_requirements>

## Summary

This is documentation and product-evidence work over existing deliverables. Keep the README as a short entry point: release package selection/installation, the no-content test card, then loading a lawful image with the current core. Put detailed controls and BIOS behavior in docs/libretro.md, and CLI behavior and statuses in docs/runner.md. The Apple Silicon install line is a CI input and should remain byte-for-byte stable. [VERIFIED: 30-CONTEXT.md:13-19; README.md:12-63; docs/libretro.md:1-24; tests/cmake/install_line.cmake:1-12]

The key corrections are bounded: README and docs/status.md link to a removed Phase 25 path; the shared status text still says 2600 refusal is “planned”; the packaged .info description wrongly groups supported Souper mapper behavior with unavailable BupChip audio; and docs/libretro.md gives a generic BankSet refusal example that needs its configuration qualified. Keep refusal status and emulation behavior unchanged. [VERIFIED: README.md:8-10; docs/status.md:8-11; host/status_text.c:28-30; libretro/muhtari_libretro.info.in:39; docs/libretro.md:3-6; docs/runner.md:68; docs/bankset-tests.md:12-18,49-52]

**Primary recommendation:** edit only the first-use docs, the existing persona/JTBD section, the .info source template, and the shared refusal wording plus its existing output assertions. Reuse current tests and verify changed local links.

## Architectural Responsibility Map

| Capability | Primary Tier | Secondary Tier | Rationale |
|------------|-------------|----------------|-----------|
| Player setup and release navigation | Documentation / static release artifacts | libretro core and RetroArch host | Muhtari owns archives, core info, and instructions; RetroArch supplies menus, directories, bindings, and host devices. |
| Runner first use | CLI / runner | C core and docs | Runner owns finite execution, probe output, hashes, and exit codes; docs explain implemented contracts. |
| User-job evidence | Project documentation | Current release and dated onboarding records | This refreshes a planning reference, not a persona system or customer database. |
| Core-info accuracy | Release metadata | libretro capability tests | The .info template is packaged for RetroArch, and CI links enabled capability claims to evidence. |

## Standard Stack

| Library/tool | Version | Purpose | Why Standard |
|---|---|---|---|
| Existing Markdown docs and CMake checks | Repository-defined | User entry points, generated compatibility page, bounded checks | The phase adds no runtime dependency; current docs and CI consumers already own these contracts. |

No external packages are required. Change libretro/muhtari_libretro.info.in, the source for the generated package metadata; do not edit build output. docs/compatibility.md is generated from the ledger and should be validated through its generator check. [VERIFIED: libretro/CMakeLists.txt:90-93; tests/policy/tests.cmake:134-151]

## Project Constraints (from AGENTS.md)

- Keep the three deliverables (C library, headless runner, libretro core); add no GUI, host-window, audio-device, or input-device code. Atari 2600 emulation belongs to twentysixhundo; keep Muhtari's recognition/refusal boundary.
- Reuse permissively licensed material only; never open or retain GPL, LGPL, or unlicensed emulator source. Add no ROM, BIOS, save, screenshot, or personal-media bytes, and keep personal paths/names out of tracked docs.
- Keep README short and user-facing; detailed behavior belongs in docs. Every behavior change needs an existing-suite test. Hardware claims need sources.
- Prefer offline, CLI and automated checks. Do not make owner/RetroArch checks recurring acceptance; run RetroArch only when an authorized check has specific evidence value.
- Reuse existing CMake/POSIX checks. A materially changed check needs a named fault and pass/fault coverage. No broad prose linter or repository-wide rewrite.
- Read relevant lessons before planning/execution. Keep evidence as a command, exit status and run URL, not a committed log.
- Before a committing phase step, branch from origin/main; never commit on main. Packaged .info or runner-output corrections use the normal release-please path and exact-source gates.
- GSD summaries retain block-style requirement coverage. Avoid forbidden chain/auto flags and hygiene bypasses.

## Architecture Patterns

### System Architecture Diagram

~~~mermaid
flowchart LR
  Release[Native archive for OS and running-process architecture] --> Install[Configured RetroArch Cores and Core Info directories]
  Docs[README and libretro reference] --> Install
  Install --> Core[Load Muhtari core]
  Core -->|Start Core, no content| Card[Built-in test card]
  Core -->|Load Content, select current core| Content[Lawful .a78 or headerless .bin]
  Content -->|supported configuration| Startup[Cartridge's own startup screen]
  Content -->|unsupported configuration| Refusal[Frontend refusal]
  RunnerRelease[Runner archive or documented local build] --> Runner[muhtari-run]
  Runner -->|no cartridge| TestCard[Finite test-card frame and pinned hash]
  Runner -->|optional probe| Probe[Image identity and hashes]
  Probe -->|does not prove run support| Runner
  Runner -->|own image and finite frames| Result[Content-dependent hash or refusal status]
~~~

This describes user flows; it does not establish host-level gameplay on every platform or support for every recognized cartridge. [VERIFIED: 30-CONTEXT.md:13-25; docs/runner.md:57-70; docs/libretro.md:3-6]

### Existing Component Responsibilities

| File | Phase responsibility |
|---|---|
| README.md | Concise player and runner first steps; preserve CI-consumed install line. |
| docs/libretro.md | Detailed controls and BIOS contract; qualify BankSet refusal example. |
| docs/runner.md | Probe/run examples, output/refusal behavior, full CLI reference. |
| docs/status.md | Latest-release link, current archive evidence, compatibility totals. |
| .planning/preparation/A7800-ECOSYSTEM-AND-USERS.md | Refresh existing persona/JTBD section; create no second persona file. |
| libretro/muhtari_libretro.info.in | Correct packaged descriptive claim at source. |
| host/status_text.c and tests/runner/tests.cmake | Neutral wording for shared 2600 refusal and update existing output assertions. |

### Pattern 1: Short entry point, detailed reference

Keep commands and the next action in README; link existing references for full behavior. [VERIFIED: README.md:35-63; 30-CONTEXT.md:14-27]

### Pattern 2: Change source metadata, check its claims

Change the .info.in template. libretro.info_claims checks enabled capability keys against the test inventory and enforces the description length; preserve the separate compatibility-disclaimer check. [VERIFIED: libretro/CMakeLists.txt:90-93; tests/libretro/tests.cmake:391-434; tests/cmake/info_claims.cmake:1-6,13-50; tests/cmake/claims_wording.cmake:1-14]

### Pattern 3: Keep generated status tied to its source

Treat the compatibility ledger as source; run policy.compat_doc_fresh to compare generated docs/compatibility.md, then compare docs/status.md totals with that result. [VERIFIED: docs/compatibility.md:3-26; tests/policy/tests.cmake:134-151]

## Don't Hand-Roll

| Problem | Don't Build | Use Instead | Why |
|---|---|---|---|
| RetroArch directories | Installer or fixed cross-platform path matrix | Existing archives and configured Cores/Core Info directories | Official docs say locations vary and are configured in RetroArch. |
| Compatibility totals | Second title table | Existing ledger and compatibility generator | The generated page checks statuses and counts. |
| Core capability metadata | New .info checker | libretro.info_claims | It maps enabled capability fields to tests. |
| General documentation lint | New generic prose/link linter | Existing budgets, claim, generated-page and install-line checks; verify touched links | The phase has specific correctness contracts and excludes a broad prose linter. |

## Common Pitfalls

### Test card versus game content

Explain Start Core as a no-content test. Give a separate Load Content path. For supported content, describe the cartridge's own startup screen without promising a universal screen or playability. [VERIFIED: README.md:35-40; docs/libretro.md:3-6; CITED: https://docs.libretro.com/guides/starting-a-game/]

### Probe versus runnable configuration

Label --probe as image identification only. A finite run is separate evidence and its frame hash depends on content. [VERIFIED: docs/runner.md:57-70; 30-CONTEXT.md:22-25]

### Overstating player evidence

Keep the Choplifter and Ninja Golf reports under one 2026-10-29 owner onboarding session. Choplifter has GUI-load/casual-play details; Ninja Golf has only brief positive feedback. Neither changes automated ledger statuses or establishes general compatibility. [VERIFIED: .planning/LESSONS.md:409-437; docs/compatibility.md:14-26; 30-CONTEXT.md:29-33]

### Updating a shared status message

The old string is in host/status_text.c, consumed by both runner and libretro logs. Update the shared message and inspect both consumers; update runner output assertions/docs while preserving code 77. [VERIFIED: host/status_text.c:9-30; runner/main.c:593-598; libretro/libretro.c:1289; tests/runner/tests.cmake:885-892,1528-1552]

### BankSet and Souper accuracy

BankSet diagnostics cover multiple geometries; the 52 KiB refusal is conditional on the documented HSC configuration. The .info's Souper exclusion contradicts current mapper support. State those limits narrowly and retain the independent BupChip audio restriction. [VERIFIED: docs/bankset-tests.md:12-18,49-52; docs/runner.md:68; libretro/muhtari_libretro.info.in:39; docs/status.md:8-24]

### RetroArch paths and bindings

Core and info folders are configured per installation. Use configured directories rather than claiming fixed OS paths. Keep the existing Apple Silicon command unchanged because CI executes it, and document RetroPad mappings rather than host keyboard keys. [CITED: https://docs.libretro.com/guides/download-cores/; https://docs.libretro.com/guides/change-directories/; VERIFIED: tests/cmake/install_line.cmake:1-12,95-123; docs/libretro.md:12-24]

## Code Examples

The existing no-content runner test is pinned in README. [VERIFIED: README.md:44-54]

<!-- DATA_63f8ab12_START -->
`bin/muhtari-run --frames 1 --hash-frame 1`
<!-- DATA_63f8ab12_END -->

Its currently documented result is:

<!-- DATA_99c071a4_START -->
`frame 1 sha256 9eb011e22feb3f48da2c0abf3dd8d5236d4d0f5c0e67a732eb2172259e0a05c4`
<!-- DATA_99c071a4_END -->

For the user's own image, keep probe and run separate. The finite frame count below is the scoped sample, not a gameplay benchmark. [VERIFIED: 30-CONTEXT.md:22-25]

<!-- DATA_03a56f2d_START -->
`bin/muhtari-run --probe "game.a78"`  
`bin/muhtari-run --frames 120 --hash-frame 120 --rom "game.a78"`
<!-- DATA_03a56f2d_END -->

Preserve the current runner refusal classes: [VERIFIED: runner/exit_status.h:7-13,21-35]

<!-- DATA_c4e37e51_START -->
`MUHTARI__EXIT_FAILURE = 1`  
`MUHTARI__EXIT_USAGE = 2`  
`MUHTARI__EXIT_REFUSED = 77`
<!-- DATA_c4e37e51_END -->

The stale 2600 wording appears in the shared status helper, runner expectations and documentation. Replace only wording. [VERIFIED: host/status_text.c:28-30; tests/runner/tests.cmake:889,1537-1538; docs/runner.md:386]

<!-- DATA_805bf93e_START -->
`unsupported: 2600 (planned)`
<!-- DATA_805bf93e_END -->

## State of the Art

| Old approach | Current approach | Evidence and impact |
|---|---|---|
| Persona horizons centered on v1.0-v6.0 plans | Three jobs mapped to shipped libretro core, muhtari-run, and C library/package route | Existing persona research is dated 2026-10-15 and predates v11.0; refresh it but keep inferred needs labeled. [VERIFIED: .planning/preparation/A7800-ECOSYSTEM-AND-USERS.md:118-135; .planning/research/next-milestone/2026-11-19-first-use-readiness-review.md:37-70] |
| README stopped at the no-content card | Add the content-loading step | README currently ends that path at Start Core. [VERIFIED: README.md:35-40] |
| “planned” refusal wording | Neutral unsupported wording with unchanged refusal status | Current source and tests identify the stale text; no behavior change is requested. [VERIFIED: host/status_text.c:28-30; 30-CONTEXT.md:26-27] |

## Assumptions Log

| # | Claim | Section | Risk if Wrong |
|---|---|---|---|
| A1 | Runner/homebrew needs are inferred from current CLI affordances and dated internal research, not validated by interviews/adoption evidence. | Persona refresh | Overstated demand could distort later scope. [ASSUMED per 30-CONTEXT.md:29-33 and 2026-11-19-first-use-readiness-review.md:64-70] |
| A2 | C host-integrator need is a hypothesis from the shipped interface/package route; its complete quickstart belongs to Phase 31. | Persona refresh | Misleadingly presenting a hypothesis as customer validation. [ASSUMED per 30-CONTEXT.md:9,29-31] |
| A3 | Release baseline remains 0.18.0 at implementation. | Release links | A newer release may exist; recheck then. [ASSUMED until rechecked; version.txt:1; .planning/milestones/v11.0-MILESTONE-AUDIT.md:130-134] |

## Open Questions

1. **RESOLVED for planning — latest release at implementation is an execution-time check.** Planning uses v0.18.0 as the documented baseline because the v11.0 audit records 20 assets for that release on 2026-11-19. Phase 30 Task 2 checks the live latest stable release and native assets before editing, uses version-free latest-release links, and keeps the v0.18.0 receipt as historical evidence. Implementation must record the fresh lookup before settling README wording. No owner decision is needed. [VERIFIED: .planning/milestones/v11.0-MILESTONE-AUDIT.md:130-134; 30-CONTEXT.md:13-14; 30-01-PLAN.md Task 2]

## Environment Availability

Skipped: documentation and existing metadata only; no external runtime/service is required to author the change. RetroArch is consulted through official docs, but no actual host run is an acceptance requirement. [VERIFIED: 30-CONTEXT.md:7-37]

## Existing Checks and Cheap Preflight

Run focused checks before the full suite; do not add a general prose linter.

| Check | Protects | Planning note |
|---|---|---|
| git diff --check | Edited diff formatting | Cheapest first pass. |
| cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake | README/reference document budgets | Use an absolute root; prior lesson records that relative ROOT=. can miss nested docs. [VERIFIED: tests/policy/tests.cmake:98-106; .planning/STATE.md deferred-items entry] |
| policy.claims_wording | Required compatibility disclaimer and overclaim deny list | Preserve the test's fault/self-test coverage. [VERIFIED: tests/policy/tests.cmake:32-48; tests/cmake/claims_wording.cmake:1-14] |
| policy.compat_doc_fresh | Generated compatibility page equals ledger-derived output | Compare manually maintained docs/status.md totals to generated evidence. [VERIFIED: tests/policy/tests.cmake:134-151] |
| libretro.info_claims | Enabled .info capabilities map to inventory tests; description length is at most 512 chars | It does not validate free-form prose semantics, so review Souper/BupChip wording against source. [VERIFIED: tests/libretro/tests.cmake:391-434; tests/cmake/info_claims.cmake:1-50] |
| runner.probe.2600, runner.hsc_rom.refused, runner.exit.table | Probe/refusal text, status 77 through HSC paths, docs matching --help status table | Retain fixture coverage and update expected message only. [VERIFIED: tests/runner/tests.cmake:1680-1717,1856-1860; tests/runner/tests.cmake:885-892,1528-1552] |
| README install-line CI consumer | Executes existing macOS command and confirms core and info files were installed | Keep the command unchanged; this script is macOS-specific. [VERIFIED: .github/workflows/ci.yml:252-254; tests/cmake/install_line.cmake:1-12,95-123] |

Then run the full configured suite with `cmake --workflow --preset ci`. The project config explicitly disables Nyquist validation and security enforcement, so no separate Validation Architecture or ASVS analysis is needed. [VERIFIED: .planning/config.json workflow.nyquist_validation=false; security_enforcement=false]

## Security Domain

Omitted: .planning/config.json explicitly sets security_enforcement to false; this phase changes first-use copy and descriptive metadata, not security controls. [VERIFIED: .planning/config.json]

## Sources

### Primary repository sources (HIGH confidence)

- 30-CONTEXT.md, REQUIREMENTS.md, ROADMAP.md, STATE.md — locked scope, requirements, active release limits, milestone state.
- README.md, docs/libretro.md, docs/runner.md, docs/status.md, docs/compatibility.md, docs/bankset-tests.md — current user paths and contracts.
- host/status_text.c, runner/exit_status.h, runner/main.c, libretro/libretro.c, libretro/muhtari_libretro.info.in, libretro/CMakeLists.txt — refusal source/codes/shared consumers and packaged metadata.
- tests/runner/tests.cmake, tests/cmake/install_line.cmake, tests/cmake/info_claims.cmake, tests/cmake/claims_wording.cmake, tests/policy/tests.cmake — existing automated gates.
- .planning/LESSONS.md and .planning/milestones/v11.0-MILESTONE-AUDIT.md — dated onboarding and release evidence.

### Official RetroArch documentation (MEDIUM confidence; accessed 2026-11-19)

- [Installing cores in RetroArch](https://docs.libretro.com/guides/download-cores/) — package/platform selection, module and info installation; last updated 2026-10-22.
- [Directory Configuration](https://docs.libretro.com/guides/change-directories/) — configured folders vary; verify in Settings; last updated 2026-07-25.
- [User Interface](https://docs.libretro.com/guides/navigating/) — menu vocabulary; last updated 2026-10-17.
- [Starting a Game](https://docs.libretro.com/guides/starting-a-game/) — Load Core, Load Content and current-core flow; last updated 2024-11-15. Do not infer identical presentation across builds.
- [Launching Content](https://docs.libretro.com/guides/launch-content/) — legal-content prerequisite; last updated 2020-07-19. Supporting context only; use newer install/directory pages for path claims.

## Metadata

**Confidence breakdown:** Standard stack HIGH (no new dependency); architecture HIGH (existing deliverables and official docs); pitfalls HIGH for repository mismatches, MEDIUM for persona hypotheses and host presentation.  
**Research date:** 2026-11-19  
**Valid until:** 2026-12-19 for repository patterns; recheck latest release and official RetroArch menu/directory docs at implementation.
