# Phase 10: Homebrew and expansion memory - Discussion Log

> **Audit trail only.** Do not use as input to planning, research or execution.
> Decisions are captured in `10-CONTEXT.md`; this records alternatives considered.

**Date:** 2026-10-25
**Phase:** 10-homebrew-expansion-memory
**Areas discussed:** BankSet coverage, expansion selection, hardware combinations,
XM subset and support claims.

## Owner instruction and method

The initial selection offered all four areas above and recommended keeping four
historical optional todo groups deferred. The owner answered with a general
fan-out brief: discuss/consider all, investigate breadth and depth through relevant
stakeholder/specialist lenses, prefer reputable primary sources, challenge the
recommendations adversarially, synthesize a coherent set and automatically follow
it. Stop expanding at diminishing returns. Favor small flat dependency trees and
an understood permissive copy over a new package where appropriate.

The decisions are agent-selected under that delegation. No fabricated individual
user choices or routine confirmations are recorded. Three read-only specialist
reviews addressed BankSet/bus, XBoard/XM hardware, and hosts/API/evidence. A bounded
interaction pass resolved conflicting assumptions and then stopped. One BankSet
response failed on model capacity; one bounded continuation recovered its retained
findings without changing the research scope or model.

## BankSet coverage

| Decision | Alternatives considered | Adopted recommendation |
|---|---|---|
| Geometry | Keep refusal; wiki-only sizes through 128 KiB per half; documented producer/emulator extensions | 32/48/52/128/256/512 KiB per half, checked equal halves and per-half banks, with explicit extension confidence |
| RAM | Infer RAM from BankSet bit; collapse normal and HALT RAM; preserve declarations | No declaration means none; normal means shared 16 KiB; HALT means split 32 KiB; refuse RAM for 48/52 KiB sets and unsupported option combinations |
| Memory ownership | Caller identity alone; existing HALT stage alone; actual timed electrical selection | SALLY until second falling phi2 after HALT assertion, then MARIA until release; distinguish no-HALT MARIA helper reads |
| Causality | Add only a timestamp; write-history overlay; narrow causal burst handoff; general chronological fetch scheduler | Research the narrow handoff correction first; chronological scheduling only if a required interleaving falsifies batching |
| Overlap | Unconditional POKEY/HSC priority; silent omission; prevalidated specific policy | Readable BankSet ROM/RAM beside write-only $4000 POKEY; refuse 52 KiB plus provisioned HSC, with explicit HSC Off resolving that conflict |

**Owner's choice:** All areas researched; adoption delegated.

**Notes:** The initial "32 KiB RAM must be HALT style" wording was narrowed:
HALT RAM is the wiki's physical hardware, while distinct shared-RAM declarations
are a documented emulator extension. No RAM is inferred from image size alone.
The source-backed BankSet specification and producer guide take priority over
generic "all MARIA reads use its half" shorthand.

The adversarial bus sequence is slow c0 RIOT access then fast c1 cartridge write,
not a slow cartridge store under MEN=1. Existing retroactive first-fetch timing
can precede a write already committed by the implementation. This was identified
from code and a synthetic bus sequence; no actual instruction stream, new runtime
test or hardware trace is claimed. Merely passing a timestamp cannot reverse a
committed memory mutation. Planning research must settle the narrow causal fix.

## Expansion selection

| Decision | Alternatives considered | Adopted recommendation |
|---|---|---|
| Attachment | Explicit-only; metadata-only; metadata Auto plus explicit override | Append load-time Auto/Off/XBoard/XM with zero/omitted Auto, explicit first then current resolved XM metadata |
| XM byte interpretation | Reinterpret byte 63 bit 0; preserve existing any-nonzero hint | Preserve nonzero request plus existing unusual-value warning |
| Reporting | Overwrite probe metadata with overrides; separate request and effective attachment | Preserve raw identity and expose actual selected profile through existing size-tagged info/host surfaces |
| Unsupported sound | Accept silently without sound; refuse at descriptive probe; refuse executable load | Preserve descriptive probe, refuse unsupported executable configuration with existing statuses/runner 77/frontend reason |
| Old callers | New struct required; preserve silent legacy execution; additive defaults and documented correction | Old complete/partial/omitted tails retain defaults, ABI 1 stays, API/behavior follow actual declaration and semantic changes |
| Host controls | Runtime attachment; unrelated host-specific selectors; one load-selected concept | Matching runner flag and reload-only libretro option; transactionally resolve before storage and allocation |

**Owner's choice:** All areas researched; adoption delegated.

**Notes:** An Off override suppresses attachment, not raw requested metadata or an
independent YM2151 requirement. XBoard is explicit because no distinct header
encoding was established. Module/firmware hash-list entries remain invalid game
images. All affected HSC discovery and late-storage consumers must agree with core
selection; a core-only implementation would miss optional firmware and report the
wrong reload requirement.

## Hardware combinations and persistence

| Decision | Alternatives considered | Adopted recommendation |
|---|---|---|
| HSC storage | Runtime allocation/resizing; always-visible SRAM workaround; fixed provision plus decode gate | Provision on successful XM load subject to HSC override; gate both ROM/SRAM without changing layout 1 or pointers |
| HSC reset | Clear saves with expansion controls; retain committed storage separately | Power cycle clears controls and retains bytes/generation; Reset switch remains software input |
| POKEY enable | Instantiate/reset/mute on control writes; gate register decode only | Installed chip continues phase, timers, output, normalization and IRQ wiring while decode is disabled |
| Sound topology | Duplicate same-address chip; silently prioritize/drop chips; general sound-owner registry; bounded explicit support | Expansion $0450 is one owner, optional cartridge $0440 is second; refuse ambiguous/unsupported extra-owner combinations |
| Memory views | Publish initial bank as fixed pointer; mix expansion RAM into saves/cart metadata; physical spans and honest map holes | Separate volatile expansion storage, physical cart span including BankSet halves, omit banked/gated CPU windows from static descriptors |

**Owner's choice:** All areas researched; adoption delegated.

**Notes:** The source explicitly calls always-on XM SRAM a persistence workaround,
which must not become emulated hardware. Gating both HSC windows is qualified as
an inference. POKEY decode gating without mute follows chip-select behavior and the
existing continuous-clock model; an independent physical observation can overturn
it. Refused chip combinations are a supported-profile boundary, not a statement
that the hardware cannot combine devices. No third chip is introduced.

## XM subset and support claims

| Decision | Alternatives considered | Adopted recommendation |
|---|---|---|
| Register model | Reuse XBoard map for XM; distinct maps; speculate full XM | XBoard eight 16 KiB banks and aliases; XM separate CPU/MARIA 8 KiB page selectors and exact register addresses |
| Service controls | Copy comments as semantics; implement inferred sticky locks; explicit inert unsupported subset | No invented ROF/BIOS/flash/PIA/flat48/protection/locks/PAL-firmware behavior; preserve cartridge pass-through and absent-device/open-bus behavior |
| Capability claim | Advertise XM generally; accept YM games soundless; name scope and refuse unsupported declarations | XM RAM/POKEY/HSC subset, confidence-qualified model, no firmware/service or whole-game playability claim |
| Primary specification | Treat unreachable developer page as verified; use qualified permissive implementation evidence | Record inaccessible source and medium physical confidence; source comments alone establish no hardware contract |

**Owner's choice:** All areas researched; adoption delegated.

**Notes:** Preparation audio §6 describes an older XBoard-style XM model that
conflicts with the separate XM implementation. Context resolves the future profile;
implementation must update graduated hardware/user docs and the stale preparation
pointer together. No GPL/LGPL/unlicensed emulator source or firmware/ROM bytes
were opened or copied. A7800DS was consulted only as documentation; the individually
audited BankSet/XM/XBoard source files carry BSD-3 licenses.

## Evidence, dependencies and stopping

| Decision | Alternatives considered | Adopted recommendation |
|---|---|---|
| Acceptance | Host hash equality alone; private-ROM/owner gate; independent numeric diagnostics then equality | Owned mailbox/CRC and distinct sentinels through actual CPU/DMA plus library/runner/module, followed by native/PowerPC comparison |
| Controls | New generic validation framework; tests mirroring implementation; existing relevant consumers | Extend current registrations and meaningful pass/fault cases for geometry, lifecycle, map holes, sound and evidence identity |
| Hosted UI | Another expansion-specific RetroArch harness; existing two cases plus in-process/module semantics | Retain the two existing hosted cases and test hardware semantics through current diagnostic drivers |
| Resources | Historical archive/LOC cutoff; replacement quota; physical capacities and existing objective-bound guards | 128 KiB expansion storage, existing chip/ROM bounds, bounded work/noalloc, existing performance guard and authenticated package reporting |
| Dependencies | New mapper framework/package; small current owners and narrow helpers | Reuse cartridge/bus/chip/storage owners; no dependency selected; copied expression still needs provenance and maintenance |
| Further review | Recursive role expansion; one relevant adversarial pass then evidence-triggered reopening | Stop after synthesis; reopen only a choice falsified by primary specs, observations or a required consumer |

**Owner's choice:** Broad specialist research and automatic coherent adoption,
adapted to this emulator and bounded at diminishing returns.

## Agent's Discretion

Private names, allocation layout, fixture organization, narrow helper/module seams
and precise independent checks remain builder decisions within `10-CONTEXT.md`.
Planning research verifies assumptions and the minimal causal timing correction.
No routine owner verification, hardware test, new dependency or additional GSD
step is authorized by this discussion's completion.

## Deferred Ideas

All four historical todo groups remain separate absent a reproduced violation of
an existing required contract: nightly/table seeds, CPU/HALT test seeds,
Dependabot/publication seeds, and WASM promotion. A file-touch match alone folds
nothing. New required HALT evidence does not blanket-close old optional seeds.

YM2151, Souper/BupChip, Covox/ADPCM, full XM service hardware, additional independent
sound owners, full OpenHSC, measured hardware calibration and whole-library gameplay
accounting retain their existing later scope.

Read-only release checks found PR 35 merged with required CI green, but v0.8.0
publication run 40249027723 failed and the release remains a draft. Preserve the
failure as separate release follow-up; do not claim publication or perform a retry/
repair inside this discussion. Context records the command/exit/run scope.
