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
