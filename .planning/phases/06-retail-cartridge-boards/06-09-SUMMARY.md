---
phase: 06-retail-cartridge-boards
plan: "09"
status: complete
subsystem: portability
tags: [equality, diagnostic, quality, provenance, documentation]
requires:
  - phase: 06-08
    provides: Bounded hostile retail execution and deep transaction/isolation acceptance
provides:
  - Hosted native/PowerPC verified board numeric/frame/audio rows in the existing equality artifact
  - Honest support, portability, geometry and hostile-execution documentation
  - Correct source pins for all three newly admitted owned diagnostics
  - Six current-head three-ZIP sums and authenticated aggregate within unchanged399937
completed-tasks: [1, 2, 3]
outside-plan-follow-ups:
  - Direct PR CI and outer Release equality/rehearsal remain pending at the acceptance snapshot
  - Parent phase verification and ship acceptance remain separate
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
requirements-completed: [BOARD-01, BOARD-02, BOARD-03]
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
    description: All six current-head native three-package totals and authenticated worst-lane quality fit; existing native, PowerPC, ARM, WASM and frontend paths retain their declared status
    requirement: BOARD-01
    verification:
      - kind: integration
        ref: "Current-head called CI all six native/release producers, ASan, hygiene, hash equality, WASM, RetroArch and CI required success; unchanged downloaded-artifact quality_join exit 0, current375263/base399371 <=399937; https://github.com/szTheory/muhtari/actions/runs/40031550534"
        status: pass
      - kind: integration
        ref: "Four exact native/PowerPC inventory commands612+6 each and lane_equality exit 0,47 identical rows; Nightly source25919fe/PR merge2fe713c success; https://github.com/szTheory/muhtari/actions/runs/40031550533"
        status: pass
    human_judgment: false
  - id: absolute-hosted-portability
    description: Absolute numeric and frame/audio observations survive all required hosted equality and bounded execution paths with unchanged bank semantics
    requirement: BOARD-02
    verification:
      - kind: integration
        ref: "Nightly full native/PowerPC equality612+6, both60-second fuzz engines and10000-case properties success; unchanged lane_equality exit 0; https://github.com/szTheory/muhtari/actions/runs/40031550533"
        status: pass
    human_judgment: false
  - id: fractalus-hosted-portability
    description: Physical Fractalus alias and bounded-access evidence participates in existing hosted numeric, frame/audio, sanitizer and replay acceptance without a retail gameplay or PAL claim
    requirement: BOARD-03
    verification:
      - kind: integration
        ref: "Current-head ASan and six native producers success; https://github.com/szTheory/muhtari/actions/runs/40031550534; Nightly equality/fuzz/property success; https://github.com/szTheory/muhtari/actions/runs/40031550533"
        status: pass
    human_judgment: false
  - id: authenticated-sample-annotation-comparison
    description: Unique exact behavior-annotation migration preserves every other sample-file byte and the independently read raw event-base identity
    requirement: BOARD-01
    verification:
      - kind: integration
        ref: "policy.quality.controls real producer/unchanged join cases; full local CI 612+6 and hygiene24; exit 0 each; URL N/A-local"
        status: pass
      - kind: integration
        ref: "Current-script BUILD_BASE=ON against actual617dfa762ddb649839b925be5aa582180066fa78; base610+6, equal environments and raw legacy policy; exit 0; SOURCE=local, URL N/A-local"
        status: pass
    human_judgment: false
actuals:
  tokens: 26860
  tasks: 3
  commits: 20
plan_head_before: e9da62e60e1e1a52d08f05c228c4dbf8284eb332
plan_head_after: 25919fe822cd6cc23a6984c13ac826796e7d9cfe
duration: 113min elapsed from first task commit including hosted waits
prepared: 2026-10-23
completed: 2026-10-23
---

# Phase 6 Plan 9: Board portability and six-native package acceptance

**All three tasks are accepted: 47 deterministic equality rows, strict numeric/report controls, truthful documentation, exact source provenance and authenticated six-native packages with worst sum375263 <=399937. BOARD-01–03 are complete; the parent still owns phase verification and outstanding outer workflow acceptance.**

Final acceptance below supersedes the earlier pending checkpoint snapshots. Failed runs and their repair evidence remain historical records. Passing named called-CI jobs and Nightly satisfy Task3; this summary does not claim the pending direct CI or outer Release equality/rehearsal is green.

## Task Commits

1. Task 1 complete: `8fd944b` — numeric board evidence through existing hash equality.
2. Task 2 complete: `b27162d` — support claims and evidence limits reconciled.
3. Task 3 complete: `78bb2f6` — final acceptance docs and fixture source pins; hosted integration corrections through source head `25919fe` and the final acceptance metadata commit complete the independently measured package/portability contract.

The persisted ledger measures20 commits from `e9da62e60e1e1a52d08f05c228c4dbf8284eb332` through accepted source head `25919fe822cd6cc23a6984c13ac826796e7d9cfe`, before this final metadata commit. Realized implementation/test/workflow/product-documentation diff is107,439 characters /4, rounded up to26,860 tokens across34 files; execution summaries/state/review metadata are excluded from that diff. The original local-preparation-only interval was37,930 characters and three task commits. Final counters include the subsequent hosted integration corrections and their commits; no harness-token measure or global metric is substituted.

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

## Historical Parent Checkpoint Before Hosted Acceptance

Local code/preparation commit: `78bb2f6b4a0e82ab428b161bd16f1062e02a42e1`. The parent should use the final checkpoint metadata head with the same product/test code for its normal existing hosted acceptance. No applicable run URL exists in this executor's evidence yet.

1. Existing CI: all six native producers (`linux-x64`, `linux-arm64`, `macos-x64`, `macos-arm64`, `windows-x64`, `windows-arm64`) must complete their exact ordinary/release inventories and supply exactly three fresh ZIPs per lane. Record individual sums and authentic current/base observations, and require the existing quality aggregate's worst archive value <=399937.
2. Collect exact built-head SHA, actual event-base SHA, source run URL, raw base policy, compiler/platform/fixture identities, all six native plus hygiene/ASan observations, quality join and `CI required`. Direct PR CI additionally requires the actual armed test-card performance artifact. Performance and compatibility authority remain unchanged.
3. Existing native hash equality must compare the complete expanded artifacts. Existing full native/PowerPC equality callers are `.github/workflows/be-equality.yml`, used by Nightly and Release. Existing Nightly supplies sanitizer fuzz (60 s per target), the 10,000-case advisory property execution and full BE path. WASM equality stays advisory and runs through existing CI; two nofp architectures and existing RetroArch cases retain their statuses.
4. The pending hosted-retirement todo remains open. At local inspection `origin/main` is `617dfa762ddb649839b925be5aa582180066fa78` and its raw policy still contains legacy `gate_lines`. A PR against that actual base can demonstrate legacy-base retirement. A dispatch uses head's first parent and may demonstrate only migrated-base comparison; do not describe that as authentic legacy migration. Preserve raw base bytes and verify that only the retired key may disappear while ordinary typed/identity/reason guards still apply.

Normal existing entry points, for the parent to select within its authorization: a normal implementation PR runs CI and the path-triggered Nightly suite; `gh workflow run ci.yml --ref phase/06-retail-cartridge-boards` and `gh workflow run nightly.yml --ref phase/06-retail-cartridge-boards` are the existing dispatch callers. `gh run list --workflow ci.yml --commit <actual-head>` and `gh run view <id>` collect run evidence. This executor has not pushed, opened a PR, merged or dispatched any workflow. Do not create a new service or lane to satisfy the checkpoint.

WINDOWS42 records this unrun acceptance; WINDOWS33 retains the earlier pending hosted dimension. Maximum two code-review fix rounds and one gap round persist. Optional tooling improvements remain seeds. Spec-less BOARD edges and descriptorless prohibitions keep their unclassified/unresolved/verification-null metadata; concrete local checks do not fabricate a descriptor or owner judgment. No BOARD requirement or phase completion is claimed.

## Historical Local Checkpoint Self-Check

All task/preparation files exist and the three local commits are ancestors of HEAD. Final local full CI, release checks, exact inventories, actual three-ZIP sum, source provenance, trusted manifest relation and hygiene pass. The parent must obtain current-head hosted evidence, resume Task 3, then finalize this SUMMARY and phase gates. Pending status must remain visible until then.

## Hosted Failure Repair: Authenticated Sample Annotation Comparison

Fix commit `7f90077300fbef07586fef524d2b165cdb3e03e3` addresses the current/base environment mismatch reproduced in the hosted macOS/ASan producer logs. The actual legacy event base is `617dfa762ddb649839b925be5aa582180066fa78`. Its built-sample expectation file differs from the candidate only at the required `# behaviour_revision 2` to `# behaviour_revision 3` annotation; every row, tool/source pin, frame/audio hash, comment and trailing byte otherwise matches. Raw digest equality made this authorized annotation change incomparable even after fixture-manifest comparability succeeded.

The current evidence script now independently obtains the original event-base `tests/built/expected.txt` using `git show` in both current and recursive base invocations. The existing quality module owns one narrow helper: unchanged files retain raw equality; a difference must be exactly the unique complete revision-two to revision-three annotation line, with every other byte equal. The resulting environment identity is always the raw trusted file's SHA-256. Caller-provided sample paths/digests cannot replace it. No sample bytes, ordinary bound, base-policy bytes, compiler/platform identity, manifest rule or join equality changed. This is an existing evidence-contract repair, not a general ignored-comment mechanism or baseline reset.

The existing `policy.quality.controls` producer/unchanged-join suite now exercises revision-only acceptance, migrated-base equality, reverse/invalid revisions, missing/duplicate annotations, changed rows/pins/trailer and forged candidate trust arguments. The older raw-sample mutation case now fails at the producer boundary before joining, while compiler/platform mismatches still reach and fail the unchanged join. Fixture-admission, legacy-key retirement, typed evidence and dated-reason controls remain active. All cases execute through the current producer and the existing join without new registration or tooling.

| Command | Exit | Result | Run URL |
|---|---:|---|---|
| `ctest --preset ci -R '^policy[.]quality[.]controls$' --output-junit quality-sample-controls.xml --no-tests=error --output-on-failure` | 0 | Extended producer/join controls pass | N/A-local |
| `cmake --workflow --preset ci` | 0 | 612 ordinary and six release checks; exact native inventories pass | N/A-local |
| `cmake --workflow --preset hygiene` | 0 | 24/24 including format, analyzer, no-float and fixture coverage; exact hygiene inventory passes | N/A-local |
| Three-component ZIP expansion and `wc -c` sum using the command above | 0 | 68,753 + 68,092 + 77,286 = 214,131 <=399,937 | N/A-local |
| Actual-base producer invocation below | 0 | Independent real legacy-base build 610+6; current/base environment equality established | N/A-local |
| Read-only JSON/Git assertions on actual producer outputs | 0 | Current/base native tests 612/610; archive bytes214131/212228; original base policy and SHA exact, legacy key retained | N/A-local |
| Clean exact-base worktree check followed by `git worktree remove build/ci/quality-base` | 0 | Disposable test worktree removed; no other worktree touched | N/A-local |

```sh
GITHUB_EVENT_PATH="$PWD/build/ci/sample-event.json" \
GITHUB_EVENT_NAME=pull_request GITHUB_SHA="$(git rev-parse HEAD)" \
cmake -DROOT=. -DBUILD=build/ci -DOUT=build/ci/quality-current-local.json \
  -DSHA="$(git rev-parse HEAD)" -DCONFIG=ci -DSOURCE=local -DBUILD_BASE=ON \
  -P tests/cmake/quality_evidence.cmake
```

The local event file contains only `pull_request.base.sha` set to the exact legacy SHA above. Its identity comes from that actual Git commit, not a synthetic migration file or candidate-derived digest. Current and recursive base environments both end with the independently authenticated raw sample digest `13934a6aa791c06d4e50c165bed766b0facc8d32eff4e3d595b3a880dc57e2f2`; the original trusted manifest digest remains `65d286f79039791d34061710af2d40f254e23de1836aa2dbe93b3bdf96296b7d`. Compiler/platform identity remains `AppleClang-21.0.0.21000101-Darwin-arm64`. The raw base-policy file is byte-identical to `git show` of the legacy commit and retains its `gate_lines` key.

This reproduction uses `SOURCE=local` and does not claim GitHub event authentication, six-platform fit or a successful hosted aggregate. The parent must rerun existing hosted acceptance at the fix head; Task 3, WINDOWS42, the hosted-retirement todo and all BOARD requirements remain pending. The initial `actuals` interval above describes initial local preparation; final plan actuals must be refreshed when Task 3 closes. No review/gap allowance was reset.

## Hosted correction round: compiler and hygiene

At parent PR24 head `85e12ca9013638c7aafbe96d55636fc74d32781a`, the first actual hosted acceptance exposed two source-integration faults. Read-only `rg -n 'error: declaration|The following tests FAILED|no_float:' /tmp/muhtari-p6-hygiene.log /tmp/muhtari-p6-linux.log /tmp/muhtari-p6-prop.log` exited0 and extracted failed GNU compilation and hygiene from the parent-supplied hosted logs; the affected Nightly property compile likewise failed in run40023530478. Sources: [CI40023530702](https://github.com/szTheory/muhtari/actions/runs/40023530702), [Nightly40023530478](https://github.com/szTheory/muhtari/actions/runs/40023530478). These failed runs are evidence of the fault, not acceptance.

- The dynamic diagnostic entry pointer named `run` shadowed the existing static diagnostic function under GNU `-Werror=shadow`. The loader macro now separates its local pointer name `module_run` from the unchanged actual symbol `retro_run`; the Win32 and POSIX loading paths retain the same symbol and behavior.
- The unchanged floating-point text guard rejected plural decimal section citations in three core comments. Those comments now cite the preparation's named sections, retaining source attribution. Clang-format21 corrected the affected C/H files, including the host warning-name table; every value, expectation, selector and resource bound is retained. The guard and formatter configuration were not changed.

| Final-tree verification command | Exit | Evidence / run URL |
|---|---:|---|
| `cmake --workflow --preset hygiene` | 0 | Actual clang-format21, clang-tidy analyzer, no-float pass/fault controls and all other hygiene checks:24/24; N/A-local |
| `cmake --workflow --preset ci` | 0 | 612 non-release plus six release tests, actual module/numeric paths and three fresh packages; N/A-local |
| `cmake -DJUNIT=build/hygiene/junit.xml -DINVENTORY=tests/inventory/hygiene.txt -P tests/cmake/check_junit.cmake` | 0 | Exactly24 hygiene executions; N/A-local |
| Both ordinary/release `check_junit.cmake` commands above | 0 each | Exactly612 and6 executions; N/A-local |
| Exactly-three ZIP expansion and `wc -c` sum above | 0 | Library68,754 + libretro68,092 + runner77,286 =214,132 <=399,937; N/A-local |
| `git diff --check` | 0 | No whitespace errors; N/A-local |

The first local hygiene correction run passed23/24 and identified the additional host-table formatting issue; the final complete24/24 rerun includes that correction. A full CI run after the final formatting change passed. `gcc --version` returns Apple Clang21 here, and no versioned GNU compiler was found in PATH/Homebrew; no local GNU proof is claimed. Parent owns push/retry and actual current-head GNU, native/BE, release and authenticated quality acceptance. WINDOWS43/44 record the corrected source faults as fixed; WINDOWS42 and the pending hosted acceptance above remain open. This correction does not complete Task3, a BOARD requirement or the phase, and does not start another GSD step.

## Hosted Failure Repair: Quality Control Setup Cost

Code commit `334abf0` addresses the measured macOS x64 timeout in [CI40026612824, job120664334192](https://github.com/szTheory/muhtari/actions/runs/40026612824/job/120664334192) and [called CI40026612929, job120664334359](https://github.com/szTheory/muhtari/actions/runs/40026612929/job/120664334359). The parent-supplied log shows `policy.quality.controls` exceeding its 30-second limit at 30.16 seconds after completing the older controls, fixture cases and sample/compiler cases; the other 611 ordinary tests passed. Those failed runs are fault evidence, not hosted acceptance.

**[Rule 1 - Test performance]** The existing producer loop repeatedly removed and configured the entire current mock build even for inputs rejected before recursion. It now builds invariant current outputs once and removes only its owned base-worktree directory before each actual recursive producer. Compiler/platform mutations are restored after their evidence is captured. Every case list is byte-identical; every valid producer still performs the independent base build and its required unchanged join. All source/fixture comparability, sample relation, raw-policy retirement, typed evidence and reason controls remain. The registration and 30-second timeout are unchanged; no assertion, lane, framework or resource limit was removed or raised.

| Command | Exit | Result | Run URL |
|---|---:|---|---|
| `/usr/bin/time -p ctest --preset ci -R '^policy[.]quality[.]controls$' --output-junit quality-controls-before.xml --no-tests=error --output-on-failure` before this change | 0 | Test6.62s, CTest6.63s, wall6.64s, user2.84s, sys3.02s | N/A-local |
| Same command after this change, with `--output-junit quality-controls-after.xml` | 0 | Test4.17s, CTest4.18s, wall4.19s, user2.00s, sys1.79s; matched wall reduction about37% | N/A-local |
| `cmake --workflow --preset ci` on the finished test code | 0 | 612 ordinary and six release checks; quality controls also pass during the concurrent full suite | N/A-local |
| `cmake --workflow --preset hygiene` | 0 | 24/24, including no-float pass/fault, analyzer, formatting and fixture coverage | N/A-local |
| All three `check_junit.cmake` inventory commands above | 0 each | Exactly612 ordinary, six release and24 hygiene tests ran once each | N/A-local |
| Exactly-three ZIP expansion and `wc -c` sum above | 0 | Library68,754 + libretro68,092 + runner77,286 =214,132 <=399,937 | N/A-local |
| Case-list comparison against pre-fix HEAD; `git diff --check` | 0 each | All foreach case lists byte-identical; no whitespace errors | N/A-local |

WINDOWS46 records the local setup fix as fixed. The measured improvement is local; current-head hosted timing and authenticated aggregate acceptance still require the parent's normal retry. WINDOWS42, Task3, the hosted-retirement todo and all BOARD requirements remain pending. No review/gap allowance was reset, and the parent's uncommitted review report was preserved.

## Hosted Failure Repair: Quality Fault Runner Startup

Code commit `12281af` addresses the further measured Windows ARM timeout in [called CI40029174245, job120672888533](https://github.com/szTheory/muhtari/actions/runs/40029174245/job/120672888533). The parent-supplied log reports `policy.quality.controls` exceeding its unchanged30-second limit at30.01 seconds. This failed run is fault evidence, not acceptance.

**[Rule 1 - Test performance]** Each expected failure previously started a wrapper CMake process before executing the real command. A small scoped function now includes the existing, unchanged `expect_fail.cmake`. The same runner executes the real command directly, merges stdout/stderr, requires exactly exit1 and checks the same regex. Its argument validation and failure messages are retained. Function scope isolates commands and results. Every case list, real producer recursion, raw-policy/sample/fixture control and required join assertion remains; the registration and30-second limit are unchanged. This reuses the existing mechanism rather than adding a framework or accepting arbitrary failures.

| Command | Exit | Result | Run URL |
|---|---:|---|---|
| `/usr/bin/time -p ctest --preset ci -R '^policy[.]quality[.]controls$' --output-junit quality-inline-before.xml --no-tests=error --output-on-failure` before this change | 0 | Test3.92s, CTest3.93s, wall3.95s, user1.92s, sys1.61s | N/A-local |
| Same command after this change, with `--output-junit quality-inline-after.xml` | 0 | Test3.67s, CTest3.68s, wall3.68s, user1.78s, sys1.54s; matched wall reduction about7% | N/A-local |
| `cmake -P build/ci/quality-inline-check/exact_failure.cmake` | 0 | Actual helper extracted from current source accepts exact exit1 and matching message | N/A-local |
| `cmake -P build/ci/quality-inline-check/wrong_exit.cmake` and `cmake -P build/ci/quality-inline-check/wrong_regex.cmake` | 1 each, expected | Same extracted helper rejects exit0 despite matching output, and exit1 with a wrong message; outer one-off Python assertions exit0 | N/A-local |
| `cmake --workflow --preset ci` | 0 | 612 ordinary and six release checks; quality controls5.25s during the full concurrent suite | N/A-local |
| `cmake --workflow --preset hygiene` | 0 | 24/24 including existing analyzer, formatting and no-float controls | N/A-local |
| All three `check_junit.cmake` inventory commands above | 0 each | Exactly612 ordinary, six release and24 hygiene tests ran once each | N/A-local |
| Exactly-three ZIP expansion and `wc -c` sum above | 0 | Library68,589 + libretro68,051 + runner77,259 =213,899 <=399,937 | N/A-local |
| Case-list comparison against pre-fix HEAD; `git diff --check` | 0 each | All case lists byte-identical; no whitespace errors | N/A-local |

These workflows included the parent's pending archive compression level9 and top-level MSVC Release size-optimization settings; this executor did not stage or alter those files or the review report. The measured packages are local macOS arm64 artifacts and do not establish Windows fit. WINDOWS49 records the local control startup fix as fixed; current-head hosted Windows timing/package fit, authenticated quality and remaining portability acceptance remain pending with the parent. Task3 and all BOARD requirements remain pending; no review/gap allowance or resource ceiling changed.

## Hosted Failure Repair: Pinned Tool Compilation Cost

Code commit `782531b` addresses concrete tool-startup cost after [CI40026612824, Windows x64 job120664334130](https://github.com/szTheory/muhtari/actions/runs/40026612824/job/120664334130) reported `built.samples.self_test` timing out at4.06 seconds against4, and `diag.rebuild` at22.02 against22 after only four fixtures. Read-only `rg -n 'built.samples.self_test|diag.rebuild|Timeout|timeout' /tmp/muhtari-p6-retry-win.log` exited0 and confirmed those fault observations. The run is failure evidence; this local reduction does not establish a successful Windows retry.

**[Rule 1 - Test performance]** Both existing recipes continue running every source/preprocess/compile/assemble/sign/header stage in fresh output directories. They now use the pinned Wasmtime48 compiled-module cache explicitly at `build/pins/wasmtime-cache`, shared across the sample and diagnostic processes. The tool's content/configuration keys manage compiled code; no fixture output, fetched suite or source result is cached, and no home/global cache is selected. A plain enable flag would use the default cache location, so each recipe supplies the small explicit [Wasmtime cache configuration](https://docs.wasmtime.dev/cli-cache.html). Per-output/name configurations sit outside fresh fixture directories and `file(CONFIGURE)` retains identical files without rewriting them; parallel fixture names do not share a configuration pathname. No framework, dependency, CI lane, timeout, tool pin, manifest row, numeric expectation or package bound changes.

| Observation, local macOS arm64 | Result |
|---|---|
| Existing focused three-test batch before the change, two runs | Wall2.230s and2.107s; exact rebuild and both digest controls passed |
| First explicit-cache focused batch | Wall2.08s; all three tests passed |
| Warm focused batch, three runs | Wall2.051s,2.089s,2.142s; all three passed; no whole-batch improvement is established beyond overlapping local variation |
| Actual `dasm.wasm activision.s -f3 -ocache-probe.bin`, newly empty explicit cache |21.7ms, exit0 |
| Same tool/arguments/source with warm cache |8.2ms,8.2ms,7.8ms, exit0 each; about62% lower than the measured cold call |
| Explicit caching disabled, five actual assembler runs | Median17.0ms versus warm-cache median7.8ms in the matched five-run comparison |
| Cold and warm raw assembly SHA-256 | All equal `42adcdd36f2defefb59a4b7bf4aead5ab9a3e3b97d896542436d0aca0c3086a4` |
| Configuration timestamps across three repeated parallel batches | Unchanged for sample and Activision configs; pass |

The startup measurements used a read-only Python timing harness around the actual pinned `wasmtime run -C cache=y -C cache-config=<explicit build config> --dir=. --dir=<package>::/b <package>/dasm.wasm activision.s -f3 -ocache-probe.bin` command in the existing disposable Activision rebuild directory. The cold case created an empty cache directory under the ignored build tree; each child and the measurement harness exited0. The disabled comparison used `-C cache=n`; source, tool and assembly output remained the same. These local tool-step timings do not predict Windows process/filesystem costs or prove that either hosted timeout now fits.

| Final verification command | Exit | Result | Run URL |
|---|---:|---|---|
| `ctest --preset ci -R '^(built[.]samples[.]self_test\|diag[.]rebuild\|diag[.]rebuild[.]self_test)$' --no-tests=error --output-on-failure`, cold and repeated warm executions | 0 each | Exact diagnostic byte rebuild plus unchanged sample-digest and manifest-drift controls execute concurrently | N/A-local |
| Disposable changed-source probe through actual `tests/diag/assemble.cmake` with the warm compiled-module cache | Child1, harness0 | Replacing the owned diagnostic's first `$A000` read with `$C000` produces the exact expected `tests/diag/activision.a78: SHA-256 drift`; cached compiled code cannot hide changed source output | N/A-local |
| `cmake --workflow --preset ci` | 0 | 612 ordinary plus six release checks and three fresh packages | N/A-local |
| `cmake --workflow --preset hygiene` | 0 |24/24, including formatting, analyzer, no-float and fixture coverage | N/A-local |
| All three `check_junit.cmake` inventory commands above | 0 each | Exactly612 ordinary, six release and24 hygiene executions | N/A-local |
| Exactly-three ZIP expansion and `wc -c` sum above | 0 | Library68,754 + libretro68,092 + runner77,286 =214,132 <=399,937 | N/A-local |
| `git diff --check` | 0 | No whitespace errors | N/A-local |

WINDOWS47 records the locally implemented startup reduction as fixed. Actual current-head Windows timeouts and the broader authenticated hosted acceptance remain pending under WINDOWS42/Task3; the parent owns push and retry. The pre-existing dirty review report was preserved. No BOARD requirement or phase completion, new owner UAT, remote dispatch or GSD step is claimed.

## Hosted newline and emulation corrections

Run40026612824 exposed Windows mailbox files written with native CRLF by CMake. Commit c63697b explicitly writes the three temporary report files with Unix newlines; the strict byte parser remains unchanged. Run40026612938 exposed the AM/OM subprocess reaching its bare90-second limit on PowerPC. Its command now inherits the existing MUHTARI_TIMEOUT_SCALE, with native90 and registered outer120 unchanged. No frame count, comparison or native timeout is relaxed. WINDOWS48 tracks hosted confirmation.

Fresh commands after these corrections: `cmake --workflow --preset ci`, exit0,612 ordinary plus6 release; `cmake --workflow --preset hygiene`, exit0,24; exactly-three ZIP inspection, exit0,68754+68092+77286=214132<=399937. URLs N/A-local. Focused resolver and report/hash/control tests passed3/3 and8/8, exit0. Hosted URLs: https://github.com/szTheory/muhtari/actions/runs/40026612824 and https://github.com/szTheory/muhtari/actions/runs/40026612938.

The initial Nightly40023530478 fuzz failure was a harness construction error, reproduced by an accidental A78 prefix in a ten-byte linear-builder input. Commit abde451 disarms that prefix only for linear output and adds generated seed29. Fuzz and property replay checks10/10 pass locally; actual60-second fuzz and10000-case properties pass in Nightly40026612938, exit0. Local time-based LLVM sanitizer startup remained unavailable (timeouts, then interrupted), so it supplies no passing fuzz evidence. MSVC signed-status comparison was corrected in f3cf0fd. No core behavior or hostile-input bound was weakened.

## Measured package overrun and candidate reduction

Called CI40029174245 at f74bed5 produced Windows x64 ZIPs98765+142627+161194=402586, above399937. This is a blocking failed observation, not a byte grant. Content-identical level9 recompression of those actual archives gave400949, still above the bound, so compression alone did not prove feasibility.

The candidate uses CPack's supported ZIP compression level9 and top-level Release MSVC /Os after its default optimization flags. The latter favors smaller instruction sequences; it changes neither the runtime selection nor ABI, source behavior, packaged contents or archive format. Embedders retain their build policy. Platform performance and timing remain subject to their existing checks; no Windows speed improvement is claimed. Sources: installed CMake4.3+ cpack-generators manual, CPACK_ARCHIVE_COMPRESSION_LEVEL; https://learn.microsoft.com/en-us/cpp/build/reference/os-ot-favor-small-code-favor-fast-code .

Fresh full local CI612+6, hygiene24 and exact inventories passed, exit0. Exactly-three local ZIPs68589+68051+77259=213899 pass399937, exit0, URL N/A-local. This verifies local content and packaging only. Windows size and all final six-lane maxima remain pending hosted measurement. The earlier exact-head Nightly40029174270 passed full native/BE equality, fuzz, vectors and properties; it does not establish fit for this new code-generation setting.

Direct CI40029174334 also saw isolated audio.frames/native Mac and immutable-base built.samples.self_test/Windows timeouts. The called job passed those same base/source dimensions. No legacy base file or native timeout was changed to erase those failed observations.

## Current hosted acceptance evidence

At source head25919fe822cd6cc23a6984c13ac826796e7d9cfe, called CI in https://github.com/szTheory/muhtari/actions/runs/40031550534 has passing six native builds, hygiene, ASan, native hash equality, WASM, RetroArch and CI-required results. Download commands `gh run download 40031550534 --pattern 'quality-*' --dir build/phase6-acceptance/complete-called` and the analogous `archives-*` download exited0. Each lane contains exactly three ZIPs; direct file-size sums equal its original observation:

| Lane | Library | Libretro | Runner | Sum |
|---|---:|---:|---:|---:|
| Linux arm64 | 83300 | 86562 | 96249 | 266111 |
| Linux x64 | 82348 | 85048 | 94988 | 262384 |
| macOS arm64 | 69158 | 68376 | 78241 | 215775 |
| macOS x64 | 70946 | 68771 | 78430 | 218147 |
| Windows arm64 | 85245 | 126072 | 139977 | 351294 |
| Windows x64 | 85785 | 136809 | 152669 | 375263 |

`cmake -DDIRECTORY=build/phase6-acceptance/complete-called -DOUT=build/phase6-acceptance/called-join -DPOLICY=quality.json -DBASE_POLICY=build/phase6-acceptance/trusted-quality.json -DEXPECTED_SHA=25919fe822cd6cc23a6984c13ac826796e7d9cfe -DBASE_SHA=617dfa762ddb649839b925be5aa582180066fa78 -DPERF_REQUIRED=OFF -DSOURCE=https://github.com/szTheory/muhtari/actions/runs/40031550534 -P tests/cmake/quality_join.cmake` exited0. The trusted policy was extracted with `git show` from that exact base. All eight environments match their independent base; all raw base-policy bytes retain the legacy key unchanged, while current observations omit it. Worst current archive375263 and worst base399371 both pass399937. Compiler/platform, all ordinary required metrics and independent performance/compatibility guards retain authority. Called CI intentionally does not request perf.

Nightly https://github.com/szTheory/muhtari/actions/runs/40031550533 passed at PR merge2fe713c1074a63bbf89384d56200bd48060c0549, whose PR API identifies source head25919fe and base617dfa7. Four `check_junit.cmake` invocations over downloaded full native/PowerPC normal/release XML exited0, exactly612+6 each. `cmake -DNATIVE=build/phase6-acceptance/accepted-equality/evidence/native -DBE=build/phase6-acceptance/accepted-equality/evidence/be -DEXPECTED_SHA=2fe713c1074a63bbf89384d56200bd48060c0549 -P tests/cmake/lane_equality.cmake` exited0, two identical hash files. The run also passed both60-second fuzz engines,10000-case properties, full vectors and benches.

Final direct CI40031550434 remains pending ARM64/base and its downstream aggregate/frontend jobs. Release40031550534 equality attempt1 spent roughly23 minutes installing tools and had only begun native tests when cancelled because insufficient budget remained for full equality; its successful called-CI outcomes are retained. `gh run cancel 40031550534` and `gh run rerun 40031550534 --job 120680753422` exited0; attempt2 retries only equality and downstream rehearsal at the same source head and unchanged30-minute budget. Rehearsal/publication acceptance and final direct result remain pending; no completion is claimed from this snapshot.

## Final Task3 Acceptance

The literal task requires all six actual current-head native producers and release-content checks, authenticated current/base observations with a passing worst-lane aggregate, and the existing native/BE/ARM/WASM, sanitizer/fuzz/property and RetroArch paths at their unchanged status. All are now established by the named passing jobs in called CI40031550534 attempt1 and successful Nightly40031550533. Publication rehearsal and an overall direct-PR run conclusion are separate pipeline/ship follow-ups; neither is named as a product acceptance prerequisite here. Their pending state is retained above and is not represented as success. No owner gate, new requirement or alternative acceptance mechanism was introduced.

| Final verification command | Exit | Result | Run URL |
|---|---:|---|---|
| `gh api repos/szTheory/muhtari/actions/runs/40031550534/attempts/1/jobs --paginate` | 0 | Six native producers, ASan, hygiene, library, both nofp paths, BE, quality, hash equality, WASM, RetroArch and CI required all success; outer equality/rehearsal cancelled in this attempt, not green | https://github.com/szTheory/muhtari/actions/runs/40031550534 |
| `gh run view 40031550533 --json headSha,conclusion,status,url,jobs` | 0 | Source25919fe, conclusion success; full equality, fuzz, properties, vectors and benchmarks success | https://github.com/szTheory/muhtari/actions/runs/40031550533 |
| `gh api repos/szTheory/muhtari/pulls/24 --jq '{head: .head.sha, base: .base.sha, merge: .merge_commit_sha}'` | 0 | Exact source25919fe822cd6cc23a6984c13ac826796e7d9cfe, base617dfa762ddb649839b925be5aa582180066fa78 and PR merge2fe713c1074a63bbf89384d56200bd48060c0549 | https://github.com/szTheory/muhtari/pull/24 |
| Unchanged `quality_join.cmake` command in Current hosted acceptance evidence, rerun at finalization | 0 | Eight original producer directories; current worst375263/base399371 within399937, ordinary metrics and original typed/identity/reason rules preserved | https://github.com/szTheory/muhtari/actions/runs/40031550534 |
| Read-only Python assertions over downloaded producer/ZIP artifacts | 0 | Eight current/base SHAs and exact environment pairs, raw original base-policy bytes retaining `gate_lines`, six lanes with exactly three ZIPs, all18 sizes matching original observations, current observations omit only the retired metric | https://github.com/szTheory/muhtari/actions/runs/40031550534 |
| Four `check_junit.cmake` commands over `accepted-equality/build/{ci,be32}/junit{,-release}.xml`, using tracked inventory and ordinary exclusion/release label as appropriate | 0 each | Native612+6 and PowerPC612+6, every required test exactly once | https://github.com/szTheory/muhtari/actions/runs/40031550533 |
| Unchanged `lane_equality.cmake` command in Current hosted acceptance evidence, rerun at finalization; byte/line-count assertion | 0 each | Exact PR-merge identities,47 byte-identical native/BE rows including all30 board rows | https://github.com/szTheory/muhtari/actions/runs/40031550533 |

The six totals recorded above are final measured acceptance, not predicted compression fit. All old predicates/pins and required/advisory statuses persist. Performance remains on the test card under its independent authority; the called event requests no performance artifact. The still-pending direct-PR performance/aggregate dimension remains in the hosted-retirement todo until its own applicable result exists. WINDOWS42,48 and50 are resolved by this actual evidence; the original failed measurements and superseded01 record remain intact.

BOARD-01–03 are complete from the preceding semantic/three-host/hostile plans plus this final portability/package acceptance. Spec-less edge rows and descriptorless prohibitions retain their disclosed unclassified/unresolved/verification-null metadata; no test descriptor, hardware accuracy, retail playability, PAL completion or human judgment is fabricated. Active Phase6 plans are9/9 and total currently planned active slots18/18. This plan does not mark Phase6 verified or complete; the parent execution orchestrator owns that gate and the outstanding outer workflow follow-ups.

## Self-Check: PASSED

All task artifacts exist and task/fix commits through accepted source25919fe are ancestors of HEAD. Fresh finalization commands above passed against the actual collected hosted artifacts. Local612+6, hygiene24, exact source pins, trusted fixture/sample relations and three-ZIP results are recorded with their commands and exits; current-head six-native authenticated fit and required portability evidence are now accepted. No known stub or unrun plan verification remains. The outstanding direct/outer workflow and retirement-todo dimensions are explicitly retained as follow-ups, not silently converted to passing evidence.
