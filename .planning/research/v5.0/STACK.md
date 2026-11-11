# Technology Stack — v5.0

Research date: 2026-11-10. Baseline: published v0.12.0, canonical main.

Reuse C17, CMake/CTest, POSIX sh, the digest-pinned Emscripten image, bundled Node and existing publisher. Add no dependency or API/ABI change. Distribute the existing libretro target in one architecture-independent ZIP rather than duplicating it in six native archives. Existing source consumers remain valid.

The current producer is `scripts/emscripten.sh`; the CI upload is in `.github/workflows/ci.yml`. The existing `tests/emscripten` host and native observer are the numerical/lifecycle oracle. Package creation should use existing archive facilities. The exact pinned generated runtime/stdlib licensing must be audited before distribution; a permissive compiler does not by itself establish every output component's notice obligations.

Unknown until implementation research: exact minimum generated-runtime notices, archive integration points and how best to retarget the existing consumer to extracted files. Verify these against actual pinned inputs; no new framework is justified.
