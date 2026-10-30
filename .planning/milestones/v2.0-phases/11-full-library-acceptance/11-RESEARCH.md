# Phase 11: Full-library acceptance - Research

**Researched:** 2026-10-29
**Domain:** compatibility-ledger accounting, ratchet enforcement, private-local evaluation, generated docs and host metadata in a C emulator core whose tooling is CMake `-P` scripts and POSIX sh
**Confidence:** HIGH (every mechanism read in this session; the PAL crash and the broken pin recipe reproduced by running the repo's own binaries)

<user_constraints>
## User Constraints (from CONTEXT.md)

### Locked Decisions

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

### Deferred Ideas (OUT OF SCOPE)

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
</user_constraints>

<phase_requirements>
## Phase Requirements

| ID | Description | Research Support |
|----|-------------|------------------|
| COMP-01 | Every entry of the pinned CC0 library is accounted for in the ledger with explicit pass/fail/unknown/unsupported status and evidence; metadata inclusion is never counted as a gameplay pass | Generator design and the 204-row equality (Findings F5, F7, F8); the D-05 "metadata is never a pass" invariant; `pin.cmake` pass-through (F9) and its broken `-DBIOS` recipe (F19); doc generator guards (F13) |
| COMP-02 | Each newly claimed board/device/region has legally admissible automated behavioral evidence in public CI, and prior passing predicates remain protected by the existing ratchet | Three new `pr` diag rows are stable by frame 10 (F18); CRC binding for the four local mailbox rows (F12); tier-demotion hole at `compat_ratchet.cmake:86-92` and fixture-repo controls (F11); `library` promotion touchpoints (F10); evidence-class derivation from `expected.csv` (F13) |
| COMP-03 | Available private cartridges run only through the isolated local library preset with reproducible command/exit/commit evidence, while public CI requires no private bytes or owner verification | `library.local` design; **three defects the context did not anticipate**: the predicate tool crashes on PAL images (F1), a private `MUHTARI_ROM_DIR` fails configure on the `pr` rows (F2), and the core does not expose which list entry matched (F3); configure-time versus test-time (F4) |
</phase_requirements>

## Summary

Phase 11 is accounting, enforcement and wording on top of mechanisms that already exist: the 16-column ledger (`tests/compat/ledger.csv`), the regenerate-and-compare pattern (`policy.cartdb_generated`, `policy.compat_doc_fresh`), the `library` preset and `compat_ratchet.cmake`, `expect_fail.cmake` negative controls, and `info_claims.cmake`. Nothing needs a new dependency. The design in CONTEXT.md is sound and almost entirely implementable as written. The pinned list was re-read this session: 204 `<software>` entries, SHA-256 `5343714e...e066b9` (matches `pins.txt`), and every slot value, description and `compatibility` tag maps cleanly through D-03's table with no `&`, `"`, `;`, `@` or non-ASCII character in any description.

Several things the context did not anticipate will break the plan if the planner follows it literally, and each needs an explicit task. (1) `muhtari-test-predicate` segfaults on any PAL image: it allocates and hashes a 320x242 buffer while PAL frames are 320x292. 64 of the 204 entries are PAL (63 PAL-only plus one `PAL,NTSC`), so `library.local` would crash on them. (2) `MUHTARI_ROM_DIR=<dump dir> cmake --workflow --preset library` fails at configure because `library.cmake` insists that every `pr` row's file be found under that directory or `tests/diag`, and the public samples live in `build/ci/built`. (3) The core's identification does not expose which list entry matched (`muhtari_cart_info` has no entry index and `cart_lookup` is static), so D-14 needs a small internal seam. A fourth defect: the documented pin recipe (`pin.cmake -DBIOS`) already fails on the committed ledger because it tries to evaluate the four local `mailbox@` rows (F19). A fifth correction concerns timing: D-13 says "at configure time it lists the present softlist rows", but D-14's identification needs the compiled predicate binary, which does not exist at configure. Detection must happen at test time.

**Primary recommendation:** Build in this order: (a) fix the predicate's PAL frame size and make `pin.cmake` skip `mailbox@` rows, (b) add the generator, the pass-through in `pin.cmake` and the committed 204 rows, (c) harden the ratchet and promote `library`, (d) add `library.local` with an identification seam in `src/cart.c` and a public fixture, (e) regenerate the page, trim README and `.info`, and update docs inside the 40,960-byte budgets.

## Architectural Responsibility Map

This is a native C library with CMake tooling, so the tiers are the project's own layers rather than client/server tiers.

| Capability | Primary owner | Secondary | Rationale |
|------------|---------------|-----------|-----------|
| Softlist row generation and drift check | `tools/softlist_ledger.cmake` (cmake -P) | `tests/hygiene/tests.cmake` registration | The pinned XML is fetched, never tracked; only the hygiene lane fetches it |
| Ledger data and schema | `tests/compat/ledger.csv` | `tests/compat/pin.cmake` | One file, sorted, 16 comma-free fields |
| "A pass stays a pass" | `tests/cmake/compat_ratchet.cmake` | `.github/workflows/ci.yml` `library` job, `ci-required` | Judge is a script; enforcement is the required check |
| Identification of a private file | `src/cart.c` (core's own tables) | `tests/compat/predicate.c` as a thin caller | D-14: the core's lookup handles multi-chip, 48K-on-64K and Activision half-swap |
| Per-row evaluation | `muhtari-test-predicate` | `tests/compat/local.cmake` (new -P driver) | Existing `--id/--result` form already writes one line per row |
| Page and claims table | `tools/compat_doc.cmake` | `tests/diag/expected.csv`, `tests/inventory/ci.txt` as read-only inputs | Everything derives from files already in the tree |
| Host capability metadata | `libretro/muhtari_libretro.info.in` | `tests/cmake/info_claims.cmake` | RetroArch reads the file; the check keeps claims and tests aligned |
| Wording policy | new `policy.claims_wording` (cmake -P) | `README.md`, `docs/*.md`, `.info.in` | One canonical disclaimer, a deny list for overclaims |

## Standard Stack

### Core (all in the repository; no installs)
| Mechanism | Where | Purpose | Why standard here |
|-----------|-------|---------|-------------------|
| CMake `-P` scripts, `cmake_minimum_required(VERSION 3.25)` | `tools/*.cmake`, `tests/cmake/*.cmake` | Generators and checks | AGENTS rule 7: repository checks use CMake `-P` or POSIX sh |
| `expect_fail.cmake` (`-DCMD -DCODE -DREGEX`) | `tests/cmake/expect_fail.cmake` | Negative controls: exact exit code plus output regex | Existing; used by every `*.self_test` |
| `muhtari_add_test(NAME ... COMMAND ... LABELS ... TIMEOUT ...)` | `cmake/MuhtariTests.cmake:13` | Registration; configure fails if registered tests differ from `tests/inventory/<preset>.txt` | Existing inventory gate |
| `muhtari-test-predicate` with `--id ID --result FILE` | `tests/compat/predicate.c` | One ledger predicate on one ROM, fast boot then BIOS | Already the `library` preset's driver |
| `git` fixture repositories | `tests/hygiene/history_control.sh` pattern | Ratchet BASE-path controls | The ratchet runs `git show BASE:tests/compat/ledger.csv` and needs a real commit |

### Alternatives Considered
| Instead of | Could Use | Tradeoff |
|------------|-----------|----------|
| New `tools/softlist_ledger.cmake` | Extend `tools/cartdb.cmake` | Rejected by D-01: cartdb's "no name, no title" promise and the name-free `src/cartdb.c` stay |
| Core accessor for the matched entry | Test-side sweep that calls `muhtari__cart_identify` once per entry with a one-entry sub-table | Sweep needs zero `src/` change but adds 40 lines of table-remapping test code that nothing else exercises; accessor is about 25 lines in `src/cart.c` with coverage added to `tests/cart/test_cart.c` (which already tests multi-chip and half-swap with its own tables) |

**Installation:** none. **Version verification:** cmake 4.4.3, git 2.41.0, jq 1.7.1, gh 2.101.0 were run locally this session [VERIFIED: command output]; the project floor is CMake 3.25.

## Package Legitimacy Audit

No external packages are installed or recommended in this phase (AGENTS "Another copy and paste beats another dependency"). Packages removed due to SLOP: none. Packages flagged SUS: none.

## Architecture Patterns

### System Architecture Diagram

```
pins.txt: mame-a7800-xml --fetch-pin.sh--> build/pins/mame/a7800.xml (204 <software>)
                                                |
tests/compat/ledger.csv --(owned rows: id has "/")--> tools/softlist_ledger.cmake --+
   (bare-id rows ignored on read)                    |  slot table, region map,     |
                                                     |  guards (unknown slot, bad   |
                                                     v  characters)                 |
                                 regenerated ledger (owned verbatim + 204 derived)  |
                                                     |                              |
                  policy.softlist_ledger  <--compare-+   (hygiene lane, has the XML)
                                                     |
   tests/compat/pin.cmake: bare-id rows pass through untouched, owned rows re-pinned
                                                     |
        +--------------------+-----------------------+----------------------+
        v                    v                       v                      v
 library.<id> (pr rows)  compat_ratchet.cmake   tools/compat_doc.cmake   library.local
 fast + BIOS, 42 rows    results == ledger;     page: two populations,   test-time scan of
        |                BASE: pass stays pass  status x tier counts,    MUHTARI_ROM_DIR:
        v                AND tier stays pr      claims table, 253 rows   candidate file
 build/library/results.csv                           |                   -> predicate --identify
                                          docs/compatibility.md          -> row id (or absent)
                                          (policy.compat_doc_fresh,      -> predicate --id/--result
                                           library.compat_doc)           -> build/library/local.csv
                                                                          + one counts line
```

### Recommended file changes
```
tools/softlist_ledger.cmake        NEW  generator + check mode (-DCHECK=1)
tools/compat_doc.cmake             EDIT page restructure; new optional -DEXPECTED -DINVENTORY
tests/compat/ledger.csv            EDIT +3 diag rows, banksets row, +204 generated rows
tests/compat/pin.cmake             EDIT bare-id pass-through before the file rule (line 47-53)
tests/compat/predicate.c           EDIT PAL-sized buffers; new --identify mode
tests/compat/library.cmake         EDIT search roots; add library.local (+ self test)
tests/compat/local.cmake           NEW  -P driver for library.local and its fixture
tests/cmake/compat_ratchet.cmake   EDIT tier comparison on the BASE path
tests/cmake/claims_wording.cmake   NEW  policy.claims_wording
tests/cmake/info_claims.cmake      EDIT description length <= 512
tests/cmake/ci_required.cmake      EDIT add library to all/light/omission lists
tests/hygiene/tests.cmake          EDIT policy.softlist_ledger(+self_test); drop library from REPORT_ONLY (3 sites)
tests/policy/tests.cmake           EDIT compat_doc args, self tests, claims_wording, ratchet controls
tests/diag/tests.cmake             EDIT CRC binding at four canonical cases
src/cart.c, src/internal.h         EDIT one internal accessor (see F3)
tests/cart/test_cart.c             EDIT assertions for the accessor
.github/workflows/ci.yml           EDIT ci-required needs + jq key list; library comment
tests/inventory/{library,ci,wasm32,hygiene}.txt  EDIT new names, sorted
docs/ README.md libretro/*.info.in  EDIT (budget-neutral; see F14)
```

### Pattern 1: derived rows, one owner per row
Softlist rows (id has no `/`) are fully derived and never edited by hand or by `pin.cmake`; owned rows (id has `/`) are copied verbatim. The generator reads the existing ledger only for owned rows, so a stale bare row can never survive. Sorting must use the same key `pin.cmake` uses, `list(SORT)` over `"<id> <predicate>\t<row>"`, or running the pin recipe after generating would reorder rows and fail `policy.softlist_ledger`.

### Pattern 2: negative control by `expect_fail.cmake`
`-DCMD=<list with ; as $<SEMICOLON>> -DCODE=1 -DREGEX=<fault name>`; the control passes only on the exact exit code and message. Build fixtures in `${CMAKE_BINARY_DIR}/<lane>/<name>_fixture` at configure time, as `policy.compat_doc_fresh.self_test` does (`tests/policy/tests.cmake:114-123`).

### Anti-Patterns to Avoid
- **A boot predicate for scenario-driven diagnostics** (D-07): a `boots@120` row for `battery` or `controller-*` would claim more than its scenario proves.
- **Counting `absent` as skipped inside the test**: the test must print `P of N present, X pass, Y fail, Z absent` where `X+Y = P` and `P+Z = N`, and fail if the row count differs from the ledger's bare-row count.
- **Hard-coding 204 in the page generator**: derive N from bare rows; assert 204 only against the XML in `policy.softlist_ledger`.
- **Planning IDs in code, test names or user docs** (AGENTS Style): no `D-xx`, `COMP-xx` in comments, docs or test names.

## Don't Hand-Roll

| Problem | Don't Build | Use Instead | Why |
|---------|-------------|-------------|-----|
| Identify which list entry a private file is | SHA-1 equality on `sha1_raw` | The core's `cart_lookup` through a `src/cart.c` accessor | Multi-chip images, 48K-on-64K and the Activision half-swap are only handled there (`src/cart.c:419-450`) |
| Exact-fault negative controls | `WILL_FAIL` | `tests/cmake/expect_fail.cmake` | It demands exit code and output regex (`expect_fail.cmake:14-18`) |
| Fixture repo for the ratchet BASE path | Mock `git` | `git init` in the build dir, as `tests/hygiene/history_control.sh` does | The ratchet calls `git cat-file` and `git show` on a commit |
| "Named test exists" | New parser | Inventory regex `^name\t` as in `info_claims.cmake:33-39` | Same inventory format |
| Frame hashing, SHA-256 | New code | `runner/sha256.c` already linked into the predicate | Existing |
| CRC-16 of the diag mailbox | Re-derive | Read bytes 8-9 of the mailbox the real runner already prints | See F12 |

**Key insight:** every new check here is a variation on three existing shapes (regenerate-and-compare, `expect_fail` fault fixture, inventory-name lookup). New abstractions are not needed.

## Findings That Change the Plan

### F1. `muhtari-test-predicate` segfaults on PAL images (blocking for COMP-03) [VERIFIED: ran it]
`tests/compat/predicate.c:35` is `#define FRAME_BYTES ((size_t)MUHTARI__WIDTH * MUHTARI__HEIGHT_NTSC)` and line 424 is `video = (uint8_t *)malloc(FRAME_BYTES);`. PAL video is 292 rows: `src/internal.h:32` `const struct muhtari__timing pal = {313u, 292u, ...}`. After rebuilding the target this session, `build/ci/tests/muhtari-test-predicate tests/diag/pal.a78 boots@120` exits 139 (SIGSEGV) on both paths; `tests/diag/activision.a78` passes. No `pr` row is PAL, so nothing hit it. A throwaway copy with the buffer sized for 292 rows and the DMA scan to 313 lines passed on both the fast boot and OpenBIOS (`pass`, rc 0, both), so the BIOS hand-off window [300,340] holds for PAL.
**Plan task:** size the video buffer for the larger region, but hash and scan only what the selected region produces (`muhtari_get_info` width and height; `muhtari__timing_for_region`). Hashing the larger buffer for NTSC would change every pinned `title@F` hash (they are SHA-256 over 320x242 bytes, `predicate.c:155-164`). Add a public CI test that runs the predicate on `tests/diag/pal.a78` (`boots@120`, fast and `--bios`) in `compat/tests.cmake` (ci and wasm32 inventories). Fault it catches: a tool that crashes or hashes the wrong rows on PAL, which would turn 64 softlist entries into harness faults.

### F2. A private `MUHTARI_ROM_DIR` fails the library preset at configure (blocking for COMP-03) [VERIFIED: tests/compat/library.cmake:87-99]
Every `pr` row must be found in `MUHTARI_ROM_DIR` or `tests/diag` by SHA-1, else `message(FATAL_ERROR "library: ${id}: no file under ${library_dir} or tests/diag has SHA-1 ...")`. The 32 public 7800basic samples are in `build/ci/built`, not in an owner's dump directory, so the command D-15 prescribes (`MUHTARI_ROM_DIR=<dump dir> ... --preset library`) cannot configure.
**Recommendation:** the `pr` row search roots become `MUHTARI_ROM_DIR`, `build/ci/built` and `tests/diag` (first match wins; in CI the first two are the same directory). `MUHTARI_ROM_DIR` stays required so the existing "no directory, no run" message holds. Skip files larger than `MUHTARI_IMAGE_SIZE_MAX` (1048704 bytes, `include/muhtari.h:13`) with `file(SIZE)` before `file(SHA1)` in this configure loop, as D-14 asks. Update `docs/emulation-tests.md` (ledger) and `CMakePresets.json` `$comment` for the new search order. Fault: a private run that dies at configure for a reason unrelated to the owner's files.

### F3. The matched list entry is not exposed anywhere [VERIFIED: src/cart.c:419-450, 602, 685; include/muhtari.h `muhtari_cart_info`]
`cart_lookup` is `static` and returns a `const struct muhtari__cartdb_entry *`; callers copy only board, options, RAM and sound into `muhtari_cart_info` and set `MUHTARI_CART_HASH_MATCH`. The entry index is the XML order (`tools/cartdb.cmake` increments `entry` per `<software>`; `src/cartdb.c` has `muhtari__cartdb_7800_entry_count = 204`). `muhtari__cart_identify` (`src/internal.h:644`) already takes the tables as parameters, which is the seam `tests/cart/test_cart.c` uses.
**Recommendation (smallest seam that keeps D-14 on the core's path):** one internal function in `src/cart.c` declared in `src/internal.h`, for example `muhtari__cart_identify_entry(<same tables>, muhtari_cart_info *out, uint32_t *entry)` with `muhtari__cart_identify` calling it with `NULL`, setting `*entry` to the index of `e` (`e - t->entries`) or `UINT32_MAX`. Behaviour of every existing call is unchanged. Add assertions in `tests/cart/test_cart.c` beside its existing multi-chip, 48K-on-64K and half-swap cases (they already use synthetic tables and bytes, so this tests the real paths with no commercial bytes). The predicate's `--identify FILE...` mode calls it with the bundled tables and prints, per file, the 40-hex SHA-1 of the entry's part-0 key (or `-`), never a path or a name. The script maps that hash to the row whose `sha1_raw` first field equals it. Key facts for the mapping: part 0 is the first SHA-1 chip in XML order (`tools/cartdb.cmake` `keyed` counter), every layout's `part[0]` is 0 (`src/cartdb.c:433-440`), and exactly one SHA-1 occurs in two entries (`a3af6769...`, `hiscore` and `xm`), both `NOT_CART` and so refused by identification (`src/cart.c:603-605`). `xboard` has no `<rom>` and therefore no SHA-1 (`-`).
Fallback if the owner wants zero `src/` change: sweep the 204 entries, building a one-entry sub-table per entry and calling the existing `muhtari__cart_identify`; works but untested glue.
**Not possible:** testing the positive bundled-table path in public CI, because the bundled keys are commercial-image hashes with no public bytes. Coverage of the logic comes from `test_cart.c` with synthetic tables; the CI fixture (F4) covers counting and evaluation.

### F4. Detection must happen at test time, not configure time [VERIFIED: CMakePresets.json:155; cmake build order]
D-13 says the test "lists the present softlist rows at configure time". The `library` build preset builds `muhtari-test-predicate` after configure, so identification (F3) cannot run during configure. Keep the registration fixed (one `library.local`, one fixture test) and let the `-P` driver do everything at test time: scan `MUHTARI_ROM_DIR` for regular files no larger than the image maximum, run `--identify`, evaluate the present rows, write `build/library/local.csv`, print the counts line. Configure time may only count candidate files to scale the CTest `TIMEOUT` (the CTest timeout is fixed at registration; D-13's "scales with the present count" can only be a function of the candidate count). Wrap each spawned process in `execute_process(... TIMEOUT n)` so a hung ROM is a bounded harness fault.
Counts line format (from D-13): `local: P of N present, X pass, Y fail, Z absent, commit=<12 hex> tree=clean|dirty`, with N = number of bare-id rows in the ledger. For the commit: `git rev-parse --short=12 HEAD`; for the tree: `git status --porcelain --untracked-files=no` empty or not. The library job has full git history, so both work there.
Edge cases to decide in the plan: an identified file that the loader refuses (predicate exit 2 with no result file) should be recorded as `fail`, not as a harness fault, because it is data about the core; unwritable output and a row-count mismatch stay harness faults. The three `skipped` expansion-device rows can never be present (NOT_CART), so they always count as `absent`. Resolved at plan revision: plan 11-04 gives a refused image its own predicate exit code, 3, so exit 2 (usage error, unreadable input, a BIOS the core refuses) stays a harness fault, and local.cmake validates the BIOS before any evaluation.

### F5. Facts of the pinned list [VERIFIED: ran counts over build/pins/mame/a7800.xml, SHA-256 matches pins.txt]
- 204 `<software>` entries; every name matches `[a-z0-9_]+`, longest 10 characters; `1on1` and `1on1u` start with a digit.
- Slots (counts): `a78_rom` 119, `a78_sg` 44, `a78_sg9` 12, `a78_sg_ram` 12, `a78_pokey` 5, `a78_act` 3, `a78_abs` 2, `a78_sg_pokey` 2, `a78_hsc` 1, `a78_mram` 1, `a78_p450_ta` 1, `a78_xboard` 1, `a78_xm` 1. All 13 are in `tools/cartdb.cmake` and D-03's table.
- Compatibility tags: NTSC 101, PAL 63, `PAL,NTSC` 1, none 39. So 39 + 1 = 40 rows get region `any`, 101 NTSC, 63 PAL.
- 10 `lightgun` peripheral features. 11 entries have more than one SHA-1 chip.
- 27 descriptions contain a comma (becomes ` / `). No description contains `&`, `"`, `;`, `@`, `[`, `]`, `\` or a non-ASCII byte. One description (`Impossible Mission (NTSC, bug-fix)`) is followed on its line by an XML comment, so extract with a non-greedy `<description>(.*?)</description>` pattern, not a line match.
- 20 entries carry `supported="no"` or `"partial"` and 85 carry `cloneof`; D-02 says copy neither.
- `hiscore` and `xm` share one chip SHA-1; `xboard` has none. Do not fail the generator on the duplicate or on an empty SHA-1 list; map them to `-` or the SHA-1 as appropriate and keep rows `skipped`.
- Slot to ledger mapping confirmed against `tools/cartdb.cmake:25-37`. Quoted values: `set(slot_a78_rom 1 0 0 0)`, `set(slot_a78_pokey 1 0 1 0)`, `set(slot_a78_sg 2 0 0 0)`, `set(slot_a78_sg_ram 2 0 0 0)`, `set(slot_a78_sg9 2 1 0 0)`, `set(slot_a78_sg_pokey 2 0 1 0)`, `set(slot_a78_p450_ta 2 1 2 0)`, `set(slot_a78_act 3 0 0 0)`, `set(slot_a78_abs 4 0 0 0)`, `set(slot_a78_mram 5 0 0 0)`, `set(slot_a78_hsc 0 0 0 1)`, `set(slot_a78_xboard 0 0 0 1)`, `set(slot_a78_xm 0 0 0 1)` (board, options, sound, not-cart). Option bit 0 is ROM at $4000; sound bit 0 is POKEY at $4000, bit 1 is POKEY at $0450 (`tools/cartdb.cmake:21-24`). D-03's board and peripheral strings follow from this (for example `a78_p450_ta` is `supergame+rom_at_4000` with `pokey450`). The RAM in `a78_sg_ram` and `a78_mram` comes from the `<dataarea name="ram">`, not the slot (`('a78_sg_ram', ram) 12`, `('a78_mram', ram) 1`, no other slot has RAM except `a78_xboard` and `a78_xm`).
**Cross-check worth adding to `policy.softlist_ledger`:** the entry index equals the XML order, so each row's board, peripherals, region and lightgun can be compared with `src/cartdb.c`'s `muhtari__cartdb_7800_entries[i]` line (`{ram_kib, board, options, sound, flags, parts, layout},`, `src/cartdb.c:225-431`). That catches the two slot tables drifting apart, which is the concrete fault of having a second copy of the mapping. Keep it to one assertion loop.
- D-03's "slice order" for multi-chip `sha1_raw`: part order (XML chip order, one SHA-1 per keyed chip) equals slice order except for the single `reload` layout `{16384, 8192, 8192}` with parts `{0, 1, 1}` (`src/cartdb.c:437`), where slice order would repeat a SHA-1. Recommend part order and state it in the generator comment; the first field is the same either way.

### F6. The `banksets` row and two stale statements [VERIFIED: tests/compat/ledger.csv:5; tests/built/expected.txt:12-13; docs/emulation-tests.md `## title`]
Row 5 is `7800basic-0.41/banksets,banksets,-,-,-,-,banksets,NTSC,-,boots@120;why=bankset-board,unsupported,pr,-,-,-,-`. D-09 changes status, predicate and tier. Also change its board field from `banksets` to `bankset`: the claims table derives board tokens from the ledger and `diag/bankset` uses `bankset`, so the stray `banksets` would appear as a second, unknown family. Stale text to fix in the same edit: `tests/built/expected.txt` line 12-13 ("and its bankset board is refused") and the `title` section of `docs/emulation-tests.md` ("remains unsupported because ... board support alone does not promote it"). It was never `pass`, so no `allow.txt` line is needed.

### F7. Sorting and the 254-line ledger [VERIFIED: tests/compat/pin.cmake:97-101]
`pin.cmake` sorts `"<id> <predicate>\t<row>"` with `list(SORT)`; the generator must emit in the same order (one `list(SORT)` over owned and derived rows together). Predicates containing `;` use the `<semicolon>` placeholder in every reader (`pin.cmake:20-21`, `compat_doc.cmake:17`, `compat_ratchet.cmake`); the generator must too, or the three `boots@120;why=expansion-device` rows will split. After the phase the file has 1 header + 49 owned + 204 derived = 254 lines (253 rows); today it has 47 lines.

### F8. Row counts after the phase [VERIFIED: awk over the ledger; computed]
Today: pass/pr 39, pass/local 4, skipped/local 2, unsupported/pr 1 (46 rows). After D-07 and D-09: owned 49 rows = pass/pr 42, pass/local 4, skipped/local 3. Softlist: 201 unknown/local and 3 skipped/local (`hiscore`, `xboard`, `xm`), 0 unsupported. The `library` inventory gains `library.diag.activision`, `library.diag.absolute`, `library.diag.fractalus`, `library.local` and any new fixture test; the three expansion rows add no tests (local tier). Expected headline: "204 of 204 pinned-list entries accounted for; 0 pass, 0 fail, 0 unsupported, 201 not evaluated, 3 skipped."

### F9. `pin.cmake` pass-through [VERIFIED: tests/compat/pin.cmake:47-53]
Today, after the field-count check: `if(id MATCHES "^7800basic-0[.]41/(.+)$") ... elseif(id MATCHES "^diag/(.+)$") ... else() message(FATAL_ERROR "pin: ${id}: no file rule for this id")`. Insert a branch before it that appends the row unchanged and `continue()`s when `id` has no `/`. Keep the field-count check first so a malformed bare row still fails. `library.cmake:82` already skips any row whose tier is not `pr`, and `compat_ratchet.cmake:44-49` ignores non-`pr` rows for the results comparison, so neither needs a change for the new rows (confirms D-06).

### F10. Promoting `library` to required: exact touchpoints [VERIFIED: ci.yml:703,714; ci_required.cmake:47-48,155; hygiene/tests.cmake:139,146,160]
- `.github/workflows/ci.yml:703` `needs: [changes, hygiene, title, build, asan, nofp, be32, hash-equality, retroarch, perf, quality]` gains `library`. Line 714's key list `keys == ["asan","be32","build","changes","hash-equality","hygiene","nofp","perf","quality","retroarch","title"]` gains `"library"` between `"hygiene"` and `"nofp"` (sorted). The `library` job runs only when `heavy == 'true'` (`.github/workflows/ci.yml:374`), and the existing clause `$heavy == "false" and .value.result == "skipped" and (.key | IN("changes","hygiene","title") | not)` already lets it be skipped on light pull requests; no jq logic change.
- `tests/cmake/ci_required.cmake:47` `set(all changes hygiene title build asan nofp be32 hash-equality retroarch perf quality)` gains `library`; line 48 `light` gains `library=skipped` (it is a heavy-only job, so the "light pull request" case must list it skipped); add cases `library failed` (fail) and `library skipped when heavy=true` (fail); line 155 `foreach(omitted be32 quality)` gains `library` so dropping it from `needs` fails with `job library is not in CI required`.
- `tests/hygiene/tests.cmake:139,146,160`: `-DREPORT_ONLY=wasm32,library` becomes `-DREPORT_ONLY=wasm32` (three sites). The check at `ci_required.cmake:35` then requires `library` in `needs`.
- Text to correct together: the comment above the `library` job (ci.yml:367-371, "Report-only: 5/10 ..."), `docs/testing.md` lines about "wasm32/library stay advisory: 9/5" and "job other than `wasm32` and `library`" (lines ~53, 231, 242, 247), `docs/emulation-tests.md` ledger "In CI" ("It reports only until it has 10 green runs on `main`"). The two counters disagree today (ci.yml says 5/10, testing.md says 9/5).
- Threshold recount: the `library` job concluded `success` in 13 distinct push-to-main runs, newest first: 40854285272, 40777253646, 40753756094, 40249027703, 40203183179, 40201154961, 40112001036, 40040634219, 39890940026, 39533130206, 39532369201, 39470886147, 39470341335 [VERIFIED: `gh api repos/szTheory/muhtari/actions/runs/<id>/jobs` per run, 2026-10-29]. The context's "18" is not reproduced; 13 clears the ten-run bar. Recount at plan time; cite run IDs, never log text.
- The job's `timeout-minutes: 5` comment is `provisional` (hygiene.timeout_comments accepts that); no change is needed for the rows added here.

### F11. The ratchet's tier-demotion hole and the missing BASE controls [VERIFIED: tests/cmake/compat_ratchet.cmake:44-50, 84-94]
The current-ledger loop reads tier (`list(GET f 0 9 10 11 13 g)`, line 34) but records only status for the base comparison (`set("now ${id} ${predicate}" ${status})`, line 36). The base loop does `list(GET f 0 9 10 g)` then `list(POP_FRONT g id predicate status)` (lines 86-87) and compares only status (lines 89-91). A base-`pass` row that is `pass` but now tier `local` is never evaluated by the results comparison (non-`pr` rows are excluded) and passes the BASE check.
**Fix:** read tier (field 11) in both loops; for a base row with status `pass` and tier `pr`, require current status `pass` and tier `pr` unless the allow line exists. Message form like the existing `... pass at ${BASE}, now '...' with no allow line`. Decide whether base `pass/local` rows (the four mailbox rows) are protected on status only (current behaviour); the context protects `pr` demotion only.
**Controls (rule 7 requires representative pass and fault behaviour; today only the results-flip is tested):** one cmake `-P` driver that creates a fixture repository with `git init`, commits `tests/compat/ledger.csv` as the base, then runs the real script with `-DBASE=<sha>` against edited copies: (1) a lowered row is named, (2) a valid allow line passes, (3) an expired allow line fails, (4) a `pr` to `local` demotion is named, (5) an unchanged ledger passes. Allow dates are computed at run time from `string(TIMESTAMP ... UTC)` as the script itself does (`compat_ratchet.cmake:62-66`). Register it in `tests/policy/tests.cmake` in the `ci` inventory (not only the library lane) so `cmake --workflow --preset ci` runs it, as AGENTS rule 5 requires; git is already a dependency of other policy tests (`notices.cmake:11`). Name the controls' fault in the header comment: "a pull request lowers or demotes a passing row without an announced, dated exception".
**Known residual hole, seed not blocker:** the `library` job runs the pull request's own `compat_ratchet.cmake`, so a PR can edit the judge. CONFORMANCE section 5 described a baseline-checkout design that was never built. Record as a seed under rule 7; the D-23 push-event seed is the same family.

### F12. Binding the four local `mailbox@` rows [VERIFIED: docs/emulation-tests.md `## diag` mailbox table; build/pal-mailbox.txt; tests/diag/tests.cmake:21-135, 196-240]
The ledger evidence is the mailbox CRC-16: `diag/pal` `52a5`, `diag/bankset` `ee7c`, `diag/xboard` `f157`, `diag/xm` `9354`. The mailbox is 256 bytes at $2200 with the CRC at $2208-$2209, low byte first. The runner prints `diagnostic <512 hex digits>`; in `build/pal-mailbox.txt` the digits begin `4d374447 01 00 08 13 a5 52`, so the CRC bytes are `a5 52` and the ledger text is `52a5`: take hex characters 16-19 after the `diagnostic ` prefix, swap the two bytes. The expected value appears nowhere else in the repository today (`grep` of the four values finds only the ledger and `PROJECT.md`), so the binding is new.
Hook points, all already holding the real-runner mailbox in a variable named `mailbox`: XBoard `expansion_case(1 0 0 0 0 2)` (tag `expansion-x0`), XM `expansion_case(2 0 0 0 0 2)` (tag `expansion-m0`), BankSet case `32n` (the `case STREQUAL "32n"` branch around `tests/diag/tests.cmake:215`), and the PAL test near line 369. Compare only the canonical call: the same tags are also produced by other option combinations (PAL, dual chip, gate) whose CRCs differ. Read the row's `evidence` with `file(STRINGS ... REGEX "^diag/xm,")` as `compat/tests.cmake:7` does. Fault: a hand-edited or stale ledger CRC passing while the diag still passes. Add a control by running the comparison helper on a mailbox with one changed byte, or on a ledger copy with a changed CRC, through `expect_fail.cmake`.
Correct the sentence "`local` rows are never run in CI" in `docs/emulation-tests.md` (ledger, Tiers) as D-08 says, and `docs/compatibility.md` line 3 of the intro is regenerated anyway.

### F13. The page generator [VERIFIED: tools/compat_doc.cmake (103 lines); tests/policy/tests.cmake:111-124]
Current page: 6.9 KB, hard-codes Phase 10 prose lines 17-39 of the generator and prints one `| status | N |` table; `policy.compat_doc_fresh.self_test` regex-edits `\n[|] pass [|] [0-9]+ [|]\n` into `| pass | 0 |` and expects `is stale`. The new status-by-tier table changes that row shape, so the self test's `string(REGEX REPLACE ...)` must be rewritten to the new shape and still produce a copy that differs (add a guard that the replacement actually changed the text, otherwise the fixture equals the page and the control proves nothing).
- New inputs are optional `-DEXPECTED=tests/diag/expected.csv` and `-DINVENTORY=tests/inventory/ci.txt`; the three call sites are `policy.compat_doc_fresh`, its self test and `library.compat_doc` (`library.cmake:154-157`). The second uses `ci.txt` even in the library lane (the file is in the tree).
- Evidence class from `expected.csv` source counts per ROM tag [VERIFIED: awk over the file]: bankset tags all `guide` (17 each); expansion tags all `guide` (19 each); activision guide 22 / fit 5; **absolute fit 12 / guide 9**; fractalus fit 12 / guide 12; pal guide 6 / fit 2; battery guide 6; light0 fit 16 / guide 3; analog0 fit 15 / guide 4, analog1 fit 15 / guide 12; motion tags fit-dominant. A "fit-dominant" rule (fit rows greater than the sum of `guide`, `hw` and `rtl` rows) puts **Absolute** in model-only, which D-12's prose did not list, and Fractalus is a tie. State the rule in the generator, apply it mechanically, and let the wording follow ("model conformance" for model-only). Do not hand-assign classes.
- Controller models are not ledger columns (`peripherals` holds only `-`, `pokey450`, `savekey` on owned rows, and `hsc`, `xboard`, `xm`, `lightgun`, `pokey4000`, `pokey450` from softlist rows). The claims table therefore needs a small static claim map in the generator for controller families keyed to `expected.csv` tag globs and test names; add a guard that any board, peripheral or region token in the ledger without a map entry fails the generator ("a future pin forces a decision"), so the table cannot silently omit one. `any` is a new region token and needs a map entry and a definition in `docs/emulation-tests.md` (ledger).
- Named backing tests must exist in `ci.txt`; candidates that exist today: `diag.retail`, `diag.bankset`, `diag.bankset_tracer`, `diag.expansion`, `diag.pal`, `diag.controllers`, `diag.battery`, `input.lightgun`, `input.savekey.protocol`, `core.hsc.decode`, `runner.region.pal`, `libretro.host.battery`.
- Guards from D-17 need fault fixtures: a ledger with an unknown status, `unsupported` or `skipped` without `why=`, a `pr` row that is `unknown`. The statuses documented in `docs/emulation-tests.md` are `pass`, `fail`, `unknown`, `unsupported`, `skipped` and `infrastructure_error`; decide whether the zero-count table lists the sixth (it is a harness result, never committed).
- Page size: 204 rows of about 100 bytes plus 49 owned rows is about 27 KB, under the 40,960-byte budget (`tests/cmake/budgets.cmake:12-15`).
- The Phase 10 prose now in the generator moves to `docs/bankset-tests.md`, `docs/expansion-tests.md` and `docs/pal-tests.md` (which already own it); reword "not whole-library gameplay accounting" (the only forbidden-phrase hit today, `docs/compatibility.md:17`).

### F14. Document budgets [VERIFIED: wc -c]
`docs/testing.md` 40,955 B and `docs/emulation-tests.md` 40,905 B against 40,960. Every added line must be paid for in the same change. The largest sections to trim or move: testing.md `## Lanes` 5,865, `## The inventory` 5,950, `## CI` 5,377, `## Performance` 4,640; emulation-tests.md `## diag` 7,369, `## fuzz` 6,664. New documentation belongs in `docs/maintaining.md` (18,265 B, room), `docs/bankset-tests.md`, `docs/expansion-tests.md`, `docs/pal-tests.md` and the generated page. `README.md` is 4,159 B against 12,288 and shrinks anyway (D-20). Run `docs.budgets` after each doc edit.

### F15. Drift to repair for COMP-03 [VERIFIED: grep of tracked files, excluding .planning/phases]
Beyond the files D-15 lists, these tracked or prep files also name things that do not exist: `ASSET_POLICY.md:42` (`.env.local`, `MUHTARI_BIOS_DIR`) and `:57` (`local.firmware_window_scan`, "local only, untested"), `.planning/preparation/ENGINEERING.md:95,309`, `.planning/preparation/PROMPT-COVERAGE.md:50,58`, `.planning/preparation/CONFORMANCE.md:151`, `.planning/preparation/GSD-HANDOFF.md:193`, `.gitignore:22` (comment) and `.env.example`. The real variables are `MUHTARI_ROM_DIR` and `MUHTARI_BIOS` (`library.cmake:37,46`); no `MUHTARI_BIOS_DIR` reader exists. Simplest accurate repair: delete `.env.example` and its `.gitignore` exception (`!.env.example`, line 25) and say in `docs/emulation-tests.md` and `ASSET_POLICY.md` that the command takes the two variables inline; remove or mark the unimplemented `local.firmware_window_scan` bullet. `firmware_scan -DTRACKED=1` runs as `hygiene.firmware_scan` (`tests/hygiene/tests.cmake`, `hygiene_scan_lists` includes `-DTRACKED=1`) in the `hygiene` lane, which is a `ci.yml` job; D-15's confirmation holds.

### F16. Notices [VERIFIED: THIRD-PARTY-NOTICES.md:73-86; PROVENANCE.md:86; tests/cmake/notices.cmake]
The MAME software lists section says "Applies to: `src/cartdb.c`" and "No name, title or ROM byte is kept". The ledger and the generated page now carry titles and SHA-1s from the same CC0 list. `policy.notices` only enforces SPDX lines in tracked `.c`/`.h` files, so nothing fails, but the statement would be incomplete: extend the Changes line to cover `tools/softlist_ledger.cmake` and `tests/compat/ledger.csv` (ids, titles, SHA-1s, board and region), and adjust PROVENANCE line 86's "bundled in the core or runner" to mention the test-tree ledger. The ledger is not shipped in any release zip (installs are `muhtari`, `muhtari.h`, `muhtari-run`, the libretro core and `.info`), so `firmware_scan` over zips is unaffected.

### F17. `.info` description [VERIFIED: libretro/muhtari_libretro.info.in; tests/libretro/tests.cmake:310-325]
The `description` value is 467 characters [VERIFIED: counted]. Appending " No universal title compatibility." (34 characters) gives 501, inside the 512 limit, so trimming is only needed if other words are added. Add a length check to `info_claims.cmake`. Keep `is_experimental = "true"`. `info_claims.cmake` is run only where the libretro core builds (`tests/libretro/tests.cmake` is excluded on wasm32); a second `*.self_test` fixture with a 513-character description via `expect_fail.cmake` covers the new rule.

### F18. The three new `pr` diag rows are sound [VERIFIED: ran muhtari-run --hash-frame on each]
`activision`, `absolute` and `fractalus` produce an identical frame hash at frames 10, 30, 60, 90, 119 and 120, so `title@120` is stable on the fast boot and the BIOS path's window. `tests/roms/manifest.txt` already lists the three images (lines 25, 26, 35). Their `board` strings must be `activision`, `absolute`, `fractalus` to equal the generated rows. Honest wording: a `title@120` hash of a results screen is a regression pin of the emulator's own output; the semantic judge is `diag.retail` against `expected.csv`. Running `pin.cmake -DBIOS ... -DCOMMIT ... -DDATE ...` updates the commit and date of every evaluated row (churn across the evaluated `pr` rows); omit `-DCOMMIT/-DDATE` if the owner prefers a small diff, and then set only the three new rows' commit and date.

### F19. The documented pin recipe fails on today's ledger, and the ledger is not sorted [VERIFIED: ran it on a copy]
`cmake -DPREDICATE=build/ci/tests/muhtari-test-predicate -DLEDGER=<copy of ledger.csv> -DBUILT=build/ci/built -DDIAG=tests/diag -DBIOS=build/ci/built/openbios/openbios.bin -P tests/compat/pin.cmake` ends with `diag/pal is pass but evaluates fail` and the same for `diag/bankset`, `diag/xboard`, `diag/xm`. `pin.cmake:67` evaluates every row that is not `unsupported`/`skipped`, including the four `local` `mailbox@` rows, and the predicate rejects a `mailbox@` predicate with a usage error (exit 2). D-07 relies on exactly this recipe (`-DBIOS`) to pin the three new rows, so the plan needs a first task that makes `pin.cmake` evaluate only rows whose predicate is `title@` or `boots@` (or only tier `pr`) and leave local `mailbox@` rows untouched. Fault: the recipe cannot run at all on the committed ledger.
The same run shows the committed ledger is not in the order the script writes: `diag/pal`, `diag/bankset`, `diag/xboard`, `diag/xm` follow `diag/sg144`, whereas a sort puts `diag/bankset` first among them and `diag/sg144` before `diag/xboard`. Titles and statuses of every other row were unchanged by the run (a `cut -d, -f1-12` diff showed only the reorder). Nothing enforces sortedness today. Since the generator sorts everything with the same key, the first generated commit reorders those four rows once; say so in the plan so the diff is not mistaken for a content change.

## Common Pitfalls

### Pitfall 1: Calling the generator output "supported"
**What goes wrong:** the page or README implies a listed title works. **Why:** the default-positive pattern (MAME's `supported` defaults to yes). **Avoid:** every softlist row is `unknown`/`local`; `policy.softlist_ledger` fails any bare-id row that is `pass`/`fail` or has non-`-` evidence; the page prints "not evaluated" and the headline sentence. **Warning sign:** a doc line using "supported", "compatible", "works" or "partial" for a board or peripheral (D-21).

### Pitfall 2: Sort-order or placeholder mismatch between generator and `pin.cmake`
**What goes wrong:** running the pin recipe reorders or splits rows, so `policy.softlist_ledger` fails after a legitimate edit. **Avoid:** share the sort key and the `<semicolon>` placeholder; add a test or a manual step that runs the generator, then `pin.cmake`, and compares (the check itself does this if the generator output is already pin-sorted).

### Pitfall 3: Hashing the wrong rows after the PAL fix
**What goes wrong:** widening the buffer and hashing all 292 rows changes every NTSC `title@F` hash and the ratchet turns red. **Avoid:** hash `width x height` from `muhtari_get_info` for the loaded region.

### Pitfall 4: The 40,960-byte docs budget
`docs/testing.md` and `docs/emulation-tests.md` are within 5 and 55 bytes of the limit; any added table row fails `docs.budgets` and blocks CI (F14).

### Pitfall 5: `ci_required.cmake` self-test cases
After adding `library` to `all`, the existing "light pull request" case (`light` list) and the "title skipped on push" cases still need every heavy job to be `success` or `skipped`. If `library=skipped` is not in `light`, the light case fails with a confusing message.

### Pitfall 6: Local results leaking into git
`build/library/` is already ignored; `local.csv` must never be added to `results.csv` (the ratchet compares `results.csv` with the `pr` rows both ways). The counts line may print ids and counts, never file names, hashes of owned dumps or frame hashes (D-13, D-16 evidence rule).

### Pitfall 7: CMake list handling of titles
`[`, `]`, `\` and `;` in a title break `list()` operations in `pin.cmake`, `compat_ratchet.cmake` and `compat_doc.cmake`. None occurs at this pin, but D-03's generator guard lists only `& " ; @` and non-ASCII; add `[`, `]` and `\` so a future pin fails loudly (cartdb.cmake rewrites exactly those characters for the same reason).

### Pitfall 8: Claims about PAL through the BIOS
The predicate's BIOS path demands a hand-off frame in [300,340]. PAL passed in the throwaway test (F1), but no committed evidence exists; the new PAL predicate test provides it, and docs must still say PAL is not retail-verified.

### Pitfall 9: The agent must not look for ROMs
`MUHTARI_ROM_DIR` is unset in this environment (`env | grep -c MUHTARI_ROM_DIR` printed 0), so the recorded result for the local run is expected to be "no private content available" unless the owner supplies a directory. Never search the filesystem (D-16).

## Code Examples

### Ledger header and a derived row (target shape)
```csv
id,title,sha1_raw,sha256_raw,sha1_a78,sha256_a78,board,region,peripherals,predicate,status,tier,evidence_commit,evidence,oracle,checked
1on1,<description>,<chip sha1>,-,-,-,supergame,NTSC,-,boots@120,unknown,local,-,-,-,-
hiscore,High Score Cartridge,<chip sha1>,-,-,-,-,any,hsc,boots@120;why=expansion-device,skipped,local,-,-,-,-
```
Header quoted from `tests/compat/ledger.csv:1`. Values after `<...>` are generator output; `board` for `hiscore` is `-` because the slot is not a cartridge.

### Tier-aware BASE comparison (sketch for compat_ratchet.cmake)
```cmake
# base loop: also read tier (field 11) and remember it for passing rows
list(GET f 0 9 10 11 g)
list(POP_FRONT g id predicate status tier)
set(name "now ${id} ${predicate}")
if(status STREQUAL "pass" AND NOT "${id} ${predicate}" IN_LIST allowed)
  if(NOT "${${name}}" STREQUAL "pass")
    list(APPEND bad "${id} ${predicate}: pass at ${BASE}, now '${${name}}' with no allow line")
  elseif(tier STREQUAL "pr" AND NOT "${now_tier_${id}_${predicate}}" STREQUAL "pr")
    list(APPEND bad "${id} ${predicate}: pr tier at ${BASE}, now '${now_tier_${id}_${predicate}}' with no allow line")
  endif()
endif()
```
Illustrative; keys with `/` and spaces inside variable names are legal in CMake (the script already does it).

### Test-time driver outline for `library.local`
```cmake
# tests/compat/local.cmake  (-P)  inputs: LEDGER PREDICATE BIOS DIR OUT [IDENTITY=sha1] [EMULATOR]
# 1. bare-id rows -> map first sha1_raw field -> id
# 2. for each regular file <= 1048704 bytes under DIR: ${PREDICATE} --identify FILE  (or file(SHA1) when IDENTITY=sha1)
# 3. for each present id: ${PREDICATE} ROM <predicate field> --bios BIOS --id ID --result DIR/ID.csv  (execute_process TIMEOUT)
# 4. write OUT (id,predicate,pass|fail|absent); message(STATUS "local: P of N present, ...")
# 5. FATAL_ERROR only on: spawn crash/timeout, unwritable OUT, rows written != N
```

## State of the Art

| Old approach | Current approach | Impact |
|--------------|------------------|--------|
| Per-file dynamic `library.local.<id>` tests | One fixed `library.local` counting pass/fail/absent | Inventory stays fixed; "no silent skip" holds by counting |
| MAME `supported` default-positive | Explicit `unknown` row for every entry | Untested is visibly different from passing |
| Advisory `library` job | Required job plus tier-aware ratchet and controls | "A pass stays a pass" becomes binding |

**Deprecated/outdated:** `scripts/library.sh`, `scripts/compat-update.sh`, `.env.local` loading and `MUHTARI_BIOS_DIR` (never existed); remove from docs.

## Runtime State Inventory

Not a rename or migration phase. The renamed or re-tiered ledger row is `7800basic-0.41/banksets` (never `pass`, no stored runtime state). Stored data, live service config, OS-registered state, secrets and build artifacts: None -- verified by grep of the tree and `ci.yml`; the only external state is the GitHub ruleset's single required check `CI required`, which already aggregates `library` once added to `needs` (no ruleset edit).

## Assumptions Log

| # | Claim | Section | Risk if Wrong |
|---|-------|---------|---------------|
| A1 | An identified file that the loader refuses (predicate exit 2, no result file) should be recorded as `fail` in `local.csv`, not as a harness fault | F4 | Counts line would show a harness failure for a legitimate core refusal; owner may prefer a distinct token |
| A2 | Part order (not slice order) is the right join order for multi-chip `sha1_raw` | F5 | A reviewer reads D-03 literally; only the single reload layout differs; first field identical either way |
| A3 | Delete `.env.example` rather than correcting it | F15 | Owner may want the template kept; either satisfies D-15 |
| A4 | Base `pass/local` rows stay protected on status only | F11 | If tier protection is wanted for local rows too, the mailbox rows' tier would also be pinned |
| A5 | The `infrastructure_error` status is excluded from the page's zero-count table | F13 | The page lists one fewer status than the docs vocabulary |
| A6 | The core accessor (F3) is preferred over the sub-table sweep | F3 | Owner may prefer zero `src/` change; fallback is described |

All other claims in this document were verified against files read or commands run in this session.

## Open Questions (RESOLVED)

1. **Should `library.local` evaluate BIOS-path as well as fast boot?** RESOLVED: yes, both paths.
   - What we know: `--result` with `--bios` runs both and a row passes only if both do; PAL passes through OpenBIOS in the throwaway test.
   - What's unclear: whether a private row failing only the BIOS window should read as `fail` (it will).
   - Recommendation: keep both paths, matching the `pr` rows, and say so on the page.
   - Adopted by plan 11-04 Task 1b: `local.cmake` runs each present row with `--bios <BIOS>` and `--result`, so a row passes only when both paths pass and a BIOS-only failure is recorded `fail`; plan 11-05 documents the command and that a fail is data.

2. **PR-editable judge.** RESOLVED: dormant seed, not a Phase 11 task.
   - What we know: the `library` job runs the PR's own `compat_ratchet.cmake`.
   - Recommendation: seed, not a Phase 11 task (rule 7: optional hardening is a seed; a violation of an existing required contract is blocking, and this contract is "a pass stays a pass", which F11 repairs for the tier case).
   - Adopted by plan 11-08 Task 2: SEED-014 records ratchet protection beyond pull-request bases (push events and a judge taken from the base commit); plan 11-03 repairs the tier hole as blocking work.

## Environment Availability

| Dependency | Required By | Available | Version | Fallback |
|------------|------------|-----------|---------|----------|
| cmake | every check | yes | 4.4.3 (floor 3.25) | none needed |
| git | ratchet controls, `local:` line | yes | 2.41.0 | none |
| jq | `ci_required` controls | yes | 1.7.1 | none |
| gh | run counts, ship step | yes | 2.101.0 | none |
| `build/pins/mame/a7800.xml` | `policy.softlist_ledger` | yes locally (SHA-256 matches `pins.txt`) | `dddf4cd1faee` | `scripts/fetch-pin.sh mame-a7800-xml build/pins/mame` |
| `build/ci` tree with predicate, runner, OpenBIOS | local verification | yes | built this session | `cmake --preset ci && ctest --preset ci -L '^built$'` |
| Private dumps | optional COMP-03 run | no (`MUHTARI_ROM_DIR` unset) | -- | record "no private content available" |

## Validation Architecture

(nyquist validation is disabled for this project; this table lists each new or changed check with its concrete fault, as AGENTS rule 7 and ENGINEERING section 11 require.)

| Check (name) | Lane / inventory | Concrete fault | Fault fixture |
|--------------|------------------|----------------|---------------|
| `policy.softlist_ledger` | hygiene | A pin bump adds, removes or renames an entry; a softlist row edited by hand; a slot the generator does not know | Ledger copy with a bare row set to `pass` (named); XML copy with one entry renamed (diff names the id) |
| invariant "metadata is never a pass" (inside the above) | hygiene | A bare-id row with status `pass`/`fail` or non-`-` evidence | Same `pass` fixture |
| `policy.compat_doc_fresh` + new counts self test | ci, wasm32 | Page disagrees with ledger; counts wrong for a row kind | Fixture ledger with one row of each kind and exact expected counts |
| `policy.compat_doc` guards | ci | Unknown status, `unsupported`/`skipped` without `why=`, `pr` row `unknown`, token with no claim entry | One fixture ledger per guard through `expect_fail.cmake` |
| `policy.claims_wording` | ci | A user-facing file drops the disclaimer or overclaims (`whole[ -]library`, `every game`, `all games`, `fully compatible`, `full compatibility`) | Fixture tree with one missing phrase and one banned phrase |
| `.info` description length | libretro | Description over 512 characters | 513-character fixture |
| ratchet BASE controls | ci (policy) | Lowered or demoted pass without a dated exception | Fixture git repo: lowered, allowed, expired, demoted, unchanged |
| mailbox CRC binding (diag tests) | ci | Stale or hand-edited ledger CRC | Ledger copy or mailbox with one changed digit |
| PAL predicate test | ci, wasm32 | Predicate crashes or mis-hashes on PAL | Runs `tests/diag/pal.a78`, fast and BIOS |
| `library.local` + fixture | library (real), ci (fixture) | A silent skip; a miscount; a crash on PAL | Temporary ledger with a bare row mapped to the built `simple` sample: expects `present=1`, `pass`, other rows `absent` |
| `ci-required` cases for `library` | hygiene | `library` failing or missing from `needs` | New cases and the omission loop |
| accessor assertions in `test_cart.c` | ci | Wrong entry index for multi-chip, 48K-on-64K, half-swap | Existing synthetic tables |

Quick commands: `ctest --preset ci -R '^(policy|title|diag|cart)\.'`, `ctest --preset hygiene -R policy`, `MUHTARI_ROM_DIR=build/ci/built cmake --workflow --preset library`. Full gate: `cmake --workflow --preset ci`. New test names go in the matching `tests/inventory/*.txt` (sorted; configure fails otherwise), and `quality.json` test-count floors only rise.

## Security Domain

Omitted: `security_enforcement` is `false` in `.planning/config.json`. The relevant controls here are the repository's own hygiene rules (no ROM, BIOS or save bytes; output prints ids and counts only; `firmware_scan -DTRACKED=1`; `scripts/hygiene.sh`), covered above.

## Sources

### Primary (HIGH confidence): files read this session
- `.planning/phases/11-full-library-acceptance/11-CONTEXT.md`, `11-DISCUSSION-LOG.md`; `.planning/REQUIREMENTS.md`, `.planning/ROADMAP.md` (Phase 11), `AGENTS.md` rules via project instructions
- `tests/compat/{ledger.csv,pin.cmake,library.cmake,predicate.c,targets.cmake,tests.cmake,allow.txt}`, `tests/cmake/{compat_ratchet,info_claims,ci_required,expect_fail,budgets,notices}.cmake`, `tools/{cartdb,compat_doc}.cmake`, `src/{cart.c,cartdb.c,cartdb.h,internal.h}`, `include/muhtari.h`
- `tests/hygiene/tests.cmake`, `tests/policy/tests.cmake`, `tests/diag/{tests.cmake,expected.csv}`, `tests/inventory/*.txt`, `cmake/MuhtariTests.cmake`, `CMakeLists.txt`, `CMakePresets.json`, `.github/workflows/ci.yml`
- `docs/{emulation-tests,testing,compatibility}.md`, `README.md`, `libretro/muhtari_libretro.info.in`, `THIRD-PARTY-NOTICES.md`, `PROVENANCE.md`, `ASSET_POLICY.md`, `.env.example`, `.gitignore`
- `build/pins/mame/a7800.xml` (SHA-256 `5343714eedd715f7f1f080f1e283d5ed56e6f6509ea3eff2f8f7c8c53ee066b9`)
- Commands run: `muhtari-test-predicate` on `tests/diag/{pal,activision}.a78`; `muhtari-run --hash-frame` on five diagnostics; `gh api .../actions/runs/<id>/jobs` for 15 runs; awk and a read-only Python pass over the XML and ledger

### Secondary
- `.planning/phases/10-homebrew-expansion-memory/10-VERIFICATION.md`, `.planning/preparation/{ENGINEERING.md §11, CONFORMANCE.md §5, GSD-HANDOFF.md, HOSTS-AND-INTEGRATION.md §1}`, `.planning/seeds/SEED-001-whole-library.md`

### Tertiary
- None. The libretro core-info 512-character `description` limit and MAME's `supported` default come from CONTEXT.md's cited external pages; not re-fetched in this session [ASSUMED: from CONTEXT.md D-18].

## Metadata

**Confidence breakdown:**
- Standard stack: HIGH, all existing in-tree mechanisms
- Architecture: HIGH, each touchpoint read with line numbers
- Pitfalls: HIGH for F1 to F3 (reproduced or read directly); MEDIUM for A1 to A6 (design choices)

**Research date:** 2026-10-29
**Valid until:** the pin or any file named above changes; the run-count in F10 should be recounted at plan time
