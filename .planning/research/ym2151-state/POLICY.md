---
title: "YM state compatibility recommendation"
summary: "Current contracts and a minimally disruptive additive policy to qualify during Phase 20 discussion; source inspection, not implementation proof."
updated: 2026-11-11
---

# State compatibility evidence and recommendation

The owner selected the feature and autonomous local follow-through. Routine
compatible additions can proceed; breaking existing non-YM snapshots, public
layout or unrelated output remains a materially different consequence. This
research records a recommendation for phase discussion, not implemented behavior.

## Current facts

At the milestone kickoff source, the library reports API 8, ABI 1, behavior 6 and
state format 1 (`src/instance.c`, `include/muhtari.h`). The public docs define
format as the portable state byte format version and behavior as changing when
the same inputs give different frame, audio or state hashes (`docs/embedding.md`,
Versions). The size-tagged versions prefix and original function signatures are
frozen; YM declaration and XM selector already exist in the public header.

`src/state.c` explicitly encodes little-endian scalars; it does not copy C struct
padding. One visitor owns sizing, validation, encoding and application. Its
layout already varies with installed topology (e.g. expansion RAM). Format and
behavior are currently exact-match constants. Full envelope, identity and
semantic validation precede application. Expected length is checked before
revision, so an older loader can report invalid length for a newer state rather
than unsupported revision; the material contract is refusal without mutation.

The immutable identity includes installed media, region, board, POKEY addresses,
IRQ mask, expansion, input, battery, boot and model profile. It currently has no
YM identity term because every YM load refuses. The audio visitor serializes ten
source levels; blindly widening that loop for every machine would change
existing non-YM byte extents and snapshots. Changes to C struct size alone do
not require serialized-format changes if old non-YM field order/values remain.

These are direct source findings, not fresh load/restore execution evidence.
LibFuturo exact-build envelopes are a separate consumer contract; this milestone
promises no migration for them and makes no sibling edits.

## Recommended ordinary additive policy

Keep API 8 and ABI 1 unless phase research establishes an actual public contract
addition. Existing declaration fields are enough to request the selected profile.
Introduce behavior revision 7 for the newly executable capability. Preserve all
old non-YM PCM/transition results and native snapshot representation, including
format 1 / behavior 6 envelopes and identities, for unchanged effective machines.
Keep decoding and emitting that historical envelope for those machines.

Use a distinct format 2 / behavior 7 envelope only for YM machines, with explicit
YM profile identity and complete engine/timer/bus/audio state. Report format 2
as the latest format supported by the library, documenting that the actual
snapshot envelope is machine-dependent. Reuse the one topology-aware visitor;
condition new state/source fields on installed YM instead of maintaining a
parallel generic migration engine. Validate against the destination machine's
expected format and identity before changing live state or battery bytes.

This is recommended because no YM snapshot exists in the old core, while existing
non-YM clients need not lose progress. Supporting this one unchanged historical
representation is narrower than generalized snapshot migration. Older cores
continue refusing YM content; new YM state carries no backward-load promise.
If inspection or tests show this policy cannot be retained without a materially
new complexity/risk or a public version-field incompatibility, return the precise
consequence for owner decision before choosing a global format break.

## Alternatives and acceptance

Doing nothing preserves refusal and cannot satisfy the selected feature. A
global format/behavior bump with rejection of every older snapshot is simpler,
but disrupts unrelated existing saves and requires a separate owner decision.
A general version-conversion layer adds unrequested migration and validation
surface; exclude it. Silently extending format 1 for YM obscures the schema
change and is not recommended.

Before implementation, adversarially check the recommendation against actual
version consumers, state identity, internal checkpoint use, fixed-size assertions
and source inventories. Prove legacy compatibility with a kickoff-built producer
and new consumer (and reverse direction for unchanged non-YM states), covering
regions, expansions and battery semantics proportionately. Do not construct a
"legacy" fixture with the new serializer and call that independent evidence.
Then verify YM mid-stream continuation, reset/isolation, partial carry and
semantic-invalid/refusal controls. Preserve wrong-media and battery no-mutation
checks. Record concrete version/layout choices in Phase 20 context and docs;
source inspection here is not a test pass or state compatibility claim.
