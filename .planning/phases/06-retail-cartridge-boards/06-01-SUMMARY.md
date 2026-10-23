---
phase: 06-retail-cartridge-boards
plan: "01"
status: halted
halted: true
tasks_completed: 0
subsystem: testing
tags: [cmake, quality, measured-fit, designed-stop]
requires:
  - phase: 05-pipeline-tune-up
    provides: Accepted 15287-line audited starting tree and independent quality authorities
provides:
  - Reproduced reviewed baseline and a measured failing disposable integration construction
  - Executed admission, trusted-manifest and host-invocation construction controls
  - Explicit stop before dependent board behavior or policy installation
affects: [06-02, 06-03, 06-04, 06-05, 06-06, 06-07, 06-08, 06-09]
tech-stack:
  added: []
  patterns: [measure the complete counted construction before expansion]
key-files:
  created:
    - .planning/phases/06-retail-cartridge-boards/06-01-SUMMARY.md
  modified: []
key-decisions:
  - Preserve the fixed 192-line allowance and stop dependent expansion after measured construction exceeds it.
  - Keep all candidate code disposable; install no partial admission or board behavior.
requirements-completed: []
requirements-contributed: []
coverage:
  - id: reviewed-baseline
    description: Independent checkout reproduces the reviewed 15287-line baseline
    requirement: BOARD-01
    verification:
      - kind: other
        ref: cmake -DROOT=/tmp/muhtari-phase06-baseline -DINVENTORY=tests/policy/gate-lines.txt -DOUT=/tmp/muhtari-phase06-base-count.json -P tests/cmake/gate_count.cmake; exit0; gate_lines15287; URL N/A-local
        status: pass
    human_judgment: false
  - id: construction-controls
    description: Disposable admission, additive-manifest, toolchain/sample and invocation controls execute
    requirement: BOARD-02
    verification:
      - kind: integration
        ref: In disposable fit tree, cmake --preset ci and ctest --preset ci -R '^policy[.]quality[.]controls$' --no-tests=error --output-on-failure; exit0/0; 1/1; URL N/A-local
        status: pass
    human_judgment: false
  - id: fixed-fit-stop
    description: Complete proposed counted wiring exceeds the fixed ceiling and blocks expansion
    requirement: BOARD-03
    verification:
      - kind: other
        ref: Fit gate_count exit0 observes15952; jq -e '.gate_lines <= 15479' /tmp/muhtari-phase06-fit-count-final.json exits1; excess473; URL N/A-local
        status: fail
    human_judgment: false
  - id: unchanged-local-byte-bound
    description: Installed baseline packages fit the unchanged byte bound; future six-native board fit is unproved
    requirement: BOARD-03
    verification:
      - kind: integration
        ref: cmake --build --preset ci --target package and ctest --preset ci-release --output-on-failure; exit0/0; 6/6; exactly3 ZIP sum212216 <=399937 exits0; URL N/A-local
        status: pass
      - kind: integration
        ref: Future board implementation and its six native package lanes were not started; no hosted run exists
        status: unknown
    human_judgment: false
actuals:
  tokens: 0
  tasks: 0
  commits: 0
plan_head_before: e774361c547bab164e41ed62cd5bc958fa443f05
plan_head_after: e774361c547bab164e41ed62cd5bc958fa443f05
duration: 16min
started: 2026-10-23T16:41:01Z
completed: 2026-10-23
---

# Phase 6 Plan 1: Counted admission and integration fit Summary

**A disposable, executable counted construction measures15952 against15479; the designed stop leaves installed code/policy unchanged and blocks02–09.**

Task1 is incomplete because its fit criterion fails. Task2 was not started.
No board behavior, admission, manifest relation, API change or user documentation
was installed. No BOARD requirement is complete. The pre-summary task diff is
empty, so actual implementation chars/4 and the persisted task-commit ledger
both measure0. These actuals exclude this halt record and subsequent state
metadata; they do not describe effort or harness tokens.

## Measured construction and fixed allocation

The independent reviewed checkout at617dfa762ddb649839b925be5aa582180066fa78
measures B15287. S remains0. N remains192 and C remains15479. No baseline
funding, dated exception, replenished savings or byte grant was used.
The installed tree also measures15287; its quality.json ceilings remain
gate_lines15287 and archive_bytes399937. Only the disposable candidate sets
gate_lines15479 and the literal provenance
`retail-cartridge-boards@617dfa762ddb649839b925be5aa582180066fa78:B15287:S0:N192:C15479`.

| Bucket | Original maximum | Visible redistribution | Construction actual | Unfunded |
|---|---:|---:|---:|---:|
| Semantic/diagnostic/fuzz/API registrations |48|34|34|0|
| Runner/libretro invocation contracts and controls |40|54|274|220|
| Manifest, inventories and claim glue |32|32|139|107|
| Narrow admission and its controls |40|40|95|55|
| Integration reserve: trusted-manifest comparison and controls |32|32|123|91|
| Total |192|192|665|473|

The only redistribution moves14 unused registration lines to invocation
contracts. It supplies no additional allowance; the remaining473 are an overrun.
Registration actual34 is ABI8 + diagnostic12 + testdb module/resolver linkage14.
Invocation actual274 is retail helper102 + runner19 + hash caller7 + invocation
controls146. Manifest actual139 is expected.csv125 + manifest3 + exact
inventories9 + gate inventory2. Admission95 is judge13 + controls82.
Reserve123 is manifest helper40 + producer7 + fixture controls61 + helper
dispatch7 + toolchain/sample controls8. All comments, blanks and shell glue
are included; the original counter is unchanged.

| Counted file | Baseline counted | Candidate counted | Net |
|---|---:|---:|---:|
| tests/abi/tests.cmake |135|143|8|
| tests/cmake/hashes.cmake |72|79|7|
| tests/cmake/quality.cmake |236|289|53|
| tests/cmake/quality_evidence.cmake |170|177|7|
| tests/diag/expected.csv |0|125|125|
| tests/diag/retail.cmake |0|102|102|
| tests/diag/tests.cmake |119|131|12|
| tests/inventory/ci.txt |618|622|4|
| tests/inventory/nofp.txt |15|16|1|
| tests/inventory/wasm32.txt |600|604|4|
| tests/libretro/tests.cmake |156|170|14|
| tests/policy/gate-lines.txt |149|151|2|
| tests/policy/quality.cmake |217|521|304|
| tests/roms/manifest.txt |31|34|3|
| tests/runner/tests.cmake |840|859|19|
| All other inventory files | unchanged | unchanged |0|
| Complete total |15287|15952|665|

The full125-line expected.csv is newly counted, including73 inherited rows and
52 concrete numeric tagged-bank/alias expectations. No inherited row becomes a
saving. tests/cart/tests.cmake and tests/fuzz/tests.cmake retain their physical
lengths while registering cart.retail.boards and changing run max_len to131200;
the loader cap and both60-second fuzz durations stay unchanged. API4, diag.retail,
cart.retail.boards and runner.probe.retail have exact native/WASM inventory rows;
the existing nofp API inventory also gains API4. Native configuration verifies
the complete registered inventory. No new lane is introduced.
ci.yml and be-equality.yml need no change: their existing workflow/CTest callers
already consume runner.hashes_file and its same hashes.txt artifact. The actual
registered hash invocation now passes the tracked root and diagnostic driver;
WASM passes its existing emulator and omits dynamic module loading. Existing
join, compatibility and performance authorities remain unchanged.

## Executed controls and limits of the evidence

The candidate preserves the original policy controls, including initial adoption,
calendar/expiry/lifetime/from/to/renewal, missing evidence, SHA/source/identity,
availability, independent-base and worst-lane regression checks. Eighteen added
admission cases cover cap/final tightening, excess, wrong base ceiling/count,
missing/forged provenance, measured regression, unavailable evidence, wrong SHA
and configuration, another metric, simultaneous archive loosening, current/old
exception abuse, missing base and malformed/missing numeric values.

Twelve manifest cases cover unchanged/all-three additions/later base, changed
old and changed already-admitted rows, deletion, duplicate old/new paths,
unrelated paths, license, digest and comment edits. The helper removes only
unique admitted added rows, requires the remaining raw text to equal the trusted
manifest byte-for-byte, and derives identity from that trusted file. Compiler,
platform and built-sample identity remain in the producer environment; separate
compiler-change/sample-change cases fail the unchanged exact join equality.

Fifteen invocation cases exercise the proposed actual CMake/POSIX calls with
controlled observations: all three host paths, the real hash-writer contract,
a positive raw/orientation probe, absent fixture, wrong board, absent/extra/
truncated report, invalid mailbox bytes, missing frame/audio fields, module
failure, missing orientation/assumption output and dropped original hashes.
The host case requires12 calls: three synthetic-tracer paths plus nine admitted
fixture paths. The hash case requires all three named diagnostic records.
The probe checks independent original full/headerless digests and truthful bits.

This proves invocation/control construction only. The disposable three added
fixture files deliberately copy the existing owned linear16 fixture and have
`controlled-fit-only` pins. They are not new board diagnostics. Intended numeric
expectations and the future API4 client exist in the fit tree, but new assembly,
board behavior, actual loaded resolver equivalence and full product CI have not
been implemented or accepted. Product C tests/owned fixture bytes retain their
existing classification; no check implementation is concealed in them.
Full fit remains failed/unproved; none of these controlled observations resolves
actual board semantics or supplies a passing future six-native package result.

The original complete candidate measured15938. A scoped reduction removed
redundant trusted-path collection, retaining exact stripped-manifest equality;
with a specific changed-later-base-row case it saved9 lines, measuring15929.
Completing original-headerless/flag checks, the positive probe and distinct
compiler/sample negatives then added23, yielding15952. No line golf, inherited
control removal or density change was used. This failed candidate is not a
proof that no implementation can fit192; it is concrete new replanning evidence.

## Commands, exits and run URLs

All commands below are local; run URL is N/A. No logs are committed.

| Command | Exit | Observation |
|---|---:|---|
| `cmake -DROOT=/tmp/muhtari-phase06-baseline -DINVENTORY=tests/policy/gate-lines.txt -DOUT=/tmp/muhtari-phase06-base-count.json -P tests/cmake/gate_count.cmake` |0| Reviewed baseline15287; full/authored source8580/6533 |
| In fit tree: `cmake --preset ci` |0| Complete native registration/inventory construction configures |
| In fit tree: `ctest --preset ci -R '^policy[.]quality[.]controls$' --no-tests=error --output-on-failure` |0|1/1; original and added controls execute |
| In fit tree: `cmake -DROOT=. -DINVENTORY=tests/policy/gate-lines.txt -DOUT=/tmp/muhtari-phase06-fit-count-final.json -P tests/cmake/gate_count.cmake` |0|15952; whole affected files/control text included |
| `jq -e '.gate_lines <= 15479' /tmp/muhtari-phase06-fit-count-final.json` |1| Explicit false; overrun473 |
| Installed tree: `cmake --preset ci && ctest --preset ci -R '^policy[.]quality[.]controls$' --no-tests=error --output-on-failure` |0|1/1 unchanged installed controls |
| Installed tree: `cmake -DROOT=. -DINVENTORY=tests/policy/gate-lines.txt -DOUT=build/ci/gate-count.json -P tests/cmake/gate_count.cmake` |0| Installed actual15287 |
| Installed tree: `cmake --build --preset ci --target package && ctest --preset ci-release --output-on-failure` |0| Three fresh component ZIPs;6/6 release checks |
| Exactly-three ZIP shell expansion, `wc -c` sum and `[ "$bytes" -le 399937 ]` |0| library68292 + libretro67400 + runner76524 =212216 |
| `git diff --check` before halt metadata |0| No installed task diff |

The bounded shell command is the plan/research command: expand
`build/ci/muhtari-*.zip`, require `$#` equal3, sum `wc -c` over those exact files,
print the total, then compare against399937. This macOS arm64 baseline result
does not establish any future board build or the six-platform maximum.

## Task commits and deviations

No task commit exists: Task1 failed its designed fit gate, so a partial admission
was not installed or committed. This SUMMARY is the canonical committed halt
record; state/roadmap updates follow it. No task/requirement completion is claimed.
The configured hygiene hooks and GitHub noreply identity remain enabled.

No scope/budget deviation was taken. During disposable configuration, sorted
inventory order and canonical label order were corrected before its successful
configuration. The nofp API row is necessary existing-inventory glue and is
charged explicitly. No review-fix or gap round was consumed; no authentication
gate, new dependency or security-relevant production surface was introduced.

## Known Stubs and deferred verification

No stub was installed. Disposable controlled observations are explicitly labeled
above and cannot count as behavior acceptance. Task2's full CI verification and
all dependent product/hosted checks were not run because Task1 failed. There is
no skip marker or safety-control weakening. A1 remains failed/unproved for this
construction; A2 remains unproved for future six-native output. Descriptorless
BOARD-01/02/03 probe/prohibition metadata remains unresolved and creates no owner
verification task.

## Next execution boundary

Plans02–09 are blocked transitively by halted01. The disposable tree remains at
`/tmp/muhtari-phase06-fit`; its companion instructions are
`/tmp/muhtari-phase06-fit-README.md`. Counter JSONs and unreduced helper/control
snapshots remain under `/tmp/muhtari-phase06-*`. They are local evidence, not
committed logs or a production change.

Next command: `$gsd-plan-phase 6`, using this concrete failed construction as new
evidence to redesign within the existing192. Any larger permanent allocation
requires a separate explicit scope decision; this halt record grants none.
Do not resume02 or mark01 complete from the existence of this SUMMARY.

## Self-Check: PASSED

The canonical halt record exists; the independent baseline and both measured
candidate stages exist; the final controls/configuration execute as recorded.
The installed task diff is empty and the persisted pre-summary ledger measures0
task commits. This self-check validates the halt record, not plan completion.
