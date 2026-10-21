---
phase: 05-pipeline-tune-up
status: gaps_found
scope: interrupted-05-03-recovery-only
author: gsd-planner
verifier_ran: false
source_head: d1b443506bac6d20c3f4cc0a1d6e51d87faa7249
gap_round: 1
gaps:
  - truth: The complete PowerPC build satisfies the unchanged undefined-symbol and no-writable-global ABI contracts.
    status: failed
    reason: Current-head hosted output rejects __divdi3 and four writable cartdb count symbols while both existing fault controls pass.
    artifacts:
      - path: src/synth.c
        issue: Dynamic signed 64-bit division in muhtari__synth_round is the source-level candidate for __divdi3; confirm the object reference before editing.
      - path: cmake/toolchains/ppc-linux-gnu.cmake
        issue: Confirm the sections of the four generated const count objects before applying a target-only read-only placement option.
    missing:
      - Preserve the existing rounding results using already-permitted unsigned integer operations.
      - Keep the generated integer counts in actual read-only storage under the PowerPC compiler.
      - Prove the real archive passes the unchanged ABI judges and both fault controls still reject their intended faults.
  - truth: One immutable current commit completes both native and full PowerPC normal and package inventories and exactly two-file hash equality through the existing read-only release/nightly producer.
    status: unverified
    reason: The cancelled run completed native 607 normal and six archive tests but stopped before complete PowerPC package and equality proof.
    artifacts:
      - path: .github/workflows/be-equality.yml
        issue: Existing full-inventory producer has no completed successful PowerPC execution at the interrupted head.
      - path: .github/workflows/release.yml
        issue: Read-only equality permission admission is proven; complete release-called equality is still unproved.
    missing:
      - Successful current immutable-SHA full native and PowerPC inventories with exact non-release and release JUnit judges.
      - Successful identity-bound exactly two-file comparison and retained negative controls.
      - Current-head hosted URL, command, exit status and honest successful timing cohort; cancellation is not a success sample.
---

# Interrupted-plan recovery input

This is a planner-authored extraction of the two narrow gaps in
`05-03-HALTED.md` and `05-03-SUMMARY.md`, not a phase verifier report.
No phase-wide verification ran. All outcomes outside interrupted 05-03 remain
pending their existing plans and later phase verification; no requirement is
declared complete here. Historical halt and partial-summary evidence is retained.

Authoritative direct evidence is
[run 39652860054](https://github.com/szTheory/muhtari/actions/runs/39652860054),
job 119405429568, at d1b443506bac6d20c3f4cc0a1d6e51d87faa7249.
`gh api --allow-escape-sequences repos/szTheory/muhtari/actions/jobs/119405429568/logs`
returned exit 0 and reported `abi.undefined` rejecting `__divdi3` and
`abi.globals` rejecting writable `D` symbols
`muhtari__cartdb_2600_prefix_count`, `muhtari__cartdb_7800_entry_count`,
`muhtari__cartdb_7800_key_count`, and `muhtari__cartdb_7800_layout_count`.
Both existing controls passed. The run was cancelled after 6m35s: its output
is diagnostic evidence, not a green result, complete failed JUnit inventory,
or successful timeout-sizing sample. Raw logs remain outside git.

Recovery is 05-10, replacing only the uncompleted acceptance work of 05-03.
The original PLAN is explicitly superseded for scheduling; its halted SUMMARY
and HALTED record remain unchanged. Plans 05-04, 05-07 and 05-09 depend on
05-10 instead of 05-03, so no historical status is rewritten as success.
