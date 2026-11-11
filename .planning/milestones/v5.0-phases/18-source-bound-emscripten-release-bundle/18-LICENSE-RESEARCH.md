# Phase 18: Emscripten Bundle Licensing Research

<user_constraints>
## User Constraints (from CONTEXT.md)

### Locked Decisions

### Bundle and identity
- **D-01:** Add one portable libretro release variant within the existing three deliverables. Retain the matched loader/Wasm, core metadata, exact commit, core-source identity, pinned SDK identity, SHA-256 pair digests, concise host contract and required notices. Exact member paths and prohibited content remain enforced.
- **D-02:** Reuse the existing Linux x64 pinned Emscripten producer; add no second SDK job, dependency, browser UI, device code or ABI/core-state behavior change. Include `muhtari-source-id.txt`, which the existing consumer requires but current evidence upload omits.

### Extracted consumer and provenance
- **D-03:** Exercise only a clean extraction through the existing offline Node lifecycle and independent native numeric oracle; forbid fallback to build outputs. Representative missing, mismatched and tampered package inputs must fail through recurring existing CI checks.
- **D-04:** Audit generated runtime/stdlib distribution rights and notice obligations at the exact pinned SDK inputs using primary sources before distribution. Carry required permissive notices and provenance; unresolved or incompatible licensing blocks distribution.

### Release contracts and instructions
- **D-05:** Extend existing exact-source inventories, checksums, attestations, downloaded/tampered archive validation and rehearsal selection from 18 ZIPs/19 assets to 19 ZIPs/20 assets. Preserve all native archives, native/core gates and required rejection behavior.
- **D-06:** Keep download/extraction/loading and companion-file resolution instructions concise and reproducible. Clearly distinguish local/offline Node/core evidence from unverified real-browser/device behavior.
- **D-07:** Keep all nine deferred v3.0 requirements and denied/spent Phase 12/14 host/remote stops unchanged. No public rehearsal, publication, remote dispatch, push, merge or actual host/device action is authorized here.

### the agent's Discretion
- Choose exact file placement, minimal manifest format and smallest existing CMake/sh integration that satisfy the locked contracts. Reuse existing mechanisms and fault controls before writing new ones.
- Plan useful early license, identity, exact-member and extracted-consumer checks before full workflows; keep review/verification proportional and avoid duplicated workloads. Local builds/retries under one hour are preauthorized after causal diagnosis.
- Adopt ordinary synthesized recommendations under explicit autonomous authorization without routine acceptance prompts. Escalate only a material scope/authority/risk/cost or irreversible-action boundary.

### Deferred Ideas (OUT OF SCOPE)

Browser player, device performance, Playstead, prior host evidence and actual public release actions remain outside this local phase.
</user_constraints>

<phase_requirements>
## Phase Requirements

| ID | Description | Research Support |
|----|-------------|------------------|
| WASM-07 | Maintainers can verify the distribution rights and notice obligations of generated runtime/stdlib code at the exact pinned SDK inputs, and the archive carries the required permissive notices and provenance. | Exact pinned Emscripten, musl, and compiler-rt source notices, revisions, local-image digest checks, and archive-member recommendation below. |
</phase_requirements>

**Researched:** 2026-11-10  
**Domain:** Emscripten-generated JavaScript and WebAssembly runtime licensing  
**Confidence:** MEDIUM overall; HIGH for pinned image identity and notice-file bytes, MEDIUM for the compiler-rt source-base mapping.

## Findings

The exact image named by the existing producer is locally available. The producer pins the Docker image index and selects linux/amd64:

DATA_7F2A9C61_START
image='emscripten/emsdk@sha256:6bd7aef93408b10bf7b36642cb7c91119b302f720b9ec2d20439f73cf07fe894'
platform='linux/amd64'
DATA_7F2A9C61_END

[VERIFIED: scripts/emscripten.sh:5-6]

The matching qualification receipt records emcc 7.0.0 at commit 777e6e5db7b4990bcaeb1b9998ed642cc84e6629 and a successful production .mjs/.wasm build with tests disabled:

DATA_1C95D0A7_START
"imageIndex": "sha256:6bd7aef93408b10bf7b36642cb7c91119b302f720b9ec2d20439f73cf07fe894"
"selectedPlatform": "linux/amd64 (x86_64)"
"emcc": "7.0.0 (777e6e5db7b4990bcaeb1b9998ed642cc84e6629)"
DATA_1C95D0A7_END

[VERIFIED: build/ci/tests/emscripten/qualification.json:4-10]

DATA_B509E13A_START
"observation": "Production .mjs and .wasm built with tests disabled and pinned CMake 4.4.3."
DATA_B509E13A_END

[VERIFIED: build/ci/tests/emscripten/qualification.json:38-43]

The production target emits a modularized ES module for web and Node. Its checked-in link flags do not opt into Emscripten's embedded-license setting:

DATA_82B17E49_START
"-sMODULARIZE=1"
"-sEXPORT_ES6=1"
"-sENVIRONMENT=web,node"
"-Wl,--no-entry"
"-sINCOMING_MODULE_JS_API=['wasmBinary']"
DATA_82B17E49_END

[VERIFIED: libretro/CMakeLists.txt:11-21]

At the pinned Emscripten source, EMIT_EMSCRIPTEN_LICENSE defaults to false; the official setting description says it emits Emscripten license information in generated JavaScript. Therefore the package must carry explicit notices rather than rely on the .mjs containing them. [CITED: [pinned settings.js](https://github.com/emscripten-core/emscripten/blob/777e6e5db7b4990bcaeb1b9998ed642cc84e6629/src/settings.js#L1496-L1498), [settings reference](https://emscripten.org/docs/tools_reference/settings_reference.html#emit-emscripten-license)]

The exact notice files in the local pinned image were hashed with a read-only, network-disabled linux/amd64 container. Command exited 0; no image pull or build ran. The hashes match the same upstream files at the immutable Emscripten 7.0.0 commit. [CITED: [Emscripten 7.0.0 release](https://github.com/emscripten-core/emscripten/releases/tag/7.0.0); pinned-image inspection: `docker run --rm --read-only --network none --platform linux/amd64 … sha256sum …` (exit 0)]

| Component in generated bundle | Pinned source and licensing | Carry in the release ZIP |
|---|---|---|
| Emscripten-generated JS runtime, including Node environment support | Emscripten 7.0.0 commit 777e6e5db7b4990bcaeb1b9998ed642cc84e6629. The pinned LICENSE gives the Emscripten dual MIT/UIUC grants and includes the Node.js MIT notice for Emscripten's Node helper. [CITED: [LICENSE](https://github.com/emscripten-core/emscripten/blob/777e6e5db7b4990bcaeb1b9998ed642cc84e6629/LICENSE)] | LICENSE and AUTHORS |
| libc functions linked into the Wasm target | The pinned Emscripten source bundles musl; its COPYRIGHT grants MIT and contains the associated notices. [CITED: [musl COPYRIGHT](https://github.com/emscripten-core/emscripten/blob/777e6e5db7b4990bcaeb1b9998ed642cc84e6629/system/lib/libc/musl/COPYRIGHT), [Emscripten LICENSE](https://github.com/emscripten-core/emscripten/blob/777e6e5db7b4990bcaeb1b9998ed642cc84e6629/LICENSE)] | COPYRIGHT |
| Compiler runtime builtins, if selected by the link | The exact vendored compiler-rt LICENSE.TXT contains Apache-2.0 with LLVM Exceptions and explicitly permits compiler-rt under either MIT or UIUC. Its copyright notices refer to CREDITS.TXT. [CITED: [compiler-rt LICENSE.TXT](https://github.com/emscripten-core/emscripten/blob/777e6e5db7b4990bcaeb1b9998ed642cc84e6629/system/lib/compiler-rt/LICENSE.TXT), [CREDITS.TXT](https://github.com/emscripten-core/emscripten/blob/777e6e5db7b4990bcaeb1b9998ed642cc84e6629/system/lib/compiler-rt/CREDITS.TXT)] | LICENSE.TXT and CREDITS.TXT |

The Emscripten release identifies compiler-rt as updated to LLVM 23.1.3. The exact vendored README says it is based on llvm-project release 23.1.3 and includes Emscripten-specific patches from the emscripten-libs-23 branch. The immutable source snapshot actually consumed is Emscripten commit 777e6e5db7b4990bcaeb1b9998ed642cc84e6629; the upstream LLVM tag resolves to commit 0d261d1ca552c95a8f007e061c787ac7132fbcbc. [CITED: [Emscripten release notes](https://github.com/emscripten-core/emscripten/releases/tag/7.0.0), [vendored compiler-rt README](https://github.com/emscripten-core/emscripten/blob/777e6e5db7b4990bcaeb1b9998ed642cc84e6629/system/lib/compiler-rt/README.md), [LLVM 23.1.3 tag](https://github.com/llvm/llvm-project/releases/tag/llvmorg-23.1.3)]

## Recommended notice payload and provenance

Keep the existing muhtari LICENSE and THIRD-PARTY-NOTICES.md members. Add these exact upstream texts as deterministic ZIP members, copying from the pinned image and verifying each digest during packaging:

| ZIP member | Upstream path | SHA-256 |
|---|---|---|
| licenses/emscripten/LICENSE | emscripten/LICENSE | 620a78084fc7ca97c0b5dea9abf891f3ffcadfdbf305276f099c9c4e12fc1d86 |
| licenses/emscripten/AUTHORS | emscripten/AUTHORS | 3fb38cc87b2b57a2a1aae3ad2e984c3bc23cf7441d20e6897739fbc3f96414c4 |
| licenses/musl/COPYRIGHT | emscripten/system/lib/libc/musl/COPYRIGHT | b870108ec5e7790e9f9919064f1b9421d62d5f9b0e6c230c6adf7ea2da62e97b |
| licenses/compiler-rt/LICENSE.TXT | emscripten/system/lib/compiler-rt/LICENSE.TXT | 1a8f1058753f1ba890de984e48f0242a3a5c29a6a8f2ed9fd813f36985387e8d |
| licenses/compiler-rt/CREDITS.TXT | emscripten/system/lib/compiler-rt/CREDITS.TXT | a9901f47a089da41e4690682d00ce4cedaa2baf41fedbe79beee366d43ac2461 |

Add an Emscripten runtime section to THIRD-PARTY-NOTICES.md with the image digest, platform, Emscripten commit, LLVM base release/commit, source paths, hashes, scope, and the applicable full license texts. This follows PROVENANCE.md §5. The upstream README hash used to identify the vendored compiler-rt base is 666db98e003717c7321c2ea6ee16b51de07ce56f209012cd7fb83a5272fe706d.

CREDITS.TXT contains upstream contributor names and email addresses. Keep this as an archive-only notice copied byte-for-byte from the pinned SDK; do not add owner identity or broadly relax hygiene's email checks. If an archive-content scanner flags required attribution, allow only this exact generated member and digest.

## Nonblocking limit

The existing qualification receipt identifies the image and successful output but does not record a link map proving which compiler-rt archive members were selected. Carrying the full pinned compiler-rt LICENSE.TXT and CREDITS.TXT unconditionally covers either result, so this does not block licensing or package planning. No incompatible license or further owner decision was found.

## Sources

- [Emscripten 7.0.0 release](https://github.com/emscripten-core/emscripten/releases/tag/7.0.0) — release commit and compiler-rt update.
- [Pinned Emscripten LICENSE](https://github.com/emscripten-core/emscripten/blob/777e6e5db7b4990bcaeb1b9998ed642cc84e6629/LICENSE) and [AUTHORS](https://github.com/emscripten-core/emscripten/blob/777e6e5db7b4990bcaeb1b9998ed642cc84e6629/AUTHORS) — generated-runtime, Node helper, and attribution terms.
- [Pinned musl COPYRIGHT](https://github.com/emscripten-core/emscripten/blob/777e6e5db7b4990bcaeb1b9998ed642cc84e6629/system/lib/libc/musl/COPYRIGHT) — libc distribution terms.
- [Pinned compiler-rt LICENSE.TXT](https://github.com/emscripten-core/emscripten/blob/777e6e5db7b4990bcaeb1b9998ed642cc84e6629/system/lib/compiler-rt/LICENSE.TXT), [CREDITS.TXT](https://github.com/emscripten-core/emscripten/blob/777e6e5db7b4990bcaeb1b9998ed642cc84e6629/system/lib/compiler-rt/CREDITS.TXT), and [README](https://github.com/emscripten-core/emscripten/blob/777e6e5db7b4990bcaeb1b9998ed642cc84e6629/system/lib/compiler-rt/README.md) — exact component licensing and LLVM base.
- [LLVM 23.1.3 release](https://github.com/llvm/llvm-project/releases/tag/llvmorg-23.1.3) and [LLVM Developer Policy](https://llvm.org/docs/DeveloperPolicy.html#legacy-license-structure) — upstream runtime licensing policy.
- [Emscripten EMIT_EMSCRIPTEN_LICENSE setting](https://emscripten.org/docs/tools_reference/settings_reference.html#emit-emscripten-license) — optional JS license emission.

**Research date:** 2026-11-10  
**Disposition:** WASM-07 licensing is resolved for planning with the conservative exact-source notice set above. No package, code change, image pull, build, distribution, or commit was performed.
