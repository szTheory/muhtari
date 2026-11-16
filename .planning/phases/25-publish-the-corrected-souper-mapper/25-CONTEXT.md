# Phase 25: Publish the corrected Souper mapper - Context

**Gathered:** 2026-11-16
**Status:** Ready for planning

<domain>
## Phase Boundary

Publish the already-merged Souper mapper support from corrected mainline as a fresh v0.16.0 release using the established release-please and source-bound Release workflow. Preserve the existing failed v0.15.0 tag and draft without changing their target or assets. Keep the README and status page accurate before and after publication. This phase adds no emulator behavior, API or state changes, new dependencies, host/device tests, GameDrive support, or new compatibility claims.

</domain>

<decisions>
## Implementation Decisions

### Release identity and trigger
- Deliver the newly downloadable Souper capability as the v0.16.0 feature release, following the settled pre-1.0 minor-bump policy.
- Use a truthful `feat:` title for the phase shipping PR because this phase makes the already-merged capability available in a package. The ordinary release-please workflow should create and merge its release PR; do not hand-create a version tag or use a `Release-As` override.
- Keep v0.15.0's release ID, tag target, draft status, and asset set unchanged. Snapshot them before release and compare them again after v0.16.0 publication.

### Source and release gates
- Use the existing `.github/workflows/release.yml` and `scripts/publish.sh` path. Bind CI, equality, artifacts, attestations, publication, and install checks to the exact release-please SHA.
- Preserve all existing required, advisory, integrity, provenance, install, and failure-control checks. Do not change workflow structure, cache policy, or deadlines without repeated evidence.
- Run applicable cheap consistency checks before long local or hosted work, then record each result as a command, exit status, and run URL where applicable.
- The released library, runner, and libretro artifacts must come from the same corrected source and retain the existing generated Souper fixture coverage.

### User-facing status
- Until v0.16.0 is actually published and verified, keep README and status copy clear that v0.14.0 is latest and Souper is merged but unpublished; v0.15.0 remains the failed draft.
- After publication is verified, update README and `docs/status.md` to identify v0.16.0 as latest and retain the known compatibility limits and project completion standard.
- Keep the README short and user-facing; keep detailed receipt and CI analysis in planning/maintainer evidence.

### the agent's Discretion
- Choose the smallest documentation and phase-evidence edits needed to meet the acceptance criteria.
- Adopt the source research recommendation for the release trigger; do not reopen the version choice unless new evidence shows the planned truthful `feat:` title will not produce v0.16.0.

</decisions>

<code_context>
## Existing Code Insights

### Reusable Assets
- `.github/workflows/release.yml` already implements release-please, exact-source CI and equality, archive publication, attestation, download verification, and latest-install checks.
- `scripts/publish.sh` and `tests/policy/publish.cmake` already validate archive inventory, source identity, checksums, attestation, tampered-copy rejection, and published downloads.
- Existing generated-fixture tests exercise Souper through the library, runner, libretro, and portable consumers.

### Established Patterns
- `release-please` uses Conventional Commit types; `feat` increments the minor version before 1.0.0, while docs/test-only commits do not create releases.
- Release evidence is source-specific. A green aggregate with publication jobs skipped is not release acceptance.
- Published artifacts are downloaded and checked against `SHA256SUMS` and the release workflow's attestation before the install-line check is accepted.

### Integration Points
- A phase shipping PR to `main` triggers release-please; its normal release PR produces the new draft/tag and source SHA for exact-source release jobs.
- `README.md` and `docs/status.md` describe release availability; update them only after the published release and latest-install checks pass.
- The v0.15.0 release/tag baseline must be rechecked after the v0.16.0 workflow completes.

</code_context>

<specifics>
## Specific Ideas

Use the successful v7.0 Souper correction and current v8.0 release receipt as source-specific precedents. Current public state is v0.14.0 latest, v0.15.0 draft on its original failing source, and no v0.16.0 release or tag. The corrected Souper source is already on `main`; the new release must prove its own exact-source path.

</specifics>

<deferred>
## Deferred Ideas

None. GameDrive, BupChip, broad title-compatibility claims, and the older Playstead/frontend requirements remain outside this release phase.

</deferred>
