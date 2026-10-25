# Testing

Run CI locally with inventory and release checks.

## The entry point

```sh
cmake --workflow --preset ci
```

Needs CMake 4.3+, Ninja and C/C++ compilers:

1. configure `build/ci` (Release, warnings fatal, macOS deployment target 10.13);
2. build;
3. test every test except those labelled `release`, writing `build/ci/junit.xml`;
4. package the three release zips into `build/ci`;
5. test the `release` label against those zips, writing `build/ci/junit-release.xml`.

Tests build samples, OpenBIOS and rcheevos' resolver from pins. Fetch them first:

```sh
scripts/fetch-pin.sh wasmtime-<arch>-<os> build/pins/wasmtime   # for example wasmtime-aarch64-macos
scripts/fetch-pin.sh 7800basic-wasm build/pins/7800basic
scripts/fetch-pin.sh 7800openbios-asm build/pins/openbios
scripts/fetch-pin.sh rcheevos build/pins/rcheevos
```

Missing pins stop configure; no sample test is skipped.

Owned board diagnostics add [numeric and portability checks](emulation-tests.md#diag).
The [PAL diagnostic](pal-tests.md) runs startup plus 600 full frames even under ASan;
arithmetic carry alone cannot replace execution.
[Package acceptance](quality.md) still requires all six native lanes.

`-DMUHTARI_TESTS=OFF` builds products without tests or sample tools. Used by `perf` (head/base), nightly's `bench` and `bench-wasm32`.

## Lanes

Each lane is one workflow preset: `cmake --workflow --preset <name>`. Its build goes to `build/<name>`.

| Preset | What it proves | Inventory | Tests run |
|---|---|---|---|
| `dev` | A Debug build passes, for day-to-day work. No packaging. | `ci` | all but `release` |
| `ci` | The workflow above: the build, the tests and the release zips. Every Linux and macOS leg of CI runs it. | `ci` | all but `release`, then `release` |
| `ci-msvc` | The same five steps with MSVC and the static C runtime. | `ci` | all but `release`, then `release` |
| `asan` | No memory or undefined-behaviour fault, using clang-21 with AddressSanitizer and UndefinedBehaviorSanitizer. Any finding stops the test. Long runs drop from 600 frames to 60 (`MUHTARI_SHORT_RUNS`). | `ci` | all but `abi` and `release`, whose binary-shape checks the instrumentation changes |
| `nofp` | The core uses no floating point. It is built with GCC's `-mgeneral-regs-only`, which rejects any floating-point type. | `nofp` | `abi` |
| `hygiene` | The repository is clean: the tree scan, formatting, the integer-only text scan, the analyzer, the pins file, and `src/cartdb.c` equal to `tools/cartdb.cmake`'s output over the two pinned MAME lists. Fetch those first with `scripts/fetch-pin.sh mame-a7800-xml build/pins/mame` and `scripts/fetch-pin.sh mame-a2600-xml build/pins/mame`; configure stops with both commands when either is missing. | `hygiene` | `hygiene` |
| `fuzz` | The cartridge loader and the run loop keep their invariants on any input: libFuzzer runs `tests/fuzz/loader.c` and `tests/fuzz/run.c` for 60 seconds each with clang-21, AddressSanitizer and UndefinedBehaviorSanitizer, starting from the images `muhtari-test-images` writes. New inputs go to `build/fuzz/fuzz/corpus` and `build/fuzz/fuzz/run/corpus`, crashes to the `crash` directory beside each, never into the tree. Linux with clang-21 only; every other lane replays the same invariants as `fuzz.regress.loader` and `fuzz.regress.run` (`docs/emulation-tests.md`, fuzz). | `fuzz` | `fuzz` |
| `vectors-full` | SALLY matches all 2,560,000 SingleStepTests 6502 tests at the pinned commit. `cpu.vectors-full.fetch` checks `6502/v1` out sparse and blobless into `build/vectors-full/vectors-full` (`MUHTARI_VECTORS_DIR`; a path in the source tree outside `build/` is refused), verifies each file against `tests/vectors/6502-v1.sha256` and fetches nothing when all verify. `cpu.vectors-full.sample_match` proves the committed sample is the first 100 tests of each file; `cmake -DWRITE=ON ... -P tests/cmake/vectors_sample_match.cmake` rewrites it for a pin bump. A failed fetch leaves the other 257 not run. Needs the network, about 1 GiB of disk and 90 s for the fetch. | `vectors-full` | `vectors-full` |
| `be32-compile` / `be32-full` | Required PR PowerPC compilation; full nightly native/BE equality under qemu-ppc. | `ci` | compile only / all normal, package, release |
| `wasm32` | The same hashes in WebAssembly. It builds with clang-21 for `wasm32-wasi` (`cmake/toolchains/wasm32-wasi.cmake`) and runs under the pinned wasmtime. WASI cannot load a libretro core, so this lane builds none. | `wasm32` | `portable` |
| `library` | The compatibility ledger holds: each `pr` row is judged afresh on its file under `MUHTARI_ROM_DIR` or `tests/diag`, on both boot paths, and the ratchet compares (label `library` below). Run the `ci` preset's `built` tests first, then `MUHTARI_ROM_DIR=build/ci/built cmake --workflow --preset library`. | `library` | `library` |

Emulated test binaries and runner subprocesses use `CMAKE_CROSSCOMPILING_EMULATOR`
(scripts take `-DEMULATOR=`). wasm32 uses
`build/pins/wasmtime/wasmtime run --dir=<source>::<source>`; the preopen includes
the build tree at matching guest paths. Fetch with
`scripts/fetch-pin.sh wasmtime-<arch>-linux build/pins/wasmtime`; configure refuses
a missing runtime. WASI outputs have no `.wasm` suffix.

`MUHTARI_TIMEOUT_SCALE`: provisional `asan`/`be32`/`wasm32` scales 2/13/2; same-run Linux x64 duration-sum
ratios max 1.713/12.127/1.612 (n=4/4/2). ASan keeps short runs. `audio.samples`: eight 600-frame driver/runner
pairs, eight fixtures; 250 s = twice 124.92 s (11 Intel Mac successes, run 39617761785). COST 125 starts it early;
builds/basic/coverage use costs 7/10/28 (control 27), basic/coverage bounds 19/55 s (control 53). Movies: 3 s from
1.05 s maximum (Windows n=7, provisional). Fixtures retain prerequisites; page writers own separate outputs.
Publisher controls start early (COST 725).

### Comparing hashes across lanes

`runner.hashes_file` writes956 rows:69 retained observations,273 controller rows
(13 scenarios:12 frames,lag,raw/PCM,mailbox),614 [PAL rows](pal-tests.md)
(601 frames,raw/PCM,mailbox,counters).
Independent numeric/CRC judgment and library agreement precede controller rows. Silent raw streams remain empty.
The retained69 match accepted source54485d6 except seven audio revision annotations4→5.
Controls reject omitted, duplicated, reordered, malformed and wrong-family reports.

CI's `hash-equality` compares lane files:

```sh
cmake "-DFILES=<file>;<file>;..." -DCOUNT=<n> -DMODE=compare -P tests/cmake/hashes.cmake
```

Comparison requires exactly `COUNT` present, byte-identical files with the complete schema.

- **Required files.** PRs compare six native files and compile PowerPC. Nightly/release calls run full native/BE
  normal and package inventories, then require matching immutable SHA, fixtures and options for two-file hash
  equality. Architecture/toolchain and timeout scale may differ. Absolute or working-directory-relative evidence
  paths are canonicalized. Publication requires equality; calls stay read-only.
- **wasm32.** It reports only, compared with the Linux x64 file outside the required check. Once it is promoted
  into `hash-equality`, the count goes up by one.

`ctest --preset ci -R <regex>` reruns part of the suite after a build; `ctest --test-dir build/ci -L <label>`
selects by label. `docs/emulation-tests.md` says what each emulation label's tests cover.

## The inventory

`tests/inventory/ci.txt` lists every registered test with its labels, one `name<TAB>labels` line each, sorted,
labels comma-separated. Configure fails when the registered tests differ from the file in either direction, so a
test cannot be dropped, renamed or registered on one platform only without the file changing in the same commit.
Every leg registers the same tests; a check that only one platform can perform runs there as a CI step instead.
Six lanes register their own lists: `hygiene.txt`, `nofp.txt`, `fuzz.txt`, `vectors-full.txt`, `library.txt`, one
`library.<id>` per ledger row of tier `pr`, and `wasm32.txt`, which is the `ci` list without the `libretro` tests.

| Label | Tests |
|---|---|
| `core` | The library's behaviour: sizes, allocation, the frame and the test card hash. |
| `vectors` | SALLY against public tests. `cpu.vectors.<op>`, one per opcode, runs the committed sample, the first 100 SingleStepTests `6502/v1` tests of that opcode, and checks every bus cycle, the final registers and RAM, and the cycle where the instruction polls NMI. `cpu.decimal.clark` runs Bruce Clark's decimal-mode test to `ERROR` = 0. `cpu.unstable.sally_constant` pins the ANE and LXA constant. The rest test the vector tooling: the reader, the converter and the pin. |
| `vectors-full` | The `vectors-full` lane: the fetch, the sample match, and `cpu.vectors-full.<op>` over all 10,000 tests of each opcode's file. |
| `sally` | SALLY on the 7800's bus (`docs/hardware/bus.md`): 4- and 6-tick cycles, RDY holding reads only, the HALT gate and its two-cycle latency, and the NMI latch, with `cpu.nmi.*` for the instruction after which an edge is taken. |
| `riot` | The RIOT (`docs/hardware/riot.md`): its timer, PA7 edge, ports, RAM window and power-on state, with the bus's INPTCTRL lock and open-bus reads. |
| `maria` | MARIA's DMA burst with no CPU: the HALTRST tick, line RAM and statistics ([emulation tests, maria](emulation-tests.md#maria)). |
| `frame` | Whole frames through the run loop: SALLY, the bus and MARIA together ([emulation tests, frame](emulation-tests.md#frame)). |
| `input` | Controllers and console switches as a game reads them, driven by movie rows ([emulation tests, input](emulation-tests.md#input)). |
| `bios` | The boot through OpenBIOS and through a BIOS built in memory ([emulation tests, bios](emulation-tests.md#bios)). |
| `portable` | Tests that hold on every platform, including the frame and audio hashes. |
| `abi` | The public header compiled alone as C17 and C++17, the API test, and the core library's symbols. |
| `host` | The palette helper shared by the runner and the core. |
| `cart` | Cartridge identification: the core's SHA-1 against the FIPS 180-4 example messages, and `muhtari_probe` on images that `tests/cart/images.c` builds in memory, one test per status class and one for the warning bits, and the internal loader, which allocates nothing for a refused image. `cart.identify.test_list` finds the built 7800basic sample through the test hash list. No image bytes are tracked. |
| `fuzz` | Loader/run seeds and regressions; run adds typed devices, signed extrema, bounded pin traffic, EEPROM coexistence and output/allocation invariants. Linux also uses libFuzzer. |
| `prop` | 100 seeded cases each; nightly 10,000. Isolation also replays all nine typed families with nonzero payloads, pin traffic, refusals and pure peeks. |
| `runner` | `muhtari-run` options, output, exit statuses, the PNG writer and MD5, the movie reader (`runner.movie_parse`: good movies, and each bad one refused at its exact line and column), and `--probe` on the images `muhtari-test-images` writes into `build/<preset>/runner-probe`: the lines, their order, the digests against CMake's own, and each refusal's status and message. |
| `built` | Samples built from pinned tools (`tests/built/`). `built.<name>` builds one 7800basic 0.41 sample per row of `expected.txt` under wasmtime with our own recipe (`build.cmake`, each stage run with the package mapped to `/b`) in a fresh `build/<preset>/built/<name>/`, and checks the `.a78` and `.bin` SHA-256s in that row. `runner.probe.sample_*` then probe `simple`: the headered `.a78`, the stripped `.bin` found by its footer (reset vector $F4AD), and the `.bin` found by the test hash list through `muhtari-run-testdb`, the runner linked with `tests/cart/test_cartdb.c` in place of `src/cartdb.c`. The built files never leave the build tree. |
| `basic` | Each built sample's 600 fast-boot frames against the chain pinned in `tests/built/expected.txt` ([emulation tests, basic](emulation-tests.md#basic)). |
| `diag` | Our own diagnostic ROMs judged through their mailbox against values derived beforehand, and the core's statistics ([emulation tests, diag](emulation-tests.md#diag)). `diag.rebuild` carries `built`. |
| `title` | The ledger's title predicates on `simple`, on the fast boot and through OpenBIOS, and the pin writer's refusal of a blank screen ([emulation tests, title](emulation-tests.md#title)). |
| `library` | The `library` lane: each `pr` ledger row judged afresh, the ratchet and the generated page, run by CI's `library` job ([emulation tests, ledger](emulation-tests.md#ledger); [CI](#ci)). |
| `bench` | The runner's benchmark report. |
| `libretro` | The libretro core in an in-process host (content, controls, devices, Restart, BIOS files, options, the memory map), BIOS file/allocation/path faults (`libretro.bios_failures`), rcheevos' resolver over that map (`libretro.rcheevos`), its `.info` file and its vendored header. |
| `install` | The library installed and found by an outside project, and embedded with `add_subdirectory`. |
| `perf` | Allocation-free frames. |
| `policy` | Rules about the repository and the tests themselves. |
| `release` | The packaged zips, run after the package step. |

## Negative controls

A check that cannot fail proves nothing, so each repository check has a self test that feeds it a known fault.
`tests/cmake/expect_fail.cmake` runs a command and passes only when it exits with the expected code and its
output matches the expected message; a crash or a missing file does not count as the expected failure.

| Self test | Fault fed to the check |
|---|---|
| `abi.undefined.self_test` | A library that calls `abort`. |
| `cpu.vectors.self_test` | One `a9` vector whose final A is changed; the harness must exit 1 naming `a9.json[0]`. |
| `cpu.decimal.clark.self_test` | Clark's test built for the 65C02, whose decimal flags differ; SALLY must leave `ERROR` = 1. |
| `policy.rom_hash_coverage.self_test` | A copy of the manifest with the vector sample's SHA-256 zeroed. |
| `abi.globals.self_test` | A library with a writable file-scope integer. |
| `abi.api_version.self_test` | A copy of the header with one field no API test names. |
| `runner.hashes_compare.count.self_test` | Two `hashes.txt` files where three are required. |
| `runner.hashes_compare.byte.self_test` | Two `hashes.txt` files one byte apart. |
| `abi.nofp_fixture` | On the `nofp` lane, a file holding one `double` division, which must fail to compile with GCC's `-mgeneral-regs-only` diagnostic. |
| `built.samples.self_test` | An `expected.txt` whose `.a78` digest differs in one digit; the build must exit 1 with `simple: .a78 SHA-256 drift`. |
| `diag.rebuild.self_test` | A manifest copy whose `dma.a78` digest differs in one digit; the rebuild must exit 1 with `tests/diag/dma.a78: SHA-256 drift`. |
| `diag.expected.self_test` | An `expected.csv` copy with the `null` value changed; the driver must exit 1 failing that record. |
| `policy.pins.self_test` | A pins row whose digest does not match its file; `fetch-pin.sh` must exit 1 with `sha256 mismatch`. |
| `policy.cartdb_generated.self_test` | A copy of `src/cartdb.c` with one byte changed; the check must exit 1 with `src/cartdb.c differs from the generator output`. |
| `hygiene.firmware_scan.self_test` | The same synthetic file tracked in a fixture repository; the tracked-file scan must exit 1 with `matches synthetic (header stripped)`. |
| `hygiene.no_float.self_test` | Seven float lines beside three look-alikes, among them an SPDX line naming `CC0-1.0`, and an SPDX line followed by `double`. |
| `policy.publish_set.self_test` | 17 of the 18 release zips; `publish.sh prepare` must exit 1 naming the missing one. |
| `policy.no_skip.self_test` | A test file that sets a skip property. |
| `libretro.info_claims.self_test` | An `.info` file claiming a capability no test covers. |
| `release.archive_contents.self_test` | A libretro zip without `THIRD-PARTY-NOTICES.md`. |
| `release.firmware_scan.self_test` | A zip holding a synthetic 256-byte A78 file and a list naming the SHA-1 of its last 128 bytes; the scan must exit 1 with `matches synthetic (header stripped)`. |
| `release.floor.self_test` | An impossible floor: glibc 2.0, macOS 9.0, or no allowed Windows DLL. |
| `release.single_version_source.self_test` | A repository whose `notes.md` repeats its version. |

`policy.no_skip` fails when any test file sets a skip return code or disables a test, naming the file and line.

## Fixtures and the firmware scans

`tests/roms/manifest.txt` lists every binary file the repository may hold: its path, source, pin, licence, SHA-256
and notice section (ASSET_POLICY.md section 2). Today that is the vector sample, three vector fixtures, Clark's two
test builds and the `cpu`, `dli` and `dma` diagnostics. `policy.rom_hash_coverage` (hygiene lane) requires each
row's file to exist with that SHA-256, a row for every file under `tests/roms/` and in a fuzz `regress` directory,
and at most 512 KiB under `tests/fuzz/`.

The firmware scan (`tests/cmake/firmware_scan.cmake`) hashes files with SHA-1 and SHA-256, whole and, for an A78
image, again without its 128-byte header. It fails on a digest in `tests/release/firmware.txt` (BIOS and HSC
firmware), a built sample's digest in `tests/built/expected.txt`, a 7800 key in `src/cartdb.c`, or a 2600 prefix
there. `hygiene.firmware_scan` scans every tracked file; `release.firmware_scan` scans the members of the three
release zips; `publish.sh prepare` runs it over all 18 zips before a release.

## The core library's symbols

`tests/cmake/abi.cmake` reads the built `libmuhtari.a` with `nm -P`:

- `abi.undefined`: every symbol the library needs and does not define is `memcpy`, `memmove`, `memset`, `memcmp`
  or a compiler helper listed in the script for that system (`bzero` on macOS, the GOT symbol on x86-64 Linux,
  libgcc's 64-bit integer routines on 32-bit PowerPC);
- `abi.globals`: no writable data symbol, so all state lives in the instance;
- `abi.symbols`: the exported `muhtari_` names are exactly the functions `include/muhtari.h` declares, and every
  other exported name starts `muhtari__`.

`abi.api_version` (`tests/cmake/api_version.cmake`) keeps the header honest: `tests/api/v<N>.c` must exist for the
header's `MUHTARI_API_VERSION` N, and every function, type, constant and struct field the header names must appear
in some `tests/api/vK.c` with K at most N. `tests/api/v1.c` is frozen once released; a later API version adds its
own file.

## The JUnit check

A zero exit from CTest does not prove that each test ran. After each test step, CI runs:

```sh
cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release \
  -P tests/cmake/check_junit.cmake
cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release \
  -P tests/cmake/check_junit.cmake
```

Each fails unless every selected inventory test appears exactly once, ran, and did not fail, error or skip.

## CI

`.github/workflows/ci.yml` runs on every pull request, on each push to `main`, by hand, and when another workflow
calls it with an optional `ref`. Each job runs the same workflow preset you run locally, then the JUnit check above
with its lane's inventory and labels.

| Job | Runner | Runs | JUnit check |
|---|---|---|---|
| `changes` | `ubuntu-26.04` | `scripts/classify-changes.sh` on the pull request's paths, old and new names, and its changed-file count | none |
| `hygiene` | `ubuntu-26.04` | preset `hygiene` | `hygiene.txt` |
| `title` | `ubuntu-26.04` | pull requests only: `scripts/check-title.sh` on the live title and body | none |
| `build` ×6 | `ubuntu-26.04`, `ubuntu-26.04-arm`, `macos-15`, `macos-15-intel`, `windows-2025`, `windows-11-arm` | fetches its wasmtime and 7800basic, then preset `ci` (`ci-msvc` on Windows); uploads `archives-<os>-<arch>` and `hashes-<os>-<arch>`; `macos-15` also runs the README install line on its zips | `ci.txt` without `release`, then `release` |
| `asan` | `ubuntu-26.04` | fetches wasmtime and 7800basic, then preset `asan` | `ci.txt` without `abi` and `release` |
| `nofp` ×2 | `ubuntu-26.04`, `ubuntu-26.04-arm` | preset `nofp` | `nofp.txt` |
| `be32` | `ubuntu-26.04` | preset `be32-compile`; full execution/equality runs nightly and on releases | none |
| `hash-equality` | `ubuntu-26.04` | compares the six native `hashes.txt` files | none |
| `wasm32` | `ubuntu-26.04` | fetches wasmtime and 7800basic, then preset `wasm32`; compares its `hashes.txt` with Linux x64 | `wasm32.txt`, label `portable` |
| `retroarch` | `macos-15` | the built core in RetroArch; see "Hosted RetroArch" | none |
| `library` | `ubuntu-26.04` | full history; builds the `ci` preset's samples and OpenBIOS (`-L '^built$'`), then preset `library` over `build/ci/built` with the pull request's base for the ratchet | `library.txt` |
| `perf` | `ubuntu-26.04` | pull requests and dispatches only: Cachegrind instructions per frame of runners built with `MUHTARI_TESTS` OFF; see "Performance" | none |
| `quality` | `ubuntu-26.04` | [trusted bounds and exact producer evidence](quality.md) | validated by producers |
| `CI required` | `ubuntu-26.04` | the roll-up below | none |

Windows legs load the native MSVC environment with `vswhere` and `vcvarsall`.
Actions are pinned by full commit SHA.

**The roll-up.** The ruleset requires one check, `CI required`. It always runs, and passes only when every job it
needs succeeded, with three exceptions: the heavy jobs may be skipped when `changes` says `heavy=false`, which
happens only when every changed path is under `.planning/`; `title` and `perf` may be skipped outside pull
requests; and `perf` may be skipped in a nested call that passes `ref`. `wasm32`/`library` stay advisory: 9/5 distinct
successful main executions as of 2026-10-22; ten are required. Skips, PR/branch controls and duplicate reruns do not count.

**Tests of CI itself.** The hygiene lane checks the rules above. `policy.ci_required` reads the roll-up's `jq`
program from `ci.yml` and runs it, with the step's own arguments, on synthetic job results; it also fails when a
job other than `wasm32` and `library` is missing from `CI required`'s needs. `ci.classify` runs the classifier on mixed,
renamed, empty and planning-only path lists, and on a planning-only list from a pull request of more than 3000
files, which the files API cuts short. `hygiene.timeout_comments` requires each `timeout-minutes` to carry a
run/count comment. Bounds: twice the last 40 comparable successful job/runner/workload executions' maximum,
rounded up to minutes; network/roll-up floor 5 min. n<10 is provisional. Toolchain/image/workload changes reset
cohorts; direct/called CI stay separate. Timeouts are censored; skips and copied rerun timestamps add no samples.
Historical bounds without current evidence say n=0. No cache is added or check removed.

**The title.** The squash commit on `main` takes the pull request's title and body, and release-please reads its
type. `title` passes a Conventional Commit title with a lower-case subject, such as `fix(runner): exit 2 on a
missing file`. It fails a scope that starts with a digit (a plan number), a `Phase 1: Name` title, `feat`, `fix`
or `perf` scoped `ci`, `release`, `build`, `test` or `docs` (pipeline work cuts no release), and any CI skip
token in the title or body. Editing the title reruns it. Try one locally:
`echo | sh scripts/check-title.sh 'fix: x'`.

**Concurrency.** A new push to a pull request cancels that pull request's run in progress. Runs on `main` are never
cancelled, and a call from another workflow does not queue behind the push run on the same commit.

**Reading a failure.** Open the red job and the first red step:

- A preset step fails: the CTest summary at the end of the step names each failed test, with its output above it.
  Rerun that lane's preset locally (see "Running the Linux lanes locally").
- An "Every test ran" step fails: a test was missing, ran twice, or failed in the JUnit file. Compare
  `tests/inventory/<lane>.txt` with what the preset registers.
- `hash-equality` fails: the message names the first file that differs and prints both files. The artifact name
  names the lane.
- `CI required` fails with every other job green: a job was skipped where the roll-up does not allow it. Its log
  prints each job's result.

## Performance

**Instructions per frame.** The `perf` job in `ci.yml` counts the instructions the runner executes per frame of
the test card under Cachegrind: Ir/frame = (Ir at 360 frames − Ir at 60) / 300, in integers, with
`--cache-sim=no`, an empty environment and runner paths of equal length, so the count is the same on every run. It
measures the pull request's merge commit once and its first parent (the merge base) twice, then
`tests/cmake/perf_compare.cmake` prints one line to the log and the job summary:

```
perf testcard head=3837643 base=3837643 delta_bp=0 verdict=PASS
```

`delta_bp` is the change in basis points (hundredths of a percent). The verdicts:

| Verdict | When |
|---|---|
| `PASS` | the head is at most 1.00 % slower than the base (exactly +1.00 % passes) |
| `FAIL` | the head is more than 1.00 % slower, or more than the allow file permits |
| `UNSTABLE` | the two base measurements differ, so no comparison is trusted |
| `NO_BASE` | the merge base has no `CMakePresets.json` to build; only the head is measured |

`NO_BASE` depends on the base tree's contents; a base with presets that fails to build fails the job.
Empty, non-numeric and nonpositive measurements fail even unarmed. Release-called CI skips `perf` because it
checks out the PR head; `CI required` accepts that skip only when the call passes `ref`.

**Arming.** `MUHTARI_PERF_ARMED` is `"true"` after the 2026-10-22 controls and nightly issue recovery.
Arming requires two fresh dispatches of one immutable commit, each with three identical positive measurements,
a current numeric PR comparison with identical duplicate base measurements, and observed nightly failure/issue
recovery. Dispatches print `perf control testcard runs=<a>,<b>,<c> spread=<n>`. Once armed, `FAIL`, `UNSTABLE`
and `NO_BASE` fail the job. Coverage is test-card-only; DMA/TIA/POKEY lack their own fresh Ir controls and charged
integration fit. Native timings alone do not qualify them; no gameplay performance claim follows.

**Exceptions.** A pull request that must be slower adds a line to `tests/perf/allow.txt`:
`<rom> <max % × 100> <expiry YYYY-MM-DD> <reason>`, for example `testcard 250 2026-12-01 the new mixer`. It
raises that ROM's limit from the day it is checked through its expiry, at most 60 days away, and is removed after
the pull request merges. `ctest -L perf` runs the gate's own tests on synthetic values.

**Nightly bench.** `.github/workflows/nightly.yml` runs `muhtari-run --bench 300000` every night on `ubuntu-26.04`,
`ubuntu-26.04-arm` and `macos-15`, and under wasmtime 48.0.6 on `ubuntu-26.04` (row `wasm32-wasmtime-48.0.6`).
Each report has the runner's nine keys (`frames`, `fps_milli`, `p50_us` to `p99_999_us`, `max_us`), then
`max_rss_kb` from `/usr/bin/time` (not for wasm32, where it would measure wasmtime) and `runner`. Find them in the
run's job summaries and its `bench-<runner>` and `bench-wasm32` artifacts; they are trend data, never a gate and
never committed. A failed night opens or comments on the one open issue labelled `nightly`, and a good night closes
it; the report decides from the jobs' results. Dispatching the workflow with `force_failure` fails a run on purpose.
The same workflow runs the `vectors-full` lane (`cmake --workflow --preset vectors-full`) and the `fuzz` lane (job
`fuzz`, `cmake --workflow --preset fuzz`) on `ubuntu-26.04`, each followed by the JUnit check on its inventory;
`fuzz` uploads both targets' crash directories when it fails. Neither caches its vectors or corpus. The `prop` job
builds the `ci` preset with `MUHTARI_TIMEOUT_SCALE=10` and runs `ctest -L prop` with `MUHTARI_PROP_CASES=10000` and
the run's ID as `MUHTARI_PROP_SEED`; it stays advisory outside `report.needs`: 2/10 successful main runs as of
2026-10-22. A pull request runs the whole workflow, writing no issue, when it changes a file listed in the trigger's
`paths`: the workflow, the core and runner sources, the build files and pins, and each job's tests, scripts and
inventories.

**Bumping the vector pin.** One `ci:` pull request: set the new commit in the `sst-6502-v1` row and the sample's
manifest row; run the `vectors-full` workflow once, whose fetch fails against the old list but leaves the new files
in `build/vectors-full/vectors-full`; regenerate `tests/vectors/6502-v1.sha256` from them and put its SHA-256 in
the row; rewrite the sample with
`cmake -DWRITE=ON` on the `cpu.vectors-full.sample_match` command (`ctest --test-dir build/vectors-full -R
sample_match -V` prints it) and put the sample's new SHA-256 in the manifest. `cpu.vectors.pin` and
`policy.rom_hash_coverage` fail until all four agree.

## Hosted RetroArch

The `retroarch` job has two cases on `macos-15`: DMA equivalence and a battery roundtrip.
It uses macOS arm64 build archives and pinned RetroArch 1.22.2 Metal. Every launch gets fresh home,
configuration, system and output directories. `SAVE_DIR` is explicit, external to each disposable launch tree;
overlapping paths fail before cleanup. Each process has the existing 50-second bound.

DMA runs the committed diagnostic for 120 frames. Its runner hash retains the `diag/dma` ledger pin, and every
screenshot pixel must match. A flipped pixel control must exit 1 naming exactly pixel (7,3).

The battery case starts two fresh RetroArch and two fresh runner processes from empty, separate save directories.
Its owned diagnostic increments 16-bit HSC and EEPROM counters through SRAM stores and RIOT pins, waits for
programming, reads back and draws them. First launches use owned synthetic C6/FE firmware with marker 5A and
no routines; second launches use stub FF. Frontends share only save storage. Immutable snapshots must match
all 34,816 independently expected bytes, counters 1/1 then 2/2, and corresponding runner saves. The mailbox
judge checks runner hardware observations.

After `sips` converts PNG to BMP, independent glyphs judge both counters and the marker before whole-frame
equality. Equal-black, wrong-counter and stale-one-medium controls must fail. Ordinary `diag.battery` runs
the shared mailbox/media faults and pixel judge; loaded-module tests retain lifecycle/no-content coverage.
Four hosted support files stay within 300 lines. Only owned diagnostic shots enter `retroarch-frames`;
saves, firmware and logs stay uncommitted.

**Rerunning it.** Rerun the job from the run page, or `gh run rerun <run-id> --failed`. On a Mac with a desktop
session, the same steps run locally from the job's commands, with `-DRETROARCH` pointing at an installed
`RetroArch.app/Contents/MacOS/RetroArch`.

**If a launch fails.** `run_retroarch.cmake` prints its private RetroArch log. The agent diagnoses it and records
command, exit status and run URL; owner action is needed only for the owner's hardware. These diagnostics
establish the exercised persistence and optional-firmware behavior, not general retail-game compatibility.

## Release archives

`release.archive_contents` unpacks the three zips under `build/ci/release/extract` and requires:

- the names `muhtari-{library,runner,libretro}-<os>-<arch>.zip`, with no version and no other `muhtari-*.zip`;
- exactly the expected members at the root, with no top-level folder;
- `LICENSE` and `THIRD-PARTY-NOTICES.md` byte-identical to the source tree's;
- no file containing the absolute source or build directory.

The README's install line is checked on macOS arm64 against the locally built zip:

```sh
cmake -DREADME=README.md -DBASE_URL=file://$PWD/build/ci/ -P tests/cmake/install_line.cmake
```

It runs the line verbatim under a scratch `HOME`, with only the download base replaced, and requires
`cores/muhtari_libretro.dylib` and `info/muhtari_libretro.info` under
`~/Library/Application Support/RetroArch`. After publishing, the same script runs with the release URL as
`BASE_URL`.

## Release rehearsal

`.github/workflows/release.yml` runs on a pull request from this repository that changes `release.yml`,
`be-equality.yml`, `scripts/publish.sh`, `release-please-config.json` or `cmake/packaging.cmake`. Its tag is
`rehearsal-<run id>-<attempt>`, new for every attempt, because a deleted immutable release's tag can never be
reused.

| Job | Runs |
|---|---|
| `ci` | `ci.yml` on the pull request's head commit |
| `equality` | full native/PowerPC normal and package inventories plus authenticated same-commit hash equality |
| `rehearsal` | prepare exactly18 archives from the head commit with no firmware; upload and attest; verify with digest-pinned gh>=2.102.0 and full repository/signer/ref constraints, including changed-byte rejection; publish a non-latest prerelease; verify19 assets, downloaded hashes and attestations. Rehearsals use the PR merge ref; real releases require main. See [maintaining](maintaining.md#the-rehearsal) for the command sequence. |
| `install-line` | the README install line on `macos-15` against the rehearsal's download URL |
| `cleanup` | always, once `rehearsal` has started: `publish.sh rehearsal-delete TAG ID` confirms release absence before tag deletion, then exact-ID/tag and complete-list absence; API errors fail |

Recover a failed rehearsal with "Re-run all jobs" or a new push. For failed cleanup, use
the retained draft ID and [guarded cleanup](maintaining.md#re-running-a-failed-release). `publish.sh` deletes only
`rehearsal-*` tags. `docs/maintaining.md` describes the real release, which runs the same steps on `main`.

`policy.publish.controls` tests real-script lifecycle and identity checks, including
actual workflow output blocks. Hosted rehearsal supplies cryptographic, publication
and install evidence; fake transport supplies argument and short-circuit controls.

## Supported systems

The oldest systems the README lists are checked from the binaries, not run on these systems. `release.floor`
(`tests/cmake/floor.cmake`, floors in one table at its top) unpacks the zips under `build/ci/release/floor` and
reads what each OS loader enforces. On every system the libretro core exports only `retro_*` names.

| System | Floor | Read with |
|---|---|---|
| Linux | `muhtari_libretro.so` needs no `GLIBC_*` above 2.17, `muhtari-run` none above 2.34; each needs only `libc.so.6` and the `ld-linux` loader; every version need is a plain `GLIBC_<n>.<n>[.<n>]`; no `__isoc23_*` reference | `readelf -dW`, `readelf -VW`, `readelf --dyn-syms -W`, `nm -D --defined-only` |
| macOS | minimum OS at most 10.13 on x86_64 (`LC_VERSION_MIN_MACOSX version`) and 11.0 on arm64 (`LC_BUILD_VERSION minos`); the runner and the core link only `/usr/lib/libSystem.B.dylib` | `otool -l`, `otool -L`, `nm -gUj` |
| Windows | `muhtari-run.exe` and `muhtari_libretro.dll` import only `KERNEL32.dll`; `muhtari.lib` carries no `/DEFAULTLIB` directive | `llvm-readobj --coff-imports`, `llvm-readobj --coff-exports` |

## Pinned downloads

`pins.txt` lists every file a build or a CI job downloads, one `name version url sha256` row each.
`scripts/fetch-pin.sh NAME DEST` downloads the row `NAME` into a temporary directory and checks its SHA-256 before
anything else happens; only then does it unpack a `.tar.xz` or `.tar.gz` into `DEST`, top folder stripped, or
copy any other file there. A `.zip` (the Windows wasmtime) is read with `cmake -E tar`, since Git Bash's tar cannot
read one, and must hold exactly one top folder, which is stripped. `policy.pins` (hygiene lane) requires four fields per row, a unique name, an `https://`
URL and a 64-digit hex digest. A wrong digest exits 1 with `sha256 mismatch` and leaves `DEST` untouched. `PINS=<file>`
reads another pins file. A bump is one `ci:` pull request that takes the new digest from the release API.
The `sst-6502-v1` row is not a download: it pins the SingleStepTests commit the vector sample comes from, and its
digest is that of `tests/vectors/6502-v1.sha256`, the per-file list of the full set. `cpu.vectors.pin` requires that
list to name `6502/v1/00.json` to `ff.json` in order, its digest to match the row, and the row's commit to match the
sample's manifest pin.

## Running the Linux lanes locally

Every Linux lane runs on a Mac or any Docker host in an `ubuntu:26.04` container. The container's apt CMake is older
than the presets need, so the recipe fetches the pinned CMake first. An anonymous volume over `build` keeps the
container's build apart from the host's:

```sh
docker run --rm --platform linux/arm64 -v "$PWD:/src" -v /src/build -w /src ubuntu:26.04 sh -c '
  apt-get update -qq &&
  apt-get install -y -qq --no-install-recommends ca-certificates curl xz-utils git ninja-build gcc g++ binutils &&
  scripts/fetch-pin.sh cmake-linux-aarch64 /opt/cmake &&
  PATH=/opt/cmake/bin:$PATH cmake --workflow --preset ci'
```

Replace `ci` with another workflow to run that lane. Some lanes need more packages in the install list:

- `asan`: `clang-21 lld-21 libclang-rt-21-dev`. The last holds the sanitizer runtimes.
- `be32`: `gcc-powerpc-linux-gnu g++-powerpc-linux-gnu qemu-user`.
- `wasm32`: `clang-21 lld-21 wasi-libc libclang-rt-21-dev-wasm32`.

Every lane but `nofp`, `hygiene` and `fuzz` builds the sample, so also run
`scripts/fetch-pin.sh wasmtime-aarch64-linux build/pins/wasmtime`,
`scripts/fetch-pin.sh 7800basic-wasm build/pins/7800basic` and
`scripts/fetch-pin.sh 7800openbios-asm build/pins/openbios` before the workflow.

To keep a lane's `hashes.txt`, mount a directory outside the repository and copy the file into it, for example
`-v "$OUT:/out"` and then `cp build/be32/hashes.txt /out/`. On an x86_64 host use `--platform linux/amd64` and the
`cmake-linux-x86_64` row. Under Docker's x86_64 emulation on Apple Silicon, GNU tar cannot create nested folders
("Function not implemented"); add `libarchive-tools` to the install list and run
`ln -s /usr/bin/bsdtar /usr/local/bin/tar` before `fetch-pin.sh`.

## Movie replay

`runner.movie.owner` measures storage and cleanup; `runner.movie.v2_edges` checks child effects: `ctest --preset ci -R runner.movie -V`.

## One version source

`version.txt` is the only file a person or release-please edits to change the version. CMake generates the
library's version numbers, the core's `display_version` and `muhtari-run --version` from it, and release asset
names carry no version.

- `release.single_version_source` fails, naming the file, when the version in `version.txt` appears in any other
  tracked file, with no digit or dot on either side.
- `release.single_version_source.next` runs the same check for the next minor and next patch version, the
  versions a release pull request writes, so that pull request passes too.
- Out of scope: `version.txt`, `.release-please-manifest.json`, `release-please-config.json` (its initial version
  is release-please's input), `CHANGELOG.md`, and everything under `.planning/`.

To check one version by hand: `cmake -DVERSION=<x.y.z> -P tests/cmake/single_version.cmake`.

## Budgets

`docs.budgets` keeps the documents small enough to read: `README.md` at most 12 KB, `include/muhtari.h` at most
24 KB, and each `docs/*.md` at most 40 KB. Raise a budget only with a reason in the same commit.

`audio.stress` runs generated, legal dual-POKEY/TIA CPU programs with fast,
base, joined and high-pass choices. AUDCTL permits direct clocks only on
channels1/3; channels2/4 retain base or joined paths (hardware/pokey.md).
Actual WSYNC, RDY/HALT carry, accepted timer IRQs and JAM remain bounded.
The recurring check compares normal, discarded and unevenly drained output,
requires no allocation during frames, zero clips, ≤150 merged kernel additions
per sample and ≤139 samples after each full frame drain. A partial drain can
retain current-frame samples too. These are NTSC drain bounds; shared physical
storage is 1103 FIFO cells, with 962 work samples and at most 141 future PAL samples.
`core.pal.bounds` exhausts 591149 phases, 4000 alignments and 8000 signed-mass streams.
Observation registration allocates its bounded private buffer before execution.

`build/ci/tests/muhtari-test-audio-stress --bench 600` measures exact synthesis
on600 completed legal dual-chip/TIA frames and a matched no-POKEY/TIA baseline.
It prints target/compiler, sample/raw checks, elapsed nanoseconds and a milli
ratio. This is whole-core workload cost, including chip/interrupt and private
observation work. A desktop result is a proxy, not calibrated low-power
hardware. The >10% fallback trigger applies only to an applicable measured
low-power target. Production retains exact-q23-16x64-dc20 and never changes
quality with workload; a future proposal must preserve raw hashes and pass the
same independent spectral judge.
