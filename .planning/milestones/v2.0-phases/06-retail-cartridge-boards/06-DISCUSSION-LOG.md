# Phase 6: Retail cartridge boards - Discussion Log

> Audit trail only. Downstream research, planning and execution consume `06-CONTEXT.md`.

**Date:** 2026-10-23
**Phase:** 06-retail-cartridge-boards
**Areas discussed:** Hardware maps; layout identity and hosts; adversarial verification; proportional delivery.

## Delegation

After invoking discussion, the owner requested all relevant decision areas, parallel breadth/depth research,
primary sources, tradeoffs, adversarial review and automatic adoption of coherent recommendations. This replaced
the routine interview. No multiple-choice answers are attributed to the owner. Three read-only advisor researchers
covered hardware, identity/hosts and verification/proportionality; the parent reconciled their findings.

Settled licensing, architecture, automation-first verification, dependency preference and one-step boundaries
were carried forward. No source or behavior changed. No new hardware run, owner UAT or external message was requested.

## Hardware maps

| Option | Tradeoff | Selected |
|---|---|---|
| Activision $FF80–$FF8F, A2–A0 | Matches preparation's compatibility policy; extended decode remains unmeasured | Yes |
| Activision all $E000–$FFFF writes | One permissive emulator's wider decoder; weak support for extra side effects | No |
| Absolute $8000, D0 before D1, neither retains | Conservative address policy; unusual values remain qualified | Yes |
| Absolute any A15 write, bank = D1 | Simpler but different side effects and values 0/3 | No |
| Fractalus physical 2 KiB, full-window aliases | Preserves adjacent-page aliases and bounded storage | Yes |
| Ordinary size mask or physical 8 KiB | Fails the documented disconnected A8 mapping or invents physical capacity | No |
| Restrict Fractalus decode to $4000–$4FFF | Matches guide's explicit range but omits preparation's wider alias policy | No |

**Agent selection under delegation:** OM's switched 8 KiB halves must reverse relative to AM; map every window
explicitly. Keep bank-zero/zero-RAM deterministic startup without claiming physical startup is measured.
Primary guide uncertainty remains visible; emulator agreement is implementation evidence, not hardware proof.

## Layout identity and host maps

| Option | Tradeoff | Selected |
|---|---|---|
| OM canonical, identified-only conversion | Aligns with bundled hashes; one installed map | Yes |
| AM canonical | Aligns with historical presentation but converts existing direct matches | No |
| Unknown declared Activision assumes OM with warning | Supports diagnostics/homebrew with a clear convention | Yes |
| Refuse every unknown layout | Avoids assumptions but needlessly rejects explicit mapper homebrew | No |
| Guess by filename/vector/execution | Can silently choose the wrong map | No |
| New override/API/database | Adds surface without a demonstrated consumer | Deferred |
| Truthful fixed ROM and bounded partial Fractalus aliases | Useful host visibility if pinned resolver capacity is demonstrated | Yes |
| Omit Fractalus RAM descriptors | Truthful capacity fallback; physical RAM span still available | Conditional fallback |
| Contiguous Fractalus descriptor | Reports wrong bytes even when the emulator runs correctly | No |

**Agent selection under delegation:** Preserve original hashes and caller bytes. Retain half-swap provenance
through existing flags, expose assumed order through a warning, and translate raw-content host offsets correctly.
Prefer sixteen 256-byte aliases at $4000–$4FFF, with higher mirrors explicitly unmapped by descriptors. Full
bus alias behavior is independent of descriptor visibility. Resolver capacity is a planned measurement.

## Adversarial evidence

| Option | Tradeoff | Selected |
|---|---|---|
| Independent semantic vectors plus executable board diagnostics | Covers precise rules and actual CPU/host integration | Yes |
| Frame hashes or non-crash alone | Can pin incorrect semantics or an unexecuted mapper | No |
| Existing corpus/property/fuzz and allocation checks | Small additional surface; must actually select each new board | Yes |
| New framework, lane, retail ROMs or hosted RetroArch case | Adds cost/licensing surface without necessary coverage | No |
| Deep refusal/lifecycle comparison | Detects mutation through unchanged pointers | Yes |
| Instance-structure memcmp alone | Misses changes to owned ROM/RAM | No |

**Agent selection under delegation:** One owned executable diagnostic per board across library, runner and
in-process libretro. Pair AM/OM equality with distinct raw identity. Check all Fractalus aliases with independent
patterns. Adapt the existing loader-fuzz raw-copy invariant and linear-only run generator. Preserve old predicates,
bounded workloads and honest retail support claims. No owner verification is required.

## Tooling and package bounds

| Option | Tradeoff | Selected |
|---|---|---|
| Fixed 192-line scoped addition, including controls/reserve | Funds counted integration before implementation; fit remains to be shown | Yes |
| Assume tests/registration glue does not count | Hides maintenance growth under current whole-file inventory | No |
| Candidate self-rebaseline or recurring temporary waivers | Defeats the trusted-base ratchet for permanent growth | No |
| Independent package-size measurement before any byte allocation | Avoids inventing a package allowance from a line allowance | Yes |
| Reopen Phase 5 or fold unrelated tooling seeds | Expands this board phase without new scope evidence | No |

**Agent selection under delegation:** B=15,287 at `617dfa762ddb649839b925be5aa582180066fa78`, S=0,
N=192, maximum C=15,479. Allocation: registrations 48; host contracts 40; manifest/inventory/claim glue 32;
narrow admission and controls 40; reserve 32. These are estimates, not measured fit. Current quality admission
does not demonstrate a permanent existing-ceiling increase; planning must resolve that within the fixed cap.
No package byte allowance is adopted. The independent archive ceiling and required evidence remain in force.

One-off baseline command:
`cmake -DROOT=. -DINVENTORY=tests/policy/gate-lines.txt -DOUT=/tmp/muhtari-phase06-gate-count.json -P tests/cmake/gate_count.cmake`
— exit 0, 15,287 physical lines. Output stays outside the repository. No hosted run was created by this local
discussion; implementation and ship evidence must supply their own run URLs.

The maintenance digest was read with `gh issue view 22 --repo szTheory/muhtari --json body,url` — exit 0;
source: https://github.com/szTheory/muhtari/issues/22. Its image notices do not add a cartridge requirement;
unavailable cost evidence is not treated as zero.

## Agent's Discretion

Names, local helper/file boundaries, diagnostic patterns and plan order remain delegated. Further fan-out stopped
after the three lenses converged without a new material decision. v4 A8 combinations, resolver capacity, permanent
allocation admission and package-size acceptance are explicit research/planning prerequisites rather than claims
of completed experiments. The next GSD step is planning, in another session.

## Deferred Ideas

Hardware measurement/oracle work, layout overrides without a consumer, later roadmap features and all four reviewed
pending todos remain deferred as listed in `06-CONTEXT.md`. No todo was promoted merely because keyword matching
assigned it a score. No source, quality policy, fixture, workflow or settled preparation decision changed here.
