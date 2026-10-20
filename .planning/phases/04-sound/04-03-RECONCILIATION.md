# TIA feedback reconciliation

Investigated 2026-10-20 inside the existing execution checkpoint. This is a
documentation-derived functional result, not a physical-chip measurement.
No emulator implementation source, including TIA.sv, was opened or reused.

## Finding

**Retain the documented feedback recurrence. Correct the assertion that the
old aggregate table describes an all-zero start.** The disputed modes have
two reachable steady output attractors with different duty counts. Both the
old counts and the checkpoint probe's counts are valid under that same
recurrence, but for different joint register states. [VERIFIED: original
schematic and exhaustive owned derivation below]

The locked startup says exactly “Deterministic startup: zero TIA audio
logic”; its next sentence qualifies this as a compatibility default.
[VERIFIED: .planning/phases/04-sound/04-CONTEXT.md:35-39] For that zero state,
the recurrence gives mode6/7/14/15 ones **18/16/49/47**, and mode11 reaches
constant high at its **12th** enabled step. The preparation instead records
“13 / 18”, “15 / 16”, “44”, “46”, and “10 for 11”.
[VERIFIED: .planning/preparation/A7800-AUDIO.md:47-56; owned derivation]

One explicit alternative initial state, N=21 and P=0, gives **13/15/44/46**
and mode11 fill10, reproducing every disputed aggregate without changing any
equation. This is a counterexample to unique aggregate counts, **not evidence
that the lost earlier simulation actually used N=21**. Its seed/history
remains unknown. [VERIFIED: exhaustive owned derivation]

## Documentary basis and recurrence

Primary source: Atari **TIA-1A Logic Diagram, revision E, sheet4 of5**,
“AUDIO NOISE GEN.” and “AUDIO OUTPUT”, archived at
[AtariAge](https://www.atariage.com/2600/archives/schematics_tia/).
The [original scan archive](https://www.atariage.com/2600/archives/schematics_tia/TIA_1A_400dpi_4.zip)
was downloaded and its audio area inspected at readable resolution. The JPG
SHA-256 is `03df74daa22654a0cc5d214e3b64b4da710db249564780ea7df4284b5a8d159b`.
The five D1 blocks occupy drawing columns4–3, the four pulse flip-flops
columns2–1, and the volume-controlled output column1. [VERIFIED: scan]

The independent
[schematic analysis](https://github.com/jigo2600/jigo2600/blob/f454593beedf763fb8ab92fc69744774086831ed/doc/TIA_Sound.md#audio-generator)
describes the same hardware equations in its “Poly5 value update”, “Poly4
clock ACLK”, and “Poly4 value update” sections. Only this documentation was
consulted, never its implementation. [CITED: linked hardware analysis]

Define N bits0–4 as the five noise stages in physical shift order, P bits0–3
as the four pulse stages, and output=P3. For fixed AUDC and an enabled
two-phase generator cycle, use old N and P on every right-hand side:

```text
L = AUDC & 3
H = AUDC >> 2

noise = 1                                      if AUDC == 0
        (N4 XOR (L != 0 ? N2 : P3))
          OR (N == 0 AND (L != 0 OR P == 0))    otherwise

gate = 1                                       if L == 0 or L == 1
       N == 8 or N == 24                        if L == 2
       N4                                      if L == 3

pulse = 1                                      if AUDC == 0
        (P2 XOR P3) OR P == 0                   otherwise if H == 0
        NOT P0                                 if H == 1
        N4                                     if H == 2
        (P & 7) in {0,1,2,3,5}                  if H == 3

N_next = ((N << 1) | noise) & 31
P_next = gate ? ((P << 1) | pulse) & 15 : P
output_next = P_next >> 3
```

These are an owned Boolean transcription of the cited sheet, independently
checked against the documentary analysis. The N4 value used by pulse input
is the old stage value, not N_next4. The ACLK condition has a phase1 latch
before the phase2 output clock. Capture noise feedback and the gate for the
enabled pair; commit shifts on its output phase without feeding the newly
shifted N back into that same pair. AUDC writes between phases need the
documented split capture/output timing; the fixed-mode orbit calculation
does not independently establish every asynchronous register-write race.
[VERIFIED: inspected sheet4 paths; CITED: hardware analysis sections above]

## Why the counts differ

Exhaustively following all512 joint N/P starts for each mode gives the
following steady observable cycles. Counts refer to **one complete minimal
output period**, after the finite state transient; they are not counts over
an arbitrary startup window. [VERIFIED: owned derivation]

| AUDC | Period | Ones from N=P=0 | Other reachable ones | Starts in each basin |
|---|---:|---:|---:|---:|
| 6 | 31 | 18 | 13 | 256 / 256 |
| 7 | 31 | 16 | 15 | 256 / 256 |
| 14 | 93 | 49 | 44 | 256 / 256 |
| 15 | 93 | 47 | 46 | 256 / 256 |

Mode6 toggles on the two decoded N states in each31-cycle N orbit; its
relative P phase persists. Mode7 toggles on16 N clocks per31-cycle orbit,
again retaining relative phase. The divide6 pulse feedback in modes14/15
retains the analogous phase distinction over93 clocks. Modes9 and10 have
only16 and18 ones respectively across all512 starts. Consequently, mode6
is **not unconditionally the complement of mode10**, nor mode7 of mode9.
[VERIFIED: schematic functions and exhaustive orbit enumeration]

The discrepancy is therefore **state/phase-dependent steady attractors**,
plus mode11's state-dependent fill transient. It is not fixed by waiting
longer from the same zero seed. Sampling at a different point within a
complete settled period cannot change the ones count. A global active-low
output inversion would also change modes1/8 and gated poly4 modes2/3,
which already agree, so it cannot selectively reconcile this table.
[VERIFIED: derived cycle invariance and enumeration]

## Zero-start vectors

Sample P3 immediately after each enabled output phase, beginning with the
first complete enabled A/B pair; start N=P=0, AUDF=0 and hold AUDC fixed.
All rows below are independently derived facts, not copied tables.
[VERIFIED: owned array-stage derivation, cross-checked over8192 transitions]

| AUDC | First40 outputs | Steady period | Ones |
|---|---|---:|---:|
| 0 | `0001111111111111111111111111111111111111` | 1 | 1 |
| 1 | `0001001101011110001001101011110001001101` | 15 | 8 |
| 2 | `0000000000000000000000000000000000000000` | 465 | 248 |
| 3 | `0000000000011100110000100111111000000000` | 465 | 248 |
| 4 | `0001010101010101010101010101010101010101` | 2 | 1 |
| 5 | `0001010101010101010101010101010101010101` | 2 | 1 |
| 6 | `0000000000000000000000000000000000000000` | 31 | 18 |
| 7 | `0000000000011101010000100101100111110001` | 31 | 16 |
| 8 | `0000000010000100011000010011100101010110` | 511 | 256 |
| 9 | `0000000010010110011111000110111010100001` | 31 | 16 |
| 10 | `0000000000000000000000000000000000000000` | 31 | 18 |
| 11 | `0000000000011111111111111111111111111111` | 1 | 1 |
| 12 | `0001110001110001110001110001110001110001` | 6 | 3 |
| 13 | `0001110001110001110001110001110001110001` | 6 | 3 |
| 14 | `0000000000000000000000000000000000000000` | 93 | 49 |
| 15 | `0000000000011111000000111100000111111111` | 93 | 47 |

Zero-start mode11's P-shift events occur at steps6,9,11,12, inserting one
each time. P consequently takes1,3,7,15 and output first becomes high at12.
This directly explains the fill latency rather than fitting an exception.
Mode0 fills at4. [VERIFIED: explicit stage trace in owned derivation]

## Execution recommendation and evidence

Amend preparation and plan together before changing the adopted assertions:
state the zero-start counts, record both reachable attractors, and remove
unconditional complement claims. Keep zero startup and the documented
recurrence. Add explanatory alternate-state coverage using N=21/P=0 through
the existing internal test seam if proportional. Physical-chip confirmation
remains confidence-qualified under the existing D-07; it is not needed to
resolve this mathematical inconsistency or assigned to the owner.
[VERIFIED: locked D-03/D-07, .planning/phases/04-sound/04-CONTEXT.md:35-39,55-58;
owned derivation]

Command: `python3 /tmp/muhtari-tia-reconcile.py`; exit status **0**. It uses
an independently written nine-stage array transition, compares all8192
mode/state transitions with the checkpoint's owned bit-packed transcription,
enumerates every joint-state orbit, and calculates the vectors above.
`python3 /tmp/muhtari-tia-derive.py`; exit status **0**. No hosted run was
started by this source investigation; no execution log is committed.

The production code, tests, STATE and existing plans were not changed by
this investigation. The temporary probe is mathematical evidence, not a
hardware oracle or a production seed.
