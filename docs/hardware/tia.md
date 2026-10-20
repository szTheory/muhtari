# TIA sound and horizontal timing

The TIA has its own 456-master-tick horizontal epoch. Audio phase A occurs at
0 and 144, B at 64 and 288. B spacing alternates 224 and 232 ticks. This
clock runs independently of MARIA's 454-tick line. The initial profile's
`tia_phase` offsets the epoch shared with WSYNC.

Phase A compares each five-bit up-counter with AUDF. Equality clears the
counter and enables the following B; otherwise it increments modulo 32.
AUDC 4 and 5 toggle the channel at each enabled B. The raw DAC level is
the output bit times the four-bit AUDV. The production synthesizer receives
only changed levels, with channel 0 preceding channel 1 at a shared tick.

Every decoded TIA write reaches the chip, including unlocked INPTCTRL
writes. Due A/B events execute before register mutation at the data tick:
start+5 for a slow cycle, whose bus completion is start+6. AUDV changes the
DAC immediately. AUDF affects the next A comparison. WSYNC releases at the
first horizontal wrap strictly after its data tick; MARIA's later pending
release still takes precedence.

These are adopted functional timing facts from [audio preparation, section
2](../../.planning/preparation/A7800-AUDIO.md), citing the horizontal-counter
decodes (AUD.01), Atari schematic (AUD.13), sound guide (AUD.16) and hardware
phase/decode reports (AUD.31). All-zero startup is a deterministic compatibility
choice: the physical chip has no reset pin and its power-on state is unknown.
