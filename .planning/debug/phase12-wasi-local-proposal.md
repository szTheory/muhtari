---
status: complete-local
created: 2026-10-31
scope: One scratch toolchain download and focused local WASI mock verification
authority: Owner answered yes to this exact finite local batch; execution permitted within its bounds
---

# Focused local WASI execution proposal

Owner subsequently answered **"yes"** to this concrete proposal. The batch is
now finished with focused local WASI verification passing. Earlier
awaiting-authority/preparation-only wording below describes
the preserved proposal history. The current execution disposition is recorded
in the active debug session and appended below, without renewing any
host/publication allowance or changing the approved limits.

The approved [mock transport repair](phase12-held-capture-diagnosis.md#approved-local-wasi-transport-batch-2026-10-31)
is locally verified and uncommitted. Its remaining local gap is execution of
the complete C mock as WASI. This proposal requests a new, finite tooling and
execution scope. No download, installation or target execution occurred here.
The packaged RetroArch input failure remains unresolved and stopped.

## Recommendation and alternatives

Use one official WASI SDK28 arm64-macOS release bundle privately under a new
temporary directory. Use its compiler, linker, builtins and bundled sysroot
together, with the existing pinned Wasmtime48.0.6. This avoids assembling an
untested combination of separately extracted tools. Repository sources,
toolchain files, pins, inventories and workflows stay byte-identical.

| Choice | Result and tradeoff |
|---|---|
| Keep stopped | No tooling cost; complete-C local WASI execution remains unverified. This remains safe without acknowledgment. |
| Fetch only a linker | Insufficient: the installed compiler also lacks WASI builtins. Its driver selects both missing inputs. |
| Mixed linker and builtins | Roughly2.48MB compressed, but adds Homebrew bottle relocation/dependency and compiler/resource/sysroot compatibility questions. Not selected or authorized as a fallback. |
| **One coherent SDK bundle** | About106.24MiB compressed and one private extraction; supplies the missing compiler/linker/runtime resources together. Recommended for the precise local gap. |
| Full hosted WASI CI | Would exercise the actual CI toolchain and full inventory, but needs publication and hosted authority. Not part of this proposal and does not prove RetroArch input. |

The preparation described LLVM21.1.0 from a release-note summary; the actual
verified SDK tools report LLVM21.1.4 at the SDK's recorded LLVM submodule
revision, as the execution correction below records. CI uses clang21 and its own
wasi-libc; the retained local CI sysroot is
`0.0~git20250726.3f7eb4c-4`. SDK28's sysroot is a different snapshot. A pass
would establish actual complete-C behavior in this named local target/runtime
combination, not exact hosted-toolchain equivalence, the full WASI suite,
cross-platform hash equality or any host-input acceptance. Existing compilation
against retained CI headers remains complementary evidence. Do not switch
sysroots or tools after a failure within this scope.

## Exact proposed tool and trust boundary

| Field | Value |
|---|---|
| Release | [WebAssembly/wasi-sdk, wasi-sdk-28](https://github.com/WebAssembly/wasi-sdk/releases/tag/wasi-sdk-28) |
| Asset | [wasi-sdk-28.0-arm64-macos.tar.gz](https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-28/wasi-sdk-28.0-arm64-macos.tar.gz) |
| Compressed bytes | 111397330 |
| SHA256 | `092d1859ad6338de40c16ee0779102ff19c067dd863ad816ba5afd8bd8cb709f` |
| Metadata authority | [Official release API](https://api.github.com/repos/WebAssembly/wasi-sdk/releases/tags/wasi-sdk-28), freshly read during preparation |
| License | [SDK license at tag](https://github.com/WebAssembly/wasi-sdk/blob/wasi-sdk-28/LICENSE): Apache-2.0 WITH LLVM-exception; retain bundled component notices and inspect libc notices before use |

The official digest proves agreement with the published asset, not independent
reproducible-build provenance. Re-read exact metadata before a permitted fetch;
stop if version, URL, digest or size differs. Verify downloaded size and digest
before extraction or execution. Inspect archive members first: reject absolute
or escaping paths/links, devices and expansion beyond the scratch allowance.
Retain relevant notices; do not copy upstream code into the repository. Use
only the selected native compiler/linker resources and WASI sysroot. No package
manager, persistent installation, PATH edit, runtime replacement, download of
content or additional asset is included.

## Requested finite batch

1. Agent refreshes source identity, service health and original complete CI
   receipts. Freeze all current worktree bytes and index; stop on unexpected
   source changes. Preserve every existing owner edit. This scope includes
   parent-owned documentation/evidence bookkeeping only in this proposal,
   the active debug session, STATE, LESSONS and the canonical halted summary.
2. Fetch this one exact asset once over HTTPS, following its normal release
   redirect. No retry, resumable second request or fallback candidate. Bound
   the download to300seconds and111397330bytes; stop on interruption, mismatch
   or failure. Read-only official metadata/license requests remain allowed.
3. Inspect and extract under a new private temporary directory only. Allow
   at most2GiB total scratch bytes for compressed tool, extracted members,
   builds and receipts. This is a requested disposable-work allowance, not a
   new repository resource gate. Check free space and expanded member sizes
   before extraction; if it will not fit, stop rather than raising the cap.
4. Build the actual unchanged `tests/policy/retroarch_preflight.c`, including
   the actual helper source, as genuine `wasm32-wasi`. Use SDK paths only in
   the private build, explicit target, `lime1`, signal emulation/library,
   `--stack-first` and the existing strict warning function. Do not fake WASI
   by defining `__wasi__` on a native build. Record compiler/linker/sysroot
   and pinned-runtime versions, hashes, actual arguments and module identity.
5. Run the focused valid build and the ten specified scratch counterfactual
   builds below, once each. Use60seconds per configure/build/observer,
   15seconds per directly invoked target process, and the owning test
   deadlines of5seconds times the unchanged WASI scale2. Stop at20minutes
   of verification after download or25minutes total from before download,
   whichever occurs first. Terminate only this batch's attributable scratch
   processes on timeout. No retry or source repair is included.
6. Perform one bounded evidence review and parent synthesis; record commands,
   exits and N/A-local, preserving original failed hosted run links. Raw logs
   stay scratch. Stop this GSD step without a commit or publication.

These limits bound the cost and lifetime of one disposable experiment; they
do not alter any repository test timeout or CI budget. A new choice is needed
if the tool cannot load on this host or any dependency is missing.

## Focused acceptance and falsification

Use a private CMake project executing the exact owning header/target/registration
prefix in `tests/policy/tests.cmake` through `policy.retroarch.child_failure`,
plus the unchanged `muhtari_add_test` and warning functions. Record extracted
byte identities. Compile the actual target; importing a native executable
stand-in is insufficient. Set only private compiler/sysroot/build/runtime
paths and the existing WASI timeout scale. Preopen at most the scratch working
directory to the runtime; no content or writable repository mapping is needed.

Inspect generated CTest commands before execution: exactly
`policy.retroarch.preflight_api` and `policy.retroarch.child_failure`, actual
WASI module, complete pinned-runtime command list, child option, expected125
and unchanged exact-code/marker observer. Run ordinary valid/fault cases to0,
child mode directly to125 with
`child exec failure: argv preserved, no input or wait`, then the owning two
registrations to0. The semantic helper127/argv/one-exec/no-post/no-wait/no-kill
assertions must still precede the marker.

Proposed focused command, after the private build exists:

```sh
ctest --test-dir "$scratch/build" \
  -R '^policy[.]retroarch[.](preflight_api|child_failure)$' \
  --no-tests=error --output-on-failure \
  --output-junit "$scratch/focused-wasi.xml"
```

Require exactly these two results. Do not overwrite the full native or WASI
JUnit files, or pass this focused report to a full-inventory consumer.

Replay existing counterfactuals as linked WASI C: helper exit0, helper exit126,
wrong argv, child post, child wait, child kill, omitted termination, outward124,
missing marker, and the frozen pre-repair mock from current HEAD exporting127.
Each must reject through unchanged `expect_fail.cmake` expecting125 and the
same marker. The six semantic faults must fail before printing the marker.
The pre-repair127 case must reproduce runtime rejection despite its marker.
This demonstrates the actual C repair matters, beyond the independent WAT
fixtures already run. Mutations live in scratch copies only.

Stop on a failed valid case, surviving mutant, missing registration, unexpected
status, tool incompatibility, source drift or resource cutoff. Preserve the
remaining gap; do not weaken assertions, change tools or infer new authority.
Success closes only this focused local execution gap. Full hosted WASI remains
advisory/unaccepted, PLAY-03 pending, accepted0/4 and downstream plans blocked.
The real helper still exits127; the hosted input deadline, permissions, content
and prior spent allowances remain unchanged. No actual helper/frontend/content,
commit/stage/push, dispatch/rerun/cancel, archive or phase promotion is requested.

## Read-only preparation evidence and review

Two disjoint bounded specialist assessments covered tool choice/provenance and
the execution/registration evidence contract. The parent adversarial synthesis
resolved their sysroot interaction in favor of the coherent bundled sysroot,
explicitly limiting the resulting claim. The alternative retained CI sysroot
could improve correspondence but adds unmeasured mixed-tool compatibility;
it is not an automatic fallback. Reconsider only if new evidence changes that
choice. No further specialist expansion is needed.

All commands below have run URL N/A-local unless the existing
[failed run](https://github.com/szTheory/muhtari/actions/runs/41158640754) is named.

| Command | Exit | Evidence |
|---|---:|---|
| `python3 /tmp/muhtari-wasi-tool-proposal.5zh8vi74/refresh.py` | 0 | Fresh operational/resolved health; exact terminal source/attempt/all20jobs/191steps/20artifacts unchanged; no open phase PR or nonterminal CI. Existing required audit remains1. |
| `gh api repos/szTheory/muhtari/actions/runs/41158640754/attempts/1` and same endpoint plus `/jobs?per_page=100` | 0 each | Exact attempt1 source/completion and full job payload agree with preserved receipts. Connector job/step/artifact snapshots independently agree. |
| `python3 /tmp/muhtari-wasi-tool-proposal.5zh8vi74/inspect.py` | 0 | Official exact asset metadata; compiler driver-only plan selects absent linker and builtins; full source/worktree/index/HEAD unchanged. No binary generated, package fetched or target executed. |
| `python3 /tmp/muhtari-wasi-tool-proposal.5zh8vi74/final.py` | 0 | Final exact five documentation edits only, protected source/worktree/index/HEAD, halt/acceptance/authority consistency, relative links, existing hygiene/diff/budget checks and summary parser. No target tests are claimed or run in this read-only preparation. |

This proposal is not execution authority.

## Decision and next step

Recommend approving exactly the finite local batch above. The new authority
is needed because the prior batch expressly excluded tool downloads and further
target execution. No acknowledgment is required to retain the existing halt.

Read-only preparation is finished once final checks pass. Context may be
cleared. Exact next separate command:
`$gsd-debug continue phase12-packaged-input-tracer`, accompanied by approval or
revision of this proposal. The agent owns fresh source/service/receipt checks
and every permitted execution/review; the owner verifies nothing. Without that
new decision the agent keeps downloads and execution stopped automatically.

## Approved execution outcome (2026-10-31)

The owner answered **"yes"** to this exact finite local batch. It is now
finished: the complete actual C mock was linked and executed as WASI using
the approved SDK's bundled toolchain/sysroot and existing pinned Wasmtime.
Both actual owning test registrations passed, and all ten linked WASI
counterfactuals were rejected. The bounded read-only evidence review returned
PASS with no blocking findings; parent synthesis agrees. This closes the
precise **focused local complete-C execution gap** left by the prior batch.

No repository source, test, build, workflow, pin, runtime or inventory file
changed. The earlier three-file repair remains uncommitted. No actual helper,
frontend, cartridge or other content was launched; mocked permission/event/
fork/exec operations are not operating-system input evidence. Nothing was
installed persistently, staged, committed or published. Original full native
JUnit files remain unchanged; this two-test report is private and focused.

### Tool identity, notices and bounds

Fresh source/service/original complete-attempt checks passed before the sole
download. The reserved fetch began at03:26:26Z; the exact111397330-byte asset
matched SHA256`092d1859ad6338de40c16ee0779102ff19c067dd863ad816ba5afd8bd8cb709f`.
Archive path/link/type/size inspection passed;364439587 expanded bytes were
extracted privately under `/tmp/muhtari-wasi-exec.ludxi_mr/sdk`. Scratch use
was about481MB, below the approved2GiB. There was no retry or fallback.
Original total cutoff03:51:26Z and verification cutoff03:46:35Z were never
restarted; target checks finished at03:29:34Z, within both.

The bundle included no license-notice files. Before executing its tools,
the agent retained the SDK license and official LLVM/libc/component notices
at the SDK's recorded revisions in scratch. SDK/LLVM use Apache2 with LLVM
exceptions; libc identifies Apache/MIT plus its CC0/BSD/MIT components.
Exact notices, source URLs and digests are in `license-review.json`; no source
was reused in the repository.

| Component | Actual verified identity |
|---|---|
| SDK | Exact approved SDK28.0 arm64-macOS asset and archive SHA above |
| Compiler/linker | Clang21.1.4-wasi-sdk / LLD21.1.4, LLVM revision`222fc11f2b8f25f6a0f4976272ef1bb7bf49521d` |
| Bundled libc | SDK submodule revision`d181bdd429f0067c92d817909b01e4970ad7d70a`; differs from CI |
| Runtime | Existing Wasmtime48.0.6, commit`72bc45bd1`; binary hash frozen and unchanged |
| Actual valid WASI module | SHA256`8f9e55e66372dea0dd8299b0abe03573620f0554ad627b1f397ffdede6d0936c` |

`build-identities.json` records protected source hashes, extracted owning
prefix/registration/warning-function hashes, tool hashes, bundled-sysroot
manifest identity and module hash. Actual macro inspection confirms WASI/
wasm32; the build uses explicit`wasm32-wasi`, bundled sysroot, strict warnings
as errors,`lime1`, signal emulation/library and`--stack-first`. Scratch source
symlinks resolve to the actual unchanged repository mock/helper/observer.

### Preserved scratch-driver correction

The initial driver exited1 on an incorrect exact compiler-version assertion:
the preparation's release-note summary said21.1.0, while the approved asset's
tools identify21.1.4. All three version commands had exited0. No target build
or target process had yet run. The agent verified that the reported LLVM
commit exactly matches the SDK's recorded submodule and corrected this
descriptive metadata check, retaining the original failed driver receipt.

Continuation reused saved version output and required an empty target-build
state; it did not repeat a version command, download, target build/attempt or
deadline. The selected asset, checksum, SDK version, compiler major, sysroot
and semantic acceptance conditions stayed unchanged. The review found this
documented metadata correction was not an authority or target-evidence blocker.
No relaxation of valid-case, marker/status, ownership or mutation checks occurred.

### Actual target results and falsification

Direct ordinary execution exited0. Child mode exited125 after the actual
helper127/argv/one-exec/no-input/no-wait/no-kill assertions, with the exact
required marker. CTest generated exactly the two intended registrations:
actual WASI module through the complete pinned runtime command list, the
unchanged exact125-plus-marker observer and10-second owning deadlines.
Focused JUnit records both passing and no skipped case.

| Scratch WASI counterfactual | Required observation, reproduced |
|---|---|
| Helper exit0 or126; wrong argv; child post/wait/kill | All six observers exit1; semantic fault prevents the marker |
| Omitted termination | Observer exits1 despite marker; return into helper continuation cannot satisfy125 |
| Outward124 | Observer exits1 on the wrong exact transport status |
| Missing marker | Observer exits1 although actual status is125 |
| Frozen pre-repair127 mock from current HEAD | Observer exits1; marker exists but pinned Wasmtime rejects127 as outside its accepted status range |

The final row demonstrates the repaired complete C mock, rather than only an
independent WAT fixture, resolves the local transport failure. Nothing here
establishes hosted input delivery or changes its prior failed evidence.

### Commands and exits

All new commands have run URL **N/A-local**. Existing required hosted verdict
remains failure at [run41158640754](https://github.com/szTheory/muhtari/actions/runs/41158640754);
no new hosted execution exists. Raw logs stay scratch.

| Command | Exit | Establishes |
|---|---:|---|
| `python3 /tmp/muhtari-wasi-exec.ludxi_mr/refresh.py` | 0 | Fresh exact terminal source/attempt/all20jobs/191steps/20artifacts, service operational/resolved, no open phase PR/nonterminal CI; required audit remains1. Run before download and again at final reconciliation. |
| `python3 /tmp/muhtari-wasi-exec.ludxi_mr/preflight.py` | 0 | Direct exact-attempt full job payload, official exact asset and SDK license, frozen reviewed source/runtime and protected edits. |
| `python3 /tmp/muhtari-wasi-exec.ludxi_mr/download.py` | 0 | Reserved sole fetch, exact size/digest, archive/member/expanded-size validation and private extraction within original bounds. |
| `python3 /tmp/muhtari-wasi-exec.ludxi_mr/licenses.py` | 0 | Official component license/notice documents retained at exact SDK submodule revisions before tool execution. |
| `python3 /tmp/muhtari-wasi-exec.ludxi_mr/execute.py`, initial invocation | 1 | Incorrect descriptive compiler-version assertion before any target build; preserved failure, not a failed target test. |
| `python3 /tmp/muhtari-wasi-exec.ludxi_mr/execute.py continue-after-metadata` | 0 | Same reserved batch resumes before target build, actual WASI macros/build/identity, direct0/125, two owning tests0, ten target builds0 and their negative observers1. No repeated target attempt. |
| `python3 /tmp/muhtari-wasi-exec.ludxi_mr/final.py` | 0 | Final five documentation-only changes, unchanged source/other edits/HEAD/index/full JUnit/runtime, expected results and resource bounds, halt/acceptance/next-action consistency, links, hygiene/diff/budgets/parser. |

### Remaining limits and handoff

This SDK's libc differs from the hosted lane. Local focused verification is
now established; full hosted WASI inventory/hash/CI-toolchain acceptance
remains advisory and unaccepted. The real RetroArch acknowledgment/readiness/
routing failure, PLAY-03 pending, accepted0/4, downstream blocks, failed history
and all spent host/publication allowances remain. The download/batch authority
is finished and grants no additional target run or tool fallback.

No owner verification or acknowledgment is needed. Context may be cleared.
Exact next separate command `$gsd-debug continue phase12-packaged-input-tracer`
is a read-only assessment of the smallest useful hosted-input readiness/routing
experiment and any concrete next scope, with fresh source/service/receipt
checks owned by the agent. No further source repair, target/helper/content
execution, staging/commit/publication, dispatch/rerun/cancel, new host window,
archive or phase promotion is authorized by that invocation. Stop this
finished local GSD step; the owner acts next only to start a separate step.
