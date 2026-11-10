---
phase: 13-portable-save-load-across-three-interfaces
verified: 2026-11-10T01:23:13Z
status: passed
score: 44/44 must-haves verified
covered_files:
  - .planning/phases/13-portable-save-load-across-three-interfaces/13-01-PLAN.md
  - .planning/phases/13-portable-save-load-across-three-interfaces/13-01-SUMMARY.md
  - .planning/phases/13-portable-save-load-across-three-interfaces/13-02-PLAN.md
  - .planning/phases/13-portable-save-load-across-three-interfaces/13-02-SUMMARY.md
  - .planning/phases/13-portable-save-load-across-three-interfaces/13-03-PLAN.md
  - .planning/phases/13-portable-save-load-across-three-interfaces/13-03-SUMMARY.md
  - .planning/phases/13-portable-save-load-across-three-interfaces/13-04-PLAN.md
  - .planning/phases/13-portable-save-load-across-three-interfaces/13-04-SUMMARY.md
  - .planning/phases/13-portable-save-load-across-three-interfaces/13-05-PLAN.md
  - .planning/phases/13-portable-save-load-across-three-interfaces/13-05-SUMMARY.md
  - .planning/phases/13-portable-save-load-across-three-interfaces/13-06-PLAN.md
  - .planning/phases/13-portable-save-load-across-three-interfaces/13-06-SUMMARY.md
  - .planning/phases/13-portable-save-load-across-three-interfaces/13-07-PLAN.md
  - .planning/phases/13-portable-save-load-across-three-interfaces/13-07-SUMMARY.md
  - .planning/phases/13-portable-save-load-across-three-interfaces/13-08-PLAN.md
  - .planning/phases/13-portable-save-load-across-three-interfaces/13-08-SUMMARY.md
  - .planning/phases/13-portable-save-load-across-three-interfaces/13-CONTINUATION-MATRIX.md
  - .planning/phases/13-portable-save-load-across-three-interfaces/13-FIELD-PROOF.md
  - .planning/phases/13-portable-save-load-across-three-interfaces/13-RESOURCE-EVIDENCE.md
  - .planning/preparation/ARCHITECTURE.md
  - .planning/preparation/DECISIONS.md
  - .planning/preparation/HOSTS-AND-INTEGRATION.md
  - CMakeLists.txt
  - docs/embedding.md
  - docs/libretro.md
  - docs/runner.md
  - docs/testing.md
  - include/muhtari.h
  - libretro/libretro.c
  - libretro/muhtari_libretro.info.in
  - runner/main.c
  - src/instance.c
  - src/internal.h
  - src/ports.c
  - src/state.c
  - tests/abi/tests.cmake
  - tests/api/v8.c
  - tests/cart/images.c
  - tests/cart/tests.cmake
  - tests/cmake/hashes.cmake
  - tests/cmake/info_claims.cmake
  - tests/compat/tests.cmake
  - tests/core/test_core.c
  - tests/core/test_state.c
  - tests/core/tests.cmake
  - tests/fuzz/run.c
  - tests/fuzz/run_seeds.c
  - tests/fuzz/tests.cmake
  - tests/inventory/ci.txt
  - tests/inventory/wasm32.txt
  - tests/libretro/host.c
  - tests/libretro/tests.cmake
  - tests/prop/test_prop.c
  - tests/prop/tests.cmake
  - tests/runner/test_runner.c
  - tests/runner/test_save.c
  - tests/runner/tests.cmake
covered_digest: "v4:sha256:3b932e41bd621322d463c75e8a72fecd87afa9eb252c8a4b3315847fae27fbcc"
behavior_unverified: 0
overrides_applied: 0
decision_coverage:
  honored: 17
  total: 17
  not_honored: []
---

# Phase 13: Portable Save/Load Across Three Interfaces Verification Report

**Phase Goal:** Hosts and runner users can restore a complete compatible machine point safely and continue identically through one bounded library-owned state contract.

**Verified:** 2026-11-10T01:23:13Z
**Status:** passed
**Re-verification:** No — initial-mode refresh; the previous report had no gaps.

### Verification Refresh

This initial-mode refresh rechecked the roadmap contract, all eight plans and summaries, all seven requirements, the current implementation, and the current docs. The 28 declared artifacts remain present and substantive. The source trace confirms the identity is bound after successful install, the public API validates the complete envelope and payload before apply, runner and libretro use that API, and the existing native/BE hash consumer checks the owned state and continuation rows. The automated key-link query returned 0/16 because plan `from` entries are descriptive names rather than relative source paths; manual source-to-test tracing confirms the links in the table below.

`docs/libretro.md` matches the current callbacks and exact length rules, keeps state-file persistence with the frontend, and explicitly says actual RetroArch save/load, rewind, and run-ahead qualification remains pending. Its current `.info` claim guard is backed by the loaded-module and property test registrations plus the host-control registration. `docs/testing.md` lists the state cases present in the current inventory and its TUNE-07 deferral matches dormant seed SEED-010. No new implementation or documentation claim gap was found.

The retained Phase 13 local receipts in `13-RESOURCE-EVIDENCE.md` are for source SHA `13bb1af4e7b63fb727854d42ad4431091f2083cc`; they record native, PowerPC/QEMU, release, equality, and sanitizer success. No tests or workflows were run for this verifier-only refresh. The current state codec and runner changes since that receipt are formatting-only; current libretro input-tracing additions are guarded by `MUHTARI_INPUT_TRACE` and do not change the serialization callbacks. This report does not claim a new current-HEAD test run.

The three plan-13-08 prohibitions remain marked `verification: judgment` and `status: unverified` in the plan and are retained as visibly unverified advisories below. Project rules assign the document and evidence judgments to the agent; none requires owner-only consent or a hardware run. They therefore do not add a human-verification checkpoint or change the passed status.

## Goal Achievement

### Observable Truths

| # | Truth | Status | Evidence |
|---:|---|---|---|
| 1 | Library and libretro callers query a fixed loaded-machine size and save/restore complete versioned little-endian state; unloaded/failed callbacks are safe. | VERIFIED | API v8, shared visitor, callback delegation and loaded-module host test. |
| 2 | Truncated, corrupt, unsupported, incompatible and checksum-valid hostile data is rejected before mutation; next execution and host battery metadata are preserved. | VERIFIED | Envelope, identity and payload preflight; core/property refusal tests and subsequent-run comparisons. |
| 3 | Compatible restore continues with independent semantic, frame, raw-audio and PCM observations; canonical bytes/continuation match on native and executed BE lanes. | VERIFIED | Independent topology scenarios, 78 report rows and exact-source native/PowerPC-QEMU identity/hash equality. |
| 4 | Save/load work is bounded and allocation-free after load; battery generation remains live host metadata and core state operations do not write durable files. | VERIFIED | Resume validation, allocator counters, battery/file sentinels, topology work and ASan/UBSan results. |
| 5 | Runner users save at selected frames, load in a fresh process, hash continuation and get checked file failure behavior. | VERIFIED | Runner state roundtrip/options/file tests and current runner documentation. |
| 6 | 13-01 D-01/D-02/D-03: Typed visitor covers mutable owners and memory spans; private tracer restores a generated machine and next frame/audio observation. | VERIFIED | Core state payload/topology cases and independent RAM/mailbox/frame/audio checks. |
| 7 | 13-01 D-05/D-06: Validation is read-only, apply follows complete validation, restores derived indexes and bounds hostile work by installed topology. | VERIFIED | Separate validation/apply paths and CRC-repaired late-field refusal test. |
| 8 | 13-01 D-17: No post-load allocation or unsupported public serialization claim is introduced. | VERIFIED | Allocator instrumentation; public API confined to declared v8 contract. |
| 9 | 13-01 STATE-01 idempotency: Unchanged saves are byte-identical and repeated loads preserve observations. | VERIFIED | Canonical state comparisons in core and seeded property cases. |
| 10 | 13-01 STATE-04 concurrency: Independent instance and storage remain isolated. | VERIFIED | Two-instance core state tests. |
| 11 | 13-02 D-04: Effective immutable machine identity is cached only after successful install and binds normalized media/configuration. | VERIFIED | Successful install calls the identity binder; identity fields and normalized inputs are hashed and tested. |
| 12 | 13-02 D-03/D-06: Failed replacement preserves identity/topology; state bytes exclude immutable media and caller addresses. | VERIFIED | Lifecycle/identity tests and envelope payload inspection. |
| 13 | 13-02 D-17: Existing SHA-256/CRC implementations are reused without a package or installed target. | VERIFIED | Core archive wiring and full consumer builds. |
| 14 | 13-02 STATE-03 concurrency: Equivalent machines have pointer/platform-independent identity. | VERIFIED | Identity tests and matching native/BE identity receipts. |
| 15 | 13-03 D-01/D-05/D-06/D-10: Public format-1 operations validate exact envelope before apply and refuse overlap/invalid input without mutation. | VERIFIED | Public API tests cover size, overlap, malformed length/header/CRC/identity and hostile fields. |
| 16 | 13-03 D-07: API/ABI/state format are independent from unchanged behavior revision; old version prefix/status values remain compatible. | VERIFIED | API v7/v8 and ABI consumers pass. |
| 17 | 13-03 D-08/D-09: Changed battery restore advances generation once, equal/refused restores preserve it, and pending programming uses the existing commit path without state-load file I/O. | VERIFIED | Battery/token, pending EEPROM, refusal and host-file sentinel assertions. |
| 18 | 13-03 STATE-01 idempotency: Same machine point saves canonical bytes; repeated restores preserve observations. | VERIFIED | Core state and property continuation cases. |
| 19 | 13-03 STATE-05 idempotency: Reapplying identical battery bytes preserves generation after the first changed restore. | VERIFIED | Core battery state cases. |
| 20 | 13-03 STATE-05 concurrency: Restoring one instance leaves another instance, spans and host files unchanged. | VERIFIED | Span identity and file sentinel assertions. |
| 21 | 13-04 D-14/D-15: Supported topology cases use independent semantic oracles and compare restored bytes/video/raw audio/PCM/counters. | VERIFIED | Core scenario matrix, independent observations and equality report. |
| 22 | 13-04 D-05/D-06: CRC-repaired hostile fields reach validators; refusal preserves execution, pending input, token, topology and spans. | VERIFIED | Core and property refusal cases including next-run checks. |
| 23 | 13-04 D-17: Existing property/fuzz consumers run bounded mutation/replay without retained personal state or new framework. | VERIFIED | Property continuation/refusal and fuzz replay are present in final inventories; no new dependency. |
| 24 | 13-04 STATE-03 idempotency: Repeated compatible state operations yield identical bytes and future observations. | VERIFIED | Seeded continuation cases and canonical report. |
| 25 | 13-04 STATE-03 concurrency: Distinct inputs on two instances do not share state. | VERIFIED | Core/property isolation cases. |
| 26 | 13-04 STATE-04 idempotency: Repeated valid/refused operations do not allocate after load or change future output. | VERIFIED | Allocator counters, bounded validator cases and sanitizer run. |
| 27 | 13-05 D-11: Runner flags use canonical bytes and invocation-local completed-frame points, including zero. | VERIFIED | Runner state scheduling/options and roundtrip cases. |
| 28 | 13-05 D-12/D-09: Bounded reads and checked sibling-temp replacement preserve prior files on refusal/failure with explicit status. | VERIFIED | Read/write fault seams, nonregular-file cases and prior-file sentinels. |
| 29 | 13-05 D-03: Fresh process consumes only explicitly supplied suffix input; state invents no movie cursor. | VERIFIED | Fresh-process runner test and documented load/input ordering. |
| 30 | 13-05 STATE-06 idempotency: Same state point yields identical hash and file bytes. | VERIFIED | Runner hash/file equality tests. |
| 31 | 13-05 STATE-06 concurrency: Runner invocations own buffers/temp files; library calls do not durably write. | VERIFIED | Runner owns I/O helpers; core sentinel tests show no state-load durable writes. |
| 32 | 13-06 D-13/D-10: Loaded libretro callbacks delegate to the shared API; extra capacity stays untouched and restore length is exact. | VERIFIED | Callback implementation and loaded-module state test. |
| 33 | 13-06 D-13: Unloaded/failed callbacks have no stale bytes; supported contexts validate LE state and context3 refuses unsupported netplay semantics. | VERIFIED | Host lifecycle/context cases and callback validation wiring. |
| 34 | 13-06 D-07/D-13: The .info claim is limited to basic callbacks, not actual RetroArch, rewind or run-ahead acceptance. | VERIFIED | Feature-claim consumer and docs; no broader host evidence claimed. |
| 35 | 13-06 FRONT-01 idempotency: Repeated callbacks produce canonical bytes and correct battery token behavior. | VERIFIED | Loaded host callback test and core battery token cases. |
| 36 | 13-06 FRONT-01 concurrency: Separate module/library instances do not share buffers or emulated state. | VERIFIED | Host module and core instance isolation tests. |
| 37 | 13-07 D-14/D-16: Existing producer emits state and continuation rows, and native/BE consumers compare them. | VERIFIED | 78-row equality report and passing lane equality consumer. |
| 38 | 13-07 D-17: Hash/inventory/fault consumers detect missing, duplicate, malformed or changed state evidence. | VERIFIED | Existing hash consumer exact count/key/digest checks and fault controls. |
| 39 | 13-07 STATE-03 idempotency: Repeated scenario report generation is byte-identical. | VERIFIED | Equality report repeat identity checked in native/BE workflows. |
| 40 | 13-07 D-16: Existing compile-only, native, advisory, release and publication lanes retain their labels. | VERIFIED | Lane definitions were not promoted; local QEMU, hosted and physical evidence are distinguished. |
| 41 | 13-08 D-01/D-03/D-07/D-17: Docs, inventories, API/ABI/provenance/package guards and complete workflow agree with implementation. | VERIFIED | Documentation/policy consumers and complete native/BE workflows pass. |
| 42 | 13-08 D-04/D-06/D-17: Maximum installed legal topology work, late refusal, stack/object growth and no post-load allocation are measured. | VERIFIED | Largest generated installed topology is 209,628 bytes and exercises public apply and late refusal. Separate synthetic 307,932-byte ceiling has size/write/validate evidence only, not public apply. |
| 43 | 13-08 D-16: Exact-source native and big-endian execution, equality and complete normal/release inventories pass. | VERIFIED | Receipts match final source SHA 13bb1af4e7b63fb727854d42ad4431091f2083cc; inventory and lane equality consumers pass. |
| 44 | 13-08 STATE-04 concurrency: No new thread guarantee, host allowance or remote publication is inferred. | VERIFIED | Same-instance limits remain; evidence records are N/A-local and explicitly disclaim publication and Phase 12 renewal. |

**Score:** 44/44 truths verified (0 present, behavior-unverified).

### Required Artifacts

All 28 declared plan artifacts are substantive and wired. No missing, stub or orphaned artifact was found.

| Plan | Artifacts | Result |
|---|---:|---|
| 13-01 | 3 | VERIFIED, 3/3 |
| 13-02 | 3 | VERIFIED, 3/3 |
| 13-03 | 3 | VERIFIED, 3/3 |
| 13-04 | 4 | VERIFIED, 4/4 |
| 13-05 | 4 | VERIFIED, 4/4 |
| 13-06 | 4 | VERIFIED, 4/4 |
| 13-07 | 3 | VERIFIED, 3/3 |
| 13-08 | 4 | VERIFIED, 4/4 |

### Key Link Verification

The automated key-link queries reported 0/16 because plan “from” entries name conceptual components rather than relative source paths. Manual source-to-test tracing verifies the intended wiring.

| Connection | Status | Evidence |
|---|---|---|
| State visitor → mutable owners, installed C/E/B spans and bus rearm | VERIFIED | state visitor, internal structure, port rearm and core state tests. |
| Successful install → cached identity → state compatibility | VERIFIED | install path calls binder; identity is computed and compared before apply. |
| Public API → envelope, validation-before-apply and battery generation | VERIFIED | public header/API, state implementation and battery/refusal tests. |
| Independent oracle/refusal tests → output and next-run preservation | VERIFIED | core state and property tests. |
| Runner frame points/files → shared public API | VERIFIED | runner uses API; runner state tests exercise fresh-process restore/files. |
| Libretro callbacks and .info → loaded-module test | VERIFIED | callback implementation, feature claim check and host state case. |
| Scenario reports → hash consumer → native/BE equality | VERIFIED | report producer, CMake consumer and exact lane equality check. |
| Public behavior → docs and local evidence records | VERIFIED | embedding, runner and libretro docs plus resource evidence. |

### Data-Flow Trace (Level 4)

There is no dynamic UI rendering in this phase. State values flow from live loaded instance memory through the serializer; runner restore bytes flow from bounded file reads through validation. Equality output is computed from independent test instances.

| Artifact | Value | Source | Real data | Status |
|---|---|---|---|---|
| src/state.c | serialized payload | Loaded chip/device/board state, pending work and installed mutable RAM/battery spans | Yes | FLOWING |
| src/state.c | compatibility identity | Installed media and normalized machine configuration | Yes | FLOWING |
| runner/main.c | state input buffer | Bounded checked read of a compatible state file | Yes | FLOWING |
| libretro/libretro.c | serialized bytes | Shared public state API over loaded core | Yes | FLOWING |
| tests/core/test_state.c | canonical report rows | Independent machines and semantic/frame/audio observations | Yes | FLOWING |

### Behavioral Spot-Checks

The retained final-source JUnit files were parsed independently. Named state, runner and libretro cases are present and have no failures in the relevant runs.

| Behavior | Evidence | Result | Status |
|---|---|---|---|
| Independent state continuation | core.state.equality_report in native, BE and ASan JUnit | Present; suites report zero failures | PASS |
| Valid/refused property state operations | prop.state_continuation and prop.state_refusal in native, BE and ASan JUnit | Present; suites report zero failures | PASS |
| Fresh-process runner state behavior | runner.state.roundtrip/options in native, BE and ASan JUnit | Present; suites report zero failures | PASS |
| Loaded libretro state callbacks | libretro.host.state in native, BE and ASan JUnit | Present; suites report zero failures | PASS |
| Complete native workflow | cmake --workflow --preset ci and native-ci.xml | Exit 0; 689 tests, zero failures; complete inventory | PASS |
| Native release tests | native-release.xml | 6 tests, zero failures | PASS |
| PowerPC/QEMU workflow | cmake --workflow --preset be32-full and BE JUnit | Exit 0; 689 normal and 6 release tests; zero failures | PASS |
| Source and canonical hash equality | lane_equality.cmake on SHA 13bb1af4e7b63fb727854d42ad4431091f2083cc | Exit 0; native and BE identity/hash receipts match | PASS |
| ASan/UBSan state and hostile-input inventory | asan.xml and full inventory consumer | Exit 0; 670 tests, zero failures/findings | PASS |

Exact commands, exits and receipt paths are recorded in 13-RESOURCE-EVIDENCE.md. These are local Docker Desktop Ubuntu ARM64 results with PowerPC QEMU user-mode execution (N/A-local), not physical PowerPC or hosted-run evidence.

### Test Quality Audit

The requirement-linked test sources contain no disabled tests. File writes in the state, runner and host tests create generated inputs or sentinels; expected behavior comes from independent semantic observations, an uninterrupted comparison path, exact public outputs, or explicit preservation checks, rather than values captured from the implementation being tested.

| Test File | Linked Requirement | Active / Skipped | Evidence Quality |
|---|---|---:|---|
| `tests/core/test_state.c` | STATE-01, STATE-02, STATE-03, STATE-04, STATE-05 | active / 0 | Canonical bytes, independent semantic observations, refusal statuses and next-run preservation. |
| `tests/prop/test_prop.c` | STATE-02, STATE-03, STATE-04 | active / 0 | Seeded independent-instance continuation and checksum-repaired refusal properties. |
| `tests/fuzz/run.c` | STATE-02, STATE-04 | active / 0 | Bounded replay mutations and refusal preservation through the existing consumer. |
| `tests/runner/test_save.c` | STATE-06 | active / 0 | Generated media, fresh-process I/O, and checked failure boundaries. |
| `tests/libretro/host.c` | FRONT-01, STATE-05 | active / 0 | Loaded module callbacks compared with the library contract and host battery metadata. |

### Probe Execution

Not applicable: this is not a migration/tooling phase and no phase plan or summary declares a probe script.

### Requirements Coverage

| Requirement | Plans | Description | Status | Human judgment |
|---|---|---|---|---|
| STATE-01 | 13-01, 13-03 | Complete caller-owned versioned little-endian machine state | SATISFIED | false |
| STATE-02 | 13-03, 13-04, 13-08 | Reject incompatible/hostile state before mutation and preserve execution/metadata | SATISFIED for the explicitly tested hostile-state contract | false |
| STATE-03 | 13-02, 13-04, 13-07, 13-08 | Independent continuation and native/BE canonical equality | SATISFIED | false |
| STATE-04 | 13-01, 13-04, 13-08 | Bounded work, no post-load allocation and safe deadlines | SATISFIED within producer-derived resume validation | false |
| STATE-05 | 13-03, 13-08 | Distinguish emulated battery bytes, live token and durable-file ownership | SATISFIED | false |
| STATE-06 | 13-05, 13-08 | Runner state points, fresh-process restore, hashing and checked file handling | SATISFIED | false |
| FRONT-01 | 13-06, 13-08 | Libretro state callbacks and truthful basic feature declaration | SATISFIED | false |

### Policy/Prohibition Checks

These judgment-tier prohibitions remain explicitly marked `unverified` in plan 13-08 and are retained as advisory flags here. Current project rules assign these source, fixture, documentation, and evidence-boundary judgments to the verifier; they do not require owner-only input.

| Prohibition | Flag | Human Judgment | Evidence |
|---|---|---:|---|
| Do not commit immutable/proprietary media or personal durable state bytes as fixtures | UNVERIFIED — ADVISORY | false | The Phase 13 receipt records no new ROM/BIOS fixtures; generated state/media stays in ignored build output. Allowed binary fixtures are listed in `tests/roms/manifest.txt`, as required by `AGENTS.md`. |
| Do not conflate restored live state with durable file, frontend recovery or Playstead history ownership | UNVERIFIED — ADVISORY | false | `src/state.c` operates on caller-owned buffers; runner owns file reads/writes, and current embedding/libretro docs distinguish live bytes, generation metadata, and frontend-owned persistence. |
| Do not promote local/callback/advisory evidence to actual-host, hosted-lane or Phase 12 acceptance | UNVERIFIED — ADVISORY | false | Receipts label native and PowerPC/QEMU runs as local, explicitly disclaim physical hardware, hosted acceptance, RetroArch, publication, and Phase 12 renewal; current libretro docs state actual-host qualification is pending. |

### Scope Limits

- The separately flagged automatic shape-classification edge probe remains unclassified. The explicit hostile-state matrix is verified; no extra interpretation of undefined shapes is claimed.
- The largest generated installed topology is 209,628 bytes and receives public apply plus repaired-CRC late refusal. The synthetic 307,932-byte independent ceiling is covered through size/write/validate only, not public apply.
- Producer-derived clock/deadline checks are verified; no universal absolute emulated-time ceiling or new concurrency promise is claimed.
- PowerPC results are local QEMU user-mode execution. No physical hardware, hosted run, actual RetroArch consumer or Playstead end-to-end result is asserted.

### Anti-Patterns Found

| File | Line | Pattern | Severity | Impact |
|---|---:|---|---|---|
| Phase 13 modified file set | — | No `TBD`, `FIXME`, `XXX`, TODO, HACK, placeholder, or unimplemented markers | None | Empty-return matches are ordinary error/null helpers; no user-visible stub or debt-marker blocker was found. |

The phase code review in 13-REVIEW.md is clean with no findings.

### Human Verification Required

None. The plan's three unverified prohibition flags remain visible as advisories; current project instructions do not reserve those source/document/evidence judgments for the owner. Actual RetroArch save/load, rewind and run-ahead qualification remains pending outside Phase 13.

### Decision Coverage

All trackable CONTEXT.md decisions are honored by shipped artifacts (17/17; non-blocking gate).

### Gaps Summary

No code-level truth or requirement failed, and no new source/document claim gap was exposed. All 44 observable truths and all seven requirements remain supported by the current source inspection and retained Phase 13 receipts. The three plan-defined prohibition flags remain visibly unverified as advisories; the evidence and current project rules resolve whether the phase needs owner input, and no owner-only fact or authorization is outstanding. The unclassified STATE-02 edge-probe boundary, local QEMU scope, and pending actual RetroArch qualification remain clearly stated.

---

_Verified: 2026-11-10T01:23:13Z_
_Verifier: the agent (gsd-verifier)_
