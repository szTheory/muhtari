---
status: local-repair-and-approved-cap-amendment-verified-host-proof-pending
date: 2026-10-30
scope: Missing held capture diagnosis and future Windows calibration preparation only
---

# Held capture and Windows producer evidence

Latest continuation: [approved local WASI transport batch](#approved-local-wasi-transport-batch-2026-10-31).
The approved repair and local checks are finished, source uncommitted. Complete
C-mock WASI execution still lacks a local linker; host proof remains halted.
Earlier awaiting-local-decision and live-proposal statements are historical.

The missing F8 capture is real. The available evidence narrows it to the frontend
keyboard/capture boundary but does not prove which event-consumption condition
failed. Do not retry the same schedule. A future local-only change should make
capture acknowledgment explicit before seeking another live allowance.

All previous live allowances remain spent. No source, workflow, deadline, pin,
permission, commit, push, dispatch, rerun, cancellation or content launch occurred
in this continuation. Accepted0/4 and the dependent-plan halt remain unchanged.

## Exact latest receipt

[Run41133992538](https://github.com/szTheory/muhtari/actions/runs/41133992538),
attempt1, source186c0cc4b39c12fb149ddc0ef3f43897fba55865, remains completed/failure.
Fresh run/attempt/CLI/job/artifact responses reconcile all20 terminal jobs and
all21 unchanged, source-bound, nonexpired artifacts. Native current/base
677+6 production and uploads pass. The required aggregate fails solely on
RetroArch; WASM's expected127/runtime1 child-failure result stays separately
visible and advisory.

Official health was refreshed from the primary summary/incident APIs immediately
before handoff at22:54:48Z: All Systems Operational; Actions/Webhooks/Pages operational;
incidentpwxsjgnvrk6b resolved. Neither current health nor the historical incident
explains the missing capture.

## What the host evidence establishes

[Host job124346024270](https://github.com/szTheory/muhtari/actions/runs/41133992538/job/124346024270)
logs permitted posting, one child, Cocoa input, MFi controller input,600frames
and the terminal released screenshot. The actual retained config contains
`input_player1_up="up"`, `input_player1_b="z"`,
`input_screenshot="f8"`, the isolated screenshot directory, and vsync.
MFi is the controller driver, not evidence that Cocoa keyboard input is absent.

The probe shell finishes its CMake invocation successfully, then fails the
raw-held-file checks before generating runner expectations or invoking either
pixel judge. The literal unmatched glob has one element, so the count check alone
passes; `test -f` rejects the absent held image. The upload searches both frame
and raw-shot locations and reports exactly one file. The source-bound frames
artifact contains only the released terminal PNG, whose released predicate passed
in the prior audited run. That image cannot establish any earlier held input.

The helper repeats Up/Z keydowns during startup, but posts F8 down only at tick20
and up at tick21: nominally about2.0/2.1seconds after fork. It has no readiness or
capture acknowledgment. Reaching tick50 and returning0 establishes that the
scheduled calls were traversed; `CGEventPostToPid` returns void and supplies no
frontend-consumption receipt. The helper logs only Up/Z ticks0/50, not F8 times.
CMake captures child stdout/stderr and prints it later, so the GitHub timestamps
on those printed lines do not measure initialization or key delivery.

The complete shell interval is about14.47s and the frontend reports10seconds of
content. This is consistent with nontrivial startup/teardown overhead; it cannot
locate startup relative to the F8 pulse and is not proof of an early event.

## Static checks and remaining hypotheses

Only the existing released binary was inspected; no upstream emulator source was
opened. Its full executable SHA256 remains
ed90b54434a2899de0ddbfed59f335255fb462c8691bd970a1a761ebb5656d65,
matching the exact pinned DMG's previously verified executable.

| Candidate | Evidence and disposition |
|---|---|
| Wrong helper keycodes | Eliminated for the arm64 native mapping: SDK constants are Z6/F8=100/Up126; exact binary's native-to-HID table maps them to29/65/82. |
| F8 occurs before a usable window or polling | Leading order-race candidate, unconfirmed. The one100ms pulse has no readiness guard. Exact binary's Cocoa event loop drains queued events; its window handler updates key state on down/up, and its input-state path reads that state. A queued down/up can therefore leave no held state for a later poll. No observation proves that happened in this run. |
| PID posting does not reach the active/key window | Still possible. Permission preflight and void posting do not establish application/window routing. No focus/window or event acknowledgment was recorded. |
| Screenshot hotkey is gated, or effective configuration differs | Still possible. The written configuration and documented F8 binding are correct; effective runtime hotkey/game-focus/override state was not dumped. No screenshot-request failure is logged beyond the automatic exit capture. |
| Screenshot was silently omitted by artifact wiring | No supporting evidence: fail-closed file check, actual config and both raw/frame upload patterns agree. A capture elsewhere or an ignored effective path remains unobserved, not disproved globally. |

This is a suspected concurrency/order failure across code and environment, with
configuration alternatives retained. No failing/passing coverage spectrum or
record-replay exists; live repetition is unauthorized. The AND-gate is relevant:
posting permission, usable routing, an observable key state and an eligible
capture window must coincide. No root-cause fix is claimed.

Primary public references: [Libretro input and controls](https://docs.libretro.com/guides/input-and-controls/)
documents F8 and hotkey-enable gating;
[driver distinction](https://docs.libretro.com/guides/input-controller-drivers/)
separates keyboard and controller drivers;
[Apple Quartz events](https://developer.apple.com/documentation/coregraphics/cgevent)
describes target-process dispatch. The local Apple SDK supplies the void posting
signature, permission-check semantics and virtual-key constants. These are API
contracts, not proof of this host's event receipt.

## Future Windows calibration

The latest exact-source successful ARM64 job lasts2207s, suggesting74m if viewed
alone. That is not the slowest relevant complete sample. Reusing72 already saved
latest-job lists plus the latest run supplies43 complete successful arm64 and46
x64 executions. Select the latest40 successful executions per lane by actual
completion time, count copied rerun peers only once at their original execution,
and require current workflow/inventories, independent-base quality production,
all three uploads, identity and whole-job success. The28 uninspected older run
metadata records predate both selected boundaries; no new exhaustive audit was
needed.

| Lane | Slowest selected successful whole job | Rule arithmetic | Recommendation for a separately authorized local change |
|---|---|---|---|
| Windowsarm64 | [40783431377/job123150624087](https://github.com/szTheory/muhtari/actions/runs/40783431377/job/123150624087),2324s | ceil(2×2324/60)=78m | Arm64-only68→78m; preserve x64 and every test/scale/producer. |
| Windowsx64 | [41099366366/job124213483752](https://github.com/szTheory/muhtari/actions/runs/41099366366/job/124213483752),1473s; original execution copied into attempt2 | ceil(2×1473/60)=50m | Existing68m already exceeds this margin; leave unchanged in this narrow recovery. |

The arm64 maximum's direct log proves662/6/662/6 suites, three packages on each
side, complete quality production and three uploads. Actual checkoutf302605 is
the PR merge of head2ba227ae and based2ec6bb; its tree equals the compared head
tree. Native job behavior and quality producer are unchanged except deadline
provenance/comments. Native presets differ only in a library comment. Maximum
and latest logs both report MSVC19.51.36260.0. Image revisions differ:
20261022.194.1 versus latest provisioner20261025.196.1, with the latter's step
environment reporting20261026.197.1; preserve both values rather than resolving
the mismatch by assumption. Current workloads expanded662→677 cases.

Latest arm64 current/base ordinary777.56/1318.71s and releases0.76/1.32s complete;
latest x64 ordinary390.80/872.34s and releases2.99/1.17s complete. Historical78m
restores the existing measured rule's margin, provisionally for the expanded
workload; it guarantees no future duration and explains no slowdown. No deadline
was changed here. A future full matrix using arm64-only78m would add10 summed
configured runner-minutes to the previous319m envelope, yielding329m; source,
closure, actual inputs and queue-inclusive window still need a fresh proposal.

## Narrow next recommendation

Prepare a separately authorized **local-only** correction through the existing
helper, wrapper and policy tests, before proposing any live work:

1. Make the isolated raw-shot directory an explicit probe input. Retain the
   existing45s owned-child/50s wrapper/600frame bounds and exactly-one held image.
2. During the existing held window, repeat F8-down while waiting for the first
   raw capture; then send F8-up and stop posting F8. Fail closed before release
   if no capture or multiple captures exist. Keep Up/Z release and final released
   screenshot proof. Capture acknowledgment closes the startup race candidate
   without assuming a fixed later delay; if PID routing is ineffective it still
   stops visibly. Existing pixel judges remain authoritative.
3. Record monotonic phase/event/capture times and cover delayed readiness,
   missing/multiple acknowledgment, early child exit, forwarded argv and cleanup
   with the existing deterministic mock/wrapper consumers. No new event tap,
   permission request, global-input target, host pin or automation framework.
4. Apply arm64-only78m provenance if authorized, with the existing timeout and
   six-lane expression pass/fault checks. Batch local verification/review. Only
   after that result is concrete should a new live source/budget be proposed.

This is a proposed finite experiment protocol, not an established input fix. A
received held capture plus subsequent released predicate would discriminate it;
another missing capture would preserve the halt and require routing evidence.
Doing nothing safely preserves the halt. Reopening the host pin has a larger
proof and provenance cost and lacks a newly established released alternative.

## Command receipts

Raw responses, logs and disassembly remain scratch-only in
`/tmp/muhtari-held-diagnosis.lOayAx`; historical files were read without rerunning
their mutation-capable scripts.

| Command | Exit | Evidence |
|---|---:|---|
| `python3 /tmp/muhtari-held-diagnosis.lOayAx/refresh.py` | 0 | Fresh seven read-only health/run/attempt/jobs/artifact/CLI calls each0; exact latest receipts above. Initial local parser1 on incident envelope was corrected against the same saved response without repeating requests. |
| `cmake -DINPUT=/tmp/muhtari-held-diagnosis.lOayAx/view.json -DEXPECTED_SHA=186c0cc4b39c12fb149ddc0ef3f43897fba55865 -P tools/check_ci_run.cmake` | 1 | All20 jobs visible; only required-aggregate failure blocks, advisory WASM remains visible. |
| `gh api --allow-escape-sequences repos/szTheory/muhtari/actions/jobs/123150624087/logs` | 0 | Complete2324s maximum's direct log, linked above. |
| `python3 /tmp/muhtari-held-diagnosis.lOayAx/windows_review.py` | 0 | Last40 selection, complete producer/upload receipts, copied-peer origin, arithmetic, direct current/max logs and source comparability. An initial comment-sensitive comparison1 was corrected to compare behavior separately from deadline/comments. |
| `python3 /tmp/muhtari-held-diagnosis.lOayAx/static_keys.py` | 0 | Exact released binary digest and native key table, N/A-local. |
| `xcrun llvm-objdump --macho --arch=arm64 --disassemble --dis-symname SYMBOL /Applications/RetroArch.app/Contents/MacOS/RetroArch` | 0 | Targeted Cocoa window/event/input-state symbols; N/A-local. Initial tool-syntax probes failed before successful bounded symbol inspection. |
| `python3 /tmp/muhtari-held-diagnosis.lOayAx/refresh.py --fresh` | 0 | Final22:54:48Z primary health and exact latest receipts unchanged; required audit remains1. |
| `git diff --check` and frozen HEAD/workflow/original-three-edit hash assertions | 0 | Only owned report/checkpoint changed here; all preexisting edits preserved; N/A-local. |

Parent owns shared STATE/LESSONS/canonical updates and any genuinely new authority
decision. No owner verification or routine acknowledgment is needed to preserve
the stop. Context may be cleared after handoff. Exact next command:
`$gsd-debug continue phase12-packaged-input-tracer`; the command alone renews no
source, deadline, permission, pin or live allowance.

## Local continuation: implemented and checked, support-cap decision blocked

The owner said exactly: **"I accidentally quit my terminal. Please continue."**
The parent interpreted this as continuation of the concrete previously presented
local-only acknowledgment/timing/tests and Windows-arm64-only68→78m correction.
It is not recorded as an explicit yes, a new live allowance or permission to
change another contract. Only that local source/workflow/deadline restriction
was superseded. Every previous failed receipt, accepted0/4 and dependent-plan
halt remains. No actual keyboard helper, event posting or RetroArch/content
process was executed. No commit, push, dispatch, rerun, cancellation, pin,
permission change or phase/plan promotion occurred.

The local implementation is **not ready for acceptance**. Its behavior controls
and full local workflow pass, but the existing300-line hosted-support contract
rejects the347-line implementation. The cap was not changed, bypassed or moved.
This is a real separate decision, not owner verification or a routine
acknowledgment. The parent owns that decision and shared bookkeeping.

### Concrete local change

- `tests/retroarch/keyboard_preflight.c`: `--probe RAW_SHOTS FRONTEND ARGS...`
  requires an initially empty capture directory, posts F8-down repeatedly while
  holding Up/Z, observes the first raw capture strictly before the existing5s
  cutoff, sends F8-up once and stops F8. Missing, multiple or disappearing
  captures fail with owned-child cleanup. Up/Z release remains at5s; the helper
  retains45s and450-iteration bounds, one child and exit/error checks. Monotonic
  milliseconds identify initial events, capture acknowledgment, release and
  child exit despite buffered frontend output. Capture observation is not pixel
  acceptance or frontend event-consumption proof.
- `tests/retroarch/run_retroarch.cmake` and `.github/workflows/ci.yml`: pass the
  explicit isolated raw directory before the frontend argv; resolve/refuse
  incorrect or terminal-overlapping paths before disposable cleanup. Retain the
  original50s wrapper,600frames, one launch, final exactly-one count/regular-file
  shell consumer, both independent pixel judges and scoped stop. A duplicate
  new wrapper count was removed in favor of testing that existing consumer;
  the original config/template remains unchanged.
- Existing `tests/policy/retroarch_preflight.c`, its CMake consumer and
  `tests/policy/tests.cmake` now exercise delayed readiness, cutoff neighbors,
  absent/multiple/disappearing acknowledgments, initial directory I/O failures,
  early child exit, forwarded probe argv, failed exec/no-child-input,
  monotonic-vs-tick release timing, clock failure/frozen-clock finite cleanup,
  first-release-event failure and every earlier adverse control. The Unix
  fixture executes the actual wrapper and actual workflow count/file recipe;
  invalid raw paths retain a sentinel before cleanup. The common non-Unix
  policy retains its earlier mutation checks plus missing-raw-input rejection.
- Only Windows ARM64 changes68→78m from the complete2324s historical maximum
  among40 successful selected executions under DEC.47. The expression evaluates
  x64=68m, Intel=37m, all other lanes=19m; scales, producers and joins are
  unchanged. Historical662/6 work differs from current677/6; expanded-workload
  margin remains provisional. Future329m configured-envelope arithmetic is
  proposal information only, with no new spending/window/content authority.
- `docs/testing.md` describes the protocol while retaining the unchanged shared
  50s launch bound,300-line contract, candidate-only proof and published-byte
  replay boundary. Its unchanged40960-byte budget passes at40957bytes.

### Verification and bounded review

All raw logs, mock binaries, generated owned media, scratch source counterfactuals
and command receipts remain in `/tmp/muhtari-capture-local.8_5_au7r`.
`baseline.json`, `checks.json`, `final.json` and `review.md` retain details.
No log or binary was added to the tree. Every receipt below is **N/A-local**.

| Command | Exit | What it establishes |
|---|---:|---|
| `python3 /tmp/muhtari-capture-local.8_5_au7r/local_checks.py` | 0 | All three CI standalone compiler consumers, owned input fixture/controls, native helper compile only, actual helper/wrapper/policy controls, timeout provenance, six-lane actual expression and old/wrong-runner/wrong-deadline faults, actionlint, docs/format/hygiene/diff. Each subcommand is recorded in scratch `checks.json`. |
| `cc -std=c17 -Wall -Wextra -Werror tests/retroarch/keyboard_preflight.c -framework ApplicationServices -o /tmp/muhtari-capture-local.8_5_au7r/keyboard-native` | 0 | Native API compile/link only; executable never run. |
| `ctest --test-dir build/ci -R '^(policy[.](retroarch[.]|ci_run[.]controls)|input[.]proline_one_button|diag[.]battery)' --no-tests=error --output-on-failure --output-junit /tmp/muhtari-capture-local.8_5_au7r/focused.xml` | 0 | Existing relevant consumers and retained independent product predicates. Focused JUnit stays separate from full inventories. |
| `cmake -DROOT="$PWD" -DWORK=/tmp/muhtari-capture-local.8_5_au7r/nonunix -P /tmp/muhtari-capture-local.8_5_au7r/nonunix.cmake` | 0 | Common policy with Unix-only execution disabled still rejects applicable producer/guard/path mutations. Not a native MSVC verdict. |
| Scratch unchanged-baseline helper with current consumer; single-pulse/no-F8-up/no-release variants; actual expected-code/marker consumer for wrong-argv/child-fallthrough/child-input variants | 1 each | Valid consumer0; all seven counterfactuals compile0 and reject1. Baseline behavior fails while corrected behavior passes, without changing the shared working tree. Exact argv/commands are in `checks.json`. Formal Stryker skipped: absent/unconfigured in this C project. |
| `python3 /tmp/muhtari-capture-local.8_5_au7r/workflow.py` invoking `cmake --workflow --preset ci` | 0 | Fresh full677 ordinary tests, packaging and6 release tests, using the inspected existing four-worker FIFO convention. |
| `cmake -DJUNIT="$PWD/build/ci/junit.xml" -DINVENTORY="$PWD/tests/inventory/ci.txt" -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | Every677 required ordinary test ran exactly once. |
| Same exact consumer with `-DJUNIT="$PWD/build/ci/junit-release.xml" -DLABELS=release` | 0 | Every6 release test ran exactly once. |
| `python3 /tmp/muhtari-capture-local.8_5_au7r/final_checks.py` | 0 | Final inventories, docs budget, format, hygiene, diff and protected baseline assertions; separately requires the cap to reject rather than concealing it. |
| `test "$(cat tests/retroarch/* | wc -l | tr -d ' ')" -le 300` | **1** | Existing required hosted-support cap rejects347lines; remains blocking. |

Initial local setup attempts returned1 for a scratch-baseline path mismatch,
2 for invoking the owned input consumer without its case, and1 for unavailable
Windows-header/driver-mode clang-cl probes. These were not native MSVC results.
Native compile and actual case invocation above pass after correcting the local
commands. New prose initially failed the unchanged document budget; condensation
retains both required proof/timeout boundaries and the final budget passes.
The original helper counterfactual initially lacked the test's directory header
declarations; the fixture now owns its explicit mock interfaces and the original
helper compiles0/rejects1. No product assertion or warning policy was weakened.

The sole typed `gsd-code-reviewer` used CLI-resolved `gpt-6-luna`/`xhigh`, with
foreground waits through its terminal result. It found no runtime/security
defect and three warnings: the cap, a removed candidate-only proof sentence and
the removed shared50s launch statement. Both documentation warnings were fixed
in one targeted round before the fresh full workflow; final budget and explicit
boundary assertions pass. No second review or recursive fan-out occurred.
The cap warning remains, so this is not a reviewPASS/source-readiness claim.

### Concrete remaining boundary

The actual count is comparator97 + helper105 + wrapper101 + report14 + config30
=347, versus the original296. Comparator/report/config are unchanged. The
helper adds38lines for acknowledgment, directory errors, monotonic evidence and
bounds; the wrapper adds13 for explicit isolated input and pre-cleanup safety.
The reviewer found no safe within-cap simplification preserving those controls.
Removing duplicate final count/default/config work already reduced356→347.
Packing code, relocating host automation, dropping the original pixel/media/
cleanup controls or replacing300 with an arbitrary new quota is not authorized.
The cap comes from DEC.56 and the concrete D-06/task2 contract, motivated by a
sibling's unbounded host-harness growth; ENGINEERING §11 requires a separate
evidence-backed boundary decision rather than silently waiving it.

HEAD, original three planning-edit hashes and the existing live proposal remain
byte-identical to the snapshot. This worker did not edit STATE, the canonical
summary, LESSONS, config, decisions, handoff rules or AGENTS. Parent-owned edits
remain preserved. The original hosted missing-capture cause and routing remain
unproved; no hosted held/released acceptance, slowdown cause, phase-versus-main
performance or phase completion follows from these local checks.

The parent acts next on the specific cap decision; no owner verification or
routine acknowledgment is needed. Context may be cleared after the parent
handoff. Exact next separate command:
`$gsd-debug continue phase12-packaged-input-tracer` — **read-only live-proposal
preparation only**, with the cap prerequisite still unresolved. The agent owns
any service-health/receipt checks needed then. The command itself grants no
source publication, cap change, content process or renewed live allowance.

### Proposed separate cap decision, not applied

Recommend retiring **only the numeric300-line hosted-support ceiling**, without
a replacement quota. Keep the two existing DMA/input and battery cases, all
independent pixel/media judges and fault controls, isolated paths, one owned
keyboard child, existing held/process/frame limits, pin/permission policy and
in-process semantic coverage. Ordinary architecture review under ENGINEERING
§11 remains mandatory for ownership, change locality, control flow, independent
tests and operational cost; support automation stays in its counted directory.

The concrete amendment would update DEC.56, HOSTS-AND-INTEGRATION's two references
and the current D-06/task2 acceptance forward-looking, preserving historical
failed receipts. In ci.yml, replace only the300-line rejection with ordinary
`wc -l tests/retroarch/*` reporting. Update the corresponding testing sentence;
add no registry, generalized host framework, new case or dependency. Verify the
actual changed shell consumer reports the real directory and fails on absent
inputs, retain all existing fault consumers, and run normal affected/local
checks. This authorizes no commit, publication or hosted process by itself.

The original ceiling guards against the sibling's7706-line harness growth; the
current review found no corresponding ownership/runtime defect in this347-line
consumer. Required acknowledgment/path/error/time logic accounts for the small
increase, and representative fault tests demonstrate what dropping it loses.
The numeric ceiling itself detects neither missing capture acknowledgment nor
unbounded child lifetime. Review and the existing direct process/capture controls
address those concrete risks, while losing the ceiling removes an automatic
source-size stop; future review must still reject generalized harness growth.

Doing nothing preserves the current halt safely. Raising the quota to350/400
has no demonstrated consumer capacity and merely moves the same cliff. Packing,
relocation or dropped safety checks would hide the burden or lose fault
detection. New evidence of a materially simpler design with the same controls,
or a concrete maintenance/operating-cost defect in this implementation, would
overturn this recommendation. Until the owner decides, the original gate remains
binding and rejects347/300; no readiness, acceptance or live authority follows.

## Approved numeric-cap retirement: local verification complete

The owner answered **"yes"** to the concrete preceding proposal to retire only
the numeric support-code gate, preserving reported totals, two cases, existing
process limits, independent pixel/fault checks and ordinary architecture review.
This supersedes the cap blocker above; the historical347/300 rejection remains
failed evidence. It supplies no replacement quota, commit/publication authority,
new live allowance or phase acceptance.

The sole workflow change relative to the reviewed local repair is replacement
of the numeric rejection with `wc -l tests/retroarch/*`. DEC.56, both host-reference
clauses, ENGINEERING §11 and active D-06/task2 clauses now reflect the approved
scope. The downstream coverage-table reference is synchronized; initial research
has an explicit amendment note rather than rewritten historical observations.
Testing docs retain50s and candidate-only proof boundaries. All host-support
bytes remain unchanged at347lines; runtime ownership/bounds/judges remain intact.

The existing `policy.retroarch.preflight_gate` extends its actual workflow
consumer: reporting301lines succeeds, absent files fail, and restoring the old
numeric gate is rejected. Its common checks also pass with both Unix-execution
branches disabled; this is portability control, not a new native MSVC verdict.
No new framework, registry, quota, host case or dependency was introduced.

Evidence is **N/A-local**; raw commands/logs remain in
`/tmp/muhtari-cap-amendment.1L3jBd`:

| Command | Exit | Result |
|---|---:|---|
| `python3 /tmp/muhtari-cap-amendment.1L3jBd/workflow.py` invoking `cmake --workflow --preset ci` | 0 | Fresh full677 ordinary tests, three packages and6 release tests; existing four-worker FIFO convention. |
| Exact `check_junit.cmake` consumer, ordinary and release filters as above | 0 each | Required677/6 tests each ran once. |
| `ctest --test-dir build/ci -R '^policy[.]retroarch[.]preflight_gate$' --no-tests=error --output-on-failure` | 0 | Actual reporting growth/missing-input/restored-cap faults and retained input/preflight controls. |
| `cmake -DROOT="$PWD" -DWORK=/tmp/muhtari-cap-amendment.1L3jBd/nonunix -P /tmp/muhtari-cap-amendment.1L3jBd/nonunix.cmake` | 0 | Applicable common mutations still reject with Unix branches disabled. |
| `actionlint -shellcheck= -ignore 'label ".+" is unknown' .github/workflows/ci.yml` | 0 | Existing schema/expression check and inherited label/shellcheck allowances unchanged. |
| `cmake -DDIR=.github/workflows -P tests/cmake/timeouts.cmake` | 0 | Every timeout retains provenance. |
| `wc -l tests/retroarch/*` | 0 | Actual347-line support total reported without a ceiling. |
| `python3 /tmp/muhtari-cap-amendment.1L3jBd/verify.py` | 0 | Workflow differs from the approved repair only by that one reporting line; host/support/runtime bytes, prior config/handoff/live-proposal edits and HEAD preserved. |

An initial scratch assertion expected one Unix guard but the actual existing
consumer has two; correcting that scratch generator allowed the forced-common
check above to run. No repository check was weakened. The relevant `run` shell
contract was checked against [official workflow syntax](https://docs.github.com/en/actions/reference/workflows-and-actions/workflow-syntax#jobsjob_idstepsrun).

The prior bounded architecture/runtime review remains applicable because this
amendment changes no host implementation; its cap warning is resolved solely by
the explicit decision. Actual hosted held/released input and the original
missing-F8 cause remain unproved. The latest hosted failure/advisory result is
unchanged, accepted0/4 and dependent-plan halt persist, and all live allowances
remain spent. Local source and handoff edits remain uncommitted. No content,
permission/pin change or remote mutation occurred.

Context may be cleared after parent handoff. Next separate command:
`$gsd-debug continue phase12-packaged-input-tracer` for **read-only future
live-proposal preparation**, with the agent owning source/receipt/health and
budget checks. Invocation alone authorizes no local commit, publication, hosted
run or content launch; any new live scope must be concrete and separately
approved. No owner verification or routine acknowledgment is needed now.

Final handoff checks: `sh scripts/hygiene.sh --tree`, `git diff --check` and
`cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake` each exit0,N/A-local.
The document consumer reports22files within budget. No further source change
followed the verified full workflow; accepted0/4 and spent live allowances remain.

## Read-only startup and WASI diagnosis after the acknowledgment experiment

The separate continuation on2026-10-31 diagnoses run41150939552 only. No source
repair, executable/content launch, commit, publication, dispatch, rerun or
cancellation is authorized. Two bounded read-only specialists covered the host
boundary and the independent advisory compiler failure; the parent owns this
synthesis and shared bookkeeping. Source, workflow, pins, permissions and runtime
bounds remain unchanged. Existing failed history and accepted0/4 remain binding.

### What is established and what is unknown

The refreshed exact-source/attempt receipt still reconciles all20jobs/191steps
and20artifacts. Sixteen jobs succeed, title skips as allowed, and host, advisory
WASM and required aggregate fail. There is no frames artifact. The existing
required checker still exits1 solely because its aggregate fails on RetroArch.
Official service summary is operational and the earlier incident remains
resolved. Health does not explain this host failure or authorize another run.

The actual helper starts monotonic time before fork. Its no-capture guard at
`tests/retroarch/keyboard_preflight.c:65` precedes the nonblocking child-status
check at70. At5s with no acknowledgment, it can kill/reap the child without first
observing a pending child exit. The last logged F8-down is at4925ms/capture0,
followed by the generic failure shared by directory, clock, event, wait, sleep,
iteration-limit and deadline paths. Timing supports the no-ack guard but cannot
identify the executed branch. The message's45s wording is not a measured45s
expiry. Repeated F8 posting makes a single missed pulse insufficient to explain
the entire current result; it still proves neither input consumption nor capture.

The frontend emits startup/swapchain/draw-observer messages and an audio error
near the end of the merged output. CMake buffers stdout/stderr, so GitHub's later
print timestamps cannot establish first-input-poll readiness or audio causality.
No trustworthy input-readiness receipt exists. Current mock readiness is synthetic:
its own event handler schedules a capture after an eligible post; readiness4.9s
is rejected when acknowledgment reaches the existing5s cutoff. These controls
prove helper behavior, not hosted event consumption. Hotkey gating and effective
configuration/routing remain alternatives. [Libretro's hotkey documentation](https://docs.libretro.com/guides/input-and-controls/#hotkeys)
describes that gating; no GPL emulator source was opened.

The separate WASI defect is reproduced. `tests/policy/retroarch_preflight.c`
declares and defines `keyboard_clock(int, ...)`, while this target's libc uses
pointer-valued `clockid_t`/`CLOCK_MONOTONIC`. An exact-version libc package was
extracted under scratch without installation. Existing Clang with
`--target=wasm32-wasi`, that sysroot, strict repository warnings and
`-fsyntax-only` reproduces the same three errors in the complete original mock.
The full corrected scratch mock passes target and native macOS syntax checks.
No generated executable was run. This local compile result is not hosted
acceptance and does not resolve the older advisory child-exit runtime failure.

### Concrete local-only recommendation, awaiting authority

1. In `tests/policy/retroarch_preflight.c`, keep the current missing-clock fallback
   and use a private `keyboard_clockid_t`: `int` when `CLOCK_MONOTONIC` is absent,
   otherwise the header's `clockid_t`. Move the forward declaration below that
   conditional and use the alias in both signatures. Directly using `clockid_t`
   everywhere passes WASI but fails the representative no-POSIX-clock header
   fixture; forcing/casting the target clock identifier would hide the mismatch.
2. In `tests/retroarch/keyboard_preflight.c`, report the actual terminating
   condition with the last valid monotonic elapsed time, capture count, relevant
   saved error and child status only when observed. Distinguish guard expiry,
   capture-directory/count faults, clock/event/wait/sleep errors, child completion
   and iteration/deadline limits. Retain existing predicate order, return statuses,
   owned-child cleanup and five/45/50-second bounds; an unobserved child status
   must stay explicitly unknown.
3. Extend the existing deterministic mock/standalone consumers in
   `tests/policy/retroarch_preflight.c` and its existing CMake driver as needed
   to assert representative valid and distinct fault reports without dropping
   behavioral assertions. Update only directly affected testing documentation.
   Check every standalone compiler consumer, current native/fallback and
   WASI compiler consumers. Run affected existing tests and the full local CI
   workflow with focused JUnit kept separate from full inventories. A real
   MSVC verdict and complete WASI runtime acceptance remain distinct from local
   representative compile evidence.

This local batch fixes a reproduced portability error and makes future host
failures distinguishable. It cannot recover the missing historical branch or
establish frontend readiness. A future no-ack report alone still would not prove
why capture failed, and would not justify a longer window or another retry.
Readiness-relative scheduling needs independent evidence of input polling first;
window creation or a startup log is insufficient. Changing focus, drivers, pins
or lifetimes now introduces another condition without a discriminating receipt.
Doing nothing preserves the stop but leaves the deterministic compiler defect and
generic reporter in place. The smallest useful batch above is recommended.

Adversarial synthesis: a named directory/event/clock failure would overturn
cutoff attribution; an independently timed late input poll would support startup
preemption; proven polling well before cutoff without capture would favor
routing/hotkey gating. Exact reason reporting has bounded diagnostic value, not
a claim to solve startup or deliver input. Stop after locally verifying this
batch if authorized. Publication and any actual host experiment require a new
concrete scope decision; no such allowance is requested or renewed here.

### Diagnosis receipts

| Command | Exit | What it establishes |
|---|---|---|
| `python3 /tmp/muhtari-startup-diagnosis.igtqEs/receipts.py` | 0 | Fresh connector job/artifact responses match the saved complete attempt; CLI confirms source/attempt/terminal status, remote head and source workflows; operational/resolved health; existing required audit remains1. The earlier snapshot comparison correctly detected three planning files reconciled by the prior parent handoff; those historical differences are excluded, while remaining protected inputs match. |
| `cmake -DINPUT=/tmp/muhtari-startup-diagnosis.igtqEs/run.out -DEXPECTED_SHA=26c8268f9c56db39308287eaceb935e97eaf3a77 -P tools/check_ci_run.cmake` | 1 | Actual required aggregate remains failed; not a new successful acceptance. |
| `python3 /tmp/muhtari-wasi-clock.SSDai9/check.py` | 0 | Original complete WASI mock compile1 with exactly three errors; corrected WASI/native/fallback compile0; signature-only fallback fault1; actual native standalone helper syntax0; scratch source-copy identities match. Exact compiler arguments/exits are in scratch `receipts.json`. |

Hosted evidence: [run41150939552](https://github.com/szTheory/muhtari/actions/runs/41150939552),
[host job](https://github.com/szTheory/muhtari/actions/runs/41150939552/job/124397648612),
[WASI compiler job](https://github.com/szTheory/muhtari/actions/runs/41150939552/job/124397648745).
Local compiler/bookkeeping checks have run URL N/A-local. Raw logs, responses,
package headers and compiler output stay in scratch, never committed logs.

Diagnosis is finished within the authorized read-only scope. Waiting for the
owner's decision on the concrete local-only batch above; no routine owner
verification is requested. Context may be cleared. Exact next separate command:
`$gsd-debug continue phase12-packaged-input-tracer`. Without explicit approval
of the local batch it preserves the stop and does not authorize source repair.
All prior live/publication allowances remain spent; no archive or phase promotion.

Final diagnosis handoff checks: `python3 /tmp/muhtari-startup-diagnosis.igtqEs/final_checks.py`,
`sh scripts/hygiene.sh --tree`, `git diff --check`,
`cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake` and the existing GSD
`query summary-extract` consumer each exit0,N/A-local. Exactly five existing
planning records changed in this invocation; all source/workflows/ROADMAP,
protected owner edits, HEAD and empty index are unchanged. The summary parser
still reports no completed requirements. Package control metadata independently
matches hosted `wasi-libc` version0.0~git20250726.3f7eb4c-4; native syntax checks
use the existing compiler, not an installed replacement toolchain.

## Approved local clock-type and failure-reporting batch

Owner approval in the continuation: “and yes i approve the batch i approve ur
recs.” This authorizes the concrete local-only recommendation above, through
implementation, local tests and bounded review. It does not authorize a commit,
push, dispatch, rerun, cancellation, actual RetroArch/content launch, new runtime
window or phase acceptance. Earlier awaiting-local-approval text is historical.

Only `tests/retroarch/keyboard_preflight.c`,
`tests/policy/retroarch_preflight.c` and `docs/testing.md` change for this batch.
The mock now uses private `keyboard_clockid_t`, preserving the header's type
when available and the existing integer missing-clock fallback. The real helper
names each stopping condition with last valid elapsed/capture observations,
saved relevant API errno and raw child status only if its ordinary poll observed
completion. Cleanup reaping keeps that status unknown. Non-probe modes report
capture observations as unavailable. Capture enumeration preserves a read error
even if closing the directory changes errno.

The existing deterministic consumer asserts valid completion and distinct
clock/fork/wait/event/sleep/directory/count/lost-capture/acknowledgment/deadline/
iteration outcomes. Simultaneous faults establish that guard-before-wait and
total-deadline-before-directory order remain unchanged. Original return values,
event schedule, five/45/50-second bounds, owned-child cleanup and all workflow,
pixel, pin and permission controls remain. No registration/inventory growth,
dependency or new check framework was needed. Docs remain within40KB.

One bounded independent review found no blocking source, portability, ownership,
control-flow or documentation defect. The parent inspected the changed boundary
and synthesized this local verdict; no additional audit/review loop was started.

| Command | Exit | What it establishes |
|---|---:|---|
| `python3 /tmp/muhtari-clock-diagnostics.HhNyhn/check.py` | 0 | Fresh actual-source strict native/fallback mock builds and valid/fault executions; actual-source WASI compile-only against the previously verified exact target headers; actual native helper link-only; every workflow standalone diagnostic/comparator/input consumer link-only; existing focused policies; formatter/diff/docs budget. Exact compiler commands and exits in scratch `receipts.json`, protected source hashes in `source.json`; focused JUnit stays separate. |
| `python3 /tmp/muhtari-approved-local-workflow.py` invoking `cmake --workflow --preset ci` | 0 | Fresh full local ordinary suite, production three-ZIP packaging and release tests, using the existing measured four-worker FIFO jobserver convention. |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | Every677 required ordinary test ran once. |
| `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | 0 | Every6 release test ran once. |
| `gh run view 41150939552 --repo szTheory/muhtari --json attempt,status,conclusion,headSha,url` | 0 | The previously fully audited run remains exact26c8268/attempt1/completed failure. No new CI acceptance. |
| `git ls-remote origin refs/heads/phase/12-player-ready-delivery-and-playstead-bridge` | 0 | Remote remains26c8268; no source publication. |
| `curl --fail --silent --show-error --max-time 20 https://www.githubstatus.com/api/v2/summary.json` | 0 | Official service remains operational. Health is not host-cause evidence. |

Local run URLs are N/A-local. Historical hosted evidence remains
[run41150939552](https://github.com/szTheory/muhtari/actions/runs/41150939552)
and its failed required aggregate; the prior complete20job/191step/20artifact
audit remains authoritative. Compiler success does not establish a complete
WASI execution, native MSVC success or browser readiness. Prospective reason
reporting cannot reconstruct the missing historical failure branch, readiness,
hotkey routing or audio causality. Accepted0/4/downstream halt remain unchanged.

Two separately owner-requested high-level ideas are captured without implementation:
[browser player](../seeds/SEED-261030-tt4-browser-libretro-player.md) and
[optional emulator interface](../seeds/SEED-261030-zk3-emulator-interface.md).
Their indexed primary-source research and later-session prompts are dormant
project ideas, not additional phase scope or live authority. No sibling file
was edited, no external emulator implementation source/content acquired.

Stop this step after final bookkeeping. Context may be cleared; exact next
separate command: `$gsd-debug continue phase12-packaged-input-tracer`. That
continuation can prepare a concrete future source-publication/host-experiment
proposal read-only; the agent owns fresh health, source and receipt checks.
No live allowance is renewed and no user acknowledgment is required to keep
the halt in place. The local repair is finished and uncommitted; a future
publication or actual experiment needs a new concrete scope decision.

Final handoff verification: `python3 /tmp/muhtari-approved-local-final.py`,
`sh scripts/hygiene.sh --tree`, `git diff --check`,
`cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake` and GSD
`query summary-extract` each exit0,N/A-local. Tested source hashes match the
worker manifest; exactly three approved source/test/doc paths changed. Existing
owner edits, workflows, ROADMAP, HEAD and index remain intact. All seven new
research/seed files have valid relative links and no personal paths. The summary
still has no completed requirement and the state retains accepted0/4/blocked.

## Read-only readiness and WASI exit assessment (2026-10-31)

This continuation completes the saved **read-only assessment**. Source remains
unchanged at cee6dea; the debug issue is unresolved, unarchived and halted.
Every earlier publication/live allowance remains spent or expired. The proposed
local batch below requires a new scope decision; this document grants none.

### Current evidence and independent review

Fresh connector run/attempt/job/artifact responses match the prior complete
[diagnostic run](https://github.com/szTheory/muhtari/actions/runs/41158640754):
all20jobs,191steps and20artifacts, one source run, no open phase PR.
The advertised branch still matches cee6dea. Official service summary is
operational, the previous incident resolved, and all five explicit nonterminal
CI listings are empty. The existing required audit still exits1 solely for
the failed required aggregate; advisory WASI remains visible and unaccepted.

Two bounded, read-only specialist assessments covered disjoint host-observation
and WASI-test boundaries. Neither changed files or launched tests/content.
The parent performed one adversarial synthesis against the actual saved logs.
No recursive review, new run, source change or phase promotion followed.

### Host: what another observation would and would not establish

The exact host attempt reports acknowledgment-deadline at5090ms, capture0,
errno0 and unknown terminal child status. The last posted event at4988ms
follows the existing successful nonblocking child poll in that loop iteration.
This supports only **no exit status available at that earlier poll**.
The guard precedes the next poll; cleanup cannot establish the child's state
when the guard selected its stopping reason.

The existing log also reports Cocoa input, a native window size822x726, Vulkan
display/swapchain initialization and the NS draw observer during the held
interval. These are frontend self-reports. Buffered replay times are not precise
readiness timestamps. They do not prove a key window, an input-ready event loop,
Up/Z consumption, F8 hotkey handling or a written screenshot. Audio errors remain
observations without established causality.

| Option | Useful distinction | Limitation and disposition |
|---|---|---|
| Keep current host implementation and stop | Preserves the demonstrated no-ack failure and all safety controls. | Recommended now; no new hosted evidence. |
| One failure-time owned-PID window-metadata query | Could independently distinguish an unavailable query, no matching reported window and a matching reported window after the guard fires. | Defer. Existing window self-reports lower its expected value; a positive result still cannot establish focus or input consumption. |
| Application-state plus owned input-boundary instrumentation | Could separate application state, core input polling and Up/Z state from screenshot-specific failure. | Requires a separate concrete design and source/acceptance scope. It still cannot directly observe released RetroArch's internal F8 handling. |

The smallest candidate query is `CGWindowListCopyWindowInfo`, filtered to the
owned child PID, once after selecting the no-ack reason and before cleanup.
It would omit names, images and other processes' identities and distinguish
null/unavailable from valid empty data. Apple documents the query as relatively
expensive with no hard call-duration guarantee. It could delay cleanup even
without changing the earlier event sequence; the existing50s wrapper would
remain the outer bound. Periodic polling introduces more overhead without
solving consumption observability.
[Apple window-query reference](https://developer.apple.com/documentation/coregraphics/cgwindowlistcopywindowinfo(_:_:)).

**Adversarial conclusion:** adding CoreFoundation mocks and another hosted run
solely to learn whether a window exists is not justified by the present
decision value. Retain this as a possible later observation only if independent
OS-level window presence would change a named action. Do not infer readiness
from the existing log or from a future positive query. Do not widen the window,
change focus/permissions/drivers, alternate F8 events or retry on another no-ack.
Any such experiment needs its own discriminating hypothesis and concrete scope.

### WASI: confirmed test-result transport defect

The real helper calls `execv` then `_exit(127)` on failed execution.
The mock replaces that terminating function, independently checks the argument
is127, complete argv, exactly one exec and no post/wait/kill, prints its marker,
then exits its own test process with127. The owning portable registration
requires outward127 plus that marker through the unchanged exact-code observer.

Pinned Wasmtime48.0.6 rejects status>=126 before creating its recognized exit
result. Its CLI therefore reports an invalid-status error and returns1.
The saved WASI stack and marker match that path. This is a deterministic
conjunction of the mock's outward127 and the runtime restriction, separate from
the hosted capture failure. It is not a general claim about every WASI runtime.
[Exact pinned runtime implementation](https://github.com/bytecodealliance/wasmtime/blob/v48.0.6/crates/wasi/src/p1.rs#L2597-L2606),
[CLI exit handling](https://github.com/bytecodealliance/wasmtime/blob/v48.0.6/src/commands/run.rs#L494-L515).

| Option | Contract and cost |
|---|---|
| Keep failing test | Safe stop; unchanged retry cannot resolve the demonstrated mismatch. |
| WASI-only mock transport125 | Recommended. Keep checking the helper supplies127; only after every semantic assertion passes, transport the mock result as125 on WASI. Native mock remains127. |
| In-process terminating-call observation | A setjmp/longjmp redesign could avoid special outward codes, but changes more test control flow for no demonstrated gain. |
| Accept host1 plus marker, skip test, or change runtime pin | Reject. The present runtime error already satisfies1 plus marker; skipping loses coverage and pin changes affect unrelated sample/runtime consumers. |

### Concrete proposed local-only batch, awaiting a new scope decision

Recommend **only the WASI transport repair**, with these owning paths:

- `tests/policy/retroarch_preflight.c`: keep the real helper's127 assertion and
  every argv/ownership assertion before the marker; use125 for the mock process
  only under the target's `__wasi__` macro. Keep native127 and failure1.
- `tests/policy/tests.cmake`: require125 only when
  `CMAKE_SYSTEM_NAME STREQUAL "WASI"`; native and PPC remain127. Keep the same
  test name, inventory membership, marker, emulator and exact-code observer.
- `docs/testing.md`: explain the helper-semantic versus mock-transport boundary
  alongside the existing keyboard test description. No new public-core behavior.

No change to `tests/retroarch/keyboard_preflight.c`, generic
`expect_fail.cmake`, clocks, workflow/pins, permissions, return values of the
real helper, event/guard ordering, five/45/50-second bounds or pixel judges.

The proposed scope includes local implementation, deterministic verification,
one bounded review and shared handoff bookkeeping. Local checks must run the
actual mock through its owning native and pinned WASI registrations, preserve
ordinary valid/fault and no-clock fallback coverage, and link the real helper
without executing it. Scratch mutations must reject wrong helper exit arguments,
missing termination, corrupted argv, post/wait/kill before termination, correct
marker with wrong outward status/runtime error, and correct sentinel without
the marker. Preserve full-inventory JUnit; use a separate focused report.
Run the required full local CI workflow and exact inventories after changes.
Local WASI runtime verification may use existing pinned tools only; no fetch,
install or pin change is included.

Read-only local tool discovery confirms the existing pinned Wasmtime reports
48.0.6 and the previous WASI sysroot is retained. The CI toolchain's named
clang-21 and a WASM linker are not on PATH; no wasm-ld exists in the checked
local LLVM locations. Therefore local native checks and target compilation do
not yet establish executable WASI verification. The proposed batch must report
that gap if no existing compatible linker is found; provisioning one would
need a separately concrete tooling scope. Do not infer runtime success from
the retained sysroot or compile-only checks.

Stop after that authorized local batch and bounded review. If the semantic
faults escape or pinned Wasmtime rejects125, stop rather than loosen the
observer. A missing local pinned runtime is a stated verification gap, not
permission to fetch it or a hosted acceptance claim. Publication, commit,
push, dispatch/rerun/cancel, actual helper/RetroArch/content launch, new budget
or time window, archive and phase promotion are outside this proposal.

Local success would repair the test transport only. Hosted WASI acceptance,
actual held/released input, PLAY-03 and accepted0/4 remain pending.

### Read-only command receipts

Hosted URL for every run-specific row:
[diagnostic run](https://github.com/szTheory/muhtari/actions/runs/41158640754).
Local rows have run URL N/A-local. Raw responses/logs remain scratch only.

| Command | Exit | What it establishes |
|---|---:|---|
| `python3 /tmp/muhtari-readonly-assessment.uZsrou/receipts.py` | 0 | Fresh connector source/event/attempt/all-job/step/artifact equality against retained raw evidence; baseline worktree snapshot. |
| `python3 /tmp/muhtari-diagnostic-exec.Hak1cS/final_refresh.py` | 0 | Fresh official operational/resolved health, unchanged source/terminal receipts, noPR, one source run and protected bytes. |
| `python3 /tmp/muhtari-diagnostic-exec.Hak1cS/active.py` | 0 | All five explicit nonterminal CI listings empty. Connector does not support this workflow-collection URL; existing read-only CLI used. |
| `python3 /tmp/muhtari-diagnostic-exec.Hak1cS/audit_outcome.py` | 0 | Existing raw native productions, actual no-ack reason, separate WASI error and required membership reconcile; no test reexecution. |
| `cmake -DINPUT=/tmp/muhtari-diagnostic-exec.Hak1cS/final-view.out -DEXPECTED_SHA=cee6dea491918282d034f8c6e9a90087d8213439 -P tools/check_ci_run.cmake` | 1 | Existing required hosted failure retained. |
| `git ls-remote origin refs/heads/phase/12-player-ready-delivery-and-playstead-bridge` | 0 | Advertised source remains cee6dea; connector rejected encoded branch path, so read-only git was used. |
| `build/pins/wasmtime/wasmtime --version` | 0 | Existing local runtime48.0.6; introspection only, no test/content execution. |
| `python3 /tmp/muhtari-readonly-assessment.uZsrou/final_checks.py` | 0 | Individually checked hygiene, diff, existing budgets, GSD summary parser and four-file handoff preservation. |
| `python3 /tmp/muhtari-readonly-assessment.uZsrou/verify_handoff.py` | 0 | Source/workflows/other owner edits/HEAD/index preserved; current halt, empty completions, next command and relative links agree. |

### Handoff

Waiting for the owner's **new local-only WASI batch decision**. No action is
required to preserve the halt. Context may be cleared now. Exact next separate
command: `$gsd-debug continue phase12-packaged-input-tracer`, with that decision.
The invocation alone grants no repair or renewed live allowance. The agent owns
fresh source, service-health and receipt checks in the next session. No owner
verification or acknowledgment merely to remain stopped is required.

## Approved local WASI transport batch (2026-10-31)

Owner answered **"yes i approve"** to the preceding concrete local-only repair
and local checks. That decision supersedes the earlier awaiting-local-decision
text, not exhausted publication/live allowances. This approved batch is now
implemented, locally verified within the explicit runtime gap, and uncommitted.
No helper/frontend/RetroArch/cartridge launch, fetch/install, remote mutation,
pin/workflow/timing/permission change or phase promotion occurred.

### Implemented behavior

The real helper is unchanged. Its failed exec still reaches `_exit(127)`.
The mock still independently checks127, complete argv, one exec, no input,
wait or kill and no accumulated ownership fault before printing its marker.
Only the mock process's successful test-result transport changes:125 under
`__wasi__`,127 elsewhere. Invalid semantics still exit1 without the marker.
The owning CMake registration expects125 only for system WASI; native and
Linux/PPC registration retain127 and the same exact-code/marker observer.

Two owning test files and `docs/testing.md` changed. No test name, inventory,
emulator invocation, target clock type/fallback, real-helper behavior or host
schedule changed. The documentation explains the distinction. Its initial
addition exceeded the existing page budget; the first full workflow failed8
solely at `docs.budgets` (41199 versus40960bytes), before packaging/release.
A compact description of the same test responsibilities restores the unchanged
budget; no check or semantic assertion was relaxed. The final full workflow
passes. The failed workflow remains evidence, not a successful earlier run.

| Verified source path | SHA256 |
|---|---|
| `tests/policy/retroarch_preflight.c` | `b92e4ead25988bf8a3434233ffe7ce6f632eb35be368ad68dcdfd516c8727bda` |
| `tests/policy/tests.cmake` | `a1e23d296de620ba49ff117e6dfaa1d90f535c3baa3a7174457c0bda6400ed40` |
| `docs/testing.md` | `aba0ff64724b7d66f173af8ce985af78e1b15d54c844fcb0bbd2f8ba1cbad021` |

### Verification and its limits

Fresh strict native and no-clock-fallback builds run the actual mock's complete
valid/fault tests and child observer. Private native builds selecting the
WASI branch also pass, using125. Actual wasm32-wasi source compilation succeeds
against the retained target sysroot. The real native helper links; it is never
executed.

Scratch helper/mock mutations reject wrong helper exit0/126, corrupted argv,
child post/wait/kill before termination, omitted termination, wrong outward
transport and missing marker. Semantic faults fail before the marker.
Native127 cannot satisfy a125 expectation and the WASI branch125 cannot satisfy
127. The original source compiled with the same branch-selection macro rejects
the new125 observer, showing the changed code is needed.

Private CMake projects execute the exact owning registration block with its
real test-registration function and native executable stand-ins. Darwin/Linux
expect127, WASI expects125, and a multi-part emulator prefix survives. These
are registration/branch checks, **not** actual WASI C or PPC execution.

Independent, self-authored WAT fixtures run in the existing pinned
Wasmtime48.0.6 through the unchanged observer:125 plus marker passes;127 plus
marker triggers the reproduced invalid-status error and fails;125 without the
marker fails. This reproduces the runtime boundary and exact-code/marker
protection without downloading a tool.

**Remaining gap:** the complete C mock has not been linked and executed as
WASI locally. No compatible WASM linker was found in the inspected local tools.
Native branch execution, target compilation, registration simulations and the
independent runtime fixtures do not replace that target execution. No tool was
fetched/installed. Full hosted WASI acceptance remains advisory and pending.

The single bounded source review returns PASS with no blocking findings; it
does not claim executable WASI acceptance. The parent checked the subsequent
documentation condensation and the complete fault/registration/runtime evidence.
No recursive review or extra source scope was introduced.

### Commands and exits

All commands below have run URL **N/A-local**. Prior hosted evidence remains
[run41158640754](https://github.com/szTheory/muhtari/actions/runs/41158640754);
no new hosted run exists. Raw logs and detailed commands remain scratch only.

| Command | Exit | Establishes |
|---|---:|---|
| `python3 /tmp/muhtari-wasi-transport.atnGLI/preflight.py` | 0 | Fresh source/health/complete-terminal-receipt reconciliation and baseline worktree snapshot. The first reused old refresh's obsolete planning hash was superseded by this current assessment baseline, without changing source/history. |
| `cmake --build build/ci --target muhtari-test-preflight` | 0 | Owning native target compiles with strict warnings. |
| `ctest --test-dir build/ci -R '^policy[.]retroarch[.]' --no-tests=error --output-on-failure --output-junit /tmp/muhtari-wasi-transport.atnGLI/focused.xml` | 0 | Existing actual helper/wrapper/report/policy consumers; focused report kept separate. |
| `python3 /tmp/muhtari-wasi-transport.atnGLI/checks.py` | 0 | Native/fallback/forced-branch controls, actual WASI compile-only, helper link-only, semantic mutations, registration simulations, real pinned-runtime WAT fixtures and baseline counterfactual. Individual expected-fault observers exit1 as intended. |
| `python3 /tmp/muhtari-approved-local-workflow.py`, first execution | 8 | Full ordinary suite detects only the new documentation budget excess; release/package phase not reached. |
| Same full-workflow command, final execution after documentation correction | 0 | Complete677 ordinary tests, all three ZIP packages and6 release tests, unchanged four-worker FIFO convention. |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | Final full ordinary inventory, exactly once each. |
| Same consumer with `-DJUNIT=build/ci/junit-release.xml -DLABELS=release` instead of `-DEXCLUDE=release` | 0 | Final full release inventory, exactly once each. |
| `python3 /tmp/muhtari-wasi-transport.atnGLI/refresh.py` | 0 | Fresh operational/resolved health, exact original terminal/source/attempt/all jobs/steps/artifacts, unchanged remote/noPR/one source run/noactiveCI. Required audit stays1. |
| `python3 /tmp/muhtari-wasi-transport.atnGLI/final.py` | 0 | Final exact three-file source manifest, five owned handoff/lesson edits, other owner edits/workflows/HEAD/index preservation, hygiene/diff/budgets, GSD summary parser, full inventories and relative links. |

### Preserved stop and next useful step

Approved local source implementation/checks/review are finished. Source remains
uncommitted; no publication or actual host experiment is authorized. Preserve
the unresolved hosted capture/readiness/routing cause, PLAY-03 pending,
accepted0/4/downstream blocks and every failed receipt automatically.

The least disruptive next step is **read-only preparation of a concrete scratch
WASI linker/target-execution proposal** so the complete C mock can eventually
be verified locally. This is a recommendation for a separate invocation, not
permission to fetch/install a tool, change pins or start a run.

Context may be cleared now. Exact next separate command:
`$gsd-debug continue phase12-packaged-input-tracer`. The agent owns fresh
source, service-health and receipt checks and read-only tooling assessment in
that session. No user verification or acknowledgment is needed merely to keep
stopped; no work from the approved batch remains awaiting user action.

## Approved local input-tracer outcome (2026-10-31)

The owner-approved [concrete local batch](phase12-input-tracer-proposal.md#approved-local-execution-outcome-2026-10-31)
is implemented, verified and uncommitted. Separate noninstalled instrumentation
observes existing callback returns and completed/null polls; exclusive helper/
child descriptors, bounded fixed records and a fail-closed parser retain valid
prefixes through synthetic child kill and actual wrapper failure handling.
Normal callback order/count/returns and frame/audio results remain equal.

The sole independent source review found two real collection gaps. One correction
round requires helper END/saturation and rejects captured late writer failures.
Actual collector and post-INIT EBADF wrapper controls reject; reintroducing either
old guard makes its consumer fail. Four compiled omission/callback/value faults
also reject. An unfinished core is explicitly unfinished with writer health
unknown; matching PIDs/valid clocks alone do not prove same-host provenance,
and no absent input conclusion follows. Five local elapsed pairs are noisy
(delta−0.139..+0.698ms); no real-time or filesystem-latency guarantee is claimed.

Final complete original workflow, all three packages, exact ordinary/release
inventories, affected tests, hygiene/format/diff and baseline reconciliation
pass0, N/A-local. Fresh all-job/step/artifact receipts remain the original hosted
failure/required1 with operational/resolved health/noPR/noactiveCI. Exact commands,
failed setup/test checks, review findings/corrections and limitations are in the
linked outcome. HEAD/tree/index/unrelated owner bytes and the earlier WASI repair
remain intact. No actual helper/platform/frontend/content/input or remote action.

The actual hosted capture/readiness/routing cause remains unresolved. Preserve
PLAY-03 pending, accepted0/4, downstream blocks, failed history and spent live
allowances. This local step is finished with no owner verification/acknowledgment.
Context may be cleared; exact next separate command
`$gsd-debug continue phase12-packaged-input-tracer` prepares the smallest concrete
hosted diagnostic proposal read-only. The agent owns fresh source/service/full
receipt checks; actual hosted execution/publication needs a new explicit decision.
No live allowance, archive or phase promotion follows this invocation.


