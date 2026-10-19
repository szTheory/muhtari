---
title: "Engineering: C rules, tests, CI, release, repo settings and quality loops"
summary: "Build, test, CI, release, repository, triage, performance and documentation decisions for muhtari, with mutable facts re-verified on 2026-10-15."
read_when: "Before creating or changing CMake files, presets, tests, workflows, repository settings, the release pipeline, quality gates or engineering docs."
updated: 2026-10-19
---

# Engineering: C rules, tests, CI, release, repo settings and quality loops

Mutable facts were re-read on 2026-10-15 [ENG.01–ENG.16]; sibling practice is cited by repo and commit [ENG.17–ENG.22]; (tested) marks local runs [ENG.23]. Choices read **Rec** (recommendation) — **Alt** (strongest alternative) — **Flip** (the observation that would change it).

## Where to look
| Question | Section |
|---|---|
| Runner labels and vCPUs, action SHAs, tool versions, release-please semantics, GitHub token rules | 1 |
| Flags, presets, integer-only / no-globals / no-UB checks, analyzers, coverage | 2 |
| Test tiers, exact inventory, no-skip, negative controls, property tests, fuzz targets | 3 |
| Workflows, jobs, aggregator, triggers, timeouts, caching, cores, minutes per PR | 4 |
| release-please config, App token, archives, attestations, rehearsal, withdrawal | 5 |
| Repo settings script, ruleset, branch policy; issue forms, labels, digest, Dependabot, YAGNI | 6, 7 |
| Ir/frame gate, wall-clock tails, allocations, tracing cost, `quality.json` ratchet | 8 |
| Big-endian, 32-bit and console lanes; style contract | 9, 10 |
| Docs budgets, evidence format; milestone tune-up data; local paths and secrets | 11, 12, 13 |

## 1. Re-verified facts (2026-10-15)
**Runners** (standard, public repo, free) [ENG.01][ENG.02][ENG.03]:
| OS × arch | Label | vCPU / RAM | On the image | Notes |
|---|---|---|---|---|
| Linux x64 | `ubuntu-26.04` | 4 / 16 GB | GCC 13.4, 14.3, 15.2; Clang, clang-format, clang-tidy 20.1.8, 21.1.8, 22.1.2; CMake 4.4.3; Ninja 1.13.2; jq 1.8.1 | GA 2026-09-17 (#14747); `ubuntu-latest` moves to it in Nov 2026 (#14748); 24.04 has GCC ≤ 14.2, LLVM ≤ 18.1.3, CMake 3.31.6 |
| Linux arm64 | `ubuntu-26.04-arm` | 4 / 16 GB | same | |
| macOS arm64 | `macos-15` | 3 (M1) / 7 GB | Apple clang 17.0.0; CMake 4.4.3 | `macos-latest` is now `macos-26` (Apple clang 21) |
| macOS x64 | `macos-15-intel` | 4 / 14 GB | Apple clang 17.0.0; CMake 4.4.2 | x86_64 offered "until August 2027" (#13045); `macos-26-intel` also listed |
| Windows x64 | `windows-2025` | 4 / 16 GB | VS 2026 18.10; LLVM 20.1.8; CMake 4.4.3 | `windows-latest` = `windows-2025` = `windows-2025-vs2026` |
| Windows arm64 | `windows-11-arm` | 4 / 16 GB | VS 2026 18.10; LLVM 22.1.8; CMake 4.4.4 | resolved to image `windows-11-vs2026-arm64` in nesturbator job 116523896193 [ENG.18] |

- **Rec** these six, Ubuntu 26.04 for every Linux job — **Alt** Ubuntu 24.04 (proven over nesturbator's three milestones) — **Flip** a 26.04 defect costs a red day, or 26.04-arm queue p95 > 2 min.

Apt-installable, not preinstalled (26.04 amd64; Valgrind also arm64) [ENG.13]: Valgrind 3.26.0 (24.04: 3.22.0), cppcheck 2.19.0, `qemu-user` 10.2.1, `gcc-{powerpc,powerpc64,s390x,i686}-linux-gnu` and `gcc-arm-linux-gnueabihf` 15.2, `clang-format-18` 18.1.8.

**Actions** — pin the full SHA with a `# vX.Y.Z` comment [ENG.04]:
| Action | Latest release | Commit SHA |
|---|---|---|
| actions/checkout | v7.0.1 (2026-07-20) | `3d3c42e5aac5ba805825da76410c181273ba90b1` |
| actions/upload-artifact | v7.0.2 (2026-10-07) | `cf430e030ddbb5b0abf93d22962f4752f3646cd9` |
| actions/download-artifact | v8.0.2 (2026-10-07) | `9000827ccba6bdab643e8b6fd33ac0654aef8333` |
| actions/create-github-app-token | v3.3.0 (2026-10-12; nesturbator pins v3.2.0) | `64ab729a6294000f5aa4a4bc0b73d1f32bc45510` |
| actions/attest | v4.2.2 (2026-08-04) | `1e69f48acb82d1966a394da916b4c1698aa569d6` |
| actions/attest-build-provenance | v4.2.2, now "simply a wrapper" over `actions/attest`: do not use | `4d101475d8b20a2381f78447822ac1eab6504dd8` |
| googleapis/release-please-action | v5.0.0 (2026-04-22), bundles release-please 17.6.0 (library at 17.11.2) | `45996ed1f6d02564a971a2fa1b5860e934307cf7` |
| amannn/action-semantic-pull-request | v6.1.1 (2025-08-22), alternative only | `48f256284bd46cdaab1048c3721360e808335d50` |
| dependabot/fetch-metadata | v3.1.0 (2026-04-20) | `25dd0e34f4fe68f24cc83900b1fe3fe149efef98` |

`create-github-app-token` takes `client-id` (`app-id` is deprecated) and `permission-<name>: write` inputs that narrow the token [ENG.11]; `actions/attest` with no predicate input makes SLSA build provenance and accepts `subject-checksums` [ENG.12].

**release-please 17.6.0, read from source** [ENG.05][ENG.06]:
| Option | Behaviour |
|---|---|
| `release-type: simple` | edits `version.txt` (must already exist) and `CHANGELOG.md`; the manifest releaser edits `.release-please-manifest.json` |
| `bump-minor-pre-major: true` | below 1.0.0 a breaking change bumps minor (otherwise major) |
| `bump-patch-for-minor-pre-major: false` | below 1.0.0 `feat` bumps minor (`true`, nesturbator's value, makes it patch) |
| `initial-version: "0.1.0"` | used only when no previous release exists; a manifest value `"0.0.0"` counts as none; the default is 1.0.0 |
| `draft: true`, `force-tag-creation: true` | draft release whose tag is created at once (GitHub otherwise tags a draft only at publish); worked for nesturbator v0.1.0–v0.1.10 |
| `include-v-in-tag: true` | `v0.2.0` (default) |
| `extra-files` | marker-based edits of other files: not used (section 5) |
| `Release-As: x.y.z` footer | newest such footer since the last release wins; the config key `release-as` is "[DEPRECATED]" and sticks until removed (nesturbator footgun) |

**GitHub behaviour** [ENG.07][ENG.08][ENG.09][ENG.10][ENG.22]:
- An event caused by `GITHUB_TOKEN` starts no workflow run (so a `GITHUB_TOKEN` merge starts no `push` run), except `workflow_dispatch`/`repository_dispatch`, and PR `opened`/`synchronize`/`reopened`, whose runs wait in an approval-required state. An App token avoids both: nesturbator's App merge of release PR #35 started release run 38432194730 [ENG.18].
- Dependabot-triggered `pull_request` and `push` runs get a read-only `GITHUB_TOKEN` and only **Dependabot** secrets; whether `vars` resolve there is (unverified).
- Immutable releases lock tag and assets at publish; a deleted immutable release's tag name can never be reused; notes, title and "latest" stay editable (`PUT /repos/{o}/{r}/immutable-releases`).
- A head commit message containing `[skip ci]`, `[ci skip]`, `[no ci]`, `[skip actions]`, `[actions skip]` or `skip-checks:true` starts no `push`/`pull_request` run. GSD 1.17.0 `/gsd-ship` titles the PR `Phase N: Name`, pushes a `[ci skip]` ship note to the PR branch and, if the PR is then BLOCKED with zero checks, an empty recovery commit.

## 2. C rules, flags and presets
C17 with `CMAKE_C_EXTENSIONS OFF`; `cmake_minimum_required(VERSION 3.25)` for embedders; `CMakePresets.json` version 11 (CMake ≥ 4.3, on every chosen image) so test presets can say `"jobs": ""` [ENG.16]; one entrypoint `cmake --workflow --preset <lane>` locally and in CI [ENG.17].
| Rule | Check |
|---|---|
| Unsigned fixed-width guest arithmetic, cast to `uint32_t` before `<<` or `*`; no signed shifts, VLAs, recursion, bit-fields, packed structs, punning unions, FP | warnings; UBSan in `asan`; property tests |
| Integer-only core | `hygiene.no_float` scans `src/` and `include/`; preset `nofp` builds with `-mgeneral-regs-only` (GCC, x64 + arm64); `abi.undefined` runs `nm -u` against an allowlist (`memcpy memmove memset memcmp` plus per-target helpers, e.g. `__udivdi3` on ppc32); a float fixture must fail `hygiene.no_float` and `nofp`; `abi.undefined`'s negative control is a libc fixture (`tests/abi/libc_fixture.c`, a call to `abort`), because hardware floating point leaves no undefined symbol for `nm -u` to catch |
| No mutable globals or statics | `abi.globals`: no `B b D d C` symbol in the Release static library; `llvm-nm` (on the Windows images) reads COFF. File-scope `const` tables hold integers only: under PIC a `const` pointer table (e.g. a board ops table) lands in `.data.rel.ro`, which `nm` reports as `d` (nesturbator `tests/cmake/global_symbols.cmake` [ENG.17]), so dispatch uses `switch` or pointers stored in the instance at load |
| One symbol namespace | `abi.symbols`: each defined external symbol is `muhtari_*` declared in `include/muhtari.h`, or internal `muhtari__*` |
| Header is C and C++ | `abi.header.c17` and `abi.header.cxx17` (`-Wall -Wextra -Wpedantic -Werror`; MSVC `/TC`, `/TP`) plus `install.consumer` and `embed.subdirectory` |
| No host paths in binaries | `-ffile-prefix-map=<source dir>=.`; `release.no_build_paths` scans each archive |

Warnings (GCC, Clang): `-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wstrict-prototypes -Wmissing-prototypes -Wvla` (nesturbator) plus `-Wundef -Wcast-qual -Wimplicit-fallthrough -Wmissing-variable-declarations` [ENG.24]; GCC adds `-Wduplicated-cond -Wlogical-op`; MSVC `/W4 /utf-8`. Applied only when `PROJECT_IS_TOP_LEVEL`, fatal only through presets (`CMAKE_COMPILE_WARNING_AS_ERROR`), so an embedder's older or console compiler never meets an unknown flag. Measured on nesturbator `src/` (19 files, 7,567 lines, Apple clang 21): its set gives 0 warnings; of 17 candidate flags only `-Wcast-qual` (3, all deliberate casts in its state visitor), `-Wswitch-default` (1), `-Wdeclaration-after-statement` (73) and `-Wpadded` (344) fire (tested) [ENG.23]. `-Wundef` guards the `#if` of the traced loop; the two noisy style flags stay out.

| Preset | Compiler and flags | Where | Tests |
|---|---|---|---|
| `dev` | Debug, host compiler | local | inventory `ci` |
| `ci`, `ci-msvc` | Release, warnings fatal; MSVC environment from vswhere + vcvarsall (no third-party action) | six legs | inventory `ci`, then package |
| `asan` | clang-21 `-O1 -g -fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer`; `MUHTARI_SHORT_RUNS` shortens long runs, never omits a test; TIMEOUT × 3 | ubuntu x64 | inventory `ci` |
| `nofp` | GCC 15 + `-mgeneral-regs-only` | ubuntu x64, arm64 | label `abi` |
| `hygiene` | clang-format-21 (configure fails on another major), clang-tidy-21, jq, git, sh | ubuntu x64 | label `hygiene` |
| `be32` | `powerpc-linux-gnu-gcc` 15 + qemu-ppc as `CMAKE_CROSSCOMPILING_EMULATOR` | ubuntu x64 | label `portable` (section 9) |
| `fuzz`, `coverage`, `vectors-full`, `oracle-*`, `bench` | clang-21 libFuzzer + ASan/UBSan; clang-21 source coverage; Release | nightly | sections 3, 8 |
| `library` | Release; private paths from `.env.local` | owner's Mac | commercial dumps, real BIOS |

- **Analyzers. Rec** `clang-tidy-21 -checks='-*,clang-analyzer-*'` over `src/` in `hygiene`, zero findings required: on nesturbator `src/` that set found 0 and the whole run, with `bugprone-*` added, took 10 s and gave 2 findings, both false on inspection (tested); GCC `-fanalyzer` and cppcheck run only at the tune-up — **Alt** no analyzer (warnings, sanitizers and fuzzing cover these bug classes) — **Flip** three `NOLINT`s needed → tune-up only; a tune-up run finds a real bug → that tool runs per PR.
- **clang-format. Rec** 21: 21.1.8 is preinstalled on ubuntu-26.04 and equals Homebrew `llvm@21` 21.1.8 (one `brew install llvm@21`); nesturbator's `.clang-format` (LLVM base, indent 4, 100 columns, Linux braces) — **Alt** 18 (owner already has `llvm@18` 18.1.8; preinstalled 18.1.3 on 24.04) — **Flip** Linux jobs stay on 24.04.
- **Sanitizers. Rec** one ASan + UBSan lane; no MSan/TSan (no threads; allocations zero-filled; hash equality exposes uninitialised reads) — **Alt** MSan nightly — **Flip** a hash mismatch traces to uninitialised memory.
- **Coverage. Rec** nightly llvm-cov on Linux x64: line % reported only; function coverage of `src/` held at 100 % (an uncalled function is dead code) — **Alt** a per-PR line-coverage ratchet (invites test-gaming [ENG.20]) — **Flip** a shipped regression sat in lines reported uncovered twice.

## 3. Tests
| Tier | Trigger | Content |
|---|---|---|
| PR, required | every PR; heavy jobs skip on `.planning/**`-only changes | units, committed SingleStepTests `6502` sample, diagnostics, fixture frame/audio/state hashes, `fuzz.regress.*`, `policy.*`, `hygiene.*`, `abi.*`, `install.consumer`, `embed.subdirectory`, `libretro.host` |
| Nightly | schedule, `workflow_dispatch`, PRs touching the nightly's inputs | full vectors fetched at their pin, libFuzzer, `suite-flake` (`--repeat until-fail:3 --schedule-random`), coverage, bench, `armhf`, oracles; red → one rolling `nightly` issue |
| Local | owner | `library` preset (dumps), real BIOS, one RetroArch session per milestone |

- **Exact inventory** (gabbaboy `cmake/ExpectedTests.cmake` [ENG.19]): `tests/inventory/<preset>.txt` holds sorted names; configure fails unless the directory `TESTS` property equals it. Subsystem files are pulled in with `include()`, never `add_subdirectory()`, so one property holds every name. One `ci.txt` serves all six legs: a test that cannot run everywhere belongs to another preset.
- After the test step `tests/cmake/check_junit.cmake` requires each listed name exactly once, with no `failure`, `error` or `skipped`; a regex plus `--no-tests=error` does not prove each test ran [ENG.19].
- **No self-skip:** `policy.no_skip` rejects `SKIP_RETURN_CODE`, `SKIP_REGULAR_EXPRESSION` and `DISABLED`; a missing tool fails configure through `find_program(... REQUIRED)`.
- **Negative controls:** every gate test `x` has `x.self_test` run through `tests/cmake/expect_fail.cmake -DCODE=<n> -DREGEX=<rule>: <file>`, which passes only on that exit code and that message; bare `WILL_FAIL` is banned because any failure satisfies it.
- **Registration:** `muhtari_add_test(NAME … LABELS … TIMEOUT … COST …)` in `cmake/MuhtariTests.cmake` multiplies TIMEOUT by `MUHTARI_TIMEOUT_SCALE` (3 under sanitizers, the measured factor under qemu); one file per subsystem, `tests/{cpu,bus,maria,tia,riot,pokey,cart,state,runner,libretro,policy,fuzz,perf,release}/tests.cmake`, each ≤ 400 lines (nesturbator's single file reached 100 KB, gabbaboy's 4,448 lines). Names are `<subsystem>.<behaviour>`, never phase, plan or requirement IDs.
- **Property tests. Rec** in-repo `tests/prop.h` (~60 lines): splitmix64 seeded per test; a failure prints seed, case and a one-line repro; 100 cases per PR, 10,000 nightly with a random printed seed; size-halving shrink. Properties: `run(N)` then `run(M)` equals `run(N+M)`; save → load → continue equals uninterrupted; state round trip is byte-identical; a rejected load or state leaves the instance unchanged; `probe` is pure; two instances never interact; input applies only at the next VBLANK start — **Alt** `theft` v0.4.5 (ISC, last release 2019, `fork`/`waitpid` in `src/theft_run.c`, so no MSVC legs) [ENG.15] — **Flip** hand-minimising a property failure takes > 30 min twice.
- **Fuzz corpus. Rec** seeds generated at build time by `tests/fuzz/seeds.c`; only minimised regressions committed (manifest rows); `tests/fuzz/**` ≤ 512 KiB, checked by hygiene — **Alt** commit evolved corpora (nesturbator's `tests/fuzz` is 9.0 MB) — **Flip** nightly coverage plateaus without an evolved corpus → keep it as a nightly artifact, still uncommitted.

Fuzz targets, each an `LLVMFuzzerTestOneInput` plus a standalone replay `main` registered as `fuzz.regress.<target>` on all six legs (Apple clang has no libFuzzer [ENG.17]); nightly 60 s per target, clang-21 + ASan/UBSan, `-timeout=2 -max_len=1048705` (`MUHTARI_IMAGE_SIZE_MAX`, 1 MiB + the 128-byte header, plus one, so no seed stops short of SuperGame geometry or the oversize check); crashes uploaded and minimised into `tests/fuzz/<target>/regress/`:
| Target | Entry | Arrives with |
|---|---|---|
| `loader` (`tests/fuzz/loader.c`; replay `fuzz.regress.loader`) | `muhtari_probe` + the internal load (A78 v3/v4, headerless, 2600 images) | phase 2 (landed) |
| `run` | random bytes as a 48 KiB linear cart, fast boot, 2 frames: no crash, UB or timeout | cartridge loader |
| `movie`, `state`, `patch` | runner movie parser; `muhtari_load_state` after a fixed load (validate before apply); a patch parser if one is ever added | movies; save states; that feature |

## 4. CI design
Workflows: `ci.yml` (required), `nightly.yml`, `release.yml`, `digest.yml` (weekly, section 7), `dependabot-automerge.yml`.
| Job (`ci.yml`) | Runner | Content | Skipped when |
|---|---|---|---|
| `changes` | ubuntu x64 | `heavy=false` only if every changed path, old and new name, is under `.planning/`; empty or unknown → heavy; `ci.classify` CTest covers mixed, renamed and empty lists | never |
| `hygiene`, `title` | ubuntu x64 | preset `hygiene`; live PR title and body via `gh pr view` (below) | `title`: not a PR |
| `build` × 6 | section 1 | `ci`/`ci-msvc`; archive check; `hashes.txt`; README install line on macOS arm64 | light |
| `asan`, `nofp` × 2, `be32` | ubuntu | presets of the same names | light |
| `hash-equality` | ubuntu x64 | seven byte-identical `hashes.txt` (six legs + `be32`) | light |
| `perf`, `quality` | ubuntu x64 | section 8 | light; `perf` also when not a PR |
| `CI required` | ubuntu x64 | `if: always()`; jq over `toJSON(needs)`: every result `success`, except heavy jobs `skipped` when `heavy=false` and `perf` `skipped` outside PRs; a CTest runs the expression on synthetic `needs` | never |

- **Aggregator. Rec** one required check, `CI required` (GitHub reports a skipped job as success and skips jobs whose dependency failed) — **Alt** each job required (exifcleaner-node needed 19 contexts and no-op legs [ENG.17]) — **Flip** the aggregator lets one real failure through (then also require the six `build` checks directly).
- **Matrix. Rec** all six legs on every PR, because identical output on six platforms is a product claim — **Alt** three legs per PR, the rest on `main` — **Flip** measured queueing dominates the critical path.
- **`.planning` skip. Rec** job-level skip via `changes`, the aggregator still reporting; no trigger-level `paths` (a skipped workflow leaves the required check pending) [ENG.17] — **Alt** run everything (nesturbator's `.planning`-only PR #47 used 15 jobs and 3,089 job-seconds, run 38415960004 [ENG.18]) — **Flip** the classifier ever lets a code change through (its CTest must catch it first).
- **Title. Rec** a regex job inside `ci.yml`: `^(feat|fix|perf|docs|test|build|ci|chore|refactor|style|revert)(\([a-z][a-z0-9._-]*\))?!?: [a-z]`, so GSD plan scopes such as `feat(02-03):` (which would inflate versions) and `Phase N: Name` titles fail (gabbaboy PR #10 `Phase 6: …` reached `main` that way [ENG.19]); a second pattern rejects `feat`, `fix` and `perf` scoped `ci`, `release`, `build`, `test` or `docs`, so pipeline work cannot cut a release (lattice_stripe cut ten patch releases, v1.7.3–v1.7.12, on the day it fixed its pipeline with `fix(release):` commits [ENG.25]); the live body must hold no skip token, because the squash body becomes `main`'s commit message — **Alt** `amannn/action-semantic-pull-request` in its own workflow (lattice_stripe, `pull_request_target`; not a required check there, so `feat(65)!:` and `docs(78-06):` titles reached `main` [ENG.21][ENG.25]) — **Flip** title rules outgrow one regex.
- **Triggers:** `pull_request` (opened, synchronize, reopened, edited), `push` to `main` only, `workflow_dispatch`, `workflow_call`; gabbaboy's push on every branch plus PR runs everything twice (its `ci.yml`) [ENG.19]. **Concurrency** `ci-${{ github.workflow }}-${{ github.event.pull_request.number || github.sha }}`, `cancel-in-progress` only for PRs (the workflow name keeps a call from `release.yml`, which sees the caller's context, from queueing behind the push run on the same commit); `release.yml` has its own group with `queue: max` (untested) and no `cancel-in-progress`: by default a newly queued run cancels the pending one even with `cancel-in-progress: false` [ENG.27], which is how three lattice_stripe release runs ended cancelled with no jobs (30474364540, 30474374555, 30474391526 [ENG.25]).
- **Permissions:** workflow `contents: read`; `title` adds `pull-requests: read`; every checkout `persist-credentials: false`. **Pins:** full SHA everywhere; repo setting `sha_pinning_required: true`, so GitHub refuses an unpinned action, and `scripts/apply-repo-settings.sh --check` watches that setting, so `hygiene.action_pins` is not built (nesturbator's live value is `false` although its doc says it is on) [ENG.18].
- **Timeouts:** twice the slowest successful run of that job among the last 40, rounded up to minutes, with run ID and sample count in a comment; a 5-minute floor for network-bound and roll-up jobs; a new job says `provisional` until 10 samples exist and is fixed at phase close; `hygiene.timeout_comments` enforces it [ENG.17].
- **Cores:** Ninja's default parallelism; test presets `"jobs": ""` = max(nproc, 2) [ENG.16]; long tests carry `COST`; run local `ctest` in the foreground (background shells on the Mac are throttled).
- **Caching. Rec** none; fetched suites never cached, so the fetch stays tested — **Alt** ccache keyed by preset + runner label + compiler version — **Flip** the tune-up measures compile share ≥ 40 % of a critical-path leg (nesturbator measured 4–22 % and rejected ccache [ENG.17]).
- **Minutes per PR** (derived from nesturbator [ENG.18]): phase-1 scale ≈ 2 min wall, 13 jobs of 2–53 s each (run 37084642541); v1-close scale ≈ 23 min wall, critical path `macos-15-intel` 1,315 s + 61 s queue, 5,588 job-seconds ≈ 93 runner-minutes per run (run 38676328559); 2–3 runs per phase PR ≈ 190–280 runner-minutes; a `.planning`-only PR ≈ 1 min. Target: critical path ≤ 12 min at v1 close; first lever: the Intel leg runs label `portable` only.

## 5. Release
```json
{"packages": {".": {"release-type": "simple", "initial-version": "0.1.0", "bump-minor-pre-major": true,
  "bump-patch-for-minor-pre-major": false, "draft": true, "force-tag-creation": true, "include-v-in-tag": true}}}
```
Manifest `{".": "0.0.0"}` and `version.txt` `0.0.0` until the first release, which is then 0.1.0 whatever the commit types [ENG.05].
1. A squash merge to `main` runs `release.yml`: it mints an App token (`client-id: ${{ vars.MUHTARI_APP_CLIENT_ID }}`, `private-key: ${{ secrets.MUHTARI_APP_PRIVATE_KEY }}`, `permission-contents`, `-pull-requests`, `-issues: write`), runs release-please and enables `gh pr merge --auto --squash` on the release PR.
2. The App-made release PR runs CI without approval; GitHub merges it as the App when `CI required` passes, and that push starts `release.yml` again (nesturbator run 38432194730).
3. On `release_created`, `uses: ./.github/workflows/ci.yml` runs CI with release-please's `sha` output as its checkout ref, never `github.sha`: when the merge's own run is dropped or fails early, a later push's run creates the release while its `github.sha` is newer (lattice_stripe; exifcleaner-node `a88f57b73636` [ENG.25][ENG.26]); `publish` needs it and fails unless every archive records the tag's commit.
4. `publish`: exactly 18 zips `muhtari-{library,runner,libretro}-{linux,macos,windows}-{x64,arm64}.zip` → `SHA256SUMS` → `actions/attest` with `subject-checksums` → upload to the draft → `gh attestation verify` each zip → a copy with its last byte flipped must fail → publish → `gh release verify vX` → re-read until 19 non-empty assets [ENG.17] → download each zip as a user would from `releases/latest/download/`, check it against `SHA256SUMS` and run `gh attestation verify --signer-workflow <repo>/.github/workflows/release.yml` (lattice_stripe `260edf43189e`, exifcleaner-node `a44704fd528d` [ENG.25][ENG.26]).
- **Re-runs:** `publish` is safe to re-run: uploads overwrite the draft's assets, the draft is found by release ID because `GET releases/tags/{tag}` omits drafts, and a version already published with 19 verified assets exits 0. Recovery is "Re-run failed jobs": a full re-run sees the PR already `autorelease: tagged` and skips `publish` (lattice_stripe `e7d6c0464437`; exifcleaner-electron `78b895773742`, `0266e1c7c4d0` [ENG.25][ENG.26]).
- **Pre-1.0 policy:** `feat` → minor, breaking → minor, `fix`/`perf` → patch; `docs chore ci test build refactor style` cut no release (write `ci:`, not `fix(ci):`); 1.0.0 only through a `Release-As: 1.0.0` final line in the PR body.
- **Version source. Rec** release-please edits only `version.txt`, the manifest and `CHANGELOG.md`; CMake `configure_file` generates the private version header, `muhtari_libretro.info` (`display_version`) and the runner's `--version`; no `extra-files`; `release.single_version_source` fails when the current version string appears in any other tracked file; asset names carry no version, so README links `releases/latest/download/<name>.zip` — **Alt** nesturbator's `extra-files` markers (aligned `x-release-please` comments broke `hygiene.format` when 9 became 10 and blocked releases for three days [ENG.17]) — **Flip** a consumer needs the version in the file name (then add a versioned copy, never a second edited string).
- **Archives:** `LICENSE` and `THIRD-PARTY-NOTICES.md` in every zip (BSD-3 notices must travel with binaries); no top-level folder; the libretro zip holds `cores/` and `info/`; `release.archive_contents` checks names, members and build paths on every leg.
- **Rehearsal** of every publish step before the first real tag, on each same-repository pull request that changes `release.yml`, `scripts/publish.sh`, `release-please-config.json` or `cmake/packaging.cmake`: CI on the head commit, then `publish.sh` against a published prerelease `rehearsal-<run_id>-<run_attempt>` with `make_latest=false` (prepare, upload by release ID, attest, tampered-copy check, publish, `gh release verify`, download and verify, the README install line), and a `cleanup` job that deletes the release and its tag whatever failed. `workflow_dispatch` fires only for workflow files on the default branch, and an untagged draft cannot be published or verified, so neither can rehearse before the first merge. Fallback, if `GITHUB_TOKEN` on `pull_request` ever loses `contents`, `id-token` or `attestations: write`: the draft-only form (prepare, upload, attest, attestation checks, delete). Green run https://github.com/szTheory/muhtari/actions/runs/38942043999 (gabbaboy merged 18 release-fix PRs, #10–#29, in about two hours on its first tag, with a 1,101-line `release.yml` [ENG.19]).
- **Phase close:** no open `autorelease: pending` PR; `releases/latest` is the expected version with 19 assets (nesturbator's release PR #35 stayed red across a milestone).
- **Withdrawal** (`docs/maintaining.md`): `gh release edit vGOOD --latest`; edit the bad notes to "Withdrawn: reason; use vX"; fix forward with a `fix:` PR; delete only content that must not be distributed (its tag name is then gone for good).
- **Bot credential. Rec** GitHub App token, minted only on `push` to `main` and in the Dependabot job — **Alt** a fine-grained PAT (lattice_stripe) — **Flip** the owner declines to create an App.
- **Release-PR merge. Rec** native auto-merge (nesturbator) — **Alt** lattice_stripe's 239-line `workflow_run` merger that re-checks the gate on the exact head [ENG.21] — **Flip** auto-merge ever merges a head whose `CI required` is not green.

## 6. Repository settings as code and branch policy
`scripts/apply-repo-settings.sh` (POSIX sh + `gh api`, idempotent; `--check` prints drift; the owner runs it after creating the repo and at each tune-up) [ENG.10]:
| Setting | Call |
|---|---|
| squash only; commit title `PR_TITLE`, message `PR_BODY`; auto-merge; delete branch on merge; no wiki/projects | `PATCH /repos/{o}/{r}`: `allow_squash_merge=true allow_merge_commit=false allow_rebase_merge=false squash_merge_commit_title=PR_TITLE squash_merge_commit_message=PR_BODY allow_auto_merge=true delete_branch_on_merge=true has_wiki=false has_projects=false` |
| secret scanning + push protection | same `PATCH`, `security_and_analysis.{secret_scanning,secret_scanning_push_protection}.status=enabled` |
| private vulnerability reporting; Dependabot alerts + security updates | `PUT …/private-vulnerability-reporting`, `PUT …/vulnerability-alerts`, `PUT …/automated-security-fixes` |
| SHA-pinned actions; read-only default token; Actions cannot approve PRs | `PUT …/actions/permissions` `sha_pinning_required=true`; `PUT …/actions/permissions/workflow` `default_workflow_permissions=read can_approve_pull_request_reviews=false` |
| immutable releases; the eight labels; the ruleset | `PUT …/immutable-releases`; `gh label create --force`; `POST`/`PUT …/rulesets` from `.github/rulesets/main.json` |

`PR_BODY` matters because, with `COMMIT_MESSAGES`, GSD's `[ci skip]` ship note would land in `main`'s squash commit and suppress its `push` runs, release included. nesturbator's live repo already uses `PR_TITLE`/`PR_BODY`, squash only, auto-merge, delete-on-merge, push protection and immutable releases [ENG.18]. The ruleset copies nesturbator's: `~DEFAULT_BRANCH`; `deletion`, `non_fast_forward`, `required_linear_history`; `pull_request` with 0 approvals and `allowed_merge_methods: ["squash"]`; required check `CI required` with `integration_id` 15368 (GitHub Actions); no bypass actors [ENG.17]. Apply it in phase 1 once a run has reported `CI required` (earlier, it blocks the PR that adds CI).
- **Rec** script + committed ruleset, drift checked by `--check` at the tune-up — **Alt** a scheduled drift workflow (nesturbator planned one and never built it) — **Flip** a setting drifts unnoticed between two tune-ups.
- `--check` compares every setting the script applies and reports all drift before exiting 1. Today it reads only the merge settings, secret scanning and the default token permission, and stops at the first merge-setting drift; phase 1 (FRAME-08) adds `sha_pinning_required`, `can_approve_pull_request_reviews`, immutable releases, the vulnerability settings, the labels and the ruleset (lattice_stripe's live `allow_auto_merge: false`, which no check covered, made every Dependabot auto-merge run fail [ENG.25]).

Branch policy: `.githooks/pre-commit` runs `hygiene.sh --staged` and refuses a commit on `main` once `origin` exists; `.githooks/pre-push` runs `hygiene.sh --history`; one branch and one squash-merged PR per phase; right after `/gsd-ship` creates the PR the agent retitles it (`gh pr edit N --title "feat: …"`), so the `edited` run supersedes the first while it is young.

## 7. Triage automation (minimal)
- **Issue forms** (`.github/ISSUE_TEMPLATE/`, lattice_stripe's `bug_report.yml` as the template [ENG.21]): `bug.yml` (muhtari version, OS/arch, frontend + version, steps, expected/actual, movie file if any); `compatibility.yml` with required ROM SHA-1 (headerless and headered, as `muhtari-run --probe` prints them), title, region (NTSC/PAL), frontend + version, muhtari version, symptom checkboxes (boot, graphics, sound, input, save, speed, crash); `feature.yml`; `config.yml` turns off blank issues and links private vulnerability reporting. Every form says: never attach ROM or BIOS files.
- **Labels (8):** `bug`, `compatibility`, `enhancement`, `docs`, `performance`, `needs-info`, `nightly`, `digest` (plus Dependabot's `dependencies` and release-please's `autorelease: *`).
- **`digest.yml`**, Mondays 06:00 UTC, `issues: write`, `actions: read`, `pull-requests: read`, sh + gh + jq: rewrites the body of the one open `digest` issue (pinned once by the owner) with unlabelled issues, PRs idle > 7 days, the open `nightly` issue and its age, Dependabot PRs > 7 days, a release PR open > 24 h or red, and runner-image deprecations; on the first Monday of a month it adds the CI-efficiency section (section 12). AGENTS.md: read it at the start of every `/gsd-discuss-phase`. **Rec** this one issue — **Alt** rely on GitHub notifications — **Flip** the agent skips the digest for two phases running (then fold it into the discuss workflow's checklist).
- **Rolling nightly issue:** opened or updated on failure, closed on success (nesturbator `nightly.yml`) [ENG.17]. Every scheduled reporter (this issue, `digest.yml`) decides from `needs.*.result` or a step's `outcome`, never a piped `$?`: GitHub's default `bash -e` has no `pipefail` [ENG.27], so lattice_stripe's drift job read `tee`'s status and stayed green while its report listed 109 actionable additions (run 38228990990 [ENG.25]). A `workflow_dispatch` input that forces a failure is run once in phase 1 to show the issue being written.
- **Dependabot:** `github-actions` only, weekly, one group for minor and patch updates (`update-types: [minor, patch]`) with majors as their own PRs, so one major never holds the group back from auto-merge (all five of lattice_stripe's Actions updates were majors [ENG.25]); `commit-message.prefix: ci` (no release). `dependabot-automerge.yml` (~20 lines): `pull_request`, `if: github.event.pull_request.user.login == 'dependabot[bot]'`, `fetch-metadata`, App token with `permission-contents`, `-pull-requests` and `-workflows: write` (merging edits under `.github/workflows/` probably needs the last; unverified), then `gh pr merge --auto --squash` for patch and minor updates; majors wait for the digest. Its private key must also be stored as a Dependabot secret [ENG.08].
- **YAGNI:** stale bot; CLA/DCO bots; path auto-labellers; CodeQL on every PR (no I/O surface; sanitizers and fuzzing find more per minute); Codecov or any third-party token; merge queue; Renovate; per-commit commitlint (squash makes it moot); release-drafter; AI triage bot; PR template; CODEOWNERS; Discussions; a Pages site; SBOM files (no dependencies; THIRD-PARTY-NOTICES covers notices); code signing and notarisation (ad-hoc signature until a user is blocked by Gatekeeper).

## 8. Performance engineering and the ratchet
| Measure | Where | Rule |
|---|---|---|
| Cachegrind Ir/frame = (Ir at 360 frames − Ir at 60) / 300 per ROM (`--cache-sim=no`, `env -i`, equal-length paths); head = the PR merge commit, base = its first parent measured twice (unequal → UNSTABLE) | `perf`, ubuntu x64, GCC 15, ≤ 4 ROMs (test card first, then one diagnostic per subsystem) | report-only until two `workflow_dispatch` control runs (one commit measured three times) show zero spread; then fail above +1.00 %; an exception is a dated line `<rom> <max % × 100> <expiry ≤ 60 days> <reason>` in `tests/perf/allow.txt`, removed after the merge [ENG.17] |
| Tracing costs nothing when off | `perf` | `muhtari-run` with the dual-compiled loop and no hooks against a build with `MUHTARI_TRACE_LOOP=OFF`: Ir/frame may differ by ≤ 32 (one hook test per run call, derived); this catches inlining lost to the second copy. Ir cannot see i-cache footprint: the size delta goes to `quality.json`, and the nightly bench compares both builds |
| Zero allocations after load | `core.no_alloc_after_load`, six legs | counting allocator through the config hook; after `muhtari_load`, 600 frames plus a state save and load into caller buffers make 0 calls |
| Nearest-rank p50, p90, p99, p99.9, p99.99, p99.999 and max µs per frame; fps; max RSS; deadline misses and the longest miss streak per budget profile | nightly `bench`, ubuntu x64 + arm64 + macos-15, ≥ 300,000 headless frames per fixture | trend data, never a gate; `tests/perf/budgets.txt`: `ntsc 16678` µs (656711/39375000 s), `pal 20032` µs (142102/7093788 s) (derived), `lowpower 1668` µs = NTSC ÷ 10 (hypothesis: a target 10× slower than the x64 runner, to calibrate on a device in v6) |

- **Rec** Ir/frame is the only per-PR speed gate; wall-clock tails are nightly trend data — **Alt** a wall-clock PR gate — **Flip** a dedicated quiet benchmark host appears (then a p50/p99 trend gate there, still never p99.999). Why never on shared runners: a zero-miss 95 % bound at p = 10⁻⁵ needs n ≥ ln 0.05 / ln(1 − 10⁻⁵) = 299,572 independent frames, 83.3 min of NTSC play (derived; glueyneo's calculation [ENG.20]); frames are correlated; shared VMs add steal time and clock changes, so tails measure the host. Ir is deterministic: precedent from the NES sibling, 23.3–24.7 M Ir/frame at +0.00 % head against base (job 115428512765) [ENG.18].

**Ratchet contract:** one committed `quality.json` holding floors (`tests.<preset>`, `compat.public`, `fuzz.targets`, `coverage.functions_pct`), ceilings (`warnings`, `analyzer_findings`, `fuzz.open_crashes`, `ir_per_frame.<rom>`, `size.<zip>`, `ci.critical_path_s`) and `reasons[]` (`key`, `from`, `to`, `why`, `date`). One CI check, job `quality` (needs build, perf and hash artifacts), runs `cmake -P tests/cmake/quality.cmake` with CMake's `string(JSON)`, no Python: a floor may not fall and a ceiling may not rise unless the same PR loosens the bound and adds a `reasons[]` entry (diffed against the merge base); tightening is free and is done at each tune-up. Sanitizer and fuzz status enter as `asan`/`fuzz.regress` results; `compat.local` (owner dumps) is recorded with its commit by the `library` preset and checked at the tune-up, not in CI.
- **Rec** one file, one check — **Alt** per-metric scoreboards (nesturbator's CONFORMANCE scoreboard, perf allow file, timeout comments) — **Flip** one metric's updates dominate PR churn (then give it its own file under the same check).

## 9. Portability lanes
| Lane | Mechanism | Tier | Reason |
|---|---|---|---|
| `be32` | `gcc-powerpc-linux-gnu` 15.2 + `qemu-user` 10.2.1 (`qemu-ppc -L /usr/powerpc-linux-gnu`) via `CMAKE_CROSSCOMPILING_EMULATOR`; scripts that start the runner receive the same emulator | per PR from phase 1: label `portable` (core units, A78 parse, state round trip, hash list) and its `hashes.txt` into `hash-equality`; full `ci` nightly | one lane covers big-endian, ILP32 and unsigned `char` (the Wii's data model); endian bugs live in serialisation, cheapest to stop from the first byte-wise helper |
| s390x, ppc64 BE | same packages | not used | 64-bit BE adds nothing for this core beyond `be32` (derived) |
| `armhf` | `gcc-arm-linux-gnueabihf` + `qemu-arm` | nightly | 32-bit ARM handheld and Raspberry Pi userlands |
| i686, Win32 | `-m32` / `vcvarsall x86` | not used | covered by `be32` and `armhf` |
| Console compile-only | containers `devkitpro/devkitppc:20260503`, `devkitpro/devkitarm:20260610`, `devkitpro/devkita64:20260219`, `pspdev/pspdev:v20261001`, pinned by digest [ENG.14] | nightly, from v6 | the core library with each SDK's toolchain file; no tests |

- **Rec** `be32` per PR as above — **Alt** big-endian nightly only — **Flip** the per-PR job exceeds 5 min wall once measured in phase 1 (apt time and qemu slowdown are untested): then full nightly, compile-only per PR. If Intel macOS runners end in August 2027, build a universal macOS binary on arm64 and test its x86_64 slice under Rosetta (availability on runners unverified).

## 10. Style contract (joy-to-read C)
| Topic | Rule |
|---|---|
| Names | public `muhtari_<noun>_<verb>()`, types `muhtari_<noun>`, macros and enum constants `MUHTARI_`; cross-file internals `muhtari__<module>_<verb>`, module = its file (cpu, bus, maria, tia, riot, pokey, ports, cart, boot, state, synth); file-local `static` names unprefixed; units in names (`ticks`, `cycles`, `lines`, `_bytes`, `_num`/`_den`); hardware registers keep Software Guide names (`DPPH`, `CTRL`, `MSTAT`, `INPTCTRL`, `SWCHA`) |
| Size | one module per chip; a file stays under ~800 lines (generated tables exempt); a function fits one screen (~60 lines) unless it is a dispatch switch of one-line cases; anything longer needs a reason in review |
| Comments | say what the hardware does and why; every hardware fact cites its primary source and the `docs/hardware/<chip>.md` section, e.g. `/* DMA starts 28 ticks into the line (Software Guide App. 3; docs/hardware/maria.md, DMA timing). */`; no planning IDs (D-NN, `SYS.07`-style IDs, phase, plan or requirement numbers) in code, tests or user docs; no other emulator's identifiers |
| Ladder | ponytail.dev: remove the need → reuse → standard library → platform → minimum new code; reused permissive code keeps its licence header and gets a THIRD-PARTY-NOTICES row |
| Errors | every fallible public call returns `muhtari_status`; an error changes no state (validate, then apply); untrusted bytes take checked paths, `assert` only states programmer invariants; the core never aborts, exits, prints, allocates after load, or reads files, clocks or the environment |
| Macros | constants and include guards only, plus the single dual-compiled traced loop (`run_loop.inc` included twice under `MUHTARI_TRACE` 0 and 1, chosen once per run call); no X-macros (use `const` tables), no function-like macros (use `static inline`) |
| Format | clang-format 21 and the committed `.clang-format`; no hand-aligned columns |
| Tests | a test name states the behaviour (`maria.dma.hole_costs_3_ticks`); each failure message prints expected and actual values |

- **Rec** enforce names, macros and format by `abi.symbols`, hygiene and clang-format; size and comments by review — **Alt** clang-tidy `readability-function-size` and naming checks — **Flip** size or naming comes up in two consecutive reviews.

## 11. Docs standards
| Item | Standard | Check |
|---|---|---|
| `README.md` | for users, order adapted from lattice_stripe (status → install one-liners from `releases/latest/download/` → RetroArch and runner quick starts → compatibility summary → docs ladder → licence); ≤ 12 KB (nesturbator's reached 114,916 bytes with its CI manual inside) | `docs.budgets`; README install line run against the built zip on macOS arm64 |
| `docs/` | `embedding.md`, `runner.md`, `testing.md`, `maintaining.md` (App and secrets setup, repo settings, release, rehearsal, withdrawal runbook, tune-up checklist), `hardware/<chip>.md` graduated from `.planning/preparation/A7800-*.md` when that subsystem lands (the prep file then shrinks to a pointer), `compatibility.md` generated from `tests/compat/ledger.csv`; each ≤ 40 KB | `docs.budgets`; `docs.compatibility_fresh` regenerates and diffs |
| Public header | terse: one line per function and field, no hardware prose; ≤ 24 KB (nesturbator's grew from 9 to 36 KB) | `docs.budgets` |
| Evidence | command + exit status + run URL in SUMMARY and VERIFICATION files, never pasted logs | review |
| `.planning` | ≤ 3 MB per milestone, archived at close (today: nesturbator 6.4 MB, gabbaboy 9.5 MB, glueyneo 9.7 MB) | digest reports size (soft) |
| Gate code | policy and hygiene scripts ≤ 10 % of `src/` lines; CMake `-P` or POSIX sh only; AGENTS.md short, each rule backed by a failing check | tune-up |

- **Rec** byte budgets as CTests (README, header, docs) — **Alt** review-only limits — **Flip** a budget blocks a correct change twice (raise it with a reason in `quality.json`).

## 12. Recurring loops
At each milestone close (about four phases), one tune-up phase, fed by the monthly section of the digest:
| Loop | Data | Output |
|---|---|---|
| CI efficiency | per-job p50/p95 duration and queue time (`started_at − created_at`) over 30 days; flake rate (runs with `run_attempt` > 1 that later passed, plus `suite-flake` failures); runner-minutes per merged PR; critical-path job; cores used = Σ test durations in JUnit ÷ `ctest` wall time; compile share for the caching rule, and caching correctness (a cached build must equal a cold one, if a cache is ever adopted); lowest-value tests (never failed in 90 days and duplicated by a stronger signal) | timeouts re-derived; caching decision; tests removed only with the redundant signal named |
| Determinism | `hash-equality` and nightly results; every intentional hash change has a behaviour-revision bump and reason | issue or fix |
| Weakest dimension | score 1–5 with links for performance, CI/CD, ease of use, architecture, docs/README, upgrade path/semver | the lowest becomes the next milestone's tune-up goal (RETROSPECTIVE.md) |
| Perf baseline | Ir/frame and sizes on `main` | `quality.json` ceilings tightened to measured |
| Docs freshness, drift | docs whose covered code changed since their last edit (`git log`); budgets; install line; `apply-repo-settings.sh --check`; Dependabot majors; runner-image announcements | doc, settings or upgrade PRs |

Results go into a seed under `.planning/seeds/` that the next `/gsd-discuss-phase` reads, plus `quality.json`; ≤ 2 review rounds per phase; the release PR is green at every phase close.
- **Rec** per milestone — **Alt** per phase — **Flip** a phase-level regression (timeouts, flakes) goes unaddressed for a whole milestone.

## 13. Configuration and secrets (12-factor)
- Local-only paths live in gitignored `.env.local` (`MUHTARI_ROM_DIR`, `MUHTARI_BIOS_DIR`); committed `.env.example` lists the names with empty values. Only the local presets' CMake drivers parse it as `KEY=VALUE` lines (never sourced as shell); the core and runner read no environment. **Rec** this — **Alt** a gitignored `CMakeUserPresets.json` with the paths as cache variables — **Flip** a second machine or contributor needs the same lanes (user presets then travel better).
- The secret-read guard refuses any agent command that names `.env`, `.env.<suffix>` (except `.example`, `.sample`, `.template`, `.dist`) or `.secrets` files; it fired on a grep in this session (tested) [ENG.23]. Agents read `.env.example`, and commands that need the values must not name the file.
- CI holds one variable and one secret: `MUHTARI_APP_CLIENT_ID` (Actions variable) and `MUHTARI_APP_PRIVATE_KEY` (Actions secret and Dependabot secret). The owner set them on 2026-10-15 through GitHub's App manifest flow (GSD-HANDOFF 7, which also says how to rotate the key); agents never handle the key. App permissions: Contents, Pull requests, Issues and Workflows read/write, Metadata read. Attestations use OIDC (`id-token: write`), with no stored secret.

## Open questions
Do `vars` resolve in Dependabot-triggered runs, and does an App merge of a workflow-file PR need `workflows: write`? (first Dependabot PR; fallback: write the client ID into the workflow, it is not secret) · `be32` per-PR cost and qemu's slowdown (phase 1) · Ubuntu 26.04 arm64 queue times (first 40 runs).

## Sources
| ID | Tier | Source (URL) | Pin or access date | Supports |
|---|---|---|---|---|
| ENG.01 | T1 | github/docs `data/reusables/actions/supported-github-runners.md` (https://github.com/github/docs) | cb051611377a | labels, vCPU/RAM, free for public repos |
| ENG.02 | T1 | actions/runner-images README and `images/{ubuntu,macos,windows}/*-Readme.md` (https://github.com/actions/runner-images) | e3fa5414d230 | preinstalled versions |
| ENG.03 | T1 | actions/runner-images announcements #13045, #14602, #14747, #14748 | accessed 2026-10-15 | Intel end, 26.04 GA, label moves |
| ENG.04 | T1 | GitHub REST `releases/latest` and `git/ref/tags` for the nine actions (https://api.github.com) | accessed 2026-10-15 | tags, SHAs |
| ENG.05 | T1 | googleapis/release-please `src/versioning-strategies/default.ts`, `src/strategies/{base,simple}.ts`, `src/manifest.ts`, `schemas/config.json` (https://github.com/googleapis/release-please) | v17.6.0 (712fcf01effd) | option semantics |
| ENG.06 | T1 | googleapis/release-please-action `package.json`; release-please releases | v5.0.0 (45996ed1f6d0); v17.11.2 | bundled version |
| ENG.07 | T1 | github/docs `data/reusables/actions/actions-do-not-trigger-workflows.md`, `content/actions/concepts/security/github_token.md` | cb051611377a | token trigger rules |
| ENG.08 | T1 | github/docs `content/code-security/reference/supply-chain-security/dependabot-on-actions.md` | cb051611377a | Dependabot restrictions |
| ENG.09 | T1 | github/docs `content/code-security/concepts/supply-chain-security/immutable-releases.md` | cb051611377a | immutable releases |
| ENG.10 | T1 | github/rest-api-description `descriptions/api.github.com/api.github.com.json` (https://github.com/github/rest-api-description) | 4f6cdff5b06b | settings fields, endpoints |
| ENG.11 | T1 | actions/create-github-app-token `action.yml`, release notes | v3.3.0 (64ab729a6294) | inputs |
| ENG.12 | T1 | actions/attest `action.yml`, README; actions/attest-build-provenance README | v4.2.2 | checksums input, provenance, wrapper |
| ENG.13 | T1 | Launchpad published binaries, noble and resolute (https://api.launchpad.net/devel/ubuntu) | accessed 2026-10-15 | Valgrind, clang-format, cppcheck, cross GCC, qemu |
| ENG.14 | T1 | Docker Hub tags of devkitpro/devkitppc, devkitarm, devkita64 and pspdev/pspdev (https://hub.docker.com) | accessed 2026-10-15 | console images |
| ENG.15 | T1 | silentbicycle/theft `LICENSE`, `src/theft_run.c` (https://github.com/silentbicycle/theft) | 62e093d9e33b (v0.4.5) | licence, POSIX use |
| ENG.16 | T1 | CMake manuals `cmake-presets(7)`, `ctest(1)` (https://cmake.org/cmake/help/latest/) | CMake 4.4.3; accessed 2026-10-15 | presets v11 `jobs`, `-j` default |
| ENG.17 | T1 | nesturbator `.planning/preparation/ENGINEERING.md`, `CMakePresets.json`, `CMakeLists.txt`, `.clang-format`, `.github/workflows/{ci,nightly,release}.yml`, `.github/rulesets/main.json`, `release-please-config.json`, `scripts/hygiene.sh`, `tests/cmake/global_symbols.cmake`, `.planning/RETROSPECTIVE.md` | 3200aa868371 | sibling practice |
| ENG.18 | T1 | nesturbator Actions runs 37084642541, 38676328559, 38415960004, job 116523896193, release run 38432194730, perf job 115428512765; live repo settings via `gh api` | accessed 2026-10-15 | measurements, live settings |
| ENG.19 | T1 | gabbaboy `cmake/ExpectedTests.cmake` (8091fb5), `tests/CMakeLists.txt`, `.github/workflows/{ci,release}.yml`, PRs #10–#29 | b7a7f46cd2f7 | inventory, lessons |
| ENG.20 | T1 | glueyneo `.planning/preparation/QUALITY-PERFORMANCE-AND-CI.md`, `C-CORE-ARCHITECTURE.md` | 313976dfeba3 | tails, budgets, aggregate check |
| ENG.21 | T1 | lattice_stripe `.github/workflows/{dependabot-automerge,pr-title,release-pr-automerge,drift}.yml`, `.github/ISSUE_TEMPLATE/` | 66379aeb38cf | bot merges, title lint, forms |
| ENG.22 | T1 | OpenGSD core `workflows/ship.md` | 1.17.0 | ship title, `[ci skip]` |
| ENG.23 | T1 | local runs: clang-tidy 18.1.8 and Apple clang 21 on nesturbator `src/` (3200aa868371); Homebrew formula metadata; secret-read guard | 2026-10-15 | (tested) results |
| ENG.24 | T1 | GCC Warning Options (https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html) | accessed 2026-10-15 | warning flags |
| ENG.25 | T1 | lattice_stripe history: `release.yml` (`e7d6c0464437`, `284a1741bb9c`, `9c488d3cc2a7`, `599b97a9d43f`, `33be042da12e`), `drift.yml` (`92ef010cdaa3`, `8fc5e4b18feb`), `pr-title.yml` (`2affc7fde862`), maintainer release checks (`260edf43189e`); tags v1.7.3–v1.7.12; PRs #47, #69; Actions runs 30474364540, 30474374555, 30474391526, 38228990990, 25654468833, 30473384398; live settings and required checks via `gh api` | accessed 2026-10-15 | dropped release runs, re-runnable publish, silent reporter, title scopes, settings drift, Dependabot majors |
| ENG.26 | T1 | exifcleaner workspace: `exifcleaner-node` `.github/workflows/release.yml` (`a88f57b73636`, `a44704fd528d`); `exifcleaner-electron` publish fixes (`78b895773742`, `0266e1c7c4d0`); `tools/automation_policy_gate.mjs` (`30c5ff7a80ca`, `6b4ce18e31ed`) | accessed 2026-10-15 | release built from the tagged commit, signer-workflow check, draft lookup by ID, completion-gate size |
| ENG.27 | T1 | GitHub Docs: "Control the concurrency of workflows and jobs"; "Workflow syntax for GitHub Actions" (`defaults.run.shell`) (https://docs.github.com/en/actions) | accessed 2026-10-15 | pending runs cancelled, `queue: max`; default `bash -e` without `pipefail` |
