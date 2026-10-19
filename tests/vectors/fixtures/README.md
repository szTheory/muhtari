# Vector fixtures

Small pieces of the SingleStepTests 65x02 vectors, from https://github.com/SingleStepTests/65x02,
directory `6502/v1` at commit `2f6980a2d95757486c7bee24355c360e40e2a224` (MIT; the notice is the
`singlesteptests` section of `THIRD-PARTY-NOTICES.md`). Each file has a row in
`tests/roms/manifest.txt` with its SHA-256, and `.gitattributes` keeps git from rewriting its bytes.

The tests in `tests/cpu/tests.cmake` run them through `muhtari-vecconv` and `muhtari-test-vectors`.

| File | Taken from | What it is | What it tests |
|------|------------|------------|---------------|
| `a9.json` | `6502/v1/a9.json` | bytes 0 to 1,799: five whole tests (the fifth ends at byte 1,662), then the sixth cut inside its initial RAM list | the spaced upstream layout through the converter and the harness, and a prefix cut mid-test |
