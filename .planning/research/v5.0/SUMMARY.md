# Project Research Summary

**Project:** muhtari v5.0 — Downloadable WebAssembly core
**Researched:** 2026-11-10
**Confidence:** High for distribution gap and architecture; medium for final packaging details pending pinned license and consumer qualification.

## Executive Summary

Three independent product, core architecture and operations/security reviews converge on packaging the already verified Emscripten libretro core. v0.12.0 publishes 18 native ZIPs plus SHA256SUMS; the loader/Wasm are only CI artifacts. One portable libretro archive is the smallest concrete follow-on to the owner's browser priority. Keep actual browser hosting, devices, Playstead and prior denied/spent host acceptance outside scope.

The owner explicitly delegated broad relevant-role research, coherent recommendation following and autonomous execution for this milestone. Adopt ordinary local scope/design choices without repeated confirmation; publication, hosted rehearsal, merge and material new consequences retain their boundaries. This selects local package/release-contract readiness, not permission to perform those remote actions.

## Key Findings

- Keep the existing pinned toolchain, CMake/sh publisher, Node host and independent native oracle. No dependency or ABI change is indicated.
- Consume the extracted archive, bind the matching pair and all release inputs to one exact source, preserve the 18 native packages and audit generated-runtime notice obligations.
- Retain strict exact-member, content, checksum, attestation, tamper and downloaded-asset checks. The complete release becomes 19 ZIPs/20 assets.
- The first phase remains SEED-007 tune-up. Reproduced stale shipping records are corrected; current CI evidence supports no safe cache, timeout or topology change. Preserve deferred advisory property failures.
- Accuracy and broader tools remain directional until a measured defect or concrete consumer warrants promotion. Separate browser-player/interface seeds remain dormant.

## Implications for Roadmap

Phase 17: measured tune-up and current release/docs reconciliation. Phase 18: source-bound package, extracted offline consumer, generated-runtime provenance and release-contract preparation. Keep acceptance entirely useful local/offline verification. Run only bounded new research that can change these decisions; one adversarial pass checks the combined recommendation.

## Confidence and revisit triggers

The release gap is observed directly; integrator demand is an inference from the owner's existing priority. Revisit the scope for a concrete title-blocking accuracy issue, a proven existing host gap with new authority, or a real browser consumer whose requirements differ. Stop distribution work if pinned output licensing cannot satisfy project policy. Primary-source implementation research remains necessary for exact generated-runtime licensing; no external emulator source was opened during this scope comparison.

## Sources

- [Product intent](../../INTENT.md), [fan-out brief](../fan-out/prompt.txt), SEED-007/008 and the existing scoped browser-player research.
- [Published v0.12.0](https://github.com/szTheory/muhtari/releases/tag/v0.12.0); `gh release view v0.12.0 --json tagName,url,publishedAt,assets`, exit 0.
- [Candidate CI](https://github.com/szTheory/muhtari/actions/runs/42918085281) and [release-PR CI](https://github.com/szTheory/muhtari/actions/runs/42923477030), existing exact-source audits exit 0; [Release](https://github.com/szTheory/muhtari/actions/runs/42931446855) succeeds. Full job responses inspected; receipt in [v4.0 ship](../../milestones/v4.0-SHIP.md).
- Repository primary sources at published source: `scripts/emscripten.sh`, `tests/emscripten`, `docs/libretro.md`, `.github/workflows/ci.yml`, `scripts/publish.sh`, `tests/cmake/archive_contents.cmake` and `tests/cmake/firmware_scan.cmake`.

The research files are milestone-namespaced to retain v3.0 research and its historical links intact. The parent synthesized the three disjoint read-only specialist returns; one writer owns planning changes.

## Adversarial synthesis

One product reviewer challenged the combined scope after the independent reviews. The recommendation stands. The current CI upload omits `muhtari-source-id.txt`, although the consumer requires it. Include exact commit identity separately from core source identity, pinned toolchain identity and digests of the loader/Wasm in the bundle contract. Use one existing Linux x64 producer, test only a clean extraction, and update every exact release inventory. Audit runtime notices before distribution. No owner choice is needed for local package preparation; actual remote rehearsal, publication and merge stay at ship. Further scope fan-out would not change this recommendation; detailed phase research addresses only the remaining concrete implementation questions.
