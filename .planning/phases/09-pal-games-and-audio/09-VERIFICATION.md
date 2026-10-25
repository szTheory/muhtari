---
phase: 09-pal-games-and-audio
verified: 2026-10-25T21:55:43Z
status: passed
score: 42/42 must-haves verified
covered_files: [
    ".github/workflows/be-equality.yml",
    ".github/workflows/ci.yml",
    ".github/workflows/nightly.yml",
    ".planning/phases/09-pal-games-and-audio/09-01-PLAN.md",
    ".planning/phases/09-pal-games-and-audio/09-01-SUMMARY.md",
    ".planning/phases/09-pal-games-and-audio/09-02-PLAN.md",
    ".planning/phases/09-pal-games-and-audio/09-02-SUMMARY.md",
    ".planning/phases/09-pal-games-and-audio/09-03-PLAN.md",
    ".planning/phases/09-pal-games-and-audio/09-03-SUMMARY.md",
    ".planning/phases/09-pal-games-and-audio/09-04-PLAN.md",
    ".planning/phases/09-pal-games-and-audio/09-04-SUMMARY.md",
    ".planning/phases/09-pal-games-and-audio/09-05-PLAN.md",
    ".planning/phases/09-pal-games-and-audio/09-05-SUMMARY.md",
    ".planning/phases/09-pal-games-and-audio/09-06-PLAN.md",
    ".planning/phases/09-pal-games-and-audio/09-06-SUMMARY.md",
    ".planning/phases/09-pal-games-and-audio/09-07-PLAN.md",
    ".planning/phases/09-pal-games-and-audio/09-07-SUMMARY.md",
    ".planning/phases/09-pal-games-and-audio/09-08-PLAN.md",
    ".planning/phases/09-pal-games-and-audio/09-08-SUMMARY.md",
    ".planning/phases/09-pal-games-and-audio/09-CONTEXT.md",
    ".planning/phases/09-pal-games-and-audio/09-RESEARCH.md",
    ".planning/phases/09-pal-games-and-audio/09-REVIEW-DISPOSITION.md",
    ".planning/phases/09-pal-games-and-audio/09-REVIEW-FIX.md",
    ".planning/phases/09-pal-games-and-audio/09-REVIEW.md",
    "AGENTS.md",
    "ASSET_POLICY.md",
    "CMakeLists.txt",
    "CMakePresets.json",
    "PROVENANCE.md",
    "README.md",
    "THIRD-PARTY-NOTICES.md",
    "cmake/MuhtariTests.cmake",
    "docs/compatibility.md",
    "docs/embedding.md",
    "docs/emulation-tests.md",
    "docs/hardware/audio.md",
    "docs/hardware/bus.md",
    "docs/hardware/controllers.md",
    "docs/hardware/maria.md",
    "docs/libretro.md",
    "docs/maintaining.md",
    "docs/pal-tests.md",
    "docs/quality.md",
    "docs/runner.md",
    "docs/testing.md",
    "host/palette.c",
    "host/palette.h",
    "include/muhtari.h",
    "libretro/libretro.c",
    "libretro/muhtari_libretro.info.in",
    "pins.txt",
    "quality.json",
    "runner/main.c",
    "runner/movie.c",
    "runner/movie.h",
    "scripts/hygiene.sh",
    "src/boot.c",
    "src/bus.c",
    "src/cart.c",
    "src/cpu.c",
    "src/frame.c",
    "src/instance.c",
    "src/internal.h",
    "src/maria.c",
    "src/pokey.c",
    "src/ports.c",
    "src/synth.c",
    "src/tia.c",
    "tests/api/v3.c",
    "tests/audio/expected.txt",
    "tests/audio/test_contracts.c",
    "tests/audio/test_spectral.c",
    "tests/audio/test_stress.c",
    "tests/audio/tests.cmake",
    "tests/boot/test_boot.c",
    "tests/boot/tests.cmake",
    "tests/built/expected.txt",
    "tests/cmake/check_junit.cmake",
    "tests/cmake/hashes.cmake",
    "tests/cmake/info_claims.cmake",
    "tests/cmake/lane_equality.cmake",
    "tests/cmake/quality.cmake",
    "tests/cmake/quality_evidence.cmake",
    "tests/cmake/quality_join.cmake",
    "tests/compat/ledger.csv",
    "tests/compat/tests.cmake",
    "tests/core/test_core.c",
    "tests/core/test_pal.c",
    "tests/core/tests.cmake",
    "tests/diag/assemble.cmake",
    "tests/diag/driver.c",
    "tests/diag/expected.csv",
    "tests/diag/pal.a78",
    "tests/diag/pal.cfg",
    "tests/diag/pal.s",
    "tests/diag/tests.cmake",
    "tests/host/test_palette.c",
    "tests/input/test_input.c",
    "tests/input/tests.cmake",
    "tests/inventory/ci.txt",
    "tests/inventory/wasm32.txt",
    "tests/libretro/host.c",
    "tests/libretro/tests.cmake",
    "tests/maria/test_maria.c",
    "tests/policy/quality.cmake",
    "tests/policy/tests.cmake",
    "tests/retroarch/run_retroarch.cmake",
    "tests/roms/manifest.txt",
    "tests/runner/test_movie.c",
    "tests/runner/test_runner.c",
    "tests/runner/tests.cmake",
    "tools/compat_doc.cmake"
  ]
covered_digest: "v3:sha256:f6e73a16e3bf55d39435071b777c34b75a609b85bcdc1d9bece6f04215dc85a0"
behavior_unverified: 0
overrides_applied: 0
decision_coverage:
  honored: 19
  total: 19
  not_honored: []
human_verification: []
source_acceptance:
  runtime_fix: af9dc83f9ff68ea14c5a007218a3477a3c2f2218
  reviewed_branch: 419bb31faa2b7dd0c1586879e478ae8dd4ab3d6e
  tested_merge: df6aeefcd4e6e5f13c4350f3a6fe71d202a0940d
  tested_tree: f2e4a895cc9f4eaaf0e4108cda087f0ffc674c3c
  independent_main: 2cbbd6a78dabe2bc098615d49f7118231cc05626
  canonical_summary_receipt: 8814d3e80e1221cc0da71b84560c4f001852212a
  ci_run: https://github.com/szTheory/muhtari/actions/runs/40233948646
  ci_attempt: 1
  nightly_run: https://github.com/szTheory/muhtari/actions/runs/40233948516/attempts/2
  nightly_attempt: 2
prohibition_reviews:
  - source_plan: 09-06
    verification: judgment
    disposition: non_authoritative_agent_review
    intent: "No prohibited emulator expression or private game/firmware/loader/save bytes."
    flagged: true
    note: "unverified-prohibition — reviewed by the agent under AGENTS rule6; no routine owner action."
  - source_plan: 09-08
    verification: judgment
    disposition: non_authoritative_agent_review
    intent: "Do not portray fitted handoff, retained mix or optics as measured production hardware or universal retail accuracy."
    flagged: true
    note: "unverified-prohibition — reviewed by the agent under AGENTS rule6; no routine owner action."
assumption_classification_flags:
  - PAL-01
  - PAL-03
---

# Phase 09: PAL games and audio Verification Report

**Phase Goal:** Run PAL content at the console timing and report matching video/audio geometry to every host.
**Verified:** 2026-10-25T21:55:43Z
**Status:** passed
**Re-verification:** No — initial verification; no prior phase VERIFICATION report existed.

## Goal Achievement

The verification contract combines all four roadmap success criteria with all 38 PLAN truth declarations. The PLAN rows below retain their added numeric, lifecycle, safety, evidence, and confidence obligations; none narrows a roadmap criterion. All eight plans declare PAL-03; plans 01–04, 07, and 08 also declare PAL-01/PAL-02. There are no orphaned Phase 09 requirements. Mode is ordinary, not MVP.

Evidence comes from production call chains, independently judged numbers, executed test results and authentic run artifacts. SUMMARY narration alone is not evidence. The baseline is main `2cbbd6a78dabe2bc098615d49f7118231cc05626`. The source review covers the 65 changed implementation/test/documentation paths and unchanged owners needed to trace their behavior.

### Observable Truths

All rows are **VERIFIED**. IDs R1–R4 identify the roadmap contract; Pxx.y identifies the corresponding PLAN truth in declaration order.

| ID | Observable truth | Evidence |
|---|---|---|
| R1 | PAL metadata/override runs 313-line timing, PAL MSTAT/DMA and documented fast/BIOS handoffs. | `internal.h:29`, `boot.c:44`, `maria.c:503`, `instance.c:333`; active `core.pal.tracer/lifecycle`, boot and `diag.pal` tests. |
| R2 | Library, actual runner and loaded libretro agree on PAL height, rational rate, palette and aspect. | `muhtari_get_info`; runner's post-load allocation/info/PNG; adapter's selected AV; independent `host.palette`, runner and both loaded-format tests. |
| R3 | Sustained PAL retains sample carry and supported-platform native/raw/PCM equality. | `synth.c:428`, `driver.c:2101`; 601 calls, 600 full intervals, 577879 samples/remainder47029; fresh native/PPC equality artifacts. |
| R4 | Existing NTSC output/predicates remain frozen. | Old expected.csv, ledger and manifest are byte-exact prefixes of current files; first342 equality rows match the actual successful main-run artifact from CI40203183179 byte for byte; frozen clients and prior regression workflow pass. |
| P01.1 | Existing size-tagged clients use existing metadata/override precedence, ABI1/status12 and transactional failure; no content stays NTSC. | `instance.c:210,309–383`, unchanged layout assertions/header declarations; `core.pal.lifecycle`, `abi.api_v3`, load-preservation and allocation tests. |
| P01.2 | One scheduler uses 7093788 ticks/s, 454×313/142102, 313 physical stats, MSTAT7306/139874, last DMA139860 and no line309 request. | Selected constants `internal.h:29–36`; active region bounds and event filter `maria.c:370,503–640`; independent literal `edges()` and executed ROM DMA predicates. |
| P01.3 | Fast snapshot140016 has future deadline281976/elapsed141960; real BIOS starts0/deadline139874 independent of 4/16KiB mapping. | `boot.c:44–146`; literal `snapshot()`, `bios.k_first_fetch`, `bios.handoff_simple`, synthetic mapping/reset/refusal and stripped OpenBIOS integration. |
| P01.4 | Info exposes320×292, FPS3546894/71051, PAR1843750/1773447; exact/padded extents and validation are safe. | `instance.c:129–163`, `frame.c:38–64,86–91`; lifecycle tests cover outer canaries, each row's padding and maximum pitch65536. |
| P01.5 | Automatic PAL reaches the real runner with queried storage,93440 native bytes,320×292 PNG and unchanged default/probe text. | `runner/main.c:1322–1362,1438–1443`; `runner.load.pal`, `runner.region.pal`, PNG and probe controls. |
| P01.6 | Both libretro formats use292 actual/293 maximum rows,962 capacity and selected AV; release/failure resets fallback without illegal load-time AV callbacks. | `libretro.c:62–67,299–313,486–506,1420–1498`; both actual loaded modules test callback absence, buffers and fallback. |
| P01.7 | PAL carry/work/FIFO/overshoot/signed-mass/width bounds are proved without changing NTSC ordering. | `test_pal.c:164–278` independently enumerates591149 phases and4000 alignments and asserts962/1103,20730/141 and all finite-width bounds; `core.pal.bounds` executed. |
| P01.8 | Gain18360 and85:342 remain; PAL analog balance is qualified as unverified. | Literal assertions `test_pal.c:243–245`, unchanged mix/kernel/pole, explicit qualification `docs/hardware/audio.md:41`. This verifies the declared compatibility policy, not physical calibration. |
| P01.9 | Pitch319/65537 and out-of-range aim fail;320/65536 and valid last rows pass; one-short capacity executes nothing. | `test_pal.c:511–640` compares full live state/callbacks, exact sample sentinel, pitch extent/padding and both region aim boundaries. Fresh `core.pal.lifecycle` passes. |
| P01.10 | Products are bounded before narrowing, long epochs use quotient/remainder, and floats remain at host boundaries. | `test_pal.c:248–274`, `synth.c:437–441`, long-epoch audio vectors, AV conversion only in adapter; nofp/integer/architecture lanes. |
| P01.11 | Independent snapshot/event/hostile-ROM checks prove specified engineering behavior without pretending the PAL-01 edge classifier was resolved. | Literal snapshot/edges, JAM/WSYNC/pathological-DMA/interrupt vectors `test_pal.c:360–465`; executed tracer/lifecycle. Serialized unclassified-assumption history remains explicitly flagged below. |
| P02.1 | Physical keypad/EEPROM use ceil2838/35469; paddles scale the unrounded fit by6502639/6562500 within uint64. | `ports.c:26,107–116,421`; independent keypad/paddle/busy deadline−1/at/+1 vectors, changed pots and unpolled/JAM paths in active input/core tests. |
| P02.2 | Motion stays454 ticks; optical fits and lower PAL raster307/x319/y291 work across wrap and NULL video without RGB input. | `maria.c:447–479` feeds native bytes to ports; `ports.c:227–255` uses active height/period; `input.lightgun` tests both ports, NULL, kill/dark/offscreen and delayed wrap. |
| P02.3 | Battery attachment/committed bytes/generations survive power cycle and failed region load; no post-load allocation or instance interference. | Load commits after cart allocation; `power_cycle` settles due writes first; both-region `core.battery.*`, `core.no_alloc_after_load`, lifecycle and independent-instance tests execute. |
| P03.1 | Legacy palette stays NTSC; all256 licensed PAL RGB values and independent RGB565 truncations agree. | `host/palette.c:95–205`, `test_palette.c:20–112`; source-pinned literals, legacy parity, notices and release integrity checks. |
| P03.2 | Real runner's opt-in info reports effective region/geometry/FPS/PAR/palette, native extent and PNG match queried info, old text remains. | `main.c:1333–1362`; actual-process `runner.region.pal`, info/default/probe/PNG assertions and reverse overrides. |
| P03.3 | PAL movie syntax and override conflicts are preserved; effective-height validation is immutable and precedes output/save effects. | `movie.c:265–270`, `main.c:813,1338–1340`; both-version parser and actual replay invalid-row/conflict/side-effect controls. |
| P03.4 | Runner uses queried capacity/geometry and raw observation before load, producing actual native/raw/PCM output. | `main.c:1322–1348`; allocation-failure owner tests and real601-call binary comparison in `diag/tests.cmake:19–39`. |
| P04.1 | Loaded libretro reports PAL320×292/293, exact-derived FPS/aspect and48kHz in both formats/audio callback forms. | `libretro.c:486–506,983–1003`; `host.c:569–582,742–799` uses independent literals and exact stereo comparisons. |
| P04.2 | Region is captured on load and reload-only; run/reset cannot retime it; auto/explicit palette affects presentation only. | Region option read only in content loading, palette updates use selected_region; `host.c:817–843` edits option, runs, resets and checks old AV before reload. |
| P04.3 | Same-module region/unload/card/failure/deinit/reinit resets AV/palette/pointers; adapter release-before-load differs from library transactionality. | `lr_release`, `retro_load_game`; `host.c:817–903` checks null native/memory pointers, fallback metadata and subsequent sessions. |
| P04.4 | No SET_SYSTEM_AV_INFO/SET_GEOMETRY occurs in load. | Production adapter contains neither callback call; host environment records either command and asserts count0 after load/lifecycle. AV is queried after success. |
| P05.1 | Raw horizon/source order, TIA456 and stretched POKEY φ2 through HALT remain; NULL and capacity errors preserve state. | `synth.c:299–364,428–456`; `test_contracts.c:492–672` checks actual timestamps632/1048, pending boundary events, one-short live-state equality and NULL parity. |
| P05.2 | Positive/negative extremizers at all4000 alignments reach independent real-ring/blocker endpoints with zero clips and gain18360. | `test_pal.c:187–239` constructs sign streams from independent coefficient prefixes, then invokes production synth; endpoint/clip/work assertions execute in `core.pal.bounds`. |
| P05.3 | Arithmetic591149-frame recurrence gives568408000/rem0; full/startup/BIOS fractions stay separate. | `test_pal.c:164–185`; active tracer and audio frames assert961/rem313811, fast960/rem336960 and BIOS946/rem269046. |
| P05.4 | Unverified85:342 is retained; physical-frequency quality, headroom and rejected bypass remain authoritative. | `test_spectral.c:79,472` selects physical tick rates and runs both regions with unchanged judges; `audio.spectral.controls` and stress execute. |
| P05.5 | Independent transition/carry/extremizer/spectral/equality checks establish specified PAL-03 behavior without inventing resolved edge classification. | Numeric/value/behavior tests above and fresh equality; unresolved classification history is separately disclosed, not a claim of calibrated analog fidelity. |
| P06.1 | Only the named owned PAL fixture addition extends comparability; every old byte/order/digest and raw base identity stays protected. | `quality.cmake:117–156` removes only exact allowed rows then compares all remaining bytes; genuine-base producer controls and direct byte-prefix assertions. |
| P06.2 | Actual recursive producer outputs reach the unchanged authenticated join; malformed, duplicate, forged, mixed and missing identities fail. | `policy/quality.cmake:645–839` calls production producer/join over real Git base objects, mutates consumer evidence and requires precise failures; full `policy.quality.controls` passes. |
| P06.3 | All NTSC numeric/digest predicates and permitted narrow annotation rules remain; no rebaseline. | Expected.csv/manifest/ledger old bytes exact; API6/ABI1/behavior5 retained; sample data and pins unchanged; historical transition guards retained. |
| P07.1 | Owned executed ROM measures raster/wrap/progress/lower-row/source clocks before hash acceptance. | MIT `pal.s`, eight fact/fit rows in expected.csv; `driver.c:1790–1828,2101–2169` and `retail_judge` independently judge values/CRC first. |
| P07.2 | Library, real runner and both loaded formats run startup plus600 full intervals; incomplete or mutated evidence fails. | `diag.pal` never shortens on MUHTARI_SHORT_RUNS; driver601 loop and exact report parser; actual599-full session and CRC-valid/wrong-number/trailing/missing controls reach the same judge/writer. |
| P07.3 | Zero-carry600 frames gives576918/rem301218; actual601 calls gives577879/rem47029 with startup separately960/rem336960. | Independent arithmetic in bounds and driver, per-call counts and final tick85543176; complete numeric report and output matching. |
| P07.4 | Source rebuild/admission precede additive PAL hash rows; old NTSC rows remain byte-identical. | `diag.rebuild`, manifest-owned source pin/digest, actual admitted-row producer tests; `hashes_pal_report` invokes numeric judge before614 PAL rows after342 NTSC rows. |
| P08.1 | Independent numbers precede all accepted hashes and fresh exact-source lanes preserve authority. | Verified driver/writer call chain; final CI/Nightly source, inventories, sanitizer/nofp/vectors/fuzz/equality detailed below; advisory properties/WASM never promoted. |
| P08.2 | Old NTSC output, authenticated current/base packages and armed+1% guard remain. | Exact old prefixes/hash block, production quality join with trusted event base and actual three-ZIP producer, current/base test-card perf comparator. |
| P08.3 | Exactly two real hosted frontend cases and six-native packages retain notices/assets/integrity and source/base provenance, without retired quotas. | Unchanged CI frontend job DMA/battery cases and support budget; all18 actual current ZIPs and six authenticated independently built base aggregates; final artifact audit below. |
| P08.4 | Released claims separate diagnostic support and licensed palette from fitted startup/optics and unverified mix; no routine owner UAT. | README, generated compatibility, embedding, hardware/audio/controller and PAL guides read against production source/tests; claims exclude retail universality and physical calibration. |

**Score:** 42/42 truths verified; 0 present-but-behavior-unverified. No overrides applied. Repeated roadmap themes add no reduced scope: each PLAN row above includes its own additional numeric/error/lifecycle obligation.

### Required Artifacts

All27 PLAN artifact declarations passed `query verify.artifacts` (eight invocations, exit0). Existence checks were followed by substantive source reading and consumer tracing. Repeated declarations are grouped below.

| Artifact(s) | Substantive implementation and consumer | Status |
|---|---|---|
| `src/internal.h` | Selected timing,313 physical statistics,962 work samples/1103-cell FIFO, instance-owned state; used by existing scheduler/boot/MARIA/synth/ports. | VERIFIED |
| `src/boot.c` | Independent fast snapshot/strict-future deadline and real BIOS reset/mapping; called by instance power_cycle after successful installation. | VERIFIED |
| `src/ports.c` | Selected physical-time and native optical scheduling; used by bus deadlines, input APIs and MARIA native emission. | VERIFIED |
| `tests/core/test_pal.c` | Literal snapshot/edges, full carry/phase/mass/extremizers and lifecycle controls; three registered ordinary/portable cases execute. | VERIFIED |
| `tests/input/test_input.c`, `docs/hardware/controllers.md` | Real pin/read/write vectors, lower-row/light and time deadlines; registration/inventories plus factual model documentation. | VERIFIED |
| `host/palette.h` | Narrow region-aware helpers implemented in palette.c; runner and adapter consume them; independent256-value oracle and notices. | VERIFIED |
| `runner/main.c`, `runner/movie.c` | Actual CLI post-load storage/info/PNG/hash and immutable effective-region replay validation; exercised through binary and allocation-injection owner tests. | VERIFIED |
| `libretro/libretro.c`, `tests/libretro/host.c`, `docs/libretro.md` | Loaded session AV/palette/lifetime implementation, both format callbacks and documented after-load/reload contract. | VERIFIED |
| `tests/audio/test_contracts.c`, `tests/audio/test_spectral.c` | Runtime clocks/horizons/capacity/null, long epochs and independent physical-frequency/headroom/bypass judges; ordinary CI execution. | VERIFIED |
| `tests/cmake/quality.cmake`, `tests/policy/quality.cmake` | Narrow raw fixture identity and real producer/join pass/fault controls; hosted/local executed policy case. | VERIFIED |
| `tests/diag/pal.s`, `pal.cfg`, `pal.a78` | Owned MIT source/config, pinned reproducible admitted bytes with SHA25667638144…74b216; rebuilt through existing assembler recipe. | VERIFIED |
| `tests/diag/driver.c`, `tests/cmake/hashes.cmake` | Independent protocol/value/source-clock/600-full-run judge and exact equality writer/schema; real runner/modules then native/PPC lane consumers. | VERIFIED |
| `tests/compat/ledger.csv`, `docs/compatibility.md`, `docs/testing.md` | Only truthful PAL diagnostic row appended; actual generator/freshness and capability claims match executed evidence. | VERIFIED |

### Key Link Verification

All26 PLAN link declarations were traced, including reverse CMake/test registrations. The filename-reference heuristic `query verify.key-links` returned exit1 for each plan (only1/26 lexical matches): C translation units do not import each other's .c filenames, and several declared relations are reverse registrations or producer/consumer orchestration. Direct calls, linker targets, CMake registrations and executed runtime tests are authoritative; there is no unresolved link.

| PLAN link(s) | Actual connection | Status |
|---|---|---|
| 01: instance→boot | `muhtari_load` validates/stages, cart_install succeeds, effective_region commits, then power_cycle→boot_fast/boot_bios. | WIRED |
| 01: frame→MARIA | frame uses selected height/deadline; CPU bus dispatcher invokes MARIA selected events; emitted pixels/statistics use the same timing owner. | WIRED |
| 01: frame→synth | Preflight uses selected rational; reset stores it, coefficient/advance/render consume it; actual rendered count asserts preflight parity. | WIRED |
| 01: PAL test→registration | tests/core/tests.cmake compiles/links test_pal and registers tracer/bounds/lifecycle; exact ci/wasm inventories contain all three. | WIRED |
| 01/03: runner→instance | Observer is installed before load; successful load then info determines allocation/extent/capacity and every run. | WIRED |
| 01/04: libretro→instance | Content load succeeds before cached info/region; run passes selected capacity and callback extents; release zeros metadata and pointers. | WIRED |
| 02: ports→effective region | All converted recovery/pot/write deadlines consult load-committed effective_region; battery attachment stays separately owned. | WIRED |
| 02: MARIA→ports | render computes native sample even without caller rows and calls ports light sensing before raster/LRC changes. | WIRED |
| 03/04: hosts→palette | Runner PNG and LR_PALETTE call region-aware helpers; native hashes and optical path remain before RGB. | WIRED |
| 03: movie→runner | Parser stores PAL token/immutable cursor; CLI conflict then post-load full-session height validation precedes save/output/emulation loops. | WIRED |
| 04: loaded test→module | Actual shared module symbol resolution/callbacks, test-only native/raw seams and both pixel-format targets. | WIRED |
| 05: contracts→TIA/POKEY | Test links real core and calls actual source/bus owners; literal456-tick and stretchedφ2 vectors compare real observer records. | WIRED |
| 05: spectral→synth | Real ring/blocker PCM reaches independent exact-frequency judge and rejected nearest-sample controls. | WIRED |
| 06/08: policy→producer→join | Real event-base Git objects feed quality_evidence; actual current/base JSON, policy and identities feed quality_join in tests and CI orchestration. | WIRED |
| 07: source→assembler | Existing rebuild registration feeds pal.s/cfg to assemble.cmake and compares admitted SHA256 bytes. | WIRED |
| 07: driver→expected | retail_expected loads explicit fact/fit rows; retail_judge checks protocol/count/CRC/value before output acceptance. | WIRED |
| 07: orchestration→runner | diag/tests.cmake invokes actual runner601 then the same numeric judge, exact reference comparison and malformed/short-run controls. | WIRED |
| 07/08: hashes→equality/CI | hashes validates exact956-row schema; six-native comparison and native/PPC immutable-source consumer call it. | WIRED |
| 08: ledger→generated page | compat_doc.cmake reads ledger; existing freshness/pass-fault cases compare generated page and truthful capabilities. | WIRED |

### Data-Flow Trace (Level 4)

| Delivered value | Actual source and flow | Status |
|---|---|---|
| Effective region/geometry | Probe/header/hash metadata or explicit media override→validated load commit→get_info→runner allocation/info/PNG; adapter captures effective session. | FLOWING |
| Native pixels | ROM instructions/display list→bus/MARIA DMA/line RAM/native emission→caller rows→93440-byte runner hash; adapter native→selected RGB conversion→actual callback. | FLOWING |
| Raw audio | ROM writes/TIA independent456 lattice/actual POKEYφ2→ordered half-open observer records→canonical little-endian digest. | FLOWING |
| PCM | Actual source mix→selected rational interpolation/ring/blocker/FIFO→logical interval render→runner digest and adapter exact stereo callbacks. | FLOWING |
| Diagnostic values | Actual owned ROM observations/progress→RAM mailbox→independent CRC/value judge→accepted report→equality writer. | FLOWING |
| Capability page | Executed owned PAL ledger predicate→existing generator→fresh generated compatibility; retail metadata is not substituted for execution. | FLOWING |

No dynamic output chain ends in a mock, hardcoded empty collection, ignored response or disconnected prop. Numeric constants for timing/presentation and an explicit no-content test card are specified product behavior.

### Behavioral Spot-Checks

Verifier-owned command:
`ctest --test-dir build/ci -R '^(core.pal.tracer|core.pal.lifecycle|bios.load_aliases|runner.region.pal)$' --parallel 4 --output-junit phase09-verifier-focused.xml --output-on-failure --no-tests=error` — **exit0,5/5,0.07s**, URL N/A-local. CTest correctly included the required `runner.probe.images` fixture. The separate focused JUnit leaves full ordinary/release evidence intact.

| Behavior | Named test / evidence | Result |
|---|---|---|
| Startup/info/bounded full output | core.pal.tracer | PASS,0.00s |
| Validation/lifecycle/edge/NULL/hostile/buffer invariants | core.pal.lifecycle | PASS,0.02s |
| Aliased BIOS candidate preservation and actual old/new firmware execution | bios.load_aliases | PASS,0.02s |
| Effective CLI selection and info/default/probe boundaries | runner.region.pal (required probe fixture included) | PASS,0.04s |

Independent full-JUnit readback found654 ordinary cases and6 release cases, no failure/skip. It confirms executed core.pal.bounds, audio.frames/observer/spectral/controls/stress, diag.pal, host.palette, both loaded formats and real policy controls; each behavior-dependent truth has actual behavioral evidence. No full suite was repeated by the verifier.

### Probe Execution

N/A — no scripts/*/tests/probe-*.sh or phase-declared probe contract. The phase's runnable evidence is its existing C/CMake tests, actual numeric driver, producer/join and artifact consumers, verified above and below.

### Final Execution Evidence

The reviewed branch head was `419bb31faa2b7dd0c1586879e478ae8dd4ab3d6e`, including runtime fix `af9dc83f9ff68ea14c5a007218a3477a3c2f2218`. The actual PR checkout was merge `df6aeefcd4e6e5f13c4350f3a6fe71d202a0940d`, with parents main2cbb and branch419. Independent `git show --format='%H %T %P'` and `git diff --exit-code 419bb31 df6aeef -- .` confirm identical full Git tree `f2e4a895cc9f4eaaf0e4108cda087f0ffc674c3c` (exit0, URL N/A-local). Producers retain df6a as the actual tested identity; no identity is relabelled. The canonical acceptance appendix was then committed as `8814d3e80e1221cc0da71b84560c4f001852212a`; it preserves the original summary prefix and changes only that summary. Verifier `git diff --exit-code 419bb31 HEAD -- <all implementation/test/doc/workflow inputs>` exits0, URL N/A-local. Subsequent acceptance/report metadata does not change implementation.

[Fresh CI40233948646](https://github.com/szTheory/muhtari/actions/runs/40233948646), attempt1, is terminal success with all20 jobs successful, including CI required. [Fresh Nightly40233948516](https://github.com/szTheory/muhtari/actions/runs/40233948516/attempts/2), attempt2, is terminal success after one targeted equality-job retry. Its full-vector/fuzz/bench job results carry forward their unchanged-source attempt1 executions; no advisory job was rerun. The report issue-publishing job is intentionally skipped for pull_request by its declared condition.

| Evidence | Actual command / consumer | Exit and result | Run |
|---|---|---|---|
| Post-review complete local regression | `node <GSD_TOOLS> run-with-timeout 0 -- sh build/ci/task08-jobserver-gate.sh` executes unchanged `cmake --workflow --preset ci`, explicit package, release, exact JUnit and hygiene sequence. | 0;654 ordinary45.13s,6 release0.17s; explicit package/release6/0.13s; exact654 JUnit. Proven standard FIFO jobserver peak4; no source/preset mutation. | N/A-local |
| Verifier terminal CI audit | `cmake -DINPUT=build/ci/task08-refresh-ci.json -DEXPECTED_SHA=419bb31faa2b7dd0c1586879e478ae8dd4ab3d6e -P tools/check_ci_run.cmake` | 0; attempt1 completed/success, required checks passed. | Fresh CI above |
| Six native lanes | Actual current/base complete workflows and downloaded producers. | Each654+6 current,649+6 independent main; all pass. | Fresh CI |
| Sanitizer/quality/portability | Actual CI jobs plus authenticated quality consumers. | ASan637 current/632 base; hygiene24/24; ABI17, replay fuzz6, title3; findings0; both nofp jobs, architecture/ABI/integer/BE compile checks pass. | Fresh CI |
| Verifier six-native schema/equality | `cmake '-DFILES=<six paths under build/ci/task08-refresh-evidence/native-hashes>' -DCOUNT=6 -DMODE=compare -P tests/cmake/hashes.cmake` | 0;6 identical exact956-row files. | Fresh CI |
| Verifier authenticated join | `cmake -DDIRECTORY=build/ci/task08-refresh-evidence/join-inputs -DOUT=build/ci/task08-refresh-evidence/verifier-joined -DPOLICY=quality.json -DBASE_POLICY=build/ci/task08-refresh-evidence/join-inputs/quality-linux-arm64/quality-base-policy.json -DEXPECTED_SHA=df6aeefcd4e6e5f13c4350f3a6fe71d202a0940d -DBASE_SHA=2cbbd6a78dabe2bc098615d49f7118231cc05626 -DSOURCE=https://github.com/szTheory/muhtari/actions/runs/40233948646 -DPERF_REQUIRED=ON -P tests/cmake/quality_join.cmake`; the base policy was first byte-compared with `git show 2cbbd6a:quality.json`. | 0; all9 genuine artifact folders, required metrics and measured three-component current/base evidence authenticated. | Fresh CI |
| Full native/PPC inventory | Verifier runs `cmake -DJUNIT=<downloaded build/{ci,be32}/{junit.xml,junit-release.xml}> -DINVENTORY=tests/inventory/ci.txt <-DEXCLUDE=release or -DLABELS=release> -P tests/cmake/check_junit.cmake` for all4 files. | All4 exit0:654 ordinary and6 release, once each, on each architecture. Native suite51.10s/release0.58s; PPC511.87s/release0.13s. | Fresh Nightly attempt2 |
| Verifier native/PPC identity/equality | `cmake -DNATIVE=build/ci/task08-refresh-evidence/nightly/lane-equality/evidence/native -DBE=build/ci/task08-refresh-evidence/nightly/lane-equality/evidence/be -DEXPECTED_SHA=df6aeefcd4e6e5f13c4350f3a6fe71d202a0940d -P tests/cmake/lane_equality.cmake`. | 0;2 immutable-source/configuration-identical956-row files; independent byte checks match six native files and genuine main342 prefix. PPC diag.pal119.194s, hashes86.7985s, alias0.302651s. | Fresh Nightly attempt2 |
| Full vectors / bounded fuzz | Existing unchanged-source Nightly jobs and exact-inventory consumers. | Jobs successful; no PAL authority/quantity/deadline change. | Fresh Nightly |
| WASM, unchanged advisory authority | Actual workflow/JUnit; verifier `cmp build/ci/task08-refresh-evidence/wasm/hashes-wasm32/hashes.txt build/ci/task08-refresh-evidence/native-hashes/hashes-linux-x64/hashes.txt`. | 0;592 tests once each, alias0.10s; actual956 rows equal. | Fresh CI |
| Existing real frontend cases | Existing CI RetroArch DMA and two-process battery case, media/pixel judges and fault controls; verifier viewed all3 downloaded PNGs. | 0; unchanged2 cases/support budget. DMA retained records/footerBEEF; all34816 battery bytes/pixels judged, counters1/1→2/2 and marker5A→FF; images agree. | Fresh CI |
| Armed performance guard | Existing actual main/candidate comparator and authenticated perf observation. | Pass; candidate3838448 Ir/frame versus main/repeat3838688. Existing+1% comparator remains armed; observation availability stays advisory. This is test-card evidence. | Fresh CI |

Native/PPC artifact paths are under `build/ci/task08-refresh-evidence/nightly/lane-equality`. Logs/screenshots/JSON remain ignored local evidence, never committed.

**Failed evidence retained:** Nightly attempt1 equality job exhausted its unchanged30-minute outer deadline after apt provisioning took29m13s (79.5MB at45.6kB/s). Only567 native cases had passed before outer cancellation; no complete native/PPC artifact existed and no test/compiler failure was reported. It was not accepted. One parent-authorized retry of only that job used identical source/base/deadlines: setup13s, full native/PPC step9m47s, all required results above pass. No timeout was enlarged, source changed or advisory lane retried.

The unchanged advisory10000-case property job reports7/8 pass and `prop.instances_isolated` timeout390.00s. It remains unaccepted advisory evidence, outside the Nightly report aggregate and CI required; no passing retry, reduced workload or promotion is claimed. Earlier CI40229267518/Nightly40229267583 accepted pre-fix9d/69d9 source and are historical only; they are not substituted for af9/419/df6 acceptance.

### Authenticated Native Package Evidence

All six lanes used the actual tested merge `df6aeefcd4e6e5f13c4350f3a6fe71d202a0940d` and independently built main `2cbbd6a78dabe2bc098615d49f7118231cc05626`. Each current/base environment matches its compiler/platform and the raw trusted main manifest `62dc23cf0a687032b3c8d82f988c79d926a1814cd7eaf0f993cbb720a002141e` and built-sample identity `406ca181ecad9c64d4030b09203f210b69d5f5c649453f0a5149fc864444cb4b`. JSON observations retain their actual merge SHA, base SHA and CI40233948646 URL.

| Native lane | Library ZIP | Libretro ZIP | Runner ZIP | Current total | Independently built main total |
|---|---:|---:|---:|---:|---:|
| Linux arm64 | 99297 | 109310 | 120416 | 329023 | 323690 |
| Linux x64 | 97177 | 106153 | 118406 | 321736 | 314951 |
| macOS arm64 | 81517 | 86847 | 96473 | 264837 | 261502 |
| macOS x64 | 83226 | 87722 | 96918 | 267866 | 262220 |
| Windows arm64 | 96899 | 138779 | 162353 | 398031 | 391869 |
| Windows x64 | 97180 | 148560 | 175079 | 420819 | 414117 |

Independent verifier standard-library assertions inspected all18 actual current ZIP CRCs and root LICENSE/THIRD-PARTY-NOTICES.md, sum-equality to authenticated metrics, all current/base source/run/environment fields, and six956-row output files. All assertions exit0, with the CI run URL above. Existing executed release tests also enforce member/asset/platform-floor integrity.

The existing producer measures exactly three independently built base ZIPs before emitting their authenticated total; uploads retain that total without component splits. PLAN08's extra request for base splits is a documented evidence-format deviation under D19/ENGINEERING11's existing producer contract. No split was invented, no schema expanded and no extra hosted size-only run requested. All genuine current components and base three-component totals are present. Archive values are report-only, with no reinstated cutoff.

The actual main baseline was downloaded from successful [main CI40203183179](https://github.com/szTheory/muhtari/actions/runs/40203183179), attempt1 at `2cbbd6a`. Its commit-tagged342-row file has SHA256 `a036c0f9da8037b0581496182024f0b42de455e08576712e0cc890dd949462b5`. Direct byte-prefix comparison against every fresh native output passes: all342 original rows remain, followed by614 PAL rows (956 total). This uses genuine accepted main output, not regenerated candidate goldens.

### Requirements Coverage

| Requirement | Source PLANs | Required outcome and evidence | Status |
|---|---|---|---|
| PAL-01 | 01,02,03,04,07,08 | Load precedence and failure preservation;313-line MSTAT/DMA/frame boundaries; selected fitted fast snapshot and actual BIOS reset/mapping; R1/P01–04/P07. | SATISFIED |
| PAL-02 | 01,02,03,04,07,08 | Queried320×292 geometry/rationals, licensed warm palette, exact RGB565, runner info/PNG and stable loaded adapter AV/last row; R2 and host tests. | SATISFIED |
| PAL-03 | 01–08 | Selected physical clocks/rational carry, finite all-phase/extremizer proof, actual600-full interval numbers then supported-platform equality; R3 and final execution evidence. | SATISFIED |

All Phase09 traceability rows are claimed by PLAN frontmatter. No additional Phase09 requirement is orphaned. Phase10 expansion memory and Phase11 whole-library/retail accounting remain explicit later scope; they were not used to excuse any failed PAL must-have.

### Decision Coverage

`node <GSD_TOOLS> query check.decision-coverage-verify .planning/phases/09-pal-games-and-audio .planning/phases/09-pal-games-and-audio/09-CONTEXT.md` — **exit0**, URL N/A-local.

All trackable CONTEXT.md decisions are honored by shipped artifacts. **19/19**, not_honored=[].
Source review separately traces D01–06 load/scheduler/boot, D07–11 geometry/storage/palette/host lifetimes, D12–16 synthesis/device time/optics/confidence and D17–19 independent diagnostics/admission/equality/retained authorities.

### Test Quality Audit

| Requirement-linked tests | Active evidence / strongest assertion | Disabled | Circular correctness oracle | Verdict |
|---|---|---:|---|---|
| core/test_pal.c, boot/test_boot.c, maria/test_maria.c | Literal snapshots/edges, transaction before/after, complete phase/carry enumeration, signed endpoint reconstruction, poison/free alias regression and real boot execution. | 0 | No; documented literal policies and independent arithmetic. | PASS |
| input/test_input.c, core/test_core.c | Actual pins/deadline−1/at/+1, changed pots, native light/NULL/wrap, battery bytes/generations and allocator/instance invariants. | 0 | No; independent deadlines and state assertions. | PASS |
| host/test_palette.c | All256 external seed literals, independent channel truncation and old-helper parity. | 0 | No; approved pinned BSD-3 table is external expected data. | PASS |
| runner/test_movie.c, runner/test_runner.c, runner/tests.cmake | Actual parser/process/PNG/default/info/movie side effects, allocation failure and effective-region replay. | 0 | No; explicit expected text/numbers/pixels and rejected inputs. | PASS |
| libretro/host.c | Real dynamically loaded modules, scalar AV/colour oracles, exact callbacks and lifecycle sequences in both formats. | 0 | Host/library parity is consistency evidence; independent scalar and palette tests establish correctness. | PASS |
| audio/test_contracts.c, test_spectral.c, test_stress.c | Actual source timestamps/horizon/NULL/capacity, independent physical-frequency/mass judges and rejected bypass. | 0 | No; mathematically independent judge plus actual-production/control paths. | PASS |
| diag/driver.c, diag/tests.cmake, expected.csv | ROM-measured protocol/count/CRC/numbers and600-full intervals; deliberate CRC-valid wrong-number, malformed and actual shortened-session rejection. | 0 | Hashes alone prove consistency; independent numbers precede hashes. | PASS |
| policy/quality.cmake, quality/hashes consumers | Actual recursive producer/join from real Git base plus precise forged/mixed/missing/malformed controls. | 0 | Controlled synthetic environments prove producer behavior; genuine hosted artifacts prove actual package observations. | PASS |

Disabled-test regex matches were inspected: C `exit()` and fields `pending/pending_ex` are lexical false positives, not skipped tests. Capture/report writes are outputs later judged against independent inputs; no system-generated expected fixture replaces correctness evidence. Mathematical or fitted hardware-policy oracles verify that selected model, not a production-hardware measurement.

### Prohibitions, Assumptions and Confidence

Both bespoke PLAN prohibitions are explicitly dispositioned by agent judgment under AGENTS rule6/DEC09's delegated, automation-first review. This is a **NON-AUTHORITATIVE LLM-judge assessment**, not a deterministic legal or hardware certification. Their descriptor-less serialized `unresolved` history is retained as **unverified-prohibition — reviewed by the agent under AGENTS rule6; no routine owner action**. No test enforcement descriptor or resolved probe classification is invented. The user's explicit delegation of routine provenance/docs/image/frontend judgment governs this phase's disposition.

| PLAN intent | Agent disposition and independent evidence |
|---|---|
| 06: MUST NOT introduce prohibited emulator expression/private game, BIOS, loader or save bytes. | Intent upheld in inspected delta: owned MIT PAL source/config/admitted reproducible fixture; only approved warm PAL BSD-3 table with pin/holders/notices; existing manifest/asset/package/provenance controls pass. No prohibited emulator source opened during verification. Legal provenance remains judgment-tier, not a fabricated wired test. |
| 08: MUST NOT call fitted boot/retained85:342/optical compatibility parameters measured production hardware or universal retail accuracy. | Intent upheld by direct source/docs review: handoff140016, zero undocumented PAL residue and optical/paddle parameters are explicitly fits; mix explicitly unverified; README/generated capability/ledger distinguish owned diagnostic success from retail playability/calibration. |

PAL-01 and PAL-03 unclassified edge-probe assumptions remain disclosed classification limitations. Their specified engineering behavior has independent literal, transition, arithmetic, extremizer, spectral, sustained and equality evidence; no universal or backstop claim is made. There are no verification:backstop truths or test-tier prohibitions falsely marked enforced. Production-calibrated mixer/startup/optics are deliberately deferred by D05/D16, not missing Phase09 behavior or an owner hardware-testing gate.

### Anti-Patterns Found

| File | Line | Pattern | Severity | Impact |
|---|---:|---|---|---|
| tools/compat_doc.cmake | 13 | Existing comment uses “placeholder” for temporary CSV escaping. | Info | Real generator replaces/parses the delimiter and produces checked content; not a stub. |
| Nightly properties | — | Full10000-case instances_isolated timeout. | Existing advisory | Unaccepted evidence remains visible; no authority promotion or test-shortening claim. See final run audit. |

The65 changed implementation/test/doc paths contain no unreferenced TBD/FIXME/XXX, product placeholder, empty handler, disabled requirement test or disconnected dynamic output. No blocker/warning implementation finding remains. CR-01 was independently read in source and confirmed by the fresh alias test; final standard65-file review is clean after one of at most two fix rounds. No gap round used.

### Architecture and Reliance Check

The selected region is established by the production loader before power_cycle; it is not merely a test fixture precondition. Strict-future deadlines, validation-before-mutation and half-open observer ordering are explicitly enforced. Hosts query the committed state and reset their own cache; fixtures supply legal input the actual caller also supplies. No verified truth relies on an undeclared state precondition, incidental execution order or fixture-only production behavior.

Existing owners remain responsible for lifetime/commit, scheduling, native rendering, synthesis and ports. One compact timing selector replaces NTSC assumptions without a second scheduler or new dependency. Streaming independent proofs avoid denominator-sized tables; real host tests constrain actual call boundaries. BIOS private staging has the measured16384-byte instance cost and preserves the final-fallible-allocation transaction without per-frame work or load allocation. Named test-timeout changes were calibrated from complete measurements with existing lane scales; no correctness assertion, case workload, package/performance authority or retired quota changed.

### Human Verification Required

None under AGENTS rule6/DEC09. Routine docs, provenance, image and frontend judgments are performed by agents; numerical invariants have executed automated coverage. No identity-bound consent or owner-only hardware operation is required for the accepted compatibility model. Merge/post-merge release confirmation belongs to the separate ship step.

### Report Self-Check

`node <GSD_TOOLS> query verification.status .planning/phases/09-pal-games-and-audio --pick status` exits0 and returns `passed`. The report readback has42 unique truth rows, no behavior-unverified items and an empty human-verification list. `git diff --check` and `scripts/hygiene.sh --tree` exit0; run URLs N/A-local. The report is excluded from its own fingerprint. No commit was made by the verifier.

### Gaps Summary

No required product, wiring, data-flow, behavioral or evidence gap remains after the final acceptance described below. No override was needed. Hardware calibration and retail/whole-library coverage remain explicitly qualified later work, not failed PAL criteria.

---

_Verified: 2026-10-25T21:55:43Z_
_Verifier: the agent (gsd-verifier)_
_Fingerprint: generated by verification.fingerprint after canonical summary commit8814d3e; 112 covered files, report excluded._

## Scoped shipping revalidation — 2026-10-25T22:54:41Z

This independent appendix revalidates only the two measured test allowances and
the appended canonical shipping-recovery receipt. The original verification time,
42 truth rows, accepted runtime/source identities, baseline, artifact observations
and historical failures above are preserved. It does not claim a successful hosted
run of the corrected registration source. Final PR-head CI and post-merge release
confirmation remain pending shipping gates.

### Scope and source applicability

Comparing all112 declared covered paths with canonical receipt8814d3e initially
finds only `tests/diag/tests.cmake` and `tests/runner/tests.cmake` changed. The
subsequent09-08-SUMMARY.md appendix is the third changed covered input. The
implementation, public interface, host adapters, firmware fix, fixture bytes,
independent numeric/fault oracles, package/provenance consumers, platform workflows
and confidence claims remain byte-identical to the original accepted runtime.
The working-tree diff changes two outer CTest allowances, the generated matching
inner PAL allowance and their measured-source comments; no command, test case,
loop count, shortened-session rejection or comparison predicate changes.

The existing42/42 product conclusions therefore retain their prior accepted
runtime evidence, supplemented by corrected-source local execution. All19 decision
judgments and the disclosed assumption/prohibition classifications remain intact.
`status: passed` describes this scoped local registration revalidation together
with the unchanged accepted runtime evidence; it does not override either failed
shipping attempt or satisfy the pending final-head hosted CI gate.

### Independently checked evidence

The canonical SUMMARY appendix was read against the complete attempts and actual
job logs. Run40238824962 atd900bd0 fails on both attempts: first PAL36.02s and
hash29.01s deadlines, then PAL36.02s with a complete28.68s hash run. Missing Windows
artifacts correctly fail quality and the required aggregate; dependent equality,
RetroArch and WASM skip. Neither attempt is accepted evidence. The one targeted
retry exhausted its temporary-slowdown hypothesis. Full current ASan PAL52.64s at
scale2 and complete Windows hash28.68s support DEC.47 allowances53 and58; censored
timeouts are not treated as full durations. CPU class differs while image, tools
and four-way concurrency match; CPU contribution remains an inference.

| Independent command or observation | Exit | Result and source |
|---|---:|---|
| `git diff --exit-code 419bb31 d900bd0 -- src include runner libretro host tests CMakeLists.txt CMakePresets.json cmake .github` | 0 | Previous shipping failures have identical product and test source; local source comparison, URL N/A-local |
| `python3 build/ship-09-check-deadlines.py` plus independent JSON comparison | 0 | All660 names and non-TIMEOUT properties identical at scales1/2/13;198 resolved command values/presence identical,462 unbuilt command fields equally absent; only PAL53/106/689 and hash58/116/754 allowances differ; generated inner PAL matches. URL N/A-local |
| Read narrow registration source diff | 0 | Command text and every numeric/protocol/value/fault-control workload unchanged; no skip or replacement oracle. URL N/A-local |
| Read parent `cmake --workflow --preset ci` receipt in `build/ship-09-corrected-local-ci.log` | 0 | Corrected source654/654 ordinary in25.77s, all three packages,6/6 release in0.11s; PAL7.67s and hash6.40s pass. URL N/A-local |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | Exactly654 ordinary tests once each; independently rerun. URL N/A-local |
| Same JUnit consumer with `junit-release.xml` and `-DLABELS=release` | 0 | Exactly6 release tests once each; independently rerun. URL N/A-local |
| `cmake -DINPUT=build/ship-09-final-ci-attempt2-complete.json -DEXPECTED_SHA=d900bd0fae1e355ad46800dd72179e302d7cbc66 -P tools/check_ci_run.cmake` | 1 | Correctly reports every failure/skip and rejects `CI required`; [failed shipping attempt2](https://github.com/szTheory/muhtari/actions/runs/40238824962/attempts/2) |
| Read complete saved Nightly response and failed property log | 0 | [Nightly40238825011](https://github.com/szTheory/muhtari/actions/runs/40238825011) completes/success atd900bd0 with required native/PPC equality, vectors, fuzz and benches passed; advisory isolation timeout390.03s remains unaccepted, not promoted into a gate |

The temporary registration probe extends no repository check and is not committed.
Existing real numeric and malformed/short-session controls execute unchanged in the
full local workflow. Source preservation plus the explicit property comparison
checks fault-detection retention without creating tests that merely mirror timeout
literals. The corrected registration snapshot must receive one complete hosted
acceptance; another failure requires a concrete stop rather than repeated margin
increases or blind retries.

### Fingerprint and disposition

After independently checking the three changed covered inputs and unchanged-source
applicability, `verification.fingerprint` regenerates the covered-input digest via
the installed CLI. The report remains excluded from its own fingerprint. This is
an explicit scoped renewal, not a silent assertion that old hosted artifacts were
built with the new allowances. The original112-path scope remains unchanged.
No required product truth is downgraded, no override applied and no owner UAT added.
Final corrected-head hosted acceptance and merge/release confirmation are pending.
