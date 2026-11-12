# Current continuation — v6.0 selected, autonomous local work active

Owner selected YM2151 homebrew sound and authorized gsd-autonomous style on
2026-11-11. Current [STATE](STATE.md), [requirements](REQUIREMENTS.md) and
[roadmap](ROADMAP.md) supersede pending-selection text below. Reconcile actual
plans and verification, then continue `$gsd-autonomous` without repeating routine
scope acceptance. No owner directory switch is needed. The agent owns service,
source and receipt checks. Existing Phase12/14 denied/spent scopes remain closed;
publication/merge and materially new remote/host/compatibility decisions remain
separate. The rest of this packet is historical provenance.

> Updated by later owner-requested fan-out: the active successor recommendation,
> source-confidence register and next action are now in
> [the research synthesis](research/next-milestone/SUMMARY.md) and
> [YM2151 proposal](research/next-milestone/PROPOSAL.md). The packet below preserves
> the earlier cross-project feedback context; it is not the current scope choice.

# Muhtari successor milestone — cross-project feedback handoff

Prepared 2026-11-11. Continue the existing `$gsd-new-milestone` discussion;
**hold successor selection until cross-project feedback returns**. This packet
uses read-only source, commit, audit and GitHub receipt inspection. No tests,
CI, apps or sibling changes were launched for this assessment.

## 1. Open decision and recommendation

The open choice is **which bounded outcome should become Muhtari's next
milestone**, rather than another project's work. No successor name,
requirements or phases are approved.

**Owner decisions:** `twentysixhundo` owns Atari 2600 emulation; Muhtari's
2600-mode seed is retired. Shared tooling and reproduced lessons remain useful.
Muhtari retains its library, headless runner and libretro deliverables; hosts
own presentation and devices. Cross-project feedback precedes scope selection.

**Updated proposal:** prioritize identifying one discriminating 7800 correctness
gap, then scope a measured accuracy correction or identified homebrew-hardware
slice with an independent oracle. No current title-blocking defect has been
established, so this is a conditional recommendation, not an implementation
commitment. Begin any selected milestone with the proportional recurring
tune-up; avoid another infrastructure-only successor without demonstrated need.

**Strongest alternative:** advance LibFuturo's existing browser PAL/profile
qualification toward useful player access. That belongs there; Muhtari work
would follow only from a demonstrated upstream limitation. Starting another
browser host here would duplicate implemented work.

## 2. Published baseline and shipping closure

[Muhtari v0.13.1](https://github.com/szTheory/muhtari/releases/tag/v0.13.1)
is published: nineteen ZIPs plus SHA256SUMS, including
`muhtari-libretro-emscripten.zip`. The immutable release source's
[Release run](https://github.com/szTheory/muhtari/actions/runs/43105582439)
succeeded, including public-download checksum/attestation verification,
tampered-copy rejection and latest installation. Release PR #52's checks passed.
Non-release fixture-margin PR #53 merged; [current-main CI](https://github.com/szTheory/muhtari/actions/runs/43119220184)
succeeded at the unchanged public main head, including the advisory SDK lane.
**Both public-byte verification and current-main closure are finished.**

The public tuple is API 8, ABI 1, behavior revision 6, state format 1
(`include/muhtari.h`, `src/instance.c`, `src/state.c`). This does not establish
arbitrary cross-build snapshot interchange or qualify LibFuturo's older pins
against this release. Preserve failed/unpublished v0.13.0 separately.

The [final ship receipt](milestones/v5.0-SHIP.md#final-publication-and-current-main-closure--reconciled-2026-11-11)
records commands, exit statuses and source identities. Existing receipts were
rechecked successfully; no publication rerun was needed. Pending-shipping prose
in STATE, milestone and roadmap records has been reconciled locally. Windows
ARM64 remains the observed critical path; no safe cache/topology change follows
from these runs. Documentation changes remain uncommitted; no main commit occurred.

## 3. LibFuturo baseline and remaining browser value

The primary LibFuturo checkout was inspected at `13b2074`; its old-looking
branch name is not evidence of stale code. Shared PROJECT/PRODUCT-ROADMAP,
all three requested workstreams, source and recent commits were read. Review
worktrees were inventoried but not imported. Browser milestone planning is dirty.

Its [browser guide](../../libfuturo/docs/browser-host.md),
[v1.9 audit](../../libfuturo/.planning/workstreams/browser-host/milestones/v1.9-MILESTONE-AUDIT.md)
and [v2.0 audit](../../libfuturo/.planning/workstreams/browser-host/milestones/v2.0-MILESTONE-AUDIT.md)
support the reported progression: local content, keyboard input, paced execution,
Canvas, activated Web Audio and lifecycle handling; then identity-bound
IndexedDB battery save/restore before execution, transaction/refusal/stale-writer
controls and scoped deletion. These are retained local qualification records,
not newly reproduced results.

The boundary is Muhtari v0.11.1/API 8, `muhtari-ntsc-v2`, generated/owned
diagnostic fixtures, pinned Chrome/Linux amd64 in Docker on local macOS arm64.
They establish neither a released player nor arbitrary-game, Safari/mobile,
gamepad, physical audibility, power-loss or Playstead-embedding support.
Current-source v2.0 hosted evidence and branch protection remain unverified.

LibFuturo links `muhtari::muhtari` with its adapter and presents `lf_*`;
Muhtari's downloadable Wasm bundle presents `retro_*`. The former builds pinned
core source and does not consume the latter ZIP. Emscripten appears in both
routes without making them interchangeable packages.

API 8 Linux snapshot and PAL work pin Muhtari v0.12.0. PAL has local source/
relocated-consumer evidence; [Phase 2 verification](../../libfuturo/.planning/workstreams/core-profiles/phases/02-canonical-native-qualification/02-VERIFICATION.md)
still reports native Linux/hosted gaps. Recent commits independently hash actual
PAL input, bind each route's records, anchor log markers and remove assumed
PAL/NTSC size equality. These fix LibFuturo evidence, not demonstrated core bugs.

**Stale/conflicting text:** shared PROJECT still describes browser v2.0 as
underway and an older Muhtari profile as lacking snapshots. Browser STATE now
selects v2.1 PAL planning, while its body retains older next-work text. Prefer
dated workstream evidence; PAL browser completion and current-source hosted
acceptance remain unsupported. The interface is implemented and experimental;
unfinished qualification/adoption is different from an exploratory-only idea.

## 4. Bounded candidates

| Outcome / owner | User value and prerequisites | Smallest meaningful proof / risk |
|---|---|---|
| One 7800 correctness or homebrew-hardware gap / Muhtari | Correct a named scene, sound or unsupported title feature; first obtain reproducible input and lawful independent expectations. | One discriminating fixture/scene and regression through affected deliverables. Risk: fitted/reference behavior mistaken for hardware truth. |
| PAL browser parity / LibFuturo | Extend the existing consumer's useful regional coverage; depends on profile-aware packaging, geometry, cadence, audio and save-capability handling. | Actual pinned-browser PAL output/pacing plus retained NTSC battery flow and PAL unsupported-save refusal. Risk: diagnostic success inflated into gameplay/browser generality. |
| One player session in Playstead / Playstead plus qualified host | Turn existing integration into accessible play; needs a selected host, actual consumer boundary and permitted content. | Load, discriminating input, pause/resume, progress restart and actionable refusal through installed artifacts. Risk: adoption, storage and permission scope exceed a core milestone. |

No demonstrated core/API/package change is currently required by LibFuturo's
PAL/browser plan. Pin upgrades, browser behavior and hosted source/ref binding
are downstream integration/qualification work unless contrary evidence appears.

## 5. Transferable lessons and feedback questions

Recent [Muhtari lessons](LESSONS.md#shipping-consumer-coverage) favor independent
non-silent audio expectations, checking actual downloaded layouts, hashing written
bytes, clean extraction and immutable source-specific receipts. Complete measured
Windows cohorts justify bounded deadlines; censored timeouts do not diagnose
provider load. Incoming sibling lessons remain hypotheses until reproduced here.

The returning session should answer:

1. Which specific game scene/input sequence best separates a guest-machine fault
   from adapter mapping, palette/scaling, pacing or audio presentation?
2. What independent hardware/document/diagnostic oracle distinguishes expected
   behavior from agreement between implementations? Respect permissive-source rules.
3. Does LibFuturo encounter a concrete Muhtari API/package limitation, or only
   missing profile exposure, host UX or qualification? Supply a minimal reproducer.
4. Which battery bytes, machine snapshot state and host revision/acknowledgment
   must survive each restart/restore? Which unsupported combinations must refuse?
5. Which installed-artifact identity/refusal/recovery checks transfer usefully
   across siblings, and which depend on system-specific fixtures or semantics?

## 6. Stops, allowed work and next decision

Phase 12's exact diagnostic-app request was denied before preflight/content;
no retry is authorized. Its host/remote allowances remain spent. Phase 14's
RetroArch save/load/rewind/run-ahead acceptance remains unproved and its host
allowance spent. LibFuturo evidence clears none of these requirements.

Preserve archived attempt-specific no-retry conditions and limits: Phase 12's
applicable 45-second helper, five-second acknowledgment and 50-second wrapper;
[Phase 14's frozen plan](milestones/v3.0-phases/14-proven-frontend-states-and-responsive-play/14-25-PLAN.md)
retains five-minute job, 45+5-second child, 120 presented frames, 64 forward
actions and its unrenewed 296-runner-minute ceiling. Expired dispatch windows,
terminal one-shot diagnostics and expressly prohibited retries stay closed.
Standing local authorization does not override specific no-retry stops.

Independent read-only research, local receipt/docs reconciliation and already
authorized shipping bookkeeping may continue. No new tests, host/CI runs,
sibling edits, package migration or successor execution follows this packet.
The owner returns feedback, then selects the bounded outcome and owning project.
Safe to clear context: local edits are preserved but uncommitted. Resume with
`$gsd-new-milestone` plus this handoff and feedback; no Phase 19 command is valid yet.
