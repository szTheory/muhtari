# TIA feedback model discrepancy

**Status: resolved, 2026-10-20.** The following original checkpoint is
retained as history. `04-03-RECONCILIATION.md` independently derives the
primary schematic recurrence and explains every aggregate discrepancy as
joint-state attractors and state-dependent fill. Acceptance/preparation
were amended in `c51da68` before production changes. Task2 GREEN is
`3872209`: all16 zero-start vectors,64 mode/divider period/count cases,
alternate states, wrap, A/B write timing, hidden-state and RSYNC/WSYNC
checks pass. Actual TIA spectral sweeps pass; the source-call probe verifies
ordering and8192 transitions. `cmake --workflow --preset ci` exits0 with590
ordinary and6 release tests passing. No owner verification is pending.
Full commands and physical/model limits are in `04-03-SUMMARY.md`.

## Original checkpoint

Task 1 has a committed, verified bus → TIA AUDC4 → production synth tracer.
Task 2 has a committed failing test with documentary period/ones assertions.
The full mode implementation is paused before changing production feedback
logic. No phase or plan completion is claimed.

## Evidence

The preparation's §2 names period/ones results from a prior simulation, but
neither its recurrence nor its first-40 startup vectors are recorded. The
required implementation cannot be recovered uniquely from aggregate counts.

Consulted documentation, without opening emulator implementation source:

- Original Atari TIA-1A logic diagram, sheet 4, audio circuits, revision E:
  https://www.atariage.com/2600/archives/schematics_tia/
- Hardware analysis of that schematic:
  https://github.com/jigo2600/jigo2600/blob/master/doc/TIA_Sound.md
- Independent programmer documentation, which explicitly labels several
  mechanisms as uncertain:
  https://problemkaputt.de/2k6specs.htm

The published schematic analysis describes a five-stage shift register N and
four-stage shift register P, bit 0 being each register's first stage, bit 3 of
P its output. For L = AUDC & 3 it gives these hardware equations:

- Noise insertion: 1 for AUDC0; otherwise XOR(N4, L ? N2 : P3), OR
  the escape condition N == 0 AND (L != 0 OR P == 0).
- Pulse gate: true for L0/1; N == 8 or 24 for L2; N4 for L3.
- Pulse insertion: 1 for AUDC0; XOR(P2,P3) OR P == 0 for group0;
  NOT P0 for group1; N4 for group2; low three P bits in
  {0,1,2,3,5} for group3.
- Shift N left, inserting noise; shift P left on the gate, inserting pulse.

An owned mathematical transcription evaluates all right-hand sides from the
old state. It starts both registers at zero and samples P3 after each step.
It gives every tabulated period and these disagreements after 1024 clocks:

| AUDC | Adopted preparation ones | Documented-equation ones | Period |
|---|---:|---:|---:|
| 6 | 13 | 18 | 31 |
| 7 | 15 | 16 | 31 |
| 14 | 44 | 49 | 93 |
| 15 | 46 | 47 | 93 |

The same equations fill AUDC11 high at step12, while preparation says step10.
AUDC0 fills high at step4, matching preparation. The other mode ones counts
match. Computing the pulse insertion from the newly shifted N instead breaks
the poly9 period and other modes; it is not a supported correction. Merely
selecting a different output bit or polarity to fit counts is insufficient
evidence and was not applied to production.

This is a discrepancy between an owned transcription of published equations
and the adopted table, not a claim that either is measured hardware truth.
The original schematic's active-low and two-phase conventions still need a
complete independent reconciliation. No source-backed unique correction has
been established. Preserve the adopted assertions until that reconciliation
provides evidence for retaining or amending them under the confidence policy.

## Commands and exit statuses

- `cmake --workflow --preset ci`: 0 after Task 1 implementation.
- `ctest --test-dir build/ci -R '^core.audio.silence$|^bus.rdy' --output-on-failure --no-tests=error`: 0 after Task 1; three selected tests passed.
- `ctest --test-dir build/ci -R '^core.audio.silence$' --output-junit tia-modes-red.xml --output-on-failure --no-tests=error`: 8 after Task 2 tests; unsupported modes fail the expected ones assertions.
- `node .../gsd-tools.cjs check tdd-red-evidence build/ci/tia-modes-red.json --raw`: 0, RED_EVIDENCE_OK. The actual JUnit report records the intended target failure.
- `python3 /tmp/muhtari-tia-derive.py`: 0 for the isolated mathematical transcription. This temporary probe is not a production implementation or a hardware oracle.

No hosted run was started by this plan executor. No execution log is committed.

## Continuation

Completed commits: `1aa96d9` (Task 1 RED), `2060afa` (Task 1 GREEN),
`8f2cb44` (Task 2 RED). The current branch deliberately contains the failing
Task 2 assertions. Do not mark it green or begin dependent plans. Task 2 still
needs the complete feedback/state machine, independent startup vectors,
RSYNC/WSYNC cases, actual-TIA spectral sweep and graduated hardware facts.
The public frame path still emits silence until the dependent integration plan.

Resolve the recurrence from permitted hardware documentation, then resume
Task 2 without repeating Task 1 or discarding its commits. If research changes
the adopted expectations, document its exact evidence and amend the plan and
preparation together before implementing those new expectations.
