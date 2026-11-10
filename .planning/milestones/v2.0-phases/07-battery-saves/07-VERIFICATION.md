---
phase: 07-battery-saves
verified: 2026-10-24T15:47:56Z
status: passed
score: 35/35 must-haves verified
covered_files:
  - ".github/workflows/be-equality.yml"
  - ".github/workflows/ci.yml"
  - ".github/workflows/nightly.yml"
  - ".planning/phases/07-battery-saves/07-01-PLAN.md"
  - ".planning/phases/07-battery-saves/07-01-SUMMARY.md"
  - ".planning/phases/07-battery-saves/07-02-PLAN.md"
  - ".planning/phases/07-battery-saves/07-02-SUMMARY.md"
  - ".planning/phases/07-battery-saves/07-03-PLAN.md"
  - ".planning/phases/07-battery-saves/07-03-SUMMARY.md"
  - ".planning/phases/07-battery-saves/07-04-PLAN.md"
  - ".planning/phases/07-battery-saves/07-04-SUMMARY.md"
  - ".planning/phases/07-battery-saves/07-05-PLAN.md"
  - ".planning/phases/07-battery-saves/07-05-SUMMARY.md"
  - ".planning/phases/07-battery-saves/07-06-PLAN.md"
  - ".planning/phases/07-battery-saves/07-06-SUMMARY.md"
  - ".planning/phases/07-battery-saves/07-07-PLAN.md"
  - ".planning/phases/07-battery-saves/07-07-SUMMARY.md"
  - ".planning/phases/07-battery-saves/07-08-PLAN.md"
  - ".planning/phases/07-battery-saves/07-08-SUMMARY.md"
  - ".planning/phases/07-battery-saves/07-09-PLAN.md"
  - ".planning/phases/07-battery-saves/07-09-SUMMARY.md"
  - ".planning/phases/07-battery-saves/07-CONTEXT.md"
  - ".planning/phases/07-battery-saves/07-RESEARCH.md"
  - ".planning/phases/07-battery-saves/07-REVIEW-DISPOSITION.md"
  - ".planning/phases/07-battery-saves/07-REVIEW-FIX.md"
  - ".planning/phases/07-battery-saves/07-REVIEW.md"
  - ".planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md"
  - ".planning/preparation/ARCHITECTURE.md"
  - ".planning/preparation/DECISIONS.md"
  - ".planning/preparation/ENGINEERING.md"
  - ".planning/preparation/HOSTS-AND-INTEGRATION.md"
  - ".planning/preparation/README.md"
  - "AGENTS.md"
  - "ASSET_POLICY.md"
  - "CMakeLists.txt"
  - "CMakePresets.json"
  - "PROVENANCE.md"
  - "docs/embedding.md"
  - "docs/emulation-tests.md"
  - "docs/hardware/bus.md"
  - "docs/hardware/cartridge.md"
  - "docs/hardware/controllers.md"
  - "docs/hardware/riot.md"
  - "docs/libretro.md"
  - "docs/quality.md"
  - "docs/runner.md"
  - "docs/testing.md"
  - "host/status_text.c"
  - "include/muhtari.h"
  - "libretro/libretro.c"
  - "libretro/muhtari_libretro.info.in"
  - "pins.txt"
  - "quality.json"
  - "runner/exit_status.h"
  - "runner/main.c"
  - "runner/movie.c"
  - "runner/movie.h"
  - "runner/stop.c"
  - "runner/stop.h"
  - "src/bus.c"
  - "src/cart.c"
  - "src/frame.c"
  - "src/instance.c"
  - "src/internal.h"
  - "src/ports.c"
  - "src/riot.c"
  - "tests/abi/tests.cmake"
  - "tests/api/v4-layout.c"
  - "tests/api/v5.c"
  - "tests/audio/expected.txt"
  - "tests/audio/test_contracts.c"
  - "tests/boot/test_boot.c"
  - "tests/built/chain.c"
  - "tests/built/expected.txt"
  - "tests/cmake/check_junit.cmake"
  - "tests/cmake/hashes.cmake"
  - "tests/cmake/info_claims.cmake"
  - "tests/cmake/lane_equality.cmake"
  - "tests/cmake/quality.cmake"
  - "tests/cmake/quality_evidence.cmake"
  - "tests/cmake/quality_join.cmake"
  - "tests/core/test_core.c"
  - "tests/core/tests.cmake"
  - "tests/diag/battery.a78"
  - "tests/diag/battery.cfg"
  - "tests/diag/battery.s"
  - "tests/diag/driver.c"
  - "tests/diag/expected.csv"
  - "tests/diag/tests.cmake"
  - "tests/fuzz/i2c.c"
  - "tests/fuzz/replay_main.h"
  - "tests/fuzz/tests.cmake"
  - "tests/header/header.c"
  - "tests/header/header.cpp"
  - "tests/input/test_input.c"
  - "tests/input/tests.cmake"
  - "tests/install/tests.cmake"
  - "tests/inventory/ci.txt"
  - "tests/inventory/fuzz.txt"
  - "tests/inventory/nofp.txt"
  - "tests/inventory/wasm32.txt"
  - "tests/libretro/bios.c"
  - "tests/libretro/host.c"
  - "tests/libretro/rcheevos.c"
  - "tests/libretro/tests.cmake"
  - "tests/policy/quality.cmake"
  - "tests/policy/tests.cmake"
  - "tests/prop/test_prop.c"
  - "tests/prop/tests.cmake"
  - "tests/retroarch/bmp.c"
  - "tests/retroarch/compare.c"
  - "tests/retroarch/run_retroarch.cmake"
  - "tests/retroarch/test.cfg.in"
  - "tests/roms/manifest.txt"
  - "tests/runner/test_audio.c"
  - "tests/runner/test_control.c"
  - "tests/runner/test_movie.c"
  - "tests/runner/test_runner.c"
  - "tests/runner/test_save.c"
  - "tests/runner/tests.cmake"
covered_digest: "v3:sha256:2b85249f207da6a8e899ec0d9b7bae9ec9a36d0a61f647539290633de01ce531"
behavior_unverified: 0
overrides_applied: 0
decision_coverage:
  honored: 18
  total: 18
  not_honored: []
must_haves:
  truths:
    - "Before battery fixture acceptance, the actual evidence producer and unchanged authenticated join accept only the named owned battery diagnostic addition and retain raw trusted base identity."
    - "Changed, deleted, duplicated or unrelated manifest rows and forged compiler/platform/base/run evidence fail the actual consumer."
    - "Only the exact supported behavior annotation transition is admitted; all prior sample digests, pins and numeric predicates remain protected."
    - "The load-stable battery span has layout version 1, a correct change generation and power-cycle retention; raw .sav/.srm bytes agree."
    - "HSC SRAM and supplied-ROM mapping work; the ROM-free signature stub is explicitly limited to compatible direct-SRAM software."
    - "Only exact 4096-byte supplied firmware is copied; unknown identity or absent signatures warn; malformed media refuses transactionally."
    - "A real client compiled with the frozen API-4 layout links against the new library and retains old minimum-size behavior."
    - "SaveKey/AtariVox EEPROM diagnostics cover repeated START, page wrap, sequential reads and emulated-time busy polling."
    - "Only one EEPROM attaches at load, and its port and battery offset stay frozen; rejected hot changes do not alter pending joystick input."
    - "Buffered page writes become persistent only at the selected 5 ms emulated deadline; frame catch-up and reset obey before/equal/after boundaries without polling."
    - "Same-value programming is busy but clean; reads/peeks, address-only traffic and aborts do not change generation."
    - "Every supported layout and either EEPROM port remains deterministic, isolated and allocation-free after load."
    - "Host import is clean; changed emulated commits alone alter generation, including defined uint64 wrap; reset and refused load preserve dirty evidence."
    - "Malformed and incomplete I2C traffic remains bounded and cannot mutate another instance or the inactive battery region."
    - "Runner reload, periodic/on-demand/exit flush and failed atomic-replace tests preserve existing progress, including short/oversized inputs."
    - "Missing/zero/short saves are FF-padded and clean; exact saves copy verbatim; oversized input exits 4 and other import errors exit 1 without frames or replacement."
    - "A dirty save replaces the destination only after exclusive own-temp full write, flush, sync and close; every pre-replacement failure preserves prior bytes."
    - "No persistence occurs without opt-in, after rejected import/load/BIOS, or as a cleanup retry after failure."
    - "Dirty progress waits at most F subsequently completed frames for periodic persistence; clean cadence/request/exit does not rewrite."
    - "POSIX USR1 flushes and INT/TERM stop after the current frame; Windows native BREAK flushes and C stops with synchronized request flags."
    - "Coalesced requests and requests arriving during a flush remain effective; handlers perform no I/O or core work."
    - "Every actual-consumer pre-replacement fault preserves old destination bytes, retains dirty evidence and exits once without cleanup retry."
    - "SAVE_RAM is the exact core span immediately after retro_load_game, before callbacks or first run; pointer/size remain stable until unload/reload."
    - "Absent/unloaded media returns NULL/0; reset retains committed bytes; late storage-bearing changes are visibly reload-only."
    - "HSC descriptors alias actual SRAM and the installed supplied ROM/stub through public API queries; serial EEPROM is not a fabricated CPU memory range."
    - "Every intended HSC address resolves to the correct byte using the pinned rcheevos resolver without changing retained cartridge maps or resolver limits."
    - "The admitted owned diagnostic increments independent HSC and EEPROM counters via CPU-visible SRAM and real I2C, waits for programming, reads back, and displays observed values."
    - "Two fresh RetroArch processes show counter pairs 1/1 then 2/2 while sharing only designated save storage; two independent fresh runner processes do the same."
    - "Each complete 34816-byte frontend snapshot equals its corresponding runner snapshot and independent expected media; independent counter pixels reject equal black/wrong frames."
    - "Hosted coverage has exactly DMA equivalence and the save roundtrip; no-content remains covered in-process and tests/retroarch stays <=300 physical lines."
    - "Complete native and PowerPC inventories and equality retain all old digest/numeric observations and add independently validated battery observations."
    - "The second bounded hosted RetroArch case proves a save survives two fresh launches; existing in-process checks carry the detailed semantics."
    - "Every native lane's real library+runner+libretro ZIP sum is <=399937 against independent authenticated current/base evidence."
    - "Optional firmware and libretro_saves claims are enabled only with actual in-process and hosted proofs; save states remain unavailable."
    - "The normal CI and retained nightly lanes pass at unchanged required/advisory status, with no rebaseline, pin/predicate weakening or owner UAT."
---

# Phase 7: Battery saves across hosts Verification Report

**Phase Goal:** Keep HSC and SaveKey progress across process and frontend restarts without putting file I/O in the core.
**Verified:** 2026-10-24T15:47:56Z
**Status:** passed
**Re-verification:** No — initial verification; no previous report or overrides existed.

The actual implementation and behavioral evidence satisfy SAVE-01–06. No BLOCKER, WARNING requiring a decision, behavior-unverified invariant, or owner verification remains. This report does not mark the phase complete or claim post-merge release behavior.

The evidence source is `54485d630c421716d51a8d7919ded429742acf4a`, with genuine independent parent `6784e4758ddb8e96a6048c986ff0e6b5e4248c01`. Later HEAD `36a9b7f` changes planning only. Reviewed production source `90b9b7a` is unchanged afterward; `54485d6` only applies the existing native1/PPC13 scale to the new closed-pipe child wait, preserving deadlines, exit assertions and full-byte checks.

## Goal Achievement

### Observable Truths

The five ROADMAP success criteria are retained verbatim in rows 4, 5, 8, 15 and 32. They are merged with the corresponding plan truths (02.1, 02.2, 03.1, 05.1 and 09.2), whose additional details are covered in the evidence. The other 30 plan truths add detail. No roadmap scope was dropped. Every runtime transition below has passing behavioral evidence, rather than only symbol or file presence.

| # | Truth | Status | Evidence |
| --- | --- | --- | --- |
| 1 | Before battery fixture acceptance, the actual evidence producer and unchanged authenticated join accept only the named owned battery diagnostic addition and retain raw trusted base identity. | ✓ VERIFIED | `quality_evidence.cmake:24–50` includes the real relation and resolves raw event-base Git objects; fresh `policy.quality.controls` passes. |
| 2 | Changed, deleted, duplicated or unrelated manifest rows and forged compiler/platform/base/run evidence fail the actual consumer. | ✓ VERIFIED | Actual producer recursion and unchanged join fault controls pass; forged run/base/compiler/platform and changed/duplicate manifest cases are rejected. |
| 3 | Only the exact supported behavior annotation transition is admitted; all prior sample digests, pins and numeric predicates remain protected. | ✓ VERIFIED | `quality.cmake` admits equality and literal adjacent annotation transitions; protected policy/pins/join/equality files are unchanged from pre-phase. |
| 4 | The load-stable battery span has layout version 1, a correct change generation and power-cycle retention; raw .sav/.srm bytes agree. | ✓ VERIFIED | `instance.c:377–402` exposes layout 1 and the authoritative span; fresh `core.hsc.tracer`, battery reset/deadline and `diag.battery` prove restore before instructions, changed stores, retained token and raw interoperability. |
| 5 | HSC SRAM and supplied-ROM mapping work; the ROM-free signature stub is explicitly limited to compatible direct-SRAM software. | ✓ VERIFIED | `cart.c:904–910,1013–1019`; fresh `core.hsc.decode` checks every mapped SRAM/ROM byte, CPU/MARIA/peek, MEN/BIOS priority and inert FF/C6/FE stub. Public docs state its limits. |
| 6 | Only exact 4096-byte supplied firmware is copied; unknown identity or absent signatures warn; malformed media refuses transactionally. | ✓ VERIFIED | `instance.c:225–289,331–364`; fresh `core.hsc.firmware` checks exact size, identity, signature warnings, alias copying, refused state and continued execution. |
| 7 | A real client compiled with the frozen API-4 layout links against the new library and retains old minimum-size behavior. | ✓ VERIFIED | `tests/api/v4-layout.c` defines frozen old structs without the current header; `tests/abi/tests.cmake:103–113` links the current archive; fresh `abi.api_v4_layout` passes. |
| 8 | SaveKey/AtariVox EEPROM diagnostics cover repeated START, page wrap, sequential reads and emulated-time busy polling. | ✓ VERIFIED | `bus.c:448` supplies the actual slow data tick to RIOT; `riot.c:155–156` observes ORA/DDRA; fresh pins/protocol/wrap/recovery tests execute CPU waveform traffic on both ports and identities. |
| 9 | Only one EEPROM attaches at load, and its port and battery offset stay frozen; rejected hot changes do not alter pending joystick input. | ✓ VERIFIED | `instance.c:239–267`, `ports.c` transactional input validation; fresh `core.hsc.topology`, loaded `layout`/`devices` cases preserve storage and ordinary pending input. |
| 10 | Buffered page writes become persistent only at the selected 5 ms emulated deadline; frame catch-up and reset obey before/equal/after boundaries without polling. | ✓ VERIFIED | `ports.c:31–48`, `frame.c:90`, `instance.c:177`; fresh deadline/no_poll/reset cases exercise minus-one/equal/plus-one, instruction overshoot, JAM, DMA and partial refusal. |
| 11 | Same-value programming is busy but clean; reads/peeks, address-only traffic and aborts do not change generation. | ✓ VERIFIED | Fresh `core.battery.deadline`, `input.savekey.pins` and `recovery` assert same-value busy/clean state and observers/address-only/aborted traffic. |
| 12 | Every supported layout and either EEPROM port remains deterministic, isolated and allocation-free after load. | ✓ VERIFIED | Fresh `prop.battery_lifecycle`, `prop.battery_observers` and `core.no_alloc_after_load`; six layout/placement configurations, peer isolation and fixed post-load ownership. |
| 13 | Host import is clean; changed emulated commits alone alter generation, including defined uint64 wrap; reset and refused load preserve dirty evidence. | ✓ VERIFIED | Fresh lifecycle property plus decode/deadline/reset/firmware cases assert clean restore, actual changes, unsigned wrap and continuation after refusal. |
| 14 | Malformed and incomplete I2C traffic remains bounded and cannot mutate another instance or the inactive battery region. | ✓ VERIFIED | `tests/fuzz/i2c.c` caps input at 4096 operations with bounded frames; fresh replay and specific isolated-instance fault control pass; actual nightly fuzz passes. |
| 15 | Runner reload, periodic/on-demand/exit flush and failed atomic-replace tests preserve existing progress, including short/oversized inputs. | ✓ VERIFIED | `runner/main.c:1256,1299,1318–1321,1384–1389`; fresh roundtrip/import/cadence/faults/controls tests prove pre-frame import, fresh process, all flush routes and failure retention. |
| 16 | Missing/zero/short saves are FF-padded and clean; exact saves copy verbatim; oversized input exits 4 and other import errors exit 1 without frames or replacement. | ✓ VERIFIED | `save_import` reads at most span+1 into FF staging before applying/arming; fresh import/options/nonregular tests check every active length, exact bytes and exit 4 versus 1. |
| 17 | A dirty save replaces the destination only after exclusive own-temp full write, flush, sync and close; every pre-replacement failure preserves prior bytes. | ✓ VERIFIED | `save_flush` exclusive 32-attempt sibling creation, complete write/flush/sync/close and one rename/MoveFileEx; fresh faults compare complete prior bytes and foreign-temp ownership. |
| 18 | No persistence occurs without opt-in, after rejected import/load/BIOS, or as a cleanup retry after failure. | ✓ VERIFIED | `save.armed`, `save.failed`, BIOS suppression and finalization ordering are exercised by fresh actual-main faults/options; no clean-up retry can replace rejected media. |
| 19 | Dirty progress waits at most F subsequently completed frames for periodic persistence; clean cadence/request/exit does not rewrite. | ✓ VERIFIED | Fresh `runner.save.cadence` checks F=1/default60/UINT32_MAX, change after boundary, clean requests and exact replacement frame numbers. |
| 20 | POSIX USR1 flushes and INT/TERM stop after the current frame; Windows native BREAK flushes and C stops with synchronized request flags. | ✓ VERIFIED | Fresh native `runner.save.controls` and full CI pass; Windows x64/arm64 current/base jobs execute actual private-console BREAK/C delivery. `stop.c` handlers only store synchronized flags. |
| 21 | Coalesced requests and requests arriving during a flush remain effective; handlers perform no I/O or core work. | ✓ VERIFIED | Fresh cadence test injects a request during the actual write and requires replacements on frames 1/2/3; `stop.c` consumes by signal-blocked clear or Interlocked exchange before I/O. |
| 22 | Every actual-consumer pre-replacement fault preserves old destination bytes, retains dirty evidence and exits once without cleanup retry. | ✓ VERIFIED | Fresh `runner.save.faults` checks create/fdopen/partial/write/flush/sync/close/replace, 32 collisions, previous full bytes, owned cleanup and one failed attempt. |
| 23 | SAVE_RAM is the exact core span immediately after retro_load_game, before callbacks or first run; pointer/size remain stable until unload/reload. | ✓ VERIFIED | `libretro.c:912` queries final core storage before successful load returns; memory getters publish cached pointer/length directly. Fresh loaded battery/order cases restore and execute before any prior setter. |
| 24 | Absent/unloaded media returns NULL/0; reset retains committed bytes; late storage-bearing changes are visibly reload-only. | ✓ VERIFIED | Fresh loaded battery/order/layout cases check absent/no-content/unload NULL/0, reset retention and once-per-changed reload notice; held-A semantics remain intact. |
| 25 | HSC descriptors alias actual SRAM and the installed supplied ROM/stub through public API queries; serial EEPROM is not a fabricated CPU memory range. | ✓ VERIFIED | `lr_memory_maps` uses public battery and installed-ROM queries at $1000/$3000, SRAM alias and CONST ROM. Fresh firmware/resolver tests inspect real descriptors and serial absence. |
| 26 | Every intended HSC address resolves to the correct byte using the pinned rcheevos resolver without changing retained cartridge maps or resolver limits. | ✓ VERIFIED | Fresh `libretro.rcheevos` passes all-byte real pinned resolver checks on retained boards with absent/stub/supplied HSC; existing capacities remain unchanged. |
| 27 | The admitted owned diagnostic increments independent HSC and EEPROM counters via CPU-visible SRAM and real I2C, waits for programming, reads back, and displays observed values. | ✓ VERIFIED | Owned `battery.s` executes $1100 SRAM and serial $6000 transactions, waits/polls/readbacks, then reports six mailbox records and own glyphs; fresh `diag.battery` and reproducible rebuild pass. |
| 28 | Two fresh RetroArch processes show counter pairs 1/1 then 2/2 while sharing only designated save storage; two independent fresh runner processes do the same. | ✓ VERIFIED | Actual final RetroArch job runs two new processes with distinct RA_DIRs; fresh runner processes use separate empty storage, then restore only their own previous save. Complete counters are 1/1 then 2/2. |
| 29 | Each complete 34816-byte frontend snapshot equals its corresponding runner snapshot and independent expected media; independent counter pixels reject equal black/wrong frames. | ✓ VERIFIED | `driver.c:87–108` independently checks all 34816 bytes, two distinct counter offsets and FF elsewhere; comparator independently checks both renderers' glyphs, then whole-frame equality; actual hosted logs confirm both counts. |
| 30 | Hosted coverage has exactly DMA equivalence and the save roundtrip; no-content remains covered in-process and tests/retroarch stays <=300 physical lines. | ✓ VERIFIED | Workflow contains DMA and battery only; no-content stays in loaded-module cases. Fresh physical count is 300 lines across four files; launch helper rejects save/run/firmware overlap before cleanup. |
| 31 | Complete native and PowerPC inventories and equality retain all old digest/numeric observations and add independently validated battery observations. | ✓ VERIFIED | Fresh complete local/native/PPC inventory consumers pass 641+6; downloaded full native/PPC/local hash files are byte-identical at 69 rows. Old 47 identities remain, with only authorized behavior metadata advanced. |
| 32 | The second bounded hosted RetroArch case proves a save survives two fresh launches; existing in-process checks carry the detailed semantics. | ✓ VERIFIED | Independently read successful final RetroArch job log: two launches, full-media count 1 and 2, strict pixel equality, negative pixel 7,3; screenshots personally inspected by verifier. |
| 33 | Every native lane's real library+runner+libretro ZIP sum is <=399937 against independent authenticated current/base evidence. | ✓ VERIFIED | Actual downloaded ZIP filesystem sums equal exact-source measurements on all six lanes, matching independent parent, raw base policy and environment; worst current 393952/base393994 <=399937. |
| 34 | Optional firmware and libretro_saves claims are enabled only with actual in-process and hosted proofs; save states remain unavailable. | ✓ VERIFIED | Fresh info claims and specific negative control pass; optional `highscre.bin` maps to real firmware test and `libretro_saves` to battery test. Serialization remains false; hosted proof is separate. |
| 35 | The normal CI and retained nightly lanes pass at unchanged required/advisory status, with no rebaseline, pin/predicate weakening or owner UAT. | ✓ VERIFIED | Independently queried exact54485d6 CI attempt2 and Nightly successes, fresh full local workflow641+6; retained quality/perf/floors/pins unchanged. Same-head Mac retry and scaled PPC harness correction are documented accurately. |

**Score:** 35/35 truths verified; 0 present but behavior-unverified. No override or coincidental-reliance finding was needed. Production establishes the required topology/defaults and ordering; test inputs are equivalent to inputs real hosts provide.

### Required Artifacts

The artifact query ran against all nine plans: all 28 declared entries pass existence/substance checks (including repeated declarations). Manual inspection verified their behavior and wiring:

| Artifact | Expected | Status | Details |
| --- | --- | --- | --- |
| `tests/cmake/quality.cmake`, `tests/policy/quality.cmake` | Narrow admission and actual producer/join faults | ✓ VERIFIED | Explicit named fixture relation, raw base identity, exact revision relations and real recursive consumer controls. |
| `include/muhtari.h` | Additive API5/ABI1/behavior4 | ✓ VERIFIED | Old media prefix/minimum retained; complete appended fields only; size-tagged battery and const-ROM views. |
| `src/internal.h`, `src/instance.c`, `src/cart.c` | Stable storage, validation, HSC map | ✓ VERIFIED | One 34816-byte backing array; separate installed ROM and unpublished firmware staging; validated candidate before installation. |
| `tests/api/v4-layout.c` | Genuine frozen client | ✓ VERIFIED | Independent old declarations, current-archive linkage, minimum-size calls and frame execution. |
| `tests/core/test_core.c` | Executed HSC/lifecycle boundaries | ✓ VERIFIED | Real CPU tracer; exhaustive bytes, generation/reset/refusal/deadline/no-poll/allocation assertions. |
| `src/ports.c`, `src/frame.c` | Bounded serial device and due completion | ✓ VERIFIED | Finite phases and 64-byte page; persistent bytes publish only at actual due tick. |
| `tests/input/test_input.c` | Pin/protocol tests | ✓ VERIFIED | Executed LDA/STA ORA/DDRA waveforms, both ports/identities, ACK/NAK/wrap/recovery. |
| `tests/prop/test_prop.c`, `tests/fuzz/i2c.c` | Lifecycle/observer/isolation/hostile evidence | ✓ VERIFIED | Fixed seed, peer/expected-byte checks, bounded replay and specific fault control; existing nightly integration. |
| `tests/inventory/ci.txt` | Complete native/PowerPC names | ✓ VERIFIED | Actual XML exact-once 641 normal and 6 release checks, zero failure/error/skip. |
| `runner/main.c`, `runner/exit_status.h` | Opt-in import and safe replacement | ✓ VERIFIED | Real run consumer, exit4, staged import, exclusive owned temp, sync/close and native replacement. |
| `runner/stop.c` | Native request synchronization | ✓ VERIFIED | POSIX signal masking and sig_atomic_t; Windows Interlocked callback flags; no handler I/O. |
| `tests/runner/test_save.c`, `tests/runner/test_control.c` | Actual-main faults and OS child controls | ✓ VERIFIED | Full-file assertions, injected consumer faults, genuine subprocess signals/console events and broken stdout pipes. |
| `libretro/libretro.c` | Immediate exact SAVE_RAM and truthful maps | ✓ VERIFIED | Public-only production API, final load-time topology, direct pointer publication; no save-file ownership. |
| `tests/libretro/host.c`, `tests/libretro/rcheevos.c` | Loaded module and real resolver | ✓ VERIFIED | Lifecycle/import/callback order, held-A and 112-header request matrix; all mapped HSC bytes. |
| `tests/diag/battery.s`, `battery.cfg`, `battery.a78` | Owned reproducible hardware diagnostic | ✓ VERIFIED | Authentic source commit4642ded and manifest SHA25610f5bc279d8147b3348e102ca9f7e4dc496dd31b5099adb2ef64320e987fb4d4; rebuild passes. |
| `tests/diag/driver.c`, `expected.csv` | Independent numeric/media oracle | ✓ VERIFIED | Exact counters/CRC/report schema and all34816 bytes, including untouched FF bytes. |
| `tests/retroarch/compare.c` | Independent pixel oracle | ✓ VERIFIED | Three glyph fields checked independently in both renderers, then every pixel compared; specific fault controls. |
| `tests/cmake/hashes.cmake` | Complete validated old/new observations | ✓ VERIFIED | Independent judgments before22 battery rows; malformed/missing/duplicate/revision/media faults tested. |
| `libretro/muhtari_libretro.info.in` | Evidence-backed claims | ✓ VERIFIED | Saves/optional HSC enabled with real tests and separate hosted evidence; states unavailable. |

### Key Link Verification

All 23 declared links were checked manually. The generic `verify.key-links` command exited1 for each plan because it looks for literal path text and cannot resolve relative includes, C symbols, inverse callers, symbol-qualified `from` strings or build targets. Those heuristic negatives were inspected, not accepted as runtime failures or silently ignored; the concrete links below prove wiring.

| Plan | From → To | Via / actual evidence | Status |
| --- | --- | --- | --- |
| 01 | quality_evidence → quality relation; policy controls → quality_join | Relative include at producer line24; raw base Git show; actual current/base outputs consumed by unchanged join. | ✓ WIRED (2) |
| 02 | instance → cart; cart → instance battery; frozen client → current archive | `muhtari__cart_install` before candidate commit/boot; `internal.h` and direct authoritative storage; ABI CMake target links old declarations without current header. | ✓ WIRED (3) |
| 03 | bus → RIOT; RIOT → ports; frame → ports | `muhtari__riot_write(...ticks + data_at(len))`; `muhtari__savekey_pins` after ORA/DDRA; catch-up after real execution. | ✓ WIRED (3) |
| 04 | fuzz registration → i2c.c; property registration → test_prop.c | Same source for replay/fuzzer/fault; fixed seed mechanism and nightly10000 override. | ✓ WIRED (2) |
| 05 | runner load → battery query/import; save_flush → native replacement | Import before frame_input/run_frame; one checked exclusive-temp transaction followed by rename/MoveFileEx. | ✓ WIRED (2) |
| 06 | stop requests → frame-loop transaction; test seam → actual runner | Consume before I/O at completed-frame boundary; test translation unit includes actual main with narrow substitutions. | ✓ WIRED (2) |
| 07 | adapter → public queries; SAVE_RAM getter → queried span; resolver test → pinned resolver | Load query at libretro.c912; cached pointer/length directly returned; real rc_libretro_memory_init/find on actual descriptors. | ✓ WIRED (3) |
| 08 | manifest → real evidence producer; launcher → config; workflow → shared diagnostic judge | Exact named admitted row; configure_file uses private RA_DIR/external SAVE_DIR; actual --battery-count/--battery-media for each snapshot. | ✓ WIRED (3) |
| 09 | hashes → equality; info claims → real inventory; evidence → authenticated join | Workflow gathers full hash outputs for unchanged equality; claim tests registered in libretro/tests.cmake; exact current/base environment/source/policy checks in unchanged join. | ✓ WIRED (3) |

### Data-Flow Trace (Level 4)

| Artifact / value | Source and path | Produces real data | Status |
| --- | --- | --- | --- |
| HSC saved bytes/token | Executed CPU stores → cartridge decode → instance battery array; changed-value comparison increments token | Yes; tracer independently restores then executes two stores and observes one change | ✓ FLOWING |
| EEPROM saved bytes/token | Executed ORA/DDRA bus data ticks → finite pin model → pending page → due completion → battery suffix | Yes; CPU readback and precise pending/commit/reset tests | ✓ FLOWING |
| Runner .sav | Opted-in raw file → FF-filled bounded staging → exact queried span before frame → actual changed generation → checked file transaction | Yes; complete import/fault/fresh-process byte assertions | ✓ FLOWING |
| Frontend .srm | Queried authoritative core span published at load → RetroArch restore → real emulation → frontend save → next fresh process restore | Yes; two full independently judged snapshots, separate fresh runner path | ✓ FLOWING |
| Visible counter glyphs | HSC readback and serial EEPROM readback in owned ROM → mailbox/pixel glyphs | Yes; counts1/1 then2/2; supplied marker5A then stubFF | ✓ FLOWING |
| Equality rows | Fresh runner hardware reports/media → independent numeric/full-media validation → appended observations | Yes; complete69-row identity and native/PowerPC/WASM equality | ✓ FLOWING |

No static return, hardcoded empty span, mirrored save array or disconnected observer stands in for device data.

### Behavioral Spot-Checks

For each named test below the verifier ran the separate command
`ctest --preset ci -R '^NAME$' --no-tests=error --output-on-failure`,
escaping dots as `[.]`. Every command exited0, selected the named test, and finished below10 seconds; CTest also ran required fixture dependencies. Run URL for these local commands: **N/A-local**.

| Named tests (one command per name) | Result |
| --- | --- |
| core.hsc.tracer, decode, firmware, topology, sizes | All pass; exact mapped bytes, clean restore, transactional firmware/topology and old-size behavior |
| core.battery.deadline, no_poll, reset | All pass; precise state transitions, no polling, reset/discard ordering |
| abi.api_v4_layout; core.no_alloc_after_load | Both pass; real frozen client and allocation counters |
| input.savekey.pins, protocol, wrap, recovery | All pass; real CPU pin traffic and complete protocol edges |
| prop.battery_lifecycle, prop.battery_observers | Both pass; fixed seeds, full-state observers and all layouts/placements |
| fuzz.regress.i2c, fuzz.regress.i2c.self_test | Both pass; actual replay and precise isolated-instance fault detection |
| runner.save.import, roundtrip, faults, options, cadence, controls, nonregular | All pass; complete media, real process/control delivery, failed-save cleanup |
| libretro.host.battery, battery_order, hsc_firmware, layout, devices | All pass; actual loaded module, callback ordering and reviewed input regressions |
| libretro.rcheevos | Pass; real resolver,9.14s named behavior,9.68s with fixtures |
| diag.battery; diag.rebuild | Both pass; independent fresh-process media and reproducible admitted fixture |
| policy.quality.controls | Pass; actual producer/join pass/fault consumers,7.64s |
| runner.hashes_file | Pass; independently validated complete observations and malformed-report/media controls |
| libretro.info_claims, libretro.info_claims.self_test | Both pass; real capability inventory and specific negative controls |

The preset writes `junit.xml` even for selected tests. An intermediate inventory check therefore correctly failed against the last selected-test receipt; that is not a product failure or a passing full-suite receipt. After all spot-checks, the verifier ran the full workflow once and rechecked its resulting exact inventories. No later preset test overwrote them.

| Final command / check | Exit | Result / run URL |
| --- | ---: | --- |
| `gsd_run run-with-timeout 60 -- caffeinate -is cmake --workflow --preset ci` | 0 | Fresh rebuild,641 normal tests (45.44s), real3-component packages,6 release tests; N/A-local |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | Exactly641 once each; N/A-local |
| Same consumer with `junit-release.xml -DLABELS=release` | 0 | Exactly6 once each; N/A-local |
| Same consumer on downloaded final native/PPC normal/release XML | 0 each | 641+6 for each full execution; [Nightly equality](https://github.com/szTheory/muhtari/actions/runs/40104750452/job/120904008838) |
| `gh run view 40104749040 --json status,conclusion,headSha,url,attempt,jobs` | 0 | completed/success,attempt2,exact54485d6; [CI](https://github.com/szTheory/muhtari/actions/runs/40104749040) |
| `gh run view 40104750452 --json status,conclusion,headSha,url,jobs` | 0 | completed/success,exact54485d6; [Nightly](https://github.com/szTheory/muhtari/actions/runs/40104750452) |
| `XDG_CACHE_HOME=/tmp/muhtari-verify-gh-cache gh run view 40104749040 --job 120911136203 --log` | 0 | Actual two-case frontend logs, both complete count verdicts and negative pixel7,3; [RetroArch](https://github.com/szTheory/muhtari/actions/runs/40104749040/job/120911136203) |
| Python standard-library archive/JSON identity assertions | 0 | Exactly3 ZIPs/lane, actual sizes match current metrics, authentic parent/base policy/environment and run identity; [quality](https://github.com/szTheory/muhtari/actions/runs/40104749040/job/120911136250) |
| Python complete local/native/PPC hash comparison | 0 |69 rows byte-identical; [equality](https://github.com/szTheory/muhtari/actions/runs/40104750452/job/120904008838) |
| `git diff --exit-code bbaa3fe -- quality.json pins.txt tests/cmake/quality_join.cmake tests/cmake/lane_equality.cmake tests/compat/ledger.csv` | 0 | Protected authority/pins/predicates unchanged; N/A-local |
| `wc -l tests/retroarch/*` | 0 |76+129+66+29=300; N/A-local |
| `git diff --check` | 0 | No whitespace errors; N/A-local |

The final CI job conclusions independently checked include all six builds, hygiene, both nofp lanes, library, be32, ASan, performance, RetroArch, quality, six-native hash equality, WASM and CI required. The non-PR title job is intentionally inapplicable. Nightly includes full vectors258, six10000-case property checks, fuzz10 including I2C, retained benchmarks, and full native/PowerPC equality. WASM executes580 and retains advisory status; ASan executes626. Native Windows x64 and arm64 execute actual console/control/replacement consumers in current and independent-base builds.

CI attempt1's Mac x64 policy timeouts remain historical failures; identical-source attempt2 succeeds without changing source or bounds. The earlier PPC harness wait failure was corrected by applying its pre-existing scale only to that new wait; the final full PPC consumer passes7.82s. Neither is hidden as a first-attempt success or used to weaken an assertion.

### Archive Bound

All values below were independently checked from actual downloaded ZIP sizes and exact-source current/base JSON, not copied from SUMMARY claims. The base policy is byte-identical to the genuine parent's Git object. Both observations identify the same compiler/platform environment and raw trusted inputs.

Reproducible artifact check command (exit0; source [CI40104749040](https://github.com/szTheory/muhtari/actions/runs/40104749040)); inputs are ignored downloads of its six `quality-<lane>` and `archives-<lane>` artifacts:

```sh
python3 -c 'import json,subprocess; from pathlib import Path
source="54485d630c421716d51a8d7919ded429742acf4a"
parent=subprocess.check_output(["git","rev-parse",source+"^1"],text=True).strip()
assert parent=="6784e4758ddb8e96a6048c986ff0e6b5e4248c01"
policy=subprocess.check_output(["git","show",parent+":quality.json"])
run="https://github.com/szTheory/muhtari/actions/runs/40104749040"
for lane in Path("build/battery-final-54485d6-quality").iterdir():
    if not lane.is_dir(): continue
    current=json.loads((lane/"quality-current.json").read_text())
    base=json.loads((lane/"quality-base.json").read_text())
    a=current["metrics"]["archive_bytes"]; b=base["metrics"]["archive_bytes"]
    assert a["sha"]==source and b["sha"]==parent and a["source"]==run
    assert current["environment"]==base["environment"]
    assert (lane/"quality-base-policy.json").read_bytes()==policy
    assert (lane/"quality-base-sha.txt").read_text().strip()==parent
    files=list((Path("build/battery-final-54485d6-archives")/lane.name).glob("*.zip"))
    assert len(files)==3
    assert sum(f.stat().st_size for f in files)==a["value"]<=399937
    assert b["value"]<=399937
    print(lane.name,a["value"],b["value"])
paths=[Path("build/ci/hashes.txt"),Path("build/battery-final-54485d6-equality/evidence/native/hashes.txt"),Path("build/battery-final-54485d6-equality/evidence/be/hashes.txt")]
data=[p.read_bytes() for p in paths]
assert data[0]==data[1]==data[2] and len(data[0].splitlines())==69'
```

| Native lane | Current3-ZIP sum | Independent parent sum | Ceiling | Status |
| --- | ---: | ---: | ---: | --- |
| Linux arm64 |284545|284545|399937|✓ VERIFIED|
| Linux x64 |277824|277824|399937|✓ VERIFIED|
| macOS arm64 |230503|230504|399937|✓ VERIFIED|
| macOS x64 |232671|232674|399937|✓ VERIFIED|
| Windows arm64 |371070|371131|399937|✓ VERIFIED|
| Windows x64 |393952|393994|399937|✓ VERIFIED|

### Probe Execution

Not applicable: no plan/SUMMARY declares a shell probe path and there is no conventional `scripts/*/tests/probe-*.sh` for this phase. The phase's runnable acceptance consumers are the named CTest cases, actual hosted workflows and one-off commands above, which the verifier executed or independently inspected. SUMMARY PASS narration was never substituted for a probe run.

### Requirements Coverage

| Requirement | Source plans | Description / actual evidence | Status |
| --- | --- | --- | --- |
| SAVE-01 |02,05,07–09|2KB HSC SRAM, copied exact4KB supplied ROM, documented inert stub; exhaustive decode/firmware tests and hosted supplied/stub markers|✓ SATISFIED|
| SAVE-02 |03,04,08,09|32KB pin-level SaveKey/AtariVox EEPROM; actual ORA/DDRA traffic, repeated START, page/read wrap, busy/deadline and hostile replay/fuzz|✓ SATISFIED|
| SAVE-03 |02–04,07–09|Stable raw layout1, exact absent/2048/32768/34816 spans, actual changed-byte generation, import/reset/refusal/wrap behavior|✓ SATISFIED|
| SAVE-04 |05,08,09|Per-game opt-in .sav reload before first frame, FF short padding, exact oversize4/error1 and fresh-process full-byte proof|✓ SATISFIED|
| SAVE-05 |05,06,08,09|Periodic/request/exit flushing, native controls, exclusive transaction, every pre-replacement fault retaining prior bytes, no cleanup retry|✓ SATISFIED|
| SAVE-06 |07–09|Real two-fresh-launch RetroArch persistence, full.srm/.sav agreement, independently checked counters/pixels/media, truthful claims|✓ SATISFIED|

All SAVE-01–06 requirement IDs appear in plan frontmatter and match REQUIREMENTS' Phase7 assignment. No orphaned requirement was found. Plan01 supplies the shared evidence prerequisite without claiming product completion on its own.

### Decision Coverage

The owner query `gsd_run query check.decision-coverage-verify .planning/phases/07-battery-saves .planning/phases/07-battery-saves/07-CONTEXT.md` exited0:
**All trackable CONTEXT.md decisions are honored by shipped artifacts.** 18/18 honored,0 unhonored. Manual boundary review also found one byte owner, load-time topology, bounded serial state, host-only files, public-only adapter access, independent test judges and retained resource/evidence authorities consistent with ENGINEERING §11.

### Flagged Assumptions and Intent Prohibitions

The ten original flag rows and four descriptor-less prohibition rows remain serialized in their original plans as unresolved/flagged-unverified. This report does not rewrite their historical lifecycle, invent an enforcement descriptor, or claim the planner had already resolved them. Project AGENTS rule6 and the explicit phase contract assign evidence and intent judgment to the agent; no routine owner interview applies.

| Original flag concern | Current evidence/disposition |
| --- | --- |
| SAVE-01 unclassified | HSC tracer/decode/firmware and stub byte/claim review satisfy the actual contract. |
| SAVE-02 unclassified | Both-port CPU pin/protocol/wrap/recovery, exact deadline and bounded fuzz evidence satisfy the model. |
| SAVE-03 unclassified | Frozen client, stable span/layout/token, allocation and lifecycle properties satisfy ownership. |
| SAVE-04 idempotency | Missing/short/exact clean imports do not create/rewrite until emulated change. |
| SAVE-04 concurrency | Explicit one writer per basename path; directory isolation and arming-before-execution tests match scope. |
| SAVE-05 concurrency | Consume-before-I/O, during-write request test, native flags, exclusive temporary ownership and faults pass. |
| SAVE-06 boundary | All four storage sizes and immediate publication before callbacks/first run pass. |
| SAVE-06 precision | Integer selected interval; exact counters, every34816 byte and independent pixels pass. |
| SAVE-06 idempotency | Reset retains bytes; import is clean; each fresh process increments each medium exactly once. |
| SAVE-06 concurrency | Frozen topology and private frontend launch trees share only designated save storage; overlap faults reject before cleanup. |

| Prohibition | Agent intent verdict / evidence |
| --- | --- |
| Do not present the signature stub as firmware routines, formatting, screens or full OpenHSC. | ✓ VERIFIED: exhaustive FF/C6/FE bytes; embedding/runner/libretro/cartridge docs explicitly state direct-SRAM limitation. |
| Do not present selected5ms as measured timing or AtariVox EEPROM as speech. | ✓ VERIFIED: controllers docs distinguish DS20001203Y maximum from selected35796-tick model; speech explicitly unavailable. |
| Do not promise universal power-loss durability or multiple-writer coordination. | ✓ VERIFIED: runner docs name replacement commit point, one writer, separate directories and explicit durability limits; actual transaction/fault tests match. |
| Do not infer persistence/firmware/compatibility solely from matching hosts. | ✓ VERIFIED: independent mailbox/count/media/pixel predicates precede cross-host equality; optional firmware has separate validation/failure evidence; no broader retail compatibility claim. |

These are explicit agent-owned documentation/intent checks, not silently passed typed test/judgment-tier prohibitions. No typed prohibition or backstop truth was declared; no missing enforcement was converted into green.

### Test Quality Audit

| Test owner | Linked requirements | Active / skipped | Circular | Strongest assertion | Verdict |
| --- | --- | --- | --- | --- | --- |
| core + frozen API |SAVE-01,03|Active;0 skipped|No|Executed CPU state transitions, exact bytes/token, size canaries and refused-load continuation|PASS|
| input |SAVE-02,03|Active;0 skipped|No|Real CPU bus waveforms, ACK/readback, full-page/region expected values and exact timing|PASS|
| properties + I2C replay/fuzzer |SAVE-01–03|Active;0 skipped|No|Independent expected bytes, whole-state observers, peer/region isolation and specific injected invariant fault|PASS|
| runner save/control consumers |SAVE-04,05|Active;0 skipped|No|Actual main plus fresh child processes, exact full files, native delivery, specific failure and no-retry assertions|PASS|
| loaded module + pinned resolver |SAVE-01,03,06|Active;0 skipped|No|Before-run restore and hardware readback, all-byte descriptors, lifecycle/callback and ordinary input behavior|PASS|
| diagnostic + hosted comparator |SAVE-01–06|Active;0 skipped|No|Independent exact numeric counters, CRC, all34816 bytes and glyph fields; real fresh-host processes|PASS|
| quality/hash/claim controls |Shared acceptance|Active;0 skipped|No|Actual producer/consumer pass/fault cases and authenticated identities; claim inventory supplemented by hosted behavior|PASS|

Disabled-test scan substring hits such as `exit(`, `_exit(` and `expect_exit(` were inspected: they are ordinary C termination/assertion helpers, not skipped tests. Expected battery media is independently constructed from the declared layout/count/FF invariant, never recorded from the emulator as a golden. Generated runtime screenshots are observation inputs; independent glyph checks prevent circular equality from becoming the oracle.

Disconfirmation pass: the .info inventory check alone only proves registration, not persistence; the separate actual hosted roundtrip supplies the missing behavioral proof. Hardware-exact EEPROM latency, firmware routines, universal crash durability and multiwriter coordination remain explicit nonclaims rather than partially fulfilled requirements. Error paths sought included invalid present firmware, partial imports, every precommit operation, output failure/SIGPIPE, late storage requests and Auto fallback; each has active concrete coverage. No required untested error path or partially met requirement was established.

### Anti-Patterns Found

| File / scope | Pattern | Severity | Impact |
| --- | --- | --- | --- |
|79 changed implementation/test/doc paths|No unreferenced TBD/FIXME/XXX, placeholder or disabled requirement test|None|No debt-marker or stub blocker.|
|Pinned untouched libretro header|Upstream TODO comments|Info|Outside phase changes; no shipped save behavior depends on an unfinished handler.|
|API unavailable state/cheat functions|Intentional false/empty unsupported interfaces|Info|Save states/cheats are not claimed; battery persistence uses real SAVE_RAM instead.|

Core source inspection finds no new host file/signal/device I/O. Mutable protocol and bytes remain instance-owned, integer-only and bounded; actual no-allocation, ABI/symbol, nofp, sanitizer, fuzz and portability gates retain their required/advisory roles.

### Human Verification Required

None. The verifier inspected the final hosted screenshots and current docs against code, and independently checked actual RetroArch/Windows/native evidence. AGENTS rule6 reserves owner questions for identity-bound consent or the owner's own hardware; neither is required for this phase. No invariant remains present-but-behavior-unverified.

### Gaps Summary

No actionable gap or deferred failed must-have. Later milestone phases cover new controllers, PAL, expansion boards and broader compatibility; none is used to excuse a Phase7 failure. No override was applied, no third review-fix round was opened, and the gap round remains unused. CR-01/SIGPIPE, CR-02/effective-device mapping and CR-03/Auto fallback are closed with current passing regressions. The descriptor-less historical flags remain visible without transferring routine verification to the owner.

The fingerprint covers durable implementation, tests, contract docs and phase PLAN/SUMMARY inputs. Mutable completion bookkeeping (STATE/ROADMAP/REQUIREMENTS/PROJECT) is excluded so the orchestrator's phase-completion update does not invalidate implementation evidence. No log or generated save/firmware/screenshot is committed.

---

_Verified: 2026-10-24T15:47:56Z_
_Verifier: the agent (gsd-verifier)_

