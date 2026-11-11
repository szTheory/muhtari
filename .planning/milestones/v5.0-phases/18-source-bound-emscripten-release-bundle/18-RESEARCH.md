# Phase 18: Source-bound Emscripten release bundle - Research

**Researched:** 2026-11-10  
**Domain:** Source-bound libretro packaging, offline consumer, release pipeline  
**Confidence:** HIGH for existing seams; MEDIUM for proposed bundle layout pending the paired licensing report.

<user_constraints>
## User Constraints (from CONTEXT.md)

### Locked Decisions
<!-- DATA_q7m4k2vp_START -->
### Bundle and identity
- **D-01:** Add one portable libretro release variant within the existing three deliverables. Retain the matched loader/Wasm, core metadata, exact commit, core-source identity, pinned SDK identity, SHA-256 pair digests, concise host contract and required notices. Exact member paths and prohibited content remain enforced.
- **D-02:** Reuse the existing Linux x64 pinned Emscripten producer; add no second SDK job, dependency, browser UI, device code or ABI/core-state behavior change. Include muhtari-source-id.txt, which the existing consumer requires but current evidence upload omits.

### Extracted consumer and provenance
- **D-03:** Exercise only a clean extraction through the existing offline Node lifecycle and independent native numeric parity oracle; forbid fallback to build outputs. Representative missing, mismatched and tampered package inputs must fail through recurring existing CI checks.
- **D-04:** Audit generated runtime/stdlib distribution rights and notice obligations at the exact pinned SDK inputs using primary sources before distribution. Carry required permissive notices and provenance; unresolved or incompatible licensing blocks distribution.

### Release contracts and instructions
- **D-05:** Extend existing exact-source inventories, checksums, attestations, downloaded/tampered archive validation and rehearsal selection from 18 ZIPs/19 assets to 19 ZIPs/20 assets. Preserve all native archives, native/core gates and required rejection behavior.
- **D-06:** Keep download/extraction/loading instructions concise and reproducible, including companion-file resolution. Clearly distinguish local/offline Node/core evidence from unverified real-browser/device behavior.
- **D-07:** Keep all nine deferred v3.0 requirements and denied/spent Phase 12/14 host/remote stops unchanged. No public rehearsal, publication, remote dispatch, push, merge or actual host/device action is authorized here.
<!-- DATA_q7m4k2vp_END -->

### the agent's Discretion
<!-- DATA_91zv5d2c_START -->
- Choose exact file placement, minimal manifest format and smallest existing CMake/sh integration that satisfy the locked contracts. Reuse existing mechanisms and fault controls before writing new ones.
- Plan useful early license, identity, exact-member and extracted-consumer checks before full workflows; keep review/verification proportional and avoid duplicated workloads. Local builds/retries under one hour are preauthorized after causal diagnosis.
- Adopt ordinary synthesized recommendations under explicit autonomous authorization without routine acceptance prompts. Escalate only a material scope/authority/risk/cost or irreversible-action boundary.
<!-- DATA_91zv5d2c_END -->

### Deferred Ideas (OUT OF SCOPE)
<!-- DATA_5c1n8x4r_START -->
Browser player, device performance, Playstead, prior host evidence and actual public release actions remain outside this local phase.
<!-- DATA_5c1n8x4r_END -->
</user_constraints>

<phase_requirements>
## Phase Requirements

| ID | Description | Research Support |
|----|-------------|------------------|
| WASM-05 | A host integrator can obtain one architecture-independent libretro ZIP containing the matching Emscripten loader/Wasm, core metadata, exact commit and core source identity, pinned toolchain identity, loader/Wasm SHA-256 digests, concise host contract and required notices, with exact allowed members and no ROM, BIOS, fixture, save or measurement bytes. | Package manifest, pair/source/toolchain identity, existing core metadata and notice paths. |
| WASM-06 | A clean extraction of that ZIP passes the existing offline lifecycle and independent native numeric parity consumer without falling back to build outputs; representative missing, mismatched and tampered package controls fail. | tests/emscripten/host.mjs already loads the adjacent .wasm and source identity and compares against native evidence; run it on the extracted loader. |
| WASM-07 | Maintainers can verify the distribution rights and notice obligations of generated runtime/stdlib code at the exact pinned SDK inputs, and the archive carries the required permissive notices and provenance. | The paired 18-LICENSE-RESEARCH.md owns the exact SDK licensing determination; package only its resolved notice/provenance payload. |
| WASM-08 | The existing release pipeline binds the new ZIP to the exact release source and includes it in exact inventories, checksums, attestations, tamper/download checks and rehearsal selection, while preserving all 18 native ZIPs and their required rejection behavior. | Extend scripts/publish.sh's single asset-name iterator and its existing source/checksum/attestation/download consumers; add publish policy fixtures for the new name. |
| WASM-09 | Host integrators have reproducible download/extraction/loading instructions for the bundle, including companion-file resolution and explicit offline Node versus real-browser/device evidence limits. | Extend the existing WebAssembly section of docs/libretro.md and link it from the short README download entry. |
</phase_requirements>

## Project Constraints (from AGENTS.md)

- Preserve three deliverables and keep browser, window, device input/audio and core/ABI/state changes out of scope. [VERIFIED: AGENTS.md:53; verbatim: DATA_2d7m9w4p_START Three deliverables. Library, runner, libretro core. No window, audio-device or input-device code. DATA_2d7m9w4p_END]
- Keep ROM, BIOS, save, fixture and measurement bytes out of the published ZIP; only manifest-approved fixtures may remain in the repository. [VERIFIED: AGENTS.md:54-56; verbatim: DATA_6w2p9r5m_START No ROM, BIOS or save bytes except fixtures listed in tests/roms/manifest.txt. DATA_6w2p9r5m_END]
- Use existing POSIX sh and CMake -P controls; a new/materially changed check must name its fault and cover pass/fault behavior. [VERIFIED: AGENTS.md:75-80; verbatim: DATA_3p8v1n6k_START Repository checks use CMake -P or POSIX sh and reuse existing mechanisms. DATA_3p8v1n6k_END]
- Prefer existing mechanisms, standard library and platform facilities before another dependency. [VERIFIED: AGENTS.md:150-153; verbatim: DATA_9m4k2t7v_START remove the need, reuse what is here, use the standard library, use the platform DATA_9m4k2t7v_END]
- Do not run public rehearsal, publication, remote dispatch, push, merge or host/device actions in this phase. [VERIFIED: 18-CONTEXT.md:26; verbatim: DATA_4r7k2m8c_START No public rehearsal, publication, remote dispatch, push, merge or actual host/device action is authorized here. DATA_4r7k2m8c_END]

## Summary

Create the ZIP from the existing build/emscripten/libretro outputs inside the current Linux x64 Emscripten lane, then run the existing Node lifecycle against a fresh extraction before uploading it. The key consumer constraint is that host.mjs derives the .wasm path and reads muhtari-source-id.txt beside the loader; the current evidence upload includes the loader and Wasm but omits that sidecar. [VERIFIED: tests/emscripten/host.mjs:52-64; verbatim: DATA_4p8k1z6r_START const wasmPath = options.module.replace(/\.mjs$/u, '.wasm'); DATA_4p8k1z6r_END and DATA_7v3m5q9b_START const sourcePath = `${dirname(options.module)}${sep}muhtari-source-id.txt`; DATA_7v3m5q9b_END; .github/workflows/ci.yml:187-201; verbatim: DATA_8c2m6r1p_START build/emscripten/libretro/muhtari_libretro.mjs DATA_8c2m6r1p_END and DATA_5n9w3t4k_START build/emscripten/libretro/muhtari_libretro.wasm DATA_5n9w3t4k_END]

Use one exact-member manifest and one CMake -P bundle check, called from scripts/emscripten.sh. It should create/extract the archive, check commit/source/toolchain metadata and loader/Wasm hashes, then run host.mjs with the extracted module plus the independent native observer inputs. Exercise missing, mismatched and byte-tampered copies. Exact runtime/stdlib licensing and notice payload belong to 18-LICENSE-RESEARCH.md; unresolved rights block distribution. [VERIFIED: 18-CONTEXT.md:20-21; verbatim: DATA_1k5q8w3m_START forbid fallback to build outputs DATA_1k5q8w3m_END and DATA_2r9v6c4p_START unresolved or incompatible licensing blocks distribution DATA_2r9v6c4p_END]

**Primary recommendation:** publish one version-free muhtari-libretro-emscripten.zip through the current archives-* artifact stream, and add it once to scripts/publish.sh's asset iterator. Its current checksum, attestation, tamper and exact-download loops will then cover it. Keep six native commit.txt receipts unchanged and validate the portable ZIP's embedded commit against EXPECTED_SHA. [VERIFIED: .github/workflows/ci.yml:187-201; verbatim: DATA_6k2r9p4m_START if: matrix.os == 'linux' && matrix.arch == 'x64' DATA_6k2r9p4m_END; .github/workflows/release.yml:82-91,230-238; verbatim: DATA_3v5m8q1r_START pattern: archives-* DATA_3v5m8q1r_END and DATA_9c2p6w4k_START merge-multiple: true DATA_9c2p6w4k_END; scripts/publish.sh:22-27,96-108,123-144; verbatim: DATA_7k4n1m8v_START for c in library runner libretro; do DATA_7k4n1m8v_END]

## Architectural Responsibility Map

| Capability | Primary Tier | Secondary Tier | Rationale |
|------------|-------------|----------------|-----------|
| Build and bundle production | Existing Linux x64 CI producer | Pinned Emscripten container | The job already runs scripts/emscripten.sh only for Linux x64. [VERIFIED: .github/workflows/ci.yml:187-189; verbatim: DATA_8v3p2m7k_START if: matrix.os == 'linux' && matrix.arch == 'x64' DATA_8v3p2m7k_END] |
| Archive membership and identity | CMake -P bundle checker | Exact member manifest | Existing archive checks extract files and compare exact member sets. [VERIFIED: tests/cmake/archive_contents.cmake:44-55; verbatim: DATA_4m8q2v6n_START file(ARCHIVE_EXTRACT DATA_4m8q2v6n_END and DATA_1z5r9p3c_START if(missing OR extra) DATA_1z5r9p3c_END] |
| Lifecycle and parity | Offline Node host.mjs | Native libretro.observe receipt | The consumer compares source identity and oracle/frame/audio outputs. [VERIFIED: tests/emscripten/host.mjs:58-99; verbatim: DATA_6p3n8v4q_START native.source_identity === sourceIdentity DATA_6p3n8v4q_END and DATA_2r7m5c9w_START native.numeric_oracles DATA_2r7m5c9w_END] |
| Release provenance | scripts/publish.sh | release.yml artifact collection | One publisher iterator feeds checksum, attestation, tamper and download validation. [VERIFIED: scripts/publish.sh:22-27,90-108,123-144; verbatim: DATA_5c1q8v3m_START for c in library runner libretro; do DATA_5c1q8v3m_END and DATA_8w4n2p6r_START gh attestation verify DATA_8w4n2p6r_END] |

## Standard Stack

No new package or job. Reuse POSIX sh, CMake -P, the pinned Emscripten image and its Node runtime, tests/emscripten/host.mjs, and scripts/publish.sh. [VERIFIED: scripts/emscripten.sh:5,96-108,131-153; verbatim: DATA_4k9p1v7m_START image='emscripten/emsdk@sha256:6bd7aef93408b10bf7b36642cb7c91119b302f720b9ec2d20439f73cf07fe894' DATA_4k9p1v7m_END, DATA_7r2m5w8c_START platform='linux/amd64' DATA_7r2m5w8c_END, and DATA_3p6v9n1k_START node tests/emscripten/host.mjs DATA_3p6v9n1k_END]

The CMake source identity is distinct from the Git commit: package both, comparing the sidecar to native source_identity and the commit to EXPECTED_SHA. [VERIFIED: libretro/CMakeLists.txt:52-80; verbatim: DATA_6m3v8p2r_START string(SHA256 MUHTARI_CORE_SOURCE_ID DATA_6m3v8p2r_END and DATA_9q4w1c7n_START file(WRITE DATA_9q4w1c7n_END]

## Architecture Patterns

### Minimal proposed bundle

Keep the existing RetroArch cores/ and info/ layout. Place the loader, Wasm and source-ID sidecar together under cores/. Add one root metadata file carrying commit, core source ID, pinned SDK image/platform; one SHA256SUMS for the loader/Wasm pair; a concise HOSTING.md; and exact notices resolved by the paired licensing report. Existing libretro archives use cores/ and info/. [CITED: .planning/preparation/ENGINEERING.md:160; VERBATIM: DATA_4v6m8p2q_START the libretro zip holds cores/ and info/ DATA_4v6m8p2q_END]

Proposed exact initial allowlist (finalize notice members from the license report):

~~~text
LICENSE
THIRD-PARTY-NOTICES.md
HOSTING.md
muhtari-bundle.txt
SHA256SUMS
cores/muhtari_libretro.mjs
cores/muhtari_libretro.wasm
cores/muhtari-source-id.txt
info/muhtari_libretro.info
~~~

Track this list at tests/emscripten/bundle-members.txt and make both ZIP creation and validation consume it. Do not put observer JSON, fixture, measurement/surface reports, ROM, BIOS or save bytes in the public ZIP. Add the omitted source ID to the separate emscripten-libretro-evidence upload. [VERIFIED: .github/workflows/ci.yml:193-200; verbatim: DATA_3m8p1v7q_START build/ci/tests/emscripten/native.json DATA_3m8p1v7q_END, DATA_5w2r9c6n_START build/ci/tests/emscripten/measurement.json DATA_5w2r9c6n_END and DATA_1p4v8m7q_START build/ci/tests/emscripten/wasm-surface.json DATA_1p4v8m7q_END]

### Producer-to-consumer flow

~~~text
Linux x64 CI -> existing emscripten.sh producer -> stage exact manifest and ZIP
  -> extract to fresh directory -> verify metadata, member list and pair hashes
  -> host.mjs with extracted loader + native fixture/observer -> upload archives-emscripten
  -> existing release prepare, checksum, attestation, tamper and download loops
~~~

The existing consumer accepts explicit module/fixture/native/mode paths. Point module at the fresh extraction, never build/emscripten; it derives the Wasm and source ID beside that loader. [VERIFIED: tests/emscripten/host.mjs:35-63; verbatim: DATA_2w8p6r4m_START --module PATH --fixture PATH --native PATH --mode tracer|verify|controls|report DATA_2w8p6r4m_END and DATA_7c3n9p1v_START const wasmPath = options.module.replace(/\.mjs$/u, '.wasm'); DATA_7c3n9p1v_END]

### Exact paths to update

| Path | Smallest change |
|------|----------------|
| scripts/emscripten.sh | Create/check the portable ZIP after the current pinned build; run lifecycle from clean extraction before success. Existing script already runs Node in the pinned container. [VERIFIED: scripts/emscripten.sh:131-153; verbatim: DATA_3q8v1m6p_START --network none DATA_3q8v1m6p_END and DATA_6k2r9v4n_START node tests/emscripten/host.mjs DATA_6k2r9v4n_END] |
| tests/emscripten/bundle-members.txt and bundle.cmake | Add exact allowlist plus one CMake -P create/extract/metadata/hash/fault control. Keep native archive_contents.cmake contract untouched. |
| .github/workflows/ci.yml | Add sidecar to evidence and upload the bundle ZIP as an archives-* artifact. The existing release workflow already downloads that pattern. [VERIFIED: .github/workflows/release.yml:82-91,230-238; verbatim: DATA_1w7r4m9p_START pattern: archives-* DATA_1w7r4m9p_END]
| scripts/publish.sh | Add ZIP to names(); update fixed 18/19 output/asset counts; check embedded commit against EXPECTED_SHA. The checksum, attestation, mutation and downloaded ZIP loops already iterate names(). Preserve six native commit receipts. [VERIFIED: scripts/publish.sh:48-55,62-63,96-108,123-144; verbatim: DATA_8p2m5v7q_START expected 6, one per build leg DATA_8p2m5v7q_END and DATA_4n6r9c2w_START 18 archives from DATA_4n6r9c2w_END] |
| tests/policy/publish.cmake | Add the synthetic Emscripten ZIP and publisher fault cases to existing policy fixtures. [VERIFIED: tests/policy/publish.cmake:57-69; verbatim: DATA_6v4q1m8p_START foreach(component library runner libretro) DATA_6v4q1m8p_END and DATA_9p3c7w2m_START foreach(os linux macos windows) DATA_9p3c7w2m_END] |
| .github/workflows/release.yml | Extend rehearsal path filters for the Emscripten producer, bundle checker/manifest and docs. Existing artifact collection already selects archives-*. Do not run the rehearsal in this phase. [VERIFIED: .github/workflows/release.yml:14-20,82-91; verbatim: DATA_3r7m1v6k_START pull_request: DATA_3r7m1v6k_END and DATA_8m4p2w9c_START pattern: archives-* DATA_8m4p2w9c_END] |
| docs/libretro.md, README.md, tests/cmake/install_line.cmake | Document download URL, extraction/loading paths, adjacent Wasm resolution and Node/browser evidence limits; exercise the new README link with the existing install-line check. [VERIFIED: docs/libretro.md:363-405; verbatim: DATA_4z8m2q6v_START const wasmBinary = await readFile(new URL('./muhtari_libretro.wasm', import.meta.url)); DATA_4z8m2q6v_END and DATA_9c1w5r7m_START does not establish browser, mobile, device or physical-input behavior DATA_9c1w5r7m_END] |

### Representative recurring faults

1. **Missing member:** remove Wasm or the source-ID sidecar from a test archive; exact-member validation or extracted consumer rejects it.
2. **Mismatched identity:** alter the embedded commit or source ID; publisher rejects commit != EXPECTED_SHA, and host.mjs rejects source ID != native observer identity.
3. **Tampering:** flip a byte in loader or Wasm; the bundle pair checksum fails. Preserve the existing archive-level tampered-copy attestation rejection and downloaded SHA check. [VERIFIED: scripts/publish.sh:100-108,137-142; verbatim: DATA_8n4r2p6m_START a copy with its last byte flipped verifies DATA_8n4r2p6m_END and DATA_7q1v5m3c_START a downloaded zip differs DATA_7q1v5m3c_END]

## Don't Hand-Roll

| Problem | Use | Why |
|---------|-----|-----|
| Libretro lifecycle and independent parity | host.mjs plus native libretro.observe receipt | Existing consumer checks native identity and numeric/audio/video results. [VERIFIED: tests/emscripten/host.mjs:58-99; verbatim: DATA_9w3m6p2r_START native.source_identity === sourceIdentity DATA_9w3m6p2r_END and DATA_6q4v8n1p_START video hash mismatch DATA_6q4v8n1p_END] |
| Exact ZIP membership | One allowlist and CMake -P checker | Existing checker extracts archive and compares expected vs actual members. [VERIFIED: tests/cmake/archive_contents.cmake:44-55; verbatim: DATA_3n7p5m8v_START file(ARCHIVE_EXTRACT DATA_3n7p5m8v_END and DATA_2r6c9w4m_START lacks: ${missing} DATA_2r6c9w4m_END] |
| Release integrity | scripts/publish.sh | Existing loops already perform checksums, attestations, archive tamper checks and exact-tag downloads. [VERIFIED: scripts/publish.sh:62,96-108,123-144; verbatim: DATA_5m2p8r6v_START SHA256SUMS written DATA_5m2p8r6v_END and DATA_1q7w4n9c_START every download matches SHA256SUMS and verifies DATA_1q7w4n9c_END] |

## Common Pitfalls

- **Missing source-ID sidecar:** host.mjs requires it adjacent to the loader; current evidence artifact paths omit it. Include it in both bundle and evidence artifact. [VERIFIED: tests/emscripten/host.mjs:60-63; verbatim: DATA_7n4m2p8q_START muhtari-source-id.txt DATA_7n4m2p8q_END; .github/workflows/ci.yml:193-200]
- **Testing only build output:** run the lifecycle with the loader under the fresh extraction; `.wasm` and sidecar are resolved alongside it. [VERIFIED: tests/emscripten/host.mjs:55-63; verbatim: DATA_4c8m1p6r_START const wasmPath = options.module.replace(/\.mjs$/u, '.wasm'); DATA_4c8m1p6r_END]
- **Treating the portable ZIP as a seventh native build receipt:** keep the native count six and validate portable commit through its bundle manifest. [VERIFIED: scripts/publish.sh:49-55; verbatim: DATA_5p2n8v4m_START expected 6, one per build leg DATA_5p2n8v4m_END]
- **Claiming browser/device behavior from Node:** state only offline Node/core evidence; current libretro docs explicitly limit its evidence. [VERIFIED: docs/libretro.md:399-405; verbatim: DATA_8r2m5c9w_START does not establish browser, mobile, device or physical-input behavior DATA_8r2m5c9w_END]

## Environment and Validation Scope

No new dependency is identified. Reuse the existing producer's SDK/Docker preflight during implementation; this research does not run it. The project config explicitly disables Nyquist validation and security enforcement, so separate Validation Architecture and Security Domain sections are omitted. Package-legitimacy audit is not applicable because this phase adds no package.

## Assumptions Log

| # | Claim | Section | Risk if Wrong |
|---|-------|---------|---------------|
| A1 | muhtari-libretro-emscripten.zip is the preferred version-free public asset name. | Architecture Patterns | Must align upload, publisher and links; can be settled before implementation. |
| A2 | HOSTING.md and muhtari-bundle.txt are the smallest useful host/provenance files. | Minimal proposed bundle | License report may require additional exact notice members; final allowlist must follow it. |
| A3 | Bundle creation and extraction fit within the existing Emscripten CI step. | Producer-to-consumer flow | Measure before changing workflow topology or deadlines. |

## Open Questions (RESOLVED)

1. **RESOLVED:** `18-LICENSE-RESEARCH.md` qualifies five exact-source notice texts and hashes directly against the local pinned SDK image. Carry its conservative set; no incompatible licensing blocker remains.
2. **RESOLVED:** Use a separate narrow `tests/emscripten/bundle.cmake` checker with the existing CMake/sh mechanisms, retaining native `archive_contents.cmake` expectations. This is the smallest change preserving native contracts.

## Sources

### Primary (HIGH confidence)
- Phase 18 CONTEXT and REQUIREMENTS — locked bundle, release, scope and fault contracts.
- scripts/emscripten.sh, tests/emscripten/host.mjs, tests/emscripten/tests.cmake, libretro/CMakeLists.txt — producer, consumer and source identity.
- .github/workflows/ci.yml, .github/workflows/release.yml, scripts/publish.sh, tests/policy/publish.cmake — artifact and publisher wiring.
- tests/cmake/archive_contents.cmake, tests/release/tests.cmake, docs/libretro.md, preparation ENGINEERING.md and LESSONS.md — current archive, documentation, release and provenance patterns.

## Metadata

**Confidence breakdown:** producer/consumer HIGH; release integration HIGH; proposed member layout MEDIUM pending license research.  
**Research date:** 2026-11-10  
**Valid until:** 2026-12-10, or until the pinned Emscripten image/release workflow changes.
