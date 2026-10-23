---
phase: 06-retail-cartridge-boards
plan: "07"
status: complete
subsystem: libretro
tags: [activision, fractalus, absolute, rcheevos, identity]
requires:
  - phase: 06-06
    provides: Three owned executable board diagnostics and independent numeric expectations
provides:
  - Truthful original-content Activision fixed ROM and physical Fractalus alias descriptors
  - Mandatory all-byte achievement resolver acceptance with explicit holes
  - Identified headered and headerless AM/OM equivalence through library, runner and loaded testdb module
affects: [06-08, 06-09]
tech-stack:
  added: []
  patterns:
    - Existing isolated test tables and core-object substitution for uninstalled test hosts
    - Raw audio observer registration before load in test-only module builds
key-files:
  created: []
  modified:
    - libretro/libretro.c
    - tests/libretro/rcheevos.c
    - tests/libretro/tests.cmake
    - tests/cart/test_cartdb.c
    - docs/libretro.md
key-decisions:
  - Retain all sixteen Fractalus aliases and fixed ROM within21 resolver regions; no omission is needed.
  - Translate Activision fixed ROM descriptors into original input orientation without changing raw identity.
requirements-completed: []
requirements-contributed: [BOARD-01, BOARD-02, BOARD-03]
coverage:
  - id: activision-loaded-equivalence
    description: Identified headered and headerless AM and OM preserve distinct original identities and equal mapped, numeric, video and audio behavior
    requirement: BOARD-01
    verification:
      - kind: integration
        ref: "libretro.rcheevos actual library/testdb runner/loaded module comparisons and numeric diagnostic driver; 600 frames per layout,479591 PCM samples; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: absolute-fixed-host-map
    description: Every intended fixed ROM byte resolves non-NULL and every switched window remains a hole
    requirement: BOARD-02
    verification:
      - kind: integration
        ref: "libretro.rcheevos all65536-address comparison; Absolute11 regions and39424 mapped bytes, zero wrong; missing fixed descriptor mutation rejected; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: fractalus-physical-alias-host-map
    description: Sixteen truthful256-byte aliases and fixed ROM fit below the pinned region cap while higher mirrors remain holes and physical RAM stays2048 bytes
    requirement: BOARD-03
    verification:
      - kind: integration
        ref: "libretro.rcheevos actual loaded Fractalus map21 regions and43520 mapped bytes, zero wrong; physical CART_RAM2048 and battery-save0; exit 0; URL N/A-local"
        status: pass
      - kind: other
        ref: "cmake --workflow --preset ci and exact inventory checks; exit 0; 612+6; exactly-three local ZIP total214131 <=399937; URL N/A-local"
        status: pass
    human_judgment: false
actuals:
  tokens: 9777
  tasks: 2
  commits: 2
plan_head_before: 37f4b509ae57797f57e690e067b4923e345e420f
plan_head_after: b564204d22a7e7cf037de96e796e2d963da4237d
duration: 10min
started: 2026-10-23T20:16:10Z
completed: 2026-10-23
---

# Phase 6 Plan 7: Truthful retail achievement maps and loaded AM/OM equivalence Summary

**All intended retail descriptor bytes are mandatory through the pinned resolver, and identified AM/OM inputs retain distinct raw identities while matching numeric, mapped, frame and audio observations across all three deliverables.**

## Task commits

| Task | Commit | Outcome |
|---|---|---|
| 1 | `b60f03c` | Original-content Activision fixed descriptors, Fractalus physical aliases and fixed ROM, mandatory resolver checks and precise visibility docs |
| 2 | `b564204` | Owned diagnostic test-only key, loaded testdb module, identified pair execution, actual runner/raw identity checks and representative fault controls |

Actual tokens are the 39,105-character realized task diff divided by four and rounded up. The persisted ledger measures two task commits; later summary/state metadata is outside that interval.

## Actual map acceptance

The existing pinned rcheevos resolver's MIT license was checked before its own map code/header were read. Its32-region cap and dependency pin are unchanged. The actual adapter loads each cartridge and runs30 frames, fills public RAM spans with nontrivial patterns, then compares all65,536 addresses. An intended byte must be non-NULL and equal to the bus; a switched window or deliberate higher-mirror hole must be NULL. The established rcheevos $2800-$2FFF system-RAM alias is checked against its exact pointer instead of the core's undecoded bus there.

| Actual loaded image | Regions | Mapped bytes | Wrong addresses |
|---|---:|---:|---:|
| Existing simple sample | 11 | 39,424 | 0 |
| Existing doublebuffer sample | 13 | 39,424 | 0 |
| Activision owned diagnostic | 13 | 39,424 | 0 |
| Absolute owned diagnostic | 11 | 39,424 | 0 |
| Fractalus owned diagnostic | 21 | 43,520 | 0 |

Fractalus's16 aliases expose $4000-$4FFF over eight physical256-byte pages. All higher descriptor mirrors $5000-$7FFF remain holes, with fixed ROM $8000-$FFFF retained. `MUHTARI_MEMORY_CART_RAM` is exactly2,048 bytes, while libretro battery-save memory remains absent because this RAM is volatile. No D-11 fallback omission is used and no complete achievement-visibility claim is made.

Activision descriptors reference canonical offsets $18000/$1A000/$1C000/$1E000 only after translating them to retained input orientation: identified AM uses XOR $2000. Switched $A000-$DFFF remains a hole. Absolute's fixed upper32KiB remains mapped and its low switched window stays unmapped.

## Identified AM/OM acceptance

Python hashlib independently measured SHA-1 `cfbfdf09bd8775710536891621506973d25c005c` over the owned Activision diagnostic's131,072-byte OM body. The key exists only in `tests/cart/test_cartdb.c`. The existing production-key-absence test passes; no production synthetic key or additional committed ROM is introduced.

The test constructs an independent runtime AM pair by copying each OM bank's two8KiB halves in reversed order. Both headered and headerless pairs are written only into the ignored build directory. Each of the four inputs is required to identify through the hash seam with the exact orientation flag. Full/headerless raw hashes remain distinct for AM versus OM and equal to independently hashed supplied files; caller-buffer SHA-256 remains unchanged after library and module execution.

For each input, the actual linked library and actual dynamically loaded testdb module run600 frames. Their mapped bytes and complete256-byte mailbox agree, as do raw transition and canonical little-endian PCM hashes/counts. Every input yields479,591 mono PCM samples. Video hashes agree between all AM/OM variants within the library's native output and within the module's frontend output. The owned diagnostic is silent: raw event count is0 and the raw stream is empty; this is disclosed rather than claimed as a new sound workload.

The actual testdb runner separately runs all four inputs for600 frames with frame600, raw audio, PCM and diagnostic output. Those complete outputs must be identical. The existing diagnostic driver judges every input against the27 independent expected records through library, actual runner report and actual loaded module. No numeric verdict is inferred from a frame hash. Representative controls reject a dropped fixed-ROM descriptor, an untranslated AM descriptor offset and a one-byte-truncated actual runner report.

The test-only module accessor/raw-observer setup is conditional on `MUHTARI_LIBRETRO_TEST` and is absent from shipped modules. Observer storage is allocated before content load, following the existing private observer contract, never during frames. Test targets and the isolated database are not installed or packaged. Existing CTest names/inventories are reused; the bounded600-frame host workload uses the existing diagnostic120-second budget.

## Commands, exits and run URLs

All commands ran locally; run URL is N/A-local. No logs are committed.

| Command | Exit | Result |
|---|---:|---|
| Initial Task1 focused command below | 8 | The added test incorrectly expected volatile Fractalus RAM as battery-save data; corrected before commit |
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(libretro[.](rcheevos\|host[.]memory_map)\|diag[.]retail)$' --no-tests=error --output-on-failure` | 0 | 6/6 including required fixtures; Task1 repeated after commit |
| Initial Task2 focused runs | 8 | Raw observer setup after load was correctly refused; test-only setup moved before content load |
| Final focused command above | 0 | 6/6, including actual pair/three-host/fault observations, resolver and retained host checks |
| `cmake --workflow --preset ci` | 0 | 612 non-release tests, three fresh packages and six release checks |
| `cmake --build --preset ci --target package && ctest --preset ci-release --output-on-failure` after Task1 | 0 | Three fresh packages, six release checks |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | Exactly612 tests ran once each |
| `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | 0 | Exactly six release tests ran once each |
| `scripts/hygiene.sh --tree` and `git diff --check` | 0 each | Hygiene/whitespace pass, with hooks enabled |
| `git diff 37f4b50..b564204 -- tests/built/expected.txt tests/audio/expected.txt` | 0 | Empty: all previous frame/audio predicates remain unchanged |
| Exactly-three ZIP expansion and `wc -c` sum below after each task | 0 each | Task1 total214132; final214131; both <=399937 |

```sh
set -- build/ci/muhtari-*.zip
test "$#" -eq 3 || exit 1
archive_bytes=0
for archive do
  archive_size=$(wc -c < "$archive")
  printf '%s %s bytes\n' "$archive" "$archive_size"
  archive_bytes=$((archive_bytes + archive_size))
done
printf 'archive_bytes=%s ceiling=399937\n' "$archive_bytes"
test "$archive_bytes" -le 399937
```

| Component ZIP, macOS arm64 | Task1 bytes | Final bytes |
|---|---:|---:|
| library | 68,754 | 68,753 |
| libretro | 68,092 | 68,092 |
| runner | 77,286 | 77,286 |
| Total | 214,132 | 214,131 |

## Deviations and integration fixes

1. **[Rule 2 - Observation setup]** The actual module's existing test-only instance accessor was exported in test builds, and a conditional pre-load raw observer setter was added. This permits actual loaded-module raw/PCM comparison without exposing a public API or allocating during emulation. The initial late registration was refused by the existing contract; the corrected pre-load seam passes full verification. WINDOWS40 is fixed.
2. **[Rule 1 - Test semantics]** Corrected the initial added assertion that confused volatile cartridge RAM with battery-save memory. The production adapter continues exposing no battery save. The physical library span2048 and absence of libretro saves are both asserted. WINDOWS39 is fixed.
3. **[Rule 2 - Reuse numeric acceptance]** Compiled the existing diagnostic driver against isolated test tables and core objects for the AM/OM library and module paths. No diagnostic parser or expectation is duplicated, and no new CTest registration is required.

## Remaining acceptance

No BOARD requirement is completed by this slice. Plan08 retains hostile input, fuzz/property, deep transaction/no-allocation and interleaved-instance expansion. Plan09 retains actual six-native package/portability and hosted migration acceptance. Local archive fit proves only macOS arm64. BOARD-01 descriptorless prohibition/edge metadata stays unresolved with verification:null; the transparency intent is exercised and documented without silently changing that metadata. No owner UAT or new resource bound is introduced.

## Self-Check: PASSED

Both task commits are reachable from the phase branch, every modified file exists, no blocking stub was introduced, all intended maps and representative faults execute, exact inventories/full workflow pass and all three local packages fit. Phase-goal verification and hosted acceptance remain the parent's responsibility after the remaining plans.
