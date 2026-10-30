---
phase: 06-retail-cartridge-boards
verified: 2026-10-23T22:52:14Z
status: passed
score: 33/33 must-haves verified
covered_files: [
  ".github/workflows/be-equality.yml",
  ".github/workflows/ci.yml",
  ".github/workflows/nightly.yml",
  ".github/workflows/release.yml",
  ".planning/phases/06-retail-cartridge-boards/06-01-PLAN.md",
  ".planning/phases/06-retail-cartridge-boards/06-01-SUMMARY.md",
  ".planning/phases/06-retail-cartridge-boards/06-02-PLAN.md",
  ".planning/phases/06-retail-cartridge-boards/06-02-SUMMARY.md",
  ".planning/phases/06-retail-cartridge-boards/06-03-PLAN.md",
  ".planning/phases/06-retail-cartridge-boards/06-03-SUMMARY.md",
  ".planning/phases/06-retail-cartridge-boards/06-04-PLAN.md",
  ".planning/phases/06-retail-cartridge-boards/06-04-SUMMARY.md",
  ".planning/phases/06-retail-cartridge-boards/06-05-PLAN.md",
  ".planning/phases/06-retail-cartridge-boards/06-05-SUMMARY.md",
  ".planning/phases/06-retail-cartridge-boards/06-06-PLAN.md",
  ".planning/phases/06-retail-cartridge-boards/06-06-SUMMARY.md",
  ".planning/phases/06-retail-cartridge-boards/06-07-PLAN.md",
  ".planning/phases/06-retail-cartridge-boards/06-07-SUMMARY.md",
  ".planning/phases/06-retail-cartridge-boards/06-08-PLAN.md",
  ".planning/phases/06-retail-cartridge-boards/06-08-SUMMARY.md",
  ".planning/phases/06-retail-cartridge-boards/06-09-PLAN.md",
  ".planning/phases/06-retail-cartridge-boards/06-09-SUMMARY.md",
  ".planning/phases/06-retail-cartridge-boards/06-10-PLAN.md",
  ".planning/phases/06-retail-cartridge-boards/06-10-SUMMARY.md",
  "AGENTS.md",
  "ASSET_POLICY.md",
  "CMakeLists.txt",
  "CMakePresets.json",
  "PROVENANCE.md",
  "README.md",
  "cmake/packaging.cmake",
  "docs/compatibility.md",
  "docs/embedding.md",
  "docs/emulation-tests.md",
  "docs/hardware/cartridge.md",
  "docs/libretro.md",
  "docs/quality.md",
  "docs/runner.md",
  "docs/testing.md",
  "host/status_text.c",
  "include/muhtari.h",
  "libretro/libretro.c",
  "pins.txt",
  "quality.json",
  "runner/main.c",
  "src/bus.c",
  "src/cart.c",
  "src/instance.c",
  "src/internal.h",
  "tests/abi/tests.cmake",
  "tests/api/v3.c",
  "tests/api/v4.c",
  "tests/audio/expected.txt",
  "tests/built/build.cmake",
  "tests/built/expected.txt",
  "tests/cart/images.c",
  "tests/cart/images.h",
  "tests/cart/test_cart.c",
  "tests/cart/test_cartdb.c",
  "tests/cart/tests.cmake",
  "tests/cart/write_images.c",
  "tests/check.h",
  "tests/cmake/check_junit.cmake",
  "tests/cmake/expect_fail.cmake",
  "tests/cmake/hashes.cmake",
  "tests/cmake/lane_equality.cmake",
  "tests/cmake/quality.cmake",
  "tests/cmake/quality_evidence.cmake",
  "tests/cmake/quality_join.cmake",
  "tests/core/test_core.c",
  "tests/diag/absolute.a78",
  "tests/diag/absolute.cfg",
  "tests/diag/absolute.s",
  "tests/diag/activision.a78",
  "tests/diag/activision.cfg",
  "tests/diag/activision.s",
  "tests/diag/assemble.cmake",
  "tests/diag/driver.c",
  "tests/diag/expected.csv",
  "tests/diag/fractalus.a78",
  "tests/diag/fractalus.cfg",
  "tests/diag/fractalus.s",
  "tests/diag/tests.cmake",
  "tests/fuzz/loader.c",
  "tests/fuzz/run.c",
  "tests/fuzz/run_cart.h",
  "tests/fuzz/run_seeds.c",
  "tests/fuzz/seeds.c",
  "tests/fuzz/tests.cmake",
  "tests/header/header.c",
  "tests/header/header.cpp",
  "tests/inventory/ci.txt",
  "tests/inventory/nofp.txt",
  "tests/inventory/wasm32.txt",
  "tests/libretro/rcheevos.c",
  "tests/libretro/tests.cmake",
  "tests/policy/quality.cmake",
  "tests/policy/tests.cmake",
  "tests/prop/test_prop.c",
  "tests/prop/tests.cmake",
  "tests/roms/manifest.txt",
  "tests/runner/tests.cmake",
  "tools/compat_doc.cmake"
]
covered_digest: "v3:sha256:b275b9b003705fa7e4b62d4c0b185a0145503d6a13fa358a7200713c175e2acd"
behavior_unverified: 0
overrides_applied: 0
decision_coverage:
  honored: 18
  total: 18
  not_honored: []
human_verification: []
requirements_verified:
  - BOARD-01
  - BOARD-02
  - BOARD-03
preserved_probe_metadata:
  edge_requirements:
    - BOARD-01
    - BOARD-02
    - BOARD-03
  classification: unclassified
  status: unresolved
  verification: null
  descriptorless_prohibitions: 3
  agent_intent_review: complete
  counts_as_test_descriptor: false
---

# Phase 6: Retail cartridge boards Verification Report

**Phase Goal:** Run Activision, Absolute and Fractalus cartridges with correct banking and physical RAM behavior.
**Verified:** 2026-10-23T22:52:14Z
**Status:** passed
**Re-verification:** No — initial verification

## Goal Achievement

The implementation achieves BOARD-01–03 under the selected compatibility/profile policies. This verdict follows source inspection, freshly executed named behavioral tests, independently replayed hosted artifact judges, exact test inventories, and current GitHub job results. SUMMARY claims were used to locate evidence, not as proof.

Scope is active plans 02–10. Historical plan 01 and its failed SUMMARY remain superseded records, not an active dependency or successful allocation/fit proof. The fingerprint owner includes historical PLAN/SUMMARY files automatically; that does not reactivate them. No previous VERIFICATION existed and no override was applied.

The roadmap query returned the correct phase section but an empty `success_criteria` array. All four numbered criteria were therefore recovered directly from ROADMAP.md and included below. Of 30 active PLAN truths, the repeated three-ZIP/six-native bound is merged into row 30. The four roadmap truths plus 29 additional plan truths give 33 scored truths. Temporal statements in plan 10 describe its prerequisite slice; they do not negate subsequent board completion.

### Observable Truths

Evidence commands C1–C15 and A1–A9 are defined below. All behavior-dependent truths have passing behavioral evidence.

| # | Truth / source | Status | Evidence |
|---|---|---|---|
| 1 | Activision image layouts produce equivalent mapped reads after identified normalization, with original hashes still available to hosts. (Roadmap 1) | ✓ VERIFIED | C1, C5, C14; private copy in `src/cart.c:782`, original-content descriptor translation in `libretro/libretro.c:781`; all four header/layout inputs agree and raw identities remain distinct. |
| 2 | Absolute and Fractalus diagnostics exercise fixed/switched banks, initial state and RAM aliases through CPU-visible accesses. (Roadmap 2) | ✓ VERIFIED | C2/C4; real 6502 programs in the owned fixtures, independent 21/24-record expectations, complete mailbox checks. |
| 3 | Malformed sizes and random bank traffic stay bounded without out-of-range reads or state changes on a refused load. (Roadmap 3) | ✓ VERIFIED | C1/C2/C6/C7/C8–C11; exact geometry before allocation, deep refused-state and audible resumed-control checks; hosted ASan and time-based fuzz success A8. |
| 4 | New board fixtures run through library, runner and libretro; prior frame/audio predicates retain their passing behavior. (Roadmap 4) | ✓ VERIFIED | C4/C5, A3/A4/A6/A8; three admitted identities, nine complete numeric fixture observations plus smoke; old predicates retained. |
| 5 | Absolute starts at bank zero, executes through library, real runner and libretro, and produces the same numeric observation. (02) | ✓ VERIFIED | C2/C4; smoke reads bank-zero tag 90 through all three deliverables, then real Absolute fixture checks 21 records. |
| 6 | Tracer validates 65536-byte geometry before allocation, retains ABI/layout, and uses instance-owned latch and existing switch. (02) | ✓ VERIFIED | `src/cart.c:88,782,926,1016`; C4 allocation/refusal/continuation controls; API1–4 exact passing inventories A3. |
| 7 | Automated acceptance reads a bounded mailbox report from the real runner, with numeric verdicts owned by tests. (02) | ✓ VERIFIED | `runner/main.c:577,1035` and `tests/diag/driver.c:393`; C4's actual process/report/module path, protocol and usage faults. |
| 8 | API4 adds descriptive 4u orientation and 1024u assumed-layout warning constants while ABI1 and public structures/signatures remain unchanged. (03) | ✓ VERIFIED | `include/muhtari.h:12,129,142`; C13 runtime versions/frozen 140-byte cart-info and values; A3 older linked clients. |
| 9 | Current/older API smokes, negative version control, exact native/nofp/WASM inventories and embedding documentation accompany the interface. (03) | ✓ VERIFIED | `tests/abi/tests.cmake`, `tests/api/v1.c`–`v4.c`, all three inventories; A3/A8 executed ABI checks; docs reviewed against current header. |
| 10 | Only a half-swapped hash match identifying Activision authorizes private OM normalization; orientation survives headered/headerless lookup. (04) | ✓ VERIFIED | `src/cart.c:401,578,658,782`; C1 covers direct/swapped/headered/headerless, misleading metadata, repeated load and caller immutability. |
| 11 | Supplied full/headerless identities and trailer meanings remain inspectable with the orientation flag. (04) | ✓ VERIFIED | Original input is hashed in `runner/main.c:233`; C14 checks actual hash lines, raw fields, trailers and missing-line faults; C5 independently hashes four inputs. |
| 12 | Unknown explicit Activision assumes OM with a warning; unknown headerless size never identifies this board; precedence remains hash/v4/v3. (04) | ✓ VERIFIED | `src/cart.c:544,578,595,658`; C1 unknown/fallback/override cases and C14 actual assumed-warning process output. |
| 13 | Exact 128KiB Activision maps six independent 8KiB windows, starts at bank0, and only FF80–FF8F CPU writes select A2–A0. (04) | ✓ VERIFIED | `src/cart.c:912,1009`; C1 independent half tags at window endpoints, eight banks, all 16 selectors ×256 data values, neighbors and passive accesses. |
| 14 | Independent vectors distinguish selected policies from measurements, enforce exact geometry and preserve ABI/transactionality. (04) | ✓ VERIFIED | C1/C2/C6; `docs/hardware/cartridge.md` explicitly qualifies extended hotspots/startup; exact sizes and no-allocation refusals precede installation. |
| 15 | Fractalus maps 32768 ROM and 2048 physical RAM bytes, with every 4000–7FFF address reaching its intended compressed-bit alias. (05) | ✓ VERIFIED | `src/cart.c:90,850,907`; C2 independently enumerates page pairs/four mirrors, 16384 addresses, 2048 physical locations, eight aliases each and multiple patterns. |
| 16 | Valid v4 Linear/A8 accepts only the Fractalus geometry; valid SuperGame/A8 and BankSet/A8 stay unsupported without v3 fallback. (05) | ✓ VERIFIED | `src/cart.c:259,463,544`; C2 tests supported/unsupported combinations over contradictory legacy declarations, malformed reserved-block fallback and hash override. |
| 17 | All-data Absolute selectors and all-address Fractalus vectors independently prove selected policies and bounds. (05) | ✓ VERIFIED | C2 uses independently tabulated Absolute transitions from both initial banks and explicit Fractalus page vectors, not production mapper results. |
| 18 | Load/power cycle restores bank0/zero volatile RAM; console RESET remains distinct and Fractalus exposes no battery storage. (05) | ✓ VERIFIED | `tests/core/test_core.c:1511` and `src/instance.c:184,234`; C3 runs three boards, public RESET/VBLANK observation, reload/cycle, physical span and peer checks; C5 confirms no libretro save span. |
| 19 | One owned diagnostic per board reports independent numbers through all three hosts within600 frames. (06) | ✓ VERIFIED | C4/C15; strict mailbox IDs10/11/12, 27/21/24 records, FRAME_CAP600, actual runner and dynamically loaded shipped module. |
| 20 | Diagnostics expose switched/fixed banks and physical aliases; exhaustive coverage remains in independent C vectors. (06) | ✓ VERIFIED | Read all three assembly/configs and CSV; selector code remains fixed, independently tagged data drives reads; C2/C4. |
| 21 | Admitted fixtures rebuild from owned source and remain comparable with unchanged trusted-base rows. (06) | ✓ VERIFIED | C15 exact rebuild; A6 exact source/config/fixture Git pins and digests; C12 actual producer/join controls plus A1/A2 against the real base. |
| 22 | Activision fixed descriptors reference correct original-content halves; Absolute/Fractalus fixed ROM is mapped and switched windows are holes. (07) | ✓ VERIFIED | `libretro/libretro.c:781`; C5 requires every intended byte non-NULL/equal, explicit holes NULL and rejects missing/incorrect AM descriptor mutations. |
| 23 | Sixteen256-byte Fractalus aliases cover4000–4FFF over physical2048 bytes, all intended regions fit below32; higher mirrors are host holes. (07) | ✓ VERIFIED | `libretro/libretro.c:765`; C5 actual21-region/43520-byte map, zero wrong addresses, all65536 addresses checked; no authorized omission used. |
| 24 | Identified AM/OM pairs have equal mapped bytes, mailbox, video and audio with distinct raw identities and unchanged input. (07) | ✓ VERIFIED | C5 library/actual runner/loaded testdb module; headered/headerless AM/OM,600 frames,479591 PCM samples each, zero raw events disclosed. Numeric validity is separately judged. |
| 25 | Every board is explicitly selected by hostile-size/traffic/fuzz/property cases; refusals preserve deep state and resumed execution. (08) | ✓ VERIFIED | C6/C7 have31/29 replay seeds; C8–C11 force all three boards before random selection; loader deep ROM/RAM/register/video/nonzero PCM continuation in `tests/fuzz/loader.c:59`. |
| 26 | Post-load traffic/frames/power cycles allocate nothing; interleaved instances agree with independent controls. (08) | ✓ VERIFIED | C3 allocation/free counters and stable physical span; C11 four independent instances; C8–C10 equality/purity/input and warmed allocation-failure checks. |
| 27 | Owned diagnostics participate in native equality and applicable BE/WASM paths; old predicates/pins and lane statuses persist. (09) | ✓ VERIFIED | `tests/cmake/hashes.cmake` validates actual board/mailbox/frame/audio before output; A3/A4/A6/A8,47 identical native/BE rows and30 board rows. WASM/library/property stay advisory. |
| 28 | Documentation distinguishes selected profile policies, diagnostics, untested retail gameplay and PAL execution. (09) | ✓ VERIFIED | Agent reviewed README, hardware/embedding/runner/libretro/emulation/testing/compatibility/quality prose against source and ledger; A6 ledger remains unchanged; no unsupported inference found. |
| 29 | Current contract/review policy applies; authenticated additive-fixture comparison supplies the prerequisite without line admission. (09) | ✓ VERIFIED | DEC.10/ENGINEERING§11, substantive existing-module changes, clean review through source25919fe; C12 actual producer/join faults; A1/A2 authentic legacy-base policy retains retired key. |
| 30 | Each slice uses exactly three component ZIPs; all six native sums and authenticated worst-lane aggregate fit399937, with performance on the original testcard. (02 +09 deduplicated) | ✓ VERIFIED | Final A1/A2/A5 prove all18 actual ZIP sizes per run and worst375270/375263; historical per-slice package records remain scoped local evidence. Direct perf artifact3837262 matches both base repeats and original testcard configuration; advisory status preserved. |
| 31 | Only unique added owned board-fixture rows differ from independently obtained trusted manifest; old rows stay byte-identical. (10) | ✓ VERIFIED | `quality_fixture_identity` in `tests/cmake/quality.cmake:117`; C12 tests allowed additions, duplicates/deletions/mutations/unrelated paths, malformed metadata and later-base row protection. |
| 32 | Actual current/base production derives shared comparison identity only after proving the relation; compiler/platform/built-sample identity and exact join persist. (10) | ✓ VERIFIED | `quality_evidence.cmake:27` reads event-base Git objects independently in recursion; C12 real producer tests and A1/A2 require all eight complete environments equal. Sample exception is exactly one unique2→3 annotation with every other byte protected. |
| 33 | Ordinary quality controls, legal admission/rebuild, packages and399937 retain authority; plan10 itself claims no completed board outcome. (10) | ✓ VERIFIED | C12/C15, A1/A2/A5/A6; plan10 coverage and empty requirements-completed remain prerequisite-only. Current runtime maps are later slices, not a retroactive claim about10. |

**Score:** 33/33 truths verified; **behavior_unverified:** 0. No coincidental-reliance flag: production establishes board geometry, normalized ownership, ordering and startup; fixtures supply caller inputs instead of creating missing production preconditions.

### Required Artifacts

All 21 declared artifact entries passed `query verify.artifacts` (nine commands, each exit0). The20 distinct paths also received substantive and consumer checks:

| Artifact | Provides / substantive implementation | Wiring / verdict |
|---|---|---|
| `src/cart.c` | Exact declaration/geometry, retained match orientation, one-copy transaction, physical aliases, finite mapper switch | Public load/probe → bus/CPU/MARIA/peek; C1/C2/C6. ✓ VERIFIED |
| `include/muhtari.h` | API4 appended constants, frozen sizes/signatures | Linked API4 and older smoke clients, host names. ✓ VERIFIED |
| `tests/api/v4.c` | Constant/layout/runtime ABI and host bit-name assertions | Registered `abi.api_v4`, portable inventories; C13. ✓ VERIFIED |
| `runner/main.c` | Actual bounded --diagnostic RAM report, original raw probe identity | Compiled runner invoked by diagnostic/probe/equality callers; C4/C5/C14. ✓ VERIFIED |
| `tests/diag/driver.c` | Independent expectations and strict protocol/record judge; library/module paths | Registered batch and real runner report parsing; C4. ✓ VERIFIED |
| `tests/cart/test_cart.c` | Exhaustive independent selectors/windows/page pairs, real CPU execution | Registered cart cases; C1/C2. ✓ VERIFIED |
| `tests/core/test_core.c` | Three-board volatile state, RESET, reload, allocation/peer controls | Existing core registration; C3. ✓ VERIFIED |
| `tests/diag/activision.s` | Fixed-E000 selector code and tagged independent halves | Owned cfg/fixture, manifest, rebuild, C4/C15. ✓ VERIFIED |
| `tests/diag/absolute.s` | Fixed-code selector precedence/fixed-bank observations | Owned cfg/fixture, manifest, rebuild, C4/C15. ✓ VERIFIED |
| `tests/diag/fractalus.s` | Initial RAM, independent aliases/locations/patterns | Owned cfg/fixture, manifest, rebuild, C4/C15. ✓ VERIFIED |
| `tests/diag/expected.csv` | 27/21/24 selected numeric expectations with evidence labels | Driver loads these rows and rejects altered protocol/value fields; C4/C5. ✓ VERIFIED |
| `libretro/libretro.c` | Original-content half translation, physical aliases/fixed ROM | Actual built module and resolver; C4/C5. ✓ VERIFIED |
| `tests/libretro/rcheevos.c` | Mandatory all-byte map/hole checks, pair equality/corruption controls | Real adapter + pinned MIT resolver + testdb module; C5. ✓ VERIFIED |
| `tests/fuzz/run_cart.h` | Explicit three-board builder,131200 bound, audible control | Run replay and properties include/call builder; C6–C11. ✓ VERIFIED |
| `tests/prop/test_prop.c` | Deep state/PCM digest, warmed refusals, independent interleaving | Existing four registered properties; C8–C11. ✓ VERIFIED |
| `tests/cmake/hashes.cmake` | Real board/report validation before deterministic equality rows | runner.hashes_file + hosted equality callers; C12/A4. ✓ VERIFIED |
| `docs/emulation-tests.md` | Accurate numeric/provenance/portability/hostile contract | User docs ladder and tracked tests match described behavior. ✓ VERIFIED |
| `tests/cmake/quality.cmake` | Narrow trusted-manifest/sample relations and typed quality judge | Included/called by actual producer/join; C12/A1/A2. ✓ VERIFIED |
| `tests/cmake/quality_evidence.cmake` | Independent event-base Git resolution/rebuild, complete observations | CI six-native/ASan/hygiene producers and recursive controls. ✓ VERIFIED |
| `tests/policy/quality.cmake` | Actual consumer positives/faults, exact exit/regex judgments | Existing policy.quality.controls registration; C12. ✓ VERIFIED |

### Key Link Verification

The CLI found2/16 declarations by literal pathname;14 returned exit1 / “Target not referenced in source.” Those are syntactic misses, not accepted wiring evidence. These15 distinct links (the host-name→runner declaration appears twice) were traced through actual C includes, link-time object substitution, generated CMake arguments and runtime calls:

| From | To | Actual connection | Status |
|---|---|---|---|
| diag/tests.cmake | runner/main.c | TARGET_FILE muhtari-run → --frames600 --diagnostic → driver --mailbox | ✓ WIRED, C4 |
| api/v4.c | muhtari.h | Include + constants/runtime calls; ABI target links real library | ✓ WIRED, C13 |
| host/status_text.c | runner/main.c | muhtari_host linked; runner calls bit-name tables while printing identity/warnings | ✓ WIRED, C14 |
| cart.c | cart/test_cartdb.c | Test target replaces bundled tables but uses production cart object/lookup | ✓ WIRED, C1 |
| cart.c | cart/test_cart.c | Registered retail test calls real cart/bus CPU/MARIA/peek entry points | ✓ WIRED, C2 |
| core/test_core.c | instance.c | Public muhtari_load/power_cycle/set_input/run_frame calls | ✓ WIRED, C3 |
| diag/assemble.cmake | roms/manifest.txt | Current manifest passed to every source/config rebuild, SHA-256 checked | ✓ WIRED, C15 |
| diag/driver.c | diag/expected.csv | Generated EXPECTED path, read_expected and retail_judge consume real records | ✓ WIRED, C4 |
| libretro.c | muhtari.h | Retained input-orientation bit translates original content descriptors | ✓ WIRED, C5 |
| libretro/rcheevos.c | cart/test_cartdb.c | Resolver/test module/driver link replacement tables with owned OM key | ✓ WIRED, C5 |
| fuzz/run_cart.h | prop/test_prop.c | Header included, run_cart/control called in pool and warm refusal paths | ✓ WIRED, C8–C11 |
| cmake/hashes.cmake | diag/expected.csv | Tracked DIAG path + actual built DRIVER --mailbox call | ✓ WIRED, C12/A4 |
| workflows/ci.yml | quality_join.cmake | Downloaded exact six-native/ASan/hygiene(+direct perf) artifact set and bound SHAs | ✓ WIRED, A1/A2/A8 |
| quality_evidence.cmake | quality.cmake | FUNCTIONS_ONLY include and trusted relation calls before environment production | ✓ WIRED, C12 |
| policy/quality.cmake | quality_join.cmake | Actual current/base recursion outputs consumed by exact join and typed judge | ✓ WIRED, C12 |

### Data-Flow Trace (Level 4)

No application UI or database is involved. The relevant dynamic host observations were traced instead:

| Output | Real source and flow | Status |
|---|---|---|
| CPU/MARIA read | Caller ROM → validated private allocation/optional authorized normalization → board switch/latch or physical RAM → shared bus | ✓ FLOWING, independent C1/C2 observations |
| Runner diagnostic | Actual emulated6502 writes → system RAM $2200–$22FF → muhtari_get_memory → 512 hex digits → strict numeric judge | ✓ FLOWING, C4 |
| Achievement bytes | Retained caller content/orientation + physical instance RAM → descriptors → pinned resolver → every intended CPU byte | ✓ FLOWING, C5; deliberate holes are required |
| AM/OM behavior | Four actual inputs → library/runner/module600-frame runs → mapped/mailbox/video/raw/PCM observations and unchanged-input hashes | ✓ FLOWING, C5 |
| Equality artifact | Actual runner board and frame/audio/report fields → real mailbox verdict →47 named rows → exact native/BE equality | ✓ FLOWING, A4 |
| Quality maximum | Actual completed inventories/cache/three ZIP sizes per lane → authenticated observations + independent raw base → exact environment join → worst-lane reduction | ✓ FLOWING, A1/A2/A5 |

Static test fixtures and tag constants are intentional independent inputs/oracles. No live cartridge output was replaced by a static fallback.

### Behavioral Spot-Checks

All commands ran during this verification against unchanged accepted source. Run URL for local commands is N/A-local. Each named test completed in under10 seconds, and no full workspace suite was rerun. `ctest --test-dir build/ci -N -R ...` first enumerated the named tests (exit0).

For C1–C15, the exact command is `ctest --test-dir build/ci -R '^<name with [.] for each dot>$' --no-tests=error --output-on-failure`; verbose `-V` was used for C5–C13. Each invocation selects one named behavior (plus CTest-required fixture setup).

| ID | Named test | Result | Exit |
|---|---|---|---:|
| C1 | cart.identify.activision_swap | All layouts/banks/selectors/copy/refusal/priority checks pass,0.02s | 0 |
| C2 | cart.retail.boards | Independent all-address aliases and all-data selectors pass,0.01s | 0 |
| C3 | core.no_alloc_after_load | Three-board lifecycle/no-allocation/RESET/peer checks pass,0.18s | 0 |
| C4 | diag.retail | Three fixtures ×three actual hosts, persistent smoke and protocol/report/usage/transaction faults pass,0.58s | 0 |
| C5 | libretro.rcheevos |13/11/21 retail regions, zero wrong bytes; all four AM/OM forms,479591 PCM samples each; pair/descriptor/report controls pass,4.18s | 0 |
| C6 | fuzz.regress.loader |31 inputs; independent normalization/deep failure continuation pass,0.11s | 0 |
| C7 | fuzz.regress.run |29 inputs; explicit boards/traffic/deadlines including accidental-magic regression pass,0.05s | 0 |
| C8 | prop.split_run |100 seeded cases, all three boards; serial final command including setup0.73s | 0 |
| C9 | prop.peek_pure |100 seeded cases, all three boards; serial final command including setup0.84s | 0 |
| C10 | prop.input_at_vblank |100 seeded cases, all three boards; serial final command including setup0.84s | 0 |
| C11 | prop.instances_isolated |100 cases, seed20261020, explicit three-board coverage;1.03s | 0 |
| C12 | policy.quality.controls | Real producer/recursion/join and exact-exit/message faults pass,3.89s | 0 |
| C13 | abi.api_v4 | Runtime API/ABI/layout/constant/name assertions pass | 0 |
| C14 | runner.probe.sample_hash_match | Original hashes, trailers, orientation/assumption and missing-output faults;3/3 with setup | 0 |
| C15 | diag.rebuild | Exact owned-source diagnostic rebuilds pass,2.48s | 0 |

Verifier scheduling correction: separate concurrent CTest invocations initially raced the same disposable simple/eightysix fixture directories. Three property invocations exited8 without running their property. They were rerun serially with normal fixture setup and all passed (C8/C9/C10); no production change or relaxed assertion was made. The one-off prior-predicate assertion was also corrected to recognize the audio file's actual eight-column schema and revision field. Its final comparison exited0; neither harness-construction error is product evidence.

### Probe Execution

No active PLAN/SUMMARY declares a `probe-*.sh` script; none exists under `scripts/**/tests/`. The migration/tooling behavior is exercised by actual `policy.quality.controls` C12 and real artifact judges A1/A2. No executor PASS marker was substituted for a runnable probe. Step7c scripted-probe list is N/A, not a claimed probe pass.

### Hosted and Artifact Evidence

Fresh connector calls `github.fetch_commit_workflow_runs(repo_full_name="szTheory/muhtari", commit_sha="25919fe…")` and `github.fetch_workflow_run_jobs(..., run_id=...)` succeeded for all three runs. Direct CI has all20 jobs successful. Nightly has successful fuzz,10000-case property, full vectors and native/PowerPC equality jobs (rolling report correctly skipped). Release attempt2 has successful exact-head equality, rehearsal, README install and cleanup; conditional main/tag-only jobs are correctly skipped. Called-CI required producers/jobs also remain successful.

The connector omits raw Git-tree identities and full attempt/pagination facilities, so exact Git-tree/PR proof used read-only `gh api`. Source25919fe and PR merge2fe713c have identical tree857e54b77e3bf5ed89faa09a46ef6407cf8c68d7. PR24 identifies base617dfa7, source25919fe and that merge. Local HEAD65ae7c0 changes planning metadata only; `git diff --quiet 25919fe HEAD -- . ':!.planning'` exits0.

| ID | Command / evidence | Exit | Result / run |
|---|---|---:|---|
| A1 | `cmake -DDIRECTORY=build/phase6-acceptance/complete-direct -DOUT=build/phase6-acceptance/direct-join -DPOLICY=quality.json -DBASE_POLICY=build/phase6-acceptance/trusted-quality.json -DEXPECTED_SHA=2fe713c1074a63bbf89384d56200bd48060c0549 -DBASE_SHA=617dfa762ddb649839b925be5aa582180066fa78 -DPERF_REQUIRED=ON -DSOURCE=https://github.com/szTheory/muhtari/actions/runs/40031550434 -P tests/cmake/quality_join.cmake` | 0 | Nine-artifact exact join, current375270/base399385; [direct CI](https://github.com/szTheory/muhtari/actions/runs/40031550434) |
| A2 | Same unchanged join with `DIRECTORY=.../complete-called`, `OUT=.../called-join`, `EXPECTED_SHA=25919fe822cd6cc23a6984c13ac826796e7d9cfe`, `PERF_REQUIRED=OFF`, `SOURCE=https://github.com/szTheory/muhtari/actions/runs/40031550534`; same policy/base arguments | 0 | Eight-artifact exact join, current375263/base399371; [called CI / Release](https://github.com/szTheory/muhtari/actions/runs/40031550534) |
| A3 | For each `lane in ci be32`: `cmake -DJUNIT=build/phase6-acceptance/accepted-equality/build/${lane}/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake`; same with `junit-release.xml` and `-DLABELS=release` | 0 each | Exact612 normal +6 release, native and PowerPC; [Nightly](https://github.com/szTheory/muhtari/actions/runs/40031550533) |
| A4 | `cmake -DNATIVE=build/phase6-acceptance/accepted-equality/evidence/native -DBE=build/phase6-acceptance/accepted-equality/evidence/be -DEXPECTED_SHA=2fe713c1074a63bbf89384d56200bd48060c0549 -P tests/cmake/lane_equality.cmake` | 0 | Two byte-identical47-row files;30 rows belong to the three boards; [Nightly](https://github.com/szTheory/muhtari/actions/runs/40031550533) |
| A5 | Read-only Python assertions over `complete-{direct,called}` and corresponding archive directories: all eight current/base identities/environments/raw policies, six lanes ×exactly3 ZIPs, sums equal original observations and≤399937 | 0 | Direct/called totals below; actual base policy bytes equal `git show 617dfa7:quality.json`, including retired gate_lines |
| A6 | Read-only Python Git/predicate/source-pin audit: compare every source/config/fixture at manifest pin, SHA-256/owned-MIT fields, numeric counts; compare built/audio expectations allowing only explicit revision2→3 metadata; byte-compare ledger and perf/compat allow stores | 0 | Legal reproducibility and27/21/24 records; every prior numeric/hash predicate and independent allow store unchanged; N/A-local |
| A7 | `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` and release equivalent | 0 each | Parent's final current-source full-workflow612+6 files retain exact executions; no full suite rerun by verifier; N/A-local |
| A8 | Fresh GitHub connector run/job queries for40031550434,40031550533,40031550534 | success | Final CI/Nightly/Release conclusions successful; every applicable required/acceptance job green |
| A9 | `gh api repos/szTheory/muhtari/git/commits/<source-or-merge> --jq '{sha: .sha, tree: .tree.sha}'` twice; `gh api repos/szTheory/muhtari/pulls/24 --jq '{head: .head.sha, base: .base.sha, merge: .merge_commit_sha}'` | 0 each | Source and tested merge same tree; exact PR source/base/merge binding; [PR24](https://github.com/szTheory/muhtari/pull/24) |

| Native lane | Direct three-ZIP sum | Called three-ZIP sum | Ceiling |
|---|---:|---:|---:|
| linux-arm64 |266111|266111|399937|
| linux-x64 |262384|262384|399937|
| macos-arm64 |215779|215775|399937|
| macos-x64 |218125|218147|399937|
| windows-arm64 |351291|351294|399937|
| windows-x64 |375270|375263|399937|

All ordinary typed, source, SHA, configuration, raw trusted-policy and complete-environment guards remain active. The only retired metric is maintenance LOC; its absence does not retire archive limits or other contracts. No old failed665/654-line construction, byte overrun or candidate prediction is reclassified as a pass. Existing `gap.*` unavailable measurements and library/WASM/property advisory status remain explicit. Direct testcard performance artifact has head3837262 Ir/frame and both base repeats3837262; it remains advisory exactly as its authentic availability field says, despite the successful run.

Rehearsal is verified by the actual successful lifecycle/cleanup jobs. This report does not use a published temporary artifact as a product requirement or claim a main merge occurred. Ship owns merge-dependent confirmation.

### Requirements Coverage

| Requirement | Active plans | Status | Evidence |
|---|---|---|---|
| BOARD-01 — correct Activision banking, identified-layout equivalence, raw identity inspectable |02–04,06–10|✓ SATISFIED|C1/C4/C5/C14; all windows/banks/selectors, private normalization, four input layouts and raw identity/probe faults; A3/A4/A8 |
| BOARD-02 — Absolute fixed/switched windows and power-on banks |02,05–10|✓ SATISFIED|C2/C3/C4/C6/C8–C11; all256 data values from both banks, neighbors,21-record real fixture and bank-zero lifecycle; hosted equality |
| BOARD-03 — physical Fractalus aliases and bounded accesses |02,05–10|✓ SATISFIED|C2/C3/C4/C5/C6/C7; all16384 addresses/2048 physical bytes/eight aliases,24-record fixture, exact host holes and deep lifecycle; hosted ASan/fuzz |

All requirements mapped to Phase6 in REQUIREMENTS.md occur in active plans; **orphaned requirements:0**. Later saves, controllers, PAL, BankSet/XBoard/XM and full-library accounting remain outside this phase. Full-roadmap inspection found no gap to defer.

### Decision Coverage

Owner query `query check.decision-coverage-verify .planning/phases/06-retail-cartridge-boards .planning/phases/06-retail-cartridge-boards/06-CONTEXT.md` exited0: **18/18**, not_honored empty. Handler message: “All trackable CONTEXT.md decisions are honored by shipped artifacts.” This heuristic is advisory; current accepted D-16/D-17 supersede the preserved historical allocation text.

### Test Quality Audit

| Test file / consumer | Requirement | Active | Skipped | Circular | Strongest assertion | Verdict |
|---|---|---|---:|---|---|---|
| cart/test_cart.c |BOARD-01–03|Registered and executed|0|No|Independent tables/page pairs, exhaustive values plus real CPU observations|PASS|
| core/test_core.c |BOARD-01–03|Existing lifecycle consumers executed|0|No|Multi-step public RESET/load/cycle, deep capacity/allocation and peer state|PASS|
| diag/*.s + expected.csv + driver.c |BOARD-01–03|Three real fixtures,72 records,three actual hosts|0|No|Independent numeric expectations; complete protocol,CRC,ID/flags/value; corrupt-report controls|PASS|
| libretro/rcheevos.c + tests.cmake |BOARD-01–03|Actual loaded adapter/module/runner|0|No|Mandatory every intended byte/hole, independent raw hashes and pair observations; mutations rejected|PASS|
| fuzz/loader.c, run.c, seeds |BOARD-01–03|31/29 replays + hosted engines|0|No|Deep post-refusal/resume equality, required nonzero PCM, explicit selection and deadlines|PASS|
| prop/test_prop.c |BOARD-01–03|Four100-case tests; hosted10000-case advisory run|0|No|Pointer-cleared deep state + ROM/RAM/CPU/picture/PCM and independent interleaving|PASS|
| api/v1.c–v4.c + ABI controls |BOARD-01|Exact native/BE inventories; fresh API4|0|No|Frozen values/layout/signatures and actual linked runtime versions|PASS|
| policy/quality.cmake |BOARD-01–03 prerequisite|Actual producer recursion/join|0|No|Independent event-base Git, complete equality and specific exact-exit/regex fault rejection|PASS|

**Disabled requirement tests:**0. **Circular expectation generators:**0. **Insufficient must-have assertions:**0. Build/report output writes are actual observations or disposable fault fixtures; they do not regenerate the independent golden CSV.

Disconfirmation limits: (1) host achievement visibility is deliberately partial—switched windows and higher Fractalus mirrors are holes, explicitly required by the accepted contract; (2) the silent diagnostics' empty raw-audio equality proves silence/layout parity, not a new sound workload; audible warmed-failure controls and unchanged sound fixtures supply the nontrivial audio evidence; (3) no new dedicated diagnostic-stdout write-error fault injection was found. The runner handles printf/fflush errors, but that optional OS-error control is not used as proof of a must-have. None is an unresolved phase-goal defect or a reason to invent owner UAT.

### Anti-Patterns Found

| Scope | Finding | Classification | Impact |
|---|---|---|---|
| All surviving changed non-planning text files from617dfa7..HEAD | No unreferenced TBD/FIXME/XXX, TODO/HACK/PLACEHOLDER debt markers | None | No debt blocker |
| Requirement-linked tests | Broad skip regex hits were `pending` emulated state and `expect_exit` names; exact declaration scan found no disabled/skip test | Info | No skipped acceptance |
| Core/runner/adapter/data paths | No stub or static fallback replaces emulated data; initial/null state is populated by actual load/execution | None | No missing/orphaned/hollow artifact |
| Final source architecture | Existing board switch, instance state, one allocation and private helper owners retained; no new dependency/framework | None | Meets current bounded review contract |

### Preserved Probe and Prohibition Metadata

The active plans deliberately carry descriptorless, unclassified/unresolved `verification:null` probe metadata. It is preserved here and in source plans; it is not upgraded into a test-tier/backstop/judgment descriptor, silently dismissed, or counted as an additional green test. Three distinct product-transparency prohibitions (BOARD-01 is repeated in03/04) remain flagged-unverified **as probe metadata**:

| Requirement / prohibition | Retained metadata | Agent intent-review evidence |
|---|---|---|
| BOARD-01: do not call assumed orientation detected or substitute normalized identity |unclassified / unresolved / verification:null|Actual unknown header warns “assumed”; docs distinguish detection; C5/C14 preserve original hashes/input, `cart_lookup` alone authorizes normalization. No contrary product claim found.|
| BOARD-03: do not infer physical8KiB from A8 or complete achievement visibility from bus aliases |unclassified / unresolved / verification:null|Docs explicitly state physical2048 bytes, metadata qualification, partial16-alias descriptors and higher holes; C2/C5 prove actual capacities/maps. No contrary claim found.|
| BOARD-02: do not promote diagnostics to untested retail gameplay, PAL completion or newly measured hardware accuracy |unclassified / unresolved / verification:null|README/compatibility/hardware/testing prose and unchanged ledger preserve those limits. No contrary claim found.|

The phase's spec-less BOARD-01/02/03 edge rows likewise remain unclassified/unresolved/verification-null; no passing descriptor or new security canon is fabricated. The score covers the33 concrete roadmap/PLAN truths, not these descriptorless metadata records. This is the explicit accepted metadata disposition in03–05/09. Product-intent review was performed by the verifier under AGENTS rule6; it requires neither identity-bound consent nor owner hardware. No typed test-tier prohibition was declared without enforcement, and no typed judgment-tier item is silently green-lighted.

### Human Verification Required

None. The agent completed documentation/intent review and inspected actual integration evidence. No behavior-dependent truth is unexercised, no visual redesign is part of this phase, and there is no identity- or owner-hardware-bound acceptance. Selected uncertain hardware edges are documented profile policies under D-08, not a new accuracy claim. No `<human-check>` block exists in the active plans.

### Gaps Summary

No blocking gap, missing/stub artifact, broken link, deferred gap or unresolved runtime-behavior verification remains. BOARD-01–03 and the phase goal are achieved within their documented evidence limits. The preserved descriptorless probe metadata above remains explicitly unresolved without inventing a passing test descriptor. No commit, product edit, tracking edit, push or subsequent GSD step was performed by this verifier.

## Fingerprint and Status Ownership

The covered set and digest are copied verbatim from `query verification.fingerprint`; no digest was hand-computed. Its command included all surviving implementation changes from617dfa7 plus supporting bus/internal/build/provenance/inventory/equality owners. The CLI rejected deleted-file arguments on the first attempt; the successful invocation uses surviving files and covers retirement through its current consumers. The report itself and unrelated tracking metadata are excluded by the owner.

After writing, `query verification.status .planning/phases/06-retail-cartridge-boards --pick status` exited0 and returned `passed`. No alternate routing status was written into frontmatter.

---

_Verified: 2026-10-23T22:52:14Z_  
_Verifier: the agent (gsd-verifier)_

