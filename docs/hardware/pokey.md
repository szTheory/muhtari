# Cartridge POKEY sound

This is owned integer code derived from the January 2, 2026 Altirra Hardware
Reference Manual, chapter 5 and appendix E (documentation only), and the
console clock facts in A7800-AUDIO section 4. No emulator source is used.
The manual is available at
https://www.virtualdub.org/downloads/Altirra%20Hardware%20Reference%20Manual.pdf.

Each machine has two fixed independent chip slots. Default power-on is the
compatibility state RUNNING, SKCTL=$03; a private profile selects INIT,
SKCTL=$00. AUDF, AUDC, AUDCTL and IRQEN start at zero, IRQST at $FF.
Timers and flip-flops have deterministic adopted state. These defaults
describe neither measured cold silicon nor a skipped BIOS's residue.
Fast boot anchors cartridge clocks at tick108984, actual phi2 zero; BIOS
starts at zero and counts its real reset and program cycles. RIOT and CPU
cycle counts are never advanced to simulate a skipped BIOS.

Fast 8-bit timers use N+4 machine clocks. STIMER establishes output
flip-flops at one, starts the reload pipeline at three clocks, and sends
no timer pulse or IRQ itself. The first reload samples AUDF, then the
first underflow/reload pipeline finishes N+4 clocks later. Output and
enabled IRQ follow one clock later: N+8 after STIMER, then N+4 apart.
This explicitly follows the manual's figure8: AUDF0 IRQs at8,12,16,
with AUDF deadlines2,6,10. Its reload text is read together with that
figure; the initial forced reload does not itself produce a pulse.
Low-channel XOR latches at one invert channels1/2. Volume-only bypasses
the XOR. Only actual DAC changes enter synthesis; volume bits contribute
12,26,56,112 units, then the existing per-chip nonlinear/current mix and
production reconstruction apply. IRQST is active low; clearing IRQEN
bits acknowledges their requests. CPU wiring is a later integration.

STIMER output polarity, cold state, analog cartridge loading and
sub-cycle races are confidence-qualified adopted behavior, pending
hardware evidence.

## Clock integration and timers

The bus supplies each actual half edge before accessing data. A fast or
held cycle starting at s rises at s+2 and falls at s+4, completing at s+4.
A slow cycle rises at s+2 and falls at s+5, completing at s+6. Thus a slow
cycle at0 is followed by a rise at8. Both chip slots and the independent
TIA lattice advance together; equal ticks process TIA then chip slots in
order, each with channel order1,2,3,4. Mixed synthesis coalesces the tick.
Held batches keep their real repeated four-tick edges.

Only channels1/3 select the direct machine clock. Other timers use shared
divide28 or divide114 prescalers, whose adopted initial phases produce
first pulses at19/78 and first zero-divisor IRQs at24/83. Base timers use
(N+1) prescaler periods. Reload samples AUDF after the split-counter delay.
Both joined pairs are implemented. The low counter wraps every256 input
pulses without reloading; its carry reaches the high counter three clocks
later. High underflow reloads both, giving N+7 fast clocks or (N+1) base
pulses for the combined16-bit N. Each high period contains AUDF_high+1
low pulses. The high output follows the last low output by three clocks.
CPU IRQ acceptance and cartridge address decoding have separate integration.

AUDF is sampled on reload, so writes throughT+2 affect the initialT+3
reload; the next unjoined fast reload isT+7 for AUDF0. STIMER atT+4
preempts the pendingT+8 pulse; T+5 preserves it. The manual prose names
T+9 for the joined second reload, while figure8 labelsT+9 as the last
AUDF write (a deadline, not the latch). The adopted pipeline follows that
figure: shared latch atT+10, high output atT+11, then seven clocks apart.
This documentary discrepancy remains a hardware-measurement question.

Two-tone uses an idle mark serial data bit: timer2 always resynchronizes
both counters, timer1 does so unless force-break selects space. This
models only the timer interaction; there is no serial peripheral. The
extra reload follows the ordinary reload by two clocks and produces
fast AUDF0 outputs at8,14,20. Adjacent delivered timer pulses retain
both resyncs; same-cycle pulses coalesce. Force-break suppresses timer1's
resync without suppressing its sound output. Resync does not fabricate
audio pulses. A base timer's reload precedes its output by one clock,
absorbing the extra delay before the next prescaler pulse.

High-pass captures the low output when its high timer fires, then updates
the XOR latch on the third subsequent actual half edge. All-fast delay
is6 ticks, wholly slow is9, and a tested slow-to-fast sequence is7.
The equal-period high timer two cycles ahead leaves a2-tick fast or3-tick
slow pulse. This is an adopted functional convention for the manual's
approximately1.5-cycle path, not a measured analog timing claim.

## Reconstruction recovery

The production spectral test retains the independent all-DAC stress and
adds72 actual register-programmed chip cases: AUDF0/1/3, three origins,
one/two chips, one or four enabled channels per chip, fast and wholly
slow bus segments. Four-channel legal maximum stress uses1/3 as aligned
fast squares and2/4 as volume-only15; it never invents direct fast clocks
on2/4. Hardware current weights and physical-only masks
are preserved. One-channel captures also match an independent timer-edge
schedule byte-for-byte through the same production synthesis.

The old 16-tap/32-phase Q23 reconstruction failed six wholly slow AUDF1
vectors: first edge53, then30 ticks apart, with a15136.23 Hz spur between
-79.43 and -78.29 dBFS. The independent edge reference reproduced the PCM.
The 64-phase production reconstruction resolves these vectors without
changing chip timestamps, current weights or masks. Its independently
renewed headroom proof requires maximum mixed level18360; both the former
comparison level18361 and final18360 pass. See [audio reconstruction](audio.md)
for generation, bounds and the finite mixed-clock/high-pass evidence.
