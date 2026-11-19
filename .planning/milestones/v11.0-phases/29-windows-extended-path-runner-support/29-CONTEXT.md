# Phase 29: Windows extended-path runner support - Context

**Gathered:** 2026-11-18
**Status:** Ready for planning

<domain>
## Phase Boundary

Make every documented filesystem-path argument to muhtari-run work with explicit absolute Windows drive (\\?\C:\...) and extended UNC (\\?\UNC\server\share\...) paths. Preserve Unicode and literal namespace names, reject conflicting paths before changing files, and preserve old output bytes if state, frame-image, trace, or battery writes fail. Document the supported forms and the longest path actually exercised. Keep changes in the headless runner, its tests, Windows CI, and runner documentation; do not change the public API/ABI, emulator behavior or state formats, add a GUI/device path, or alter general CI topology.

</domain>

<decisions>
## Implementation Decisions

### Supported path contract
- **D-01:** Support only absolute drive-letter \\?\C:\... and extended UNC \\?\UNC\server\share\... paths for all documented runner file arguments: cartridge, movie, BIOS, HSC firmware, state input, save directory, state output, frame image, and trace output. Reject malformed extended names, relative extended paths, NT device paths, volume-GUID paths, and other namespaces with a clear usage or I/O error. Do not promise ordinary unprefixed long paths or lengths beyond tested filesystem cases.
- **D-02:** On Windows, preserve path arguments as Unicode from process entry through each open, metadata, comparison, temporary-file, and replacement operation. Keep the explicit extended prefix, backslashes, Unicode code points and trailing dot/space characters intact; do not trim suffixes or Unicode-normalize file names. Use wide Windows/CRT APIs for filesystem operations and retain current ordinary-path behavior and its alias controls. Microsoft documents that \\?\ disables normal Win32 path parsing, and ordinary dot/dot-dot, separator and suffix assumptions cannot be carried into the extended namespace: [namespace and naming rules](https://learn.microsoft.com/en-us/windows/win32/fileio/naming-a-file), [path-length behavior](https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation), [wide wmain arguments](https://learn.microsoft.com/en-us/cpp/text/support-for-using-wmain?view=msvc-170).

### Collision protection and output safety
- **D-03:** Apply collision checks consistently across all input and output path roles. Distinct extended names such as capture and capture. must be allowed when they are distinct; paths resolving to the same existing file, aliased through ordinary/extended spelling or a parent junction, or overlapping another input/output must be refused before writes. Compare existing targets by handle-based filesystem identity and absent targets by their parent identity plus namespace-aware leaf name; fail closed if required identity evidence cannot be obtained, including on an SMB path. Microsoft notes that final-path lookup on network paths can fail because of share permissions: [handle file identity](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfileinformationbyhandle), [network limits for final-path lookup](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfinalpathnamebyhandlew).
- **D-04:** Reuse the existing sibling-temporary write-and-replace flow for state, trace, and battery outputs, converting its Windows operations to wide paths. Route PNG frame output through that same checked atomic publication path so a failed write cannot truncate an existing image. Test create, mid-write, and replacement failures for every output class while preserving prior bytes. Keep the repair scoped to the runner’s existing private path boundary; only extract a helper/module if planning shows it reduces real duplication or risk without broadening the subsystem.

### Tests, CI, and documentation
- **D-05:** Acceptance must invoke the actual Windows muhtari-run executable so tests exercise wide process arguments as well as file operations. Cover both namespace forms, each documented path role, non-ACP Unicode, literal suffix names, a representative path longer than MAX_PATH, suffix-distinct destinations, ordinary/extended same-file aliases, junction aliases, hard-link identity where supported, input/output overlaps, safe failure preservation, and ordinary-path regression controls. Include generated battery names and sibling temporary names in length and collision cases.
- **D-06:** Add UNC acceptance to the existing Windows CI lanes using a temporary local SMB share when the hosted VM supports create/access/remove. GitHub documents Windows hosted VMs as administrator-capable, and Microsoft provides New-SmbShare to expose a local folder, so this is the smallest in-repo CI path; do not add a remote share, credentials, self-hosted runner, CI job topology change, or dependency. The actual UNC read/write and cleanup must still pass on the Windows runners before WINPATH-04 is accepted. If the ephemeral share is unavailable, retain that exact requirement as blocked and continue independent local work; do not substitute local drive-path results for UNC evidence.
- **D-07:** Document the two supported forms and measured end-to-end path/name boundary, including filesystem and component-length limits. State only what the actual Windows tests prove; do not turn Windows’ theoretical extended path ceiling into a product promise. Correct any runner docs that claim frame output is atomically published if implementation/tests do not support that claim.
- **D-08:** Add no third-party dependency. Reuse the C runtime and Win32 wide APIs, existing atomic writer, and CMake/CTest harness. Keep required behavior tests in the existing CI inventory and run cheap formatting/consistency checks before a long workflow.

### Reviewed Todos
- **Folded:** Define extended Windows output path support (.planning/todos/pending/2026-11-17-define-extended-windows-output-path-support.md). Phase 27 review found the trace comparator removes trailing dots/spaces under ordinary Windows rules, which can falsely reject distinct extended-namespace outputs. Phase 29 now defines explicit drive/UNC support and tests distinct destinations while retaining alias refusal.

### the agent's Discretion
- Choose the private wide-path representation, helper boundaries, exact Win32 identity query, failure-injection seams, fixture names, and measured test-path length. Prefer the smallest implementation that covers every listed path role; keep all behavior deterministic and do not weaken collision checks when an alias cannot be resolved.
- Add a small share setup/cleanup routine to existing Windows acceptance only if it can prove cleanup and avoid external credentials or services. Keep share failure explicit in test output.
- Keep user-facing messages concise and avoid adding a separate generalized path framework unless reuse within the runner justifies it.

</decisions>

<canonical_refs>
## Canonical References

**Downstream agents MUST read these before planning or implementing.**

### Scope and product intent
- .planning/ROADMAP.md — Phase 29 goal, acceptance criteria, dependency and deferred boundaries.
- .planning/REQUIREMENTS.md — WINPATH-01 through WINPATH-04 and deferred WINPATH-05/06.
- .planning/PROJECT.md — the three deliverables and deterministic, bounded core contract.
- .planning/INTENT.md — owner priorities, research, autonomous-work and dependency preferences.
- .planning/research/fan-out/prompt.txt — role-specific research, adversarial pass, primary-source and synthesis requirements.
- .planning/todos/pending/2026-11-17-define-extended-windows-output-path-support.md — carried Phase 27 finding and selected follow-up.
- .planning/milestones/v10.0-phases/27-headless-developer-trace/27-CONTEXT.md — trace path and file-safety conventions immediately upstream of this follow-up.
- .planning/milestones/v10.0-phases/27-headless-developer-trace/27-REVIEW.md — WR-01 evidence and limits.

### Platform, architecture and runner conventions
- .planning/preparation/README.md — index for project preparation references.
- .planning/preparation/DECISIONS.md — settled dependency, runner and CI decisions; do not reopen without evidence.
- .planning/preparation/ENGINEERING.md §11 — architecture, resource-bound and documentation review; §3–4 — recurring tests and CI policy.
- .planning/preparation/HOSTS-AND-INTEGRATION.md §4 — runner path, output and exit-status contract.
- .planning/preparation/GSD-HANDOFF.md §3.1 — bounded specialist fan-out, evidence and one adversarial synthesis.
- .planning/LESSONS.md — recent Windows/compiler, path, CI preflight and workflow lessons.
- .planning/WINDOWS.md — prior Windows-specific findings; use only relevant rows without changing unrelated dispositions.
- docs/runner.md — public runner CLI and output safety promises.

### Implementation and recurring acceptance
- runner/main.c — command-line entry, file I/O, path comparison, trace/state/battery writes and frame PNG output.
- tests/runner/tests.cmake — registered runner acceptance controls.
- tests/runner/test_save.c — atomic output and preservation controls.
- tests/runner/trace.cmake — path-collision and trace-output controls.
- .github/workflows/ci.yml — current Windows x64 and ARM64 jobs and CMake workflow invocation.
- CMakePresets.json — configured ci workflow and test registration.
- tests/roms/manifest.txt — only authority for committed ROM-like fixture bytes.

</canonical_refs>

<code_context>
## Existing Code Insights

### Reusable Assets
- write_sibling_atomic_stream() in runner/main.c already creates a sibling temporary, checks stream completion and flush/sync/close, then replaces the destination. Its current Windows replacement and path handling use narrow APIs and need a wide equivalent.
- trace_paths_equal() and trace_path_conflicts() provide an existing starting point for ordinary path identity, but current checks are uneven across state/frame paths and remove suffixes according to ordinary Win32 rules.
- tests/runner/test_save.c, tests/runner/trace.cmake, tests/runner/tests.cmake, and the existing Windows CI inventory provide recurring test locations; the new namespace acceptance must also run the real executable, not only the in-process runner harness.

### Established Patterns
- Runner filesystem arguments are documented in docs/runner.md; output files are runner-owned and must not change the core API or serialized machine state.
- Outputs that replace an existing destination use sibling-temp publication; failed writes must leave prior bytes unchanged.
- Windows builds use MSVC warnings as errors and run on existing x64 and ARM64 CI lanes. Run focused formatting/consistency checks before full CMake CI, and do not modify topology or deadlines without repeated measurements.
- runner/main.c currently enters through main(char **) and uses narrow fopen, _open, _stat64, CreateFileA, _fullpath, MoveFileExA, and ACP conversion. dump_frame() directly opens with "wb", so it currently can truncate a destination on write failure, contrary to WINPATH-03's required behavior.

### Integration Points
- The wide argument and file-path boundary connects CLI parsing to cartridge/movie/BIOS/HSC/state reads, save-directory-derived battery paths, trace path identity, temporary file creation and replacement, and PNG output.
- Collision validation must see all documented input/output roles and run before emulation or file modification; Windows actual-process tests need to cover argument conversion and filesystem behavior together.
- The Windows acceptance belongs in current runner CTest registration and its existing Windows CI jobs, with a per-job ephemeral SMB share to exercise extended UNC paths.

</code_context>

<specifics>
## Specific Ideas

Use tests that distinguish capture from capture. or capture-space through \\?\, then prove the ordinary/extended aliases that really share one file are still rejected. Include Unicode outside the active ANSI code page and a directory tree that pushes the final test path beyond MAX_PATH. Validate a frame-image write failure against a pre-existing PNG because source inspection shows that gap today.

</specifics>

<deferred>
## Deferred Ideas

- Native NT device paths, volume-GUID paths, relative extended paths, general unprefixed long-path opt-in, and any claim beyond the longest measured Windows test case (WINPATH-05/06).
- External SMB infrastructure or self-hosted runners; reconsider only if the existing Windows hosted VM cannot create and use a temporary local share.
- Broad CI topology, cache, timeout, job-parallelism, dependency, CLI/API, core behavior, GUI, device, or state-format changes.

### Reviewed Todos (not folded)
- Close the re-review's findings on the Dependabot gate and the publish checks — CI/release scope unrelated to runner path support.
- Promote the wasm32 lane into CI required — separate CI membership decision; no topology or membership change is in scope.
- Close the phase 2 review seeds on the nightly filter and the ROM tables — unrelated Phase 2/CI work.
- Close the phase 2 re-review findings when MARIA and the run loop land — unrelated core implementation review.
- Keep the phase 11 first review's info findings whose ids the gap-round review reused — unrelated historical test findings.

</deferred>

---

*Phase: 29-windows-extended-path-runner-support*
*Context gathered: 2026-11-18*
