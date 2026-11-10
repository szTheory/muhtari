---
audit_acknowledged:
  milestone: v3.0
  at: 2026-11-10
  status: unknown
---

# Local diagnostic-test portability repair

Revised execution on 2026-10-31. **The approved narrow observer continuation
finished the remaining local controls, full workflow and sole repair review.**
The two C repairs remain reviewed and uncommitted. See
[the current outcome](#completed-observer-continuation-and-local-repair-2026-10-31).
The earlier supervisor failure and its receipts remain intact.
The completed scratch-only
fortify2 comparison establishes local ignored-write diagnostic sensitivity.
The original unfortified comparison did not. Frozen originals remain retained;
two current test sources changed and the optional consumer is unchanged.
The current disposition is the completed local batch immediately below;
all older recommendations/approvals remain historical where superseded.
The prior publication/diagnostic batch is finished. Its commit, push and dispatch
allowances are spent; unused child capacity does not carry forward. Keep the
host halt, accepted 0/4, PLAY-03 pending and downstream blocks automatically.

## Recommended decision

The owner selected option1 for the narrow observer continuation. Its finite
local work is finished; no local verification or acknowledgment is pending.
Stop with the reviewed uncommitted bytes. The next separate step may prepare a
concrete publication/diagnostic proposal read-only, with fresh agent-owned
service/source/complete-receipt checks. A later publication or live attempt
requires new concrete authority. Original host acceptance and every spent
allowance remain stopped; no archive or promotion follows.

Historical recommendation follows:

Approve one revised local-only repair/offline verification batch described in
[the current scope](#revised-narrow-local-repair-scope-2026-10-31), ending with
reviewed uncommitted bytes. The earlier broader repair approval reached its
binding stop and does not resume. This proposal grants no execution authority.

## Revised narrow local repair scope (2026-10-31)

The consumer is the existing offline input-trace test chain. Two deterministic
test-source defects block required Linux producers: an unchecked synthetic-child
acknowledgment and suffix paths whose destinations are no larger than their
directory buffer. Fix those responsibilities in their current owners. Retain
strict warnings, all CI membership and existing runtime bounds. No production
core/helper, workflow, pins, dependencies, install/package membership, public
API or inventory changes are proposed.

### Exact files and behavior

| Allowed later source path | Concrete repair and regression responsibility |
|---|---|
| `tests/libretro/test_input_trace.c` | Check that the synthetic child's acknowledgment write returns exactly one; otherwise `_exit(1)` before pause, allowing pipe closure. On parent EOF/read error, close its descriptors and terminate/reap only its owned child before returning failure. Preserve successful SIGKILL, reaping and unfinished three-record-prefix assertions. Exercise valid, failed/zero-byte acknowledgment and parent read-error paths through this executable. |
| `tests/retroarch/test_input_trace_helper.c` | Keep `dir[4096]`; reject negative/truncated initial `snprintf` before `mkdtemp`. Size helper/core/raw leaf arrays as `sizeof dir + sizeof "/helper.trace" - 1` and corresponding suffix expressions, including NUL. Check formatting boundaries before filesystem access, retaining existing synthetic descriptor/collection assertions. |
| `tests/libretro/input_trace.cmake` | Optional existing consumer seam only: invoke new negative modes if needed, require their intended nonzero result/diagnostic and successful owned-child cleanup. Keep current success/fault consumers, exact inventory and all existing 5s process deadlines. Leave unchanged if the executable internally covers the new cases adequately. |

Only these three source/test paths may change in the later batch. Existing
shared planning handoff updates use these five paths only: this proposal,
`phase12-packaged-input-tracer.md`, `phase12-hosted-input-tracer-proposal.md`,
`../STATE.md`, and the halted `12-01-SUMMARY.md`. Source edits remain forbidden
in this proposal step. Reject a warning suppression, cast/discard workaround,
reduced directory capacity, new shared path/process abstraction, or deletion
of fault consumers as outside this repair.

The acknowledgment fault needs cleanup, not just warning cleanliness. A child
that ignores a failed write can retain the pipe writer while pausing; parent
read can then wait for the outer timeout. Conversely, a parent `REQUIRE` after
read failure currently returns before owned-child kill/reap. The new fault
consumers must exercise cleanup directly, and must not let an outer timeout
stand in for a passing assertion. One-byte writes have only exact-one success;
zero or negative results fail. A real write to an invalid descriptor can cover
the negative case; a narrow injected zero return can cover the short case.
Fault injection must exercise the same repaired branch, without a separate
copy of its logic. An outer synthetic supervisor may observe child identity and
verify no owned process survives; no actual frontend/device activity is allowed.

### Fixed offline compiler and runtime acceptance

1. Before editing, the agent repeats complete original/latest hosted receipts,
   official service, advertised source, PR/active-run and HEAD/index/all unrelated
   byte checks. Freeze all three source hashes in the existing boundary table.
   Reconcile the retained compiler receipts and exact immutable cached image;
   a discrepancy stops before editing. No pull, acquisition or installation.
2. Use only image `sha256:7991bc43622028874916da3d0f1aadecc795c3a72bae6a55570d21d7e62961e4`,
   Linux arm64, with `--rm --pull=never --network=none --read-only`, read-only
   source mounts and private scratch outputs. Keep the recorded strict flags
   verbatim from the fixed comparison, `-O3 -DNDEBUG -std=c17`, existing include
   paths and retained ApplicationServices declaration stub. `-D_FORTIFY_SOURCE=2`
   is allowed only in the explicitly enumerated scratch commands below; do not
   change any repository/compiler defaults. Do not infer exact hosted equivalence.
3. Fixed compiler work is **eleven initial invocations, with at most three
   conditional correction checks (fourteen total maximum), no retries/sweep**:
   one metadata command; fortify0 and fortify2 macro/declaration commands
   (four); the frozen original protocol object under fortify0 and fortify2
   (two); the frozen original helper object under fortify0 (one); repaired
   protocol objects under fortify0 and fortify2 plus repaired helper under
   fortify0 (three). Metadata/macro/declaration and every repaired compile
   must exit0. Original protocol must exit0 at fortify0 and exit1 solely the
   known ignored-write diagnostic at fortify2. Original helper must exit1 with
   the three known suffix-truncation errors. Run old-source discrimination
   before editing; any missing or unrelated error stops. Retained results
   justify this fixed gate; no exploratory fortification tuning is proposed.
4. Every container subprocess keeps its existing60s deadline. Reserve an owned
   name before launch, retain Docker's `--cidfile` and exact ID, and on any
   timeout/error remove only that owned container and verify cleanup. A timeout,
   cleanup uncertainty or unexpected compiler result stops the batch with the
   first failure retained; do not enter subsequent stages or use another image.
   No C program is linked or run in the container. This checks diagnostic
   sensitivity and repaired object compatibility, not Linux runtime acceptance.
5. Build actual native protocol/helper targets with
   `cmake --build --preset ci --target muhtari-test-trace-protocol muhtari-test-trace-helper`.
   Through `policy.retroarch.preflight_api` and the existing consumer, cover
   exact-one acknowledgment success, actual negative and injected zero write,
   EOF and parent read error, prompt intended failure and zero surviving owned
   children. Keep current successful killed-child three-record prefix. For
   initial directory formatting test largest fit and first overflow (NUL
   included), and negative formatting return if the chosen seam can produce it;
   prove every derived leaf buffer fits a maximum directory payload and each
   suffix. Formatting-only large inputs must not reach `mkdtemp` or assume
   an OS-valid long path. Empty/small valid inputs retain current behavior.
6. Add regression assertions to those existing consumers so the unmodified
   `cmake --workflow --preset ci` runs them. Use scratch-only branch-omission/
   old-capacity counterfactuals to show that intended fault assertions reject
   the broken behavior, including cleanup; do not add a mutation framework.
   Preserve corruption/clock/write/saturation, normal callback/order/value/
   frame/audio comparisons, exclusive descriptor ownership, unfinished-helper
   rejection, late core-write rejection and collection-before-wrapper-error.
7. Run focused `libretro`/`policy.retroarch` checks with a separate JUnit path,
   then one unmodified complete `cmake --workflow --preset ci`, original
   ordinary/release exact inventories, three-package diagnostic exclusions,
   format/hygiene/budgets/diff checks through existing mechanisms. Keep existing
   bounded workers and CTest/process deadlines; do not overwrite full-inventory
   evidence with focused reports. A failure outside the three seams stops for
   a separate decision; do not broaden this batch.
8. One bounded independent source/evidence repair review and at most one
   correction round, followed only by affected rechecks and a final full gate
   if source changed. Only if that round changes either C file, allow one final
   fixed set of the three repaired object commands from step3 (protocol
   fortify0/2 and helper fortify0), each required0; do not repeat metadata or
   old-source discrimination. This is the sole conditional three-command
   reserve, not a retry of a failed gate. Finish uncommitted with exact changed paths/hashes,
   command/exit/N/A-local evidence and unrelated-byte/index protection checks.
   Stop for a separate later publication scope decision. No staging, commit,
   push/PR, dispatch/rerun/cancel, actual frontend/content/input, allowance
   renewal, archive or phase promotion follows.

The strongest alternative is an exact hosted Linux environment, which could
reduce compiler/libc differences but adds acquisition or hosted scope without
being necessary to test these observed source defects. Keeping stopped costs
nothing but leaves required builds blocked. Recommend the fixed local batch;
stop instead of widening it if any expected diagnostic does not discriminate,
a protected byte changes, a synthetic child survives, an unrelated gate fails,
or another source/tooling change is needed. Local success would neither clear
the two failed hosted audits nor resolve readiness/routing/F8/audio. Accepted
0/4, PLAY-03 pending, downstream blocks and all spent allowances persist.

Focused complete-C WASI execution already passed in its separately approved
scratch batch. The older SDK28 linker-only proposal is historical and is not
reopened by this repair. Full hosted WASI remains advisory/unaccepted. There
is no new runtime/toolchain acquisition question here.

### Current proposal evidence and handoff

`python3 /private/tmp/muhtari-portability-preflight.py` exits0: original
20jobs/191steps/20artifacts/19executedlogs and latest
20jobs/170steps/15artifacts/16executedlogs match complete retained receipts.
Both `cmake -DINPUT=<complete-view> -DEXPECTED_SHA=<run-source> -P tools/check_ci_run.cmake`
audits exit1, preserving required failures at
[run41158640754](https://github.com/szTheory/muhtari/actions/runs/41158640754)
and [run41208847091](https://github.com/szTheory/muhtari/actions/runs/41208847091).
Retained diagnostic ZIP/source/member hashes verify0; official service is
operational/resolved, advertised source matches, no open phase PR or active
CI/new source run exists, HEAD/index/protected bytes agree. Private receipts:
`muhtari-portability-proposal.q6edwrwr`; no committed logs.

One bounded independent read-only proposal review identified that a correction
round needed an explicit compiler allowance. The current scope resolves this
with only one conditional three-command repaired-object set, fourteen maximum;
all original faults must already discriminate before repair. The reviewer found
no other material scope/oracle/cleanup gap. This is proposal review, not repair
acceptance; no further proposal fan-out follows.

`python3 /private/tmp/muhtari-portability-stop-check.py` exits0: all three
frozen source hashes, HEAD/index and unrelated tracked/untracked bytes remain
unchanged; only the existing five planning paths changed. Links, halted
summary, accepted0/4, both required failed audits and no active compiler
container agree. `git diff --check` exits0. Read-only inspection of the retained
fortify receipts confirms the seven individual exits/oracles and prior cleanup;
no compiler comparison was repeated. No compiler/container/target/program was launched
and no repair is claimed in this proposal step. Await only the NEW separate
local repair-scope decision; no owner verification or acknowledgment is needed
to maintain the stop. Context may be cleared. Exact next separate command:
`$gsd-debug continue phase12-packaged-input-tracer` with the scope decision.
The next agent owns fresh service/source/complete-receipt checks and, only if
authorized, the fixed batch above.

## Revised local repair stopped at supervisor gate (2026-10-31)

The owner selected option1 for the revised scope. Fresh
`python3 /private/tmp/muhtari-portability-preflight.py` exits0: complete
original/latest run/attempt/jobs/steps/artifacts/all executed logs, retained
diagnostic digest/source/members, service operational/resolved, advertised
source/noPR/noactiveCI and HEAD/index/protected bytes reconcile. Both required
hosted audits remain1 at the two linked run URLs above. Private preflight
receipts: `muhtari-portability-proposal.yrkgbg7r`.

`python3 /private/tmp/muhtari-portability-repair-compiler.py` exits0 after the
eight prescribed original-stage invocations; `... repaired` exits0 after the
three prescribed repaired objects. Exactly eleven container invocations ran;
no correction reserve was used. Metadata/macros/declarations and old protocol
fortify0 object0/fortify2 intended ignored-write object1/old helper three
intended truncation errors1 all discriminate. Repaired protocol fortify0/2
and helper fortify0 objects each0, unchanged strict flags/stub/exact cached
immutable image, offline/read-only source,60s bounds, every owned container
cleanup confirmed. No C program was linked/run in a container. Receipts and
frozen old copies: `muhtari-portability-repair.gwvxgqus/receipts.json`.

Only the two owning C files changed. The protocol checks exact-one write,
exits the child before pause on negative/zero return, closes parent descriptors
and kills/reaps its child before reporting read failure. Internal controls
cover real EBADF write, injected zero, EOF and real EBADF read; a distinct
failure sentinel prevents assertion failure from looking like an expected
negative result. Successful SIGKILL/three-record-prefix assertions remain.
The helper retains4096 directory bytes, derives all three suffix capacities,
checks initial negative/truncated formatting before filesystem use and tests
largest fit/first overflow/max leaves/empty/small strings. A natural negative
snprintf return is N/A for this plain-string format; the guard checks it.
The optional CMake consumer is unchanged; these controls run in its existing
executable chain.

`cmake --build --preset ci --target muhtari-test-trace-protocol muhtari-test-trace-helper`
exits0. The existing `policy.retroarch.preflight_api` focused check exits0;
focused `^(libretro\.|policy\.retroarch\.)` checks exit0 with separate JUnit
paths, retaining callbacks/order/values/frame/audio and all existing fault
collection controls. Three native protocol runs and the instrumented valid
supervisor control pass0.

**First failure retained:**
`python3 /private/tmp/muhtari-portability-repair-runtime.py` exits1 during the
scratch child-ack-branch-omission control: the sandbox denies its read-only
`ps -p <owned-child> -o state=` query with `PermissionError: Operation not
permitted`. This is a supervisor tooling failure, not a demonstrated C defect
or accepted mutation oracle. Its finally path killed only its recorded owned
parent/children. A permitted read-only cleanup query
`ps -axo pid,ppid,stat,command | rg '[m]uhtari-portability-repair.gwvxgqus/(child-ack-omission|observed-valid)'`
exits1 with no match, verifying no such process survives. This query used
approved sandbox escalation; automatic approval did not reject it.

The first-failure stop applies: no runtime-script retry, further mutant/control,
full workflow, inventory/package acceptance or independent repair review was
run afterward. Parent-kill omission and path-capacity/formatting-guard mutants
remain unrun. No repair acceptance/guardrail verdict is claimed. Full local
workflow and sole repair review remain required in a later concrete scope.
No actual frontend/device/input/content, acquisition/install, stage/commit,
push/PR, hosted action, allowance renewal, archive or phase promotion occurred.
The original actual-host cause remains unresolved; accepted0/4, PLAY-03 pending,
downstream halt, both failed audits and spent allowances persist.

Context may be cleared after the handoff. Exact next separate command:
`$gsd-debug continue phase12-packaged-input-tracer` for a narrow continuation
decision around the retained supervisor's process-observation seam. The agent
owns fresh service/source/complete-receipt checks and a concrete continuation;
no owner verification or acknowledgment is needed to preserve the halt.

Final `python3 /private/tmp/muhtari-portability-repair-handoff.py` exits0:
exactly these two source paths plus the five allowed planning paths changed;
HEAD/index/every unrelated tracked-untracked byte, old frozen copies and the
optional consumer agree. Exactly11container invocations/no correction reserve,
all owned container cleanup, zero active cached-image containers, formatting,
diff, links, halted summary/accepted0/4 and both failed audits verify. Evidence
is `muhtari-portability-repair.gwvxgqus/handoff.json`, N/A-local; no committed
log. Hygiene/budgets/full inventories/packages remain unrun acceptance work.

| Final retained path | SHA256 |
|---|---|
| `tests/libretro/test_input_trace.c` | `6032417aeaf3f46138ef262d7a5bffc3f3aee7e7ca2a9bc2e4809ae57482091e` |
| `tests/retroarch/test_input_trace_helper.c` | `e492133dd5d81618a2db24a578c7e8596eb9e26596044a3cd24b0d05b7686475` |
| `tests/libretro/input_trace.cmake` | `d01eff92bb57a864129e3687c9f9de309972f3b52807d5680a56b1f5602d538e` |

## Completed observer continuation and local repair (2026-10-31)

Owner response option1 authorized only the scratch observer replacement,
remaining unrun fault consumers, one unmodified full local workflow and the
sole bounded independent repair review. No repository source changed during
this continuation. The two C hashes in the retained table above still match;
the optional CMake consumer remains byte-identical.

`python3 /private/tmp/muhtari-portability-observer-preflight.py` exits0:
fresh complete original/latest run/attempt/jobs/steps/artifacts/all executed
logs reconcile, both required audits remain1, retained diagnostic identity
verifies, service is operational/resolved, advertised source matches and no
phase PR or active CI exists. HEAD/index/all unrelated tracked-untracked bytes
and exact repaired source hashes agree. Old compiler/runtime/first-failure
receipts remain immutable. Private fresh receipts:
`muhtari-portability-proposal.buhpvrfu`; N/A-local. The two failed run URLs
above remain authoritative for hosted acceptance.

The permitted observer uses `kill(owned_pid,0)`: a known owned synthetic
process is present before exit and absent after its parent waits. No process
query escalation was used. `python3 /private/tmp/muhtari-portability-observer-runtime.py`
exits0 using the retained child-ack-omission binary and only the remaining
unrun controls. The child-ack omission needs targeted intervention and then
exits1; parent-kill omission needs targeted intervention even though its
executable exits0. Requiring that intervention is the supervisor fault oracle,
not a passing timeout. Both finish before the unchanged5s bound and every
recorded owned child is absent after reaping. Old leaf capacities and omitted
directory guard each compile and fail1 at their intended existing assertion.
The passed native/compiler stages were not replayed. Plain-string formatting
has no natural negative-return input; the explicit negative guard remains.

`python3 /private/tmp/muhtari-portability-observer-local.py` exits0. Its single
unmodified `cmake --workflow --preset ci` exits0 in about54s; original
ordinary677/release6 inventories each exit0, all three ordinary packages
exclude diagnostic artifacts, and existing explicit source format/hygiene/
budgets/diff checks exit0. Focused reports did not overwrite these inventories.
The retained eleven GCC invocations were not repeated; no correction reserve
was consumed and no container program was linked or run.

CLI `query resolve-execution gsd-code-reviewer` resolved `gpt-6-luna`/`xhigh`.
The sole authorized independent source/evidence review is PASS with no
blocking or warning findings and zero correction rounds. It independently
checked source identity, direct consumers and the retained fault/full-gate
receipts. Formal Stryker is absent/unconfigured for this C project; the explicit
branch/capacity/guard counterfactuals supply scoped fault evidence instead.
Frozen-original strict compile failures versus repaired objects and the
scratch omissions establish the local repair without rolling back shared edits.
No behavior/warning/test/bound/platform consumer was suppressed or deleted.

Local guardrails accept these two test-source repairs only. Linux runtime,
exact hosted-toolchain equivalence and actual frontend/input acceptance are
unverified. The original host cause remains unknown; accepted0/4, PLAY-03
pending, downstream blocks, both failed audits and all spent allowances persist.
No staging/commit/push/PR/dispatch/rerun/cancel, actual frontend/content/input,
download/install, new allowance, archive or promotion occurred.

Private continuation receipts/review: `muhtari-portability-observer.ep756gha`.
`python3 /private/tmp/muhtari-portability-observer-handoff.py` exits0: exactly
two C and five permitted planning paths changed since original preflight,
optional consumer unchanged, HEAD/index/all unrelated tracked-untracked bytes/
frozen originals/old failure and receipts preserved, no recorded owned child
or cached-image container remains. Links/diff/halt/both failed audits verify.
Private `handoff.json` records exact changed paths/source hashes and command/
exit/N/A-local evidence. No raw log is committed. This finite step is stopped.
Context may be cleared; exact next separate command:
`$gsd-debug continue phase12-packaged-input-tracer` for read-only preparation
of a concrete publication/diagnostic proposal. The agent owns fresh checks;
no owner verification or acknowledgment is needed to preserve the halt.

## Historical original repair recommendation

The [tooling-only comparison](#approved-fixed-tooling-comparison-outcome-2026-10-31)
is finished. Next separate step should prepare the revised narrow local repair
scope read-only using its evidence. The original repair recommendation that
follows is historical; repair authority does not resume automatically.

The execution result below supersedes the original recommendation to execute
this batch. A separate narrower tooling-verification proposal is needed; the
existing stop remains in force without another confirmation.

Approve one local-only repair and offline verification batch in the three
existing paths below. Fix the two test compilation blockers, exercise the
affected failure paths through the existing test consumer, run the original
local workflow and one bounded independent review, then stop with uncommitted
reviewed bytes. This decision grants no publication or hosted allowance.

| Path | Proposed responsibility |
|---|---|
| `tests/libretro/test_input_trace.c` | Require the synthetic child's one-byte acknowledgment write to succeed; close through `_exit(1)` on failure. On a failed parent acknowledgment read, terminate/reap its owned child before reporting failure. Preserve the successful signal-kill and retained three-record prefix assertions. Keep regression cases in this executable. |
| `tests/retroarch/test_input_trace_helper.c` | Keep the existing directory-template capacity; check its `snprintf` result before `mkdtemp`. Derive helper/core/raw leaf capacities from `sizeof dir` plus the corresponding literal suffix including its terminator, rather than another equally sized buffer. Add the necessary formatting-boundary checks in this test executable. |
| `tests/libretro/input_trace.cmake` | Extend only the existing offline consumer if separate invocation of the new negative modes is needed. Check expected failures and owned-child cleanup; retain existing synthetic protocol/helper/host/wrapper controls. No new test registration or inventory entry is needed. |

The third path is an allowed test seam, not a requirement to change a file
without a useful consumer. Shared planning handoff updates are bookkeeping.
All other source, workflow, warning flags, pins, runtime/timeouts, permissions,
normal/diagnostic module selection, package membership and required CI gates
remain outside the repair. No new dependency, generalized path layer or
synchronization framework is proposed.

## Evidence and diagnosis

Fresh read-only reconciliation matched both saved complete runs, attempts,
all jobs and steps, source-bound unexpired artifact metadata and every executed
job log. The original [run41158640754](https://github.com/szTheory/muhtari/actions/runs/41158640754)
still fails actual hosted capture. Latest
[run41208847091](https://github.com/szTheory/muhtari/actions/runs/41208847091)
still fails before RetroArch: both Linux producers reject the ignored `write`
result at `test_input_trace.c:247`; x64 additionally rejects the appended
helper/core/raw paths at `test_input_trace_helper.c:139–141`.
Quality lacks those Linux producer artifacts and the required aggregate fails.
Both required CMake audits still exit 1. No new run, retry, access preflight,
actual frontend child, input event, raw trace or image exists.

The retained latest Mac diagnostic ZIP still matches the fresh API digest;
its five members, source/run/attempt identity, exact source manifest and member
hashes verify. This checks retained producer evidence, not host execution.
The advertised branch and working source remain at
`55f30d64ad1999231a97ea05a96e6a29d061c116`, with unchanged index and protected
worktree bytes. Official service status is operational and the earlier incident
resolved; no open phase PR or active CI run was found across all five active
statuses. None of these facts establishes the original host cause.

The ignored acknowledgment is more than a compiler complaint. If `write` fails
and the child continues into `pause` while retaining its pipe writer, its
parent can block reading indefinitely until the outer test deadline. Checking
the result must close that failure path, and a parent read error must not skip
owned-child cleanup. A checked return stored and discarded later, a warning
suppression, or a cast does not restore the contract.

The path warning comes from appending a suffix to a directory string with the
same maximum capacity as its destination. For example, 4095 directory bytes
plus `/helper.trace` and NUL need 4109 bytes. Express that relationship with
`sizeof`; do not shrink the existing directory capacity simply to silence GCC.
The original directory formatting must itself reject a negative or truncated
result before filesystem use. Capacity checks do not promise the operating
system accepts an arbitrarily long path.

These are source-backed mechanisms and hosted compiler failures. New local
fault executions have not occurred in this read-only step. Actual frontend
readiness, routing, F8 consumption and audio remain unknown. Reproduced lesson
MU-TEST-010 in `../LESSONS.md` already covers the escaped compiler mismatch;
this proposal does not claim another reproduced lesson.

## Alternatives and stopping conditions

| Option | Assessment |
|---|---|
| Keep source unchanged or retry | The required Linux build is still blocked by deterministic source errors. No transient hypothesis supports a retry. |
| Repair the two tests and extend their existing offline consumer | Recommended: owns the actual formatting and process-lifecycle responsibilities; preserves strict warnings and the original acceptance gates. |
| Suppress warnings, reduce directory capacity, remove targets, or introduce shared dynamic helpers | Suppression/removal loses fault detection; reduced capacity hides the relationship; shared machinery adds interface and review cost without a present shared need. |

One source/ownership specialist and one independent build/evidence specialist
reviewed disjoint questions; the parent owns synthesis and shared files. The
source review identified the child-pause/parent-reap interaction above. The
build review identified AppleClang's limits; subsequent Docker metadata gave
new evidence for the offline GCC option below. No code review acceptance or
implemented-fix claim is made here.

Stop the later batch rather than widening scope if a protected byte changes,
the old compiler faults cannot be discriminated, a synthetic child survives,
the existing gate fails outside these seams, or a different source/tooling
change is necessary. Preserve the first failure and report the concrete gap.
Use at most one independent repair review and one correction round for this
batch; no repeated general audit or extra hosted experiment follows.

## Local verification proposed after approval

1. Agent repeats source/index/protected-byte, complete hosted-receipt, service,
   advertised-head, PR and active-run checks. Freeze the three file hashes below
   and all unrelated work. Do not stage or commit.
2. Use only the already cached Linux arm64 image recorded below, by immutable
   image ID with `--pull=never --network=none`; source mounts are read-only and
   outputs go to private scratch. No package installation or image pull.
   First record actual compiler version/target, libc, effective fortify macros
   and the preprocessed `write` prototype's unused-result attribute.
   Compile the **original** and repaired two actual translation units to
   objects with the retained Linux Release command's `-O3 -DNDEBUG -std=c17`,
   complete existing warning set and `-Werror`, plus the existing generated
   ApplicationServices declaration stub for the helper. Compile failure must
   name the intended fault, not a missing include or unrelated option. Retain
   focused old-source variants if separating the two compiler faults is needed.
   The old failures must be observed before a repaired result is called a fix.
3. If either old translation unit does not fail for its expected warning,
   record that limitation and stop the affected compiler claim. Do not tune
   flags or fortification to force sensitivity, suppress warnings or treat a
   non-discriminating GCC build as proof. Stop for a narrower tooling decision
   if the compiler boundary remains untested. The image is not the exact
   Ubuntu x64/arm64 hosted toolchain.
4. Verify successful acknowledgment and retained killed-child prefix, actual
   failed/short child write and parent read error, prompt nonzero failure and
   no surviving owned child. Exercise initial-template fit/one-byte-overflow
   formatting neighbors before filesystem use and maximum appended-leaf
   capacities. A largest-fitting formatting case need not create an OS-valid
   directory. Put meaningful new runtime assertions in the existing test
   executables/consumer so the original CI workflow runs them. Scratch-only
   old-guard or omission variants may establish that those assertions detect
   the intended fault; do not add a permanent generic mutation framework.
5. Build the actual native targets with
   `cmake --build --preset ci --target muhtari-test-trace-protocol muhtari-test-trace-helper`;
   run the actual `policy.retroarch.preflight_api` consumer and affected
   `libretro`/`policy.retroarch` checks with a separate focused JUnit path.
   Preserve corruption/clock/write/saturation controls, callback count/order/
   values and frame/audio comparisons, exclusive descriptor ownership,
   unfinished-helper rejection, late core-write failure rejection and
   collection-before-wrapper-error. Existing synthetic controls execute no
   actual frontend or input device.
6. Run the unmodified `cmake --workflow --preset ci`, then its existing
   ordinary/release exact-inventory checks and three-package exclusions.
   Run existing format, hygiene, budgets and diff checks. Use the existing
   bounded worker mechanism and individual CTest deadlines; do not overwrite
   full JUnit evidence with focused reports. Complete one independent review
   of the changed source and actual evidence, with at most one correction
   round. Finish uncommitted with exact final hashes and a separate later
   publication decision; local success proves no packaged-host acceptance.

Cached compiler candidate (metadata inspected only; never launched here):

- tag: `gcc:15`
- image ID: `sha256:7991bc43622028874916da3d0f1aadecc795c3a72bae6a55570d21d7e62961e4`
- repository digest: `gcc@sha256:1fdcd041afad61e21d4ca0f8046e36f8da61baaa815f5ba290e39a15c487926a`
- platform: Linux arm64/v8; image configuration declares `GCC_VERSION=15.3.0`.

Native `/usr/bin/gcc` is AppleClang, and the existing native CMake cache uses
AppleClang. The Docker daemon responds and the compiler image is already cached;
actual compiler execution/version and baseline fault reproduction remain future
verification, not facts inferred from a tag.

## Frozen source boundary

| Path | Current SHA256 before any repair |
|---|---|
| `tests/libretro/test_input_trace.c` | `9b05378c10c06b5f409028048c8a52d009b693d91b4b27c437c2f0f49225dca1` |
| `tests/retroarch/test_input_trace_helper.c` | `bc8ade4e15d37ccc971a953c080ec8253cde6903678b643fef36002bf4c7d372` |
| `tests/libretro/input_trace.cmake` | `d01eff92bb57a864129e3687c9f9de309972f3b52807d5680a56b1f5602d538e` |

## Evidence commands for this read-only step

`python3 /private/tmp/muhtari-portability-preflight.py` exits 0 after fresh
complete source/receipt/service reconciliation: original 20 jobs/191 steps/
20 artifacts/19 logs; latest 20 jobs/170 steps/15 artifacts/16 logs. Each
`cmake -DINPUT=<complete-view> -DEXPECTED_SHA=<run-source> -P tools/check_ci_run.cmake`
exits 1, preserving its failed required aggregate. Run URLs are linked above.
Retained diagnostic ZIP/member/source checks exit 0 without another download.
Docker image inventory and `docker image inspect gcc:15` exit 0; no container
was launched. These metadata checks are N/A-local. Raw receipts and hashes
stay in private scratch `muhtari-portability-proposal.dvf7m85u`; no logs are
committed. `python3 /private/tmp/muhtari-portability-handoff-check.py` exits 0:
the proposal's three source hashes and local links agree, only the five intended
planning paths changed, and HEAD/index/all other tracked and untracked bytes
are preserved. Halted status, zero accepted plans and both failed audits remain;
`git diff --check` exits 0. The initial handoff assertion found a preexisting
duplicate historical `next_action`; relabelling the old WASI focus fields as
historical preserves their text and leaves one current focus/next action.

## Handoff and authority

Current disposition: the approved local execution reached its mandatory
compiler-discrimination stop before implementation. Earlier next-session
implementation wording below is historical and grants no workaround.

The owner answered **"Yes, I approve it."** on 2026-10-31, approving the exact
local-only batch above: the three allowed test paths, offline cached-image
compiler checks without pulls or installation, native/full local verification,
one bounded independent review and shared handoff bookkeeping. The proposed
stopping rules and failure-discrimination requirements remain binding.

This approval is recorded after the read-only GSD step finished. Implementation
belongs to the next session, honoring one GSD step per session. The agent owns
fresh source/service/complete-receipt checks and then the approved execution;
no further scope confirmation or owner verification is needed. Context may be
cleared. Exact next command:
`$gsd-debug continue phase12-packaged-input-tracer`.

No source edit, target execution, image launch, download/install, stage/commit,
push/PR, dispatch/rerun/cancel, actual helper/frontend/content/input activity,
allowance renewal, archive or phase promotion occurred or is authorized by
preparing this proposal. The approval grants only the local implementation,
offline compiler-container execution and synthetic checks explicitly listed
above in the next session. Publication, hosted activity, new downloads,
allowance renewal, archive and promotion remain unauthorized. The original
actual-host cause remains unresolved.

## Local execution stopped before repair (2026-10-31)

`python3 /private/tmp/muhtari-portability-preflight.py` exits0 after fresh
complete original/latest run, attempt, jobs, steps, artifacts and every executed
log reconciliation. Both required `tools/check_ci_run.cmake` audits still exit1.
The retained diagnostic ZIP digest/source/members verify0. Source/index,
advertised HEAD, operational/resolved service and noPR/noactiveCI checks exit0.
Hosted run URLs above remain authoritative failures, with no new run or attempt.

`python3 /private/tmp/muhtari-portability-gcc-old.py` exits0 as a command-receipt
collector, **not** as a portability acceptance gate. Actual immutable-image
compiler/version/target/libc/macros/preprocessed-declaration checks exit0:
GCC15.3.0, `aarch64-linux-gnu`, glibc2.41, `__USE_FORTIFY_LEVEL=0` and a `write`
prototype without `warn_unused_result`. It uses the exact cached image ID above,
`docker run --rm --pull=never --network=none --read-only`, read-only source,
private scratch output and the existing generated ApplicationServices stub
(SHA256 `1d5ef7038582caa45c63add5b9f851f21166b7b759be377bd3e6ed3b32712b4b`).

Both actual old translation units were compiled to objects with `-O3 -DNDEBUG
-std=c17`, the retained complete warning set and `-Werror`. The protocol object
unexpectedly exits0: this environment cannot distinguish the ignored-write
fault. The helper object exits1 with the three intended `format-truncation`
diagnostics and no missing-header or unrelated-option failure. Exact argv,
individual exits and raw local evidence are retained only in private scratch
`muhtari-portability-gcc.w9ev07g1/receipts.json`; no committed log.

This triggers the approved stopping rule. No flags/fortification were tuned,
no warning was suppressed, and no source repair, NEW object comparison,
synthetic runtime control, native/full workflow or independent repair review
was reached. No claim is made that either defect is fixed. Final SHA256 values
for all three test paths equal the frozen boundary table above. The original
host readiness/routing/F8/audio cause remains unresolved, accepted0/4 and
PLAY-03 pending; downstream halt and spent allowances persist automatically.

Recommend read-only preparation of a narrower compiler-verification proposal
that accounts for the hosted `write` declaration. Context may be cleared;
exact next separate command is `$gsd-debug continue phase12-packaged-input-tracer`.
The next agent owns fresh source/service/complete-receipt checks. This command
does not authorize source repair, warning/flag/fortification tuning, downloads,
installation, publication, live activity, allowance renewal, archive or phase
promotion. No owner verification or acknowledgment is needed to preserve stop.

Final `python3 /private/tmp/muhtari-portability-stop-check.py` exits0: all three
source SHA256 values still equal the frozen table; HEAD/index and every unrelated
tracked/untracked byte are unchanged. Only the five shared planning handoff
paths changed. Local links, blocked state, halted canonical summary, zero
accepted plans and both failed audits agree; `git diff --check` exits0, and no
compiler container remains active. This is stop/handoff verification,
N/A-local, not a successful portability fix or original hosted acceptance.

## Narrower tooling-verification decision (2026-10-31)

**Owner approved option1 on 2026-10-31; fixed batch finished.** The approval is
limited to the fixed seven offline invocations, private receipts and preservation/
handoff checks below. It grants no source repair or later workflow. Recommend one offline,
compile-only comparison of the unchanged protocol translation unit in the
already cached immutable GCC image. Add only `-D_FORTIFY_SOURCE=2` to one
scratch compiler command. This explicitly proposes a change that the prior
batch prohibited; no flag change or compiler execution has occurred here.
Successful tooling discrimination ends this batch. A later separate decision
is required before source repair, runtime controls or the full local workflow.

The concrete choice is whether to permit this fixed local compiler comparison.
It needs no download, installation or hosted minutes. Source remains mounted
read-only, and compiler output and receipts stay in private scratch. Keep
accepted0/4, PLAY-03 pending, downstream halt, both required failed audits and
all spent publication/live allowances automatically.

### Facts, hypothesis and limits

The retained actual Linux x64 and arm64 logs identify GNU15.2.0 and reject the
ignored acknowledgment at `test_input_trace.c:247` with
`-Werror=unused-result`, naming `warn_unused_result`. The x64 helper diagnostics
also show fortified `snprintf` header expansion. Neither printed compiler
command includes an explicit `_FORTIFY_SOURCE` definition. The hosted
effective fortification level, compiler configuration and libc version are
not independently captured; do not infer an exact level or complete toolchain
match from these logs.

The cached GCC15.3.0/glibc2.41 metadata and object receipts directly establish
the different local boundary: fortify0, no `write` unused-result attribute,
old protocol0 and old helper1. In glibc2.41,
[`unistd.h`](https://raw.githubusercontent.com/bminor/glibc/glibc-2.41/posix/unistd.h)
declares `write` with `__wur`, while
[`sys/cdefs.h`](https://raw.githubusercontent.com/bminor/glibc/glibc-2.41/misc/sys/cdefs.h)
enables that annotation when fortification is positive. The
[glibc manual](https://sourceware.org/glibc/manual/latest/html_node/Source-Fortification.html)
documents command-line fortification and additional unchecked-return
diagnostics; the
[GCC warning manual](https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html#index-Wno-unused-result)
ties unused-result diagnostics to that annotation.

Hypothesis: the fixed scratch definition will activate the real cached libc
annotation and make the unchanged ignored write fail under the unchanged
warning set. This is unexecuted. Level1 would also satisfy the header's
positive-level condition; level2 is a fixed documented comparison, neither
uniquely necessary nor asserted to match hosted defaults. The local compiler
and architecture still differ from hosted x64, and this comparison proves
neither repaired source nor actual packaged input.

### Exact proposed batch and oracle

1. Agent refreshes complete original/latest run/attempt/jobs/steps/artifacts/
   executed-log receipts, source/index/protected-byte hashes, remote source,
   service health, open PRs and active CI. Confirm the unchanged three hashes
   in the frozen table and the immutable image ID above. A mismatch stops.
2. Reuse the existing strict flags verbatim:
   `-O3 -DNDEBUG -std=c17 -Wall -Wextra -Wpedantic -Wshadow -Wconversion
   -Wsign-conversion -Wstrict-prototypes -Wmissing-prototypes -Wvla -Wundef
   -Wcast-qual -Wimplicit-fallthrough -Wmissing-variable-declarations
   -Wduplicated-cond -Wlogical-op -Werror`.
   Use the same protocol include paths and unchanged actual
   `tests/libretro/test_input_trace.c`. Only the variant adds
   `-D_FORTIFY_SOURCE=2`; do not redefine libc internals, inject declarations,
   change warnings or optimization, or alter any repository configuration.
3. Fixed work: one compiler/version/target/libc metadata command; for each
   baseline and variant, one `-dM -E -include unistd.h -x c /dev/null` macro
   command, one `-E -P -include unistd.h -x c /dev/null` declaration command,
   and one `-c` command for the frozen actual protocol source. There are seven
   offline container invocations in this enumerated batch, no sweep/retry.
   Each retains the existing60s subprocess deadline. Use the exact cached
   image ID, `--rm --pull=never --network=none --read-only`, read-only `/src`,
   private scratch `/work`, and the existing temporary-files mount. Record
   each container's owned ID before use; on timeout/error the driver removes
   only its own container and verifies no owned container survives. Failure
   to confirm cleanup stops; no replacement execution follows.
4. Required baseline: effective fortify0, an unannotated real `write`
   declaration, protocol object0. Required variant: effective fortify2,
   `warn_unused_result` on that real declaration, object1 naming the same
   ignored write and `-Werror=unused-result`. Macro/declaration commands must
   exit0. Preserve exact argv, individual exits, diagnostics and hashes in
   private receipts; collector exit0 alone is never success. Any unrelated
   error, missing expected attribute/diagnostic, different exit, timeout or
   protected-byte change ends the batch without another flag variant.
5. Verify unchanged source/index/all unrelated bytes and stopped owned
   containers, record the comparison outcome in these existing planning
   files, then stop. No C program is linked or executed, no synthetic child
   or actual frontend/input runs, and the previously proposed repair/full
   workflow/repair review is not entered. No hosted dispatch or publication
   allowance is requested or implied.

The old-helper truncation result remains retained evidence; recompiling that
unrelated translation unit adds no discriminating observation to this narrow
question. Success would establish only a local environment capable of
rejecting the known ignored-call defect. Failure preserves the stop and
requires a separate evidence-supported tooling decision; no other image,
fortification level or warning workaround is authorized.

### Alternatives and independent review

| Choice | Consequence |
|---|---|
| Keep stopped | No new execution or cost; neither compilation blocker is repaired. |
| Fixed cached-image baseline/fortify2 comparison | Recommended smallest experiment: isolates the documented annotation boundary without a download or source change. It establishes local diagnostic sensitivity only. |
| Acquire an exact Ubuntu toolchain or repeat hosted execution | Could reduce environment differences, but needs a separate acquisition or live allowance and broader provenance/cost review. Current evidence does not justify it before this cheaper comparison. |

One independent bounded tooling specialist checked the header mechanism,
oracle, limits and stopping rule; the parent separately reconciled receipts
and hosted command evidence and synthesized once. The reviewer agreed that
level2 is a valid fixed test but not a uniquely required or proven hosted
setting. No repair review was consumed and no source fix is claimed. No
further specialist expansion would change this decision without new evidence.

### Fresh evidence and handoff

`python3 /private/tmp/muhtari-portability-preflight.py` exits0: original
20jobs/191steps/20artifacts/19logs and latest20jobs/170steps/15artifacts/16logs
match all retained complete receipts, with both required CMake audits1.
The retained diagnostic ZIP/source/member checks exit0, service is
operational/resolved, advertised source matches, and no open phase PR, active
CI or new source run exists. Raw receipts stay in private scratch
`muhtari-portability-proposal.mf3hgi5i`. Hosted run URLs above remain failed.
`docker image inspect <immutable-ID> --format ...` and targeted `docker ps`
each exit0: exact Linuxarm64 image remains cached, no compiler container is
active. These are metadata-only reads; no container or target launched.

Final `python3 /private/tmp/muhtari-portability-stop-check.py` exits0: all three
frozen source hashes, HEAD/index and unrelated tracked/untracked bytes are
unchanged; only the five allowed planning paths changed. Local links, halt,
accepted0/4, both failed audits, diff check and no active compiler container
agree. `git diff --check` exits0. These verify read-only proposal/handoff scope,
not a compiler comparison, repair or hosted acceptance.

Awaiting the owner's NEW tooling-only scope decision, not verification of a
fix or acknowledgment to stay stopped. Context may be cleared. Exact next
separate command: `$gsd-debug continue phase12-packaged-input-tracer` with the
tooling decision. Agent owns fresh receipt/source/service checks and later
comparison execution if authorized. Invocation alone grants no execution,
source repair, download/install, staging/commit, push/PR, hosted dispatch/
retry/cancel, actual frontend/content/input, allowance renewal, archive or
phase promotion.

## Approved fixed tooling comparison outcome (2026-10-31)

The owner selected option1 for exactly the seven-invocation offline comparison
above. `python3 /private/tmp/muhtari-portability-preflight.py` exits0: complete
original20jobs191steps20artifacts19logs and latest20jobs170steps15artifacts16logs
still agree, both required audits remain1, retained diagnostic identity agrees,
official service is operational/resolved, no open phase PR/active CI/new source
run exists, and advertised source/HEAD/index/protected bytes remain unchanged.
Run URLs are linked above; no hosted action occurred.

`python3 /private/tmp/muhtari-portability-fortify-comparison.py` exits0 after
exactly seven invocations in the immutable cached image. Every invocation used
`--rm --pull=never --network=none --read-only`, read-only `/src`, private `/work`
and the existing temporary-files mount, with60s deadline and no retry. Unique
owned names were reserved before launch; Docker wrote each `--cidfile` before
start, and the driver retained all seven exact IDs and confirmed cleanup after
each. No program was linked or executed.

| Invocation | Exit | Observed oracle |
|---|---|---|
| Compiler/target/libc metadata | 0 | GCC15.3.0, aarch64-linux-gnu, glibc2.41 |
| Baseline macros | 0 | Effective fortify0 |
| Baseline real declaration | 0 | `write` has no unused-result annotation |
| Baseline actual protocol object | 0 | Object produced, no diagnostics |
| Fixed fortify2 macros | 0 | Effective fortify2 |
| Fixed fortify2 real declaration | 0 | `write` has `__warn_unused_result__` |
| Fixed fortify2 actual protocol object | 1 | Sole error at line247: ignored `write`, `-Werror=unused-result` |

Only the variant adds `-D_FORTIFY_SOURCE=2`; strict warnings, optimization,
include paths and frozen actual source stay identical. There was no unrelated
error or timeout. The retained old-helper truncation failure was not recompiled.
Exact commands, individual exits, owned IDs, raw diagnostics and source/output
hashes stay in private scratch `muhtari-portability-fortify.hg7v7vtf`; fresh
complete preflight receipts stay in `muhtari-portability-proposal.k0z4vz_a`.
These are N/A-local; no logs are committed.

This establishes a local environment capable of rejecting the known ignored
call. It does not establish exact hosted fortification/compiler/libc equivalence,
repair either source blocker or resolve original packaged-input readiness/
routing/F8/audio. All three source hashes equal the frozen table, HEAD/index
and unrelated bytes remain unchanged, and every owned container is stopped.
No source repair, synthetic child, actual frontend/input, native/full workflow,
repair review, download/install, stage/commit/push/dispatch/rerun/cancel, archive,
phase promotion or allowance renewal followed. Accepted0/4, PLAY-03 pending,
downstream halt, both failed audits and all spent allowances persist.

Stop this completed tooling batch after final handoff preservation checks.
`python3 /private/tmp/muhtari-portability-fortify-handoff-check.py` exits0:
exact seven payloads/exits/oracles/output hashes and all owned cleanup agree;
frozen source/HEAD/index/unrelated bytes unchanged, only five allowed planning
paths changed, links/halt/both failed audits/diff0. No repair claim follows.
No owner verification or acknowledgment is needed. Context may be cleared;
exact next separate command `$gsd-debug continue phase12-packaged-input-tracer`
prepares a revised narrow local portability-repair proposal read-only. The
agent owns fresh source/service/complete-receipt checks; a later separate
decision is required for source repair. Invocation alone renews nothing.
