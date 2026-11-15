# Phase 23 — post-merge CI timeout correction

Updated 2026-11-15. This follow-up records new CI evidence that arrived after
the original bounded no-change result and the v8.0 phase PR merged. It changes
one test deadline only; emulator behavior and feature scope are unchanged.

## Failure and downstream jobs

Main CI run [43748043795](https://github.com/szTheory/muhtari/actions/runs/43748043795)
tested merge commit `6b6c7874fd0dd1791159fad32cb65143d2a7c4ef`, attempt 1. The
`build (macos-x64)` job ran the unchanged `title.pal.bios` predicate against
`tests/diag/pal.a78` and public OpenBIOS. It timed out at 8.06 seconds; the
other 722 tests in that suite passed. The registered timeout was 4 seconds,
multiplied by the macOS x64 lane's scale of 2.

That failed build did not upload `quality-macos-x64`. The `quality` job then
found seven quality artifacts where `quality_join.cmake` requires eight and
failed the exact-set check. `CI required` correctly failed because both the
build matrix and quality job were red. These are downstream effects of the
same missing macOS x64 result, not separate test failures. The other native
build jobs and hygiene passed.

The complete run response was saved to `build/ci-run-43748043795.json` and
audited with:

```text
gh run view 43748043795 --json attempt,status,conclusion,headSha,url,jobs
cmake -DINPUT=build/ci-run-43748043795.json -DEXPECTED_SHA=6b6c7874fd0dd1791159fad32cb65143d2a7c4ef -P tools/check_ci_run.cmake
```

The receipt query exited 0. The CMake audit exited 1 as expected: it confirmed
attempt 1, the exact source, all failed/skipped jobs, and that `CI required`
was not successful. The failure remains preserved; no hosted retry was used.

## Comparable macOS x64 samples

Before this correction, the test registration last changed in commit
[`12953bcf615d6e13575f41644bad81f0bb627743`](https://github.com/szTheory/muhtari/commit/12953bcf615d6e13575f41644bad81f0bb627743)
on 2026-11-11. Six successful main CI runs after that change each completed
both the current and independent-base `ci` suites, giving 12 completed samples:

| Run | Head SHA | Current (s) | Independent base (s) |
|---|---|---:|---:|
| [43341494350](https://github.com/szTheory/muhtari/actions/runs/43341494350) | `b3649cdd0a939708ac9ce420e341d64fa247707b` | 1.99 | 2.13 |
| [43362598993](https://github.com/szTheory/muhtari/actions/runs/43362598993) | `63bc02f4a154f266014f4fbe3b61abd8d9e1d7a5` | 2.44 | 3.51 |
| [43399974556](https://github.com/szTheory/muhtari/actions/runs/43399974556) | `9b7d6547bd025a786442465f3916564534788fc1` | 5.52 | 3.73 |
| [43427086666](https://github.com/szTheory/muhtari/actions/runs/43427086666) | `461dda5545bcc202fb4b90c1232fb2e9a00b6c28` | 2.99 | 3.13 |
| [43474047661](https://github.com/szTheory/muhtari/actions/runs/43474047661) | `40de59e965864d4327ae77a8ab8b542845db015c` | 5.88 | 2.54 |
| [43725190953](https://github.com/szTheory/muhtari/actions/runs/43725190953) | `5a1336f91bf401bab96a8e868e99c661cdca8dae` | 3.76 | 3.28 |

The slowest complete sample is 5.88 seconds. Under DEC.47, twice that sample
is 11.76 seconds, rounded up to 12. `tests/compat/tests.cmake` now selects a
6-second base deadline only for macOS x64; the existing scale of 2 gives 12
seconds. Other platforms, workload, fixture, assertions, job deadlines and
scaling remain unchanged. No root cause beyond insufficient deadline margin
is inferred from the one timeout.

## Local validation

- `MUHTARI_TIMEOUT_SCALE=2 cmake -S . -B build/phase23-x64-registration -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_COMPILE_WARNING_AS_ERROR=ON -DCMAKE_OSX_DEPLOYMENT_TARGET=10.13 -DCMAKE_OSX_ARCHITECTURES=x86_64` — exit 0. This Apple Silicon configure targeted x86_64 without building or executing an x86_64 binary.
- `ctest --test-dir build/phase23-x64-registration --show-only=json-v1 | jq '.tests[] | select(.name == "title.pal.bios") | .properties'` — exit 0; the generated x86_64 test registration reports `TIMEOUT` 12.
- `cmake --preset ci && ctest --test-dir build/ci --output-on-failure -R '^title[.]pal[.]bios$'` — exit 0 on macOS arm64; the OpenBIOS setup and PAL predicate passed.
- `cmake --workflow --preset ci` — exit 0 on macOS arm64; 723/723 CI tests and 8/8 release tests passed.
- `scripts/hygiene.sh`, `scripts/check-title.sh 'test: calibrate PAL BIOS timeout from x64 samples'`, and `git diff --check` — each exited 0.

The native-arm64 results cover the unchanged test and local workflow. The x64
configuration check confirms the new macOS x64 deadline value; hosted macOS
x64 CI remains the executable acceptance for that runner.
