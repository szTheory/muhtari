# Phase 13 complete-work and local-gate evidence

## Source and scope

Task 2's source/test/documentation commit was `d37f4ccfaa33d4cf2966a72a78bad32a46b9bf7c`; the final
source commit after two evidence-driven fixes is `13bb1af4e7b63fb727854d42ad4431091f2083cc` on
`phase/13-portable-save-load-across-three-interfaces`. The preceding Task 1 documentation commit is
`8dd36f4`. The final commit has complete local native, PowerPC/QEMU, release, equality, and
ASan/UBSan receipts. All scratch programs, state buffers, JUnit outputs and command artifacts stayed
under ignored `build/`; no state bytes or logs were committed. No ROM or BIOS fixture was added.

## Complete serialization work

`tests/core/test_state.c::test_topologies` loads the largest generated topology covered by the owned
fixtures: 32,768 bytes of cartridge RAM (C), 131,072 bytes of XM expansion RAM (E), and 34,816 bytes of
HSC plus SaveKey battery data (B). Including the 64-byte envelope, its public state is 209,628 bytes:

| Part | Bytes |
|---|---:|
| Header | 64 |
| Fixed payload | 10,908 |
| C + E + B | 198,656 |
| Total public state | 209,628 |

The independent compiled ceiling is 307,932 bytes (C=131,072, E=131,072, B=34,816 plus fixed payload
and header). The test reaches that value with an internal synthetic normalized cartridge and checks
size/write/validate only. It does not establish a generated cartridge, public load, or APPLY at that
ceiling.

The updated largest-generated case performs public save/load/apply, compares canonical bytes, and then
rejects a CRC-repaired state whose final fixed field (`savekey.ack`, public offset 10,971) is invalid.
The visitor keeps advancing through the appended C/E/B extents after recording that invalid field, so
the late rejection still reaches the complete 209,564-byte payload extent. The rejection returns before
APPLY. Custom allocator counters around both the valid round trip and late rejection show no allocation
or free after load.

Source-derived work for one complete operation:

| Operation | Complete work at the generated maximum |
|---|---|
| SIZE | One fixed visitor walk reserves the 209,564-byte payload extent; it reads no output buffer. |
| SAVE | One SIZE walk, one public live-state walk, one write walk over all payload fields, and CRC-32 over the 209,628-byte envelope stream. The write copies 198,656 raw C/E/B bytes. |
| Valid LOAD | One SIZE walk, one CRC-32 pass over 209,628 bytes, one public validation walk, then APPLY's defensive second validation walk and its apply walk. It compares the 34,816-byte battery span before applying and copies the 198,656 raw C/E/B bytes in place. |
| Late-reject LOAD | One SIZE walk, one full-envelope CRC-32 pass, and one public validation walk. It rejects `savekey.ack` after earlier fixed fields and still advances through the remaining topology extent; APPLY does not run. |

Validation checks fixed fields, canonicalized arrays and cross-field relationships. Raw RAM and battery
spans are unconstrained data, so validation advances across their installed extents without byte-scanning
them; APPLY copies those spans. CRC-32 processes the full envelope length, treating its four-byte CRC
slot as zero. The existing tests also load the maximum 1 MiB installed ROM and confirm its effective
SHA-256 identity stays unchanged after caller-buffer mutation and repeated state operations. That ROM is
hashed once at successful load; later state operations reuse the 32-byte instance cache.

## Local measurements and checks

The existing core state executable was built against the already-built `build/ci/libmuhtari.a`; no test
framework or package was added, and no pinned asset was used:

```sh
cc -std=c17 -O3 -DNDEBUG -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wstrict-prototypes -Wmissing-prototypes -Wvla -Wundef -Wcast-qual -Wimplicit-fallthrough -Wmissing-variable-declarations -Werror -Iinclude -Isrc -Irunner -Ibuild/ci/generated tests/core/test_state.c tests/cart/images.c runner/sha256.c build/ci/libmuhtari.a -o build/state08-policy/muhtari-test-state
```

Exit 0. Running the existing `aliases`, `battery`, `fields`, `lifecycle`, `payload`, `topologies`, and
`equality_report` cases from that executable also exited 0. The report emitted all 78 expected state
rows. A separate final run of `topologies`, timed after the optimized compile, took 0.30 s real / 0.08 s
user / 0.00 s system. This is one local sample, not a new timeout calibration. The existing
`core.state.topologies` registration remains at 10 seconds.

Compiler `-fstack-usage` output for `src/state.c`, compiled with the CI Release compiler and
`-O3 -DNDEBUG`, reports 416-byte `state_walk`, 320-byte `state_visit`, 112-byte `state_pokey`,
416-byte `muhtari__state_payload_validate`, 448-byte `muhtari__state_payload_apply`, and 432-byte
`muhtari_load_state` frames. The deepest state-load chain is 2,144 bytes across those frames; this
excludes the external caller's frame. A small compile-time layout probe reports `sizeof(muhtari)` as
89,144 bytes; removing only the 32-byte cached identity field from the same current header yields
89,112 bytes, an exact 32-byte instance increase.

These direct consumers passed after the final documentation edits:

| Command | Exit | Result |
|---|---:|---|
| `cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake` | 0 | 22 documentation files within budget; `docs/testing.md` is 40,710 bytes against a 40,960-byte limit. |
| `cmake -DHEADER="$PWD/include/muhtari.h" -DDOC="$PWD/docs/embedding.md" -P tests/cmake/api_names.cmake` | 0 | All 154 public names are documented; none extra. |
| `cmake -DHEADER="$PWD/include/muhtari.h" -DAPI_DIR="$PWD/tests/api" -P tests/cmake/api_version.cmake` | 0 | 228 header names are covered through API version 8. |
| `cmake -DROOT="$PWD" -P tests/cmake/vendored.cmake` | 0 | The vendored file matches `PROVENANCE.md` section 8. |
| `git diff --check` | 0 | No whitespace errors. |

The existing `tests/cmake/expect_fail.cmake` consumer was also exercised with the existing API-name and
API-version fault fixtures. Each inner policy command exited 1 with the required diagnostic, and each
outer expected-failure command exited 0. This exercises the two existing fault controls without relying
on CTest registration. No repository check or test was skipped in its source; the unavailable CTest
inventory is recorded separately below.

## Earlier tests-off checkpoint (superseded)

The following results explain why the original Task 3 checkpoint paused on November 1. The owner later
authorized the local Linux setup described below; its successful runs supersede this checkpoint.

The cached `build/ci/CMakeCache.txt` has `MUHTARI_TESTS=OFF`. `build/pins/wasmtime`,
`build/pins/7800basic`, `build/pins/openbios`, and `build/pins/rcheevos` are absent. Tests-on configure
would stop at the existing missing-pin checks; no pin fetch was run.

| Command | Exit | Result |
|---|---:|---|
| Task 1 exact selector: `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(docs\.(budgets|api_names(\.self_test)?)|abi\.api_version(\.self_test)?|libretro\.vendored)$' --no-tests=error --output-on-failure --output-junit build/ci/junit-state-08-1.xml` | 8 | Configure/build passed; CTest found no registered tests because tests are disabled in the cache. |
| `cmake --workflow --preset ci` | 8 | Configure and product build passed; workflow stopped at test step 3 with `No tests were found`. Packaging and release steps did not run. |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 1 | All 689 selected normal-inventory rows were absent from JUnit. |
| `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | 1 | Release JUnit was not produced because the workflow stopped before packaging/release tests. |
| Exact asan chain beginning `command -v clang-21 && command -v clang++-21 && cmake --preset asan ...` | 1 | Stopped at the first required compiler lookup; no asan configure, build, tests or inventory validation ran. |

At this checkpoint, the required Linux tools and pins were unavailable in the tests-off host build.
The separately authorized container run below resolved that environment limitation.

## Architecture review

| Review area | Result |
|---|---|
| Ownership | Core state is instance-owned; the SHA-256 machine identity is cached on that instance. Battery generation remains a host equality token. Runner file handling and frontend state storage remain outside the core. |
| Interfaces and coupling | Runner and libretro use the same public state API and visitor; there is no second serializer or format-specific adapter. C/E/B lengths derive from the installed topology. No new endpoint, thread guarantee, host allowance or durable-file behavior was added. |
| Control flow | The core bounds sizes from its installed layout, checks envelope/version/CRC/identity, validates before apply, then copies into the existing instance. The repaired-CRC late-field case covers refusal after substantial payload traversal. No absolute emulated-time cap was introduced. |
| Independent checks | Core state cases and 78-row report pass locally; runner, libretro, properties, fuzz replay, and full normal/release inventories passed in the final native and PowerPC/QEMU workflows. |
| Recurring cost | Machine identity hashes the installed ROM once. Each save writes the payload and CRC; each valid load performs two validation walks and one in-place apply. The maximum generated case completes locally in 0.30 seconds with no post-load allocation/free. No timeout was changed from this single sample. |

## Authorized local Linux container execution — completed

**Exact source:** `13bb1af4e7b63fb727854d42ad4431091f2083cc` (final branch HEAD).

The owner authorized a local, bounded Docker setup. Docker Desktop's existing `desktop-linux` engine
provided an ARM64 Linux VM. The already-cached `ubuntu:26.04` image was used; its repository digest is
`ubuntu@sha256:f144425ff09be612d6d9ad965196e9cdc23dae1f42110a8a11a3e9a8198759f7` and image config ID is
`sha256:a1b016b8418dc08803fa0fb3aea39d5029a8c6a05a861adf9905a20d7307bdb4`.

A named, non-privileged container with no host checkout mount was provisioned. It installed the
documented Ubuntu build tools plus `gcc-powerpc-linux-gnu`, `g++-powerpc-linux-gnu`, `qemu-user`,
`clang-21`, `lld-21`, `libclang-rt-21-dev`, and `jq`. The repository's pinned ARM64 CMake 4.4.3 and
the `wasmtime`, `7800basic-wasm`, `7800openbios-asm`, and `rcheevos` pins were fetched through
`scripts/fetch-pin.sh` and checked against the repository pin manifest. The exact source bundle was
checked out at the SHA above. No project dependency was added.

| Command | Exit | Result |
|---|---:|---|
| `cmake --workflow --preset ci` | 0 | 689 normal CI tests passed; packaging completed; 6 release tests passed. |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | All 689 normal inventory entries ran once. |
| `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | 0 | All 6 release inventory entries ran once. |
| `cmake --workflow --preset be32-full` | 0 | 689 normal tests and 6 release tests passed using the PowerPC cross compiler and `qemu-ppc`. |
| `cmake -DJUNIT=build/be32-full/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | All 689 PowerPC/QEMU normal inventory entries ran once. |
| `cmake -DJUNIT=build/be32-full/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | 0 | All 6 PowerPC/QEMU release inventory entries ran once. |
| `cmake -DNATIVE=evidence/native -DBE=evidence/be -DEXPECTED_SHA=13bb1af4e7b63fb727854d42ad4431091f2083cc -P tests/cmake/lane_equality.cmake` | 0 | Native and PowerPC/QEMU identity receipts match; both canonical hash files match (2 files). |
| `command -v clang-21 && command -v clang++-21 && cmake --preset asan && cmake --build --preset asan && ctest --preset asan --no-tests=error --output-on-failure && cmake -DJUNIT=build/asan/junit.xml -DINVENTORY=tests/inventory/ci.txt "-DEXCLUDE=abi|release" -P tests/cmake/check_junit.cmake` | 0 | 670 ASan/UBSan tests passed with no findings; the full selected inventory ran once. |

The phase regression gate is covered by these complete normal and release inventories on final HEAD,
which ran all registered prior-phase tests as well as Phase 13 tests. No narrower rerun was needed.
The host evidence bundle was copied to ignored `build/phase13-local/`: native and BE `commit.txt`,
`identity.txt`, and `hashes.txt`, plus five JUnit reports. No logs or raw state bytes were copied.
All commands were local (`N/A-local`); no hosted URL is claimed. This proves Linux execution using
QEMU user-mode emulation, not physical PowerPC hardware behavior. No PR, workflow dispatch, RetroArch
run, global Docker prune, or Phase 12 allowance renewal occurred. The named container was retained.

**Result:** The exact-source native and PowerPC/QEMU producer steps, complete inventories, all three
evidence members per lane, and `lane_equality.cmake` passed against final HEAD. `STATE-02` remains
unclassified where its separately listed design assumptions lack direct evidence; the unresolved
prohibitions remain explicitly listed below for the phase verifier to assess against actual code/tests.

## Unresolved design assumptions and prohibitions

These remain unverified judgments; the local tests and documentation do not resolve them:

- State delivery must not commit immutable/proprietary media or personal durable save/state bytes as fixtures.
- State rollback must not assume that durable files, frontend recovery, or Playstead history share ownership with live machine state.
- Callback, compile-only, or advisory evidence must not be promoted to actual-host, required-lane, or publication acceptance, and does not renew any Phase 12 allowance.
- `STATE-02` remains unclassified; no absolute emulated-time ceiling or additional host guarantee is inferred from the producer-derived clock checks.
