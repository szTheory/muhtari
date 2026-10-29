# Deferred items, phase 11

## Found in plan 04

- `diag.bankset_tracer` and `diag.bankset` both write `build/ci/tests/diag/bankset/32n/` and carry no resource lock, so under `ctest -j` one can read the other's `pixels.png` half written (`bankset --bankset-png ... png: chunk longer than the file`). Seen once in `cmake --workflow --preset ci`; passes alone and on rerun. Not caused by plan 04; a fix is a `RESOURCE_LOCK` or a separate output directory in `tests/diag/tests.cmake`.
