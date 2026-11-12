# Candidate digital YM mono gain and unchanged filter bounds

**Status:** Independent source/arithmetic proof for Phase 20, not implemented
behavior, runtime measurement or analog calibration. No production files changed.
**Evidence:** python3 build/v6-ym-mixer-bound.py — exit 0, N/A-local. The ignored
script/JSON receipt enumerates each component's full integer domain and checks
the exact inequalities below. Source: kickoff-equivalent current synth/state,
existing NTSC/PAL numeric tests, and the approved exact-pinned OPM DAC helper.

## Recommended candidate

Let B be the UNCHANGED current TIA/POKEY normalized integer level in [0,18360].
For a declared, installed YM topology only, use:

```
A = trunc_toward_zero((L + R) / 2)
Y = trunc_toward_zero((A * 4590) / 32768)
level = floor(B / 2) + Y
```

L/R are the post-YM3012-quantization signed DAC samples, each in
[-32768,32767], not unbounded channel/operator sums. The pinned
ymfm_opm.cpp generate path applies output.roundtrip_fp(); ymfm.h roundtrip_fp
clamps its input to that signed domain and returns int16_t. The C port must
prove the same output domain and exact quantization with its genuine oracle.
Private observer words can retain the two's-complement uint16 bit encoding;
decode to bounded signed int32 before arithmetic.

This is a deliberately declared digital gain: existing node audio is attenuated
by one half and a full-scale YM mono mean contributes about 4590 units. It is
not a measured XM resistor/gain fit. Apply it based on installed YM topology,
not current D7 decode enable, so register gating does not toggle the gain.
When YM is absent, execute the old calculation unmodified. At reset with all
sources zero the candidate level is exactly zero; no DC bias is introduced.

C17 positive B/2 and signed divisions above have defined truncation toward
zero. L+R is in [-65536,65534], and A*4590 is in
[-150405120,150400530], strictly inside int32. No negative signed shift,
implementation-defined uint16-to-int16 cast or floating-point core operation is
needed. Integer-to-unsigned conversion can encode the observer bits modulo
65536; signed decoding can explicitly subtract 65536 in int32 when D15 is set.

## Complete domain and reconstruction proof

Both component maps are monotone. Exhaustive B=0..18360 gives floor(B/2) in
[0,9180]; exhaustive A=-32768..32767 gives Y in [-4590,4589]. Every A is
reachable in the conservative stereo domain by L=R=A, so no Cartesian four-
billion-pair enumeration is needed. Adding their independent bounds gives:

- Lower level l = -4590; upper level u = 13769.
- Width W = u-l = 18359, strictly less than the old M=18360.
- Initial level 0 is inside the interval; every transition magnitude is at most W.

Existing docs/hardware/audio.md and the independently enumerated tests establish
Q=8388608 and per-alignment signed reconstruction weights with positive mass P,
negative magnitude N and P-N=Q. For ANY source sequence in [l,u], summation by
parts gives the Q23 integrator interval:

```
[l*P - u*N, u*P - l*N]
```

The DC blocker has positive and negative impulse masses each one, hence its
PCM magnitude bound depends on the interval WIDTH, not its offset:

```
peak <= W*(P+N)/Q + one_PCM_LSB
```

The one-LSB allowance already exceeds the existing feedback truncation and
final rounding error. The argument includes a zero prehistory because zero is
inside [l,u]. It does not assume that the signed level is an unsigned DAC or
that a clamp proves headroom.

| Quantity | NTSC | PAL |
|---|---:|---:|
| P | 10865505 | 10865492 |
| N | 2476897 | 2476884 |
| Q23 integrator lower | -83977062743 | -83976824076 |
| Q23 integrator upper | 160976095575 | 160975856908 |
| Maximum previous-Q30 magnitude | 20604940233600 | 20604909684224 |
| PCM bound including one LSB | 29201.691976309 | 29201.635073662 |

Both complete peaks are below the EXISTING conservative 29203.6 guard and
-1 dBFS threshold. Both integrator absolute endpoints are below old P*M and
2^38; multiplying by 128 remains below 2^45. Blocker state is below 2^45 Q30;
subtraction/feedback sum remains below the existing 2^47 pre-cancellation bound.
The unchanged split quotient/remainder pole multiply retains remainder-product
bound (2^30-1)*1070934451 < 2^60. No existing state/filter numeric limit needs
to rise for this candidate. YM-specific mixed-level validation can use the
actual [l,u] domain; unchanged non-YM serialization and validation stay untouched.

References: src/synth.c flush/sample/multiply_q30,
src/internal.h synthesis constants, src/state.c state limits,
tests/core/test_core.c signed masses and tests/core/test_pal.c signed masses.
This uses their published numeric results, not an independent re-enumeration
of every production interpolation coefficient in the new topology.

## Ring cells, event density and ownership

Adding YM raw sources need not add multiple MIXED deltas at one master tick:
retain the current pending_tick coalescing and stable event-before-sample order.
All events have integer MARIA ticks. With the unchanged public 48 kHz sample
lattice, NTSC intervals have at most 150 master ticks and PAL at most 148.
Therefore the existing conservative 150 mixed deltas per output interval still
applies, even when several chip/source changes coincide at a tick.

Using Cmax=7283993 (the larger PAL coefficient bound), 16 ring intervals and W:

```
ring_cell <= 16*150*18359*7283993 = 320944385968800
          < old limit 320961867552000 < 2^49
```

A single delta product remains below 2^37, and the integrator-plus-cell
intermediate remains below 2^50. The original positive mixer current calculation
and its products are unchanged before the final conditional combination.

This is NOT a proof that the RAW observer's ten-source capacity suffices.
Its uint8 raw/cache/pending/carry levels must retain full YM words, and its
raw-event count/memory/ordering need the separately derived YM-qualified bound.
Do not flush the mixed synthesis separately for left/right at the same tick;
that would invalidate the one-mixed-delta-per-tick argument. Public PCM cadence,
frame overshoot and FIFO extent remain tied to existing master-time sampling,
not the number of YM source events. Any alternate internal test sample lattice
needs its own interval/coefficient bounds; this proof claims the public 48 kHz
NTSC/PAL contract only.

## Required implementation confirmation

The arithmetic proof justifies choosing this candidate without an analog-gain
or owner-scope question. It does not prove the future C mixer uses it correctly.
Before accepting implementation, exercise both DAC polarities, all old B
endpoints, panning/anti-phase cancellation, simultaneous chip changes, actual
ring/DC/PCM/pre-clip/clipped-count paths, both regions and partial-frame carry.
The sum/scale divisions and signed raw-word decode must match this stated rule.
Use independent reference arithmetic in the recurring C tests, not the production
helper itself. Preserve old no-YM golden PCM/transition/state/identity bytes.

Canonical YM restores need all new source/filter fields and valid chronology;
this gain proof is not permission to omit semantic validation, enlarge unrelated
limits or accept malformed filter state. The headroom argument covers reset and
legal generated histories (including their exact canonical continuation), not a
claim that every arbitrary repaired-CRC payload is physically reachable.

Exact-seed output equivalence remains independent of chosen mono gain: compare
both signed chip outputs before the core's conditional mono mix. A different
coefficient is possible but must supply an equally complete proof and keep the
same compatibility and clipping contracts.
