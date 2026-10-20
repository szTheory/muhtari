---
gsd_state_version: "1.0"
milestone: v1.0
milestone_name: 7800 games play with sound in RetroArch
current_phase: 3
current_phase_name: Games play in RetroArch
status: executing
stopped_at: Completed 03-16-PLAN.md
last_updated: "2026-10-20T05:40:39.597Z"
last_activity: 2026-10-20
last_activity_desc: Plan 03-16 complete (boot to play on spacephysics with five negative controls)
state_head: f906c1bfb07c437fbcd1f251195aad484d40ce9a
progress:
  total_phases: 4
  completed_phases: 2
  total_plans: 56
  completed_plans: 53
  percent: 50
---

# Project State

## Project Reference

See: .planning/PROJECT.md (updated 2026-10-19)

**Core value:** 7800 games behave as they do on the console, identically on every platform, from a small MIT-licensed C library that any host can embed.
**Current focus:** Phase 3 — Games play in RetroArch

## Current Position

Phase: 3 (Games play in RetroArch) — EXECUTING
Plan: 17 of 19
Status: Ready to execute
Last activity: 2026-10-20 — Plan 03-16 complete: runner.boot_to_play plays spacephysics from the fast boot through the real muhtari-run (four movies, a hash for every frame to 180, dumps at 120/140/160/180); Up+Left on port 1 from frame 121 changes frame 122 and moves the ship up 17 px, Up+Right its mirror down 17 px; controls found by running the game (Up thrusts at F+1, Left/Right turn on a global 8-frame cadence, button 2 fires, Down, button 1 and port 2 do nothing); (a) bound measured at F+1 inside [F, F+2] so a one-frame-late latch fails; runner.boot_to_play.{no_input,port2,static_mutant,late_mutant} fail with mask 1, 1, 2, 1 and mirrored with sign did not flip; PNG reader shared as tests/runner/png_read.c; green on every hosted leg (run 39375738182); probe PR #12 still a draft

Progress: [█████░░░░░] 50%

## Performance Metrics

**Velocity:**
- Total plans completed: 37
- Average duration: -
- Total execution time: 0.0 hours

**By Phase:**

| Phase | Plans | Total | Avg/Plan |
|-------|-------|-------|----------|
| 01 | 21 | - | - |
| 02 | 16 | - | - |

**Recent Trend:**
- Last 5 plans: -
- Trend: -

*Updated after each plan completion*
**Per-Plan Metrics:**

| Plan | Duration | Tasks | Files |
|------|----------|-------|-------|
| Phase 01 P01 | 10 min | 2 tasks | 13 files |
| Phase 01 P02 | 6 min | 2 tasks | 9 files |
| Phase 01 P03 | 3 min | 2 tasks | 6 files |
| Phase 01 P04 | 4 min | 2 tasks | 10 files |
| Phase 01 P05 | 4 min | 2 tasks | 9 files |
| Phase 01 P06 | 6 min | 3 tasks | 14 files |
| Phase 01 P07 | 5 min | 3 tasks | 14 files |
| Phase 01 P08 | 4 min | 2 tasks | 11 files |
| Phase 01 P09 | 10 min | 3 tasks | 14 files |
| Phase 01 P10 | 6 min | 3 tasks | 11 files |
| Phase 01 P11 | 6 min | 3 tasks | 15 files |
| Phase 01 P12 | 19 min | 3 tasks | 17 files |
| Phase 01 P13 | 10 min | 3 tasks | 11 files |
| Phase 01 P14 | 16 min | 2 tasks | 7 files |
| Phase 01 P15 | 13 min | 3 tasks | 13 files |
| Phase 01 P16 | 11 min | 2 tasks | 8 files |
| Phase 01 P17 | 20 min | 2 tasks | 8 files |
| Phase 01 P18 | 20 min | 2 tasks | 9 files |
| Phase 01 P19 | 21 min | 3 tasks | 12 files |
| Phase 01 P20 | 17 min | 3 tasks | 8 files |
| Phase 01 P21 | 33 min | 3 tasks | 8 files |
| Phase 02 P01 | 15 min | 3 tasks | 22 files |
| Phase 02 P02 | 9 min | 3 tasks | 11 files |
| Phase 02 P03 | 10 min | 3 tasks | 19 files |
| Phase 02 P04 | 9 min | 2 tasks | 12 files |
| Phase 02 P05 | 17 min | 2 tasks | 10 files |
| Phase 02 P06 | 18 min | 3 tasks | 12 files |
| Phase 02 P07 | 12 min | 3 tasks | 12 files |
| Phase 02 P08 | 13 min | 2 tasks | 19 files |
| Phase 02 P09 | 21 min | 3 tasks | 21 files |
| Phase 02 P10 | 11 min | 2 tasks | 11 files |
| Phase 02 P11 | 14 min | 3 tasks | 21 files |
| Phase 02 P12 | 9 min | 2 tasks | 14 files |
| Phase 02 P13 | 10 min | 3 tasks | 16 files |
| Phase 02 P14 | 15 min | 2 tasks | 9 files |
| Phase 02 P15 | 17 min | 2 tasks | 12 files |
| Phase 02 P16 | 33 min | 3 tasks | 17 files |
| Phase 03 P01 | 10 min | 3 tasks | 11 files |
| Phase 03 P02 | 13 min | 3 tasks | 17 files |
| Phase 03 P03 | 11 min | 3 tasks | 14 files |
| Phase 03 P04 | 13 min | 3 tasks | 26 files |
| Phase 03 P05 | 16 min | 3 tasks | 13 files |
| Phase 03 P06 | 26 min | 3 tasks | 19 files |
| Phase 03 P07 | 23 min | 3 tasks | 23 files |
| Phase 03 P08 | 16 min | 3 tasks | 28 files |
| Phase 03 P09 | 14 min | 3 tasks | 15 files |
| Phase 03 P10 | 21 min | 3 tasks | 20 files |
| Phase 03 P11 | 13 min | 3 tasks | 9 files |
| Phase 03 P12 | 11 min | 2 tasks | 10 files |
| Phase 03 P13 | 17 min | 3 tasks | 21 files |
| Phase 03 P14 | 39 min | 3 tasks | 8 files |
| Phase 03 P15 | 29 min | 3 tasks | 14 files |
| Phase 03 P16 | 22 min | 2 tasks | 9 files |

## Accumulated Context

### Decisions

Decisions are logged in PROJECT.md Key Decisions table; the full list with sources is
`.planning/preparation/DECISIONS.md`.
Recent decisions affecting current work:

- Kickoff: CI and release refinements from the owner's lattice_stripe and exifcleaner histories are in ENGINEERING 4–7 (LESSONS XFER-026 to XFER-033) for phase 1.
- [Phase 01]: Seeds come from published nesturbator main 9f9386c661f6; the pinned 3200aa868371 is not on GitHub and differs only in CMakePresets.json
- [Phase 01]: muhtari_run_frame writes audio_count = 0 on error paths; 800 or 801 on success
- [Phase 01]: muhtari_config keeps 4 alignment bytes after size on 64-bit targets; static asserts pin allocator at sizeof(void *)
- [Phase 01]: The test inventory compares names and labels; tests/inventory/<preset>.txt holds sorted name<TAB>labels lines with comma-separated labels
- [Phase 01]: Plan 01's core already met the 0.1.0 contract; 15 mutation runs stood in for a failing RED and the core was not changed
- [Phase 01]: no_skip scans test CMake source for skip and disable properties and names file:line; check_junit treats any testcase without status run as not run
- [Phase 01]: A command list passed through add_test to a -P script escapes its semicolons as $<SEMICOLON>
- [Phase 01]: D-03 holds on the fetched a7800 warm table: $46 = 0xAF5163 (red largest), $C6 = 0x469A46 (green largest); the test card is unchanged
- [Phase 01]: nesturbator citations in PROVENANCE and ASSET_POLICY pin published main 9f9386c661f6; PV.12 cites check_archives.cmake L32-50
- [Phase 01]: muhtari_host is PIC with hidden visibility so the libretro module can link it
- [Phase 01]: The PNG writer takes RGB rows; the runner converts native bytes through muhtari__palette_rgb one row at a time
- [Phase 01]: runner.dump_png pins the card PNG at 232,645 bytes, SHA-256 1c0b16cb9c92d70869d3aee5cd581627f8902cb528e90f1f1d4252d9288ac956
- [Phase 01]: muhtari-run --bench is an alternative to --frames (give one, once); only muhtari_run_frame is timed, and the report is nine key value lines
- [Phase 01]: The exit table lives in runner/exit_status.h as a const struct array (0 done, 1 failure, 2 usage); runner.exit.table requires every --help table line verbatim in docs/runner.md
- [Phase 01]: Runner command-line checks dispatch on -DCASE in tests/runner/tests.cmake; exit-status cases include tests/cmake/expect_fail.cmake from a function
- [Phase 01]: The libretro instance is created in retro_load_game(NULL) with the adapter's own malloc wrapper; retro_init is empty and unload or deinit frees it
- [Phase 01]: The .info keeps is_experimental = true; info_claims exempts it by name as a label, not a capability
- [Phase 01]: library_version comes from MUHTARI_VERSION_STRING in the private generated version header
- [Phase 01]: libretro.vendored reads its file table from PROVENANCE.md section 8 and is registered even when the core is not built
- [Phase 01]: Library installs to literal lib/ and include/ so muhtari.pc keeps prefix=${pcfiledir}/../..; the consumer's MSVC runtime follows the build configuration through a generator expression
- [Phase 01]: Release zips are written into the build directory (build/ci/muhtari-<component>-<os>-<arch>.zip); the ci workflows test, package, then test label release into junit-release.xml
- [Phase 01]: The dev test preset also excludes label release because the dev workflow has no package step
- [Phase 01]: release.single_version_source scans tracked files only, outside version.txt, release-please files, CHANGELOG.md and .planning/; a version followed by a dot is outside the bounded match
- [Phase 01]: install_line.cmake runs the README line verbatim under sh with a scratch HOME; it is a CI step on macOS arm64, not a CTest
- [Phase 01]: abi.undefined allows bzero and __bzero on Darwin through the per-system helper table instead of building the core with -fno-builtin
- [Phase 01]: tests/api/v1.c names muhtari_allocator: abi.api_version found it missing, and v1.c is not frozen until 0.1.0 is released
- [Phase 01]: Hygiene preset sets CMAKE_OSX_SYSROOT=macosx so Homebrew clang-tidy finds the SDK headers; CMake 4 leaves the sysroot empty
- [Phase 01]: The float scan exempts cited section numbers such as section 6.2, so hardware citations in src/ stay as written
- [Phase 01]: abi self-test regexes allow a line break anywhere in the finding title, because CMake wraps messages at a path-dependent column
- [Phase 01]: abi.api_v1 and abi.header.* are registered from tests/abi/tests.cmake so the nofp lane includes one file
- [Phase 01]: The asan container lane needs libclang-rt-21-dev; local x86_64 emulated runs substitute bsdtar for GNU tar
- [Phase 01]: MUHTARI_SHORT_RUNS cuts the 600-frame runs to 60; MUHTARI_NOFP builds the core with -mgeneral-regs-only and registers only abi tests
- [Phase 01]: runner.stop stays portable on wasm32: raise(SIGINT) reaches the handler under wasmtime 48.0.6
- [Phase 01]: Libretro tests register only with the libretro core; wasm32.txt is the ci list minus libretro.*
- [Phase 01]: No be32 flip: the hosted be32 job takes 24-33 s, so hash-equality compares seven files with -DCOUNT=7
- [Phase 01]: R-10 settled on GitHub: MSVC /W4 /WX is clean, the /MT runner and DLL import only KERNEL32.dll, llvm-nm reads the COFF .lib, muhtari.lib has no /DEFAULTLIB
- [Phase 01]: hashes.txt is written with Unix newlines; CMake file(WRITE) writes CRLF on Windows
- [Phase 01]: hygiene.analyzer leaves out clang-tidy's Annex K buffer-handling check, which clang-tidy 21 enables against glibc
- [Phase 01]: check-title.sh rejects a skip token in the title as well as the body, because the title becomes the squash commit's subject
- [Phase 01]: policy.ci_required also requires every job but the report-only wasm32 in CI required's needs
- [Phase 01]: apply-repo-settings.sh --check reads sha_pinning_required, which replaces the unbuilt hygiene.action_pins
- [Phase 01]: Hosted RetroArch 1.22.2 starts the no-game core with -L and no content on macos-15 (run 38939482093); the retroarch job compares its screenshot with the runner's frame through sips and is in CI required
- [Phase 01]: Release rehearsal is a published prerelease rehearsal-<run>-<attempt> deleted by a cleanup job; A4 held (GITHUB_TOKEN on same-repo pull_request gets contents/id-token/attestations write), so no draft-only fallback
- [Phase 01]: publish.sh addresses draft releases by the ID from the create response; a new draft is missing from the release list at first (run 38941477560)
- [Phase 01]: Real release path: release-please with the App token, release-ci at release-please's sha, publish to the draft by ID as latest; re-run on a published release only verifies
- [Phase 01]: publish.sh attested runs attestation and tampered-copy checks before the irreversible publish on both paths
- [Phase 01]: The perf gate reports NO_BASE while the merge base has no CMakePresets.json and is skipped in release.yml's nested ci call (inputs.ref); measured test card Ir/frame 3837643 on ubuntu-26.04
- [Phase 01]: Nightly bench writes max_rss_kb from /usr/bin/time (-v kB on Linux, -l bytes on macOS); the forced failure step runs after bench-wasm32's upload
- [Phase 01]: Ruleset drift compares only the fields and rule parameters the committed file sets, after dropping server-only fields
- [Phase 01]: Dependabot auto-merge writes the App client ID into the workflow because vars may not reach Dependabot runs
- [Phase 01]: workflow.test_gate_timeout is 3: twice the measured 1.36 s cmake --workflow --preset ci time; re-measure it when the suite grows (phase 2 adds the CPU tests)
- [Phase 01]: Timeouts: twice the slowest of 10+ successful runs (5-minute floor for network-bound and roll-up jobs); be32 scale 11, wasm32 scale 3 from measured slowdowns
- [Phase 01]: The phase PR's squash title is feat: a test frame in RetroArch from the library, runner and libretro core, so release-please cuts 0.1.0
- [Phase 01]: The owner verifies nothing by default (AGENTS rule 6, DEC.09, 2026-10-16): the UAT closed with docs.api_names, the hosted RetroArch job and the agent's one-off checks; truths only a pull request or a merge can show are confirmed at ship
- [Phase 01]: The re-review after fix round 1 found the Dependabot actor gate and the rehearsal cleanup lookup incomplete; WR-01 to WR-03 wait for a ci: pull request after the merge, and one code-review fix round is left in this phase
- [Phase 01]: workflow.code_review_depth_overrides is set to [] explicitly: unset, config-get returns the string "[]" and the code-review depth resolver halts
- [Phase 02]: embed.subdirectory accepts muhtari_* OBJECT libraries beside muhtari and still rejects any other target an embedding adds
- [Phase 02]: The vector harness reads .json as one full 10,000-test upstream file and any other path as the 256-chunk N65V sample; a zero-test run exits 1
- [Phase 02]: Hosted run 39179237062: the OBJECT split links on MSVC, wasm-ld and nofp with no fallback; be32 took 43 s
- [Phase 02]: Before decimal mode, 223 of 256 opcodes pass all upstream vectors; the 33 failures are exactly the ADC, SBC, RRA, ISC and ARR forms
- [Phase 02]: The default ANE and LXA constant is MUHTARI__ANE_LXA_SALLY in internal.h, so cpu.unstable.sally_constant pins the shipped default without linking muhtari
- [Phase 02]: All 256 opcodes pass all 2,560,000 upstream 6502/v1 tests after decimal ADC/SBC/ARR; no other opcode needed a fix
- [Phase 02]: Hosted run 39180164525: the 256 vector tests are green on every leg; be32 took 45 s, so no ci.yml timeout changed
- [Phase 02]: The vector harness takes --first N for a .json prefix and reads an N65V file whose first chunk is not opcode 00 as one chunk
- [Phase 02]: The N65V reader refuses a chunk with count 0, naming the count's offset
- [Phase 02]: Hosted run 39181184945: the seven vector tooling tests pass on every leg, wasm32 and Windows included
- [Phase 02]: The Clark harness returns through a sentinel RTS to $FFF0 and fails on a JAM or at 150,000,000 cycles; the 6502 build leaves ERROR 0 after 53,953,831 cycles, the 65C02 build ERROR 1
- [Phase 02]: policy.rom_hash_coverage reads the working tree under ROOT and exempts only tests/roms/manifest.txt; its self test zeros the sample digest in a configure-time copy of the real manifest
- [Phase 02]: Hosted run 39182041117: cpu.decimal.clark takes 3.78 s on be32 and under 1 s elsewhere, so TIMEOUT 60 stays until the D-31 measurement
- [Phase 02]: 6502.org answers 404 to curl's default user agent; fetch the tutorial with a browser -A string
- [Phase 02]: NMI is taken when nmi_phi2 is at or before poll_phi2, which each instruction but JAM stores once, right before its final bus access; the NMI entry and JAM store none
- [Phase 02]: The vector harness checks every opcode poll mark (4 for BRK, 1 for a three-cycle branch, N - 1 otherwise) and gives each JAM an edge and a stale mark of before + 12
- [Phase 02]: Hosted run 39183525965: the six cpu.nmi tests and the poll-checked vectors are green on every leg; MSVC rejects (uint8_t)~CONSTANT as C4310
- [Phase 02]: HALT_RUN_CYCLES = 2 is the one named fitted HALT constant: /HALT falls at the first cycle boundary at or after the request, c0 and c1 run, c2 is frozen for ceil((until - fall)/4) whole 4-tick cycles
- [Phase 02]: TIA WSYNC releases RDY at the first tick after the write's data with (tick + tia_phase) mod 456 = 0; a held read runs in the first cycle starting at or after it, at its own decoded length
- [Phase 02]: The NMI latch is a cycle's data tick (4 fast, 5 slow), a held cycle's own tick 4; held cycles carry their own phi2 index
- [Phase 02]: abi.undefined allows __ImageBase on Windows x64: MSVC addresses cpu.c's switch jump table through it now cpu.c is in the archive
- [Phase 02]: Hosted run 39185135366: the 16 bus cases and the archive with the CPU are green on every leg; be32 took 32 s
- [Phase 02]: RIOT timer is computed on read from N, an interval shift and the writing cycle's phi2; power-on reads as 256 written on 1024T, so $FF at p = 1
- [Phase 02]: PA7's edge detector input is the level XOR the rising-edge select, and the flag sets when it falls; marked as a model in docs/hardware/riot.md
- [Phase 02]: Cachegrind: split archive 48.449 vs one translation unit 48.299 Ir per emulated cycle (0.31 %), under 5 %, so the core stays split
- [Phase 02]: Hosted run 39186310654: the 16 riot-labelled tests are green on every leg; be32 took 31 s
- [Phase 02]: The cartdb key's part is its slice position; a layout row lists slice sizes and each slice's part, so a reloaded chip appears twice
- [Phase 02]: src/cartdb.c's header spans lines 2-3: clang-format 21 reflows the one-line form; tables are 19,284 B of const data
- [Phase 02]: hygiene.no_float removes an SPDX licence expression up to the first character no expression uses, so text after */ is still scanned
- [Phase 02]: Hosted run 39187517812: cart.sha1.fips and policy.cartdb_generated are green on every leg; be32 took 48 s, the hygiene job 43 s with its fetch
- [Phase 02]: API version 2 publishes muhtari_probe; v1.c checks its version as a floor, v2.c names every new name and value, and abi.api_v2 runs on ci, wasm32 and nofp
- [Phase 02]: Probe geometry: linear 4-48 KiB in 4 KiB steps or 64 KiB; SuperGame 32 KiB-1 MiB in 16 KiB steps; SG9 (2^n + 1) x 16 KiB; every MAME 7800 list size fits
- [Phase 02]: The v4 block replaces the cart type only when it validates and its board fits the body; otherwise HEADER_V4_IGNORED
- [Phase 02]: A core string constant is a static const byte array: MSVC on ARM64 exports a literal as an external COMDAT name (run 39189208610)
- [Phase 02]: Hosted run 39189405561: the probe and API version 2 are green on every leg; be32 took 41 s
- [Phase 02]: A header hash match reports source hash; the entry's region applies only when TV bit 2 (multi-region) leaves the header naming none
- [Phase 02]: The half-swapped pass runs only for a 128 KiB miss and only an Activision entry may match it
- [Phase 02]: The leak check compares full 20-byte rows and every row's 8-byte prefix against src/cartdb.c
- [Phase 02]: The loader identifies before it allocates; struct muhtari__cart keeps the ROM pointer and the loaded muhtari_cart_info, whose rom_size is the allocation's size
- [Phase 02]: The fuzz preset builds every object with fuzzer-no-link coverage under ASan and UBSan (632 counters); the replay takes files, so tests/fuzz/tests.cmake globs the regressions at configure time
- [Phase 02]: Container fuzz run: 32,253 runs in 61 s, 528 exec/s, cov 173 to 245, corpus 18 to 85 units, no crash; hosted run 39191834717 green on every leg
- [Phase 02]: Plan 12: host/status_text.c holds the English; muhtari__status_text gives "unsupported board" and the runner appends the board name
- [Phase 02]: Plan 12: --help prints the exit code as %2d so 77 refused aligns and rows 0-2 stay byte-identical
- [Phase 02]: Plan 13: Hosted run 39193868744: the 7800basic sample builds byte-identical on all six build legs, asan, be32 and wasm32, so no leg moves to its own preset; be32 took 39 s
- [Phase 02]: Plan 13: MUHTARI_TESTS OFF registers no test; the bench preset, the perf gate and bench-wasm32 use it so their configure needs no sample tool
- [Phase 02]: Plan 13: muhtari-run-testdb links the runner's sources and the core's objects with tests/cart/test_cartdb.c; never installed
- [Phase 02]: Plan 14: the checkout's GIT_HTTP_LOW_SPEED_TIME is 240 s, the fetch's 60 s; GitHub sends nothing for 75 to 80 s while it packs the 6502/v1 blobs
- [Phase 02]: Plan 14: Hosted nightly run 39195320935: all 2,560,000 vectors and the loader fuzz target are green; vectors-full 1 min 43 s (fetch 83 s), loader-fuzz 1 min 30 s
- [Phase 02]: Plan 15: the firmware scan strips an A78 header with file(DOWNLOAD file:// RANGE_START 128), the one CMake call that copies a byte range with NULs, and makes its own temporary directory outside every zip's directory
- [Phase 02]: Plan 16: No be32 flip: the hosted be32 job's slowest of 13 runs is 45 s (run 39198577488), so hash-equality keeps seven files
- [Phase 02]: Plan 16: Timeouts measured from 13 ci.yml and 10 to 17 nightly.yml runs; a job that fetches or installs takes the 5-minute network-bound floor; nightly vectors-full is 8 min, the fetch TIMEOUT 220 s plus 2 x 118 s
- [Phase 02]: Plan 16: CTest TIMEOUTs are twice the slowest time over the lane's scale; the firmware scans stay provisional with 9 samples
- [Phase 02]: Plan 16: workflow.test_gate_timeout is 4: twice the slowest clean local gate, 1.82 s
- [Phase 02]: Plan 16: The ship title is feat: the CPU matches the public 6502 vectors, with the 7800 bus, RIOT and cartridge probing (check-title.sh exit 0); probe PR #9 closed unmerged
- [Phase 02]: Review fix round 1 fixed WR-02 to WR-04. The bus takes one HALT window and one NMI edge at a time and ignores a second while one is in flight; a step on a jammed CPU reads $FFFF once, so time keeps moving. Phase 3's MARIA and run loop build on both (docs/hardware/bus.md, docs/hardware/sally.md, JAM). WR-01 and IN-01 to IN-03 wait in the 2026-10-19 todo; one code-review fix round is left in this phase
- [Phase 02]: The re-review of fix round 1 (2cca8b3) found no blocker. Its new WR-01 (the bus entry points cannot say a request was ignored), WR-02 (a test seed) and IN-01 wait in the MARIA and run-loop todo for phase 3, so one code-review fix round is still left. The re-verification passed 5/5 with a fresh covered digest
- [Phase 02]: Fix round 2, the last one rule 7 allows, fixed the re-review's WR-01 (62b6bdc): `muhtari__bus_request_halt` and `muhtari__bus_raise_nmi` return 1 when they take a request and 0 when they ignore it, and MARIA checks it in phase 3. WR-02 is skipped as a test seed; it and IN-01 wait in the MARIA and run-loop todo. No code-review fix round is left in this phase
- [Phase 02]: The third review (4f8de62) found no blocker. WR-05: `muhtari__bus_raise_nmi` can return 1 for an edge the latch later drops, so its 1 means "queued", not "will be taken". It, IN-04 and the IN-05 seed wait in the MARIA and run-loop todo; phase 3 settles what the 1 promises. The re-verification passed 5/5 with the same 139 covered files
- [Phase 03]: Plan 01: MARIA's holes come from the DLL entry's H16/H8 bits, not CTRL; muhtari__maria_vbe loads zone 0's entry from DPPH/DPPL for plan 03-02's t 43 event to call
- [Phase 03]: Plan 01: muhtari__maria_dma_line fills *st and stats[line], clamps LRC to T0 + 454, and sets a feature bit only when an object uses it; docs/hardware/maria.md derives the 16/24 empty-DL ticks from T0 = /HALT fall + 6
- [Phase 03]: Plan 02: the run loop steps SALLY to absolute VBLANK-start deadlines (line 258 t 42); MARIA's line events fire from the tick count and split every hold; the burst runs at c2 with T0 = fall + 6; the origin moves at line 263's t 0
- [Phase 03]: Plan 02: the fast-boot hand-off is line 240 t 24 (cpu_phase 0 derived); frame 1 is 8,190 ticks and 54 samples; rows of a frame with content are $00 until span rendering; a HALT window inside an RDY hold counts held cycles as c0-c2
- [Phase 03]: A register write's data tick is the last pixel shown with the old value; the next pixel shows the new one
- [Phase 03]: Only the first frame after a load pre-writes $00 to every row; rendering overwrites the scanned rows, so no rows_scanned bitmap
- [Phase 03]: The DLI's NMI is the second grid tick strictly after HALTRST on SALLY's 4-tick grid from c2
- [Phase 03]: A DLI on zone 0's entry, read by VBE with no HALT window, raises no NMI yet; its timing waits for the diagnostic ROMs
- [Phase 03]: The input latch runs at the start of a frame that begins at VBLANK start; input set before the first frame after a load applies at frame 2
- [Phase 03]: A header naming no controller leaves the port unplugged; an unmodelled header device is wired as a ProLine
- [Phase 03]: Applied and pending input survive load and power cycle; with no content every frame is a lag frame
- [Phase 03]: Plan 05: frames.cmake under -DWRITE=ON at a core behaviour revision above the file's rewrites the row and the revision line and resets every other row's frame pin to -; a - column fails the check
- [Phase 03]: Plan 05: hosted run 39351079824 with all 29 linear samples: be32 101 s, asan 36 s, so basic stays per-PR on both lanes with no lane inventory; slowest build leg macos-x64 101 s; no timeout-minutes changed
- [Phase 03]: Plan 05: probe PR #12 is open as a draft; src/maria.c's MSTAT read no longer casts ~MARIA_VBLANK (MSVC C4310 on the first phase-3 push)
- [Phase 03]: Plan 06: the dma diagnostic times one burst per frame on line 100 with DMA off elsewhere after VBE; INTIM = 245 - ceil((15 + D) / 4) ordinary, 245 - ceil((21 + D) / 4) last; all 27 records and 26 statistics rows matched their prior derivations on the first run
- [Phase 03]: Plan 06: expected.csv and stats.csv accept lo-hi ranges; the overrun rows (INTIM 137-138, cost 420-423, 13 objects) are ranges because the abort's lost cycles depend on the CPU phase at t 28; a record's id is its row's position
- [Phase 03]: Plan 06: a diagnostic's .a78 is committed after its source, with the manifest pin set to that source commit; diag.rebuild also rebuilds both Clark builds; hosted run 39354194917 green on every leg
- [Phase 03]: 03-07: the cpu ROM ends each 64-load run with MARIA's WSYNC, so slow loads read 205 and RAM loads 173, for every CPU phase
- [Phase 03]: 03-07: a DLI on zone 0's entry raises NMI on the second phi1 after VBE's end, t 50 of line 16; broken-windows item 4 fixed
- [Phase 03]: 03-07: parked's first derivation (20) left out line 18's burst; the doc now derives 16, which the first run read
- [Phase 03]: 03-08: SuperGame boards (RAM, bank 6, SG9, banked RAM) in one switch; ROM and cart RAM in one allocation; SG9 maps bank 1 + register at $8000, so bank 1 at power-on
- [Phase 03]: 03-08: all 34 buildable samples pinned; the POKEY-at-$0450 samples will move with a behaviour-revision bump when sound lands
- [Phase 03]: 03-09: modes is a plain SuperGame board of 4 banks (at_4000 reads bank 2), since no other ROM was SuperGame without RAM or options; it draws 160B, 320B, 320D, kangaroo mode and 2-byte characters
- [Phase 03]: 03-09: basic.coverage judges 31 claims over 17 required features from the probe and the sticky mask; holey bits follow the DLL's H8/H16, so samples show them too
- [Phase 03]: Two-button mode follows the levels at each read: PB2 [PB4] an output driven 0 and INPTCTRL TIA-EN clear; CTLSWB $14 alone stays one-button since ORB powers on $FF
- [Phase 03]: INPT0-INPT3 follow the ProLine's buttons in one-button mode too (MAME, a7800, MiSTer); io_hazard counts frames ending with a stick's fire held in two-button mode and is internal
- [Phase 03]: A movie line is at most 64 bytes except a rom-sha256 or bios-sha256 line, which is its key, a space and 64 hex digits; 03-11 decides how a movie's line 1 maps to frames, since the frame after a load latches nothing
- [Phase 03]: Movie line k is the input set before the runner's frame k, latched at the VBLANK start that opens it; under fast boot line 1 reaches no game, so a movie records what a host passes to muhtari_set_input before each frame
- [Phase 03]: The movie header wins: a conflicting --port1, --port2 or --region exits 2, as does boot bios until 03-13; --frames defaults to the movie length; past the end buttons release and difficulties stay; --region ntsc forces NTSC, auto refuses PAL
- [Phase 03]: OpenBIOS is a pinned CC0 source assembled by tests/built/openbios.cmake; built.openbios builds the $FF image only, and both fill digests are firmware rows
- [Phase 03]: 03-13: a BIOS boot starts at power-on (line 0, t 0, CTRL $00) and runs RES; the BIOS answers A15 = 1 (MEN = 1) or A12 = 1 (MEN = 0) while EXT = 0, for the CPU and MARIA
- [Phase 03]: 03-13: K is recorded once at the first cartridge opcode fetch with EXT = 1 (simple: 323; Pause held frames 1-3 gives 3); the fast boot records 1; input set before a BIOS boot's frame 1 applies from power-on, so movie line 1 counts under boot bios
- [Phase 03]: 03-13: INPTCTRL locked with MEN = 0 before the hand-off is a sticky ERR_BIOS_REJECTED_CART (runner 77); a movie's boot key wins over --boot (exit 2), and bios-sha256 is checked before frame 1 (exit 1)
- [Phase 03]: 03-14: the ledger has 42 rows; title@F=H is the fast boot's frame F hash, and through OpenBIOS H must appear in [K, K + 300) with K in [300, 340]; pin.cmake refuses a single-colour frame
- [Phase 03]: 03-14: five samples that draw with rand (seeded from INTIM, which the BIOS leaves differently) fall back to boots@F;why=rng-seeded-from-intim; backuptest and languagetest draw nothing by design and are skipped
- [Phase 03]: 03-15: libretro memory map covers the whole 64 KiB bus in aligned power-of-two descriptors with pointer-less descriptors over unmapped spans, so rcheevos reaches the page 0 shadows and the fixed ROM
- [Phase 03]: 03-15: muhtari_boot applies at content load; Restart power-cycles the loaded media and reapplies only the difficulty options
- [Phase 03]: 03-15: rcheevos v12.1.0 is a test-only pin built with warnings and sanitizers off, with two generated rhash stubs
- [Phase 03]: 03-16: Boot to play holds Up+Left on port 1 from frame 121 with Up+Right as its mirror; (a) uses the measured one-frame bound inside [F, F+2] so a one-frame-late latch fails

### Pending Todos

- [2026-10-16] [ci] Promote the wasm32 lane into CI required — [todo file](.planning/todos/pending/2026-10-16-promote-the-wasm32-lane-into-ci-required.md)
- [2026-10-16] [ci] Arm the instructions-per-frame gate — [todo file](.planning/todos/pending/2026-10-16-arm-the-instructions-per-frame-gate.md)
- [2026-10-16] [ci] Limit release rehearsals to pushes that change release inputs — [todo file](.planning/todos/pending/2026-10-16-limit-release-rehearsals-to-pushes-that-change-release-inputs.md)
- [2026-10-16] [ci] Close the re-review's findings on the Dependabot gate and the publish checks — [todo file](.planning/todos/pending/2026-10-16-close-the-re-review-findings-on-the-dependabot-gate-and-the-publish-checks.md)
- [2026-10-19] [ci] Close the phase 2 review seeds on the nightly filter and the ROM tables — [todo file](.planning/todos/pending/2026-10-19-close-the-phase-2-review-seeds-on-the-nightly-filter-and-the-rom-tables.md)
- [2026-10-19] [core] Close the phase 2 re-review findings when MARIA and the run loop land — [todo file](.planning/todos/pending/2026-10-19-close-the-phase-2-re-review-findings-when-maria-and-the-run-loop-land.md)

### Ship step (phase 01)

- Shipped 2026-10-16 as PR #7, titled `feat: a test frame in RetroArch from the library, runner and libretro core`
  so release-please cuts 0.1.0 (`scripts/check-title.sh` exit 0 on the title and body). This note is the head
  commit the pull request opened on, so CI and the Release rehearsal run once and no CI-skip commit is needed.
- Before the merge (01-UAT.md test 4), confirmed: the pull request's own Release run 38975022865 (ci,
  rehearsal, install-line, cleanup) was green on its head commit; evidence on PR #7, comment 6225897054. The owner
  merged it on 2026-10-16 as f36c884.
- After the merge (01-UAT.md test 3), confirmed 2026-10-16: release pull request #8 merged itself as 952fce9,
  and its Release run 38983378877 published v0.1.0 as latest with 19 assets and passed install-line-latest.
  `shasum -a 256 -c SHA256SUMS` and `gh attestation verify` (signer workflow, `refs/heads/main`) passed on all 18
  zips; evidence on PR #7, comment 6226988821.
- The `main` ruleset requiring `CI required` is live and `scripts/apply-repo-settings.sh --check` reports no
  drift.

### Blockers/Concerns

- [Phase 02] The four firmware-scan test TIMEOUTs stay provisional on 9 hosted samples; re-measure once 10 exist.
- [Phase 02] 02-16-SUMMARY.md criterion 5 coverage reads `status: pending-ship`, outside the schema's
  pass/fail/unknown; set it to `pass` at ship once the 0.2.0 Release run is confirmed.

## Deferred Items

Items acknowledged and deferred at milestone close, most recent first:

| Category | Item | Status | Deferred At | Milestone |
|----------|------|--------|-------------|-----------|
| *(none)* | | | | |

## Session Continuity

Last session: 2026-10-20T05:40:39.471Z
Stopped at: Completed 03-16-PLAN.md
Resume file: None
