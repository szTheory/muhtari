---
phase: 08-controllers-at-the-pins
verified: 2026-10-25T04:07:05Z
status: passed
score: 49/49 must-haves verified
behavior_unverified: 0
overrides_applied: 0
decision_coverage:
  honored: 20
  total: 20
  not_honored: []
human_verification: []
source: 70366fdab87b2f83d65c3bf865c850b2f08d9f24
source_parent: 7887ad65e7c7887032601109449758d6a8b0e9a9
advisory_failures:
  - run: 40157750598
    attempt: 1
    job: prop
    job_id: 121062663058
    test: prop.instances_isolated
    cases: 10000
    seed: 40157750598
    result: timeout
    seconds: 390.08
    ctest_exit: 8
    inventory_exit: 1
    disposition: Unaccepted advisory evidence; existing lane authority unchanged; no retry or flake claim.
covered_files: [
    ".github/workflows/be-equality.yml",
    ".github/workflows/ci.yml",
    ".github/workflows/nightly.yml",
    ".planning/phases/08-controllers-at-the-pins/08-01-PLAN.md",
    ".planning/phases/08-controllers-at-the-pins/08-01-SUMMARY.md",
    ".planning/phases/08-controllers-at-the-pins/08-02-PLAN.md",
    ".planning/phases/08-controllers-at-the-pins/08-02-SUMMARY.md",
    ".planning/phases/08-controllers-at-the-pins/08-03-PLAN.md",
    ".planning/phases/08-controllers-at-the-pins/08-03-SUMMARY.md",
    ".planning/phases/08-controllers-at-the-pins/08-04-PLAN.md",
    ".planning/phases/08-controllers-at-the-pins/08-04-SUMMARY.md",
    ".planning/phases/08-controllers-at-the-pins/08-05-PLAN.md",
    ".planning/phases/08-controllers-at-the-pins/08-05-SUMMARY.md",
    ".planning/phases/08-controllers-at-the-pins/08-06-PLAN.md",
    ".planning/phases/08-controllers-at-the-pins/08-06-SUMMARY.md",
    ".planning/phases/08-controllers-at-the-pins/08-07-PLAN.md",
    ".planning/phases/08-controllers-at-the-pins/08-07-SUMMARY.md",
    ".planning/phases/08-controllers-at-the-pins/08-08-PLAN.md",
    ".planning/phases/08-controllers-at-the-pins/08-08-SUMMARY.md",
    ".planning/phases/08-controllers-at-the-pins/08-09-PLAN.md",
    ".planning/phases/08-controllers-at-the-pins/08-09-SUMMARY.md",
    ".planning/phases/08-controllers-at-the-pins/08-10-PLAN.md",
    ".planning/phases/08-controllers-at-the-pins/08-10-SUMMARY.md",
    ".planning/phases/08-controllers-at-the-pins/08-11-PLAN.md",
    ".planning/phases/08-controllers-at-the-pins/08-11-SUMMARY.md",
    ".planning/phases/08-controllers-at-the-pins/08-12-PLAN.md",
    ".planning/phases/08-controllers-at-the-pins/08-12-SUMMARY.md",
    ".planning/phases/08-controllers-at-the-pins/08-13-PLAN.md",
    ".planning/phases/08-controllers-at-the-pins/08-13-SUMMARY.md",
    ".planning/phases/08-controllers-at-the-pins/08-14-PLAN.md",
    ".planning/phases/08-controllers-at-the-pins/08-14-SUMMARY.md",
    ".planning/phases/08-controllers-at-the-pins/08-CONTEXT.md",
    ".planning/phases/08-controllers-at-the-pins/08-RESEARCH.md",
    ".planning/phases/08-controllers-at-the-pins/08-REVIEW-DISPOSITION.md",
    ".planning/phases/08-controllers-at-the-pins/08-REVIEW-FIX.md",
    ".planning/phases/08-controllers-at-the-pins/08-REVIEW.iter1.md",
    ".planning/phases/08-controllers-at-the-pins/08-REVIEW.md",
    ".planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md",
    ".planning/preparation/ARCHITECTURE.md",
    ".planning/preparation/CONFORMANCE.md",
    ".planning/preparation/DECISIONS.md",
    ".planning/preparation/ENGINEERING.md",
    ".planning/preparation/GSD-HANDOFF.md",
    ".planning/preparation/HOSTS-AND-INTEGRATION.md",
    ".planning/preparation/README.md",
    "AGENTS.md",
    "CMakeLists.txt",
    "CMakePresets.json",
    "LICENSE",
    "PROVENANCE.md",
    "README.md",
    "THIRD-PARTY-NOTICES.md",
    "cmake/MuhtariTests.cmake",
    "cmake/packaging.cmake",
    "cmake/version.h.in",
    "docs/embedding.md",
    "docs/emulation-tests.md",
    "docs/hardware/bus.md",
    "docs/hardware/controllers.md",
    "docs/hardware/maria.md",
    "docs/hardware/riot.md",
    "docs/hardware/tia.md",
    "docs/libretro.md",
    "docs/maintaining.md",
    "docs/quality.md",
    "docs/runner.md",
    "docs/testing.md",
    "include/muhtari.h",
    "libretro/libretro.c",
    "libretro/muhtari_libretro.info.in",
    "pins.txt",
    "quality.json",
    "runner/main.c",
    "runner/movie.c",
    "runner/movie.h",
    "scripts/fetch-pin.sh",
    "scripts/hygiene.sh",
    "src/bus.c",
    "src/frame.c",
    "src/instance.c",
    "src/internal.h",
    "src/maria.c",
    "src/ports.c",
    "src/riot.c",
    "src/tia.c",
    "tests/abi/tests.cmake",
    "tests/api/v1.c",
    "tests/api/v2.c",
    "tests/api/v3.c",
    "tests/api/v4-layout.c",
    "tests/api/v4.c",
    "tests/api/v5.c",
    "tests/api/v6-layout.c",
    "tests/api/v6.c",
    "tests/audio/expected.txt",
    "tests/built/expected.txt",
    "tests/cmake/budgets.cmake",
    "tests/cmake/check_junit.cmake",
    "tests/cmake/ci_required.cmake",
    "tests/cmake/hashes.cmake",
    "tests/cmake/lane_equality.cmake",
    "tests/cmake/quality.cmake",
    "tests/cmake/quality_evidence.cmake",
    "tests/cmake/quality_join.cmake",
    "tests/compat/ledger.csv",
    "tests/core/test_core.c",
    "tests/diag/controller-analog.a78",
    "tests/diag/controller-analog.cfg",
    "tests/diag/controller-analog.s",
    "tests/diag/controller-light.a78",
    "tests/diag/controller-light.cfg",
    "tests/diag/controller-light.s",
    "tests/diag/controller-motion.a78",
    "tests/diag/controller-motion.cfg",
    "tests/diag/controller-motion.s",
    "tests/diag/driver.c",
    "tests/diag/expected.csv",
    "tests/diag/tests.cmake",
    "tests/fuzz/run.c",
    "tests/fuzz/run_seeds.c",
    "tests/fuzz/seeds.c",
    "tests/fuzz/tests.cmake",
    "tests/header/header.c",
    "tests/header/header.cpp",
    "tests/input/test_input.c",
    "tests/input/tests.cmake",
    "tests/inventory/ci.txt",
    "tests/inventory/fuzz.txt",
    "tests/inventory/hygiene.txt",
    "tests/inventory/library.txt",
    "tests/inventory/nofp.txt",
    "tests/inventory/vectors-full.txt",
    "tests/inventory/wasm32.txt",
    "tests/libretro/host.c",
    "tests/libretro/tests.cmake",
    "tests/policy/ci_run.cmake",
    "tests/policy/quality.cmake",
    "tests/policy/tests.cmake",
    "tests/prop/test_prop.c",
    "tests/retroarch/bmp.c",
    "tests/retroarch/compare.c",
    "tests/retroarch/run_retroarch.cmake",
    "tests/retroarch/test.cfg.in",
    "tests/roms/manifest.txt",
    "tests/runner/test_movie.c",
    "tests/runner/test_runner.c",
    "tests/runner/tests.cmake",
    "tools/check_ci_run.cmake"
  ]
covered_digest: "v3:sha256:dca38f8542eb7e9adf13d3acc15b85f0d44060a75b5b6338ad3fe3924fb8c24b"
---

# Phase 8: Controllers at the pins Verification Report

**Phase Goal:** Let software use light guns, analog and matrix controllers, quadrature devices and modern controller adapters.
**Verified:** 2026-10-25T04:07:05Z
**Status:** passed
**Re-verification:** No — initial goal verification. Existing review/fix reports are evidence inputs, not a previous phase VERIFICATION.
**Source:** `70366fdab87b2f83d65c3bf865c850b2f08d9f24`.
**Genuine first parent:** `7887ad65e7c7887032601109449758d6a8b0e9a9`.
**Scope:** All 72 unique paths in the submitted `build/phase08-review-files.txt`, including source, hosts, tests, owned fixtures, contracts, inventories, workflow and public documentation. Nothing in that scope was omitted. No source was changed by verification.

## Goal Achievement

The four roadmap success criteria and 45 plan-specific truths were merged without dropping roadmap scope. Every behavioral truth below has executed behavioral evidence, rather than only matching symbols. Requirements INPUT-01 through INPUT-05 are implemented. Their still-pending parent-owned checklist entries are intentionally awaiting this acceptance, not missing implementation.

The original08-01 literal `archive_bytes/native/399937` is **superseded by CONTEXT D-20 and DEC.61**, after an independent policy recovery. Its reproduced failure remains historical evidence; this report does not claim that cutoff passed. The current contract requires typed authenticated measurements and package integrity, with no replacement quota or promotion of advisory lanes. This authorized contract revision is not an invented verification override.

### Observable Truths

| # | Truth | Status | Evidence |
|---|---|---|---|
| 1 | Header defaults and host overrides select the devices consistently through append-only API, deterministic runner input and applicable libretro mappings; old callers retain behavior. | VERIFIED | `abi.api_v3/api_v6/api_v6_layout`, input, movie owner/replay and loaded host tests pass; packet validation and legacy branch traced. |
| 2 | XG-1 trigger/sensor diagnostics distinguish lit targets, dark/blanked pixels and off-screen aim with fitted timing parameters identified. | VERIFIED | Named `input.lightgun`, independent light0 diagnostic and actual host vectors pass, including24 corrected complete-prefix cases. D64/W30/L9/T8 is explicitly unmeasured. |
| 3 | Paddles and keypad diagnostics exercise dump/charge and matrix behavior at actual access ticks, including the documented timing qualifications. | VERIFIED | Named `input.directions`, actual CPU data-tick helpers and real analog0/1 three-host numeric diagnostics pass Q32 charge/2864-tick matrix tests. |
| 4 | Driving, CX22, ST/Amiga mice, SNES2Atari and Mega7800 diagnostics observe the documented pin sequences and bounded motion/button handling. | VERIFIED | Named PA7/directions/savekey tests and all motion0..5c three-host numeric scenarios pass; complete serial words/pairs and bounded1020-event probe observed. |
| 5 (08-01) | D-19: Only unique new controller-light, controller-analog and controller-motion fixture rows can extend the trusted manifest; every trusted old byte remains protected. | VERIFIED | `quality_fixture_identity`, actual raw `git show` base, exact three additions and actual-producer faults; `policy.quality.controls` in fresh full CI. |
| 6 (08-01) | D-17: Only equality or the exact evidence-backed 4-to-5 behavior annotation transition can preserve sample comparability; old digests and numeric predicates stay fixed. | VERIFIED | `quality_sample_identity` admits exact adjacent annotation relation; independent accepted 69-row comparison and eight audio rows retain digests/numbers. |
| 7 (08-01) | D-20 as revised by CONTEXT/DEC.61: archive_bytes remains a required authenticated native candidate/base observation; the exceeded historical cutoff is retired. | VERIFIED | Current CONTEXT D-20/DEC.61 supersedes only the literal 399937 cutoff; required typed native observations and actual eight-lane join are verified. The old cutoff was exceeded, never passed. |
| 8 (08-02) | D-01: Existing linked clients keep the 24-byte muhtari_input, constant values, old mask/refusal behavior and legacy Auto fallback. | VERIFIED | `ports_check_device` remains separate; unchanged v1–v5/v4-layout clients; named `abi.api_v3`, `abi.api_v6`, `abi.api_v6_layout` pass. |
| 9 (08-02) | D-02: Extended packets resolve explicit frame choice > remembered load override > descriptive header and validate both ports before mutation. | VERIFIED | `ports_resolve_ex` and validate-before-assignment in `src/ports.c:567`; `input.directions`, `core.load.keeps_previous` pass. |
| 10 (08-02) | D-03: One pending packet replaces earlier submissions; held state persists and relative contribution transfers once. | VERIFIED | `muhtari__ports_latch` clears only pending dx/dy; `input.pa7_edge`, `prop.instances_isolated`, actual movie owner pass. |
| 11 (08-02) | D-04: Ordinary-device replacement resets only its volatile state; EEPROM identity/port/span/layout/bytes remain frozen until successful reload. | VERIFIED | Per-port replacement/reset and immutable attachment validation; `core.hsc.topology`, `core.battery.reset`, `input.savekey.pins` pass. |
| 12 (08-03) | D-10: Driving +X walks connector pins(1,2) 11→10→00→01→11; reverse reverses that sequence and idle keeps phase. | VERIFIED | Separate driving closure table in `src/ports.c`; `input.pa7_edge` executes both directions/ports through CPU reads. |
| 13 (08-03) | D-03 and D-11: One host unit becomes one timed edge at454-tick cadence; dx is transferred once with bounded residual and discarded saturation excess. | VERIFIED | Clamp-before-add, fixed next tick and chronological bus; `input.pa7_edge`, generated quiet/drain assertions and fresh 255-edge probe pass. |
| 14 (08-03) | D-17: A real owned executable records numeric driving observations whose expected values are documented before hashes. | VERIFIED | Owned motion0 assembly and static expected.csv/derived tick oracle; `diag.controllers` judges 19 driving records before equality. |
| 15 (08-04) | D-13: Movie1 grammar, meanings, errors, legacy setter path and EOF semantics remain unchanged. | VERIFIED | Version-1 parser/device branch, allocation and legacy setter remain distinct; named movie parse/replay and retained old runner cases pass. |
| 16 (08-04) | D-14: Movie2 validates the whole bounded file before media/save side effects and decodes sequentially from immutable bytes into one extended packet. | VERIFIED | `read_movie` validates before run/load/save, owns immutable bytes and one cursor; `runner.movie.owner`, `runner.movie.v2_edges` pass actual refusal/side-effect/allocation/EOF checks. |
| 17 (08-04) | D-02 and D-13: Runner CLI/header device resolution and neutral initial modern Auto work before the first frame. | VERIFIED | CLI resolution and initial extended submission before `muhtari_run_frame`; named owner/edges tests exercise short-first-frame selection. |
| 18 (08-05) | D-15: Driving is selected using a MOUSE subclass and polled once per frame through base MOUSE IDs. | VERIFIED | `lr_input` polls base MOUSE X/LEFT, `retro_run` polls once; loaded `libretro.host.input` and driving diagnostic pass. |
| 19 (08-05) | D-16: Existing ordinary-pad mappings remain and selected extended controls do not double as console actions. | VERIFIED | `lr_refresh_inputs` selects one console provider and device-specific controls; loaded input/devices tests assert labels, callback counts and noncollision. |
| 20 (08-05) | D-17: Independent driving numbers pass through library, actual runner replay and in-process module before hashes/equality. | VERIFIED | `diag.controllers` executes library, actual movie2 runner child and loaded module; numeric judgment precedes native/public pixel/raw/PCM checks. |
| 21 (08-05) | D-20: Actual exact-source candidate/base three-ZIP observations on the limiting native lane are authenticated before08-06 may start. | VERIFIED | Re-fetched CI40139744520 exact62fa325 required audit exits0, real early cohort is retained; Git ancestry proves that accepted source precedes 08-06. Current full cohort is separately verified. |
| 22 (08-06) | D-07: D6 captures a low without a CPU read, repeated enable retains it and disabling clears/bypasses it; D7 clamps only INPT0–3. | VERIFIED | `src/tia.c:60,150` captures low events and applies D6/D7 truth tables; named ProLine one/two-button tests pass unpolled/repeated-write/open-bus cases. |
| 23 (08-06) | D-05 and D-06: Native visible luma>=8 at historical aim emits a delayed [sample+64,sample+94) pulse independent of host buffers or palette. | VERIFIED | `src/maria.c:450` computes historical native sample, `ports_light_sample` schedules64/30; named `input.lightgun` passes dark/bright/palette/no-buffer cases. |
| 24 (08-06) | D-17: Shared hardware behavior becomes revision5 with unchanged ABI, battery layout, old numeric/frame/audio pins except individually evidenced D6/D7 expectations. | VERIFIED | API6/ABI1/behavior5 is reported consistently; independent old69 and eight audio-row checks show only identified revision annotations changed. |
| 25 (08-07) | D-05: Rightmost/last-visible samples retain original raster and delayed response across line/frame boundaries, with NULL video equality. | VERIFIED | Original raster/x/tick feeds sensor before LRC; `input.lightgun` executes x319/last-row, next-host-call, NULL/video, JAM/HALT and exact edge cases. |
| 26 (08-07) | D-06: Diagnostics prove the named unmeasured D64/W30/L9/T8 model rather than calibrated retail accuracy. | VERIFIED | Light0 owned executable, static numeric oracle and named lightgun test pass against D64/W30/L9/T8; docs explicitly deny measured calibration. |
| 27 (08-07) | D-15 and D-16: Absolute LIGHTGUN SCREEN_X/Y/offscreen/reload mappings are bounded and console controls do not collide. | VERIFIED | `lr_gun` excludes either -32768 sentinel, uses uint64 native-geometry conversion and separate reload/offscreen; loaded host/diagnostic boundary vectors pass. |
| 28 (08-08) | D-08: Both pot inputs follow full0..65535 resistance and integer elapsed charge, independent of polling frequency. | VERIFIED | `ports_pot_duration/project/materialize` implement full resistance and elapsed Q32; `input.directions` exercises both channels/ports, 1878/95750/95753/189625 boundaries. |
| 29 (08-08) | D-07 and D-08: Dump/release, repeated writes and mid-charge position changes obey the documented ideal-clamp/Q32 model. | VERIFIED | Real D7 edges change the charge epoch; no polling materialization; named directions test covers 1000-tick change plus88654 remaining, repeats/stalls/lifecycle. |
| 30 (08-08) | D-15: Host analog X axes and independent buttons reach both paddles through the extended packet and actual replay/module. | VERIFIED | Base ANALOG left/right X and JOYPAD B/A normalize exact endpoints; actual library/movie2/module analog0 passes 19 records. |
| 31 (08-09) | D-09: Row selection uses effective ORA/DDRA, held-key connectivity and source-backed row-major ordering with low-dominant contention policy. | VERIFIED | `ports_rows` uses effective ORA/DDRA and seven-node closure, no key cap; named directions and analog1 tests cover all12 keys/ghost/contention/readback. |
| 32 (08-09) | D-09: Pot-column recovery is2864 master ticks after the last low source disappears; trigger release is immediate unless D6 captured low. | VERIFIED | Last-source removal schedules2864 master ticks, trigger is live/D6-latched; named directions test covers deadlines, unchanged writes, D7, HALT/JAM. |
| 33 (08-09) | D-17: Independent matrix ghost/history/multi-key records pass real library, movie2 and module input. | VERIFIED | Analog1 requires27 records in each real host before equality, preserving analog0; CRC-valid changed/missing matrix records are rejected. |
| 34 (08-10) | D-10: ST and Amiga forward/reverse tables use their own connector pin order; API+Y down reverses ST's documented RIGHT/UP waveform. | VERIFIED | Connector-specific ST horizontal/vertical and Amiga closure arrays; named PA7 case tests both axes/signs/ports and actual diagnostic tuples. |
| 35 (08-10) | D-10: CX22 uses direction plus toggles with stable idle, not a reused quadrature table. | VERIFIED | CX22 separately retains negative/toggle state, stable idle and shared fire; PA7 test plus independent motion1 numeric records pass. |
| 36 (08-10) | D-11: Every autonomous PA7 edge reaches RIOT through HALT/JAM even when a full cycle ends at its initial phase; state/work remain bounded. | VERIFIED | `timed_events` merges deadlines through access/stall/overshoot; every intermediate pin updates RIOT. Complete unpolled cycles and fresh1020-edge probe pass within stated bounds. |
| 37 (08-11) | D-12: SNES2Atari snapshots on LATCH rising and advances only CLOCK rising with latch low, returning all17 bits and saturated low tail on either port. | VERIFIED | Effective latch rise snapshots; low-latch clock rise advances/saturates; named directions test checks every17+ bit, DDR edges, snapshot, simultaneous priority and both ports. |
| 38 (08-11) | D-12: Mega7800 returns six exact two-bit observations, advances only its real strobe and rearms only by the documented finite release path. | VERIFIED | Mega select/strobe finite state and six observations; named directions test independently checks every pair/button, duplicate/partial/extra/rearm/normal modes. |
| 39 (08-11) | D-04 and D-12: Reads/peeks/duplicate writes do not clock, DDR-only edges do, and controller scans preserve frozen EEPROM programming on the other port. | VERIFIED | ORA/DDRA notifies effective host state separate from responses; read/peek snapshots, DDR-only edges and eight opposite-EEPROM combinations pass in `input.directions`/`input.savekey.pins`. |
| 40 (08-12) | D-15 and D-16: SNES and conventional six-button Mega layouts are complete and never also invoke console shortcuts. | VERIFIED | Conventional mapping tables plus supplemental L2/R2/L3; actual loaded CPU scan assertions and callback/provider tests pass for every button on both ports. |
| 41 (08-12) | D-17: Every newly claimed adapter has an owned executable numeric oracle before hashes, on both ports with meaningful buttons. | VERIFIED | Motion4a..c/5a..c are six fresh26-record groups covering all buttons/tail/class/pairs; actual consumer rejects swapped serial words/pairs and other numeric faults. |
| 42 (08-12) | D-04: Adapter controller selection/scanning cannot reinterpret SAVE_RAM or suppress pending programming. | VERIFIED | Published SAVE_RAM pointer/span/bytes and pending programming survive scans and refused late topology changes; named battery_order/savekey pins plus full host cases pass. |
| 43 (08-13) | D-17: Every family participates in real no-allocation-after-load and independent-instance replay/peek/transaction invariants. | VERIFIED | `extended_safety` establishes active pulse/charge/recovery/partial scans, allocator failure and untouched peers; named no_alloc and isolation tests pass for nine families/both ports/layouts. |
| 44 (08-13) | D-11 and D-14: Maximum accepted controller work and replay allocation are finite and measured on comparable builds. | VERIFIED | Fresh same-probe current/slice-base builds use identical cc/O3/config and immutable fixtures; bounded events, no runtime allocations, maximum-file/late-invalid parsing and one-cursor storage are observed. No speed threshold inferred. |
| 45 (08-13) | D-18 and D-19: Lag/read/peek semantics, old guards, architecture ownership and finite review/gap limits remain. | VERIFIED | CPU-only polling ownership and pure peek tests pass; old guards remain, fixed module ownership inspected, review round1 clean and gap round0. |
| 46 (08-14) | D-17: All newly claimed families have independently validated numeric records before native/BE equality; every old observation/predicate/pin remains protected except exact justified metadata transitions. | VERIFIED | Actual runner reports undergo shape/numeric/mailbox checks before 273 rows append to69; named producer/schema controls and same-source342-row native/PPC/WASM equality pass. |
| 47 (08-14) | D-18: Exactly two hosted RetroArch cases remain within300 physical test lines, with no private ROM/firmware/owner hardware dependency. | VERIFIED | Actual pinned RetroArch DMA and two-launch battery case pass full pixels/all34816 bytes/negative controls; tests/retroarch remains exactly300 physical lines and two conceptual cases. |
| 48 (08-14) | D-20: All six native exact-source candidate/base library+runner+libretro ZIP measurements are present and authenticated with authentic raw identities and unchanged lane authority. | VERIFIED | All18 actual current component ZIPs match authenticated six-native current/base producer sums; raw policy/SHA/environment/fixture/sample identities and actual eight-lane join pass. |
| 49 (08-14) | D-18 and D-19: Public capabilities/docs accurately distinguish model conformance, fitted parameters and later calibrated/retail claims; no required owner UAT is invented. | VERIFIED | README/help/embedding/runner/libretro/hardware docs qualify all fits and physical bindings; docs/info checks pass. Agent image/provenance judgment closes routine review without owner UAT. |

**Score: 49/49 truths verified; 0 present-but-behavior-unverified; 0 overrides.**
No specific undeclared precondition, incidental ordering or fixture-only production dependency was found in the verified evidence. Fixtures supply host inputs the real hosts also supply; production code itself establishes normalization, time ordering, and transaction boundaries.

### Required Artifacts

All43 declared artifact entries pass `gsd_run query verify.artifacts <plan>` (14 commands, exit0 each). Repeated declarations resolve to26 paths below. Existence was followed by substantive implementation and caller/consumer tracing.

| Artifact | Status | Details |
|---|---|---|
| `tests/cmake/quality.cmake` | Substantive and WIRED | Exact fixture/annotation relations and retained policy checks, included by real producer and join; actual controls and artifact join execute it. |
| `tests/policy/quality.cmake` | Substantive and WIRED | Recursive Git-base producer and actual join pass/fault cases; registered `policy.quality.controls` runs in complete CI. |
| `include/muhtari.h` | Substantive and WIRED | API6 additive40/88-byte types and setter, old24-byte type unchanged; independently declared linked layout and old clients execute. |
| `src/ports.c` | Substantive and WIRED | Substantive validators, one-packet latch and finite light/pot/matrix/motion/adapter owners; reached from both hosts, load/frame, bus, TIA and RIOT. |
| `tests/api/v6.c` | Substantive and WIRED | Linked client exercises every added public name/layout/refusal boundary; exact native/PPC/nofp execution. |
| `tests/api/v6-layout.c` | Substantive and WIRED | Header-independent frozen declarations, offsets/canary and old refusal calls; linked executable runs in required inventories. |
| `src/bus.c` | Substantive and WIRED | Real chronological merge, access data ticks and held/JAM/overshoot deadlines; pin event and unpolled-edge assertions execute. |
| `tests/diag/controller-motion.s` | Substantive and WIRED | Owned finite CPU pin/serial/select programs with 19 driving,27 per mouse/CX22 scenario and six26-record adapter groups; binary rebuilt/admitted and executed in all hosts. |
| `runner/movie.c` | Substantive and WIRED | Strict separate movie1/movie2 validation and immutable cursor decoder, consumed by actual runner main; parse/owner/subprocess checks exercise it. |
| `runner/main.c` | Substantive and WIRED | Whole-file validation precedes emulation/save/output; real CLI device selection and one decoded packet per frame; actual subprocess diagnostics and negative side-effect cases. |
| `tests/runner/test_movie.c` | Substantive and WIRED | Value/range/canonical/line-limit/EOF assertions for every typed family and unchanged old grammar; registered movie_parse executes. |
| `libretro/libretro.c` | Substantive and WIRED | All actual base-ID mappings, stable refreshed descriptors, one provider and extended submissions; module loading and real CPU scan/callback tests execute. |
| `tests/diag/driver.c` | Substantive and WIRED | Static expected.csv numeric/mailbox judge, explicit stimuli and actual library/module observations; real runner report consumer and CRC-valid fault tests. |
| `tests/diag/tests.cmake` | Substantive and WIRED | Thirteen complete real-host scenarios and report-fault consumers; included by diagnostic registration and the actual hash producer. |
| `src/tia.c` | Substantive and WIRED | D6 low capture/D7 clamp are genuine shared chip logic, called on pin/mode/write events and read through actual CPU bus. |
| `src/maria.c` | Substantive and WIRED | Native emission sampling is independent of output rows and precedes register changes/buffer swap; actual null/palette/raster tests. |
| `tests/diag/controller-light.s` | Substantive and WIRED | Owned real background/DMA-pixel/latch program, identity15 and19 independently derived records; source-pinned binary executes. |
| `tests/input/test_input.c` | Substantive and WIRED | Actual CPU LDA/STA tick helpers and independently specified numeric vectors, including24 corrected gun-prefix cases; named input groups execute. |
| `tests/diag/controller-analog.s` | Substantive and WIRED | Owned pot waits and matrix scan/recovery program, identity16, separate19/27-record scenarios; source-pinned binary executes. |
| `src/riot.c` | Substantive and WIRED | Actual data-tick ORA/DDRA notifications and retained PA7/readback owner; effective-edge and complete unpolled-cycle tests execute. |
| `tests/core/test_core.c` | Substantive and WIRED | Fail-after-load allocators, explicit active histories, untouched peer comparisons and frozen battery lifecycle; no_alloc/load/topology/reset run. |
| `tests/prop/test_prop.c` | Substantive and WIRED | Real typed family generator, independent drain/no-replay assertions and legacy replay; required100-case native/PPC evidence passes. Advisory10000 timeout is separately unaccepted. |
| `tests/fuzz/run.c` | Substantive and WIRED | Additional nonneutral typed packet and bounded effective-pin traffic after retained legacy frames; real seed replay and hosted10-check fuzz suite execute. |
| `tests/cmake/hashes.cmake` | Substantive and WIRED | Strict21-field numeric-before-hash controller reports plus exact342-row schema; actual runner/lane consumer and mutation controls execute. |
| `tests/inventory/ci.txt` | Substantive and WIRED | Authoritative selected649 ordinary plus6 release names/labels; actual local/hosted/native/PPC XML checks confirm once each, no skip. |
| `tests/inventory/wasm32.txt` | Substantive and WIRED | Authoritative587 portable names/labels, excluding unavailable module work by declared configuration; actual WASM XML/log audit passes. |

The remaining scoped paths are supporting declarations, real host tests, input/fuzz registrations, independent numeric predicates and fixture identities, retained old expected rows, exact inventories, documentation and CI contracts. They were read/scanned as part of the full72-file review; they are not omitted because they lack a PLAN artifact declaration.

### Key Link Verification

| Plan | Link | Status | Actual connection and execution |
|---|---|---|---|
| 08-01 | tests/cmake/quality_evidence.cmake → tests/cmake/quality.cmake | WIRED | `quality_evidence.cmake:24,44` includes its sibling helper and calls fixture/sample identity with raw Git objects; actual producer controls run. |
| 08-01 | tests/policy/quality.cmake → tests/cmake/quality_join.cmake | WIRED | `tests/policy/quality.cmake` invokes the actual `quality_join.cmake`; fresh required controls and actual eight-lane replay pass. |
| 08-02 | muhtari_set_input_ex → muhtari__ports_latch | WIRED | `src/ports.c:586,711` validates into local normalized state, commits pending once, and frame invokes latch; real input tests pass. |
| 08-02 | muhtari_load → muhtari__ports_device | WIRED | `src/instance.c:361,178` remembers load choices; `src/ports.c:567` resolves them and `frame.c:66` activates initial input. |
| 08-02 | muhtari__ports_latch → muhtari_get_battery | WIRED | `ports_latch` only changes volatile ordinary-port state; battery owner remains separate. Actual published-pointer/all-byte/programming tests pass. |
| 08-03 | muhtari_set_input_ex → muhtari__ports_next_event | WIRED | `ports_latch` adds clamped dx once and arms next_tick; bus merges `ports_next_event`; actual driving ticks pass. |
| 08-03 | src/bus.c → muhtari__riot_pins_changed | WIRED | `src/bus.c:139` calls `ports_catch_up`, which calls `riot_pins_changed` after each intermediate resolved deadline. Indirect wiring is intentional and exercised. |
| 08-04 | read_movie → muhtari__movie_validate_ex | WIRED | `runner/main.c:761` calls pure validate_ex before main enters run/load/save; actual late-row output/save negative checks pass. |
| 08-04 | frame_input → muhtari_set_input_ex | WIRED | `runner/main.c:1304` keeps legacy frame_input/set_input; movie2 calls next_ex then set_input_ex. Actual typed child replay/EOF passes. |
| 08-05 | retro_run → muhtari_set_input_ex | WIRED | `retro_run:925` polls once then `lr_input:829` submits one extended packet; actual loaded callback counts and pins pass. |
| 08-05 | tests/diag/tests.cmake → runner/main.c | WIRED | `tests/diag/tests.cmake` invokes built `${RUNNER}` with movie2, and compares independently judged library/module output; it need not reference runner/main.c literally. |
| 08-05 | tests/cmake/quality_evidence.cmake → tests/cmake/quality_join.cmake | WIRED | Workflow produces current/base artifacts using quality_evidence and invokes quality_join as the separate consuming job; actual cohort replay passes. |
| 08-06 | muhtari__ports_catch_up → muhtari__tia_pins_changed | WIRED | `ports_catch_up:274` calls `tia_pins_changed` after due lows; unpolled D6 under real frames/JAM/HALT passes. |
| 08-06 | src/maria.c render → muhtari__ports_light_sample | WIRED | `maria.c:450` passes native color, original raster/x/tick to ports_light_sample; actual historical-raster vectors pass. |
| 08-06 | src/bus.c → muhtari__ports_catch_up | WIRED | `bus.c:139,191,215,322` merges and drains controller deadlines through data and end ticks, including held execution; named gun/PA7 tests pass. |
| 08-07 | MARIA x319 emission → TIA D6 latch | WIRED | `maria` flushes prior raster x319 before LRC, ports preserves delayed response, TIA captures low; actual successive-host-call test passes. |
| 08-07 | RETRO_DEVICE_LIGHTGUN SCREEN_X/Y → muhtari_input_ex port gun_x/gun_y | WIRED | `lr_gun:784` queries absolute base IDs and computes native integer coordinates/sentinels; actual callback boundary vectors pass. |
| 08-07 | diag.controllers → controller-light.a78 | WIRED | `diag.controllers` uses the lightgun scenario and actual admitted controller-light binary in library/runner/module;19 numeric records pass. |
| 08-08 | TIA D7 transition → ports pot progress/deadline | WIRED | `tia.c:150` detects genuine dump changes then calls ports_dump and rearm; actual CPU before/at/after charge tests pass. |
| 08-08 | ANALOG left/right X → muhtari_port_input paddle[2] | WIRED | `lr_input:863` converts base left/right ANALOG X into independent paddle scalars; endpoint/button module checks pass. |
| 08-08 | diag.controllers → controller-analog.a78 | WIRED | `diag.controllers` executes analog0 from controller-analog binary and judges numeric records before native/raw/PCM comparison. |
| 08-09 | RIOT ORA/DDRA data tick → keypad connected switch network | WIRED | `riot.c:157` notifies rows after effective host output/DDR mutations at the real data tick; CPU matrix tests pass. |
| 08-09 | keypad pot recovery deadline → TIA INPT/D6 | WIRED | `ports_rows:65` schedules recovered pot deadlines and connects trigger low; `ports_catch_up`/TIA owns elapsed recovery and D6. Actual timing/ghost tests pass. |
| 08-09 | JOYPAD keypad callbacks → port.keypad | WIRED | `lr_input` queries all12 base-JOYPAD key IDs into the keypad mask; individual/all-key and noncollision tests pass. |
| 08-10 | motion event tick → muhtari__riot_pins_changed | WIRED | `ports_catch_up` publishes every intermediate resolved level to RIOT before CPU flag reads; complete-cycle JAM/HALT/read-clear tests pass. |
| 08-10 | MOUSE X/Y → muhtari_input_ex dx/dy | WIRED | `lr_input` reads each selected base-MOUSE axis once into dx/dy; latching consumes once. Callback counts/actual replay pass. |
| 08-10 | controller-motion records → docs/hardware/controllers.md | WIRED | Static scenario-specific expected.csv and controller documentation define distinct connector tuples; real numeric judge rejects wrong-family/axis-swap mutants. |
| 08-11 | RIOT ORA/DDRA actual data tick → muhtari__ports_host_pins | WIRED | `riot.c:157` calls ports_host_pins after ORA/DDRA at data tick; `ports_host:24` derives host drive separately from responses. DDR-only/duplicate tests pass. |
| 08-11 | SNES snapshot/Mega scan → TIA/RIOT response pins | WIRED | `ports_host_pins:30` updates finite positions; ports_swcha/inpt expose only selected responses; actual serial/pair/nibble/EEPROM tests pass. |
| 08-12 | RetroPad adapter buttons → muhtari_port_input buttons | WIRED | `libretro.c:200,205,850` maps named native buttons separately from console provider switches; loaded CPU/control tests pass. |
| 08-12 | controller-motion CPU ORA/DDRA scan → tests/diag/expected.csv | WIRED | Owned motion assembly actually clocks ORA/DDRA and produces complete words/pairs; retail_judge loads static expected.csv and rejects tail/class/pair faults. |
| 08-12 | movie2 explicit adapter tags → actual runner/module/library | WIRED | Fixed typed adapter tags are generated and replayed by the actual runner, with independent library and loaded-module numeric/native/raw/PCM checks. |
| 08-13 | extended validation failures → accepted pending controls | WIRED | `extended_safety:1830` submits valid active/pending input, applies invalid second-port variants only to one peer, then compares future execution; real test passes. |
| 08-13 | tests/fuzz/run.c → muhtari_set_input_ex | WIRED | `tests/fuzz/run.c:103` selects actual device families with nonzero fields and calls set_input_ex plus bounded pin traffic; actual replay/fuzz executes. |
| 08-13 | operating-cost measurements → ENGINEERING §11 review | WIRED | Fresh current/slice-base relinks of the same ignored one-time probe and owner allocation test substantiate fixed work/storage; review explains existing ownership without a new quota. |
| 08-14 | diag.controllers numeric judge → tests/cmake/hashes.cmake | WIRED | `hashes.cmake:107` calls the actual numeric report judge and only then emits controller rows via diag/tests.cmake; wrong/partial/report-schema controls pass. |
| 08-14 | hashes.cmake → tests/cmake/lane_equality.cmake | WIRED | `lane_equality.cmake` enforces exact member/source/fixture/configuration identity and includes hashes compare; downloaded full native/PPC342 rows pass. |
| 08-14 | quality_evidence.cmake → quality_join.cmake | WIRED | Actual per-lane3-ZIP measurements and raw event-base policies enter quality_join before worst-value reduction; all six native pairs and eight lanes are authenticated. |

Raw `verify.key-links` output was inspected for every plan:3/38 patterns match its file-oriented schema. Most other PLAN endpoints are symbol names or descriptive labels, not literal filesystem paths. Those helper negatives are not accepted as wiring evidence either way. The38 concrete chains above were resolved manually to actual code and executed consumers; no override was used to conceal a broken link.

### Data-Flow Trace (Level 4)

| Consumer | Real source and transformations | Status |
|---|---|---|
| CPU-visible input pins | API packet or actual host callbacks → complete size/domain validation → one pending packet → frame latch → per-instance finite controller state → chronological bus deadlines → RIOT/TIA pin reads and PA7/D6 events | FLOWING |
| Beam sensor | MARIA native pixel/color and original raster/x/tick → aperture/threshold test → delayed finite pulse → TIA low capture; independent of host row buffers/palette | FLOWING |
| Analog/matrix | Real ORA/DDRA/D7 access-tick transitions and host scalar/key input → Q32 charge or connected-key graph → comparator/recovery deadlines → CPU INPT values | FLOWING |
| Adapter responses | Actual CPU ORA/DDRA effective edges → snapshots/select state → full serial word or six pairs → CPU reads; reads/peeks never clock it | FLOWING |
| Numeric controller diagnostics | Three admitted source-pinned owned executables → actual CPU mailboxes in library, runner child and loaded module → static independently derived expected.csv predicates → frame/raw/PCM comparison | FLOWING |
| Architecture equality | Actual runner/report numeric validation →342-row hash emission → strict source/config/fixture-aware native/PPC/WASM comparison | FLOWING |
| Quality/package report | Actual XML, artifacts and three ZIP byte counts → original candidate/base producer JSON plus raw Git parent policy → authenticated eight-lane join → actual worst-value observation | FLOWING |

Silent diagnostic PCM and unchanged counters are independently checked outputs, not hardcoded data fallbacks. No hollow prop, orphaned phase artifact or static replacement for a dynamic source was found.

## Behavioral Evidence

All commands below were executed during this verification. Local commands have no hosted run URL (N/A-local); hosted commands belong to the exact runs linked below. Ignored downloaded artifacts/logs are working evidence, not committed logs.

### Behavioral Spot-Checks

Each of28 existing named CTest cases ran independently in under10seconds with exit0, using `ctest --preset ci -R '^<name-with-literal-dots>$' --no-tests=error --output-on-failure`:

| Named behavioral check | Exit |
|---|---:|
| `abi.api_v3` | 0 |
| `abi.api_v6` | 0 |
| `abi.api_v6_layout` | 0 |
| `input.directions` | 0 |
| `input.proline_one_button` | 0 |
| `input.proline_two_button` | 0 |
| `input.pa7_edge` | 0 |
| `input.lightgun` | 0 |
| `input.savekey.pins` | 0 |
| `input.lag_dma_and_peek` | 0 |
| `core.load.keeps_previous` | 0 |
| `core.hsc.topology` | 0 |
| `core.battery.reset` | 0 |
| `core.no_alloc_after_load` | 0 |
| `prop.instances_isolated` | 0 |
| `runner.movie_parse` | 0 |
| `runner.movie.owner` | 0 |
| `runner.movie.v2_edges` | 0 |
| `runner.movie.replay` | 0 |
| `libretro.host.devices` | 0 |
| `libretro.host.input` | 0 |
| `libretro.host.battery_order` | 0 |
| `diag.controllers` | 0 |
| `runner.hashes_file` | 0 |
| `policy.lane_equality.controls` | 0 |
| `policy.ci_run.controls` | 0 |
| `docs.budgets` | 0 |
| `libretro.info_claims` | 0 |

These are actual existing assertions, not newly written tests that mirror implementation. They cover old/API6 ABI, transaction/replacement/rollback, shared TIA D6/D7, all controller finite transitions, real host replay, EEPROM coexistence, no-allocation/instance isolation, numeric-before-hash and evidence/authority faults.

| Check | Command | Exit / result | Run URL |
|---|---|---|---|
| One fresh complete local workflow | `gsd_run run-with-timeout 60 -- cmake --workflow --preset ci` | 0;649 ordinary +6 release, three deliverables packaged | N/A-local |
| Exact local inventories | `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake`; release XML with `-DLABELS=release` | 0 each;649 and6 once each | N/A-local |
| Fresh hygiene workflow and exact inventory | `cmake --workflow --preset hygiene`; matching `tests/cmake/check_junit.cmake` | 0 each;24 | N/A-local |
| ASan corrected gun behavior | `build/asan/tests/muhtari-test-input lightgun` | 0 | N/A-local |
| Actual local producer | `cmake -DROOT=. -DBUILD=build/ci -DOUT=build/phase08-verified/local-quality.json -DSHA=70366fdab87b2f83d65c3bf865c850b2f08d9f24 -DCONFIG=ci -DSOURCE=local -P tests/cmake/quality_evidence.cmake` | 0;17ABI/6fuzz/3title; zero findings; local3ZIP total259555 | N/A-local |
| Decision coverage | `gsd_run query check.decision-coverage-verify .planning/phases/08-controllers-at-the-pins .planning/phases/08-controllers-at-the-pins/08-CONTEXT.md` | 0;20/20 honored, no not-honored decisions | N/A-local |
| Current native/PPC replay | `cmake -DNATIVE=build/phase08-verified/nightly-artifacts/lane-equality/evidence/native -DBE=build/phase08-verified/nightly-artifacts/lane-equality/evidence/be -DEXPECTED_SHA=70366fdab87b2f83d65c3bf865c850b2f08d9f24 -P tests/cmake/lane_equality.cmake` | 0;two actual342-row files; four native/PPC ordinary/release XML checks also0 | [Nightly40157750598](https://github.com/szTheory/muhtari/actions/runs/40157750598) |
| Real eight-lane join | `cmake -DDIRECTORY=build/phase08-verified/quality-inputs -DOUT=build/phase08-verified/quality-output -DPOLICY=quality.json -DBASE_POLICY=build/phase08-verified/trusted-quality.json -DEXPECTED_SHA=70366fdab87b2f83d65c3bf865c850b2f08d9f24 -DBASE_SHA=7887ad65e7c7887032601109449758d6a8b0e9a9 -DPERF_REQUIRED=OFF -DSOURCE=https://github.com/szTheory/muhtari/actions/runs/40157749294 -P tests/cmake/quality_join.cmake` | 0;all exact8 lanes, raw policy/base/current provenance, required findings/counts | [CI40157749294](https://github.com/szTheory/muhtari/actions/runs/40157749294) |

Scoped CTest writes the preset's default JUnit output. An exact-inventory check correctly rejected the interim scoped XML; the one full local workflow restored the actual649+6 output before producer acceptance. This was an evidence-file side effect, not a product failure or a waived inventory. No second broad run was used to manufacture per-truth proof.

### Bounded work and operating cost

The existing ignored one-time probe was read and freshly compiled against current production code, and against the verified slice-base22-file source at `e83d4782cba90a8a788c841b56d740ca7e60f917`. Both used `/usr/bin/cc -std=c17 -O3 -DNDEBUG`, the same owned fixtures and matching Release configuration. Compile/run commands exited0. An initial mistyped base-library path failed at link time, was corrected to its actual `core/libmuhtari.a`, and is not represented as successful evidence.

The current command was `/usr/bin/cc -std=c17 -O3 -DNDEBUG -Isrc -Iinclude -Irunner build/phase08-controller-cost.c runner/movie.c build/ci/libmuhtari.a -o build/phase08-verified/controller-cost`, followed by `build/phase08-verified/controller-cost` (exit0 each, N/A-local). The comparable relink used the same probe, slice-base include/movie source, and `build/phase08-13-base-build/core/libmuhtari.a`, then ran `build/phase08-verified/controller-cost-base` (exit0 each).

Six1000-frame neutral/ordinary/four-axis/gun-with-rows/gun-without-rows/keypad workloads each execute119401992 ticks, with zero runtime allocations. Current CPU microseconds were230375/232223/237703/242067/208921/229512; slice-base290245/299616/324406/303736/265733/296899. These are single operating-cost observations, **not a speedup or threshold claim**; the slice-base is not a phase-v-main benchmark.

A fresh maximum packet causes255 transitions per axis,1020 total, last at115770 ticks with454-tick spacing; one call ends at119404 ticks with no residual replay. Gun probes observe9 samples/9 pulses per port with and without host rows. A maximum33554432-byte movie has399456 valid rows; valid and late-invalid parsing terminate (current48667/48635 CPUµs; comparable slice-base61989/59018), cursor104bytes and packet88bytes. The production cap, saturate-before-add arithmetic, fixed event cadence and bounded file/cursor storage establish bounds; these observations add no new quota.

### Probe Execution

No shell probe was declared by these plans and no conventional phase probe script exists. Step7c is not applicable; no SUMMARY PASS marker was substituted for executed evidence. Actual registered numeric/report/policy fault consumers and named behavioral tests above were executed.

## Exact-source Hosted Acceptance

[CI40157749294](https://github.com/szTheory/muhtari/actions/runs/40157749294) and [Nightly40157750598](https://github.com/szTheory/muhtari/actions/runs/40157750598) are both **attempt1**, `workflow_dispatch`, exact source `70366fdab87b2f83d65c3bf865c850b2f08d9f24`, completed/success. Raw `gh run view --json attempt,status,conclusion,headSha,url,jobs` and raw REST run objects were independently inspected, including every job and its failed/cancelled steps. Neither JSON was normalized.

`cmake -DINPUT=build/phase08-verified/ci.json -DEXPECTED_SHA=70366fdab87b2f83d65c3bf865c850b2f08d9f24 -P tools/check_ci_run.cmake` exits0 on the actual final20-job CI response: exactly one successful CI required aggregate; expected dispatch title skip. The9-job Nightly JSON is separately audited because it has no CI required job. All29 jobs are terminal.

| Run | Actual job | Job ID | Status/conclusion |
|---|---|---:|---|
| ci | changes | 121062659747 | completed/success |
| ci | hygiene | 121062659800 | completed/success |
| ci | title | 121062660353 | completed/skipped |
| ci | perf | 121062682162 | completed/success |
| ci | asan | 121062682186 | completed/success |
| ci | library | 121062682211 | completed/success |
| ci | nofp (ubuntu-26.04) | 121062682225 | completed/success |
| ci | nofp (ubuntu-26.04-arm) | 121062682231 | completed/success |
| ci | build (windows-x64) | 121062682251 | completed/success |
| ci | be32 | 121062682257 | completed/success |
| ci | build (linux-arm64) | 121062682273 | completed/success |
| ci | build (linux-x64) | 121062682303 | completed/success |
| ci | build (macos-x64) | 121062682317 | completed/success |
| ci | build (windows-arm64) | 121062682356 | completed/success |
| ci | build (macos-arm64) | 121062682357 | completed/success |
| ci | retroarch | 121067368305 | completed/success |
| ci | hash-equality | 121067368332 | completed/success |
| ci | quality | 121067368339 | completed/success |
| ci | wasm32 | 121067368358 | completed/success |
| ci | CI required | 121071198779 | completed/success |
| nightly | fuzz | 121062662897 | completed/success |
| nightly | be-equality / equality | 121062663010 | completed/success |
| nightly | bench (macos-15) | 121062663026 | completed/success |
| nightly | prop | 121062663058 | completed/failure |
| nightly | bench (ubuntu-26.04) | 121062663067 | completed/success |
| nightly | bench (ubuntu-26.04-arm) | 121062663075 | completed/success |
| nightly | bench-wasm32 | 121062663142 | completed/success |
| nightly | vectors-full | 121062663292 | completed/success |
| nightly | report | 121064494685 | completed/success |

All final CI steps are success or intentional skip. Nightly prop has two failing steps: “10,000 cases per property” (CTest exit8) and “Every test ran” (exit1). Its `prop.instances_isolated` times out at390.08s at10000 cases/seed40157750598; this10000-case result is **unaccepted**. Five other10000-case properties pass: split_run164.01s, peek_pure205.40s, input_at_vblank210.81s, battery_lifecycle116.63s, battery_observers93.58s;7/8 fixture-inclusive cases pass. No assertion failure appears. No retry, flake diagnosis or successful10000-case isolation is claimed. Existing `continue-on-error` and omission from report needs remain authoritative. Required100-case isolation and fixed active-state independent-peer assertions pass locally and in actual native/PPC complete runs.

Actual hosted inventory evidence: six native current/base649+6 each;ASan632 current/base;hygiene24 current/base;no-fp18 on each architecture;WASM587;advisory library43. Nightly full native/PPC649+6 each, vectors258 and fuzz10 pass. Four actual300000-frame bench reports are retained: fps_milli4134556 macOS,3671054 Linuxx64,5349235 Linuxarm64,3256246 WASM. These reports do not establish a new performance threshold.


### Native packages and raw identity

Component sizes are from all18 actual downloaded current ZIPs, not inferred from compressed baselines. Each package passes CRC, exact LICENSE/THIRD-PARTY-NOTICES membership/content, exact component naming, and exclusion of ROM/BIOS/save assets. Baseline component ZIPs are not published: only their authenticated producer totals are asserted.

| Native lane | Library ZIP | Libretro ZIP | Runner ZIP | Current total | Authenticated base total |
|---|---:|---:|---:|---:|---:|
| linux-arm64 | 98496 | 107086 | 118111 | 323693 | 323693 |
| linux-x64 | 96120 | 103073 | 115759 | 314952 | 314952 |
| macos-arm64 | 81321 | 85300 | 94866 | 261487 | 261489 |
| macos-x64 | 82329 | 85204 | 94677 | 262210 | 262209 |
| windows-arm64 | 94860 | 136644 | 160247 | 391751 | 391841 |
| windows-x64 | 94784 | 146453 | 172877 | 414114 | 414134 |

Current peak414114 and base peak414134 bytes are both Windowsx64. The eight-lane join retains exact existing authority and actual worst observations. It reports required17ABI (floor12),6fuzz (floor4),3title, native649,ASan632,hygiene24, and zero analyzer/sanitizer/warning/ABI findings. Configuration inventories and zero-skip/once-only checks remain enforced.

Raw candidate/base JSON metric SHA/source, producer/units/configuration/direction/status, genuine first-parent SHA, byte-identical parent `quality.json`, and paired compiler/OS/architecture environments were inspected. All native environments include fixture digest `62dc23cf0a687032b3c8d82f988c79d926a1814cd7eaf0f993cbb720a002141e` and sample digest `406ca181ecad9c64d4030b09203f210b69d5f5c649453f0a5149fc864444cb4b`.

All six native files have342 rows and match WASM and the separately downloaded full-native/PPC cohort. The first69 rows were compared with the independent accepted `54485d630c421716d51a8d7919ded429742acf4a` evidence, not regenerated candidate goldens: only seven named `audio_behaviour_revision 4→5` annotations change. Historical numeric/frame/raw/audio digest predicates remain. Old manifest records remain, with exactly three uniquely named controller additions; each actual16512-byte ROM matches its manifest digest and pinned owned assembly source. Frozen old API clients remain byte-identical.

### Hosted frontend and visual judgment

Actual current-source job[121067368305](https://github.com/szTheory/muhtari/actions/runs/40157749294/job/121067368305) runs pinned RetroArch1.22.2 with the actual published macOSarm64 core/runner/library. The two conceptual cases are DMA and a fresh two-launch battery roundtrip. `tests/retroarch/*` totals exactly300 physical lines.

DMA runs120 frames and `build/frames/compare dma-runner.bmp dma-shot.bmp` exits0: **320x242, every pixel equal**, with the retained ledger frame pin checked. Battery runs120 frames twice: independent mailboxes show198/254, HSC/EEPROM counts1/1 then2/2, marker90 then255. `diag --battery-media` passes **all34816 bytes** for each host on both launches; actual frontend/runner files compare equal. `compare --battery-controls` exits0 for both complete frames and rejects altered counters, markers and pixels as expected. Its intentionally emitted mismatch diagnostics are fault-control output, not waived integration failures.

The compare negative control inverts pixel7,3 and must exit1 naming exactly7,3; it does. Four unsafe path/firmware cases exit1 before destructive cleanup and retain their sentinel. Unknown synthetic HSC identity and second-launch signature-only stub are asserted; no private firmware is needed.

The verifier opened **all six actual downloaded PNGs** (`dma-shot/runner.png`, `battery-shot/runner-1.png`, `battery-shot/runner-2.png`). They show the diagnostic record list on black, with battery C6/FE, counts0001→0002 and marker005A→00FF; corresponding frontend/runner images agree. This agent visual judgment accompanies the full automated pixel/numeric/media checks; no owner UAT or old screenshot was substituted.


### Historical failures and correction boundary

Historical successful `df8f9a6` CI40153312357/Nightly40153313963 remains valid evidence for that earlier source only. It is not substituted for corrected-source acceptance. The preserved14-summary history includes77013a3 dispatch-race cancellations;751f488 GCC shadow and fuzz timeout; a5ffd6f input timeout failures; and earlier policy/link/transport/deadline failures. Raw earlier failed/cancelled jobs and logs were examined, including failures preceding cancellation: host-input2.01s in the cancelled ASan lane, Windows1.01/2s lightgun/host input, and macOS publication130.29s/audio.frames10.06s. These are not erased, labelled flakes or accepted by dropping assertions/inventories.

Formal review independently identified CR-01 (short complete gun aim prefixes) and WR-01 (physical ProLine labels). Fixesac76249 and463b3a0 are present; actual prefix cases and public physical labels were re-read and tested. The calibrated `input.lightgun`2s timeout at3419436 follows twelve complete AppleClang ASan observations (maximum0.968114s) plus accepted complete Windows0.97s; normal scaling remains. Review is clean at25690a1 after one of two permitted fix rounds; no new verification gap round was consumed. Review/FIX/DISPOSITION history is preserved unchanged.

## Requirements Coverage

| Requirement | Claiming plans | Actual requirement | Status and evidence |
|---|---|---|---|
| INPUT-01 | 01–14 | Additive defaults/overrides/input across library, runner and applicable libretro without old caller change | SATISFIED — frozen clients/layout, two-port validation before mutation, remembered load/header precedence, actual typed replay and loaded base-ID mappings |
| INPUT-02 | 01,06,07,13,14 | Trigger and brightness-sensitive beam-timed XG-1 pins, dark/off-screen/blanking, named fits | SATISFIED — native emission → delayed pulse → actual D6 capture; complete-prefix/raster/no-buffer/blanking tests and three-host numeric oracle |
| INPUT-03 | 01,06,08,09,13,14 | Paddle buttons/dump/charge and keypad matrix at access time, named analog fits | SATISFIED — real D7/ORA/DDRA tick tests, Q32/full resistance, connected matrix/ghost/recovery and19/27 independent records |
| INPUT-04 | 01,03,05,10,13,14 | Driving/CX22/ST/Amiga sequences/buttons with bounded deltas | SATISFIED — distinct connector tables, real complete PA7 cycles through HALT/JAM, host delta-once and max1020-event bound |
| INPUT-05 | 01,11,12,13,14 | SNES/Mega serial/select handshakes and buttons from RIOT output/DDR | SATISFIED — all17+ serial bits, six pairs, all physical buttons/both ports, DDR/duplicate/peek cases and frozen opposite EEPROM |

Every Phase8 requirement is claimed in PLAN frontmatter; none is orphaned. Pending checklist state is intentionally left for the parent to update after reading this report.

### Decision Coverage

`gsd_run query check.decision-coverage-verify .planning/phases/08-controllers-at-the-pins .planning/phases/08-controllers-at-the-pins/08-CONTEXT.md` exits0: **20/20**. “All trackable CONTEXT.md decisions are honored by shipped artifacts.” This warning-only heuristic supplements, rather than substitutes for, the actual code/test inspection.

### Test Quality Audit

| Test file(s) | Linked requirement | Active | Skipped | Circular | Strongest assertion | Verdict |
|---|---|---|---|---|---|---|
| `tests/api/v1.c` through `v6.c`, frozen layout clients | INPUT-01 | Yes | 0 | 0 | Value + linked ABI/refusal behavior | Required passes |
| `tests/input/test_input.c` | INPUT-01–05 | Yes | 0 | 0 | Actual CPU multistep pin/timing/edge/lifecycle values | Required passes |
| `tests/core/test_core.c` | INPUT-01–05 | Yes | 0 | 0 | Rollback, active-state no-allocation, independent peer and battery lifecycle | Required passes |
| `tests/runner/test_movie.c`, `test_runner.c` | INPUT-01–05 | Yes | 0 | 0 | Strict values, real child process, side effects/EOF/ownership | Required passes |
| `tests/libretro/host.c` | INPUT-01–05 | Yes | 0 | 0 | Loaded module callbacks, all buttons and CPU observations | Required passes |
| `tests/diag/driver.c`, `tests.cmake`, owned assembly/`expected.csv` | INPUT-02–05 | Yes | 0 | 0 | Independent numeric records before actual three-host/native/raw/PCM comparisons, CRC-valid fault rejection | Required passes |
| `tests/prop/test_prop.c` | INPUT-01–05 | Yes | 0 | 0 | Generated real inputs, future-state invariants and independent quiet/drain assertions | Required100-case passes; advisory10000-case isolation timeout is unaccepted |
| `tests/fuzz/run.c`, `run_seeds.c`, `seeds.c` | INPUT-01–05 | Yes | 0 | 0 | Actual typed traffic and sanitizer/replay invariants | Required and hosted fuzz passes |
| `tests/policy/quality.cmake`, `ci_run.cmake`, hash/lane consumers | INPUT-01–05 evidence contracts | Yes | 0 | 0 | Actual producer/join and representatively mutated identities, fields, members and outcomes | Required pass/fault controls pass |

**Disabled requirement tests:0. Circular expected-value generators found:0. Insufficient required assertions:0.** Expected controller numbers are derived independently from documented connector/timing models; that is valid evidence of the specified model, and **PARTIAL for any hypothetical hardware-equivalence claim**, which this phase expressly does not make. Old retained behavior pins use independent previously accepted evidence (VALID legacy provenance). No expected output was silently regenerated from the candidate to make a mismatch pass.

## Quality, Prohibitions and Limits

### Anti-Patterns Found

No missing/stub artifact, disconnected data path, empty controller handler, disabled requirement assertion, unreferenced TBD/FIXME/XXX marker or scope-reducing inventory waiver was found in all72 scoped paths. Initial zero state is populated by real input or intentionally represents electrical idle; it is not a rendered fallback. No required blocker remains. The advisory10000-case isolation timeout is recorded above as failed evidence, not hidden by the aggregate.

The two legacy PLAN prohibition entries have `status: unresolved` and **no verification descriptor**. Their literal metadata is preserved. Agent dispositions under AGENTS6:

- **Provenance/private bytes:** no prohibited emulator source was opened; controller expression is owned, source-pinned and manifest-listed; actual package notices/asset exclusions and hygiene controls pass. The review found no GPL/LGPL/unlicensed expression or private ROM/firmware added. This is an agent provenance judgment plus concrete integrity checks, not a fabricated universal negative probe.
- **Honest model claims:** public docs/help/capabilities consistently call gun aperture/threshold/latency, analog comparator/clamp, keypad contention/recovery, motion cadence and undocumented adapter corners compatibility policy or unmeasured fits. Independent expected numbers and cross-host agreement establish model conformance, not hardware calibration or universal retail playability. Agent documentation judgment found no contradictory claim. No descriptorless prohibition is silently represented as a wired deterministic enforcement test.

The public concurrency contract remains one host thread per instance; interleaved independent-instance invariants are tested. This report does not assert simultaneous same-instance thread safety. Undefined electrical corners, calibrated retail sensor/analog tolerances, physical wheel counts, broad private-library playability and PAL completion remain expressly outside the current contract, not unclassified promises. Later phases9–11 cover their own PAL/expansion/library goals; no failed Phase8 requirement was deferred to make this phase pass.

Dispatch perf uses same-source medians3838688/3838688/3838688 (spread0) and proves repeatability only. The PR phase-v-main+1% guard remains armed and belongs to the subsequent ship step. No dispatch performance comparison is claimed against main. Advisory library/WASM/property authority, unavailable coverage/open_crashes/backlog/duration evidence, and absence of published baseline component sizes remain explicit.

### Human Verification Required

None. Required state transitions and host integration have executed tests; actual hosted screenshots were inspected by the agent. Under AGENTS6 routine visual, documentation, provenance and model-qualification judgments belong to the agent. No identity-bound action or owner-only hardware run is required by this phase contract. No owner UAT is invented to compensate for automation or the advisory timeout.

### Gaps Summary

No existing required contract defect remains. All49 current-contract truths and5 requirements are supported by code, actual wiring, real data and executed behavior. The failed advisory extended isolation run is unaccepted evidence and remains visible without changing existing lane authority. Parent-owned phase closeout and the later ship/PR gate are separate work; this report does not perform them.

### Final Report Checks

| Command/check | Exit/result |
|---|---|
| `gsd_run query verification.status .planning/phases/08-controllers-at-the-pins --pick status` | 0; passed |
| `git diff --check` | 0 |
| `sh scripts/hygiene.sh --tree` | 0 |
| `git rev-parse HEAD` | 0;70366fdab87b2f83d65c3bf865c850b2f08d9f24 unchanged |
| Repeated `gsd_run query verification.fingerprint .planning/phases/08-controllers-at-the-pins` with120 explicit paths and28 automatically included PLAN/SUMMARY paths | 0;exact148-file covered_files/digest match the owner-generated frontmatter after the addendum; the report itself is excluded |
| Summary preservation | Original24245-character prefix unchanged; append-only45 insertions,0 deletions; all earlier outcomes retained |

These local checks have N/A-local run URLs. Only08-14-SUMMARY.md and this report were written; no implementation, STATE/ROADMAP/REQUIREMENTS, prior review ledger or commit was changed.

_Verifier: gsd-verifier_
