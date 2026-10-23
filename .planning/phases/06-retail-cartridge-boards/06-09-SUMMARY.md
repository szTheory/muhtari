---
phase: 06-retail-cartridge-boards
plan: "09"
status: pending
subsystem: portability
tags: [equality, diagnostic, quality, provenance, documentation]
requires:
  - phase: 06-08
    provides: Bounded hostile retail execution and deep transaction/isolation acceptance
provides:
  - Locally verified board numeric/frame/audio rows in the existing equality artifact
  - Honest support, portability, geometry and hostile-execution documentation
  - Correct source pins for all three newly admitted owned diagnostics
pending:
  - Current-head six-native package sums and authenticated quality reduction
  - Existing current-head hosted portability and frontend executions
completed-tasks: [1, 2]
local-preparation-complete: [3]
affects: [phase-verification, phase-ship]
tech-stack:
  added: []
  patterns:
    - Append validated diagnostic observations to the existing hashes artifact
    - Reuse the built mailbox judge and existing policy control registration
key-files:
  created: []
  modified:
    - tests/cmake/hashes.cmake
    - tests/runner/tests.cmake
    - tests/policy/quality.cmake
    - tests/policy/tests.cmake
    - .github/workflows/ci.yml
    - .github/workflows/be-equality.yml
    - README.md
    - docs/quality.md
    - docs/compatibility.md
    - tools/compat_doc.cmake
    - .planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md
    - docs/testing.md
    - docs/emulation-tests.md
    - docs/embedding.md
    - tests/roms/manifest.txt
key-decisions:
  - Keep all 17 prior hash rows byte-identical and append ten validated rows for each owned board diagnostic.
  - Retain existing lane membership, advisory statuses, archive ceiling, test-card performance workload and compatibility authority.
  - Correct source pins using exact existing source commits without changing fixture bytes or digests.
requirements-completed: []
requirements-contributed: [BOARD-01, BOARD-02, BOARD-03]
coverage:
  - id: board-equality-producer
    description: Actual runner board identity, numeric mailbox, requested frame hashes and all raw-audio/PCM fields become deterministic named rows
    requirement: BOARD-01
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci; exit 0; 612 non-release and 6 release checks; URL N/A-local"
        status: pass
      - kind: other
        ref: "Prior producer versus current hashes.txt prefix assertion; exit 0; 17 old rows byte-identical and 30 board rows appended; URL N/A-local"
        status: pass
    human_judgment: false
  - id: equality-command-faults
    description: Actual producer helper rejects absent fixtures, wrong board or numeric identity, absent/extra reports and every missing frame/audio field
    requirement: BOARD-02
    verification:
      - kind: integration
        ref: "ctest --preset ci -R '^(runner[.]hashes_file|policy[.]quality[.]controls)$' --no-tests=error --output-on-failure; exit 0 before and after tracer commit; URL N/A-local"
        status: pass
    human_judgment: false
  - id: truthful-docs-and-source-pins
    description: Generated claims distinguish diagnostics from retail/PAL/hardware evidence, preserve descriptor holes, and exact fixture source commits reproduce admitted bytes
    requirement: BOARD-03
    verification:
      - kind: other
        ref: "Agent claim review; full local CI, exact source/config/fixture pin comparison, actual trusted-manifest relation, hygiene and whitespace checks exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: final-hosted-acceptance
    description: All six actual native three-package totals, authenticated worst-lane quality join and existing hosted portability/frontend outcomes still need current-head run evidence
    requirement: BOARD-01
    verification:
      - kind: integration
        ref: "Pending parent execution orchestrator: current-head CI and existing Nightly/full native-PowerPC callers; no run URL yet"
        status: unknown
    human_judgment: false
actuals:
  tokens: 9483
  tasks: 2
  commits: 3
plan_head_before: e9da62e4a313d5d0169ce1bf204c8f278946b617
plan_head_after: 78bb2f6b4a0e82ab428b161bd16f1062e02a42e1
duration: 11min local preparation
prepared: 2026-10-23
---

# Phase 6 Plan 9: Local acceptance prepared; hosted evidence pending

**All local work is committed and verified: 47 deterministic equality rows, strict numeric/report fault controls, truthful documentation and exact fixture source provenance. Task 3 and all BOARD requirements remain incomplete until current-head hosted acceptance.**

## Task Commits and Remaining Work

1. Task 1 complete: `8fd944b` — numeric board evidence through existing hash equality.
2. Task 2 complete: `b27162d` — support claims and evidence limits reconciled.
3. Task 3 local preparation committed: `78bb2f6` — final acceptance docs and fixture source pins. Hosted acceptance remains pending; this is not a completed-task claim.

The persisted ledger measures three local task/preparation commits. Realized task diff is 37,930 characters / 4, rounded up to 9,483 tokens. Summary and state metadata are outside that interval. This pending SUMMARY must not be counted as a completed plan or used to mark requirements complete.

## Local Results

- The producer retains all 17 old card/PNG and soundtest/POKEY rows byte-identically and appends ten rows per board: frame 1, frame 600, seven audio fields and the fixed numeric mailbox. Each actual runner probe must name the expected board. Reports must have exactly ten correctly formed fields, including 64-digit hashes and exactly 512 lowercase mailbox hex digits. The existing built driver validates magic/version/status, ROM identity, count, CRC, record IDs/flags and expected values. Numeric success is never inferred from hashes. Silent diagnostics retain their expected empty raw-transition stream while comparing PCM and sample counts.
- Existing native, full PowerPC and WASM workflows inherit tracked fixture and built-driver paths from `runner.hashes_file`; the emulator prefix applies to runner and judge. No lane, artifact service, required/advisory status, timeout, performance workload or quality bound changed. Workflow changes are explanatory comments only.
- Controls reuse actual passing runner output and the actual producer helper, then remove each frame/audio field, omit/add the numeric report, substitute a wrong board or change the CRC-independent ROM ID. The real mailbox judge rejects the wrong identity. An absent fixture also fails. The existing controls retain their prior semantic cases.
- README stays short. The compatibility page is regenerated through its existing generator and all ledger statuses/predicates/pins remain unchanged. Documentation distinguishes selected map/profile semantics, identified-only AM normalization, original identity, physical 2 KiB Fractalus RAM, partial descriptor visibility, untested retail gameplay and unsupported PAL execution.
- Three new manifest source pins now identify the exact commits containing each source/config/fixture triplet: Activision `fc1b35a`, Absolute `7875422`, Fractalus `bc16579`. Every pinned byte and digest matches current files. All pre-phase manifest rows remain unchanged. The actual additive-manifest helper still yields the original trusted identity against `origin/main`; no candidate digest or ignored identity was introduced.

## Commands, Exits and Run URLs

All evidence here is local; URL is N/A-local. No logs are committed.

| Command | Exit | Result |
|---|---:|---|
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(runner[.]hashes_file\|policy[.]quality[.]controls)$' --no-tests=error --output-on-failure` | 0 | Producer and fault controls pass, including fixture setup |
| Post-commit repeat of the same focused CTest with separate JUnit output | 0 | Tracer feedback passes |
| Previous producer extracted with `git show e9da62e:tests/cmake/hashes.cmake`, run against the same current products; byte-prefix assertion on old/current hashes files | 0 | 17 old rows identical, 30 new rows, 47 total |
| `cmake --workflow --preset ci`, after each local task/preparation slice | 0 final | 612/612 non-release and 6/6 release checks; no old frame/audio predicate change |
| `ctest --preset ci -R '^(docs[.]\|policy[.]compat_doc)' --no-tests=error --output-on-failure` | 0 | Five documentation/generator controls pass |
| `cmake --build --preset ci --target package && ctest --preset ci-release --output-on-failure` | 0 | Exactly three fresh local ZIPs, all six release checks |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | Exactly 612 tests ran once each |
| `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | 0 | Exactly six release tests ran once each |
| `cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake` | 0 | All 19 files within unchanged bounds; testing guide 40,891 <= 40,960 bytes |
| Source-pin comparison below and SHA-256 assertions | 0 | Exact source/config/fixture reproduction for all three pins |
| `cmake -DROOT=. -DTRUSTED=build/ci/quality-actual-base-manifest.txt -P build/ci/check-actual-fixture-comparison.cmake` after extracting trusted manifest with `git show origin/main:tests/roms/manifest.txt` | 0 | Original trusted identity `65d286f79039791d34061710af2d40f254e23de1836aa2dbe93b3bdf96296b7d` preserved |
| `scripts/hygiene.sh --tree` and `git diff --check` | 0 each | Hygiene and whitespace pass; task hooks enabled |
| `git diff e9da62e -- tests/compat/ledger.csv tests/compat/allow.txt tests/perf/allow.txt tests/built/expected.txt tests/audio/expected.txt quality.json` | 0 | Empty: existing authorities, old pins and ceiling unchanged |

```sh
for name in activision absolute fractalus; do
  pin=$(awk -F '\t' -v name="$name" '$1 == "tests/diag/" name ".a78" {print $3}' tests/roms/manifest.txt)
  for ext in s cfg a78; do
    git show "${pin}:tests/diag/${name}.${ext}" | cmp - "tests/diag/${name}.${ext}" || exit 1
  done
done
```

The local comparison script sets `QUALITY_FUNCTIONS_ONLY`, includes the existing quality judge, calls `quality_fixture_identity` on current/trusted manifests, and requires its result equal the original trusted file's SHA-256. This is a local check of the actual helper inputs, not an authentic hosted event/base run.

```sh
set -- build/ci/muhtari-*.zip
test "$#" -eq 3 || exit 1
archive_bytes=0
for archive do
  archive_size=$(wc -c < "$archive")
  printf '%s %s bytes\n' "$archive" "$archive_size"
  archive_bytes=$((archive_bytes + archive_size))
done
test "$archive_bytes" -le 399937
```

| Component, local macOS arm64 | Task 1 | Task 2 | Task 3 preparation |
|---|---:|---:|---:|
| library | 68,753 | 68,753 | 68,753 |
| libretro | 68,092 | 68,092 | 68,092 |
| runner | 77,286 | 77,286 | 77,286 |
| Total | 214,131 | 214,131 | 214,131 |

## Deviations and Issues

- **[Rule 2 - Required control plumbing]** Added actual built runner/driver and emulator arguments to the existing `tests/policy/tests.cmake` control registration. This prevents guessing platform-specific binary paths; no new registration or framework was added.
- **[Rule 2 - Generated documentation]** Updated the existing `tools/compat_doc.cmake` prose template and regenerated its page, preserving ledger contents and the existing stale-page negative control.
- **[Rule 1 - Provenance]** Corrected the three newly admitted fixture pins from predecessor commits without their source to the exact already-existing source commits. Bytes/digests and every pre-phase row persist; actual trusted-manifest comparison passes. Recorded as fixed WINDOWS41.
- Final documentation first exceeded the existing 40 KiB testing-page bound (workflow exit 8). A first link-only reduction still exceeded it by 134 bytes. Consolidated the existing introduction without losing its prerequisites, steps or no-test configuration semantics; final size 40,891 bytes, all guards and final full CI pass. No bound was raised. The local `-DROOT=.` budget probe discovered only two root files; the authoritative absolute-root probe and CTest checked all 19 files.
- Git metadata writes required normal sandbox escalation; hook-bearing commits succeeded. No user action, authentication, remote dispatch or new GSD step was performed.

## Required Parent Checkpoint: Current-Head Hosted Evidence

Local code/preparation commit: `78bb2f6b4a0e82ab428b161bd16f1062e02a42e1`. The parent should use the final checkpoint metadata head with the same product/test code for its normal existing hosted acceptance. No applicable run URL exists in this executor's evidence yet.

1. Existing CI: all six native producers (`linux-x64`, `linux-arm64`, `macos-x64`, `macos-arm64`, `windows-x64`, `windows-arm64`) must complete their exact ordinary/release inventories and supply exactly three fresh ZIPs per lane. Record individual sums and authentic current/base observations, and require the existing quality aggregate's worst archive value <=399937.
2. Collect exact built-head SHA, actual event-base SHA, source run URL, raw base policy, compiler/platform/fixture identities, all six native plus hygiene/ASan observations, quality join and `CI required`. Direct PR CI additionally requires the actual armed test-card performance artifact. Performance and compatibility authority remain unchanged.
3. Existing native hash equality must compare the complete expanded artifacts. Existing full native/PowerPC equality callers are `.github/workflows/be-equality.yml`, used by Nightly and Release. Existing Nightly supplies sanitizer fuzz (60 s per target), the 10,000-case advisory property execution and full BE path. WASM equality stays advisory and runs through existing CI; two nofp architectures and existing RetroArch cases retain their statuses.
4. The pending hosted-retirement todo remains open. At local inspection `origin/main` is `617dfa762ddb649839b925be5aa582180066fa78` and its raw policy still contains legacy `gate_lines`. A PR against that actual base can demonstrate legacy-base retirement. A dispatch uses head's first parent and may demonstrate only migrated-base comparison; do not describe that as authentic legacy migration. Preserve raw base bytes and verify that only the retired key may disappear while ordinary typed/identity/reason guards still apply.

Normal existing entry points, for the parent to select within its authorization: a normal implementation PR runs CI and the path-triggered Nightly suite; `gh workflow run ci.yml --ref phase/06-retail-cartridge-boards` and `gh workflow run nightly.yml --ref phase/06-retail-cartridge-boards` are the existing dispatch callers. `gh run list --workflow ci.yml --commit <actual-head>` and `gh run view <id>` collect run evidence. This executor has not pushed, opened a PR, merged or dispatched any workflow. Do not create a new service or lane to satisfy the checkpoint.

WINDOWS42 records this unrun acceptance; WINDOWS33 retains the earlier pending hosted dimension. Maximum two code-review fix rounds and one gap round persist. Optional tooling improvements remain seeds. Spec-less BOARD edges and descriptorless prohibitions keep their unclassified/unresolved/verification-null metadata; concrete local checks do not fabricate a descriptor or owner judgment. No BOARD requirement or phase completion is claimed.

## Self-Check: LOCAL PASSED; HOSTED PENDING

All task/preparation files exist and the three local commits are ancestors of HEAD. Final local full CI, release checks, exact inventories, actual three-ZIP sum, source provenance, trusted manifest relation and hygiene pass. The parent must obtain current-head hosted evidence, resume Task 3, then finalize this SUMMARY and phase gates. Pending status must remain visible until then.
