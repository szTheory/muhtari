---
phase: 06-retail-cartridge-boards
status: decision-required
date: 2026-10-23
requirements: [BOARD-01, BOARD-02, BOARD-03]
---

# Bounded redesign: allocation decision required

The single selected redesign measures **15,941 counted lines**, or **654 net**
against B=15,287. It exceeds the authorized C=15,479 by **462**. The bounded
investigation stops here. This is a measured scope/budget proposal, not a ready
plan or proof that no design can fit192. No product or policy change is installed.

The historical construction and halted `06-01-SUMMARY.md` remain intact. Plans
01–09 remain blocked; no BOARD requirement is complete. Review/gap allowances
are unchanged. Approval of investigation did not authorize spending654.

## Choices and selected construction

| Choice | Assessment |
|---|---|
| Install the historical665-line construction | Complete counted wiring exists, but it repeats the grant-control dispatch and several report-parsing passes. Not selected. |
| Reuse existing control dispatch and one fixed report contract | Selected and fully constructed in a separate disposable tree. Retains all numeric rows and prior faults; measures654, including added controls. |
| Drop host/equality checks, reinterpret expectation data as free, or relocate controls into C | Would change locked scope or counting. Not constructed or claimed as a fitting alternative. |

The candidate extends the existing policy-control loop and its existing
`expect_fail.cmake` invocation rather than adding another dispatch loop. It
replaces separate mailbox/frame/audio shape passes with one explicit CMake
wire-format expression: two64-digit frame hashes, seven typed audio fields,
and one512-digit mailbox, in runner output order. The existing diagnostic driver
continues to judge numeric mailbox values. No check predicate moves into C.
All125 expectation lines remain counted, including73 inherited and52 new rows.

The new helper measures83 lines rather than102; the control file measures529
rather than521 because it includes the stronger controls below. Total saving
against the historical failed construction is11, not baseline savings: **S stays0**.
No inherited implementation was rewritten to finance additions.

## Exact allocation proposal

Recommend explicit approval of **N192→654 and C15479→15941**, solely for the
existing BOARD-01–03 integration scope. B remains15287 at
`617dfa762ddb649839b925be5aa582180066fa78`; S remains0. The installed gate
ceiling remains15287 pending a separate authorized implementation step.

| Scoped bucket | Existing maximum | Historical actual | Proposed measured allocation | Included fault/control construction |
|---|---:|---:|---:|---|
| Semantic/diagnostic/API registrations |48|34|34| ABI8 + diagnostic12 + testdb/resolver linkage14; existing fuzz bounds change without line growth |
| Runner/libretro invocation contracts and controls |40|274|267| Retail helper83 + runner19 + hash caller7 + invocation controls158 |
| Manifest, inventories and claim glue |32|139|139| Whole expected.csv125 + manifest3 + exact inventories9 + gate inventory2 |
| Narrow admission and controls |40|95|91| Judge13 + grant controls78, reusing existing dispatch |
| Integration reserve consumed by fixture comparability |32|123|123| Manifest helper40 + producer7 + fixture controls61 + helper dispatch7 + compiler/sample controls8 |
| **Total** |**192**|**665**|**654**| **C=15287−0+654=15941; no unspent reserve** |

The historical visible redistribution48/40→34/54 remains historical; it created
no additional allowance. This proposal replaces the scoped allocation only if
explicitly accepted. Later actuals must tighten the ceiling, and further growth
must stop rather than replenish this allocation.

The disposable candidate includes the prospective literal admission and its
controls in the654 count. Its exact proposed selector is
`retail-cartridge-boards@617dfa762ddb649839b925be5aa582180066fa78:B15287:S0:N654:C15941`.
It is not installed or authorized. The two additional grant controls reject the
old192-line selector at the larger bound and candidate-appended budget claims.
The unchanged **archive_bytes=399937** receives **zero additional allocation**;
performance, compatibility and exact environment equality retain their existing
independent authorities. No dated exception or initial-adoption escape funds this.

## Fault coverage retained and changed

| Boundary | Retained controls | Changed/new controls |
|---|---|---|
| Policy/evidence/admission | All31 original policy cases; all18 historical grant cases, including cap/tightening, excess, base ceiling/count, provenance, measured/type/availability/SHA/configuration, unrelated metric/archive and exception abuse | Two explicit prospective-grant cases above;20 grant cases execute through the existing dispatcher |
| Trusted fixture/environment identity | All12 manifest cases: unchanged/additions/later base; changed/deleted/duplicated rows; unrelated path/license/digest/comment; compiler/sample negatives and all10 join cases | Helper, producer and join are unchanged from the historical construction; trusted raw manifest equality is retained |
| Host invocation/report identity | All15 historical cases: three positive paths, missing fixture/wrong board, absent/extra/truncated report, bad mailbox, dropped frame/audio, failed module, orientation/assumption/raw identity | Five additional cases: short hash, duplicate audio, wrong audio frame count, nonnumeric sample count and unexpected field. The latter two strengthen the report contract; others exercise retained faults under the replacement parser.20 invocation cases execute |
| Registrations/portability | Native configure validates exact inventory; API4, retail vectors, probe, diagnostic and testdb/resolver wiring remain present | No lane/status change; existing registered hash writer is still the native/BE/WASM caller |

The counter and join are byte-identical to the reviewed baseline. The case-set
audit verifies every historical case survives. No inventory exclusion, compressed
record format, hidden glue, weakened fault or new generic framework is used.

## Measured evidence and limits

Candidate: `/tmp/muhtari-phase06-redesign`. Historical tree:
`/tmp/muhtari-phase06-fit`. Its existing README remains authoritative for the
historical construction. Commands below are local; **every run URL is N/A-local**.

| Command | Exit | Observation |
|---|---:|---|
| In candidate: `cmake --preset ci` |0| Complete native registration/inventory construction configures |
| In candidate and historical tree: `ctest --preset ci -R '^policy[.]quality[.]controls$' --no-tests=error --output-on-failure` |0 /0|1/1 in each tree; original and scoped controls execute |
| `python3 /tmp/muhtari-phase06-redesign-audit.py` |0|31 original policy cases; grant18→20, manifest12, join10, invocation15→20; whole-file net654;125 expectation rows unchanged |
| In candidate: `cmake -DROOT=. -DINVENTORY=tests/policy/gate-lines.txt -DOUT=/tmp/muhtari-phase06-redesign-count.json -P tests/cmake/gate_count.cmake` |0|15941; unmodified whole-file counter |
| `jq -e '.gate_lines <= 15479' /tmp/muhtari-phase06-redesign-count.json` |1| Authorized fit fails by462 |
| In reviewed baseline: `cmake -DROOT=. -DINVENTORY=tests/policy/gate-lines.txt -DOUT=/tmp/muhtari-phase06-redesign-baseline-count.json -P tests/cmake/gate_count.cmake` |0|15287 |
| In installed tree: `cmake -DROOT=. -DINVENTORY=tests/policy/gate-lines.txt -DOUT=/tmp/muhtari-phase06-redesign-installed-count.json -P tests/cmake/gate_count.cmake` |0|15287; installed scope/policy unchanged |

The audit script is disposable one-off evidence, not a proposed Python gate or
committed log. It inventories complete files and cross-checks the unmodified
counter; in particular it charges all125 newly inventoried expectation lines,
not merely the52 added lines.

These checks use controlled fixture/process observations. The three disposable
fixtures remain copies of the owned linear fixture with `controlled-fit-only`
pins. They do not implement these boards. Numeric assembly, actual board
semantics, loaded-host equivalence, full product CI and future six-native package
results remain **unproved/unrun**. No archive-fit claim follows from this proposal.
The prior local baseline package observation stays historical.

## Source audit and next boundary

Independent plan-checker review passed for proposal adequacy only. It reproduced
native configuration, the focused policy CTest (1/1), the preservation audit and
the unmodified counter, each exit0; the authorized-fit assertion exited1. Its
inspection confirmed retained fault mutations and verdict checks, full counting,
unchanged counter/join, and no installed scope or policy increase. Commands and
local N/A run URLs are the same reproductions above. This is not plan approval,
board acceptance or an allocation grant. No revision loop was run.

| Source | Required scope | Disposition |
|---|---|---|
| GOAL | Correct Activision/Absolute/Fractalus banking and physical RAM | Existing plans02–09 retained, blocked by01; not delivered |
| REQ | BOARD-01 identified layout equivalence and raw identity | Plans03–04,06–09 retained; incomplete |
| REQ | BOARD-02 fixed/switched/initial banks | Plans02,05–09 retained; incomplete |
| REQ | BOARD-03 physical aliases and bounded access | Plans05–09 retained; incomplete |
| RESEARCH | Exact maps/A8 refusal, resolver cap/holes, numeric three-host diagnostics, deep/fuzz bounds, equality and package acceptance | Existing coverage retained; no source feature omitted; execution remains blocked |
| CONTEXT | D-01–15 | Unchanged contracts and existing task coverage; no deferred idea added |
| CONTEXT | D-16–17 | Authorized fit fails. This exact654-line proposal awaits explicit allocation decision |
| CONTEXT | D-18 |399937 unchanged; all future six-native results remain required and unproved |

A smaller board scope would change BOARD-01–03; removing a numeric host path or
equality check would change D-13/D-15. Splitting execution alone does not remove
the shared admission/comparability obligations. No such reduction was made or
priced as a fitting alternative.

**Decision required:** approve the exact scoped N654/C15941 proposal above, or
retain N192/C15479 and leave Phase06 halted. No plan is marked ready and no
execution is launched. If approved, the next step is `$gsd-plan-phase 6` to
record the accepted allocation and prepare a successor admission plan while
preserving the historical halt; approval does not mark01 complete or authorize
automatic execution. If declined, no dependent execution is scheduled.
