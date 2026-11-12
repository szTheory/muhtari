# YM2151 State Policy Review

**Scope:** Adversarial source review of `POLICY.md` against the exact pre-feature kickoff source at `fef56919cc145eb09fdb0a0939dfbd7f17012104` and its current consumers. Read-only review; no build or tests were run. The inspected source, API tests, state tests, runner, and diagnostic files are unchanged between that commit and `HEAD` (`fc7d0cb`). Conclusions about cross-version execution below are therefore inferences, not test results.

## Recommendation

Adopt the ordinary additive policy under the already-authorized v6.0 autonomous scope, with the clarifications below. It is the least disruptive option: API 8 and ABI 1 can remain fixed, non-YM state bytes can retain format 1 / behavior 6, and the existing single state visitor already sizes, validates, writes, and applies topology-dependent payloads. A global format break or a general migration layer would unnecessarily affect unrelated snapshots and add unrequested conversion behavior.

The compatibility claim is not proven by source inspection. Before accepting it as implemented, compare old and new producer bytes using the genuine kickoff-built producer and verify both load directions for unchanged non-YM machines. Do not synthesize a purported legacy snapshot using the new serializer.

## Findings

### The visitor is a suitable compatibility seam

`src/state.c` has one `state_visit` implementation used for sizing, writing, validation, and application. Its fixed payload is followed by topology-dependent cartridge RAM, expansion RAM, and battery data (`src/state.c:636-997`, `1189-1238`). This is a workable place to add YM-only state while leaving the non-YM walk and its extent unchanged.

Two details are compatibility-critical:

- The current visitor serializes ten `synth.raw` entries for every machine (`src/state.c:903-907`). Adding YM levels by widening that loop globally would change every non-YM payload length and break the proposed exact legacy envelope. Keep the historical non-YM field sequence and extent.
- `muhtari__state_bind_identity` hashes tagged machine fields and currently ends with synthesis topology at tags 21 and 22 (`src/state.c:174-220`). A YM identity term must be absent for non-YM machines. Hashing a new zero-valued YM field for every machine would change every legacy identity and defeat old-state loading.

The state header currently records fixed format/revision constants and the loader checks the destination's exact expected byte count before checking format/revision (`src/state.c:1249-1325`). Per-machine format selection is not present today, but it can be layered on the current visitor and envelope path without needing a second migration engine. This is a feasibility inference, not an implementation result.

### API 8 / ABI 1 can stay fixed

YM2151 and the load-selection fields already exist in `include/muhtari.h`; the handle is opaque. `muhtari_versions.state_format` was appended by API 8 at byte 28, while the frozen API 7 prefix ends at `behaviour_revision` (`include/muhtari.h:51-58`, `src/internal.h:490-497`). Changing reported version values does not change that public layout. The current public interface reports ABI 1, behavior 6, and format 1 (`src/instance.c:9-14,45-66`).

There is no source evidence that a public field or function must be added for this policy. Keep the current header size, offsets, signatures, and ABI. Update the comment on `state_format`; it currently says `/* 1 */`.

### Global versions and per-machine envelopes need explicit semantics

`muhtari_get_versions` has no instance argument and can only report build-wide values. Under the proposed policy it will report behavior 7 and the latest supported format 2, while non-YM snapshots still carry format 1 / revision 6 and YM snapshots carry format 2 / revision 7. The current docs instead say “The current interface is API 8, ABI 1, behaviour revision 6 and state format 1,” describe `state_format` as “The version of the portable state byte format,” and describe one format-1 envelope (`docs/embedding.md:527-557,603-625`). Clarify that these fields report the latest version supported by the library and that the actual envelope version is machine-dependent. Without that wording, a caller could reasonably assume the global value labels every saved state.

The global behavior bump also has one observable non-YM consequence: `muhtari-run --hash-audio` prints `audio_behaviour_revision` from `muhtari_get_versions` (`runner/main.c:1768-1777`), so that line changes from 6 to 7 for all machines despite unchanged non-YM frame/audio hashes. A movie carrying behavior 6 will likewise produce the runner's existing “replaying anyway” warning (`runner/main.c:1824-1836`). These are concrete metadata/warning changes, not a PCM or frame change. Existing docs define the value as a library behavior revision and the runner output as that global value (`docs/embedding.md:614-620`, `docs/runner.md:518`). Under the stated milestone authority, I consider this expected version metadata rather than a separate scope change; if exact CLI metadata or quiet playback of old movies is a compatibility requirement, the proposed global revision policy would need owner direction.

## Current consumer contradictions to resolve

The repository has version consumers that currently freeze the kickoff values:

- `tests/api/v8.c:30-50` expects reported behavior 6 and format 1, including the older 28-byte prefix behavior.
- `tests/core/test_core.c:837-845` expects behavior 6.
- `tests/diag/driver.c:1327-1331` rejects any behavior value other than 6; its PAL hash report also hard-codes `audio_behaviour_revision 6` (`tests/diag/driver.c:2553-2559,2875-2879`).
- Runner golden/contract checks contain revision-6 metadata expectations (`tests/runner/tests.cmake:122,227,268`; generated audio expectations are also annotated with 6).
- The public header and embedding guide state the current fixed values (`include/muhtari.h:56-57`, `docs/embedding.md:608-612`).

Those consumers must distinguish global reported values (7 / 2) from legacy non-YM state headers (6 / 1). The payload, audio, and state digest baselines for unchanged non-YM machines should remain unchanged; only the documented version metadata should move. The current rejection test that mutates a non-YM snapshot to format 2 or revision 7 remains a useful unsupported-pair check, but it does not prove acceptance of a qualified YM v2/rev7 snapshot.

The existing state suite already covers refusal without machine mutation, checksum and semantic validation, identity mismatch, cartridge/expansion topologies, and committed battery data/generation (`tests/core/test_state.c:129-153,1633-1695,1940-2152,2167-2240`). Preserve those non-YM checks. Keep the fixed legacy identity vector at `tests/core/test_state.c:2224-2249`; it is a useful guard against accidental identity drift.

## Independent compatibility evidence to require

Use commit `fef56919cc145eb09fdb0a0939dfbd7f17012104` as the old producer/consumer source. Its state constants are format 1 / revision 6, it has the same visitor and state API, and its relevant source files match the current inspected tree. Generate deterministic test content and state files in temporary storage using a binary built from that exact commit; do not add save bytes to the repository.

The useful two-way compatibility checks are:

1. Old producer → new consumer: for unchanged non-YM machines, load kickoff-produced format-1/revision-6 states and compare resumed frame/audio/state results with the old producer's continuation.
2. New producer → old consumer: have the new build save the same non-YM machine at the same point and confirm the kickoff consumer restores it and resumes equivalently.
3. Compare same-point old/new non-YM state bytes directly. This proves envelope, identity, and payload stability more strongly than round-tripping bytes produced by one implementation.
4. Repeat the compatibility sample across the meaningful existing state topologies (plain/cart RAM, expansion RAM, and battery-backed state) and both regions where state behavior differs. Reuse the current battery no-mutation and generation assertions.
5. On the new consumer, assert non-YM header 1/6 and YM header 2/7, plus YM-specific refusal/no-mutation controls. An old core cannot install YM content; for a v2 file passed to an old non-YM instance, require safe refusal and no mutation, not a particular status. Its current size-before-revision order can return `ERR_INVALID_STATE` when the new payload extent differs, even though docs generally describe unknown revisions as `ERR_UNSUPPORTED_STATE`.

These are test recommendations only; none were executed in this review.

## Decision

**Compatible enough to adopt without new owner input:** yes, within the already selected autonomous milestone, provided the plan preserves the exact non-YM visitor/identity path, documents global-latest versus per-machine envelope versions, updates version-metadata consumers, and obtains the independent two-way evidence above. No API/ABI bump, global rejection of format-1 snapshots, or general migration framework is justified by the inspected source.

**Must-fix caveat:** the current docs' single-format wording and source-level version assertions do not express the proposed contract. Resolve them before calling the policy implemented. The only additional owner decision would be required if the project intends to freeze the non-YM runner's global revision metadata or suppress the expected warning for old behavior-6 movies; those surfaces will change under global revision 7.

**Confidence:** MEDIUM for feasibility and source-consumer mapping; LOW / untested for cross-version byte and continuation compatibility.
