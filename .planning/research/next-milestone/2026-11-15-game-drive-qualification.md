---
title: "v8.0 recommendation: qualify the GameDrive streamed-audio contract"
summary: "Reconciles the v7.0 shipping outcome and newer GameDrive firmware evidence before selecting a bounded qualification milestone."
read_when: "At the v8.0 milestone boundary and before any GameDrive audio implementation proposal."
updated: 2026-11-15
muhtari_revision: "bb70dcf86a3872d3981304bd8c6f664dc58dc401"
---

# v8.0 recommendation: qualify the GameDrive streamed-audio contract

## Recommendation

Select **v8.0 — GameDrive streamed-audio contract qualification**. This milestone
will decide whether a future Muhtari implementation can be based on a complete,
bounded, source-qualified contract and a lawful independent audio oracle. It will
not implement ADPCM, add a sidecar API, change public state format, or choose an
API/ABI compatibility policy. Those choices remain owner decisions after the
evidence is ready.

The strongest alternative is to keep the feature horizon open until a complete
source profile or a lawful, reproducible title-blocking defect appears. That
avoids spending a milestone on a candidate whose timing and stream format remain
underspecified. GameDrive is still the best bounded qualification candidate:
the published command and sidecar convention provide a concrete starting point,
and the current firmware notes now make audio state save/load an observed
requirement worth qualifying.

## Reconciled baseline

- v7.0's mapper implementation is merged on `main`. PR #63 added the Souper
  profile; its v0.15.0 release draft and tag target
  `2b47656cb0307fb5fc10d9ea3fbfd2236830c355`. That exact source failed the
  Wasm32 `core.state.souper` test because two 32 KiB local arrays exceeded the
  64 KiB WASI test stack.
- PR #65 moved the reusable test scratch buffer to static test storage without
  weakening assertions. Its exact PR CI and Nightly equality passed, including
  the full hosted Wasm32 test lane. The fix is merged as `bb70dcf`; the
  post-merge main CI is recorded in the v7.0 shipping receipt.
- The release workflow on the test-only follow-up succeeded but skipped
  publication. The v0.15.0 tag still identifies the earlier failed source, so
  v7.0 is implemented and merged but the package remains unpublished. Preserve
  that exact-source release stop; do not retarget the tag or claim publication.
- The v7.0 mapper scope remains mapper-only. BupChip audio, Rikki & Vikki
  compatibility, extra flash variants and physical-cartridge claims are not
  part of this recommendation.
- LibFuturo owns its browser host and consumer work. No new Muhtari-side need
  from that project is established here. Atari 2600 remains owned by
  `twentysixhundo`.

## Candidate comparison

| Candidate | Value | Evidence and cost | Decision |
|---|---|---|---|
| GameDrive streamed ADPCM qualification | Determines whether 7800 homebrew audio can later use an accurately bounded Muhtari contract. | First-party material documents `$420` controls and sidecar tracks. Current firmware notes mention a possible playback-glitch fix and save/load of ADPCM audio. The complete stream timing/framing and independent expected PCM contract are still missing. | **Select a qualification-only milestone.** |
| Hold the horizon open | Avoids spending work on an incompletely specified feature. | Preserves compatibility and resource budgets; yields no new contract evidence unless upstream material changes. | Strongest alternative; revisit if qualification cannot close the evidence gaps. |
| Implement ADPCM now | Could enable music in a subset of homebrew. | Would guess at framing, playback clock, mixing and state semantics and would add a new sidecar/API/state surface. The sample/encoder repositories have no license grant. | Reject until the qualification gate and a separate compatibility decision pass. |
| BupChip/BupBoop | Could add music to Souper homebrew. | Public `$8007` material still lacks a complete bus command, audio cadence and payload format. | Defer; weaker evidence than GameDrive. |
| General 7800 accuracy correction | Direct player value if a real game is blocked. | Saved review has no reproduced defect with a lawful independent oracle. | Keep open as a trigger, not a measurement-only milestone. |

## Current source evidence

| Finding | Evidence class and confidence | Source and limit |
|---|---|---|
| GameDrive firmware 26.01.2266 notes a potential ADPCM playback-glitch fix and adds saving/loading ADPCM audio state. | First-party firmware release notes; high confidence about what the note says. | [RetroHQ 7800 Game Drive firmware history](https://www.retrohq.co.uk/FW/DW6CQ1Q4J6VCK), reviewed 2026-11-15. “Potential fix” is not a correctness guarantee or a timing specification. |
| The `$420` interface has stream controls and a same-basename sidecar convention. | Flashcart developer examples; medium confidence for the documented command surface. | [7800 GameDrive homebrew code examples](https://forums.atariage.com/topic/353011-homebrew-code-examples/). These examples do not define the complete wire/media timing contract. |
| GameDrive audio continuation deserves explicit state qualification. | Inference from the firmware note; medium confidence as a requirement to investigate. | The release note confirms firmware-side save/load support, not the exact emulated state tuple or serialization format Muhtari needs. |
| The linked audio sample and encoder have no declared license grant. | Repository metadata at inspected revisions; high for those revisions. | Sample at `0ef62a554354c39d2a6647b7513b23c6729c4d6e`; encoder at `e55371a49445e41f82f9dc21ad01ad7cf98c3f13`. Do not copy or retain their source as a Muhtari seed. |
| A sidecar route would affect caller-owned media, public mono output and save-state compatibility. | Inference from Muhtari's existing media and state interfaces; high. | `include/muhtari.h`, current v6/v7 compatibility decisions and `.planning/preparation/DECISIONS.md`. Exact fields and version changes require a later decision. |

The newer firmware note raises the relevance of playback recovery and state
continuation. It does **not** resolve stream framing, per-sample clocking,
independent expected PCM, mixing into Muhtari's public mono output, or what must
be serialized. Those remain explicit unknowns for Phase 24.

## Scope and stop conditions

The planned Phase 23 tune-up will inspect current release, CI, hygiene and
quality evidence, including job queue time, the actual critical path and
canceled work. It will run cheap, relevant consistency checks before any longer
suite and record either a reproduced low-risk correction or a bounded no-change
result. Current PR evidence shows zero queue time; Windows ARM64 took 41m55s and
was the CI critical path, while Nightly equality took 22m58s. These single-run
measurements do not justify changing timeouts, caches or parallelism.

Phase 24 will inventory the authoritative stream contract, lawful fixture
options, independent expected audio, lifecycle/state consequences, finite
resource limits and existing-consumer integration. It will produce a go/no-go
recommendation. If authoritative material cannot close a required contract
field, the accepted outcome is a documented no-go with the precise missing
evidence and revisit trigger; no implementation may guess across it.

No physical-console/host run, copyrighted ROM, unlicensed source reuse, product
implementation, new dependency, public API/ABI change or state-format change is
authorized by this milestone selection. Preserve all existing Phase 12/14,
host, remote, identity and release-source stops.
