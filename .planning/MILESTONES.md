# Project Milestones: muhtari

## v2.0 — The whole library (Shipped: 2026-10-29)

**Delivered:** Expanded boards, persistent saves, scoped controllers and PAL across all three deliverables, with honest library evidence.
**Package release:** [v0.10.0](https://github.com/szTheory/muhtari/releases/tag/v0.10.0), 19 assets; planning v2.0 is separate from package semver.
**Closeout type:** override_closeout under AGENTS rules 6–7 and GSD-HANDOFF §3; audit status tech_debt.
**Phases completed:** 5–11; 7 phase goals and 27/27 scoped requirements.
**Plans:** 75 accepted; 76 summarized active slots; 82 total historical summaries.
**Recorded tasks:** 192 across all 82 summaries, including superseded/partial work; no halted plan is counted as accepted.

**Key accomplishments:**

- Shipped Activision, Absolute, Fractalus, BankSet and the XBoard/XM RAM/POKEY/HSC subset through all three deliverables.
- Added load-stable HSC/SaveKey battery storage, checked runner saves and real fresh-launch RetroArch persistence.
- Delivered typed controller input and movie2 replay for lightgun, paddle/keypad, quadrature and modern adapters, with independent pin-level diagnostics.
- Added PAL timing, geometry, boot and sustained audio carry while retaining existing NTSC observations.
- Accounted for every pinned-list entry, bound compatibility claims to public evidence and made the library ratchet required in CI.
- Improved pipeline reliability, armed the controlled performance comparison and completed automated release/install verification.

**Stats:** 469 files changed, 85,109 insertions and 1,737 deletions from initialized
v2.0 `65b360d` to shipped `d5fd241`; 58,665 tracked C/header lines, including 10,263
in src/. Timeline: 2026-10-21 to 2026-10-29, eight elapsed calendar days. Counts
exclude this closeout. The helper's six-task prose-only count is superseded by
82 parsed summary actuals/tasks_completed values totaling 192.

### Known verification overrides and debt

Known verification overrides: 15 newly acknowledged, 13 carried forward from a prior close
(see STATE.md Deferred Items). Separately, all seven historical phase fingerprints remain
stale after later accepted changes. No fingerprint or failed verdict was rewritten.
The audit reports 25 fully wired paths plus two accepted Phase 5 warnings; eight complete
flows plus one qualified monthly-report flow; no broken core/host flow or requirement gap.
COST-01 raw monthly numeric totals, WR-01 delayed-release equal-base selection, advisory
property calibration, optional Phase 10/11 findings and hardware/HSC limits remain deferred.
The cancelled Nightly remains cancelled; current release success does not renew that evidence.
Pinned-list status remains 201 unknown/3 skipped, with no gameplay passes.

**Release evidence:** `gh run view 40956113369 --json attempt,status,conclusion,headSha,url,jobs`
— exit 0, attempt 1 success at d5fd241, including publish and install-line-latest:
[release run](https://github.com/szTheory/muhtari/actions/runs/40956113369).
`gh release view v0.10.0 --json tagName,isDraft,publishedAt,targetCommitish,url,assets`
— exit 0, published with 19 assets. `cmake -DINPUT=/tmp/muhtari-v2-main-ci.json -DEXPECTED_SHA=d5fd241e3f4d701d73867cc2e465c56a6fe6793d -P tools/check_ci_run.cmake`
— exit 0, [merged CI](https://github.com/szTheory/muhtari/actions/runs/40956113335).
Release PR41 merged green: [PR CI](https://github.com/szTheory/muhtari/actions/runs/40946415397).

**Archives:** [roadmap](milestones/v2.0-ROADMAP.md),
[requirements/outcomes](milestones/v2.0-REQUIREMENTS.md),
[audit](milestones/v2.0-MILESTONE-AUDIT.md),
[phase records](milestones/v2.0-phases/),
[product direction](milestones/v2.0-PRODUCT-ROADMAP.md),
[historical position receipts](milestones/v2.0-STATE.md).

**Next:** `$gsd-new-milestone` selects scope and creates fresh requirements. Tune-up first;
states/movies and browser remain alternatives for that discussion. No new milestone begins here.
Tag creation is disabled; release-please owns package tags. Completed quick-task history stays in place.
Lesson export is available with `export lessons`; lessons remain project-local by default.


### v2.0 closeout verification

| Command | Exit / result | Run URL |
|---|---|---|
| `cmake --workflow --preset ci` | 0; 673 CI tests and 6 release/archive checks after requirements removal | N/A-local |
| `cmake --workflow --preset hygiene` | 0; 30/30 checks | N/A-local |
| `sh scripts/hygiene.sh --tree` with stable index | 0; an earlier scan overlapping staging reported old missing paths and remains an unaccepted scan | N/A-local |
| `python3 /tmp/muhtari-v2-verify.py` | 0; 221 phase files preserved, 82 plans/summaries, 7 verifications, 27/27 requirements, links resolve, originals safely archived, user edits/implementation unchanged | N/A-local |
| `node <GSD_TOOLS> query summary-extract <archived-summary> --fields requirements_completed` for all 82 summaries | 0 each; every archived frontmatter parses | N/A-local |
| `node <GSD_TOOLS> query audit-open --json` | 0; zero open, 28 acknowledged (15 new, 13 carried) | N/A-local |
| `git diff --check` and `git diff --cached --check` | 0 | N/A-local |

The safety commit `98dd410` contains the archive before active REQUIREMENTS deletion.
No verification log, raw API payload or JUnit is committed.

---

## v1.0 — 7800 games play with sound in RetroArch (Shipped: 2026-10-21)

**Delivered:** NTSC linear and SuperGame cartridges play with TIA and POKEY audio through all three deliverables.
**Package release:** v0.4.0. Planning milestone v1.0 does not change the package version.
**Closeout type:** override_closeout, explicitly approved at milestone close.
**Phases completed:** 1–4; 73 plans, 149 recorded tasks; 25/25 milestone requirements covered.

**Key accomplishments:**

- Shipped an embeddable C17 library, headless runner and libretro core with automated attested releases and one-line RetroArch installation.
- Matched all 2,560,000 public 6502 vectors and implemented bounded SALLY bus timing, RIOT and validated cartridge loading.
- Rendered linear and SuperGame cartridges with measured MARIA DMA timing, diagnostic ROMs and cross-platform frame hashes.
- Integrated fast boot, CC0 OpenBIOS, controller movies, achievement memory maps and automated RetroArch gameplay checks.
- Delivered integer TIA and POKEY sound with canonical raw/PCM hashes, libretro callback parity and clipping-free sample pins.
- Verified the production 64-phase Q23 synthesiser against spectral and alias controls, with bounded allocation-free frame execution.

**Stats:** 492 files changed, 97,449 insertions and 369 deletions from initial preparation
`8ab05fa` to shipped release commit `40ce425`; 38,684 tracked C/header lines,
including 8,577 in `src/`. Timeline: 2026-10-15 to 2026-10-21 (six elapsed calendar days).
Counts exclude this closeout. Task count is the recorded summary count, not an estimate.

### Known verification overrides

Known verification overrides: 15 newly acknowledged, 0 carried forward from a prior close
(see STATE.md Deferred Items).

Separately, phase 1, 2, 3 and 4 verification fingerprints remain stale after later
covered-file changes. The owner explicitly accepted all four overrides. Historical
reports and digests retain their original verdicts; this is not a verified_closeout.
The milestone audit found no functional, requirements or integration gaps. 15 items acknowledged as deferred (six todos, eight dormant seeds and one compatibility-test output race); four stale phase verification fingerprints explicitly accepted.

The instructions-per-frame gate remains report-only, WASM/library jobs remain
advisory, and gate code is 1,367/8,577 source lines (15.94%, above the 10% budget).
Documented title-predicate and finite spectral-matrix limits remain in force.

**Release evidence:** [Release publication and latest-install run](https://github.com/szTheory/muhtari/actions/runs/39533130210) completed successfully; v0.4.0 is published with 19 assets.

**Archives:** [Roadmap](milestones/v1.0-ROADMAP.md),
[requirements and outcomes](milestones/v1.0-REQUIREMENTS.md),
[audit](milestones/v1.0-MILESTONE-AUDIT.md),
[phase records](milestones/v1.0-phases/).

**Next:** `$gsd-new-milestone` selects the next scope and defines fresh requirements.
No next milestone is started by this closeout. Tag creation is disabled by project configuration.

## Closeout verification

`cmake --workflow --preset ci` — exit 0: 605 regular tests and six release/archive
checks passed after archival. `scripts/hygiene.sh --tree` and `git diff --check` —
exit 0. Archive integrity check — exit 0: all 177 phase files retained, including
73 plans, 73 summaries and four unchanged verification reports; only the deferred
item acknowledgment modifies a moved phase artifact. `query audit-open --json` —
exit 0: zero open, 15 acknowledged. Hosted release evidence is linked above.
