---
phase: 20-complete-ym2151-homebrew-sound
plan: "04"
subsystem: native-consumers
requires:
  - phase: 20-03
    provides: Complete conditional YM state
provides:
  - Same-byte public C and runner continuation
  - Actual loaded libretro YM and transactional unsupported reload
  - Source-bound native ordinary and worst receipts and strict equality identity
plan_head_before: 2fdca72218305aeb2b95cb56109ec37fce26acd5
plan_head_after: 7691782afc731fa62ccf47b178ab2e1bb3e9889f
requirements-completed: []
coverage:
  - id: native-routes
    description: Same cartridge executes through public C, real runner and loaded libretro in both regions with numeric and continuation checks.
    requirement: YM-06
    verification:
      - kind: integration
        ref: "build/v6-phase20-04-root-final.log; focused native consumer CTest, exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: native-receipts
    description: Ordinary and worst native observations bind fixture, source, regions, callbacks, PCM, state replay and complete timing windows.
    requirement: YM-07
    verification:
      - kind: integration
        ref: "libretro.observe.ym and libretro.observe.ym.controls, exit 0; build/ci/tests/emscripten/ym-native.json and ym-worst-native.json; URL N/A-local"
        status: pass
    human_judgment: false
  - id: equality-identity
    description: Exact YM row family and fixture identity reject missing, duplicate, reordered and forged evidence while preserving historical rows.
    requirement: YM-03
    verification:
      - kind: integration
        ref: "build/v6-phase20-04-root-policy-fixed.log; policy.quality.controls and policy.lane_equality.controls, exit 0; URL N/A-local"
        status: pass
    human_judgment: false
status: complete
---

# Native YM consumers

The same generated cartridge now has independently checked public C, runner and dynamically loaded libretro routes in NTSC and PAL. Checks cover numeric busy/timer/IRQ mailbox, sample cadence and non-silence before equality, duplicated mono, callback alternatives, reset, save/load continuation, buffer extents and unsupported YM+Covox reload preserving the loaded session.

The existing native observer additionally emits separate ordinary/worst receipts. Its two regions each execute six checked frames, replay four frames, warm up to twelve, then run three restored sixty-frame windows including callback copy and canonical video/audio hashes. This directly compiled observer supplies portable parity evidence; the separate loaded-module host establishes actual module delivery. These are digital diagnostics, not physical XM or listening evidence.

The lane producer retains historical rows and adds fixed YM fixture/frame/raw/mono/state/resumed rows. The quality producer records this additive capability separately from historical metrics; unavailable old capability is not invented old execution. Genuine old-core refusal remains Plan 20-06.

Commands and exits (all URL N/A-local):

Command: `cmake --preset ci` and `cmake --build build/ci --parallel 2`: 0.
Command: `ctest --test-dir build/ci --output-on-failure --no-tests=error --parallel 1 -R '^(ym[.](fixture|public_runner|tracer|lifecycle|hostile)|runner[.]ym|core[.]state[.]ym|libretro[.](host[.]ym|observe([.]ym)?([.]controls)?))$'`: 0; final native routes and prior state/lifecycle controls.
Command: `ctest --test-dir build/ci --output-on-failure --no-tests=error --parallel 1 -R '^(policy[.](quality|lane_equality)[.]controls|ym[.]public_runner)$'`: 0 after diagnosing the inherited CMake `frame` variable affecting an unquoted constant.
Command: cmake -P build/v6-ym-quality-scope.cmake: 0; executes the exact production helper and checks report-path preservation.
Command: `git diff --check`: 0.

Retained failures include an incorrect runner path, fixture mailbox mapping and guest stack/timer setup, partial-first-frame expectations, PAL video extent, public input device selection, an unquoted CMake constant, and inventory labels. Corrections were scoped to their cause. The unsupported reload check exposed and fixed actual libretro early teardown. A direct quality producer invocation correctly refused stale/incomplete JUnit; no full-source quality pass is claimed before Plan 20-07. Logs remain ignored build output.

Plan deviations: existing helper ownership required `tests/libretro/ym_observe.cmake`, `tests/cmake/quality.cmake`, `quality_evidence.cmake` and `tests/policy/tests.cmake`. No new framework or dependency. Parent completed the stalled executor's consumer work directly, preserving all edits. No review/gap round was consumed. Current requirements remain open until downstream portable/package, genuine compatibility and final gates finish. WASI compiler availability is qualified with the final local lanes; no absent lane is called passing.

## Self-Check: PASSED

The source commit and summary exist; named native and parser gates exited zero. Downstream requirement closure remains pending.
