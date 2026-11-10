---
phase: 12-player-ready-delivery-and-playstead-bridge
plan: "03"
subsystem: documentation
tags: [libretro, release, playstead]
requires: []
provides:
  - Accurate player-facing save-state qualification and an exact published libretro artifact contract
  - Source-linked Playstead handoff and concrete future published-byte replay instructions
affects: [phase-12, release]
actuals:
  tokens: 7400
  tasks: 2
  commits: 2
tech-stack:
  added: []
  patterns:
    - "Keep candidate, published-byte, and paired consumer evidence distinct."
key-files:
  created:
    - .planning/phases/12-player-ready-delivery-and-playstead-bridge/12-03-SUMMARY.md
  modified:
    - README.md
    - docs/libretro.md
    - docs/maintaining.md
    - .planning/research/PLAYSTEAD-HANDOFF.md
key-decisions:
  - "A source-bound release run is recorded narrowly; it does not stand in for held/released input or a separate post-publication downloaded-byte replay."
  - "The paired Playstead result remains pending and no sibling source was changed."
requirements-completed: []
coverage:
  - id: D-01
    description: "The short player entry retains its install path and describes battery saves separately from unqualified frontend state behavior."
    requirement: PLAY-01
    verification:
      - kind: integration
        ref: "cmake -DREADME=README.md \"-DBASE_URL=file://$PWD/build/ci/\" -P tests/cmake/install_line.cmake; exit 0; local run (no run URL); build/ci package is v0.10.0 and does not verify published v0.11.1 bytes."
        status: pass
      - kind: other
        ref: "README.md review against docs/libretro.md; git diff --check; exit 0; local run (no run URL)."
        status: pass
    human_judgment: false
  - id: D-02
    description: "The libretro guide records exact app/core identities, archive and member hashes, attestation constraints, matching architecture, and extracted paths."
    requirement: PLAY-01
    verification:
      - kind: other
        ref: "docs/libretro.md artifact table; release archive SHA-256 7775c014de046c514ab4ad6535ed95ae99643344104d0a43b753895726dc2de3; core SHA-256 7ef06f36bfa8a357e47834dc1c04fa85dcb1ef058a2db8551bce4ae4b85492cd; info SHA-256 2a89f2ac1379800c871bc89d7c9fe4bac0c6af88140a5356a1ea820bb1d5d8b5; offline attestation verified for refs/heads/main and source d58f125297c4d63a7a8108112a5eafff6f53a76a."
        status: pass
    human_judgment: false
  - id: D-03
    description: "The external launch contract documents isolated paths, input/focus ownership, refusal and exit distinctions, and graceful persistence limits."
    requirement: PLAY-02
    verification:
      - kind: other
        ref: "docs/libretro.md reviewed against libretro/libretro.c and tests/retroarch/run_retroarch.cmake; BankSet/HSC refusal and host path semantics checked."
        status: pass
    human_judgment: false
  - id: D-07
    description: "The Playstead handoff records its inspected revision and dirty-source limits, ownership boundary, and exact still-pending paired acceptance evidence."
    requirement: PLAY-04
    verification:
      - kind: other
        ref: ".planning/research/PLAYSTEAD-HANDOFF.md; read-only Playstead inspection at 67d25183468d3cbc774b6c91946f8a0c25518d4b; no sibling files changed."
        status: pass
    human_judgment: false
  - id: D-08
    description: "The handoff preserves RetroArch-managed initial state UI, distinguishes recent play from exact restore, and keeps native hosting contingent on measured friction or an explicit one-app trigger."
    requirement: PLAY-04
    verification:
      - kind: other
        ref: ".planning/research/PLAYSTEAD-HANDOFF.md reviewed against the inspected ContinueShelfView and state-support paths; paired flow and 7800 state support remain explicitly pending."
        status: pass
    human_judgment: false
metrics:
  duration: "Continued from the prior execution session; exact start time unavailable"
  completed: 2026-11-09
  status: complete
duration: "Continued from the prior execution session; exact start time unavailable"
completed: 2026-11-09
status: complete
---

# Phase 12 Plan 03: Player and Release Documentation Summary

**The player contract now describes the current release accurately, and maintainers have a source-pinned procedure for replaying the published archive through the existing host checks.**

## Accomplishments

- Kept README installation instructions intact and clarified that core state serialization does not qualify RetroArch's state UI, rewind, or run-ahead.
- Documented the v0.11.1 macOS arm64 archive, extracted core and `.info` digests, pinned RetroArch package, source-bound attestation, launch configuration, refusal reporting, and persistence ownership.
- Refreshed the Playstead handoff from a read-only checkout inspection. It records the inspected revision and dirty paths, assigns adapter and launch responsibilities, and keeps the paired launch/input/return/battery result pending.
- Added a ship-time procedure that pins attestation to the exact source SHA and reuses the existing DMA, held/released input, and two-process battery checks with the downloaded release archive.

## Verification

- `cmake -DREADME=README.md "-DBASE_URL=file://$PWD/build/ci/" -P tests/cmake/install_line.cmake` — exit 0. The unchanged install-line consumer installed core and info files from the local `build/ci` package. That package is v0.10.0, so this proves the local installer contract only; it is not a v0.11.1 published-byte replay.
- `ctest --test-dir build/ci -R '^docs[.]budgets$' --no-tests=error --output-on-failure` — exit 0, 1/1.
- `git diff --check` — exit 0.
- Source and documentation review checked launch/refusal behavior against the loader and `tests/retroarch/run_retroarch.cmake`, and checked the handoff against the inspected Playstead source snapshot.
- The retained [v0.11.1 release run](https://github.com/szTheory/muhtari/actions/runs/41513877501/attempts/1) passed DMA screenshot comparison and fresh-process battery continuity. It had no held/released gameplay-input step. A separate post-publication download and replay remains pending.

## Scope Still Pending

No RetroArch process was launched and no host allowance was used. This plan did not establish held/released gameplay input, a separate replay of downloaded published bytes, or a paired Playstead launch. The Windows pipeline measurement remains with blocked plan 12-02. Phase 12 acceptance remains blocked; `12-VERIFICATION.md`, its accepted 0/4 result, phase state, and its pre-existing dirty content were left unchanged. The protected verification report SHA-256 remains `97c4dd55f212e69927c59ee11b5c36b441f38f2f3e4d9e2115d6a618004750ea`.

## Task Commits

1. `f8cfbcb` — clarify player and libretro launch contract.
2. Recorded in the commit that adds this summary and the ship/handoff documentation.

## Self-Check

Plan 12-03's two documentation tasks and local checks are complete. This closes the documentation plan only; it does not claim Phase 12 acceptance or authorize host, remote, or sibling-repository work.
