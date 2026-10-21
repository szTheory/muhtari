---
phase: 04-sound
plan: "08"
subsystem: cartridge-audio
tags: [pokey, cartridge, mapping, compatibility]
requires:
  - phase: 04-06
    provides: Reconciled actual-clock POKEY and exact-q23-16x64-dc20 synthesis recovery
  - phase: 04-07
    provides: Expanded documentary chip vectors
provides:
  - Declared cartridge POKEY bus windows with fixed primary and secondary identities
  - Accepted-v4 effective IRQ wiring separated from raw probe metadata
  - Atomic 0450 decode and behavior revision 2 released-pin migration
affects: [sound-mapping-vectors, irq, audio-hashes]
tech-stack:
  added: []
  patterns:
    - Fixed chip slots with actual declared-count mixer normalization
key-files:
  created: []
  modified:
    - src/cart.c
    - src/internal.h
    - src/boot.c
    - src/pokey.c
    - src/synth.c
    - src/instance.c
    - tests/core/test_core.c
    - tests/runner/tests.cmake
    - tests/built/expected.txt
    - tests/compat/ledger.csv
    - tests/compat/allow.txt
    - docs/hardware/cartridge.md
key-decisions:
  - Preserve measured unchanged pins; fixed-address sample setup bypasses detection, so only pokey changes its displayed detection result.
  - A lone 0440 chip retains secondary slot/source IDs and IRQ bit 1 while normalization counts one actual chip.
  - Use the existing dated ratchet exception for the retired detection-dependent title predicate.
requirements-completed:
  - SND-02
  - SND-03
coverage:
  - id: D1
    description: CPU programs at 4000, 0800, 0440 and 0450 produce public PCM with documentary register values and mirrors
    requirement: SND-02
    verification:
      - kind: integration
        ref: core.audio.silence
        status: pass
    human_judgment: false
  - id: D2
    description: ROM and RAM overlap reads survive chip writes while peek and DMA leave chip state unchanged
    requirement: SND-02
    verification:
      - kind: integration
        ref: core.audio.silence
        status: pass
    human_judgment: false
  - id: D3
    description: Ignored v4 metadata supplies no wiring, reload clears mappings, and lone 0440 retains secondary source and IRQ identity
    requirement: SND-02
    verification:
      - kind: integration
        ref: core.audio.silence
        status: pass
    human_judgment: false
  - id: D4
    description: Atomic behavior revision 2 mapping migration reproduces all 34 sample frame chains with only the detection-dependent pokey chain changing
    requirement: SND-03
    verification:
      - kind: integration
        ref: ctest --test-dir build/ci -L '^basic$' --output-on-failure --no-tests=error
        status: pass
    human_judgment: false
  - id: D5
    description: All four candidate title pins are remeasured on fast and OpenBIOS paths and the base-aware compatibility ratchet passes
    requirement: SND-03
    verification:
      - kind: integration
        ref: MUHTARI_ROM_DIR=build/ci/built MUHTARI_LEDGER_BASE=origin/main cmake --workflow --preset library
        status: pass
    human_judgment: false
actuals:
  tokens: 5542
  tasks: 2
  commits: 7
plan_head_before: 94fceb35ec3bf03da6ce9bb0d06fce9071ec7e35
plan_head_after: 84153a4acc9dae5f77e3709f9447d7b3b6478f2d
duration: 7min
started: 2026-10-20T23:07:34Z
completed: 2026-10-20
status: complete
---

# Phase 4 Plan 8: Runtime mappings and atomic 0450 migration Summary

**Declared POKEY cartridges now reach public PCM through the CPU bus; 0450 decoding and its measured released pins move atomically to behavior revision 2.**

## Accomplishments

- Mapped the 16-register mirrors at 4000-7FFF and 0800-0FFF, plus 0440-044F and 0450-045F. Dual 440+450 uses fixed independent slots. Ambiguous legacy primary declarations select 450, then 4000, then 800, bounding the total to two chips.
- Split CPU register reads/writes from board-memory peeks and DMA. Writes select the chip before board RAM; documented overlapping ROM/RAM stays readable. Existing board normalization remains intact.
- Stored effective accepted-v4 IRQ bits privately, preserving the public raw probe fields. Invalid/ignored blocks and subsequent reloads cannot retain stale wiring. Interrupt delivery remains the later IRQ plan's work.
- Restored declared count, source selection and clock origin through cartridge reset, including the fast-boot path. A lone secondary chip contributes source IDs 6-9 with one-chip normalization; dual chips retain primary IDs 2-5 and secondary IDs 6-9.
- Preserved the recovered exact-q23-16x64-dc20 implementation, final M18360, current ratio 85:342, gain4096, 16-tap support, 48 kHz cadence, frame boundaries and API3/ABI1. No synthesis-derived video or raw-transition re-pin occurred.

## Task commits

| Task | Commit | Purpose |
|---|---|---|
| 1 RED | aa64158 | Generated 4000/800/440 CPU-to-PCM test exposes missing mapping |
| 1 GREEN | 0d1a4b4 | Runtime mapping, overlap/wiring tests, boot origin and fixed secondary mixing |
| 2 RED | d3f9f7b | Standalone 450 and dual-chip assertions expose missing primary mapping |
| 2 support | d89eeab | Core version test accepts the reported nonzero revision |
| 2 support | 05647d0 | Movie warning checks the reported revision without assuming 1 |
| 2 support | 3f65618 | Narrow dated exception for retired pokey title predicate |
| 2 GREEN | 84153a4 | Atomic 450 decode, revision 1 to 2, frame pins and title evidence |

The atomic GREEN commit changes four files. `docs/compatibility.md` was regenerated and compared byte for byte; it is unchanged because it reports predicate kind/status rather than hashes. No empty or artificial documentation diff was introduced. No tracked file was deleted. Metrics measure the seven implementation/test commits before this summary commit; tokens are ceiling(22,166 realized diff characters / 4), on the plan estimate's scale.

## TDD evidence

- Task 1: cmake --build build/ci --target muhtari-test-core exited 0. The selected `core.audio.silence` CTest run exited 8 with planned assertions: silent PCM, open-bus values 40/08/04 instead of FF, and declared chip count 0 instead of 1. Its complete CTest JUnit report passed `node .../gsd-tools.cjs check tdd-red-evidence build/ci/cart-audio-red.json --raw`, exit 0, `RED_EVIDENCE_OK`. Semantic assessment: the target executed and failed on absent mapping behavior, with no setup or compilation failure.
- Task 2: the same target's JUnit run exited 8 on silent 450 PCM, missing primary DAC, open-bus read and dual count 1 instead of 2. `check tdd-red-evidence build/ci/cart-450-red.json --raw` exited 0 with `RED_EVIDENCE_OK`. Other mapped paths remained green. Semantic assessment: the target exercised the planned standalone and dual declarations and failed on their absent decode.
- Both GREEN implementations passed `core.audio.silence`; the first tracer passed the full CI workflow and the focused cartridge selection before 450 expansion. No refactor commit was needed. RED reports remain ignored build artifacts, never committed logs. `workflow.tdd_mode` was false; task-level TDD still followed both RED/GREEN sequences.

## Released frame-chain comparison

Every one of the 34 rows ran the established writer after the revision bump:

cmake -DCHAIN=build/ci/tests/muhtari-test-chain -DROM=build/ci/built/<name>/<name>.bas.a78 -DNAME=<name> -DEXPECTED=tests/built/expected.txt -DWRITE=ON -P tests/built/frames.cmake

All invocations exited 0. The first writer reset other rows as designed; every row was then filled by its own real 600-frame run. Old values were saved in ignored build scratch before migration. No unresolved frame pin remains.

| Declared 450 sample | Old chain | New chain | Measured cause |
|---|---|---|---|
| eightysix | 5f32f5067c35257da5984dc956a878f2e8a31e5a3048137f9a70d1b0d1f67299 | 5f32f5067c35257da5984dc956a878f2e8a31e5a3048137f9a70d1b0d1f67299 | Fixed address; unchanged video |
| pokey | 58f96bedc2097f9c81d5400d4bd91f89d53a46bb9e8820daa1644260017bb598 | 89a47db32322d6c497f5b8d77721aa743fc87ea40485f53fff5f22c628ce079b | Auto-detection now sees registers instead of open bus and displays the detected-chip title |
| rmtramcompressiondemo | 39c14b52eb6a8dc960f41a434c705b1f9695cf749b25c2a5537e024e2ef2ff90 | 39c14b52eb6a8dc960f41a434c705b1f9695cf749b25c2a5537e024e2ef2ff90 | Fixed address; static title unchanged |
| rmtramdemo | 01eeb2f730d30ffb163cbd087265c8c0768472891814b5cb37a97f8fc5609889 | 01eeb2f730d30ffb163cbd087265c8c0768472891814b5cb37a97f8fc5609889 | Fixed address; static title unchanged |
| rmtromdemo | 23d4ccd5ee418509eb0b290ca1c1283620faf47fdccd177d5eb64eb508c87899 | 23d4ccd5ee418509eb0b290ca1c1283620faf47fdccd177d5eb64eb508c87899 | Fixed address; song menu unchanged |

The remaining 29 rows reproduced unchanged at revision 2. In this table the value is both old and new:

| Sample | Old = new frame chain |
|---|---|
| adventurer | 2ee28bc4fcaf2be8d392b56785b7f1b47b029f33f8dd41459b5e3790930c433c |
| atarivoxspeech | 547356bc6a53c31285a5e634bce37d8b281c95c6dad2e4b7c61291eddf2c7e1a |
| backuptest | defbbd061ad811c58d1fe2f4f8af556869eeee7efe997dd9565d76e5ba24f928 |
| banner | 8a3a45c3cbf5996c47cb03c7dfc4af951c8478f780af51425c9dc732fa3ab385 |
| boxcollision | 569f1ff09ebd2aa4ec15b97ee1483fd85a1cee2abd5aa0598d00246658946ba9 |
| colorfade | 8a868b24329f4bd07636cf76da0669fc3ee7bc6bb0cdeeee447e7b662031c1a9 |
| doublebuffer | a0759ca6ae20dd20a73346cdc8984da407cb433f81b0035b9536893c71a1ab1d |
| drivingcontrol | f7636dc219b4a1257012644bcaf2cd4514dde83a1fd0b6551b278e74ba5674fc |
| fontcheck | 218471a10520aa82d1ac5dec41becb7165fa25b1974f8524f0988e285e17223a |
| helloworld | 0bcad35bf4f8a0acc41783ca4a2bbda9a947e7c67bf91a0613e349d8fb39df80 |
| hiscoretable | 9a32df28f09a0cdc002dbb7e6c463e8b3d09edc86ad63b520c5ea5882ce0bd9e |
| keypadcontrol | 9cea5660c65db2b3dc7fe7abbc718f46e2d31a3ff9bbead009436dbf2dc4049c |
| languagetest | 77aa5b51c1c78ce0153cbd3c5fe022a1775eb4aceeeeec046e84c3d14204a634 |
| mega7800controllers | 8a824c5fc3dccafb6797cbe5336f713746e20aef4db5f21cdc5800383112b0d4 |
| mousedemo | c819552e2cb8b98e656581b3215a724ac747db6e9a40e87552b544728c520327 |
| multibuttoncontrollers | 99264210d496383e4a7a9346a306e9ca248e22441e76cc0c4304a5b104b386bd |
| multisprite | 135a33d79ddf49080b149596e2a8a08bd64ee921ae9608e86c7cd2cf0a08eca0 |
| onehundredandone | df2a3ec6f43bb8133e36bd5733159a44fc87b6506edb10b19c6bc1111618f273 |
| plotmapfile | 62d99cde6648097f923baff4e89e96c2a52f3fea5c0a136b32b0a5dede73e430 |
| ramcharmap | 55599431c2d99c16ac27ca88044e0ef1576399d41c3154e965fdf5043172a24b |
| rotationalcontrols | 893be07272c704a332d71a08c2d3439c9947ac979f6a382447926ac2b8f13eee |
| screensafe | 7cbf9f511a6c21f19ddc3053fb95451ddd3c5b464be331179fb9bb5ca9f1f700 |
| simple | d35eaf8110c0ffd3748193225d62ec782dd91b6c0c2b28a23340906ace6ef40b |
| smasteroids | 10ebb86fe85bbb33c4d4f247613d7cd765b1e998c14b3ee9be4ce1990f24113e |
| snescontrollers | d9985f1c67c4723d84110353567814c3d4eec6127b109a64dfabd1e10d56b946 |
| soundtest | 39abd4bc3f97c6085b3ca5c78722ffadcc04950bc4ff6b2cfc6f23457664146c |
| spacephysics | 0159c82d0d288b67d23024ac833d28d68f225a3ecce1fe77a0b8d5b46b58219a |
| trackertest | cebf3541b8647978fa7945512fca2968f18f060983ee38e5688f873e36178c3e |
| underthehood | 88cd56b43bb5579fe8f13fd5e02a0717d4abe29e068b4b4e3856aaaeef1a0036 |

## Title pin comparison

Only the four named candidates were reset to `title@120=-`. The established pin recipe filled all four from actual runs, then evaluated all eligible ledger rows on fast and OpenBIOS paths, exit 0. No unresolved title pin remains. Eightysix retains its documented weak `boots@120;why=rng-seeded-from-intim` predicate.

| Sample | Old title@120 | New title@120 |
|---|---|---|
| pokey | 8fed977457666af8e363f6bd4e5f37375e1f457d67156ef326042fc684f1ed9e | dcef4b423da52d04a824def3dfa1f0167c5b9e5bdfbf5af55906d21b46464244 |
| rmtramcompressiondemo | 44ca450f4a2354a07aefaa3c14cb5e23c2b4e1e8a5e7b3f9ff23f23442adc87a | 44ca450f4a2354a07aefaa3c14cb5e23c2b4e1e8a5e7b3f9ff23f23442adc87a |
| rmtramdemo | 9e51762697c5916ccd52d7687f98406109033e84adc923ac63941cc5f2b401a5 | 9e51762697c5916ccd52d7687f98406109033e84adc923ac63941cc5f2b401a5 |
| rmtromdemo | b6794706e93167afbcd4e43521217c3484b6b2140137bf46b1f395e8b05ac10d | b6794706e93167afbcd4e43521217c3484b6b2140137bf46b1f395e8b05ac10d |

The same detection/static-title causes apply as above. Evidence prefixes were rewritten by the pin recipe. The expected file's stale prediction was replaced with the measured result and a reference to the reported revision. The pending re-pin todo is retained as instructed; its migration is now satisfied by this evidence, with no deletion in the atomic task.

## Verification

All evidence is local to this step; no hosted CI run exists because this wave neither pushes nor ships.

| Command | Exit | Evidence |
|---|---|---|
| cmake --workflow --preset ci after Task 1 | 0 | 594/594 ordinary checks and 6/6 release checks |
| `ctest --test-dir build/ci -R '^core.audio.silence$|^cart\.' --output-on-failure --no-tests=error` | 0 | 21/21 including fixture; tracer verified before expansion |
| `ctest --test-dir build/ci -L '^built$' --output-on-failure --no-tests=error` | 0 | 43/43 including dependencies; all sample fixtures built |
| `frames.cmake -DWRITE=ON` once per row, expanded command above | 0 | 34/34 writers; only pokey changed, all 33 others equal |
| cmake -DPREDICATE=build/ci/tests/muhtari-test-predicate -DLEDGER=tests/compat/ledger.csv -DBUILT=build/ci/built -DDIAG=tests/diag -DBIOS=build/ci/built/openbios/openbios.bin -P tests/compat/pin.cmake | 0 | Four remeasured title pins and every eligible row passing both boot paths |
| cmake --workflow --preset ci after Task 2 | 0 | 594/594 ordinary checks and 6/6 release checks |
| `ctest --test-dir build/ci -L '^basic$' --output-on-failure --no-tests=error` | 0 | 71/71 including 34 fixture builds, all frame chains and revision guard |
| `ctest --test-dir build/ci -L '^title$' --output-on-failure --no-tests=error` | 0 | 6/6 including dependencies and unpinnable-blank negative control |
| `MUHTARI_ROM_DIR=build/ci/built MUHTARI_LEDGER_BASE=origin/main cmake --workflow --preset library` | 0 | 43/43 including base-aware ratchet, both-path predicates and regenerated compatibility page |
| `ctest --test-dir build/ci -R '^core.audio.silence$|^core.info$' --output-on-failure --no-tests=error` | 0 | 2/2 on final migration objects |
| cmake -DLEDGER=tests/compat/ledger.csv -DOUT=docs/compatibility.md -P tools/compat_doc.cmake | 0 | Regenerated page is byte-identical |
| cmake -DLEDGER=tests/compat/ledger.csv -DOUT=build/ci/cart-compatibility.md -DCHECK=docs/compatibility.md -P tools/compat_doc.cmake | 0 | Final byte comparison |
| `scripts/hygiene.sh --tree` | 0 | Tree privacy/fixture hygiene |
| `git diff --check` | 0 | No whitespace errors |

An ignored, one-off host probe linked the shipped static library and ran each sample for 600 frames. Build command: `cc -std=c17 -Iinclude -Isrc build/ci/cart-pcm-probe.c build/ci/libmuhtari.a -o build/ci/cart-pcm-probe`, exit 0. Runs `build/ci/cart-pcm-probe build/ci/built/<name>/<name>.bas.a78` exited 0. Without input, nonzero PCM sample counts were eightysix 471989, pokey 470452, rmtramcompressiondemo 468844 and rmtramdemo 471259, with nonzero raw POKEY frames in every case. Rmtromdemo's no-input menu is intentionally silent. Pressing both fire buttons at frame 100 and releasing at 120 starts its selected song: 336729 nonzero PCM samples, 318 frames with nonzero raw POKEY levels, SKCTL3. These finite playback checks establish connected output, not spectral or physical-hardware accuracy. The temporary probe is not a recurring gate or committed artifact.

## Deviations from Plan

1. **Measured pin prediction correction.** The plan expected five changed frame chains and four changed titles. Actual measurement changes only pokey. All five images probe as declared 450; four explicitly say `set pokeysupport $450`, while pokey says `set pokeysupport on`. The pinned package's includes/pokeysound.asm fixed-address path initializes directly and bypasses auto-detection. Their video therefore stays unchanged while audio becomes active. The unchanged values above are deliberate evidence, not omitted migration. No non-POKEY chain drift occurred.
2. **Rule 2 — missing supporting state initialization.** The fast-boot path reset synthesis to zero chips after cartridge reset, and fixed secondary source IDs require the mixer to sum the secondary slot with one actual chip. Supporting edits to `src/boot.c`, `src/pokey.c` and `src/synth.c` were required for the planned tracer; 0d1a4b4 includes them and their tests. The source timeline and synthesis coefficients are unchanged.
3. **Rules 1/3 — stale revision assertions.** `core.info` assumed behavior1, and the movie-warning test's regex assumed the core reports 1. Removed those stale assumptions in test-only commits d89eeab and 05647d0. The first post-migration full workflow exposed the movie regex mismatch; the final full workflow passes. No unrelated tooling cleanup was added.
4. **Rule 2 — existing base-aware ratchet contract.** All fresh rows passed, but the first base-aware library run exited 8 because the old passing title predicate was retired. The existing documented mechanism requires a narrow dated allow line. Commit 3f65618 names exactly that old predicate, its cause and expiry 2026-11-19; the replacement still passes on both boot paths. Remove the exception after merge under the existing allow-file policy. No ratchet code or new policy machinery changed.

No auth gates, owner checks, package installs or architecture changes occurred. The initial library invocation without its documented ROM-directory environment failed configuration, exit 1; supplying the documented environment resolved it. The upstream recovery RED history and reconciled 04-06 summary were preserved. Shared STATE/ROADMAP/REQUIREMENTS updates belong to the wave orchestrator and were deliberately not committed by this executor.

## Limits and next work

- The final production synthesis remains the recovered integer model; physical cartridge gain and coupling corners remain unmeasured. This plan adds no new physical-accuracy claim.
- Focused `pokey.maps` expansion, richer adversarial mapping vectors, CPU IRQ delivery, audio hashing and host/platform parity remain their separately planned waves. No later wave was executed here.
- SND requirement coverage here is this plan's cartridge integration slice, not phase completion or a substitute for later IRQ/platform proofs.
- No new implementation stubs, skipped tests or unrun plan verification commands remain. The preexisting audio-hash `-` columns are the later audio-hash plan's output and were not redefined here.

## Self-Check: PASSED

All 12 modified implementation/test/documentation paths exist. All seven listed commits are ancestors of the final implementation HEAD; the ledger measures seven commits from the recorded base. The atomic commit contains decode, revision and actual changed pins together. Both required task gates, the cartridge tracer and the additional base-aware compatibility check passed. No tracked deletions or new external trust surfaces were introduced.
