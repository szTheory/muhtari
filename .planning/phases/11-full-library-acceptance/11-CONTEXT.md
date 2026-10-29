# Phase 11: Full-library acceptance - Context

**Gathered:** 2026-10-29
**Status:** Ready for planning

<domain>
## Phase Boundary

Account for the pinned library honestly and prove every v2.0 compatibility claim without private bytes
in CI (COMP-01, COMP-02, COMP-03). The "pinned library" is MAME's CC0 `hash/a7800.xml` at
`dddf4cd1faee` (204 `<software>` entries; already fetched by `scripts/fetch-pin.sh mame-a7800-xml` and
consumed by `tools/cartdb.cmake`). Today `tests/compat/ledger.csv` holds 46 owned rows (35 built
7800basic-0.41 samples, 11 `diag/*`) and no softlist row.

This phase is accounting, enforcement and wording. It adds no emulator behaviour, no new board,
device or region, and no new predicate kind. Phases 6–10 already produced the behavioural evidence;
Phase 11 ledgers it, binds it, protects it and states its limits.

</domain>

<decisions>
## Implementation Decisions

Decisions come from four parallel advisor fan-outs (softlist accounting, public claim evidence, private
local run, docs and host metadata), each with an adversarial pass, reconciled for coherence and
auto-followed per the owner's standing brief. Conflicts between advisors and how they were resolved are
recorded in 11-DISCUSSION-LOG.md.

### Softlist accounting (COMP-01)
- **D-01:** A new generator `tools/softlist_ledger.cmake` (cmake -P; inputs the pinned XML and the
  ledger) writes one row per softlist `<software>` entry. Softlist rows are *fully derived*: the
  generator owns every column of a row whose id has no `/`, and copies rows whose id contains `/`
  verbatim. Check mode regenerates to a temp file and compares, registered beside
  `policy.cartdb_generated` as `policy.softlist_ledger` plus a self test (on the lane that already
  fetches the pin). A pin bump that adds, removes or renames an entry fails it. Do not extend
  `tools/cartdb.cmake`: its "no name, no title" promise and the name-free `src/cartdb.c` stay.
  (Merge-preservation is unnecessary because D-11 forbids local results in the ledger.)
- **D-02:** Id is the bare MAME short name; owned ids always contain `/`, so "no `/`" means
  softlist-owned. One row per entry, 204 rows = 204 entries (an auditable equality the check asserts).
  Never collapse clones, NTSC/PAL pairs, prototypes, diagnostics or homebrew; multi-chip entries are one
  row. `cloneof` and MAME's `supported` attribute are not copied (the DTD gives `supported` no meaning
  and defaults it to yes — the default-positive pattern to avoid).
- **D-03:** Columns filled from metadata: `id`, `title` (`,` → ` / `), `sha1_raw` (the chip SHA-1, or
  chip SHA-1s `+`-joined in slice order), `board`, `region`, `peripherals`. `sha256_raw`, `sha1_a78`,
  `sha256_a78`, `oracle`, `checked`, `evidence_commit`, `evidence` are `-`. Region: the single tag maps
  directly; `PAL,NTSC` and no tag map to `any`. `lightgun` is `+`-appended for the 10 gun entries.
  Slot → board/peripherals is one table in the generator: `a78_rom` linear/-; `a78_pokey`
  linear/pokey4000; `a78_sg` supergame/-; `a78_sg_ram` supergame+ram/-; `a78_sg9`
  supergame+rom_at_4000/-; `a78_sg_pokey` supergame/pokey4000; `a78_p450_ta`
  supergame+rom_at_4000/pokey450; `a78_act` activision/-; `a78_abs` absolute/-; `a78_mram`
  fractalus/-; `a78_hsc` -/hsc; `a78_xboard` -/xboard; `a78_xm` -/xm. The researcher confirms each
  mapping against `src/cartdb.c`/`tools/cartdb.cmake` and `.planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md`
  §2.1 before planning; the board names must equal those used by the new diag rows (D-07).
  The generator fails on an unknown slot and on a description containing `&`, `"`, `;`, `@` or non-ASCII,
  so a future pin forces a decision instead of writing mangled text. Known-wrong metadata (e.g.
  `devcard`) is kept as declared; the page says these columns repeat the softlist's declarations.
- **D-04:** Defaults: `predicate=boots@120`, `status=unknown`, `tier=local`. The three expansion-device
  entries (`hsc`, `xboard`, `xm`) are `skipped` with `boots@120;why=expansion-device` (the owned
  `diag/xboard`, `diag/xm` and HSC tests carry their evidence). `unsupported` is never a default; at this
  pin every slot maps to a supported board, so zero softlist rows are unsupported and the page prints the
  0. A future slot the core refuses becomes `unsupported` with a `why=` through the generator's table.
- **D-05:** Check-mode invariant (the "metadata is never a pass" test): a row whose id has no `/` has
  status in {unknown, skipped, unsupported} and `-` evidence fields. Its fault fixture is a softlist row
  set to `pass`; the check must name it.
- **D-06:** `tests/compat/pin.cmake` passes rows whose id has no `/` through untouched before the file
  rule lookup (today it dies "no file rule for this id"). `compat_ratchet.cmake` and `library.cmake` need
  no change for the new rows: `local` rows demand no results and no files.

### Public claim evidence and protection (COMP-02)
- **D-07:** The owned self-contained retail diagnostics `tests/diag/activision.a78`, `absolute.a78` and
  `fractalus.a78` become tier-`pr` `title@120` ledger rows (`diag/activision`, `diag/absolute`,
  `diag/fractalus`, boards `activision`, `absolute`, `fractalus`), pinned by `pin.cmake` with `-DBIOS`
  on both paths, following the `diag/bank6`/`diag/sg144` pattern. Scenario-driven diagnostics
  (`battery`, `controller-analog`, `controller-light`, `controller-motion`, `expansion`) do not become
  default-boot rows — a boot predicate would not assert their scenarios and would overclaim. They are
  listed in a claim → test table (D-16) instead.
- **D-08:** Bind the four local `mailbox@` rows (`diag/bankset`, `diag/xboard`, `diag/xm`, `diag/pal`):
  the diag test that runs each tagged case compares its CRC with that row's `evidence`, so a stale or
  hand-edited ledger CRC fails while the diag still passes. Correct `docs/emulation-tests.md`'s "local
  rows are never run in CI" to say local rows are not judged by the library preset (some are executed by
  diag tests). `predicate.c` gains no `mailbox@` support.
- **D-09:** `7800basic-0.41/banksets` changes from `unsupported` (`why=bankset-board`, now false) to
  `skipped` with `boots@...;why=recipe-does-not-assemble`, tier `local`. It was never `pass`, so no
  allow line is needed. The other 45 owned rows stay verbatim.
- **D-10:** Make protection binding:
  - Promote the `library` CI job to required: add it to `ci-required`'s `needs:` and jq key list in
    `.github/workflows/ci.yml`, and drop `library` from `-DREPORT_ONLY` in `tests/hygiene/tests.cmake`
    (three call sites). Recount the "10 green runs on main" threshold at plan time from hosted runs (the
    advisor counted 18; the ci.yml and `docs/testing.md` counters disagree and are fixed together).
  - Close the tier-demotion hole in `tests/cmake/compat_ratchet.cmake`: a base-`pass` row must keep its
    tier (`pr` stays `pr`) unless an allow line exists; today the base loop reads only id, predicate and
    status, so moving a passing row to `local` escapes.
  - Give the BASE/allow path negative controls: a lowered row is named, a valid allow line passes, an
    expired allow line fails, a tier demotion is named. This is a violation of an existing required
    contract ("a pass stays a pass"), so it is blocking, not a seed.
- **D-11:** Admissible evidence is by input class, per `ASSET_POLICY.md` §1–3 and
  `.planning/preparation/CONFORMANCE.md` §7: (1) owned MIT diagnostics, synthetic images, firmware stubs
  and hand-written movies; (2) third-party CC0/permissive code built from pinned tools in CI (7800basic
  samples, OpenBIOS), hashes only; (3) MAME CC0 softlist facts — dispositions only, never a pass;
  (4) never: commercial dumps, real BIOS/HSC firmware, saves, frames, movies or logs derived from them,
  GPL test ROMs, reference-emulator output as a gate (DEC.41). A committed `pass` must be reproducible in
  public CI.
- **D-12:** Narrow wording, add no new device tests. Each claim is tagged by evidence class:
  "owned-oracle" (expected records sourced `guide`/`hw`/`rtl`) or "model-only" (`fit`-dominant: XG-1
  latency, paddle/keypad timing, trak-ball/ST/Amiga motion, SNES2Atari/Mega7800). Say "model" and
  "documented sequences", never "supports titles"; no real HSC ROM is exercised; PAL is not
  retail-verified. Movie-driven response tests using the CC0 samples as independent decoders become a
  seed (D-23).

### Private local run (COMP-03)
- **D-13:** One always-registered test `library.local` (one fixed line added to
  `tests/inventory/library.txt`). At configure time it lists the present softlist rows; a cmake -P script
  evaluates each and writes `build/library/local.csv` with every softlist row accounted as `pass`,
  `fail` or `absent`, and prints `local: P of N present, X pass, Y fail, Z absent, commit=<12 hex>
  tree=clean|dirty`. It fails only on harness faults (crash, timeout, unwritable output, row count
  mismatch); a predicate `fail` is data. Zero, some or all files configure identically, so "no silent
  skip" holds by counting. `pr` rows keep their hard configure failure. Only bare-id rows run here.
  The timeout scales with the present count; output prints ids and counts only, never paths.
- **D-14:** Matching a private file to a row goes through the core's own identification (the cartdb
  lookup that already handles multi-chip, 48K-on-64K and Activision half-swap layouts), not raw file
  SHA-1 equality against `sha1_raw`. The researcher confirms how `muhtari-test-predicate` or the runner
  exposes the identified softlist entry. Skip files larger than the image maximum when scanning, so an
  accidental broad `MUHTARI_ROM_DIR` does not hash everything.
- **D-15:** Local results never update the committed ledger (enforced by D-05). They live only under the
  ignored `build/library/`; never concatenated into `results.csv`. No new hygiene rule: gitignore,
  `scripts/hygiene.sh` (name, magic, NUL on staged/tree/history) and `tests/cmake/firmware_scan.cmake`
  (cartdb SHA-1 over release-zip members and tracked files) already block bytes; the plan confirms
  `firmware_scan -DTRACKED=1` runs in a `ci` lane. No `scripts/library.sh`, no `.env.local` reader: the
  command is `MUHTARI_ROM_DIR=<dump dir> [MUHTARI_BIOS=<file>] cmake --workflow --preset library`.
  Repair the drift — `.env.example`, `.planning/preparation/GSD-HANDOFF.md` (~line 193) and
  CONFORMANCE §5 name `.env.local`, `MUHTARI_BIOS_DIR`, `scripts/library.sh` and
  `scripts/compat-update.sh`, none of which exist or are read. Either correct `.env.example` to the real
  variables or delete it (and its `.gitignore` exception).
- **D-16:** The phase does not wait for private content. The agent never searches the filesystem for
  ROMs; it reads `MUHTARI_ROM_DIR` from the environment only. A public CI fixture proves the mechanism: a
  temporary ledger with a softlist-style row mapped to a public built sample expects `present=1` and a
  pass, and a missing row counted `absent`. Because public samples are not in `src/cartdb.c`, the
  fixture exercises the counting and evaluation path through a test-only identity seam; the planner
  chooses the smallest seam that leaves D-14's real identification path covered by an existing cartdb
  test. The agent runs the preset once and records command (with
  `<dump dir>`), exit status, 12-hex commit and the counts line — no file names, hashes or frames of
  owned dumps. If none are available, it records "no private content available". The ship note offers
  the owner one optional command (rule 6: a run on their own hardware, reason: only their machine has
  the dumps); the reply is a note, never UAT, never a ledger change.

### Generated docs and host metadata
- **D-17:** `tools/compat_doc.cmake` derives everything from the 16 existing columns (no schema change):
  - Two populations, never blended: "Pinned CC0 list: N entries" (N computed from bare-id rows) and
    "Owned samples and diagnostics: M rows".
  - Status × tier counts for each population, always printing all statuses including zeros, and a
    headline: "N of 204 pinned-list entries accounted for; X pass, Y fail, Z unsupported, W not
    evaluated, V skipped. Not evaluated means untested, not failing." `src/cartdb.c` recognising an image
    is metadata, not playability.
  - Evidence kinds by rule: public gameplay (7800basic `title` pass), public boot only (`boots` pass),
    owned diagnostic = model conformance (never gameplay), pinned-list pass/fail, not evaluated,
    unsupported/skipped with reason.
  - A claims table whose rows are derived board families, peripheral tokens and regions, with the
    D-12 evidence class and the backing tests (claim → test names; a check asserts each named test exists
    in `tests/inventory/ci.txt`, reusing the `info_claims.cmake` logic). A claim with no evidence prints
    "none".
  - Every row listed, softlist rows in their own table (about 27 KB, under the 40 KB budget), so a user
    can find their title.
  - Generator guards: fail on a status outside the vocabulary, on `unsupported`/`skipped` without
    `why=`, on a `pr` row that is `unknown`. Extend `policy.compat_doc_fresh.self_test` with a fixture
    ledger holding one row of each kind and exact expected counts (the existing regex on `| pass | N |`
    must be updated for the new shape).
  - The hand-written Phase 10 detail prose moves out of the generator into `docs/bankset-tests.md`,
    `docs/expansion-tests.md` and `docs/pal-tests.md`, which already own it.
- **D-18:** `is_experimental` stays `"true"`. A pass count is gameable (owned rows already meet any
  count), and D-05 means no pinned-list row can pass from public evidence, so no ledger-derived criterion
  can be met in this phase. Add no gate. Update `.planning/preparation/HOSTS-AND-INTEGRATION.md` §1 and
  `.planning/seeds/SEED-001-whole-library.md` to say the flip needs pinned-list (commercial) evidence a
  future milestone defines, and record that as a seed (D-23). No capability key changes. Edit only the
  `.info` `description` (D-19) and add a one-line, ≤512-character description check to
  `tests/cmake/info_claims.cmake` (the libretro core-info template's documented limit; currently ~466).
- **D-19:** One canonical disclaimer — "Evidence covers the rows listed; there is no claim of universal
  title compatibility." — in the generated page header and the README status; the `.info` description
  carries the short form "No universal title compatibility." (trim existing text to fit). A small
  `policy.claims_wording` check (cmake -P, with a self test naming the fault "a user-facing file drops the
  disclaimer or overclaims") requires the phrase in README and `libretro/muhtari_libretro.info.in` and
  rejects, case-insensitively, `whole[ -]library`, `every game`, `all games`, `fully compatible` and
  `full compatibility` in `README.md`, `docs/*.md` and `libretro/*.info.in`. Reword the current page's
  "not whole-library gameplay accounting".
- **D-20:** README Status shrinks to a few lines that link the compatibility page instead of enumerating
  boards, controllers and PAL (enumerated prose is unchecked drift). `docs/libretro.md#devices` stays
  (capability, not compatibility).
- **D-21:** Vocabulary one-to-one with ledger statuses: "pass" (predicate met on both boot paths),
  "fail", "not evaluated" (`unknown`), "unsupported (reason)", "skipped (reason)". Never "supported",
  "compatible", "works" or "partial" for a board or peripheral; say "has diagnostic evidence" or
  "model conformance". Counts always carry a denominator; no percentage-of-204 headline.
- **D-22:** The milestone name "The whole library" stays a planning label under `.planning/` only. The
  phase PR title is neutral (e.g. `feat: account for the pinned library in the compatibility ledger`),
  yielding the next 0.x release; never `Release-As: 2.0.0` or a package rename. One line in
  `docs/maintaining.md` says milestone names are not release names.

### Budgets and process
- `docs/testing.md` (40,955 B) and `docs/emulation-tests.md` (40,905 B) are at the 40,960 B budget:
  every addition there is offset by moving or trimming text in the same change.
- Every new or materially changed check names its fault and has pass/fault fixtures (rule 7):
  `policy.softlist_ledger`, the D-05 invariant, the D-08 CRC binding, the D-10 ratchet controls,
  `library.local` and its fixture, the extended `compat_doc_fresh` self test, `policy.claims_wording`,
  the `.info` length check. New test names go in the matching `tests/inventory/*.txt`.

### Claude's Discretion
- Exact file split, script names other than those fixed above, page layout details and wording beyond
  D-19/D-21, plan/wave decomposition, and how D-08's CRC comparison hooks into the existing diag tests.

</decisions>

<canonical_refs>
## Canonical References

**Downstream agents MUST read these before planning or implementing.**

### Requirements and scope
- `.planning/ROADMAP.md` — Phase 11 goal and four success criteria
- `.planning/REQUIREMENTS.md` — COMP-01, COMP-02, COMP-03; Deferred list
- `AGENTS.md` — rules 1, 3, 6, 7, 10 govern this phase
- `.planning/preparation/DECISIONS.md` — DEC.20 (softlist as bundled database), DEC.41 (no reference-emulator gate), DEC.42 (owner dumps only in a local preset)

### Ledger, ratchet and tiers
- `.planning/preparation/CONFORMANCE.md` §5 (ledger design; its `scripts/library.sh`/`compat-update.sh` are superseded by D-13–D-15) and §7 (admissible inputs)
- `docs/emulation-tests.md` `## title`, `## ledger`, `## diag`
- `tests/compat/ledger.csv`, `pin.cmake`, `library.cmake`, `targets.cmake`, `predicate.c`, `allow.txt`
- `tests/cmake/compat_ratchet.cmake`, `tests/cmake/ci_required.cmake`, `tests/hygiene/tests.cmake` (lines ~139–160)
- `.github/workflows/ci.yml` (`library` job ~line 372, `ci-required` ~line 700)
- `tests/inventory/library.txt`, `tests/inventory/ci.txt`, `tests/inventory/hygiene.txt`, `cmake/MuhtariTests.cmake`

### Softlist
- `pins.txt` (`mame-a7800-xml`, `mame-a2600-xml` rows), `scripts/fetch-pin.sh`
- `tools/cartdb.cmake`, `src/cartdb.c`
- `.planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md` §1 (identification, two-chip, 48K-on-64K, Activision layouts), §2.1 (schemes)

### Evidence already shipped
- `tests/diag/` (`*.s`, `expected.csv` with `source` column, `tests.cmake`, `driver.c`)
- `.planning/phases/06-retail-cartridge-boards/06-VERIFICATION.md`, `07-battery-saves/07-VERIFICATION.md`, `08-controllers-at-the-pins/08-VERIFICATION.md` (circularity table), `08-CONTEXT.md` (D-06, D-17, D-18), `09-pal-games-and-audio/09-VERIFICATION.md`, `10-homebrew-expansion-memory/10-VERIFICATION.md`
- `ASSET_POLICY.md` §1–3, §5; `PROVENANCE.md`

### Docs and host metadata
- `tools/compat_doc.cmake`, `docs/compatibility.md`, `tests/policy/tests.cmake` (`policy.compat_doc_fresh`)
- `libretro/muhtari_libretro.info.in`, `tests/cmake/info_claims.cmake`, `tests/libretro/tests.cmake`
- `.planning/preparation/HOSTS-AND-INTEGRATION.md` §1 (`is_experimental` schedule), `.planning/seeds/SEED-001-whole-library.md`
- `README.md`, `docs/maintaining.md`, `docs/testing.md`, `docs/bankset-tests.md`, `docs/expansion-tests.md`, `docs/pal-tests.md`, `release-please-config.json`, `tests/cmake/budgets.cmake`
- `.env.example`, `.gitignore`, `.planning/preparation/GSD-HANDOFF.md` (~line 193), `scripts/hygiene.sh`, `tests/cmake/firmware_scan.cmake`
- External: libretro core-info `00_example_libretro.info` (https://github.com/libretro/libretro-core-info/blob/master/00_example_libretro.info) — `is_experimental` meaning, 512-character description; MAME `softwarelist.dtd` at dddf4cd1faee — `supported` attribute

</canonical_refs>

<code_context>
## Existing Code Insights

### Reusable Assets
- `tools/cartdb.cmake`: already parses the pinned softlist `<software>` blocks; the new generator copies its ~10-line block split rather than sharing code (keeps cartdb name-free).
- `policy.cartdb_generated` and `policy.compat_doc_fresh`: the regenerate-and-compare pattern for D-01 and D-17.
- `tests/compat/pin.cmake` with `-DBIOS`: pins and evaluates the new `diag/*` title rows on both paths.
- `tests/cmake/info_claims.cmake`: its "named test exists in the inventory" logic is reused for the claims table check.
- `tests/diag/expected.csv` `source` column: drives the owned-oracle vs model-only tag (D-12).

### Established Patterns
- The ledger has 16 comma-free fields, sorted by `id` then `predicate`; schema enforced.
- Ratchet key is (id, predicate); `unknown` → anything and `fail` → `pass` are free; `allow.txt` lines expire within 60 days.
- Configure fails when registered tests differ from `tests/inventory/<preset>.txt`, so per-file dynamic registration is out (hence one `library.local`).
- Hashes are never typed by hand; pins come from the recipe.

### Integration Points
- `ci-required` aggregate in `ci.yml` and `ci_required.cmake`'s `REPORT_ONLY` list (D-10).
- Hygiene lane that fetches the MAME pins hosts `policy.softlist_ledger`.
- The diag tests that run the bankset/expansion/PAL tagged cases host the D-08 CRC comparison.

</code_context>

<specifics>
## Specific Ideas

- Safe state is the default state: every softlist row starts `unknown`/`local`, and a test forbids a softlist `pass`.
- The page answers "is my game listed?" — every entry visible, untested clearly distinct from failing.
- Prior art: MAME's default-positive `supported`, RPCS3's ladder without an "untested" tier, and libretro DATs that identify without judging all show why explicit `unknown` rows matter.

</specifics>

<deferred>
## Deferred Ideas

- **D-23 seeds (optional tooling/test improvements under rule 7):**
  - Movie-driven independent-consumer checks: `mousedemo` (ST/Amiga/trak-ball direction and sign), `snescontrollers`, `mega7800controllers`, and a `hiscoretable` SaveKey write check. A sample that fails after one investigation narrows its device to "owned model only".
  - `is_experimental` flip criterion: define the pinned-list (commercial) evidence path (for example a ratio over evaluable pinned-list rows with zero `fail`) in a later milestone, then flip in one line.
  - A non-ratcheted "locally observed" doc section generated from a local-only file, if the owner later wants local passes visible.
  - Ratchet protection on push events (`MUHTARI_LEDGER_BASE` is empty on push; protection exists only on pull requests).

### Reviewed Todos (not folded)
- "Close the re-review's findings on the Dependabot gate and the publish checks" — CI tooling, not ledger acceptance; stays a seed.
- "Close the phase 2 review seeds on the nightly filter and the ROM tables" — nightly paths/cartdb edge cases; unrelated to accounting; stays a seed.
- "Close the phase 2 re-review findings when MARIA and the run loop land" — core review leftovers; out of scope.
- "Promote the wasm32 lane into CI required" — separate lane promotion; D-10 promotes only `library`.

</deferred>

---

*Phase: 11-full-library-acceptance*
*Context gathered: 2026-10-29*
