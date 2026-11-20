# Phase 30: User jobs and player/runner first use - Context

**Gathered:** 2026-11-19
**Status:** Ready for planning

<domain>
## Phase Boundary

Make the current player and runner paths understandable from a Muhtari release, and refresh the evidence behind the three primary product jobs. Correct stale or contradictory claims encountered along those paths. The C host-integrator job is part of the persona refresh; its complete installed-package example belongs to Phase 31. This phase adds no emulator behavior, API, host UI, device code, or compatibility claim.

</domain>

<decisions>
## Implementation Decisions

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

### the agent's Discretion
Choose concise wording, headings, and link placement that satisfy these decisions and preserve the existing checks. Remove generic or redundant comments only in files touched by the journey; retain sourced hardware explanations and non-obvious intent.

</decisions>

<code_context>
## Existing Code Insights

### Reusable Assets
- `README.md` already has the macOS Apple Silicon release install command, supported-platform requirements, test-card description, runner hash example, and links to the detailed references.
- `docs/libretro.md`, `docs/runner.md`, `docs/status.md`, and `docs/compatibility.md` contain the current controls, BIOS, CLI, refusal, release, and evidence details.
- `libretro/muhtari_libretro.info.in` is the source for packaged RetroArch core information; the `.info` file is generated from it.
- The current release evidence and package baseline are in `.planning/milestones/v11.0-MILESTONE-AUDIT.md` and the v0.18.0 release record.

### Established Patterns
- Keep the README as a short user entry point and put full behavior contracts in `docs/`.
- The release install-line check consumes the README's current macOS command; compatibility wording and core capability metadata also have existing CMake checks. Reuse these checks and preserve their intended assertions.
- Separate direct observation, source-backed need, inference, and unknowns. A probe result is identification evidence, not proof that execution will succeed.

### Integration Points
- Release archives contain the platform core and its RetroArch info file; the package version is shown in the generated info file.
- The CLI contract is implemented in `runner/main.c` and `runner/exit_status.h`, with expected examples and refusal details in `docs/runner.md`.
- The player path connects the release archive, RetroArch's configured core directories, the core display name, and `Load Content`; the legal content and BIOS sources remain the user's responsibility.

</code_context>

<specifics>
## Specific Ideas

- Current project release baseline: v0.18.0, source `ac4ee132449f82848da1db493c428f78d463d6cd`; source-backed packaging is not evidence of every platform's host-level gameplay behavior.
- Official RetroArch guidance checked 2026-11-19: [manual core installation](https://docs.libretro.com/guides/download-cores/) and [loading content](https://docs.libretro.com/guides/launch-content/). Use these for current menu and directory terms.
- Existing owner onboarding evidence is dated 2026-10-29 and describes RetroArch 1.22.2 on Apple Silicon; preserve its narrow setup and confidence limits.

</specifics>

<deferred>
## Deferred Ideas

No new feature ideas were raised. Cross-platform physical input, broad game-compatibility validation, new host/UI work, and generalized comment or prose cleanup remain outside this phase.

</deferred>
