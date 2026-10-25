# TIA input ports, sound and horizontal timing

## Input ports

The [Atari TIA input-port specification, sections8B–8C](https://www.blarworld.net/stella/8.%20Input%20ports.html)
defines four dumped inputs and two low-memory inputs. VBLANK ($01, mirrored
every64 addresses) D7 grounds INPT0–3, including a ProLine button's high
source. Clearing D7 releases the ground; repeated zero or one writes retain
the electrical condition. D7 never affects INPT4/5.

VBLANK D6 enables the independent INPT4/5 low latches. Enabling captures an
already-low pin; every later low captures without a CPU read, even during
HALT/JAM. Repeated enable writes and reads retain a captured low. Disabling
clears both stored latches high and bypasses them, so a currently low pin
still reads low. Power-on disables and clears. Due pin events run through
the write data tick before changing D6; a simultaneous low/enable captures,
while low/disable leaves the live level. VBLANK-applied host controls and
PB/INPTCTRL two-button mode changes deliver their electrical levels too.

Reads and peeks only observe. INPT0–5 drive D7 alone; all other bits retain
open bus. This shared correction applies to legacy and extended hosts and
is identified by behavior revision5. TIA video blanking does not suppress
MARIA emission. Input control leaves decode, WSYNC and the audio clock
below unchanged.

The TIA has its own 456-master-tick horizontal epoch. Audio phase A occurs at
0 and 144, B at 64 and 288. B spacing alternates 224 and 232 ticks. This
clock runs independently of MARIA's 454-tick line. The initial profile's
`tia_phase` offsets the epoch shared with WSYNC.

Every decoded TIA write reaches the chip, including unlocked INPTCTRL writes.
Due A/B events execute before register mutation at the data tick: start+5 for
a slow cycle, whose bus completion is start+6. Registers AUDC, AUDF and AUDV
retain four, five and four bits respectively, on both channels and mirrors.

Phase A compares each five-bit up-counter with AUDF. Equality clears the
counter and enables the following B; otherwise it increments modulo32.
There is no reload latch. Lowering AUDF20 to5 with count12 delays the next
enabled step by26 audio clocks, including wrap through31. An AUDF write
after A does not cancel that A's enabled B.

## Feedback and phase latency

The owned model follows the [Atari TIA-1A logic diagram, revision E,
sheet4](https://www.atariage.com/2600/archives/schematics_tia/), AUDIO NOISE
GEN. and AUDIO OUTPUT. The scanned sheet's SHA-256 is
`03df74daa22654a0cc5d214e3b64b4da710db249564780ea7df4284b5a8d159b`.
The [documentary schematic analysis](https://github.com/jigo2600/jigo2600/blob/f454593beedf763fb8ab92fc69744774086831ed/doc/TIA_Sound.md#audio-generator)
independently describes its feedback functions. No emulator implementation
source is used.

Let N be the five noise stages, P the four pulse stages, bit0 the first
physical shift stage, L=AUDC&3 and H=AUDC>>2. The output is P3.
At an enabled A, capture:

```text
noise_input = 1                                if AUDC == 0
              (N4 XOR (L != 0 ? N2 : P3))
                OR (N == 0 AND (L != 0 OR P == 0)) otherwise
pulse_gate = 1                                 if L is 0 or 1
             N == 8 or N == 24                  if L == 2
             N4                                if L == 3
```

At the enabled B, select the pulse input from the current AUDC and the
pre-shift register outputs:

```text
pulse_input = 1                                if AUDC == 0
              (P2 XOR P3) OR P == 0             otherwise if H == 0
              NOT P0                           if H == 1
              N4                               if H == 2
              (P & 7) in {0,1,2,3,5}            if H == 3
P_next = pulse_gate ? ((P << 1) | pulse_input) & 15 : P
N_next = ((N << 1) | noise_input) & 31
```

The pulse input uses old N4, never the newly shifted noise bit. The gate
has an A-phase latch. Thus an AUDC write between A and B affects pulse
feedback at B while retaining A's captured noise input and gate. This is
the adopted functional split-phase convention, without claiming to model
analog races inside a physical clock edge.

AUDV changes the DAC immediately: level=AUDV×P3. Zero volume suppresses
output changes while the divider and both registers keep advancing. Only
changed raw levels reach synthesis. Known initial zero levels emit nothing.
Simultaneous B changes reach synthesis in channel0, channel1 order; a
same-tick register write follows those changes. Synthesis may coalesce the
mixed delta at one tick while chip calls retain their order.

## Zero-start expectations

Start N=P=0, hold AUDC fixed and use AUDF0. Sample P3 after each complete
enabled A/B pair. Period/ones describe a complete settled minimal output
cycle, not a startup window. AUDF multiplies period and ones by AUDF+1.

| AUDC | First40 outputs | Period | Ones |
|---|---|---:|---:|
| 0 | `0001111111111111111111111111111111111111` | 1 | 1 |
| 1 | `0001001101011110001001101011110001001101` | 15 | 8 |
| 2 | `0000000000000000000000000000000000000000` | 465 | 248 |
| 3 | `0000000000011100110000100111111000000000` | 465 | 248 |
| 4, 5 | `0001010101010101010101010101010101010101` | 2 | 1 |
| 6 | `0000000000000000000000000000000000000000` | 31 | 18 |
| 7 | `0000000000011101010000100101100111110001` | 31 | 16 |
| 8 | `0000000010000100011000010011100101010110` | 511 | 256 |
| 9 | `0000000010010110011111000110111010100001` | 31 | 16 |
| 10 | `0000000000000000000000000000000000000000` | 31 | 18 |
| 11 | `0000000000011111111111111111111111111111` | 1 | 1 |
| 12, 13 | `0001110001110001110001110001110001110001` | 6 | 3 |
| 14 | `0000000000000000000000000000000000000000` | 93 | 49 |
| 15 | `0000000000011111000000111100000111111111` | 93 | 47 |

Mode0 fills high at step4. Mode11's pulse shifts occur at steps6,9,11,12,
taking P through1,3,7,15, so it first becomes high at12. The div2 mode also
has three initial low outputs before its first high; it is not an immediate
toggle from power-on. Frequency is (315000000/44)/(228×(AUDF+1)×period).

The independent stage-array derivation agrees with a separate Boolean
transcription for all8192 mode/joint-state transitions. Enumerating every
one of512 starts per mode finds two256-start basins for modes6/7/14/15.
Starting N=21/P=0 instead produces13/15/44/46 ones and mode11 fill10, under
the identical recurrence. The earlier preparation table's simulation seed
is unknown; reproducing its aggregates does not establish its history.
Modes9 and10 always have16 and18 ones respectively. Consequently6/10 and
7/9 are not unconditional complements. Waiting longer from zero cannot
switch its steady attractor.

All-zero startup is a deterministic compatibility choice: the physical TIA
has no reset pin and its power-on state is unknown. The recurrence and its
derived vectors are documentary functional evidence. Disputed physical
patterns, BIOS residue and asynchronous edge races remain qualified until
later hardware measurements; they are not owner verification gates here.

## Resynchronization and reconstruction

An RSYNC write completes all old due phases through its data tick, then
restarts the shared horizontal epoch there. It schedules only future
events: B at+64, A at+144, B at+288, A at+456. It does not replay A at zero,
reset the channel registers or invent a retroactive output. The previously
captured enabled/gate values remain until the next A. This explicit
functional strobe convention keeps audio and subsequent WSYNC aligned.

WSYNC releases at the first wrap strictly after its data tick, using the
current epoch; MARIA's later pending release still takes precedence. Without
RSYNC this preserves the original profile-offset WSYNC timing. The
horizontal decode facts and hardware drift reports are indexed in
[audio preparation, section2](../../.planning/preparation/A7800-AUDIO.md).

`audio.spectral` programs both real TIA channels to AUDC4, AUDV15 and every
AUDF0..31, at epoch offsets0/7/137. It renders the actual catch-up path
through the production mixer, Q23 kernel, DC blocker and int16 output. The
same hardware scale and physical-only harmonic masks judge it alongside
nearest-sample alias controls. POKEY schedules remain equivalent stimuli
until that chip's integration. See [audio reconstruction](audio.md).
