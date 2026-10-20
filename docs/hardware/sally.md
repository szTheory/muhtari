# SALLY, the 7800's CPU

SALLY is the 6502 variant in the Atari 7800. To the program it is an NMOS 6502: it has the same registers, opcodes,
cycle order and decimal mode, and the unofficial opcodes behave the same way. The 7800 adds a HALT input, which MARIA
uses, and RDY. Both belong to the bus, not to this page, which covers the CPU alone: the behaviour `src/cpu.c`
implements and the SingleStepTests 6502 vectors check.

Sources:

- **64doc** (https://www.nesdev.org/6502_cpu.txt): the cycle-by-cycle tables, the unofficial opcodes and ARR in
  decimal mode.
- **The decimal tutorial**, Bruce Clark, "Decimal Mode" (http://www.6502.org/tutorials/decimal_mode.html), Appendix A
  (the NMOS ADC and SBC sequences) and Appendix B (the test program).
- **The vectors**, SingleStepTests 65x02, `6502/v1` at commit `2f6980a2d957`
  (https://github.com/SingleStepTests/65x02, MIT): 10,000 tests per opcode, each with its initial and final state
  and every bus cycle. `tests/vectors/6502-sample.n65v` holds the first 100 tests of each opcode.
- **CPU interrupts**, the NESdev wiki (https://www.nesdev.org/wiki/CPU_interrupts, accessed 2026-10-16): when the
  NMOS 6502 polls its interrupt lines, for ordinary instructions, branches and BRK.
- **The 7800 system notes**, `.planning/preparation/A7800-SYSTEM.md` section 3: on the 7800, MARIA drives NMI, and
  the edge is latched in every cycle, held ones included.
- **The SALLY readings**, "fingerprinting 6502 processors" (https://forums.atariage.com/topic/385521, accessed
  2026-10-15): ANE and LXA run on three real 7800 consoles.

## Registers

A, X and Y are 8-bit, and so are S, the stack pointer, and P, the status register. PC is 16-bit. The stack is page
1: a push writes at `$0100 + S` and then decrements S, and a pull increments S and then reads. S wraps within the
page. Cartridge POKEY IRQ reaches SALLY only through accepted v4 wiring metadata. Reset, NMI and IRQ have their own sections below.

## Status register

| Bit | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|-----|---|---|---|---|---|---|---|---|
| Flag | N | V | - | B | D | I | Z | C |

Bits 4 (B) and 5 are not stored in the CPU. They exist only in the byte pushed onto the stack. PHP and BRK push P
with both bits set. An interrupt pushes it with bit 4 clear and bit 5 set. PLP and RTI ignore bit 4 and bit 5 of the
byte they pull (64doc).

The vectors load and compare P as one raw byte. `src/cpu.c` therefore keeps whatever bits 4 and 5 hold, and only a
pull changes them: it clears bit 4 and sets bit 5. No other instruction writes either bit.

## Cycle order

Every cycle is one bus access, either a read or a write. The CPU makes an access even on a cycle where it needs
nothing from the bus. The vectors record every access, so these dummy accesses are checked like real ones. The rules
below come from 64doc and agree with the vectors:

- **Implied and accumulator instructions** read the byte after the opcode and discard it. PC does not move.
- **zp,X and zp,Y** read the unindexed zero-page address while the index is added. The sum wraps within page zero.
  So do the two pointer bytes of `(zp,X)` and `(zp),Y`: a pointer at `$FF` takes its high byte from `$00`.
- **abs,X, abs,Y and (zp),Y** first read at the base's high byte with the low byte already indexed. For a read
  instruction, that cycle is a dummy only when the index carries into the high byte. Otherwise it is the real read
  and the instruction is one cycle shorter. Stores and read-modify-writes always make that read, then access the
  correct address.
- **Read-modify-write** instructions read the value, write it back unchanged while the ALU works, then write the
  result. Both writes are real bus writes.
- **Branches** read the offset. A taken branch reads at PC while the low byte is added. When the target is in
  another page, it makes one more read at the old high byte with the new low byte.
- **JMP (ind)** takes the pointer's high byte from the same page, so a pointer at `$xxFF` reads `$xx00`.
- **JSR** reads the low address byte, makes a dummy stack read, pushes PC high then low, and only then reads the high
  address byte. **RTS** and **RTI** make a dummy read at PC and one on the stack before pulling. RTS then reads at
  the pulled address and steps past it.
- **BRK** reads and skips the byte after the opcode. It pushes the return address and P with bits 4 and 5 set, sets
  I, and loads PC from `$FFFE` and `$FFFF`. D is left as it was.

16-bit address sums wrap at `$FFFF`. An indexed address near the top of memory, or a page-crossing branch there,
continues at `$00xx`.

## Decimal mode

With D set, ADC and SBC treat each byte as two decimal digits. The NMOS 6502 computes the result digit by digit,
and its flags follow rules of their own. The steps below are the tutorial's Appendix A, written in unsigned
arithmetic. They are also defined for invalid digits ($A to $F), and the vectors cover such values.

**ADC**, with B the operand and C the carry in:

1. Low digit: `AL = (A & $0F) + (B & $0F) + C`. If `AL >= $0A`, then `AL = ((AL + $06) & $0F) + $10`.
2. Sum: `S = (A & $F0) + (B & $F0) + AL`.
3. N is bit 7 of S. V is set when A and B have the same bit 7 and S's bit 7 differs. Both are taken before the next
   step.
4. If `S >= $A0`, then `S = S + $60`.
5. A is the low byte of S, and C is set when `S >= $100`.

Z is not decimal at all: it is set when the binary sum `A + B + C` is zero in its low byte.

**SBC** leaves every flag (N, V, Z and C) as the binary subtraction sets it. Only A is decimal. With C the carry in
(set means no borrow) and B the operand:

1. Low digit: `AL = (A & $0F) - (B & $0F) + C - 1`. If AL is negative, then `AL = ((AL - $06) & $0F) - $10`.
2. `S = (A & $F0) - (B & $F0) + AL`. If S is negative, then `S = S - $60`.
3. A is the low byte of S.

**RRA** (ROR then ADC) and **ISC** (INC then SBC) use the same ADC and SBC, so they are decimal when D is set.

**ARR** in decimal mode (64doc): let `t = A & imm`, with high digit `AH` and low digit `AL`. A becomes t rotated right
through C. N and Z follow that value, and V is bit 6 of `t XOR A`. Then:

- if `AL + (AL & 1) > 5`, the low digit of A becomes `(A + 6) & $F`, and the high digit is unchanged;
- if `AH + (AH & 1) > 5`, C is set and `$60` is added to A; otherwise C is clear.

With D clear, ARR sets C from bit 6 of the result and V from bit 6 XOR bit 5.

These rules match all 10,000 vectors of every ADC, SBC, RRA, ISC and ARR opcode. Appendix B of the tutorial has a
program that checks ADC and SBC over every pair of operands and both carries, on A, C, N, V and Z.

## Unofficial opcodes

The NMOS 6502 decodes all 256 opcodes, and the undocumented ones do repeatable things. They follow the cycle rules
above for their addressing mode (64doc):

- **NOP forms** (`1A 3A 5A 7A DA FA`, the immediate `80 82 89 C2 E2`, zp `04 44 64`, zp,X `14 34 54 74 D4 F4`, abs
  `0C`, abs,X `1C 3C 5C 7C DC FC`) make the accesses of their mode and change nothing. The memory forms read the
  operand, and abs,X makes the extra read only on a page cross, as a read instruction does.
- **Combined read-modify-writes** use the timing of a read-modify-write and then feed the result to the ALU: SLO
  (ASL, ORA), RLA (ROL, AND), SRE (LSR, EOR), RRA (ROR, ADC with the carry ROR left), DCP (DEC, CMP) and ISC
  (INC, SBC). Their abs,X, abs,Y and (zp),Y forms always make the extra read.
- **LAX** loads A and X with the same value. **SAX** stores A AND X and sets no flag.
- **ANC** (`0B`, `2B`) is AND, then C copies N. **ALR** (`4B`) is AND, then LSR of A. **ARR** (`6B`) is AND, then ROR
  of A, with the flags in the previous section. **SBX** (`CB`) sets X to (A AND X) minus the operand, with no borrow
  in, and sets C, N and Z as CMP does. **`EB`** is SBC immediate.
- **LAS** (`BB`, abs,Y) ANDs the value read with S and puts the result in S, A and X.

## Unstable opcodes

**ANE** (`8B`) and **LXA** (`AB`) OR A with a constant that differs between chips, then AND:

- ANE: `A = (A | K) & X & imm`
- LXA: `A = X = (A | K) & imm`

The constant K lives in the machine profile:

- **$00 is the default.** Of three real SALLYs, two left $00 after `LDA #$00; LDX #$FF; ANE #$FF`, and an NCR
  chip's ANE result drifted after a minute or two of warm-up. `LAX #$FF` (LXA) left $00 on all three (the SALLY
  readings).
  `cpu.unstable.sally_constant` checks that the default gives these results.
- **$EE in the vectors.** The public vectors were generated with K = $EE, so the vector harness sets it.

**SHA** (`9F` abs,Y and `93` (zp),Y), **SHX** (`9E` abs,Y), **SHY** (`9C` abs,X) and **TAS** (`9B` abs,Y) store a
register ANDed with H + 1. H is the high byte of the base address before the index is added. For (zp),Y, it is the
pointer's high byte.

| Opcode | Stored value |
|--------|--------------|
| SHA | A AND X AND (H + 1) |
| SHX | X AND (H + 1) |
| SHY | Y AND (H + 1) |
| TAS | first S = A AND X, then S AND (H + 1) |

The read at the address without the high-byte carry is always made. When the index carries into the high byte, the
stored value also replaces the high byte of the address written. These rules match the vectors for all five
opcodes.

## JAM

Twelve opcodes stop the CPU: `02 12 22 32 42 52 62 72 92 B2 D2 F2`. On the vectors, each makes eleven reads: the
opcode at PC, the next byte at PC + 1, then `$FFFF`, `$FFFE`, `$FFFE`, and `$FFFF` six times. PC ends at PC + 1, and no
register or flag changes. After that the CPU executes nothing until reset. `src/cpu.c` sets `jammed`, and each later
step of a jammed CPU reads `$FFFF` once and changes no register. `frame.jam_ends` shows that a cartridge jammed at its
reset vector still ends every frame within one 4-tick step of its deadline. The vector harness checks that `jammed` is set after exactly these twelve opcodes. A
jammed CPU also takes no NMI, so only reset recovers it.

## Reset

RES runs once, at power-on, when the machine boots through a BIOS; the fast boot sets the state the BIOS leaves
instead. Like the NMI entry it takes seven cycles: two reads at PC, then the three stack cycles, which are reads, so S
steps down by 3 with nothing written, then PC from `$FFFC` and `$FFFD`. The registers are 0 at power-on, so S ends at
`$FD`; I is set and the other flags keep their power-on 0 (System notes, section 8). At power-on MEN = 0, so every
one of these cycles is slow and the vector comes from the BIOS's top 4 KiB (`docs/hardware/bus.md`, Memory map). RES
also clears a jam. Every BIOS starts with SEI and CLD, so a value other than these is overwritten (System notes,
section 3).

`src/cpu.c` marks each opcode fetch, the cycle in which SALLY raises SYNC, so the bus can see the first one from
cartridge space after a BIOS sets EXT: that frame is the hand-off frame (`docs/embedding.md`, Booting with a BIOS).

## NMI

On the 7800, NMI comes from MARIA. The bus latches a falling edge at the end of a cycle and records `nmi_phi2`, the
φ2 count of the cycle it fell in. φ2 counts cycles as they complete, so the first cycle of the run is 1, and a held
cycle counts each of its φ2 periods.

The CPU does not look at the edge in every cycle. Each instruction has one deciding cycle, and the CPU takes the NMI
after the instruction when the edge was latched by the end of that cycle. A later edge is taken after the next
instruction. With the cycles of an instruction numbered 1 to N (CPU interrupts):

| Instruction | Deciding cycle |
|-------------|----------------|
| Most instructions | N - 1, the penultimate cycle |
| Branch not taken (N = 2) | 1, the opcode fetch |
| Branch taken within the page (N = 3) | 1, the opcode fetch, so it polls one cycle early |
| Branch taken across a page (N = 4) | 3 |
| BRK (N = 7) | 4, the push of the return address's low byte |

`src/cpu.c` stores the φ2 count once per instruction, as `poll_phi2`, right after the deciding cycle's bus access
returns. At the end of the instruction it takes the NMI when `nmi_phi2` is not 0 and is at or below `poll_phi2`.

RDY and HALT hold cycles, and the count includes them. A hold inside the deciding cycle, or before it, moves the poll
point later by its length. A hold after the deciding cycle does not move it. An edge latched during a hold carries the
φ2 count of the held period it fell in. So an edge that falls while `CLC`'s opcode fetch is held is taken after `CLC`.
Counting cycles from the instruction's start would take it one instruction late. An edge that falls while the last
cycle of `LDA zp` is held is taken after the next instruction. Taking φ2 minus 1 at the instruction's end would take
it after `LDA`.

BRK compares at its cycle 4, before it reads its vector. If the edge came by then, BRK reads the vector at `$FFFA`
instead of `$FFFE`, and the NMI is consumed. Everything else stays as BRK does it: the return address is BRK + 2, and
the pushed P has bit 4 set. An edge from cycle 5 on gets the BRK vector, and the NMI follows the handler's first
instruction.

The NMI entry takes 7 cycles. It makes two dummy reads at PC, pushes PCH, PCL and P with bit 4 clear and bit 5 set,
sets I, then reads PC from `$FFFA` and `$FFFB`. The edge is consumed before the first of these cycles. The entry does
not poll, so the handler's first instruction always runs. An edge latched during the entry is kept, and the poll at
the end of that first instruction takes it.

The twelve JAM opcodes store no poll mark and do not check for an NMI: a jammed CPU ignores it.

## IRQ

Cartridge timers drive a level, sampled with the current I flag at the same
deciding cycle as NMI. The decision survives changes to I in the final cycle:
CLI, SEI and PLP use old I; RTI has already restored I when it polls.
A persistent level can retrigger after RTI until the device acknowledges it.
NMI wins when both are eligible. A JAM ignores both.

Entry takes seven cycles: two reads at the next PC, PCH/PCL/status pushes,
then the low/high vector reads at $FFFE/$FFFF. Status has B clear and U set;
I is then set and D retained. An NMI arriving through entry cycle 4 redirects
the vector to $FFFA/$FFFB without changing the IRQ return address or status.
Later edges wait for the handler's first instruction. BRK keeps its existing
cycle-4 hijack window and B-set stack byte. The entry itself does not poll.

These are the adopted functional NMOS semantics from the January 2, 2026
[Altirra Hardware Reference Manual](https://www.virtualdub.org/downloads/Altirra%20Hardware%20Reference%20Manual.pdf),
section 3.4, together with the existing CPU interrupt cycle references above.
They do not claim analog setup/hold or temperature-dependent race behavior.
