# Phase 29: Windows extended-path runner support - Research

**Researched:** 2026-11-18
**Domain:** Windows paths, wide APIs, identity checks, atomic outputs and CMake/CTest
**Confidence:** MEDIUM

<user_constraints>
## User Constraints (from CONTEXT.md)

### Locked Decisions

### Supported path contract
- **D-01:** Support only absolute drive-letter \\?\C:\... and extended UNC \\?\UNC\server\share\... paths for all documented runner file arguments: cartridge, movie, BIOS, HSC firmware, state input, save directory, state output, frame image, and trace output. Reject malformed extended names, relative extended paths, NT device paths, volume-GUID paths, and other namespaces with a clear usage or I/O error. Do not promise ordinary unprefixed long paths or lengths beyond tested filesystem cases.
- **D-02:** On Windows, preserve path arguments as Unicode from process entry through each open, metadata, comparison, temporary-file, and replacement operation. Keep the explicit extended prefix, backslashes, Unicode code points and trailing dot/space characters intact; do not trim suffixes or Unicode-normalize file names. Use wide Windows/CRT APIs for filesystem operations and retain current ordinary-path behavior and its alias controls. Microsoft documents that \\?\ disables normal Win32 path parsing, and ordinary dot/dot-dot, separator and suffix assumptions cannot be carried into the extended namespace: [namespace and naming rules](https://learn.microsoft.com/en-us/windows/win32/fileio/naming-a-file), [path-length behavior](https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation), [wide wmain arguments](https://learn.microsoft.com/en-us/cpp/c-language/using-wmain?view=msvc-170).

### Collision protection and output safety
- **D-03:** Apply collision checks consistently across all input and output path roles. Distinct extended names such as capture and capture. must be allowed when they are distinct; paths resolving to the same existing file, aliased through ordinary/extended spelling or a parent junction, or overlapping another input/output must be refused before writes. Compare existing targets by handle-based filesystem identity and absent targets by their parent identity plus namespace-aware leaf name; fail closed if required identity evidence cannot be obtained, including on an SMB path. Microsoft notes that final-path lookup on network paths can fail because of share permissions: [handle file identity](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfileinformationbyhandle), [network limits for final-path lookup](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfinalpathnamebyhandlew).
- **D-04:** Reuse the existing sibling-temporary write-and-replace flow for state, trace, and battery outputs, converting its Windows operations to wide paths. Route PNG frame output through that same checked atomic publication path so a failed write cannot truncate an existing image. Test create, mid-write, and replacement failures for every output class while preserving prior bytes. Keep the repair scoped to the runner’s existing private path boundary; only extract a helper/module if planning shows it reduces real duplication or risk without broadening the subsystem.

### Tests, CI, and documentation
- **D-05:** Acceptance must invoke the actual Windows muhtari-run executable so tests exercise wide process arguments as well as file operations. Cover both namespace forms, each documented path role, non-ACP Unicode, literal suffix names, a representative path longer than MAX_PATH, suffix-distinct destinations, ordinary/extended same-file aliases, junction aliases, hard-link identity where supported, input/output overlaps, safe failure preservation, and ordinary-path regression controls. Include generated battery names and sibling temporary names in length and collision cases.
- **D-06:** Add UNC acceptance to the existing Windows CI lanes using a temporary local SMB share when the hosted VM supports create/access/remove. GitHub documents Windows hosted VMs as administrator-capable, and Microsoft provides New-SmbShare to expose a local folder, so this is the smallest in-repo CI path; do not add a remote share, credentials, self-hosted runner, CI job topology change, or dependency. The actual UNC read/write and cleanup must still pass on the Windows runners before WINPATH-04 is accepted. If the ephemeral share is unavailable, retain that exact requirement as blocked and continue independent local work; do not substitute local drive-path results for UNC evidence.
- **D-07:** Document the two supported forms and measured test-path/name boundary, including filesystem and component-length limits. State only what the actual Windows tests prove; do not turn Windows’ theoretical extended path ceiling into a product promise. Correct any runner docs that claim frame output is atomically published if implementation/tests do not support that claim.
- **D-08:** Add no third-party dependency. Reuse the C runtime and Win32 wide APIs, existing atomic writer, and CMake/CTest harness. Keep required behavior tests in the existing CI inventory and run cheap formatting/consistency checks before a long workflow.

### the agent's Discretion
- Choose the private wide-path representation, helper boundaries, exact Win32 identity query, failure-injection seams, fixture names, and measured test-path length. Prefer the smallest implementation that covers every listed path role; keep all behavior deterministic and do not weaken collision checks when an alias cannot be resolved.
- Add a small share setup/cleanup routine to existing Windows acceptance only if it can prove cleanup and avoid external credentials or services. Keep share failure explicit in test output.
- Keep user-facing messages concise and avoid adding a separate generalized path framework unless reuse within the runner justifies it.

### Deferred Ideas (OUT OF SCOPE)
- Native NT device paths, volume-GUID paths, relative extended paths, general unprefixed long-path opt-in, and any claim beyond the longest measured Windows test case (WINPATH-05/06).
- External SMB infrastructure or self-hosted runners; reconsider only if the existing Windows hosted VM cannot create and use a temporary local share.
- Broad CI topology, cache, timeout, job-parallelism, dependency, CLI/API, core behavior, GUI, device, or state-format changes.
</user_constraints>

<phase_requirements>
## Phase Requirements

| ID | Description | Research Support |
|----|-------------|------------------|
| WINPATH-01 | Users can pass explicit local \\?\ and extended UNC \\?\UNC\ paths to every documented muhtari-run filesystem-path argument: cartridge, movie, BIOS, HSC firmware, state input, save directory, state output, frame image output and trace output. Supported absolute paths preserve Unicode names and literal extended-namespace semantics, including names ending in a dot or space and paths longer than MAX_PATH where the filesystem permits them. | Use wide process arguments and retain wide paths through every open, metadata, comparison, derived-name, temporary and replacement step. |
| WINPATH-02 | muhtari-run distinguishes separate extended-namespace destinations that differ only by names preserved by that namespace, while refusing destinations that resolve to the same file or overlap an input or another output. Existing ordinary Windows alias behavior, including existing-file identity and parent-junction aliases, remains protected across ordinary and extended forms. | Use handle identity for existing files and parent identity plus namespace-aware leaf comparison for absent destinations; validate every path role before emulation or writes; fail closed on missing SMB identity evidence. |
| WINPATH-03 | State, frame-image, trace and battery outputs use the runner's safe write behavior for supported extended paths; a failed create, write or replacement reports an error and does not truncate or partially replace an existing destination. | Reuse the sibling-temp writer for all four outputs. PNG currently opens the final destination in wb and can truncate it before serialization completes. |
| WINPATH-04 | The Windows runner contract names the supported namespace forms and tested path-length boundary. Windows CI exercises the actual runner with local paths longer than MAX_PATH, Unicode and literal suffix names, extended UNC paths through an accessible test share, distinct outputs, true aliases, input/output overlap, ordinary-path controls and output failure preservation. | Add process-level acceptance to existing x64 and ARM64 Windows lanes; require real UNC I/O and cleanup; document only measured path/component limits. |

Source: [VERIFIED: .planning/REQUIREMENTS.md:18-21].
</phase_requirements>

## Summary

Use a Windows wide-character process and file path boundary from entry through parsing, opens, metadata, identity, generated names, temporary creation and replacement. Microsoft documents that extended prefixes preserve literal path interpretation, require backslashes, disallow relative/dot navigation, and pass Unicode names as WCHAR data. The approximate 32,767-character system ceiling and component limits that vary by filesystem are not product promises; report only the longest paths exercised by Windows CI. [CITED: https://learn.microsoft.com/en-us/windows/win32/fileio/naming-a-file] [CITED: https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation]

The runner already has a sibling-temp writer for state, trace and battery. PNG instead calls fopen(path, \"wb\") at runner/main.c:235, truncating the existing file before serialization can fail; the nearby docs/runner.md:252-255 passage concerns trace output, not PNG publication. Move frame serialization onto the shared writer as WINPATH-03 requires. Extend collision preflight across every input/output role, use handle identity for existing targets, and fail closed when SMB identity is unavailable. Add no dependency. [VERIFIED: runner/main.c:232-259, 1638-1729] [VERIFIED: docs/runner.md:252-255 describes trace output]

**Primary recommendation:** Keep one private Windows wide-path representation and W-API boundary, preserve ordinary short-path behavior, perform complete collision preflight, route all outputs through the existing atomic writer, and prove the actual executable on existing Windows x64/ARM64 CI.

## Project Constraints (from AGENTS.md)

- Keep changes to runner, tests, existing Windows CI and runner documentation. Do not change API/ABI, core behavior, state formats, deliverables or CI topology. [VERIFIED: .planning/phases/29-windows-extended-path-runner-support/29-CONTEXT.md:7-9]
- Use C17 and existing CMake/CTest; every behavior change needs recurring CI coverage. [VERIFIED: .planning/preparation/ENGINEERING.md:73-90]
- No third-party dependency or unlicensed/GPL emulator source; use generated or manifest-authorized fixtures. [VERIFIED: .planning/phases/29-windows-extended-path-runner-support/29-CONTEXT.md:22-26] [CITED: AGENTS.md]
- Keep behavior deterministic and bounded; preserve ordinary-path alias checks and old output bytes on failures. [VERIFIED: .planning/phases/29-windows-extended-path-runner-support/29-CONTEXT.md:17-26]
- Run cheap consistency/formatting checks before the longer workflow. [VERIFIED: .planning/phases/29-windows-extended-path-runner-support/29-CONTEXT.md:26,32]
- Verification is automation-first; no owner-run Windows check is selected. [CITED: .planning/AGENTS.md]

## Architectural Responsibility Map

| Capability | Primary Tier | Secondary Tier | Rationale |
|------------|-------------|----------------|-----------|
| Argument/path preservation | Runner process boundary | Windows CRT | Wide argv must be retained before parsing paths. |
| Opens, identity and replacement | Runner | Win32 / CRT | All file operations must preserve extended Unicode paths. |
| Collision preflight | Runner | Filesystem | Runner knows all input, output and derived paths before execution. |
| Acceptance | CMake/CTest | Existing Windows CI | Only Windows process and filesystem execution proves the contract. |
| Supported path documentation | Runner docs | CI evidence | State observed boundaries, not OS theoretical limits. |

## Standard Stack

| Facility | Purpose | Recommendation |
|----------|---------|----------------|
| C17 and existing MSVC toolchain | Runner and tests | Retain project baseline. [VERIFIED: .planning/preparation/ENGINEERING.md:73-90] |
| wmain, _wfopen/_wopen, CreateFileW and other W APIs | Wide arguments, opens and metadata | Keep path data wide throughout; narrow fopen uses CP_ACP. [CITED: https://learn.microsoft.com/en-us/cpp/c-language/using-wmain?view=msvc-170] [CITED: https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/fopen-wfopen?view=msvc-170] [CITED: https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilew] |
| GetFileInformationByHandle / file-ID query | Existing target identity | Compare volume and file ID from handles; fail closed on incomplete network data. [CITED: https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfileinformationbyhandle] |
| GetFinalPathNameByHandleW | Parent/junction resolution | Use only when adequate evidence is returned; normalized SMB lookup can fail access checks. [CITED: https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfinalpathnamebyhandlew] |
| MoveFileExW | Wide sibling-temp replacement | Retain the existing replace/write-through policy and test it on local and UNC paths. [CITED: https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-movefileexw] |
| Existing CMake/CTest | Process-level acceptance | Register in current runner inventory and existing Windows jobs. [VERIFIED: tests/runner/tests.cmake:1887-1939] |

**Installation:** None. No package legitimacy audit is needed because the phase installs no package.

### Alternatives Considered

| Instead of | Alternative | Tradeoff |
|------------|-------------|----------|
| Wide paths end to end | Convert from ANSI at each call | Loses names outside CP_ACP and leaves multiple conversion failure points. |
| Existing private atomic writer | New framework or package | Adds maintenance/dependency cost without a requirement benefit. |
| Temporary local share in current jobs | External SMB or self-hosted CI | Out of scope; local share remains conditional on observed access and cleanup. |
| Measured boundary | Promise the system maximum | The OS limit is approximate and component limits vary. |

## Architecture Patterns

### System Architecture

    Wide Windows command line
      -> wmain -> parse options, retain file paths as WCHAR
      -> validate namespace -> preflight all inputs/outputs
           existing target: compare handle identity
           absent target: parent identity plus namespace-aware leaf
           insufficient identity: refuse before work
      -> wide opens -> emulation
      -> sibling temporary -> finish/flush/sync/close -> W replacement
      -> result

    Windows CTest -> actual muhtari-run -> drive and temporary-share UNC cases
                  -> assert names, aliases, failures, old bytes and cleanup

### Recommended Project Structure

Keep the private path handling in runner/main.c unless a small helper reduces repeated logic. Add acceptance under tests/runner/, register it in the existing CTest inventory and Windows lanes, and update docs/runner.md. [VERIFIED: .planning/phases/29-windows-extended-path-runner-support/29-CONTEXT.md:65-93]

### Pattern 1: Wide path flow

Use wmain with wchar_t arguments and wide CRT/Win32 path APIs; keep non-Windows code unchanged. Cover every documented path role, including the derived battery name and sibling temp. CMake documents that Windows execute_process arguments are encoded for CommandLineToArgvW consumers, but the actual executable must prove the complete CRT process chain. [CITED: https://learn.microsoft.com/en-us/cpp/c-language/using-wmain?view=msvc-170] [CITED: https://cmake.org/cmake/help/latest/command/execute_process.html]

    int wmain(int argc, wchar_t *argv[]);
    FILE *input = _wfopen(path, L"rb");

Do not use ACP conversions, normalize Unicode, strip prefix/suffix characters, or substitute forward slashes. The extended prefix changes parsing and separator behavior. [CITED: https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation]

### Pattern 2: Complete collision preflight

Existing code checks trace collisions only when trace is selected; some state/frame checks are lexical. Its Windows comparator uses CP_ACP and strips trailing dots/spaces. [VERIFIED: runner/main.c:726-813, 913-960, 1288-1303] Compare all input/output and output/output pairs before emulation. Existing targets use handle identity; absent targets use parent identity and the namespace-aware leaf. Microsoft warns network identity may be partial and normalized SMB lookup can fail with access denied, so do not fall back to “different” when evidence is missing. [CITED: https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfileinformationbyhandle] [CITED: https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfinalpathnamebyhandlew]

### Pattern 3: One atomic output path

Reuse write_sibling_atomic_stream() for state, trace and battery. It already checks serialization, flush, sync and close before replacement. [VERIFIED: runner/main.c:1638-1729] Add PNG as a stream callback so it does not need a second full image buffer. Test create, mid-write and replacement failures for all four output classes against pre-existing sentinel bytes. Measure generated .sav and sibling temp names too.

### Anti-Patterns

- Converting path arguments through CP_ACP.
- Testing only opens while comparison, temp naming and replacement still use narrow APIs.
- Checking only trace collisions instead of every path role.
- Building extended paths with forward slashes; current battery construction uses slash. [VERIFIED: runner/main.c:945-954]
- Treating the in-process runner_entry harness as proof of the wide process boundary. It compiles runner/main.c under a renamed main. [VERIFIED: tests/runner/test_save.c:302-323]
- Claiming support from a destination path when its derived .sav/temp component was not tested.
- Falling back to lexical comparison after SMB identity lookup fails.
- Reporting 32,767 as the runner's supported limit.
- Adding a dependency or generalized path framework without demonstrated need.

## Don't Hand-Roll

| Problem | Avoid | Use |
|---------|-------|-----|
| Wide Windows args/filesystem names | Custom ANSI/codepage conversion | wmain and W/CRT-wide APIs |
| Existing-file identity | Timestamps or normalized strings | Handle-based volume/file identity |
| Atomic publication | Separate writer per output or dependency | Existing sibling-temp writer |
| SMB test environment | External server or self-hosted runner | Temporary local share inside current Windows CI, if proven |

## Common Pitfalls

1. **Process-boundary loss:** narrow argv and fopen use ACP; test a non-ACP character through the actual executable. [VERIFIED: runner/main.c:966-1007] [CITED: https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/fopen-wfopen?view=msvc-170]
2. **Literal suffix mutation:** current comparator trims trailing dots/spaces; include suffix-distinct outputs and true aliases. [VERIFIED: runner/main.c:782-785]
3. **UNC uncertainty:** SMB normalized paths or file IDs may be unavailable; fail closed and require actual UNC acceptance. [CITED: https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfinalpathnamebyhandlew]
4. **PNG truncation:** direct wb open can destroy old bytes; route through atomic writer and add failure-preservation coverage. [VERIFIED: runner/main.c:232-259]
5. **Generated path too long:** ensure measured path includes battery and temp suffixes, with components below the active filesystem limit. [CITED: https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation]

## Code Examples

Wide entry and file open:

    int wmain(int argc, wchar_t *argv[]);
    FILE *stream = _wfopen(path, L"rb");

After writing and closing the sibling temporary, use the W replacement API and current policy:

    MoveFileExW(temporary, destination, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);

The API supports explicit prefixed extended paths; actual failure preservation still requires Windows acceptance. [CITED: https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-movefileexw]

## State of the Art

| Current source pattern | Required phase pattern | Effect |
|------------------------|------------------------|--------|
| Narrow argv and A/CRT narrow paths | Wide argv, storage and W calls | Preserve Unicode and extended path semantics. |
| Trace-only/nonuniform path comparison | Full invocation identity preflight | Keep alias protection across all roles. |
| Direct PNG truncation | Shared sibling-temp atomic writer | Preserve old image on failure. |
| Implied OS maximum | Measured Windows CI boundary | Avoid an unsupported length promise. |

## Assumptions Log

| # | Uncertainty | Risk |
|---|-------------|------|
| A1 | Actual local SMB share create/access/remove behavior on the existing hosted Windows images has not been tested. | UNC evidence stays open and WINPATH-04 cannot be accepted until CI proves it. |
| A2 | Exact longest successful path/component on both Windows lanes, including .sav and temporary suffixes, is unknown. | Documentation must not claim a length beyond measured results. |

## Open Questions

1. **Can the existing Windows x64 and ARM64 lanes use a local SMB share?** GitHub documents administrator capability and Microsoft documents New-SmbShare/Remove-SmbShare, but actual local UNC I/O and cleanup remain unverified. Add a contained setup/read/write/cleanup case; if any step fails, retain UNC as blocked.
2. **What path-length boundary can CI prove?** The OS total ceiling is approximate and component limits are filesystem-specific. Measure the longest successful drive and UNC cases including derived battery/temp names, then document only those values.

## Environment Availability

| Dependency | Required By | Available | Version | Fallback |
|------------|-------------|-----------|---------|----------|
| Windows x64 CI | Actual path behavior | Existing lane configured, not run in research | windows-2025 | None |
| Windows ARM64 CI | Architecture acceptance | Existing lane configured, not run in research | windows-11-arm | None |
| Windows/PowerShell locally | Local share proof | Not available; host is Darwin, pwsh/powershell absent | CMake 4.4.3 present | Use existing Windows CI |
| Hosted local SMB capability | UNC acceptance | Unverified | — | Keep UNC requirement open; no drive-only substitute |

The workflow matrix names windows-2025 x64 and windows-11-arm ARM64. [VERIFIED: .github/workflows/ci.yml:156-161] GitHub says hosted Windows VMs run as administrators with UAC disabled; Microsoft documents New-SmbShare, but this does not prove share use on these images. [CITED: https://docs.github.com/en/actions/reference/runners/github-hosted-runners] [CITED: https://learn.microsoft.com/en-us/powershell/module/smbshare/new-smbshare?view=windowsserver2025-ps]

## Security Domain

Security enforcement is explicitly disabled in project config, but the locked path-identity, fail-closed, exclusive-temp and preservation requirements still apply. [VERIFIED: .planning/config.json:60-61]

## Validation Architecture

Omitted because workflow.nyquist_validation is explicitly false. [VERIFIED: .planning/config.json:32-37]

## Sources

### Primary

- [Microsoft: Naming Files, Paths, and Namespaces](https://learn.microsoft.com/en-us/windows/win32/fileio/naming-a-file)
- [Microsoft: Maximum Path Length Limitation](https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation)
- [Microsoft: Using wmain](https://learn.microsoft.com/en-us/cpp/c-language/using-wmain?view=msvc-170)
- [Microsoft: fopen / _wfopen](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/fopen-wfopen?view=msvc-170)
- [Microsoft: CreateFileW](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilew)
- [Microsoft: GetFileInformationByHandle](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfileinformationbyhandle)
- [Microsoft: FILE_ID_INFO](https://learn.microsoft.com/en-us/windows/win32/api/winbase/ns-winbase-file_id_info)
- [Microsoft: GetFinalPathNameByHandleW](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfinalpathnamebyhandlew)
- [Microsoft: MoveFileExW](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-movefileexw)
- [Microsoft: New-SmbShare](https://learn.microsoft.com/en-us/powershell/module/smbshare/new-smbshare?view=windowsserver2025-ps)
- [Microsoft: GetVolumeInformationW](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getvolumeinformationw)
- [Microsoft: GetVolumePathNameW](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getvolumepathnamew)
- [Microsoft: System Error Codes 0-499](https://learn.microsoft.com/en-us/windows/win32/debug/system-error-codes--0-499-)
- [GitHub: Hosted runners reference](https://docs.github.com/en/actions/reference/runners/github-hosted-runners)
- [CMake: execute_process](https://cmake.org/cmake/help/latest/command/execute_process.html)

### Project sources

- .planning/phases/29-windows-extended-path-runner-support/29-CONTEXT.md
- .planning/REQUIREMENTS.md:18-21
- .planning/preparation/ENGINEERING.md:73-90
- runner/main.c:232-259, 498-530, 692-960, 1288-1315, 1513-1729, 1783-1807, 1843-1867
- tests/runner/test_save.c:302-323, 545-615, 818-900
- tests/runner/trace.cmake:518-565
- tests/runner/tests.cmake:1887-1939; tests/inventory/ci.txt:691-712
- .github/workflows/ci.yml:156-190
- docs/runner.md:252-255

## Implementation Follow-up (2026-11-18)

The first current-milestone Windows run, [44406933777](https://github.com/szTheory/muhtari/actions/runs/44406933777), reached the x64 path process test and failed while asking `GetVolumeInformationW` about the deep test directory (Win32 error 144). Microsoft specifies that this API takes a volume root path with a trailing backslash; the system error table defines 144 as `ERROR_DIR_NOT_ROOT`. Its documentation also says SMB 3.0 does not support volume-management functions. The test now derives the actual local drive root for drive measurements and uses the share's backing local drive to get its filesystem/component limit while still executing the component-boundary operations over the actual UNC share. The run's initial failures remain recorded in `29-ACCEPTANCE.md`; no UNC result is accepted from this correction until the Windows jobs pass.

The same x64 run exposed an allocation-fault classification bug: a failed Windows UTF-8-to-UTF-16 allocation was reported as malformed path usage. Path validation now preserves a third outcome for allocation failure so the runner returns its runtime failure status. The Windows test harness also closed a CRT file descriptor while keeping its owning `FILE *` open to induce a PNG-write failure; the new test seam fails after one real PNG row instead, leaving a partial sibling temporary for the runner's cleanup and preservation behavior to handle. This test-harness diagnosis is inferred from the run log ending during that case; the next Windows CI run must confirm it.

Primary evidence: [Microsoft GetVolumeInformationW](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getvolumeinformationw) requires a volume root with a trailing slash and states that SMB 3.0 does not support volume-management functions; [Microsoft GetVolumePathNameW](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getvolumepathnamew) resolves the mount point containing a local path, including mounted folders; [Microsoft System Error Codes](https://learn.microsoft.com/en-us/windows/win32/debug/system-error-codes--0-499-) identifies 144 as `ERROR_DIR_NOT_ROOT`.

## Metadata

**Confidence breakdown:**
- Standard stack: HIGH — existing project baseline and official APIs checked directly.
- Architecture: MEDIUM — source and platform docs support it; actual UNC behavior remains untested.
- Pitfalls: HIGH — narrow calls, suffix trimming, slash construction and PNG truncation are visible in source.
- Environment: LOW for SMB capability and measured path boundary; neither was executed in this session.

**Research date:** 2026-11-18
**Valid until:** 2026-12-18 for stable API details; recheck hosted image behavior during implementation.
