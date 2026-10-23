---
phase: 05-pipeline-tune-up
plan: "15"
status: complete
halted: false
tasks_completed: 3
subsystem: infra
tags: [release, cleanup, attestations, measured-scheduling]
requires:
  - phase: 05-13
    provides: Accepted ordinary aggregate and retained legacy domain guards
  - phase: 05-07
    provides: Accepted controlled performance arming and advisory lane disposition
provides:
  - Empty204-safe status/body handling and complete retained-ID lifecycle controls
  - Checked workflow outputs and verified patched CLI provisioning
  - Measured publisher priority and final tightened physical accounting
affects: [05-09]
requirements-completed: []
requirements-contributed: [TUNE-02, TUNE-03, TUNE-05]
coverage:
  - id: trusted-allocation
    description: Owner-approved D-35 allocation is adopted against the independently identified actual base
    requirement: TUNE-03
    verification:
      - kind: integration
        ref: Remote main and PR20 commit/tree APIs plus git ls-tree; exit0; base65b360d has no quality.json
        status: pass
      - kind: integration
        ref: Existing quality_join with authenticated predecessor9 and independent absent-policy base; exit0; structural adoption15365 before affected GREEN
        status: pass
    human_judgment: false
  - id: finite-lifecycle
    description: Real publisher retains typed complete lookup, explicit HTTP status/body and release-before-tag confirmation
    requirement: TUNE-02
    verification:
      - kind: unit
        ref: cmake --workflow --preset ci; exit0; policy.publish.controls covers LF/CRLF204, malformed/failed/contradictory responses, known IDs, duplicate/empty pages, ordering and conditional callers
        status: pass
    human_judgment: false
  - id: checked-callers-and-pin
    description: Actual workflow blocks propagate failures and both attestation jobs consume the verified supported CLI
    requirement: TUNE-05
    verification:
      - kind: unit
        ref: RED7961201 exits8 and unchanged JUnit classifies RED_EVIDENCE_OK; GREEN592656c and final policy.publish.controls exit0
        status: pass
      - kind: integration
        ref: sh scripts/fetch-pin.sh gh-linux-x86_64 with an isolated destination; exit0; official2.102.0 Linux archive digest and ELF verified locally
        status: pass
    human_judgment: false
  - id: measured-priority
    description: Publisher priority improves matched local full-inventory latency without dropping executions
    requirement: TUNE-02
    verification:
      - kind: other
        ref: python3 build/plan-evidence/release-continuation/compare-publisher-order.py atdb24f4a; exit0; two workers, ABBA,610 exact executions each,45.6775s before and37.3475s after
        status: pass
    human_judgment: false
  - id: complete-count-and-local
    description: All147 whole-file entries, embedded controls and glue fit the approved allocation and tightened actual
    requirement: TUNE-03
    verification:
      - kind: integration
        ref: Final local ci610+6, hygiene24 and generated-corpus library43 workflows plus exact check_junit judges; exit0
        status: pass
      - kind: other
        ref: gate_count and complete component audit; exit0; actual15287, safety447, fixes477, N2404, broad15346; budgets19 exit0
        status: pass
    human_judgment: false
  - id: current-remote-acceptance
    description: Final direct9, called8 and genuine patched-CLI publication/download/install/confirmed cleanup are accepted
    requirement: TUNE-05
    verification:
      - kind: e2e
        ref: https://github.com/szTheory/muhtari/actions/runs/39879193530 attempt1 and https://github.com/szTheory/muhtari/actions/runs/39879193669 attempt2; both success; authenticated9/8 joins, full610+6 native/BE, genuine gh2.102.0 publication/download/install/cleanup; independent absence exit0
        status: pass
    human_judgment: false
plan_head_before: 13d6b23
plan_head_after: 098bb5e767b35e34e8f6ffb9545612a4d9d594e8
completed: 2026-10-22
---

# Release safety continuation

This latest record is **accepted3/3** under owner-approved D-35. All final
local/hosted/genuine acceptance has passed.15 releases09 for its final outcome
record; no further production/control/documentation edit is required.
The original15 halt remains immutable in Git atca64a29;14 and earlier historical
PLAN/SUMMARY/HANDOFF files are unchanged. No plan16 or new gap/review round exists.

## Finite repair and lineage

Task1 RED457a095/GREEN444b657 repairs empty204 handling with a sentinel that
preserves real trailing bytes through command substitution. Header/body boundaries,
command/status agreement, typed complete listings and retained-ID cleanup remain.
Task1 was accepted at15163/safety323. Historical14 Task1/RED99ceff0/ec95123 and
08 REDe0c18a0 remain ancestors; their partial acceptance is not rewritten.

Task2 REDe2e6cbd exercises the complete lifecycle/download queues; RED7961201
reproduces a concealed failed state lookup. Explicit checked assignments prevent
failed create/state commands from emitting success outputs. Draft state must be
exactly true or false. Both Ubuntu attestation jobs reuse fetch-pin.sh for official
gh2.102.0, SHA256bb766f710eef8ede859c18578c72c327597cd4c8a85b06001b1f3843c6019386,
then add only the verified bin directory to subsequent PATH and record its version.
Provisioning failure has no image-binary fallback.

The real-script queue covers successful empty/later-page listings; malformed,
partial, failed and duplicate listings; positive integer IDs; create state/status;
LF/CRLF204 and status/body/command contradictions; retained known IDs; idempotent
absence; release-first deletion with fresh complete listing and exact resource
confirmation; forbidden mutations; actual conditional/substitution callers; and
genuine/downloaded signer/ref/version/tamper invocation paths. Synthetic checks
prove invocation and short-circuit behavior; genuine upstream acceptance belongs
to Task3. Repo, full signer workflow and intended source ref remain separate from
archive/commit/firmware/hash identity. No source-digest shortcut is introduced.

## Hosted integration and measured scheduling

Earlier final-source attempts exposed real integration issues rather than a new
scope decision.30-second Windows/Intel-Mac control timeouts were censored evidence.
The expanded Windows positive path also exposed the checksum fixture's native
format mismatch. It now uses the host shell checksum tool, including its native
mode marker and line endings, without weakening the production byte comparison.
The direct/calleddb24f4a executions pass the full control; only the called trusted
base fails its old OpenBIOS timeout. Those are predecessor evidence after the
subsequent scheduling-only edit, not acceptance of098bb5e.

The completed current-workload matrix measured max724.35s on Windows arm64 and
63.21s on Intel Mac. Provisional1450s/130s control bounds derive from those prior twice-max samples;
Windows job34min derives from2x1013s, other native10min from2x286s. ASan10min
accounts for complete independent current/base work; PowerPC compile15min includes
slow compiler downloads. Small image/caller cohorts remain separate and provisional;
failed/cancelled/censored observations are not successful timeout samples.

A matched ABBA experiment atdb24f4a uses the same compiled source, fixtures,610
names, two workers and fresh scheduling history. Current costs take47.281/44.074s;
publisher-first takes37.910/36.785s: mean45.6775→37.3475s, **18.24% lower wall**.
Every sample passes exact Counter membership and check_junit. The experiment
restores generated files;098bb5e applies COST725 in the existing registration.
An earlier equal-cost comparison showed no speedup and is not presented as a gain.
No hosted/global improvement or low-power support is inferred from this local pair.

## Final fixed accounting

D-35 explicitly authorizes maximum safety512/fixes542/N2482/C15365/strict15353.
Final code tightens the sole gate metric to **15287**, with B12885/S2/N2404.
Versus accepted7ed793e actual14840, safety is447 and fixes are477. Components:

| Component | Final actual |
|---|---:|
| Counter/inventory |368|
| BE integration |193|
| Digest |447|
| Ordinary quality |919|
| Correctness/release safety |477|
| Reserve |0|

All147 entries are whole files, including counter, inventory, controls, configuration
and embedded glue. Zero-line-delta edited policy/caller paths are still audited.
Retired allowance is78: safety65, BE12 and quality1; none funds another component.
Broad sensitivity is15346, adding the unchanged59-line product RetroArch wrapper.
Full/authored source denominators8580/6533 remain advisory; no old hard ratio is
claimed. The pre-D-35 strict15225 is historical, not the current approved ceiling.

Post-final-edit local commands all exit0: complete ci610+6, hygiene24, library43
with verified origin/main and generated public corpus, all three exact JUnit
judges, gate_count/component audit, budgets19 and tree hygiene. Repeated parallel/
random freshness checks retain independent output ownership and exact stale-page
rejection. README built/latest installation and read-only settings checks pass.

## Task3 accepted: final immutable identities

Final implementation is098bb5e767b35e34e8f6ffb9545612a4d9d594e8. Actual main/PR base
is65b360d1afd37b7dad9f94dcff497c85df84fd1d, tree0f712e7a9df091a3dbaf5bc176a538a0a5869cd5;
git ls-tree proves quality.json absent. The independent trusted bootstrap policy is
materialized from that actual tree, not taken from a candidate artifact. Raw ZIP
bytes/digests are preserved; only native text line endings are normalized for the
independent comparison, matching the existing CMake reader.

| Role | Run/attempt | Tested producer | Result |
|---|---|---|---|
| Direct | [39879193530](https://github.com/szTheory/muhtari/actions/runs/39879193530),1 | bbc4da9172ee3983d361f5c7e67dadda50d4ef79 | Success;9 authentic quality artifacts; six native hash files agree; library43, WASM and RetroArch execute |
| Called/rehearsal | [39879193669](https://github.com/szTheory/muhtari/actions/runs/39879193669),2 | 098bb5e767b35e34e8f6ffb9545612a4d9d594e8 | Success;8 authentic quality artifacts; intended perf absence; genuine publication/install/confirmed cleanup |

The direct producer's authenticated parents are base65b360d and raw head098bb5e.
Every original artifact is authenticated against the API run/ID/immutable SHA256,
exact safe members and its actual successful producing job/attempt before reduction.
Called items reused from attempt1 stay attributed to their real original jobs;
copied retry metadata is not another execution. The failed original Windows x64
job120172221417 passes current610+6 but times out two old-base fixtures. An unchanged
head failed-jobs rerun recovers the full base with job120180715250 in attempt2.
That failure remains evidence; no base substitution, bound change or source repair
is claimed for the recovery.

`python3 build/plan-evidence/release-continuation/authenticate-artifacts.py RUN KIND`
exits0 for each role and invokes the existing quality_join with observed producer,
base,source, trusted BASE_POLICY and PERF_REQUIRED=ON/OFF. Direct testcard is
actually ARMED=true/PASS:3837262 Ir/frame at head, baseline and repeated base,
delta0, Ubuntu GCC15.2.0/Valgrind3.26.0. This remains one testcard fixture.
Unavailable coverage/open-crash/backlog/duration fields and advisory library/WASM/
property semantics are retained; no promotion or fabricated numeric value occurs.

`python3 build/plan-evidence/release-continuation/check-execution-artifacts.py RUN KIND`
exits0 for both roles: six authenticated direct hash archives agree, and downloaded
called native/PowerPC JUnit reports each execute610 normal plus6 release tests once.
The original full-equality producer is attempt1/job120172173115, including both
same-head identities and hash files; its attempt2 metadata copy confers no sample.

### Original quality artifact identities

#### Direct

| Lane | Artifact ID | Original job / attempt | SHA256 ZIP digest |
|---|---:|---|---|
| quality-asan | 12469762637 | 120172220164 / 1 | afaafa6eaefe466c7a2dc80112fcbdcc82fcc1a59778a111e8131c93d1f10aa2 |
| quality-hygiene | 12469926965 | 120172171556 / 1 | a57ebd4bf5227b60d9fc261161f2ce2c214f9cf10db0f1b03d46aaa04f0de7c0 |
| quality-linux-arm64 | 12469727971 | 120172220261 / 1 | 9a439251d99f69951d496f010a91c102c3cd297e83ef9768f0f9c6c0f99afe7b |
| quality-linux-x64 | 12469204087 | 120172220298 / 1 | f9caa59fe94252017ac71eaa6f0492cee943444565bcb3a2a424d7202372f4d3 |
| quality-macos-arm64 | 12470050899 | 120172220312 / 1 | 284557ea75d6f9b928efdb67ee41d530a8831ee05168262e3a1397e8a9f5ae0f |
| quality-macos-x64 | 12469877809 | 120172220346 / 1 | d53035e720c8ede097c75be2ca75ed31c06c45eadc3c75f5764e2f36540fc5c2 |
| quality-perf | 12469074513 | 120172220231 / 1 | 1f21ea39d2997747cb2028c39a76954223e940f219f1e890f1f83c3ad554f509 |
| quality-windows-arm64 | 12470610997 | 120172220362 / 1 | a48a14a93456afa4f3ca1e1025760453b6b2a40e679a30d7d3df1c023bdc153a |
| quality-windows-x64 | 12469968020 | 120172220332 / 1 | 821d32497a1c74e8feeee567a0412b810d4d27d72849740c026f2eee3eae5692 |

#### Called

| Lane | Artifact ID | Original job / attempt | SHA256 ZIP digest |
|---|---:|---|---|
| quality-asan | 12470260658 | 120172221128 / 1 | ed1363d1dd87dd86ea815d54a228a1dcca3eccc61a91be96f95116affc72b64c |
| quality-hygiene | 12468973998 | 120172173253 / 1 | d2be9fa00ce1877787524af31fe4630380baddd674a8db61431a8eefc06a8c7a |
| quality-linux-arm64 | 12470270529 | 120172221159 / 1 | 5815913047d90efa989f8cb4cde9e2f980ceb6b6df221326f8c2c4aea7b8d9c6 |
| quality-linux-x64 | 12469971272 | 120172221204 / 1 | 5e15ce1811f60563743b48126b888ae40a0110a6e8aa83f643283ada0e9681d9 |
| quality-macos-arm64 | 12469462698 | 120172221452 / 1 | 2085885895d69d2bdfb9a5511cad5d0934b37a2944951b91f59251d2c5605fed |
| quality-macos-x64 | 12470410347 | 120172221261 / 1 | 36a1a1b34bd73a3a7fc16c61d4abaf4d8c921ae0450ed43c9b4305c55299113e |
| quality-windows-arm64 | 12470680778 | 120172221220 / 1 | 2f624be7d24bda6c6a29efdcb2345a77b177321c03a4f8a11b486f34456b2d6c |
| quality-windows-x64 | 12470574438 | 120180715250 / 2 | e53abde670d6bff26cd6636b35ba2618751ff27ee259967e39cc3390e224c6ec |

### Genuine lifecycle and fresh absence

Rehearsal job120183602427 records **gh2.102.0 (2026-09-30)** from the verified pin,
prepares18 archives at raw098bb5e, creates retained ID**421353911**, uploads19 assets,
verifies correct and last-byte-tampered copies, then publishes the never-latest
prerelease `rehearsal-39879193669-2`. Independent API observation confirms target098bb5e,
draft=false, prerelease=true, immutable=true, published2026-10-23T01:15:31Z.
Actual downloaded ZIPs match SHA256SUMS and verify against the full repository,
signer workflow and refs/pull/20/merge. Archive commit and workflow-trigger commit
remain separate identities. The latest alias remains v0.4.0.

Install job120185505590 runs the actual README line successfully. Cleanup job
120185554406 confirms release absence before deleting/confirming its tag, exercising
the real empty204 response path. `python3 build/plan-evidence/release-continuation/confirm-absence.py`
exits0 at2026-10-23T01:22:42Z: one fresh authenticated complete release-list page,
no matching tag/ID, exact release-ID404 and exact tag404. Agent execution of
`GITHUB_REPOSITORY=szTheory/muhtari sh scripts/publish.sh rehearsal-delete rehearsal-39879193669-2 421353911`
also exits0 on the already-absent branch. No disposable resource remains.

Both job definitions' verified provisioning and checked main-publish state capture
are exercised by actual extracted workflow-block controls. This PR genuinely runs
the rehearsal path; it does not invent a main product release. Built/latest README
installs and read-only settings check also exit0; latest v0.4.0 targets40ce425099e16a286d5ac7f96ff697c45bc48138.

### Cost and bounded disposition

Pinned-CLI/sound-archive successful publication cohort is **n=1**, provisional.
Actual job execution intervals are rehearsal372s, install7s, cleanup11s:390s for
those three stages. Whole current Release uses4729 observed job-seconds across
both attempts, including the failed base job; execution envelope2531s, creation-to-
last-completion2533s. These are observed occupied intervals, not billed/CPU time or
separately measured queue latency. Full native/BE equality includes a slow compiler
fetch, so it is not pooled with a warm-network cohort.

The actual monthly reporter has a reproduced advisory cost bug, COST-01: it counts
copied retry intervals and can subtract negative skipped placeholders. Its raw
450-run preview is not accepted numeric cost evidence. This closeout uses
`python3 build/plan-evidence/release-continuation/rehearsal-cost.py` and
`hosted-summary.py`, exit0, excluding confirmed copies and nonexecuted skipped
placeholders while retaining real failures/cancellations. No reporter repair or
new requirement is silently added under AGENTS7/D-35. WR-01's delayed-release base
selection is the other deferred tooling seed. Both are explicit, not fixed claims.

The agent judges docs and evidence. No owner UAT, extra framework, promotion or
review/gap reset occurs. Historical14/08/06/11/12 and15's original halt remain
truthful. Accepted active slots now8/9;09 alone remains to record the shared final
outcome. Default-branch digest publication, merge/head CI and release-PR health
are ship checks under AGENTS6/9. Stop this execution step before ship/merge.
