# Architecture — v5.0

Keep the deterministic core and three existing deliverables. The browser-targeted package is a libretro variant, not a fourth product. No UI, device handling, core scheduler or state-format change is proposed.

Use the existing pinned Linux x64 Emscripten producer once. Package its matched `.mjs` and `.wasm` with source identity and audited notices. Extract into a clean directory and direct the existing offline Node/native oracle to the extracted loader. The package path must be exercised; testing only the original build directory would miss packaging errors.

Carry that archive through the exact-source producer/join, checksums, attestations, tamper rejection and downloaded-asset verification. The release contract becomes 19 ZIPs and 20 assets including SHA256SUMS. Preserve native archive membership, metadata and validation. Derive precise changes from actual consumers rather than adding a second publishing pipeline.

Order: measured tune-up (Phase 17), then packaging and release-contract preparation (Phase 18). The orchestrator owns shared STATE/ROADMAP. Preserve older dirty worktrees and failed evidence; import nothing implicitly.
