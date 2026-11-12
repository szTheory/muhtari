---
phase: 20-complete-ym2151-homebrew-sound
plan: "06"
subsystem: state-compatibility
requires:
  - phase: 20-04
    provides: Complete native YM consumers and public state
provides:
  - Genuine independent ten-case bidirectional legacy compatibility matrix
  - Actual old YM refusal and transactional cross-topology state refusal
  - Wrong source, library, header and repaired-integrity fault controls
plan_head_before: 1b37c0e
plan_head_after: 1f8aacd
requirements-completed: []
coverage:
  - id: genuine-state-matrix
    description: Independently linked old/current public callers produce identical non-YM states and both continuation directions for ten topology/region cases.
    requirement: YM-05
    verification:
      - kind: integration
        ref: "build/v6-phase20-06-final.log and build/v6-state-compat/result.json; genuine matrix plus controls; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: safe-capability-refusal
    description: Genuine old core refuses YM content and new states; old/current refusal preserves live state, battery, allocator count and subsequent output.
    requirement: YM-05
    verification:
      - kind: integration
        ref: "state_compat MODE=controls; actual old/new linked drivers; exit 0; URL N/A-local"
        status: pass
      - kind: unit
        ref: "core.state.fields, topologies, battery and ym; build/v6-phase20-06-core.log; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
status: complete
---

# Genuine compatibility

One public-header-only driver is independently compiled/linked against the frozen genuine kickoff products and current products. It never reads private instance layout or calls the current serializer to manufacture old states. The kickoff worktree, header and library remain unchanged; source commit, clean status, build origin, trusted preparation receipt and exact artifact/header digests are checked before execution. Runtime API/ABI/behavior checks reject a swapped link. Each resulting driver and library/header digest is recorded in ignored result JSON.

The ten cases are NTSC/PAL pairs for linear, SuperGame cartridge RAM, XBoard RAM/POKEY, non-YM XM RAM/HSC and SaveKey with an actual in-flight guest EEPROM write. Public spans seed nonzero RAM and committed battery data; guest instructions produce TIA/POKEY activity, increment RAM, write HSC and bit-bang EEPROM. The bounded SaveKey capture requires native-format-1 pending-write state at a completed frame boundary. Every capture also contains a posted pending input.

Old and current independently produced checkpoint bytes, four subsequent frames of video/mono PCM, counters, all public RAM spans, battery bytes and relative generation changes, and ending state bytes agree. Old-to-new and new-to-old resume in separate processes and match each independent producer's uninterrupted result. Both regions also prove genuine old refusal of YM content without allocation, old refusal of real YM snapshots, current non-YM refusal of YM snapshots and current YM refusal of legacy snapshots. Exact live state/battery/generation and subsequent outputs are preserved; the old size-before-version status ordering is accepted.

The actual entry point rejects wrong source, substituted library and wrong header origins. Its changed-byte control repairs CRC after changing a current state revision: the same-point comparison and real loader reject the intended fault, preserving continuation. Existing identity/topology/battery/semantic unit cases remain intact.

Commands (exit 0, URL N/A-local):

```sh
cmake -DROOT=. -DOLD_SOURCE=.worktrees/ym-state-baseline \
  -DOLD_BUILD=build/v6-state-baseline -DNEW_BUILD=build/ci \
  -DWORK=build/v6-state-compat -P tests/core/state_compat.cmake
cmake -DROOT=. -DOLD_SOURCE=.worktrees/ym-state-baseline \
  -DOLD_BUILD=build/v6-state-baseline -DNEW_BUILD=build/ci \
  -DWORK=build/v6-state-compat -DMODE=controls -P tests/core/state_compat.cmake
cmake --build --preset ci --target muhtari-test-state
ctest --test-dir build/ci --output-on-failure --no-tests=error --parallel 1 \
  -R '^core[.]state[.](fields|topologies|battery|ym)$'
ctest --test-dir build/ci --output-on-failure --no-tests=error -R '^docs[.]budgets$'
```

The first driver compile exposed an unused helper, resolved by implementing the required integrity fault mode. The initial compatibility prose exceeded the existing 40 KiB embedding page budget; the prose was shortened, and the unchanged budget passes. Pinned formatting and diff checks pass. The two-way matrix and controls were rerun after the final bounded path check. No baseline setup/build was repeated, no old vectors/digests changed, and no review/gap round or host/remote scope was consumed. This proves the named unchanged envelopes, not general migration or YM support in old code.

## Self-Check: PASSED

The source commit, public caller/helper files and summary exist. Genuine matrix, controls and recurring state checks passed; final-source phase acceptance remains Plan 20-07.
