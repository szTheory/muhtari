# Audio reconstruction

The internal synthesis identity is `exact-q23-16x32-dc20`. The output is mono
signed int16 at exactly 48,000/1 Hz. One master tick advances the output phase
by 88 modulo 13,125. Events at tick t use that tick's phase before advancing
the interval [t,t+1); equal-tick raw source changes are merged before adding
one mixed delta. Rendering drains the logical half-open interval, and NULL
discards samples while retaining identical history. Chip/bus integration is
added separately; no-content public frames retain their silent test card.

The interpolation and 16-slot int64 impulse ring follow the approved MIT
sibling at `9f9386c661f6`, `src/synth.c`. The historical Q15 coefficients are
replaced by regenerated Q23 coefficients under the dated precision amendment
in [sound research](../../.planning/phases/04-sound/04-RESEARCH.md),
the dated coefficient-precision amendment. No NES filters or
NES mixer tables are retained.

For phase p=0..31 and tap j=0..15, integrate
`sinc(x) I0(8 sqrt(1-(x/7.5)^2))/I0(8)` over the intersection of
`[j-7-p/32-0.5,j-7-p/32+0.5]` and `[-7.5,7.5]`. Here sinc is
`sin(pi*x)/(pi*x)`, with its continuous value 1 at zero. Composite Simpson
integration with 128 and 256 subdivisions gives identical rounded Q23 rows.
Support endpoints use their actual integrand value, not an artificial zero.
Round each cell by `floor(area*8388608+0.5)`, then correct tap 7 for p<16,
otherwise tap 8, to sum to 8,388,608. The 512 int32 cells occupy 2,048 bytes.
Phase 32 is phase 0 shifted one tap right; phase 0's final cell is zero.

Runtime interpolation rounds the signed difference times the phase remainder
over 13,125 to nearest, with ties away from zero. Tap 7/8 then corrects its
row to exactly 8,388,608. No division or signed right shift is implicit.
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
are documented with the owned node-current tables.
