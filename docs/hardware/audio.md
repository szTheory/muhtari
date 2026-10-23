# Audio reconstruction

The internal synthesis identity is `exact-q23-16x64-dc20`. The output is mono
signed int16 at exactly 48,000/1 Hz. One master tick advances the output phase
by 88 modulo 13,125. Events at tick t use that tick's phase before advancing
the interval [t,t+1); equal-tick raw source changes are merged before adding
one mixed delta. Rendering drains the logical half-open interval, and NULL
discards samples while retaining identical history. The bus merges actual TIA
and cartridge POKEY events on this timeline; no-content public frames retain
their silent test card.

The interpolation and 16-slot int64 impulse ring follow the approved MIT
sibling at `9f9386c661f6`, `src/synth.c`. The historical Q15 coefficients are
replaced by regenerated Q23 coefficients under the dated precision amendment
in [sound research](../../.planning/milestones/v1.0-phases/04-sound/04-RESEARCH.md),
the dated coefficient-precision amendment. No NES filters or
NES mixer tables are retained.

For phase p=0..63 and tap j=0..15, integrate
`sinc(x) I0(8 sqrt(1-(x/7.5)^2))/I0(8)` over the intersection of
`[j-7-p/64-0.5,j-7-p/64+0.5]` and `[-7.5,7.5]`. Here sinc is
`sin(pi*x)/(pi*x)`, with its continuous value 1 at zero. Composite Simpson
integration with 128, 256 and 512 subdivisions gives identical rounded Q23 rows.
Support endpoints use their actual integrand value, not an artificial zero.
Round each cell by `floor(area*8388608+0.5)`, then correct tap 7 for p<32,
otherwise tap 8, to sum to 8,388,608. The 1024 int32 cells occupy 4,096 bytes.
Phase 64 is phase 0 shifted one tap right; phase 0's final cell is zero.

Regenerate and check from the repository root:

```sh
cmake --preset ci
cmake --build build/ci --target muhtari-audio-coefficients
build/ci/tools/muhtari-audio-coefficients --emit 64 128
build/ci/tools/muhtari-audio-coefficients --check
```

The explicit maintainer target uses owned C17 math and the runner's SHA256;
ordinary CI and shipped objects consume only the fixed integer table. `--emit`
also supports 128 phases and 256/512 subdivisions. `--check` regenerates all
1024 selected cells at 128/256/512 subdivisions, compares production cells and
checks phase-major/tap-major LE-int32 SHA256
`8fdfc75effc7565eff4996b71ad966f4c0d327909ede230d052dc58bc239d66e`.
The larger 128-phase candidate also passed the measured recovery matrix;
64 is the smallest measured passing density. The old 32-phase table passed
132 equivalent schedules but failed six actual wholly slow AUDF1 cases,
worst -78.28741 dBFS. The compact support, window and nonlinear tables remain.

Runtime interpolation rounds the signed difference times the phase remainder
over 13,125 to nearest, with ties away from zero. Unsigned magnitude plus
half the positive divisor is divided before restoring the sign; bounded
quotients fit int64. Tap 7/8 corrects the row to exactly 8,388,608.
The ring integrates Q23 impulses. Multiplication by 128 converts the integrated
value to Q30. The single DC blocker is
`y=x-x_previous+R*y_previous`, with Q30 pole 1,070,934,451, generated from
`exp(-2*pi*20/48000)`. Its feedback product uses quotient and remainder to
avoid multiplying two large Q30 states. Feedback division truncates toward
zero; final PCM rounds ties away from zero. Pre-clamp peak and clip counts
are retained so saturation cannot conceal a failed bound.

There is no event array. At most 150 merged ticks fit in a sample interval;
each nonzero mixed delta adds 16 impulses. The FIFO holds at most 801 samples
for the current logical frame and separately bounded future instruction
carry. A conservative single held-access bound is 456 RDY ticks, two 454-tick
DMA lines, 12 ticks for the nine-tick abort tail plus whole-cycle alignment,
then a six-tick slow access. RDY releases once; a DMA burst ends no later
than nine ticks past the line's t454 cutoff, before the
next t28 request, allowing a six-tick access to finish. A WSYNC hold may span
two such lines. Eight opcode accesses plus seven interrupt-entry accesses
therefore bound overshoot at 20,730 ticks, or ceil(20,730*88/13,125)=139
future samples. These deliberately loose bounds follow `bus.c`'s held-cycle
loop, `maria.c`'s burst cutoff, `tia.c`'s RDY release and `cpu.c`'s instruction
and NMI entry; they are not observed maxima. The FIFO uses 801+139 cells,
and current versus future samples are separated by the logical horizon.

All persistent values, including fraction, integrator, blocker, raw levels,
pending mixed tick, FIFO and audit counters, belong to the instance. Reset
takes a clock origin and a declared POKEY count (0..2); no allocation occurs
while reconstructing. Full nonlinear scaling and intermediate-width bounds
follow below.

## Nonlinear node-current mix

The owned `audio_tables.h` contains 31 TIA and 825 POKEY uint32 cells.
Each is nearest Q24, `floor(curve*16777216+0.5)`, generated offline using
only the standard math library. TIA uses `2n/(n+30)` for the sum n=0..30.
This reaches one at a full-scale voltage of 2.5 V; the literal half-scale
curve `n/(n+30)` weighted at 5 V is dimensionally equivalent.

For each POKEY, raw is the sum of the four channels' weighted DAC drops in
10 mV units: volume bits contribute 12, 26, 56 and 112. Thus a channel is
0..206, and a chip is 0..824. With x=raw/824, P(x) is `2.171*x` for x<=0.14,
otherwise `2.171*(0.14+(1-exp(-2.85*(x-0.14)))/2.85)`. P(1) is slightly above
one in this measured fit; normalization retains that value rather than
clamping it. P[206]=8,538,596 and P[824]=16,777,645 in Q24.

The current weights `(2.5/18):(3.8/6.8)` reduce exactly to 85:342.
Cartridge gain is the fixed Q12 value 4096. For c declared POKEYs (0,1,2),
sum the TIA table current and each chip's nonlinear current, then round
`18360*current/(85*16777216+c*342*P[824])` to nearest. The maximum is exactly
18,360 for each declaration, independent of which sources are active.
POKEY-only all-channel levels are 14,705 for one declared chip and 16,331
for two; these are fixed hardware-derived spectral-test amplitudes. One
maximum POKEY channel has 2.048 times the full TIA current with the adopted
18 kOhm resistance (about 2.1 with 18.5 kOhm). The measured POKEY fit came
from an 800XL load; actual cartridge loading and the physical node ratio
remain unmeasured, as A7800-AUDIO section 3 records.

## Complete-path headroom and widths

`core.audio.silence` enumerates all 13,125 integer phases through production
interpolation, then all 88 distinct output alignments. For event tick t,
q=floor(88t/13125). Let S(n,t) be the sum of the event's coefficients through
tap n-q, zero before support and 8,388,608 after it. Summation by parts
expresses any reconstruction as a sum of bounded levels times
`w(n,t)=S(n,t)-S(n,t+1)`. Only 16 samples of finite support contribute.
For every alignment, the weight sum is exactly 8,388,608. The maximum
positive mass is P=10,865,505 and negative magnitude N=2,476,897, so
L1=(P+N)/8,388,608=1.590538263320923. The test also constructs both extremizing
0/M sequences at every alignment and checks these integrator values through
the real event/ring path. This establishes ordering as well as coefficients.

For levels in [0,M], reconstruction lies in [-NM/Q,PM/Q]. The blocker
`(1-z^-1)/(1-R*z^-1)` has positive and negative impulse masses each one, so
its output is bounded by `(P+N)M/Q`. Feedback truncation contributes less
than `1/(1-R)=382.473` Q30 units in total, less than 0.000000357 PCM units;
final PCM rounding contributes at most 0.5. A whole PCM LSB safely covers
both. Therefore
`M=floor((32767*10^(-1/20)-1)*8388608/13342402)=18360` gives a conservative
complete peak of 29,203.282515, below 29,203.619490 (-1 dBFS). Neither a
unity DC row sum nor a saturation clamp is used as a clipping argument.

At the common comparison level 18361, the new bound plus one LSB is
29204.873052835464, above the one-dB limit 29203.619489828441.
The final 18360 scale follows this bound, not spectral tuning. Both scales
pass the retained 132 equivalent, 72 actual and 132 rejected-control cases.
The enumerated signed masses replace the old 32-phase numerical proof.

Intermediate bounds are separate from headroom:

- Corrected coefficients, including every interpolated phase, have maximum
  magnitude Cmax=7,283,992. Each pending ring cell has at most 16 intervals
  of 150 deltas of magnitude M. Thus
  `16*150*18360*7283992=320961823488000 < 2^49`. A single delta product is
  below 2^37, and even the temporary integrator-plus-cell sum is below 2^50.
- The much tighter integrated Q23 magnitude is at most
  `P*M=199490671800 < 2^38`; conversion times 128 is
  25,534,805,990,400 < 2^45. No Q15 width estimate is reused.
- The blocker state bound including feedback error is below 2^45 Q30
  units. Subtracting consecutive inputs and adding feedback fits below
  2^47 even before cancellation. The quotient product is below 2^45;
  the signed remainder product is less than
  `(2^30-1)*1070934451 < 2^60`. Division truncates toward zero, including
  negative values. Final rounding forms magnitude by unsigned subtraction;
  its addition and signed quotient conversion fit the bounds above.
  No signed negative shift or 128-bit extension is needed on MSVC/be32.
- The phase numerator is at most 13124*64=839936 < 2^20, fitting uint32.
- The maximum mixed Q24 current is below 2^34. Multiplication by 18,360
  and the fixed Q12 cartridge-gain intermediates fit below 2^49. Table
  indexes are bounded by the ten raw DAC channels and two fixed chips.

The same existing CTest checks every generated mix cell against the formulas,
monotonicity, endpoints, current ratios, exact phase normalization, negative
rounding, maximum hardware declarations, rapid full-range transitions,
partition equality, future carry and discard equality. It checks pre-clamp
peaks and counts, so clamping cannot turn a failed stress check into a pass.
Actual chip schedules and spectral acceptance are additional tests added
with the respective chip and spectral modules.

## Spectral measurement

`audio.spectral` captures the shipped mixer, kernel, blocker and int16 output
after 12,000 delivered settling samples, then measures 65,536 samples at
48 kHz. The periodic four-term Blackman-Harris window uses coefficients
0.35875, 0.48829, 0.14128 and 0.01168. Amplitude is
`2*abs(DFT)/sum(window)`. The search covers strictly `0 < f < 16000 Hz`.
An audible fundamental is evaluated at its exact physical frequency for
the dBc reference; ultrasonic-only cases use 32767 as the dBFS reference.
Acceptance is strictly below -80 dB.

Only physical in-band harmonic lines receive a five-bin leakage mask.
Uniform square waves have only odd harmonics; TIA's uneven B spacings
permit even lines when an odd number of B edges separates toggles.
No ultrasonic line is folded into a mask. `audio.spectral.controls` checks
off-bin sine amplitude, aggregate physical-line leakage, an injected
-60 dBc spur six bins from the fundamental, and nearest-sample kernel
bypass at identical current-derived gain, Q30 blocker and int16 rounding.
The deliberately aliased output must fail the same -80 dB judge.

Each path evaluates exactly 132 distinct schedules: 96 TIA cases program
the real chip's AUDC4, AUDF0..31 and AUDV15, at epoch offsets0,7,137 master
ticks, with both channels synchronous and no declared POKEY. Catch-up sends
the actual chip's DAC changes through production synthesis. The other36 use equivalent
POKEY AUDF 0/1/3, one/two declared chips, one/four active full-volume
channels per chip, and the same offsets. Both chips share phase in the
two-chip stress. Coordinate-derived case identities reject missing and
duplicate evaluations. The diagnostic also reports the conservative
nearest-bin TIA reference used by the feasibility research, separately
from the validated exact-frequency reference used for acceptance.

The TIA capture includes its real divider, feedback and uneven phase
schedule; its independent vectors and confidence limits are documented in
[TIA sound](tia.md). POKEY stimuli remain DAC-equivalent schedules until
that chip's integration. The adopted node-current model and its unmeasured
physical loading limits remain as described above.


The recovery also checked 84 bounded periodic mixed-clock/high-pass cases,
using clock words 1/2, 1/3, 3/3, 1/4, 3/4, 5/4, 7/4 (low bit first,
1=slow), origins 1/29/661 and two chips. Squares use AUDF0/1/3;
high-pass uses equal-period AUDF2 with the high timer initially zero,
then 2 at the fourth fall. Actual square PCM equals independent arithmetic
falling-cycle schedules. Physical periods use lcm(word length,2*(N+4))
cycles for squares and lcm(word length,12) for high-pass, summed through
the actual 4/6-tick word. The maximum square period is 224 ticks; every
fundamental is above 16 kHz, so none receives a harmonic mask.
Compact square and high-pass/origin representatives stay in `audio.spectral`.
More than 1000 continuing raw changes and nonzero kernel work are required
even when settled ultrasonic PCM quantizes to silence.

One-off independent peak refinement maximizes the same windowed DFT within
±0.6 bin around the largest unmasked FFT peak (28 ternary iterations),
without changing capture, normalization, masks or threshold. These are
finite-matrix checks, not a bound on every legal register stream or a claim
of measured analog accuracy. Platform/hash and workload certification follow
in their respective integration checks.

## Private raw observation

The runner's private observer receives absolute master ticks, source identity and
four-bit gated DAC codes. Sources 0–1 are TIA; 2–5 are the primary POKEY;
6–9 are the secondary at $440, including when it is the only POKEY. Mixer
weights and POKEY bit-current conversion do not enter this stream. Known initial
zeros and unchanged levels produce no records. Equal ticks use ascending source
identity, with stable order for multiple changes of one source.

The callback and its host context survive load and power cycle. Register before
the first load; callbacks must never reenter the core. Registration can fail with the
existing allocation error. A first registration after load is rejected without
allocation. A NULL callback frees the private observation storage
and bypasses all observation work; synthesis still advances normally. Registration
is private and adds nothing to the installed API or ABI.

Frames own half-open intervals: events at the deadline or beyond are retained for
the following frame. Only instruction overshoot is buffered, never a whole frame.
The conservative 20,730-tick bound permits ten raw changes per tick (eight chip
channels, TIA and CPU-write margin); offsets fit uint16. A separate 32-record
same-tick stage orders half-edge subevents and writes before delivery. These are
functional bounds, not a claim about analog propagation. A BIOS-refused frame
may already have invoked callbacks: its host must discard tentative evidence.
