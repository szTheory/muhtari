# Phase 14: Proven frontend states and responsive play — Research

**Researched:** 2026-11-03  
**Confidence:** Medium. The shipped callbacks, current RetroArch job, fixture tools and official NCI commands are verified. NCI timing and the pinned app's run-ahead settings still need an actual bounded host run.

## Requirements

| Requirement | Research finding |
|---|---|
| FRONT-02 | The core supports the Phase 13 state API. Phase 14 must exercise RetroArch's save slot and restore path and verify visible progress after restore. |
| FRONT-03 | RetroArch's NCI exposes rewind. Verify repeated visible progress reversals in the host; retain the existing in-process audio, timing, input and battery checks. |
| FRONT-04 | RetroArch documents single-instance run-ahead as in-memory save/restore. Verify it through the pinned app, measure the same fixture with the feature off/on, and compare durable battery bytes after an explicit flush. |

## What exists

- Phase 13 is present on this branch. `libretro/libretro.c` returns the loaded machine's state size and delegates save/load to `muhtari_save_state` and `muhtari_load_state`. Its host tests cover normal, same-instance and same-binary contexts, refuse rollback-netplay context, and exercise battery spans. `.info` truthfully declares `savestate = "true"`. Sources: `libretro/libretro.c`, `libretro/muhtari_libretro.info.in`, `tests/libretro/host.c`, and the passed `13-VERIFICATION.md`.
- The only actual RetroArch consumer is the existing `retroarch` CI job on `macos-15`. It downloads the core from the same workflow build and verifies the pinned RetroArch 1.22.2 app. The job timeout is five minutes; `tests/retroarch/run_retroarch.cmake` bounds each child to 50 seconds, separates save storage from disposable RetroArch files, and currently runs a fixed frame count before taking a screenshot. Sources: `.github/workflows/ci.yml:407-550`, `pins.txt`, `tests/retroarch/run_retroarch.cmake`, `tests/retroarch/test.cfg.in`.
- The current battery diagnostic is a boot-time persistent-counter test with a results screen, not a live progress display. `tests/retroarch/compare.c` already checks independent 3x5 pixel glyphs as well as full-frame equality. Extend that oracle for the new progress fixture; equality alone can accept matching wrong images. Sources: `tests/diag/battery.s`, `tests/retroarch/compare.c`, `.planning/LESSONS.md` entry MU-TEST-003.
- The host config disables audio output. The Phase 13 library and loaded-module tests already compare state continuation, audio, input, timing, battery metadata and instance isolation. Reuse those proofs rather than treating a silent RetroArch job as audio evidence. Sources: `tests/retroarch/test.cfg.in`, `tests/core/test_state.c`, `tests/libretro/host.c`, `13-VERIFICATION.md`.
- `docs/libretro.md` explicitly says actual RetroArch save/load, rewind and run-ahead are not yet qualified. This phase must update that claim to the exact host evidence it obtains.

## Recommended implementation

Use one new owned progress-and-battery diagnostic and one NCI-controlled RetroArch process inside the existing host job. Keep the fixture and driver under `tests/diag/` and `tests/retroarch/`; generate screenshots, savestates and save files under the existing build output. Do not add a frontend framework or a new dependency.

The fixture should display an independently decodable frame/progress value and advance a battery-backed value only for committed gameplay frames. The host driver should:

1. Confirm the process version and loaded fixture with NCI `VERSION` and `GET_STATUS`.
2. Save at a known visible value; advance a fixed number of frames; restore; and verify both the restored progress and the same expected picture after replaying that interval.
3. Trigger rewind several times and verify each captured progress value moves backward. NCI button commands may not acknowledge a state change, so accept them only when the fixture oracle observes the expected change.
4. Measure the same fixed workload with run-ahead off and on. Configure one frame in single-instance mode, verify committed progress, then issue `SAVE_FILES` and require its `OK` response before comparing the raw saved bytes with the fixture's committed value.
5. Quit and reap the owned RetroArch child on every path. Bound NCI replies, polling and total child lifetime; keep the existing 50-second child and five-minute job limits unless later measurements justify one concrete adjustment.

Official NCI documentation lists `VERSION`, `GET_STATUS`, `FRAMEADVANCE` (while paused), `SAVE_STATE`, `LOAD_STATE`, `REWIND`, `SCREENSHOT`, `RUNAHEAD_TOGGLE`, `SAVE_FILES` and `QUIT`. NCI is UDP; send only to localhost on a test-selected port and keep the process, config and save paths isolated. The docs do not specify rewind frames per command, so the fixture's observed progress is the acceptance signal. References: [NCI](https://docs.libretro.com/development/retroarch/network-control-interface/) and [Run Ahead](https://docs.libretro.com/guides/runahead/).

RetroArch's run-ahead guide describes single-instance mode as speculative frames with audio/video disabled followed by a state restore, and says its state operations stay in RAM. It also describes a separate two-instance mode. The adapter and Phase 13 tests support contexts 1 and 2; the smallest frontend claim is one-frame, single-instance mode. The guide does not establish SRAM flush behavior, so compare the actual isolated save file after explicit `SAVE_FILES`.

## Bounds and exclusions

- Phase 12's failed host attempts and spent allowance remain untouched. Phase 14 gets its own host case within the existing RetroArch job bounds.
- Use only the owned fixture and pinned RetroArch 1.22.2 plus the same-run macOS arm64 core artifact. Do not claim support for other RetroArch versions or frontends.
- No Playstead work, rollback-netplay claim, user ROMs, release or publication action is included.
- Do not claim actual audible rewind quality: host audio is disabled. Keep the required audio/timing/input/battery semantics in the existing deterministic tests.
- Record performance observations as completed baseline and run-ahead work, without inventing a pass/fail timing threshold.

## Details to confirm in the bounded host test

- NCI's documented UDP examples target localhost, but the docs do not state which local interfaces the pinned app binds. Keep the test on the isolated hosted runner and verify its socket is not exposed beyond that runner before sending commands.
- NCI button commands do not promise a completion response. Use fresh screenshots and decoded progress; `SAVE_FILES` does provide `OK`/`NO`.
- The official docs do not specify how many frames one `REWIND` command restores. Establish this from repeated observed captures, not an assumed decrement.
- Configure the pinned app for one run-ahead frame and single-instance mode; report that path only if the host run proves the setting took effect.

## Sources

- Repository: `ROADMAP.md` Phase 14; `REQUIREMENTS.md` FRONT-02–04; `14-CONTEXT.md`; `libretro/libretro.c`; `libretro/muhtari_libretro.info.in`; `tests/libretro/host.c`; `tests/core/test_state.c`; `tests/retroarch/run_retroarch.cmake`; `tests/retroarch/test.cfg.in`; `tests/retroarch/compare.c`; `.github/workflows/ci.yml`; `pins.txt`; `docs/libretro.md`; `13-VERIFICATION.md`; `.planning/LESSONS.md` MU-TEST-003.
- Official: [RetroArch 1.22.2 release](https://github.com/libretro/RetroArch/releases/tag/v1.22.2), [Network Control Interface](https://docs.libretro.com/development/retroarch/network-control-interface/), and [Run Ahead](https://docs.libretro.com/guides/runahead/).

**Research limit:** No RetroArch process, CI job, release, publication or remote project action was run for this research.
