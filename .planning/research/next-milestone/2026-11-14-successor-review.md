---
title: "Successor review after v7.0: homebrew audio candidates"
summary: "Ranks BupChip and GameDrive streamed audio against the evidence threshold for a new Muhtari core milestone."
read_when: "At the next Muhtari milestone boundary, before promoting homebrew audio or accuracy work."
updated: 2026-11-14
muhtari_revision: "3db7b9bf12adbad9f853ded5040aebcbbcd31582"
---

# Successor review after v7.0

## Recommendation

Do not select a new core-behavior milestone yet. The strongest candidate is a
bounded **GameDrive streamed ADPCM profile** for homebrew music, but it would
add sidecar media to the public C API and serialized machine state, while the
available sources do not establish a complete timing and stream contract. This
is a material compatibility decision, not a routine implementation detail.

The strongest alternative is to keep the successor horizon open until a
source-qualified feature profile or a lawful, reproducible title-blocking
counterexample is available. No supported accuracy defect or new Muhtari need
from a sibling consumer is established today.

## Reconciled baseline

- v7.0 Souper mapper support is complete and archived locally. Its audit records
  all eight requirements, both phases, twelve integration seams and five flows
  passing. It claims the selected software mapper model, not retail-game or
  physical-cartridge compatibility. It was not published or merged.
- v7.0 deliberately leaves the Souper `$8007` BupChip audio command inert and
  accepts exactly the 512 KiB ROM profile without a music payload.
- The nine v3.0 host/player requirements and original Phase 12/14 stops remain
  unchanged. No new Playstead or LibFuturo core/API requirement is demonstrated.
- The LibFuturo checkout was observed at `a0f7d956221baa634136a7364c76983f10c542a5`
  with substantial dirty browser-host and GabbaBoy changes. Those changes are
  unqualified and were not used as accepted consumer requirements.

## Candidate comparison

| Candidate | User value | Evidence and cost | Disposition |
|---|---|---|---|
| **GameDrive streamed ADPCM** | Lets 7800 homebrew use external streamed music through a published flashcart interface. | RetroHQ documents `$420`, the `adpcm@420` declaration, volume/play/stop/pause/resume/reset commands and sidecar tracks `0–31.bup`. Muhtari has no sidecar input today. The sample and encoder repositories have no license grant; their code cannot be copied. Public material does not establish playback clock, complete stream framing or independent expected audio. A new versioned media contract, bounded sidecar identity/storage and save-state continuation need an explicit compatibility decision. | **Best candidate if the owner accepts the new media/API contract and a later source-qualified profile gate. Not ready as a committed implementation scope.** |
| **BupChip/BupBoop** | Adds music to Souper-style homebrew and could support a narrow class of games. | TailChao describes a microcontroller running a modified BupBoop and separate music data. A BupBoop subtree has a Zlib notice at `libretro/prosystem-libretro@8a88014287c7a01cd568067e5a557d0a2b2a051f`, but this does not license the adjacent GPL bus adapter. The public `$8007` note omits command meanings, audio cadence and payload layout. The renderer also needs per-instance bounded C17 integration, integer-only setup, mono mixing and serialized state. No lawful retail music fixture is available. | Defer until an authoritative command/media profile and generated acceptance case exist. |
| **One 7800 accuracy/gameplay correction** | Direct player value if it fixes a real blocking case. | Saved accuracy research found no reproduced defect or lawful independent gameplay oracle. DLI timing still needs a separate hardware discriminator. | Promote only after a reproducible counterexample exists; no measurement-only milestone. |
| **No new core behavior** | Avoids unsupported claims and public compatibility changes. | Keeps current contracts stable while preserving candidates and old host stops. | **Strongest alternative to selecting a candidate now.** |

## Evidence register

| Finding | Evidence class and confidence | Source and limit |
|---|---|---|
| Souper is locally complete, but not published/merged | Verified repository artifact; high | [v7.0 audit](../../milestones/v7.0-MILESTONE-AUDIT.md), [roadmap](../../milestones/v7.0-ROADMAP.md). |
| BupChip is a microcontroller running modified BupBoop; Rikki & Vikki has separate game/music data and no standalone raw ROM sale | Original developer statement; high for that product description | [TailChao's Rikki & Vikki post](https://forums.atariage.com/topic/285896-rikki-vikki/). Does not define a general cartridge payload or license game assets. |
| `$8007` sends writes to BupChip | Published flashcart code example; medium | [7800 GameDrive homebrew code example](https://forums.atariage.com/topic/353011-homebrew-code-examples/#findComment-5292592). No command table or clock is supplied. |
| BupBoop's isolated subtree carries a Zlib license | Exact license file at pinned revision; high for that subtree only | [BupBoop license at `8a880142`](https://raw.githubusercontent.com/libretro/prosystem-libretro/8a88014287c7a01cd568067e5a557d0a2b2a051f/bupboop/License.txt). No GPL `core/` files were opened or used. |
| GameDrive exposes `$420` with track and playback controls and stores `.bup` tracks beside the A78 | Published developer instructions plus first-party sample source; medium | [Audio streaming instructions](https://forums.atariage.com/topic/353011-homebrew-code-examples/), [sample repository at `0ef62a5`](https://github.com/RetroHQ/7800GD-Homebrew/tree/0ef62a554354c39d2a6647b7513b23c6729c4d6e). The sample repository has no explicit license and is not a reusable seed. |
| The linked GameDrive encoder repository has no declared license | GitHub metadata/tree inspection; high for the inspected revision | [`7800GD-AudioEncode` at `e55371a`](https://github.com/RetroHQ/7800GD-AudioEncode/tree/e55371a49445e41f82f9dc21ad01ad7cf98c3f13). Do not copy its source. |
| The encoder's implementation appears to use 48 kHz stereo 16-bit input, 4-bit stereo ADPCM, BUP headers and scrub checkpoints | Read-only implementation observation; low as a device specification | [`AudioFile.cpp`](https://github.com/RetroHQ/7800GD-AudioEncode/blob/e55371a49445e41f82f9dc21ad01ad7cf98c3f13/AudioFile.cpp) and [`ADPCMEncode.cpp`](https://github.com/RetroHQ/7800GD-AudioEncode/blob/e55371a49445e41f82f9dc21ad01ad7cf98c3f13/ADPCMEncode.cpp). This unlicensed encoder is not a normative hardware contract or reusable code. |
| A sidecar route changes the public media and snapshot surface | Inference from current interface | `muhtari_media` contains image, BIOS and HSC buffers only; track data would need bounded ownership, identity/refusal rules, mono output policy and continuation state. See [`muhtari_media`](../../../include/muhtari.h). Exact API/ABI and state-version changes require phase design and owner review. |

## Conditions to revisit

Promote GameDrive streamed audio only when the scope can state a bounded,
source-qualified stream contract; define a lawful generated fixture and
independent sample expectations; choose a versioned sidecar API/state policy;
and verify the actual library, runner, loaded libretro core and portable
consumer without host-device or owner-run acceptance. Promote BupChip only when
its command/media schedule is documented well enough to exercise the actual
Souper bus path. Promote accuracy only with a lawful discriminator and
independent expected result.

No builds, tests, emulator hosts, sibling mutations, asset acquisition or remote
actions were performed during this review.
