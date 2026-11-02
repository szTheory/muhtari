# Technology Stack

**Project:** muhtari v3.0 — Playstead-ready play and states
**Researched:** 2026-10-29
**Scope:** New dependency, host deployment, test, and recurring tune-up surfaces.

## Recommendation

Keep the current C17/CMake/CTest stack and add no runtime or test-framework dependency. Implement one project-owned, fixed-size, versioned, byte-wise little-endian state format behind caller-buffer library APIs. Reuse it in the runner and the libretro adapter; keep file I/O in the runner and the host. Extend the existing text movie reader with recording only for the gamer and Playstead continuation paths. Defer compressed states and BK2 import until a real consumer requires them.

| Area | Current | Smallest useful addition | Reason |
|---|---|---|---|
| Core | C17 instance-owned library; no allocation after load; project-owned checksums and chip modules | `state` module and public size/save/load calls | The three consumers need one deterministic source of truth. Explicit LE encoding works on the existing big-endian lane and avoids persisting compiler layout. |
| Runner | C17 headless executable; standard file I/O; movie2 replay; atomic battery-file replacement | State file read/write and movie recording using existing CLI, parser, hashing, and file-writing patterns | The runner already owns paths and persistence. Keep the core free of filesystem and platform APIs. |
| Libretro | Thin adapter with the vendored MIT `libretro.h`; serialize functions return unsupported and `.info` claims `savestate = false` | Delegate serialize/unserialize to the same library state API; claim `deterministic` only after round-trip and replay evidence | A second adapter-specific serializer would duplicate state ownership and invite drift. The current host harness already loads the module and resolves all three serializer exports. |
| Build and tests | CMake 3.25+, CTest, `cmake --workflow --preset ci`, exact test inventories, seeded properties, standalone fuzz replays and nightly libFuzzer | Register state/movie cases in existing subsystem files and inventories; add the two planned fuzz targets | The existing cross-platform pipeline already handles core, runner and adapter consumers. CMake 3.25 introduced workflow presets and `cmake --workflow`; no build-system upgrade is needed. |
| Release and host install | Separate library, runner, and libretro ZIPs for supported native platforms; release digests and attestations | Publish the updated runner/core through the same component archives; Playstead consumes pinned release artifacts | No plugin or new package manager is needed. For interactive Playstead use, RetroArch remains the GUI host and muhtari supplies its core; the runner remains the headless automation path. |

Current release is v0.10.0, API 7, ABI 1, behavior revision 6. A public library API addition advances the project API version; state-format versioning stays a separate compatibility boundary. Preserve the append-only ABI rules and existing CMake package/export path.

## Alternatives Considered

| Category | Recommended | Alternative | Why not now |
|---|---|---|---|
| State codec | Fixed-size project format, byte-wise LE, validate before apply | zlib-compressed states or a general serialization package | Libretro benefits from fixed size and stable offsets; compression adds runtime/build and update work without a demonstrated storage or bandwidth problem. |
| Libretro support | Reuse the library state API | Serialize adapter fields independently or import `libretro-common` | Either duplicates ownership or adds a broad helper dependency where three API calls already provide the seam. |
| Recording | Extend the existing versioned text movie format | Add ZIP/BK2 support now | Reproducible input and continuation do not require TAS interchange; imported formats have timing/synchronization questions and additional parsing surface. Revisit for a demonstrated external consumer. |
| Properties | Existing deterministic in-repo property harness and CTest | `theft` or another generator | The current harness runs on MSVC and reports seeded repros; `theft` needs POSIX process APIs and has no Windows leg. |
| CI reporting | Repair the current GitHub Actions digest and test registrations | Add a metrics service, dashboard, or new workflow framework | Current debt is about evidence provenance and censored measurements, not missing infrastructure. |

## Host and Compatibility Contract

The [official libretro core guidance](https://github.com/libretro/docs/blob/master/docs/development/cores/developing-cores.md) treats serialization as optional, requires a nonzero size to mean fully supported, says the frontend queries size before allocating, accepts larger buffers, and forbids the reported size from increasing between game load and unload. It recommends stable offsets because rewind stores successive states. RetroArch [run-ahead](https://docs.libretro.com/guides/runahead/) also relies on clean, fast save/load. Keep the reported size fixed for a loaded game, ignore or clear caller excess, return failure for unsupported/no-game and rejected input, and avoid serialization quirks that claim platform dependence. The official [core-info template](https://github.com/libretro/libretro-super/blob/master/dist/info/00_example_libretro.info) distinguishes `basic`, `serialized`, and `deterministic` savestate claims.

The project's format should be portable across endian and word-size boundaries, not a raw struct dump. Use its own format version and bounds; retain transactional rejection (a bad state leaves the running instance unchanged), caller-owned buffers, and no post-load allocation. Preserve the existing ownership of battery spans and user-supplied firmware; state inclusion and restore ordering must be confirmed with the architecture decision before coding. The libretro `.info` claim and its current `info_claims` test must change together. Do not promise compatibility across every future core version unless the format's explicit version window supports it.

## Minimal New Verification Surface

- Core: state byte round-trip; malformed, mismatched, and truncated states rejected with no mutation; save/load/continue frame and audio hashes equal an uninterrupted run; same state bytes across the existing platform and big-endian checks; allocator count remains zero after load.
- Runner: state read/write, corrupt/oversized input, interrupted replacement, and recorded movie replay through existing CLI tests. Reuse its current atomic save-file strategy.
- Libretro: extend `libretro.host` to test size stability, a valid save/load, wrong-length or corrupt rejection, and deterministic continuation. Extend the `.info` claim guard. Keep the existing bounded RetroArch integration and package checks; add a host run only if it covers a recurring regression the in-process test cannot catch.
- Hostile input: add `fuzz.state` over a fixed loaded instance and `fuzz.movie` over the existing bounded parser. Reuse standalone regression replay on the normal six build legs and the existing nightly libFuzzer lane. Preserve exact CTest inventories and per-target limits.

## Bounded Tune-up Outcomes

Treat SEED-007, SEED-009 and SEED-010 as three evidence-led dispositions inside the existing tune-up, not a broad pipeline audit.

1. **COST-01:** The monthly digest's raw runner-minute total is untrusted: a GitHub retry record copied the original job start/end while having a later `created_at`, duplicating elapsed time; skipped placeholders can also yield negative intervals. Repair only job-duration attribution and membership using the existing GitHub Actions records. Add representative positive and fault controls for ordinary, genuine failed/cancelled, copied retry timestamps, skipped and negative intervals. Exit when a source-linked 30-day report retains real failed/cancelled work and excludes copied or invalid intervals; do not claim savings without comparable complete cohorts. No new service or arbitrary cost quota.
2. **WR-01:** A delayed release checkout can have a first parent equal to the triggering push's `before` SHA; the current producer/join correctly refuse that as an independent base. Keep fail-closed behavior until both producer and join authenticate the selected release parent. Exercise equal-before and valid-independent-parent fixtures. Exit only when both paths agree on the authenticated base and still reject missing or ambiguous evidence.
3. **Property calibration:** The 10,000-case nightly property job has censored timeout evidence, so do not raise deadlines from those runs. First obtain one complete run of the seeded workload, preserve every property and case, then set deadlines/job limit from complete successful measurements. If complete work does not fit, reduce generated per-case image size only with the same property/oracle coverage and remeasure. Keep the job advisory until its evidence is complete; do not infer a promotion.

Tune-up exit is a passing representative fault-control set, one complete measured report/run for each changed workload, and a written defer disposition when evidence is insufficient. Compare actual critical-path and test time before/after any scheduling change; keep results report-only where the runner is shared or cohorts are incomparable. No line quotas, test-count padding, new scoring system, or owner-run UAT.

## Sources and Confidence

- **HIGH — current stack and seams:** `CMakeLists.txt`, `CMakePresets.json`, `runner/main.c`, `libretro/libretro.c`, `libretro/muhtari_libretro.info.in`, `tests/libretro/host.c`, `tests/libretro/tests.cmake`, `cmake/packaging.cmake`, and `docs/embedding.md`.
- **HIGH — project decisions and seeds:** `PROVENANCE.md` §4 (the approved MIT state visitor seed and vendored notices); `ARCHITECTURE.md` §7; `HOSTS-AND-INTEGRATION.md` §§1, 3–5; `ENGINEERING.md` §§3–4, 8–9, 11–12; SEED-002/007/009/010; v2.0 tune-up `deferred-items.md`.
- **MEDIUM — external host/build behavior:** [libretro core development docs](https://github.com/libretro/docs/blob/master/docs/development/cores/developing-cores.md), [core-info template](https://github.com/libretro/libretro-super/blob/master/dist/info/00_example_libretro.info), [RetroArch run-ahead guide](https://docs.libretro.com/guides/runahead/), and [CMake 3.25 release notes](https://cmake.org/cmake/help/v3.25/release/3.25.html), checked 2026-10-29 and cross-checked against the vendored API header and local preset.

**Revisit if:** Playstead requires a non-libretro direct runner GUI, state size or disk usage is measured as a material problem, a real consumer requests BK2 interchange, a host requires cross-version state migration, or a complete CI cohort shows the existing test/runtime architecture is the bottleneck.
