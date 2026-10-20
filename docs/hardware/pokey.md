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
bits acknowledges their requests. Accepted v4 wiring supplies the CPU interrupt level.

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
CPU IRQ acceptance follows the sampled-I rules in [SALLY](sally.md).

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

## Noise, initialization and register reads

All channels share owned XNOR shift registers at the actual machine
clock: poly4 is1+x3+x4, poly5 is1+x3+x5, and AUDCTL bit7 selects
1+x4+x9 or1+x12+x17 for the long register. From the adopted zero
state their short patterns are000011101100101 and
0000011100100010101111011010011. A history pipeline delivers each
bit to channels1,2,3,4 one cycle apart, with channel1 sampling first.
Long noise samples bit8, the right end of the CPU-visible RANDOM byte.
AUDC bit7 bypasses the poly5 clock mask; bit5 chooses toggle instead
of noise sampling; bit6 chooses poly4 instead of long noise. These
circuits continue under volume-only and volume zero.

SKCTL bits0–1 both zero enter init. Prescalers reset and hold their
19/78 startup phase. Noise registers drain toward zero rather than
stopping instantly; poly5's audio gate is held high in init. Timer
counters, outputs and audio registers are retained. Fast timers keep
running, including joined ones; base-clock timers receive no pulses.
On exit, the first base pulses are19/78 clocks later, with first
zero-divisor IRQs at24/83. Rewriting SKCTL in running mode does not
reset prescaler or noise phase.

RANDOM returns the inverted bits15..8 of the long register. Short
mode feeds back from bits8/13 into bit16, retaining the lower drain
stages; long mode uses bits0/5. Each init clock shifts in zero, so
RANDOM is$FF after nine clocks and the entire register drains within
18. Four clocks after a fully drained short-mode exit, RANDOM is$1F.
The unique$00 state occurs382 clocks after exit; successive114-clock
reads return00 DF EE 16 B9. These are documentary values, not hashes.
Changing AUDCTL mode preserves register state.

IRQST is the implemented timer status read and RANDOM the noise read.
Other register reads return$FF, an explicit disconnected-peripheral
convention. No keyboard, paddle conversion or serial transport is
implemented. Two chip slots and separate machines retain all counters,
noise pipelines and DAC history independently.

The portable pokey.logic/pokey.clock cases include exact timer/IRQ and
noise values, half-edge high-pass delays, both joined pairs, two-tone
resync collisions, all16 distortion modes, init drain and exit, silent
state resumption and split catch-up equality. A joined+high-pass trace
compares real-bus PCM with independently scheduled raw edges through
the production synthesizer. The spectral gate also covers joined+HP
and two-tone+HP at fast and wholly slow clocks; these particular signals
have no physical lines below16kHz, so no alias is masked. This finite
coverage is a functional model check, not a silicon measurement.

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

## Cartridge windows

The A78 declarations select the following windows (cartridge preparation
section 2.1, sources CART.01/CART.20; see [cartridge decoding](cartridge.md)):

| Legacy cart-type bit | v4 audio code | CPU window | Identity |
|---|---|---|---|
| 0 | 5 | $4000–$7FFF, every 16 bytes mirrored | primary |
| 6 | 2 | $0450–$045F | primary |
| 15 | 4 | $0800–$0FFF, every 16 bytes mirrored | primary |
| 10 | 1 | $0440–$044F | secondary |
| 6 and 10 | 3 | $0450–$045F and $0440–$044F | both |

These are two fixed identities, with source IDs 2–5 for primary and 6–9
for secondary. A lone $0440 chip remains secondary while the mixer counts
one actual chip; two declared chips feed the same mono output. Loading
another layout or power cycling clears old DAC/timer state and restores
the current declaration. `pokey.maps` runs generated legacy and v4 images
through SALLY and public frame PCM, checks INIT RANDOM/IRQST against $FF,
visits every mirrored register block and tests both sides of each window.
Its dual-chip PCM differs from either single-chip rendering under the same
instruction schedule, and from muting either input with dual-chip normalization. These checks establish the implemented digital model,
not physical cartridge loading or analog mix calibration.

CPU writes select POKEY before ROM/RAM at a declared window. At $4000,
CPU reads preserve overlapping linear ROM, EXROM, EXFIX or RAM; this makes
the chip write-only there. Legacy RAM/bank6/banked-RAM conflicts drop the
$4000 sound flag during header normalization; accepted v4 can explicitly
combine those devices with POKEY. Multiple primary sound flags retain
$0450 before $4000 before $0800, with at most one primary and one secondary.
A hash match overrides board/RAM/sound before this runtime selection.

A CPU register access takes its real bus cycle, advancing both chip clocks.
RANDOM and IRQST reads do not acknowledge timer requests; clearing IRQEN
acknowledges them. STIMER changes the reload/output state described above.
Peeks and the board-read path used by MARIA fetches select board memory or
open bus at those same addresses, without reading registers or advancing
chip/synthesis state. A full DMA fetch still updates the bus's floating
byte as documented in [MARIA](maria.md).

Only an accepted validated v4 block supplies effective IRQ bits 0/1;
raw IRQ fields from ignored blocks remain visible to probing but cannot
wire a chip. Versions below4 reserve these bytes; versions above4 follow
v4 with a warning. Bit0 names primary and bit1 names $0440 regardless of
how many chips are declared. Bit2 belongs to another audio device and is
excluded from the POKEY mask. `pokey.maps` covers accepted codes/masks,
invalid mapper/options/audio/IRQ and geometry fallback, reserved legacy
bytes, hash overrides, repeated loads and reset. It checks overlap read
values, whole-instance purity of peeks/DMA board reads, and actual MARIA
objects fetching floating or board bytes at mapped RANDOM addresses. CPU interrupt
sampling and entry follow [SALLY](sally.md).

## Interrupts

The bus consumes both chips' actual falling edges before publishing their
wired OR level for the CPU poll. Bit 0 selects the primary slot; bit 1
selects the fixed $0440 slot even when it is the only declared chip. Ignored
v4 data and v3 reserve fields cannot assert the line. `core.audio.silence`
executes a generated timer cartridge through handler acknowledgment and
continued public PCM, with unwired and rejected-header controls.

Timers 1/2/4 use IRQST bits 0/1/2, active low; timer 3 has no IRQ. Disabled
sources lose their events, and unsupported serial/keyboard status stays high.
IRQST reads do not acknowledge. Clearing an IRQEN bit immediately resets
its status; enabling it does not restore a lost event. Timer and status
logic continues while muted, in INIT for direct-clock timers, and during
RDY/HALT holds. Base timers remain stopped in INIT as described above.

The January 2, 2026 manual section 5.7 requires four clocks of setup when
enabling a source and two when disabling it. A fixed three-clock enable
history at falling edges enforces these boundaries, since register writes
follow that edge. An event with enough setup can already be propagating
when IRQEN is cleared one clock before it; it can still reach the CPU even
with IRQST reset. The bus publishes the delayed line, rather than deriving
it afresh from IRQST, so acknowledgment cannot revoke a sampled CPU decision.

The adopted propagation is two unheld CPU cycles from signaled status to
the poll-visible line. The CPU then completes its final instruction cycle
before starting entry. This chooses the manual's common three-cycle case;
the manual reports two-cycle cases and temperature/system variation too.
Holds advance the chip with real clock edges but freeze CPU synchronization;
only completed unheld bus accesses shift the line pipeline. They also postpone
the CPU poll until its bus access completes. This deterministic digital convention
does not model analog setup/hold or machine-dependent metastability.
No IRQ deadline scheduler is needed: the bus already delivers every actual
half edge, including every held clock, before it publishes the line.

`sally.irq` checks all timer sources, disabled peripheral bits, enable at
T4 versus T5 for a T8 event, disable at T6/T7/T8, acknowledgment propagation,
INIT, muted operation, held clocks and both physical wiring identities.
`cpu.irq.timing` checks concrete stack/vector accesses, sampled-I, branch
polls, deciding/final and held cycles, persistent RTI and NMI overlap.
The generated cartridge cases in `core.audio.silence` cover all three
sources on primary, lone secondary and both dual-chip identities, with
v3/unwired/invalid-v4 controls. `audio.frames` exercises real timer IRQ
entry after combined RDY/HALT holds across the frame deadline, preserving
prefix/tail PCM parity and the existing 20730-tick/139-sample carry bound.
