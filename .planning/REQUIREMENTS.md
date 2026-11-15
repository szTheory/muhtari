# Requirements: muhtari v8.0 — GameDrive streamed-audio contract qualification

**Defined:** 2026-11-15
**Core value:** 7800 games behave as on the console, identically across platforms, from a small MIT-licensed C library hosts can embed.
**Milestone goal:** Determine whether GameDrive streamed ADPCM can be supported by a complete, bounded, source-qualified and lawfully testable contract, and produce a clear go/no-go recommendation without changing Muhtari's public compatibility surface.

## v8.0 Requirements

### Measured milestone tune-up

- [x] **TUNE-12**: Maintainers begin the milestone from current release, CI, hygiene and quality evidence; inspect queue time, critical path, cancellations and reproduced lessons; run cheap applicable consistency checks before longer suites; and record one evidence-backed low-risk correction or a bounded no-change result without weakening required checks or obscuring failures.

### GameDrive streamed-audio qualification

- [ ] **GDA-01**: The qualification records an evidence-backed status for each needed GameDrive audio contract field, including command/register behavior, sidecar discovery and identity, stream framing, playback clock, sample/output mixing, pause/resume/reset, error behavior and state continuation; it separates sourced facts, inference and unknowns and includes source revision and confidence.
- [ ] **GDA-02**: The qualification establishes whether legally authored generated fixtures and an independent expected-PCM oracle can exercise the documented profile without using unlicensed sample/encoder code or copyrighted game data; if not, it records the precise missing evidence and a no-go result.
- [ ] **GDA-03**: The qualification evaluates caller-owned media identity and bounds, deterministic mono output, finite work/memory, lifecycle behavior and save/load continuation against existing Muhtari consumers; it documents possible public API/ABI and state-format consequences without implementing or selecting them.
- [ ] **GDA-04**: The milestone ends with an evidence-backed go/no-go recommendation, strongest alternative, unresolved contradictions, rejected options and concrete revisit triggers; a no-go is accepted when the source or independent-oracle threshold cannot be met.

## Out of Scope

- Implementing ADPCM playback, adding sidecar input, changing `muhtari_media`, changing API/ABI versions or changing state format.
- Physical console, cartridge or owner-device runs; copyrighted ROMs; reuse or retention of unlicensed sample/encoder code.
- BupChip/BupBoop audio, Rikki & Vikki compatibility, new hosts/UI, dependencies, or unrelated accuracy work.
- Renewing any existing host, remote, identity-bound or no-retry stop.

## Requirement-to-Phase Mapping

| Requirement | Phase |
|---|---:|
| TUNE-12 | 23 |
| GDA-01 | 24 |
| GDA-02 | 24 |
| GDA-03 | 24 |
| GDA-04 | 24 |
