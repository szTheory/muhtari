# Phase 13: Portable save/load across three interfaces - Discussion Log

> **Audit trail only.** Downstream agents use `13-CONTEXT.md`.
> This records alternatives and agent judgment, not implementation evidence.

**Date:** 2026-11-01
**Phase:** 13-portable-save-load-across-three-interfaces
**Areas considered:** Complete machine point; compatibility/refusal/resource
safety; battery/durability; library/runner/libretro behavior; independent evidence.

## Selection and authority

The owner invoked `$gsd-discuss-phase 13`, then explicitly requested consideration
of all relevant decisions using subagents, appropriate role lenses, primary-source
research, alternatives, footguns and one coherent set of recommendations to follow.
The existing `.planning/research/fan-out/prompt.txt` and `.planning/INTENT.md`
already preserve that posture. No new workflow preference needed an AGENTS edit.

Three read-only `gsd-advisor-researcher` reviews used GPT-6 Luna with xhigh effort:
state format/completeness/compatibility/security; public/host/progress semantics;
and independent evidence/portability/resource costs. The parent owns the synthesis
and all shared planning writes. There were no fabricated interactive answers and
no unanswered owner-only scope or risk decision. Technical details within scope
remain agent-owned; this is not an approval for a later major GSD step.

## Complete machine point

| Alternative | Benefit | Cost or failure boundary | Selected |
|---|---|---|---|
| Keep serialization unavailable | No new parser or public contract | Fails every accepted state requirement | No |
| Fixed canonical LE visitor and stable per-load offsets | Reuses DEC.26/35, complete same-contract continuation, simple host delegation | Requires a real field/invariant inventory and strict refusal | Yes |
| Self-describing sections, per-section versions and migrations | More extension and inspection flexibility | Adds length handling and long-lived compatibility policy with no present consumer | Deferred |

**Resolution:** Capture pending/applied input and once-only motion held in the
core, device work and audio carry along with ordinary registers/RAM. Exclude the
host input source, callbacks, output destinations, immutable media and lifetime
storage. Old preparation/internal prose excluding pending input is not acceptance
authority. Exact layout, field map and bounds belong to planning.

## Compatibility and hostile-state safety

| Alternative | Benefit | Cost or failure boundary | Selected |
|---|---|---|---|
| Old illustrative 64-byte header and ROM CRC only | Small, historically documented sketch | Does not bind firmware/effective machine identity; CRC-valid hostile fields remain dangerous | No |
| Strong effective media/firmware/configuration identity, format/behavior versions, full validation then infallible apply | Same machine contract across native widths/endian and host interfaces | SHA-256 helper needs a private shared core owner; cross-field validation is necessary | Yes |
| Bind exact package/build/OS or create a host manifest and compatibility registry | Can enforce a host's stricter sharing policy | Needlessly rejects portable equivalent cores and expands host policy | Deferred until a consumer requires it |

**Resolution:** Reuse the owned SHA-256; checksum is corruption detection, not
authentication. Refuse unsupported compatibility before mutation. No automatic
migration. Failed restore preserves execution and metadata; output buffer and
input/live-span overlap behavior must be explicit. Producer invariants and complete
work measurements determine clock/scratch bounds, rather than an invented quota.

## Emulated battery and durable progress

| Alternative | Benefit | Cost or failure boundary | Selected |
|---|---|---|---|
| Omit battery data from states | Avoids intentional rollback of current SRAM | Incomplete machine state and pending-write continuation | No |
| Include bytes and pending programming; retain live dirty token | Complete replay and host notification through existing raw span | A later committed flush can replace newer progress after an intentional old-state load | Yes |
| Core-managed immutable save history and host transaction policy | Could recover newer progress | Violates core file ownership; belongs in a frontend with a recovery promise | Deferred |

**Resolution:** Changed restored bytes advance the live token once; equal/refused
restores preserve it. Pending programming alone is not a committed byte change.
No state call performs durable I/O. Do not confuse speculative emulated mutation
with a disk commit, or claim arbitrary frontend safety without actual host proof.
Document the rollback consequence; Playstead history stays outside this phase.

## Library, runner and libretro

| Alternative | Benefit | Cost or failure boundary | Selected |
|---|---|---|---|
| One library format, thin host consumers, checked runner files | Meets all three deliverables and fresh-process workflow with existing mechanisms | Must specify loaded lifecycle, buffer sizes and frame/input sequencing | Yes |
| Keep runner state optional | Less CLI work | Contradicts accepted STATE-06 despite older research wording | No |
| Add host envelopes, recording protocol or a second codec | Could serve later interchange/netplay requirements | Duplicates ownership and adds unsupported claims | Deferred |

**Resolution:** Saves accept sufficient output capacity and preserve its unused
tail; restores consume an exact state. Official libretro guidance explicitly
allows oversized serialize buffers but gives no matching unserialize guarantee.
Unloaded callbacks return zero/false; failed-load behavior must not expose stale
content. Keep all validation in normal/run-ahead contexts. The pinned MIT header
requires BE encoding for explicit rollback-netplay context 3; refuse that context
rather than silently using LE or adding another encoding.

Runner save/hash points mean completed frame calls in this invocation. The host
advisor recommended positive 1-based points; synthesis also permits zero completed
calls, giving an explicit before-first-frame point without consuming input. A
restored state's internal frame counter is not the CLI point. Replay continuation
must provide the correct remaining input sequence; no automatic movie cursor is
serialized. State hash uses SHA-256 of canonical bytes. Reuse bounded exact reads
and checked temporary replacement; never truncate an existing target before
validation/success. Diagnostics distinguish state refusal, usage and file errors.

**Adversarial correction:** One advisor proposed a dedicated state-refusal exit 5.
The current `runner/exit_status.h` defines 0, 1, 2, 4 and 77, with no 5. The
synthesis retains failure 1 and usage 2 plus specific diagnostics by default;
unsupported-game 77 must not be repurposed. A new numeric state category needs a
concrete consumer justification. No invented exit is recorded as an existing fact.

## Evidence and recurring cost

| Alternative | Benefit | Cost or failure boundary | Selected |
|---|---|---|---|
| Split-run hash equality only | Cheap regression smoke | Both branches may share an omission or wrong input | Insufficient alone |
| Independent owned semantic expectations plus equality, hostile/refusal/allocator/file/module controls | Addresses shared omissions and interface failures using current consumers | Requires targeted topology/pending-boundary coverage and measured runtime | Yes |
| External emulator/hardware state oracle and exhaustive per-lane cross-product | Could expose some model errors | No portable shared state format, greater provenance/cost, unnecessary phase scope | Deferred unless a reproduced gap requires it |

**Lane correction:** PR `CI required` consumes compile-only `be32`; its required
`hash-equality` compares six native producers. Native+PowerPC execution lives in
`.github/workflows/be-equality.yml`, called by nightly and release workflows.
Nightly is advisory; release equality gates later publication. Phase 13 acceptance
still needs an actual exact-source BE byte/continuation receipt. The planner must
identify the existing executing consumer; no PR-required BE execution or future
release success is claimed here. No lane promotion or matrix redesign is selected.

**Acceptance posture:** Every behavior check joins existing CMake/inventory owners.
Use owned/permissive inputs and ephemeral state buffers/files. Preserve required
contracts, advisory labels and bounded review/gap rounds. No actual host case,
generic audit framework, new dependency, timeout increase or unrelated tune-up.

## Confidence, sources and revisit triggers

Local contracts and inspected source have high confidence about current ownership
and missing serialization. Recommendations are inference from requirements; no
build, benchmark, hardware result, CI receipt or integration pass was produced.
External primary guidance: [Libretro serialization](https://docs.libretro.com/development/cores/developing-cores/),
[canonical MIT header](https://github.com/libretro/libretro-common/blob/master/include/libretro.h),
and [NIST CRC glossary](https://csrc.nist.gov/glossary/term/cyclic_redundancy_check).
The local pinned header remains the binding API source; moving upstream examples
do not establish pinned-host behavior. Exact local references are in CONTEXT.md.

Revisit only for a real cross-version/interchange consumer, changed
machine-affecting config, measured state cost or a reproduced required-contract
conflict. No further fan-out changes the current choices; no lesson is marked
reproduced from research alone. No export was requested.

## Deferred items and handoff

Phase 14 owns actual RetroArch save/load, rewind and run-ahead proof. Native
Playstead history/Continue, movies/TAS/AI, netplay and browser work retain existing
triggers. The five todo matcher hits are generic keyword overlap, retain their
prior dispositions and add no requirement (listed in CONTEXT.md).

Stop after this discussion. Next separate command: `$gsd-plan-phase 13`.
Phase 12 acceptance, safety stop and every spent allowance remain unchanged.

### Git and verification bookkeeping

The primary workspace contains pre-existing committed and uncommitted Phase 12
work. Commit only these two new discussion documents on
`phase/13-portable-save-load-across-three-interfaces`, created directly from
`origin/main` in `.claude/worktrees/phase-13`. Keep the primary branch/index and
all pre-existing bytes except the intentional STATE session update unchanged.
Do not stage the shared STATE file: its older uncommitted changes belong to the
halted Phase 12 scope. Keep matching discussion copies in the primary workspace
so the next invocation can discover them. The next agent owns bringing the needed
current planning references into the isolated worktree before planning there;
never copy Phase 12 runtime/test/workflow changes into the independent baseline.

Local checks: `sh scripts/hygiene.sh --tree` exited 0;
`git diff --check` exited 0. A one-time Python inspection exited 0 for context
sections, local-reference existence, seven explicit requirement references,
whitespace, checkpoint removal and handoff, plus byte equality of all pre-existing
files except STATE and equality of the primary index. Run URL: N/A-local.
The commit hook checks the final staged discussion bytes. No behavior changed,
so no emulator test run, CI dispatch, hosted contact or allowance was needed.
