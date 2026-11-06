# Local input-tracer proposal

Status: owner-approved local-only batch completed and verified2026-10-31,
uncommitted. The approved design below is retained; original proposal wording
does not grant a live allowance. The latest hosted receipt remains
[a failure](https://github.com/szTheory/muhtari/actions/runs/41158640754),
and no packaged-core acceptance is implied by this proposal.

## Decision and scope

Recommend one local-only implementation and verification batch to learn whether
a separately instrumented libretro core can reliably report input-poll and
port-1 Up/B observations through forced child cleanup. This does not authorize
RetroArch, actual keyboard events, content probes, publication or a hosted run.
The actual helper's platform API is exercised only through existing mocks.

Proposed ownership: `libretro/libretro.c` for compile-time diagnostic hooks;
`libretro/CMakeLists.txt` and `tests/libretro/tests.cmake` for a test-only,
non-installed diagnostic module and registration; a new small
`tests/libretro/input_trace.c` and associated header for bounded trace encoding;
`tests/libretro/host.c` (or the existing host harness file identified during
implementation) and a new focused test for callback and frame comparisons;
`tests/retroarch/keyboard_preflight.c`, `tests/retroarch/run_retroarch.cmake`,
and their existing policy mocks for trace descriptor setup and collection;
`docs/testing.md` for the diagnostic limitation; inventories only if required by
the existing test registration contract. Minimal additional test support files
may be added under those test directories. Preserve all existing unrelated edits.
If these seams cannot support the design without changing the shipped core's
behavior or the existing host bounds, stop and revise the proposal.

## Trace contract

The helper creates two fresh, exclusive regular files in a private output
directory outside disposable `RA_DIR`: one owned by the helper and one by the
instrumented child. Reject path overlap, existing files, symlinks and failed
setup; retain both paths after cleanup. The helper opens the child's descriptor
before fork and explicitly makes only that descriptor inheritable. The diagnostic
core receives the descriptor through a diagnostic-only environment setting;
the normal build contains no trace path. The helper closes its child copy after
fork and retains ownership of its own file. A local test must prove descriptor
ownership, no accidental overwrite and collection after forced child death.

Use versioned fixed-size records with magic, type, sequence, process identity,
absolute `CLOCK_MONOTONIC` seconds/nanoseconds, clock status, and event-specific
fields. Each writer emits one bounded write per record; a short write or error
permanently disables that writer and is reported when possible. No retries in
the polling path. At most 64 records per writer, at most 128 bytes each: 8 KiB
per file and 16 KiB total. Reserve the last slot for an explicit saturation
record; missing saturation or terminal records are never interpreted as clean
completion. These limits bound record count and bytes, not filesystem latency;
local regular-file writes can still perturb scheduling or block. Measure local
elapsed overhead and report it rather than asserting a real-time guarantee.

Helper records identify initialization, its PID and forked child PID, start,
each relevant event posting with actual event type and clock status, and final
reason. Core records identify initialization, the first completed non-null poll,
the first actual port-1 Up/B query result and subsequent changes, and explicit
null callback or unavailable/non-ordinary port state. Capture values already
returned by the existing callbacks; do not call a callback again. If no instance
exists after the poll, record that separately. Record clock errors without
inventing timestamps. Compare event times only after validating both streams,
matching child identity and same host/clock domain; logging arrival time is not
an event time. A parser distinguishes absent, empty, truncated, invalid, complete
and saturated traces, and identifies the last valid record after a forced kill.
An absent final record after a kill is expected, not a successful close.

## Local verification and limits

Run the existing complete `cmake --workflow --preset ci`, focused libretro and
preflight policy tests, and the existing hygiene/format and inventory checks.
Add deterministic tests for held, released and changing states, wrong port or
button, no poll, null callbacks, no instance, a late first poll, non-ordinary
port, clock failure, failed and short writes, record saturation, malformed and
truncated output, forced child kill and output collection. Compare instrumented
and normal modules under the same synthetic frontend: callback order, count and
return values, frame and audio results must match. Tests must demonstrate that
representative omitted or corrupted records and changed callback behavior are
rejected. Use local synthetic processes and existing mocks only; no actual
frontend, input device, or content launch. Record commands and exit statuses,
one bounded review, and any failing checks without weakening contracts.

The diagnostic module is not the packaged release core. A valid record could
show polling and returned button states at observed instants; it cannot prove
window focus, F8 processing, event delivery between polls, or behavior without
the instrumentation. A missing trace proves neither missing polling nor missing
input unless setup, identity, writes and collection establish that conclusion.
Successful local checks authorize no host run. Any later hosted diagnostic
requires a separate exact proposal, fresh source/service/receipt checks, and a
new explicit decision; it cannot restore spent allowances or accept the phase.

## Approved local execution outcome (2026-10-31)

The owner approved this concrete batch. Implementation and local verification
are finished; actual host input/readiness/routing remains unresolved. The normal
module has no diagnostic environment string or trace symbols. The separate POSIX
diagnostic module is never installed; all three package/member checks pass.
Existing test inventories and every normal host bound remain unchanged.

Records are64 bytes, versioned/little-endian, at most64 per writer (4096B/file,
8192B/pair), with a reserved saturation slot. Helper/core files are exclusive,
private, outside disposable/save paths; only the child descriptor crosses exec.
Hooks observe existing raw Up/B returns and completed/null polls without extra
callbacks; unavailable states and clock errors remain explicit. Parser/collector
report absent/empty/truncated/invalid/unfinished/complete/saturated states and
the last valid record. Parent/child identity and valid clocks alone do not prove
same-host/clock provenance; fresh descriptor ownership supplies that provenance.
An unfinished writer's health is unknown. Valid prefix collection never proves
missing input or a clean completion. Known write failures invalidate collection.

Exact source/test/documentation paths changed by this batch:

- `libretro/libretro.c`, `libretro/CMakeLists.txt`.
- `tests/libretro/tests.cmake`, `tests/libretro/input_trace.h`,
  `tests/libretro/input_trace.c`, `tests/libretro/test_input_trace.c`,
  `tests/libretro/input_trace_host.c`, `tests/libretro/input_trace.cmake`.
- `tests/retroarch/keyboard_preflight.c`, `tests/retroarch/input_trace_helper.h`,
  `tests/retroarch/test_input_trace_helper.c`, `tests/retroarch/run_retroarch.cmake`.
- `tests/policy/tests.cmake`, `docs/testing.md`.

The existing offline preflight consumer gains POSIX protocol/helper/frontend
controls when the diagnostic target exists; other platform registrations and
native/WASI status assertions remain intact. Synthetic tests cover held/released/
changed, wrong-port/button, no-run/null callbacks/no-instance/late-poll/nonordinary
states, clock/write/short-write faults, limit neighbors, malformed/truncated/
omitted records, descriptor collisions/permissions/symlinks, forced child kill,
and collection through the actual wrapper before its launch-error handling.
Normal/diagnostic callbacks, order, return values, frames and audio agree.

The sole typed independent source review resolved `gpt-6-luna`/`xhigh` and returned
two blockers: an unfinished helper prefix could pass, and a late core write error
could be ignored. One correction round requires helper END/saturation and rejects
the captured writer-failure diagnostic. The actual collector rejects the helper
prefix; a real EBADF after core INIT leaves exactly one record but the wrapper
rejects it. Reintroducing either old guard causes its new consumer to fail.
No second review or optional specialist expansion. The original review report
and correction evidence remain in private scratch; this outcome records both
findings and their verified disposition rather than relabelling the review PASS.

All following local evidence has run URL **N/A-local**; raw logs are scratch only.

| Command | Exit | Establishes |
|---|---|---|
| `python3 /private/tmp/muhtari-tracer-preflight.py` | 0 | Exact source/tree/index/worktree baseline; fresh full original receipt/service checks before implementation |
| `cmake --preset ci` and focused warning-clean target builds | 0 final | Separate diagnostic and actual standalone mocked consumers compile |
| `ctest --preset ci -R '^policy[.]retroarch[.]preflight_api$' --output-junit /private/tmp/muhtari-tracer-review-fix.xml` | 0 | Protocol, synthetic comparisons, kill/collection and review corrections |
| `ctest --preset ci -R '^(libretro[.]\|policy[.]retroarch[.])' --output-junit /private/tmp/muhtari-tracer-adjacent.xml` | 0 | Existing adjacent libretro/policy functionality |
| `python3 /private/tmp/muhtari-tracer-workflow.py` invokes unchanged `cmake --workflow --preset ci` with existing four-worker FIFO jobserver | 0 twice | Complete ordinary/release suites and three packages; final corrected source about61s |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | Exact677 ordinary tests, once each |
| Same checker with `-DJUNIT=build/ci/junit-release.xml -DLABELS=release` | 0 | Exact6 release tests, once each |
| `python3 /private/tmp/muhtari-tracer-counterfactuals.py` | 0 | Four compiled omitted-poll/state, extra-callback and wrong-value observers reject1; baseline without hooks1/current diagnostic0 |
| `python3 /private/tmp/muhtari-tracer-review-controls.py` | 0 | Old helper-prefix guard and omitted writer-error guard each reject1 under new consumers |
| `scripts/hygiene.sh --tree`; explicit clang-format21 `--dry-run --Werror` on all owned C/header files; `git diff --check` | 0 each | Hygiene/format/diff clean, including new untracked sources |
| `python3 /private/tmp/muhtari-tracer-final.py` | 0 | Fresh receipts, five repeated synthetic observations, normal module separation, protected hashes/index/source reconciliation |
| `python3 /private/tmp/muhtari-tracer-protected-docs.py` | 0 | Docs outside edited host section reconstruct exactly to baseline; prior WASI description retained |

Initial configure1 exposed registration order; initial build1 exposed Darwin
feature visibility/prototype requirements. Initial protocol test8 used a valid
nearby nanosecond as an invalid fault. These were corrected without weakening
contracts. The independent review's two blockers and correction round are
preserved above. No full-workflow failure occurred in this batch.

Five paired elapsed measurements of the synthetic `retro_run` workload ranged
from normal16.131–17.496ms and diagnostic16.523–17.512ms. Paired diagnostic-minus-
normal deltas ranged−0.139 to+0.698ms, median−0.014ms. This noisy local sample
establishes neither an improvement nor a real-time/maximum filesystem-latency
guarantee; setup/deinit are outside the measured run interval.

Fresh read-only GitHub receipts before implementation and after final checks
match the full original [run41158640754](https://github.com/szTheory/muhtari/actions/runs/41158640754):
sourcecee6dea491918282d034f8c6e9a90087d8213439, attempt1,20jobs/191steps/20artifacts,
terminal failure. Required CMake aggregate audit remains1; all job/step/artifact
outcomes are retained, including advisory WASI. Service operational, prior
incident resolved, no open phase PR or active CI. HEAD/tree/index and all
unrelated baseline bytes remain exact. Prior dirty WASI source/policy hunk and
docs description are preserved. No staging/commit/publication/remote mutation,
actual helper/platform API/frontend/input/content launch, fetch or renewed
allowance occurred. Local synthetic processes and the existing mocks only.

Local guardrail: target/adjacent/full checks pass; explicit compiled faults and
baseline/current differential reject/accept as expected; behavior is added only
to the diagnostic path. Formal Stryker is skipped (absent/unconfigured for C).
This accepts the local diagnostic batch only, never the overall host plan.
Accepted0/4, PLAY-03 pending, downstream blocks, failed history and every spent
live allowance remain unchanged. No owner verification/acknowledgment is needed.

Stop this GSD step. Context may be cleared. Exact next separate command:
`$gsd-debug continue phase12-packaged-input-tracer` for read-only preparation
of the smallest concrete hosted diagnostic proposal. The agent owns fresh
source/service/full-receipt checks. Any actual instrumented hosted run,
publication or content/input action needs a new explicit scope decision;
invocation restores no allowance and promotes no phase.
