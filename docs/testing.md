# Testing

Every check that CI runs, a contributor can run with one command. This page covers that command, the presets,
the test inventory, the negative controls and the checks on releases.

## The entry point

```sh
cmake --workflow --preset ci
```

It needs CMake 4.3 or later, Ninja and a C and C++ compiler. The workflow runs five steps:

1. configure into `build/ci` (Release, warnings as errors, macOS deployment target 10.13);
2. build;
3. test every test except those labelled `release`, writing `build/ci/junit.xml`;
4. package the three release zips into `build/ci`;
5. test the `release` label against those zips, writing `build/ci/junit-release.xml`.

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
| `fuzz` | The cartridge loader keeps its invariants on any input: libFuzzer runs `tests/fuzz/loader.c` for 60 seconds with clang-21, AddressSanitizer and UndefinedBehaviorSanitizer, starting from the images `muhtari-test-images` writes. New inputs go to `build/fuzz/fuzz/corpus` and crashes to `build/fuzz/fuzz/crash`, never into the tree. Linux with clang-21 only; every other lane replays the same invariants as `fuzz.regress.loader`. | `fuzz` | `fuzz` |
| `be32` | The same hashes on a big-endian, 32-bit target with unsigned `char`. It cross-compiles for 32-bit PowerPC (`cmake/toolchains/ppc-linux-gnu.cmake`) and runs under `qemu-ppc`. | `ci` | `portable` |
| `wasm32` | The same hashes in WebAssembly. It builds with clang-21 for `wasm32-wasi` (`cmake/toolchains/wasm32-wasi.cmake`) and runs under the pinned wasmtime. WASI cannot load a libretro core, so this lane builds none. | `wasm32` | `portable` |

On the two emulated lanes, CMake runs each test binary through `CMAKE_CROSSCOMPILING_EMULATOR`. Every script that
starts `muhtari-run` takes the same emulator as `-DEMULATOR=`, so a test never runs a host binary in place of the
guest one. The wasm32 emulator is `build/pins/wasmtime/wasmtime run --dir=<source>::<source>`. The build tree sits
under the source tree, so this one preopen covers both, at the same paths inside the guest. Fetch the runtime first
with `scripts/fetch-pin.sh wasmtime-<arch>-linux build/pins/wasmtime`; configure stops with that command when the
runtime is missing. WASI output files have no `.wasm` suffix.

`MUHTARI_TIMEOUT_SCALE` multiplies every test timeout. The scales are 3 for `asan`, 11 for `be32` and 3 for
`wasm32`. The emulator scales are measured: over ten hosted CI runs, the tests a lane shares with Linux x64 took
10.7 times as long in total under `qemu-ppc` and 2.5 times as long under `wasmtime`, rounded up. In an arm64
container on an Apple Silicon Mac, the `be32` test step took 1.1 s and the `wasm32` step 0.3 s.

### Comparing hashes across lanes

`runner.hashes_file` (`tests/cmake/hashes.cmake`, mode `write`) writes `build/<preset>/hashes.txt`. It has three
lines: the frame 1 and frame 600 hashes and the SHA-256 of the frame 1 PNG.

```text
frame1 <hex>
frame600 <hex>
png <hex>
```

CI collects the file from each lane, and the `hash-equality` job compares them:

```sh
cmake "-DFILES=<file>;<file>;..." -DCOUNT=<n> -DMODE=compare -P tests/cmake/hashes.cmake
```

The comparison passes only when it gets exactly `COUNT` files, all present and byte-identical. Otherwise it names
the first file that differs. The fixed count means a lane that stops uploading fails the job, instead of silently
dropping out.

- **Required files.** Seven: the six `build` legs and `be32`. If `be32` moves to nightly because its job takes more
  than five minutes, the per-PR count is six and the full `be32` comparison runs nightly.
- **wasm32.** It reports only, compared with the Linux x64 file outside the required check. Once it is promoted
  into `hash-equality`, the count goes up by one.

`ctest --preset ci -R <regex>` reruns part of the suite after a build; `ctest --test-dir build/ci -L <label>`
selects by label.

## The inventory

`tests/inventory/ci.txt` lists every registered test with its labels, one `name<TAB>labels` line each, sorted,
labels comma-separated. Configure fails when the registered tests differ from the file in either direction, so a
test cannot be dropped, renamed or registered on one platform only without the file changing in the same commit.
Every leg registers the same tests; a check that only one platform can perform runs there as a CI step instead.
Four lanes register their own lists: `hygiene.txt`, `nofp.txt`, `fuzz.txt`, and `wasm32.txt`, which is the
`ci` list without the `libretro` tests.

| Label | Tests |
|---|---|
| `core` | The library's behaviour: sizes, allocation, the frame and the test card hash. |
| `portable` | Tests that hold on every platform, including the frame and audio hashes. |
| `abi` | The public header compiled alone as C17 and C++17, the API test, and the core library's symbols. |
| `host` | The palette helper shared by the runner and the core. |
| `cart` | Cartridge identification: the core's SHA-1 against the FIPS 180-4 example messages, and `muhtari_probe` on images that `tests/cart/images.c` builds in memory, one test per status class and one for the warning bits, and the internal loader, which allocates nothing for a refused image. No image bytes are tracked. |
| `fuzz` | The loader fuzz target's invariants replayed over its seeds and committed regressions, and on the `fuzz` lane libFuzzer itself. |
| `runner` | `muhtari-run` options, output, exit statuses and the PNG writer. |
| `bench` | The runner's benchmark report. |
| `libretro` | The libretro core in an in-process host, its `.info` file and its vendored header. |
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
| `abi.globals.self_test` | A library with a writable file-scope integer. |
| `abi.api_version.self_test` | A copy of the header with one field no API test names. |
| `runner.hashes_compare.count.self_test` | Two `hashes.txt` files where three are required. |
| `runner.hashes_compare.byte.self_test` | Two `hashes.txt` files one byte apart. |
| `abi.nofp_fixture` | On the `nofp` lane, a file holding one `double` division, which must fail to compile with GCC's `-mgeneral-regs-only` diagnostic. |
| `policy.pins.self_test` | A pins row whose digest does not match its file; `fetch-pin.sh` must exit 1 with `sha256 mismatch`. |
| `policy.cartdb_generated.self_test` | A copy of `src/cartdb.c` with one byte changed; the check must exit 1 with `src/cartdb.c differs from the generator output`. |
| `hygiene.no_float.self_test` | Seven float lines beside three look-alikes, among them an SPDX line naming `CC0-1.0`, and an SPDX line followed by `double`. |
| `policy.publish_set.self_test` | 17 of the 18 release zips; `publish.sh prepare` must exit 1 naming the missing one. |
| `policy.no_skip.self_test` | A test file that sets a skip property. |
| `libretro.info_claims.self_test` | An `.info` file claiming a capability no test covers. |
| `release.archive_contents.self_test` | A libretro zip without `THIRD-PARTY-NOTICES.md`. |
| `release.floor.self_test` | An impossible floor: glibc 2.0, macOS 9.0, or no allowed Windows DLL. |
| `release.single_version_source.self_test` | A repository whose `notes.md` repeats its version. |

`policy.no_skip` fails when any test file sets a skip return code or disables a test, naming the file and line.

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
| `build` ×6 | `ubuntu-26.04`, `ubuntu-26.04-arm`, `macos-15`, `macos-15-intel`, `windows-2025`, `windows-11-arm` | preset `ci` (`ci-msvc` on Windows); uploads `archives-<os>-<arch>` and `hashes-<os>-<arch>`; `macos-15` also runs the README install line on its zips | `ci.txt` without `release`, then `release` |
| `asan` | `ubuntu-26.04` | preset `asan` | `ci.txt` without `abi` and `release` |
| `nofp` ×2 | `ubuntu-26.04`, `ubuntu-26.04-arm` | preset `nofp` | `nofp.txt` |
| `be32` | `ubuntu-26.04` | preset `be32`; uploads `hashes-be32` | `ci.txt`, label `portable` |
| `hash-equality` | `ubuntu-26.04` | compares exactly seven `hashes.txt` files: the six `build` legs and `be32` | none |
| `wasm32` | `ubuntu-26.04` | preset `wasm32`; compares its `hashes.txt` with Linux x64 | `wasm32.txt`, label `portable` |
| `retroarch` | `macos-15` | the built core in RetroArch; see "Hosted RetroArch" | none |
| `perf` | `ubuntu-26.04` | pull requests and dispatches only: Cachegrind instructions per frame; see "Performance" | none |
| `CI required` | `ubuntu-26.04` | the roll-up below | none |

The hosted `be32` job takes about 25 seconds, well inside the five-minute limit for a per-pull-request lane, so
`hash-equality` compares seven files. Windows legs load the MSVC environment with `vswhere` and `vcvarsall` for the runner's own architecture. Every
action is pinned by full commit SHA.

**The roll-up.** The ruleset requires one check, `CI required`. It always runs, and passes only when every job it
needs succeeded, with three exceptions: the heavy jobs may be skipped when `changes` says `heavy=false`, which
happens only when every changed path is under `.planning/`; `title` and `perf` may be skipped outside pull
requests; and `perf` may be skipped in a nested call that passes `ref`. `wasm32` reports only and is not among the
jobs `CI required` needs.

**Tests of CI itself.** The hygiene lane checks the rules above. `policy.ci_required` reads the roll-up's `jq`
program from `ci.yml` and runs it, with the step's own arguments, on synthetic job results; it also fails when a
job other than `wasm32` is missing from `CI required`'s needs. `ci.classify` runs the classifier on mixed,
renamed, empty and planning-only path lists, and on a planning-only list from a pull request of more than 3000
files, which the files API cuts short. `hygiene.timeout_comments` requires each `timeout-minutes` to carry a
comment with a run ID or the word `provisional`. A measured timeout is twice the job's slowest successful run,
rounded up to minutes, with a 5-minute floor for network-bound and roll-up jobs; its comment names that run and
the sample count. A job with fewer than ten successful runs stays `provisional` and gives its sample count.

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

`NO_BASE` appears on direct pull request runs until the first pull request merges, because `main` held only
planning files before then; it is decided from the base tree's contents, and a base that has presets but fails to
build fails the job. An empty or non-numeric measurement is an input error (`perf: error ...`, exit 1), never a
pass. `perf` runs only in direct runs of `ci.yml`: in `release.yml`'s nested call, which checks out the pull
request head rather than the merge commit, it is skipped, and `CI required` accepts that skip only in a call that
passes `ref`.

**Arming.** Until `MUHTARI_PERF_ARMED` in `ci.yml` is `"true"` every verdict reports only and the job succeeds.
Once armed, `FAIL`, `UNSTABLE` and `NO_BASE` fail it. It is armed by one `ci:` pull request after two dispatched
control runs on `main` show zero spread: on `workflow_dispatch` the job measures one commit three times and prints
`perf control testcard runs=<a>,<b>,<c> spread=<n>`.

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
A pull request that changes the bench or the workflow runs the bench jobs but writes no issue.

## Hosted RetroArch

The `retroarch` job proves that a real frontend shows the frame the runner hashes. It is a CI step, not a CTest,
so it never skips itself off macOS. On `macos-15` it:

1. unzips the core and the runner from the `archives-macos-arm64` artifact of the macOS arm64 `build` leg;
2. fetches RetroArch 1.22.2 Metal with `scripts/fetch-pin.sh retroarch-dmg` (the `RetroArch_Metal.dmg` row in
   `pins.txt`, checked by SHA-256), copies the app out of the image and checks its bundle version;
3. runs `tests/retroarch/run_retroarch.cmake`: RetroArch gets its own `HOME`, `CFFIXED_USER_HOME` and the
   configuration from `tests/retroarch/test.cfg.in`, and starts with `--verbose -L <core>`, no content,
   `--max-frames=5 --max-frames-ss`. The launch is bounded at 50 seconds; a non-zero exit or a missing
   screenshot fails with RetroArch's log;
4. writes the runner's frame with `muhtari-run --frames 5 --dump-frame 5:runner.png`, decodes both PNGs with
   `sips --deleteColorManagementProperties` and `sips -s format bmp`, and builds `tests/retroarch/compare.c` and
   `bmp.c`. The compare must print `320x242, every pixel equal`;
5. runs a negative control: it inverts one byte of pixel (7,3) in a copy of the runner's BMP, and the compare
   must exit 1 and name that pixel first.

`video_gpu_screenshot = "false"` makes RetroArch save the core's own frame at the size the core reports, with no
scaling or on-screen text. The card is static, so five frames only need the core loaded.

**Rerunning it.** Rerun the job from the run page, or `gh run rerun <run-id> --failed`. On a Mac with a desktop
session, the same steps run locally from the job's commands, with `-DRETROARCH` pointing at an installed
`RetroArch.app/Contents/MacOS/RetroArch`.

**If RetroArch will not start the core without content.** The job relies on RetroArch starting a core that
reports no-game support when `-L` is given and no content. If it stops doing so, the log printed by
`run_retroarch.cmake` shows it. Treat that as a blocker for the owner, with the log and the run URL. Every fix,
such as letting the core accept a content file, changes a settled decision.

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
`scripts/publish.sh`, `release-please-config.json` or `cmake/packaging.cmake`. Its tag is
`rehearsal-<run id>-<attempt>`, new for every attempt, because a deleted immutable release's tag can never be
reused.

| Job | Runs |
|---|---|
| `ci` | `ci.yml` on the pull request's head commit |
| `rehearsal` | `scripts/publish.sh prepare`: exactly the 18 zips, each build leg's `commit.txt` equal to the head commit, `SHA256SUMS` in C-locale name order (`policy.publish_set` checks this on placeholders); a draft prerelease at that commit; upload; `actions/attest` over `SHA256SUMS`; `publish.sh attested`: each zip's attestation verifies from the pull request's `refs/pull/<number>/merge` and a copy with its last byte flipped does not; publish with `--prerelease --latest=false`; `publish.sh verify`: `attested` again, `gh release verify`, 19 non-empty assets, and every zip downloaded from the release URL matches `SHA256SUMS` and verifies as signed by `release.yml` from that ref. A release tag verifies only from `refs/heads/main` (`policy.publish_ref`) |
| `install-line` | the README install line on `macos-15` against the rehearsal's download URL |
| `cleanup` | always, once `rehearsal` has started: `publish.sh rehearsal-delete` removes the release and its tag, and fails if either remains |

A failed rehearsal recovers with "Re-run all jobs" or a new push. If `cleanup` itself failed, delete the leftover
by hand: `gh release delete rehearsal-<id>-<attempt> --cleanup-tag --yes`. `publish.sh` deletes only
`rehearsal-*` tags. `docs/maintaining.md` describes the real release, which runs the same steps on `main`.

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
copy any other file there. `policy.pins` (hygiene lane) requires four fields per row, a unique name, an `https://`
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
- `wasm32`: `clang-21 lld-21 wasi-libc libclang-rt-21-dev-wasm32`. Also run
  `scripts/fetch-pin.sh wasmtime-aarch64-linux build/pins/wasmtime` before the workflow.

To keep a lane's `hashes.txt`, mount a directory outside the repository and copy the file into it, for example
`-v "$OUT:/out"` and then `cp build/be32/hashes.txt /out/`. On an x86_64 host use `--platform linux/amd64` and the
`cmake-linux-x86_64` row. Under Docker's x86_64 emulation on Apple Silicon, GNU tar cannot create nested folders
("Function not implemented"); add `libarchive-tools` to the install list and run
`ln -s /usr/bin/bsdtar /usr/local/bin/tar` before `fetch-pin.sh`.

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
