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

## Presets

| Workflow | Use |
|---|---|
| `dev` | Debug build in `build/dev`; configure, build and test, without packaging or the `release` tests. |
| `ci` | The workflow above. Every Linux and macOS leg of CI runs it. |
| `ci-msvc` | The same five steps with MSVC and the static C runtime, in `build/ci-msvc`. |

`ctest --preset ci -R <regex>` reruns part of the suite after a build; `ctest --test-dir build/ci -L <label>`
selects by label.

## The inventory

`tests/inventory/ci.txt` lists every registered test with its labels, one `name<TAB>labels` line each, sorted,
labels comma-separated. Configure fails when the registered tests differ from the file in either direction, so a
test cannot be dropped, renamed or registered on one platform only without the file changing in the same commit.
Every leg registers the same tests; a check that only one platform can perform runs there as a CI step instead.

| Label | Tests |
|---|---|
| `core` | The library's behaviour: sizes, allocation, the frame and the test card hash. |
| `portable` | Tests that hold on every platform, including the frame and audio hashes. |
| `abi` | The public header compiled alone as C17 and C++17, the API test, and the core library's symbols. |
| `host` | The palette helper shared by the runner and the core. |
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
| `policy.no_skip.self_test` | A test file that sets a skip property. |
| `libretro.info_claims.self_test` | An `.info` file claiming a capability no test covers. |
| `release.archive_contents.self_test` | A libretro zip without `THIRD-PARTY-NOTICES.md`. |
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
