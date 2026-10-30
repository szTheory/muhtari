# Phase 11: Full-library acceptance - Discussion Log

> **Audit trail only.** Do not use as input to planning, research, or execution agents.
> Decisions are captured in CONTEXT.md — this log preserves the alternatives considered.

**Date:** 2026-10-29
**Phase:** 11-full-library-acceptance
**Areas discussed:** Softlist accounting, Public claim evidence, Private local run, Docs and host metadata

The owner pasted the standing fan-out brief during this step (every decision point researched through all
relevant lenses, primary sources first, adversarial pass, one coherent recommendation per point,
auto-followed; "another copy and paste is better than another dep"). All four areas were researched by
parallel advisor agents (calibration `minimal_decisive`) and their recommendations were followed without
per-area questions.

---

## Softlist accounting

| Option | Description | Selected |
|--------|-------------|----------|
| Generated rows + regenerate-and-compare policy test | New `tools/softlist_ledger.cmake`, one row per entry, bare MAME ids | ✓ |
| Hand-written rows + count test | No tool; drift caught only by count | |
| Prefix ids with `mame/` | Clearer provenance, departs from CONFORMANCE §5 | |
| Default `tier=pr` | Breaks ratchet and library preset (no public files) | |

**Choice:** generated, bare ids, `unknown`/`local`/`boots@120`, expansion devices `skipped`, zero default `unsupported`.
**Notes:** The advisor proposed a merge-preserving generator to keep local results; reconciled to a fully
derived generator because the private-run decision forbids local results in the ledger.

## Public claim evidence

| Option | Description | Selected |
|--------|-------------|----------|
| Ledger the self-contained retail diags as `pr` title rows; scenario diags in a claim → test table; bind local mailbox CRCs | Reuses pin/ratchet | ✓ |
| Every diag as a `mailbox@` ledger row | Needs a new predicate kind; overclaims for scenario diags | |
| Narrow wording by evidence class | owned-oracle vs model-only | ✓ |
| Add movie-driven independent-consumer tests now | Optional under rule 7 | seed |
| Promote `library` to required + close tier-demotion hole + ratchet negative controls | Makes "pass stays pass" binding | ✓ |
| Keep `library` advisory | Protection stays advisory | |

**Choice:** as selected. `7800basic/banksets` moves from a false `unsupported` to `skipped (recipe-does-not-assemble)`.

## Private local run

| Option | Description | Selected |
|--------|-------------|----------|
| One fixed `library.local` test counting pass/fail/absent | Inventory stays fixed; no silent skip | ✓ |
| Per-file `library.local.<id>` tests | Needs an inventory-gate exemption | |
| Local results never enter the ledger | Invariant-checked | ✓ |
| `compat-update.sh` merges local results | Unreproducible passes frozen by the base ratchet | |
| No script/dotenv; env-var preset; fix `.env.example` drift | | ✓ |
| Gate the phase on an owner run | Violates rule 6 | |

**Choice:** as selected; matching via the core's own identification (from the softlist advisor's finding
on multi-chip and half-swap layouts), not raw file SHA-1.

## Docs and host metadata

| Option | Description | Selected |
|--------|-------------|----------|
| Keep `is_experimental = "true"`, no gate, seed the criterion | | ✓ |
| Keep true + ratio gate over pinned-list passes | Gate unsatisfiable while softlist rows cannot pass | |
| Flip to `"false"` | Count threshold met by owned rows only | |
| Restructured generated page: two populations, evidence kinds, claims table, all rows | | ✓ |
| One disclaimer + `policy.claims_wording` + README shrink | | ✓ |
| No `.info` key changes; description-length check | | ✓ |

**Choice:** as selected. Advisor suggested collapsing unknown softlist rows to counts; the softlist advisor
preferred listing every row (fits the budget, answers "is my game listed?") — every row is listed.

## Claude's Discretion

File split, non-fixed names, page layout, plan/wave decomposition, how the CRC binding hooks into diag
tests, the identity seam for the `library.local` fixture.

## Deferred Ideas

Movie-driven independent-consumer checks; `is_experimental` flip criterion; a locally-observed doc
section; ratchet protection on push events. Four matched CI/core todos reviewed and not folded.
