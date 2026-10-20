/* SALLY, the 7800's 6502: one instruction per call, one bus access per cycle.
   Cycle order follows 64doc (nesdev.org/6502_cpu.txt) and the SingleStepTests
   65x02 vectors (docs/hardware/sally.md, Cycle order).

   The only functions called from here are muhtari__bus_read and
   muhtari__bus_write. No expression holds two bus calls whose order C leaves
   unspecified: a helper that returns an address finishes its cycles before
   the call it is an argument of starts, so the order of bus cycles is the
   order of the source. */
#include "internal.h"

/* Flag bits of P. Bits 4 (B) and 5 exist only on the stack: PHP and BRK push
   P | 0x30, and PLP and RTI load (v & ~0x10) | 0x20. No other instruction
   writes them (docs/hardware/sally.md, Status register). */
#define FLAG_C 0x01u
#define FLAG_Z 0x02u
#define FLAG_I 0x04u
#define FLAG_D 0x08u
#define FLAG_B 0x10u
#define FLAG_U 0x20u
#define FLAG_V 0x40u
#define FLAG_N 0x80u

/* Reads the byte at PC and steps PC: one cycle. */
static uint8_t fetch(muhtari *m)
{
    uint8_t v = muhtari__bus_read(m, m->cpu.pc);
    m->cpu.pc = (uint16_t)(m->cpu.pc + 1u);
    return v;
}

/* The poll mark. An NMI is taken after an instruction when its edge was
   latched at or before the phi2 count at which the instruction's deciding
   cycle completed; for most instructions that is cycle N - 1 of N. Every
   instruction but JAM stores the mark once, right after that cycle's bus
   call returns, so a held cycle up to then moves the mark and a held final
   cycle does not. Nothing is stored per cycle (docs/hardware/sally.md,
   NMI). */
static void poll_mark(muhtari *m)
{
    m->cpu.poll_phi2 = m->bus.phi2;
}

/* The final cycle of an instruction: the poll mark, then the access. */
static uint8_t last_fetch(muhtari *m)
{
    poll_mark(m);
    return fetch(m);
}

static uint8_t last_read(muhtari *m, uint16_t addr)
{
    poll_mark(m);
    return muhtari__bus_read(m, addr);
}

static void last_write(muhtari *m, uint16_t addr, uint8_t v)
{
    poll_mark(m);
    muhtari__bus_write(m, addr, v);
}

/* Sets the bits in mask when cond is nonzero and clears them otherwise.
   Other bits of P stay as they are: bits 4 and 5 change only on a pull. */
static void set_flag(muhtari *m, uint8_t mask, int cond)
{
    uint8_t p = (uint8_t)(m->cpu.p & (uint8_t)~mask);
    m->cpu.p = cond ? (uint8_t)(p | mask) : p;
}

/* N from bit 7 of v, Z when v is zero. */
static void set_nz(muhtari *m, uint8_t v)
{
    set_flag(m, FLAG_N, (v & 0x80u) != 0u);
    set_flag(m, FLAG_Z, v == 0u);
}

/* Effective addresses. Each helper makes exactly the bus accesses of its
   addressing mode up to, not including, the final access at the address it
   returns (64doc, "addressing modes"). Indexed low-byte sums and zero-page
   pointers wrap within their page. */

/* zp: the operand is the address. */
static uint16_t ea_zp(muhtari *m)
{
    return fetch(m);
}

/* zp,X and zp,Y: a dummy read at the unindexed address while the index is
   added, and the sum wraps within zero page. */
static uint16_t ea_zpi(muhtari *m, uint8_t idx)
{
    uint8_t b = fetch(m);
    muhtari__bus_read(m, b);
    return (uint8_t)(b + idx);
}

static uint16_t ea_zpx(muhtari *m)
{
    return ea_zpi(m, m->cpu.x);
}

static uint16_t ea_zpy(muhtari *m)
{
    return ea_zpi(m, m->cpu.y);
}

/* abs: low byte, then high byte. */
static uint16_t ea_abs(muhtari *m)
{
    uint8_t lo = fetch(m);
    uint8_t hi = fetch(m);
    return (uint16_t)(((uint32_t)hi << 8) | lo);
}

/* Adds idx to the base hi:lo. The CPU first reads at hi with the low byte
   already summed; that read is a dummy when the sum carries into the high
   byte. Reads skip it when there is no carry; writes and read-modify-writes
   always make it (always_dummy). */
static uint16_t index_base(muhtari *m, uint8_t lo, uint8_t hi, uint8_t idx, int always_dummy)
{
    uint32_t sum = (uint32_t)lo + idx;
    if (sum > 0xFFu || always_dummy) {
        muhtari__bus_read(m, (uint16_t)(((uint32_t)hi << 8) | (sum & 0xFFu)));
    }
    return (uint16_t)(((uint32_t)hi << 8) + sum);
}

/* abs,X and abs,Y. */
static uint16_t ea_absi(muhtari *m, uint8_t idx, int always_dummy)
{
    uint8_t lo = fetch(m);
    uint8_t hi = fetch(m);
    return index_base(m, lo, hi, idx, always_dummy);
}

/* (zp,X): a dummy read at the operand while X is added, then the pointer
   bytes at (b + X) and (b + X + 1), both wrapping within zero page. */
static uint16_t ea_izx(muhtari *m)
{
    uint8_t b = fetch(m);
    muhtari__bus_read(m, b);
    uint8_t ptr = (uint8_t)(b + m->cpu.x);
    uint8_t lo = muhtari__bus_read(m, ptr);
    uint8_t hi = muhtari__bus_read(m, (uint8_t)(ptr + 1u));
    return (uint16_t)(((uint32_t)hi << 8) | lo);
}

/* (zp),Y: the pointer bytes at p and (p + 1) within zero page, then Y is
   added as for abs,Y. */
static uint16_t ea_izy(muhtari *m, int always_dummy)
{
    uint8_t p = fetch(m);
    uint8_t lo = muhtari__bus_read(m, p);
    uint8_t hi = muhtari__bus_read(m, (uint8_t)(p + 1u));
    return index_base(m, lo, hi, m->cpu.y, always_dummy);
}

/* Implied and accumulator modes: the cycle after the opcode reads the next
   byte and discards it; PC does not move. */
static void implied(muhtari *m)
{
    muhtari__bus_read(m, m->cpu.pc);
}

/* implied as an instruction's final cycle. */
static void last_implied(muhtari *m)
{
    poll_mark(m);
    implied(m);
}

/* Loads and transfers: the register takes v, N and Z follow it. */
static void assign(muhtari *m, uint8_t *reg, uint8_t v)
{
    *reg = v;
    set_nz(m, v);
}

/* LDA, LDX, LDY from memory. */
static void load(muhtari *m, uint8_t *reg, uint16_t ea)
{
    uint8_t v = last_read(m, ea);
    assign(m, reg, v);
}

/* Reads the operand of a read instruction at ea. */
static uint8_t read_at(muhtari *m, uint16_t ea)
{
    return last_read(m, ea);
}

/* The binary sum A + v + C. V is set when both inputs have the same sign and
   the result's sign differs. */
static void adc_binary(muhtari *m, uint8_t v)
{
    uint32_t a = m->cpu.a;
    uint32_t sum = a + v + (m->cpu.p & FLAG_C);
    set_flag(m, FLAG_C, sum > 0xFFu);
    set_flag(m, FLAG_V, ((~(a ^ v) & (a ^ sum)) & 0x80u) != 0u);
    assign(m, &m->cpu.a, (uint8_t)sum);
}

/* ADC. With D set, the NMOS chip adds digit by digit: the low digit is
   corrected by 6 when it passes 9 and carries 0x10, and the high digits are
   added on top. N and V come from that sum before the high digit's
   correction, Z from the binary sum, and A and C after the high digit is
   corrected by 0x60 when the sum reaches 0xA0. Invalid digits go through the
   same steps (docs/hardware/sally.md, Decimal mode). */
static void adc(muhtari *m, uint8_t v)
{
    if ((m->cpu.p & FLAG_D) == 0u) {
        adc_binary(m, v);
        return;
    }
    uint32_t a = m->cpu.a;
    uint32_t c = m->cpu.p & FLAG_C;
    uint32_t al = (a & 0x0Fu) + (v & 0x0Fu) + c;
    if (al >= 0x0Au) {
        al = ((al + 0x06u) & 0x0Fu) + 0x10u;
    }
    uint32_t s = (a & 0xF0u) + (v & 0xF0u) + al;
    set_flag(m, FLAG_N, (s & 0x80u) != 0u);
    set_flag(m, FLAG_V, ((~(a ^ v) & (a ^ s)) & 0x80u) != 0u);
    set_flag(m, FLAG_Z, ((a + v + c) & 0xFFu) == 0u);
    if (s >= 0xA0u) {
        s += 0x60u;
    }
    set_flag(m, FLAG_C, s >= 0x100u);
    m->cpu.a = (uint8_t)s;
}

/* SBC, as ADC of the operand's complement. With D set, every flag is still
   the binary result's, and only A differs: the digits are subtracted with a
   borrow, a low digit that goes below zero is corrected by 6 and borrows
   0x10, and a result below zero is corrected by 0x60. Unsigned wraparound
   stands for the negative values; bit 31 is the sign
   (docs/hardware/sally.md, Decimal mode). */
static void sbc(muhtari *m, uint8_t v)
{
    if ((m->cpu.p & FLAG_D) == 0u) {
        adc_binary(m, (uint8_t)~v);
        return;
    }
    uint32_t a = m->cpu.a;
    uint32_t c = m->cpu.p & FLAG_C;
    uint32_t al = (a & 0x0Fu) + c - (v & 0x0Fu) - 1u;
    if ((al & 0x80000000u) != 0u) {
        al = ((al - 0x06u) & 0x0Fu) - 0x10u;
    }
    uint32_t s = (a & 0xF0u) - (v & 0xF0u) + al;
    if ((s & 0x80000000u) != 0u) {
        s -= 0x60u;
    }
    adc_binary(m, (uint8_t)~v);
    m->cpu.a = (uint8_t)s;
}

static void ora(muhtari *m, uint8_t v)
{
    assign(m, &m->cpu.a, (uint8_t)(m->cpu.a | v));
}

static void and_op(muhtari *m, uint8_t v)
{
    assign(m, &m->cpu.a, (uint8_t)(m->cpu.a & v));
}

static void eor(muhtari *m, uint8_t v)
{
    assign(m, &m->cpu.a, (uint8_t)(m->cpu.a ^ v));
}

/* CMP, CPX, CPY: C when reg >= v unsigned; N and Z from reg - v. */
static void cmp_reg(muhtari *m, uint8_t reg, uint8_t v)
{
    set_flag(m, FLAG_C, reg >= v);
    set_nz(m, (uint8_t)(reg - v));
}

/* BIT: Z from A AND v; N and V copy bits 7 and 6 of v. */
static void bit_op(muhtari *m, uint8_t v)
{
    set_flag(m, FLAG_Z, (m->cpu.a & v) == 0u);
    set_flag(m, FLAG_N, (v & 0x80u) != 0u);
    set_flag(m, FLAG_V, (v & 0x40u) != 0u);
}

/* Register increments and decrements wrap at 8 bits. */
static void step_reg(muhtari *m, uint8_t *reg, uint8_t delta)
{
    last_implied(m);
    assign(m, reg, (uint8_t)(*reg + delta));
}

/* Flag instructions: one dummy cycle, then one bit of P. */
static void flag_op(muhtari *m, uint8_t mask, int cond)
{
    last_implied(m);
    set_flag(m, mask, cond);
}

/* Transfers between registers: one dummy cycle. */
static void transfer(muhtari *m, uint8_t *dst, uint8_t v)
{
    last_implied(m);
    assign(m, dst, v);
}

/* Shifts and rotates for the accumulator and read-modify-write forms. Each
   touches only N, Z and C. */
typedef uint8_t (*rmw_op)(muhtari *m, uint8_t v);

/* ASL: bit 7 goes to C, a zero comes in at bit 0. */
static uint8_t asl(muhtari *m, uint8_t v)
{
    uint8_t r = (uint8_t)(v << 1);
    set_flag(m, FLAG_C, (v & 0x80u) != 0u);
    set_nz(m, r);
    return r;
}

/* LSR: bit 0 goes to C, a zero comes in at bit 7. */
static uint8_t lsr(muhtari *m, uint8_t v)
{
    uint8_t r = (uint8_t)(v >> 1);
    set_flag(m, FLAG_C, (v & 0x01u) != 0u);
    set_nz(m, r);
    return r;
}

/* ROL: bit 7 goes to C, the old C comes in at bit 0. */
static uint8_t rol(muhtari *m, uint8_t v)
{
    uint8_t r = (uint8_t)((uint32_t)(v << 1) | (m->cpu.p & FLAG_C));
    set_flag(m, FLAG_C, (v & 0x80u) != 0u);
    set_nz(m, r);
    return r;
}

/* ROR: bit 0 goes to C, the old C comes in at bit 7. */
static uint8_t ror(muhtari *m, uint8_t v)
{
    uint8_t r = (uint8_t)((uint32_t)(v >> 1) | ((uint32_t)(m->cpu.p & FLAG_C) << 7));
    set_flag(m, FLAG_C, (v & 0x01u) != 0u);
    set_nz(m, r);
    return r;
}

/* INC and DEC in memory: N and Z only. */
static uint8_t inc(muhtari *m, uint8_t v)
{
    uint8_t r = (uint8_t)(v + 1u);
    set_nz(m, r);
    return r;
}

static uint8_t dec(muhtari *m, uint8_t v)
{
    uint8_t r = (uint8_t)(v - 1u);
    set_nz(m, r);
    return r;
}

/* The combined unofficial read-modify-writes: the shift, rotate, increment
   or decrement writes memory, then its result feeds an ALU operation on A
   (docs/hardware/sally.md, Unofficial opcodes). The timing is that of rmw.
   SLO: ASL then ORA. RLA: ROL then AND. SRE: LSR then EOR. RRA: ROR then ADC
   with the C that ROR left. DCP: DEC then CMP. ISC: INC then SBC. */
static uint8_t slo(muhtari *m, uint8_t v)
{
    uint8_t r = asl(m, v);
    ora(m, r);
    return r;
}

static uint8_t rla(muhtari *m, uint8_t v)
{
    uint8_t r = rol(m, v);
    and_op(m, r);
    return r;
}

static uint8_t sre(muhtari *m, uint8_t v)
{
    uint8_t r = lsr(m, v);
    eor(m, r);
    return r;
}

static uint8_t rra(muhtari *m, uint8_t v)
{
    uint8_t r = ror(m, v);
    adc(m, r);
    return r;
}

static uint8_t dcp(muhtari *m, uint8_t v)
{
    uint8_t r = (uint8_t)(v - 1u);
    cmp_reg(m, m->cpu.a, r);
    return r;
}

static uint8_t isc(muhtari *m, uint8_t v)
{
    uint8_t r = (uint8_t)(v + 1u);
    sbc(m, r);
    return r;
}

/* LAX: A and X both take the value read; N and Z follow it. */
static void lax(muhtari *m, uint8_t v)
{
    m->cpu.x = v;
    assign(m, &m->cpu.a, v);
}

/* ANC: AND, then C copies N. */
static void anc(muhtari *m, uint8_t v)
{
    and_op(m, v);
    set_flag(m, FLAG_C, (m->cpu.a & 0x80u) != 0u);
}

/* ARR: t = A AND v, then A = t rotated right through C. N and Z follow A;
   C is bit 6 of A and V is bit 6 XOR bit 5. No other flag changes.
   With D set, N and Z still follow the rotated value and V is bit 6 of
   t XOR A. Then each digit of t is tested: when the low digit plus its own
   bit 0 exceeds 5, the low digit of A gains 6; when the high digit plus its
   own bit 0 exceeds 5, C is set and A gains 0x60; otherwise C is clear
   (64doc; docs/hardware/sally.md, Decimal mode). */
static void arr(muhtari *m, uint8_t v)
{
    uint32_t t = (uint32_t)(m->cpu.a & v);
    uint8_t r = (uint8_t)((t >> 1) | ((uint32_t)(m->cpu.p & FLAG_C) << 7));
    assign(m, &m->cpu.a, r);
    if ((m->cpu.p & FLAG_D) == 0u) {
        set_flag(m, FLAG_C, (r & 0x40u) != 0u);
        set_flag(m, FLAG_V, ((((uint32_t)r >> 6) ^ ((uint32_t)r >> 5)) & 1u) != 0u);
        return;
    }
    set_flag(m, FLAG_V, ((t ^ r) & 0x40u) != 0u);
    uint32_t lo = t & 0x0Fu;
    uint32_t hi = t >> 4;
    uint32_t a = r;
    if (lo + (lo & 1u) > 5u) {
        a = (a & 0xF0u) | ((a + 0x06u) & 0x0Fu);
    }
    int carry = hi + (hi & 1u) > 5u;
    set_flag(m, FLAG_C, carry);
    if (carry) {
        a += 0x60u;
    }
    m->cpu.a = (uint8_t)a;
}

/* SBX: X = (A AND X) - v without borrow in; C, N and Z as CMP sets them. */
static void sbx(muhtari *m, uint8_t v)
{
    uint8_t t = (uint8_t)(m->cpu.a & m->cpu.x);
    cmp_reg(m, t, v);
    m->cpu.x = (uint8_t)(t - v);
}

/* The accumulator forms of ASL, LSR, ROL and ROR: one dummy cycle. */
static void accumulator(muhtari *m, rmw_op op)
{
    last_implied(m);
    m->cpu.a = op(m, m->cpu.a);
}

/* Read-modify-write at ea: the CPU reads v, writes v back unchanged while it
   computes, then writes the result (64doc, "Read-Modify-Write instructions").
   Both writes are real bus accesses. */
static void rmw(muhtari *m, uint16_t ea, rmw_op op)
{
    uint8_t v = muhtari__bus_read(m, ea);
    muhtari__bus_write(m, ea, v);
    uint8_t r = op(m, v);
    last_write(m, ea, r);
}

/* The stack is page 1. A push writes at 0x0100 + S and then decrements S; a
   pull increments S and then reads (64doc, "stack"). S wraps within the
   page. */
static void push(muhtari *m, uint8_t v)
{
    muhtari__bus_write(m, (uint16_t)(0x0100u | m->cpu.s), v);
    m->cpu.s = (uint8_t)(m->cpu.s - 1u);
}

static uint8_t pull(muhtari *m)
{
    m->cpu.s = (uint8_t)(m->cpu.s + 1u);
    return muhtari__bus_read(m, (uint16_t)(0x0100u | m->cpu.s));
}

/* The dummy read of the stack at 0x0100 + S that PLA, PLP, RTS, RTI and JSR
   make before S moves. */
static void stack_dummy(muhtari *m)
{
    muhtari__bus_read(m, (uint16_t)(0x0100u | m->cpu.s));
}

/* Conditional branches: the offset byte, then, when taken, a dummy read at
   PC while the low byte is added, and a second dummy read at
   (old PCH << 8) | new PCL when the target is in another page (64doc,
   "Relative addressing"). The offset is signed.
   The deciding cycle is cycle 1, the opcode fetch, when the branch is not
   taken or is taken within the page, and cycle 3 when it crosses a page
   (docs/hardware/sally.md, NMI). */
static void branch(muhtari *m, int cond)
{
    uint64_t opcode_done = m->bus.phi2;
    uint8_t off = fetch(m);
    if (!cond) {
        m->cpu.poll_phi2 = opcode_done;
        return;
    }
    implied(m);
    uint16_t pc = m->cpu.pc;
    uint32_t delta = (off & 0x80u) != 0u ? (uint32_t)off + 0xFF00u : (uint32_t)off;
    uint16_t target = (uint16_t)(pc + delta);
    if ((target & 0xFF00u) != (pc & 0xFF00u)) {
        last_read(m, (uint16_t)((pc & 0xFF00u) | (target & 0x00FFu)));
    } else {
        m->cpu.poll_phi2 = opcode_done;
    }
    m->cpu.pc = target;
}

/* PLP and RTI: bit 4 clear and bit 5 set whatever the stack held
   (docs/hardware/sally.md, Status register). */
static void pull_p(muhtari *m)
{
    uint8_t v = pull(m);
    m->cpu.p = (uint8_t)((v & ~FLAG_B) | FLAG_U);
}

/* PHP and BRK push P with bits 4 and 5 set; P itself is unchanged. */
static void push_p(muhtari *m)
{
    push(m, (uint8_t)(m->cpu.p | FLAG_B | FLAG_U));
}

/* Reads a little-endian address: low byte at lo_addr, high byte at
   hi_addr. */
static uint16_t read_addr(muhtari *m, uint16_t lo_addr, uint16_t hi_addr)
{
    uint8_t lo = muhtari__bus_read(m, lo_addr);
    uint8_t hi = muhtari__bus_read(m, hi_addr);
    return (uint16_t)(((uint32_t)hi << 8) | lo);
}

/* JAM (0x02 0x12 0x22 0x32 0x42 0x52 0x62 0x72 0x92 0xB2 0xD2 0xF2): after
   the opcode, a read of the next byte that leaves PC alone, then reads of
   0xFFFF, 0xFFFE, 0xFFFE and six of 0xFFFF, eleven in all. No write, no
   register or flag change; the CPU stays jammed until reset
   (docs/hardware/sally.md, JAM). */
static void jam(muhtari *m)
{
    muhtari__bus_read(m, m->cpu.pc);
    muhtari__bus_read(m, 0xFFFFu);
    muhtari__bus_read(m, 0xFFFEu);
    muhtari__bus_read(m, 0xFFFEu);
    for (uint32_t i = 0u; i < 6u; i++) {
        muhtari__bus_read(m, 0xFFFFu);
    }
    m->cpu.jammed = 1u;
}

/* SHY, SHX, SHA and TAS store reg AND (H + 1), where hi:lo is the base
   address before idx is added. The dummy read at the uncorrected address is
   always made. On a page cross the stored value also replaces the high byte
   of the address (docs/hardware/sally.md, Unstable opcodes). */
static void store_sh(muhtari *m, uint8_t reg, uint8_t lo, uint8_t hi, uint8_t idx)
{
    uint32_t sum = (uint32_t)lo + idx;
    uint8_t value = (uint8_t)(reg & (uint8_t)(hi + 1u));
    uint8_t high = sum > 0xFFu ? value : hi;
    muhtari__bus_read(m, (uint16_t)(((uint32_t)hi << 8) | (sum & 0xFFu)));
    last_write(m, (uint16_t)(((uint32_t)high << 8) | (sum & 0xFFu)), value);
}

/* The abs,X and abs,Y forms of store_sh: the two address bytes, then the
   store. */
static void store_sh_abs(muhtari *m, uint8_t reg, uint8_t idx)
{
    uint8_t lo = fetch(m);
    uint8_t hi = fetch(m);
    store_sh(m, reg, lo, hi, idx);
}

/* Nonzero when an NMI edge is latched at or before the poll mark. */
static int nmi_due(const muhtari *m)
{
    return m->bus.nmi_phi2 != 0u && m->bus.nmi_phi2 <= m->cpu.poll_phi2;
}

/* The NMI entry, seven cycles: two dummy reads at PC, PCH, PCL and P pushed
   with bit 4 clear and bit 5 set, I set, then PC from 0xFFFA/0xFFFB. The edge
   is consumed before the first cycle, and the entry stores no poll mark, so
   the handler's first instruction always runs and an edge latched during the
   entry waits for the next poll (docs/hardware/sally.md, NMI). */
static void nmi(muhtari *m)
{
    m->bus.nmi_phi2 = 0u;
    muhtari__bus_read(m, m->cpu.pc);
    muhtari__bus_read(m, m->cpu.pc);
    push(m, (uint8_t)(m->cpu.pc >> 8));
    push(m, (uint8_t)m->cpu.pc);
    push(m, (uint8_t)((m->cpu.p & ~FLAG_B) | FLAG_U));
    set_flag(m, FLAG_I, 1);
    m->cpu.pc = read_addr(m, 0xFFFAu, 0xFFFBu);
}

/* RES, run once at power-on under a BIOS: like the NMI entry, but the
   three pushes are reads, so S steps down by 3 with nothing written, from
   the power-on 0 to $FD. I is set and PC comes from $FFFC/$FFFD
   (docs/hardware/sally.md, Reset; A7800-SYSTEM section 8). */
void muhtari__cpu_reset(muhtari *m)
{
    m->cpu.jammed = 0u;
    muhtari__bus_read(m, m->cpu.pc);
    muhtari__bus_read(m, m->cpu.pc);
    for (int i = 0; i < 3; i++) {
        stack_dummy(m);
        m->cpu.s = (uint8_t)(m->cpu.s - 1u);
    }
    m->cpu.p = (uint8_t)(m->cpu.p | FLAG_I | FLAG_U);
    m->cpu.pc = read_addr(m, 0xFFFCu, 0xFFFDu);
}

void muhtari__cpu_step(muhtari *m)
{
    if (m->cpu.jammed != 0u) {
        /* A jammed CPU keeps reading $FFFF, as the vectors' last reads do,
           and takes no NMI: one read per step keeps time moving and changes
           no register (docs/hardware/sally.md, JAM). */
        muhtari__bus_read(m, 0xFFFFu);
        return;
    }
    m->bus.sync = 1u; /* the opcode fetch: SYNC high for this read */
    uint8_t opcode = fetch(m);
    switch (opcode) {
    /* Loads. */
    case 0xA9: /* LDA immediate */
        assign(m, &m->cpu.a, last_fetch(m));
        break;
    case 0xA5: /* LDA zp */
        load(m, &m->cpu.a, ea_zp(m));
        break;
    case 0xB5: /* LDA zp,X */
        load(m, &m->cpu.a, ea_zpx(m));
        break;
    case 0xAD: /* LDA abs */
        load(m, &m->cpu.a, ea_abs(m));
        break;
    case 0xBD: /* LDA abs,X */
        load(m, &m->cpu.a, ea_absi(m, m->cpu.x, 0));
        break;
    case 0xB9: /* LDA abs,Y */
        load(m, &m->cpu.a, ea_absi(m, m->cpu.y, 0));
        break;
    case 0xA1: /* LDA (zp,X) */
        load(m, &m->cpu.a, ea_izx(m));
        break;
    case 0xB1: /* LDA (zp),Y */
        load(m, &m->cpu.a, ea_izy(m, 0));
        break;
    case 0xA2: /* LDX immediate */
        assign(m, &m->cpu.x, last_fetch(m));
        break;
    case 0xA6: /* LDX zp */
        load(m, &m->cpu.x, ea_zp(m));
        break;
    case 0xB6: /* LDX zp,Y */
        load(m, &m->cpu.x, ea_zpy(m));
        break;
    case 0xAE: /* LDX abs */
        load(m, &m->cpu.x, ea_abs(m));
        break;
    case 0xBE: /* LDX abs,Y */
        load(m, &m->cpu.x, ea_absi(m, m->cpu.y, 0));
        break;
    case 0xA0: /* LDY immediate */
        assign(m, &m->cpu.y, last_fetch(m));
        break;
    case 0xA4: /* LDY zp */
        load(m, &m->cpu.y, ea_zp(m));
        break;
    case 0xB4: /* LDY zp,X */
        load(m, &m->cpu.y, ea_zpx(m));
        break;
    case 0xAC: /* LDY abs */
        load(m, &m->cpu.y, ea_abs(m));
        break;
    case 0xBC: /* LDY abs,X */
        load(m, &m->cpu.y, ea_absi(m, m->cpu.x, 0));
        break;

    /* Stores. abs,X, abs,Y and (zp),Y always make the dummy read. */
    case 0x85: /* STA zp */
        last_write(m, ea_zp(m), m->cpu.a);
        break;
    case 0x95: /* STA zp,X */
        last_write(m, ea_zpx(m), m->cpu.a);
        break;
    case 0x8D: /* STA abs */
        last_write(m, ea_abs(m), m->cpu.a);
        break;
    case 0x9D: /* STA abs,X */
        last_write(m, ea_absi(m, m->cpu.x, 1), m->cpu.a);
        break;
    case 0x99: /* STA abs,Y */
        last_write(m, ea_absi(m, m->cpu.y, 1), m->cpu.a);
        break;
    case 0x81: /* STA (zp,X) */
        last_write(m, ea_izx(m), m->cpu.a);
        break;
    case 0x91: /* STA (zp),Y */
        last_write(m, ea_izy(m, 1), m->cpu.a);
        break;
    case 0x86: /* STX zp */
        last_write(m, ea_zp(m), m->cpu.x);
        break;
    case 0x96: /* STX zp,Y */
        last_write(m, ea_zpy(m), m->cpu.x);
        break;
    case 0x8E: /* STX abs */
        last_write(m, ea_abs(m), m->cpu.x);
        break;
    case 0x84: /* STY zp */
        last_write(m, ea_zp(m), m->cpu.y);
        break;
    case 0x94: /* STY zp,X */
        last_write(m, ea_zpx(m), m->cpu.y);
        break;
    case 0x8C: /* STY abs */
        last_write(m, ea_abs(m), m->cpu.y);
        break;

    /* Transfers. TXS sets no flag. */
    case 0xAA: /* TAX */
        transfer(m, &m->cpu.x, m->cpu.a);
        break;
    case 0xA8: /* TAY */
        transfer(m, &m->cpu.y, m->cpu.a);
        break;
    case 0x8A: /* TXA */
        transfer(m, &m->cpu.a, m->cpu.x);
        break;
    case 0x98: /* TYA */
        transfer(m, &m->cpu.a, m->cpu.y);
        break;
    case 0xBA: /* TSX */
        transfer(m, &m->cpu.x, m->cpu.s);
        break;
    case 0x9A: /* TXS */
        last_implied(m);
        m->cpu.s = m->cpu.x;
        break;

    /* ORA */
    case 0x09:
        ora(m, last_fetch(m));
        break;
    case 0x05:
        ora(m, read_at(m, ea_zp(m)));
        break;
    case 0x15:
        ora(m, read_at(m, ea_zpx(m)));
        break;
    case 0x0D:
        ora(m, read_at(m, ea_abs(m)));
        break;
    case 0x1D:
        ora(m, read_at(m, ea_absi(m, m->cpu.x, 0)));
        break;
    case 0x19:
        ora(m, read_at(m, ea_absi(m, m->cpu.y, 0)));
        break;
    case 0x01:
        ora(m, read_at(m, ea_izx(m)));
        break;
    case 0x11:
        ora(m, read_at(m, ea_izy(m, 0)));
        break;

    /* AND */
    case 0x29:
        and_op(m, last_fetch(m));
        break;
    case 0x25:
        and_op(m, read_at(m, ea_zp(m)));
        break;
    case 0x35:
        and_op(m, read_at(m, ea_zpx(m)));
        break;
    case 0x2D:
        and_op(m, read_at(m, ea_abs(m)));
        break;
    case 0x3D:
        and_op(m, read_at(m, ea_absi(m, m->cpu.x, 0)));
        break;
    case 0x39:
        and_op(m, read_at(m, ea_absi(m, m->cpu.y, 0)));
        break;
    case 0x21:
        and_op(m, read_at(m, ea_izx(m)));
        break;
    case 0x31:
        and_op(m, read_at(m, ea_izy(m, 0)));
        break;

    /* EOR */
    case 0x49:
        eor(m, last_fetch(m));
        break;
    case 0x45:
        eor(m, read_at(m, ea_zp(m)));
        break;
    case 0x55:
        eor(m, read_at(m, ea_zpx(m)));
        break;
    case 0x4D:
        eor(m, read_at(m, ea_abs(m)));
        break;
    case 0x5D:
        eor(m, read_at(m, ea_absi(m, m->cpu.x, 0)));
        break;
    case 0x59:
        eor(m, read_at(m, ea_absi(m, m->cpu.y, 0)));
        break;
    case 0x41:
        eor(m, read_at(m, ea_izx(m)));
        break;
    case 0x51:
        eor(m, read_at(m, ea_izy(m, 0)));
        break;

    /* ADC */
    case 0x69:
        adc(m, last_fetch(m));
        break;
    case 0x65:
        adc(m, read_at(m, ea_zp(m)));
        break;
    case 0x75:
        adc(m, read_at(m, ea_zpx(m)));
        break;
    case 0x6D:
        adc(m, read_at(m, ea_abs(m)));
        break;
    case 0x7D:
        adc(m, read_at(m, ea_absi(m, m->cpu.x, 0)));
        break;
    case 0x79:
        adc(m, read_at(m, ea_absi(m, m->cpu.y, 0)));
        break;
    case 0x61:
        adc(m, read_at(m, ea_izx(m)));
        break;
    case 0x71:
        adc(m, read_at(m, ea_izy(m, 0)));
        break;

    /* SBC */
    case 0xE9:
        sbc(m, last_fetch(m));
        break;
    case 0xE5:
        sbc(m, read_at(m, ea_zp(m)));
        break;
    case 0xF5:
        sbc(m, read_at(m, ea_zpx(m)));
        break;
    case 0xED:
        sbc(m, read_at(m, ea_abs(m)));
        break;
    case 0xFD:
        sbc(m, read_at(m, ea_absi(m, m->cpu.x, 0)));
        break;
    case 0xF9:
        sbc(m, read_at(m, ea_absi(m, m->cpu.y, 0)));
        break;
    case 0xE1:
        sbc(m, read_at(m, ea_izx(m)));
        break;
    case 0xF1:
        sbc(m, read_at(m, ea_izy(m, 0)));
        break;

    /* CMP */
    case 0xC9:
        cmp_reg(m, m->cpu.a, last_fetch(m));
        break;
    case 0xC5:
        cmp_reg(m, m->cpu.a, read_at(m, ea_zp(m)));
        break;
    case 0xD5:
        cmp_reg(m, m->cpu.a, read_at(m, ea_zpx(m)));
        break;
    case 0xCD:
        cmp_reg(m, m->cpu.a, read_at(m, ea_abs(m)));
        break;
    case 0xDD:
        cmp_reg(m, m->cpu.a, read_at(m, ea_absi(m, m->cpu.x, 0)));
        break;
    case 0xD9:
        cmp_reg(m, m->cpu.a, read_at(m, ea_absi(m, m->cpu.y, 0)));
        break;
    case 0xC1:
        cmp_reg(m, m->cpu.a, read_at(m, ea_izx(m)));
        break;
    case 0xD1:
        cmp_reg(m, m->cpu.a, read_at(m, ea_izy(m, 0)));
        break;

    /* CPX and CPY */
    case 0xE0:
        cmp_reg(m, m->cpu.x, last_fetch(m));
        break;
    case 0xE4:
        cmp_reg(m, m->cpu.x, read_at(m, ea_zp(m)));
        break;
    case 0xEC:
        cmp_reg(m, m->cpu.x, read_at(m, ea_abs(m)));
        break;
    case 0xC0:
        cmp_reg(m, m->cpu.y, last_fetch(m));
        break;
    case 0xC4:
        cmp_reg(m, m->cpu.y, read_at(m, ea_zp(m)));
        break;
    case 0xCC:
        cmp_reg(m, m->cpu.y, read_at(m, ea_abs(m)));
        break;

    /* BIT */
    case 0x24:
        bit_op(m, read_at(m, ea_zp(m)));
        break;
    case 0x2C:
        bit_op(m, read_at(m, ea_abs(m)));
        break;

    /* Flags. */
    case 0x18: /* CLC */
        flag_op(m, FLAG_C, 0);
        break;
    case 0x38: /* SEC */
        flag_op(m, FLAG_C, 1);
        break;
    case 0x58: /* CLI */
        flag_op(m, FLAG_I, 0);
        break;
    case 0x78: /* SEI */
        flag_op(m, FLAG_I, 1);
        break;
    case 0xB8: /* CLV */
        flag_op(m, FLAG_V, 0);
        break;
    case 0xD8: /* CLD */
        flag_op(m, FLAG_D, 0);
        break;
    case 0xF8: /* SED */
        flag_op(m, FLAG_D, 1);
        break;

    /* Register increments and decrements. */
    case 0xE8: /* INX */
        step_reg(m, &m->cpu.x, 0x01u);
        break;
    case 0xC8: /* INY */
        step_reg(m, &m->cpu.y, 0x01u);
        break;
    case 0xCA: /* DEX */
        step_reg(m, &m->cpu.x, 0xFFu);
        break;
    case 0x88: /* DEY */
        step_reg(m, &m->cpu.y, 0xFFu);
        break;

    case 0xEA: /* NOP */
        last_implied(m);
        break;

    /* Shifts and rotates. Read-modify-write abs,X always makes the dummy
       read. */
    case 0x0A: /* ASL A */
        accumulator(m, asl);
        break;
    case 0x06: /* ASL zp */
        rmw(m, ea_zp(m), asl);
        break;
    case 0x16: /* ASL zp,X */
        rmw(m, ea_zpx(m), asl);
        break;
    case 0x0E: /* ASL abs */
        rmw(m, ea_abs(m), asl);
        break;
    case 0x1E: /* ASL abs,X */
        rmw(m, ea_absi(m, m->cpu.x, 1), asl);
        break;
    case 0x4A: /* LSR A */
        accumulator(m, lsr);
        break;
    case 0x46: /* LSR zp */
        rmw(m, ea_zp(m), lsr);
        break;
    case 0x56: /* LSR zp,X */
        rmw(m, ea_zpx(m), lsr);
        break;
    case 0x4E: /* LSR abs */
        rmw(m, ea_abs(m), lsr);
        break;
    case 0x5E: /* LSR abs,X */
        rmw(m, ea_absi(m, m->cpu.x, 1), lsr);
        break;
    case 0x2A: /* ROL A */
        accumulator(m, rol);
        break;
    case 0x26: /* ROL zp */
        rmw(m, ea_zp(m), rol);
        break;
    case 0x36: /* ROL zp,X */
        rmw(m, ea_zpx(m), rol);
        break;
    case 0x2E: /* ROL abs */
        rmw(m, ea_abs(m), rol);
        break;
    case 0x3E: /* ROL abs,X */
        rmw(m, ea_absi(m, m->cpu.x, 1), rol);
        break;
    case 0x6A: /* ROR A */
        accumulator(m, ror);
        break;
    case 0x66: /* ROR zp */
        rmw(m, ea_zp(m), ror);
        break;
    case 0x76: /* ROR zp,X */
        rmw(m, ea_zpx(m), ror);
        break;
    case 0x6E: /* ROR abs */
        rmw(m, ea_abs(m), ror);
        break;
    case 0x7E: /* ROR abs,X */
        rmw(m, ea_absi(m, m->cpu.x, 1), ror);
        break;

    /* INC and DEC in memory. */
    case 0xE6: /* INC zp */
        rmw(m, ea_zp(m), inc);
        break;
    case 0xF6: /* INC zp,X */
        rmw(m, ea_zpx(m), inc);
        break;
    case 0xEE: /* INC abs */
        rmw(m, ea_abs(m), inc);
        break;
    case 0xFE: /* INC abs,X */
        rmw(m, ea_absi(m, m->cpu.x, 1), inc);
        break;
    case 0xC6: /* DEC zp */
        rmw(m, ea_zp(m), dec);
        break;
    case 0xD6: /* DEC zp,X */
        rmw(m, ea_zpx(m), dec);
        break;
    case 0xCE: /* DEC abs */
        rmw(m, ea_abs(m), dec);
        break;
    case 0xDE: /* DEC abs,X */
        rmw(m, ea_absi(m, m->cpu.x, 1), dec);
        break;

    /* Stack. */
    case 0x48: /* PHA */
        implied(m);
        poll_mark(m);
        push(m, m->cpu.a);
        break;
    case 0x08: /* PHP */
        implied(m);
        poll_mark(m);
        push_p(m);
        break;
    case 0x68: /* PLA */
        implied(m);
        stack_dummy(m);
        poll_mark(m);
        assign(m, &m->cpu.a, pull(m));
        break;
    case 0x28: /* PLP */
        implied(m);
        stack_dummy(m);
        poll_mark(m);
        pull_p(m);
        break;

    /* Branches. */
    case 0x10: /* BPL */
        branch(m, (m->cpu.p & FLAG_N) == 0u);
        break;
    case 0x30: /* BMI */
        branch(m, (m->cpu.p & FLAG_N) != 0u);
        break;
    case 0x50: /* BVC */
        branch(m, (m->cpu.p & FLAG_V) == 0u);
        break;
    case 0x70: /* BVS */
        branch(m, (m->cpu.p & FLAG_V) != 0u);
        break;
    case 0x90: /* BCC */
        branch(m, (m->cpu.p & FLAG_C) == 0u);
        break;
    case 0xB0: /* BCS */
        branch(m, (m->cpu.p & FLAG_C) != 0u);
        break;
    case 0xD0: /* BNE */
        branch(m, (m->cpu.p & FLAG_Z) == 0u);
        break;
    case 0xF0: /* BEQ */
        branch(m, (m->cpu.p & FLAG_Z) != 0u);
        break;

    /* Jumps. */
    case 0x4C: { /* JMP abs */
        uint8_t lo = fetch(m);
        uint8_t hi = last_fetch(m);
        m->cpu.pc = (uint16_t)(((uint32_t)hi << 8) | lo);
        break;
    }
    case 0x6C: { /* JMP (ind): the pointer's high byte comes from the same
                    page, so a pointer at xxFF reads xx00 (64doc). */
        uint16_t ptr = ea_abs(m);
        uint16_t next = (uint16_t)((ptr & 0xFF00u) | ((ptr + 1u) & 0x00FFu));
        uint8_t lo = muhtari__bus_read(m, ptr);
        uint8_t hi = last_read(m, next);
        m->cpu.pc = (uint16_t)(((uint32_t)hi << 8) | lo);
        break;
    }

    /* RTS: a dummy read at PC, a dummy stack read, PCL and PCH pulled, a
       dummy read at the pulled address, then PC steps past it. */
    case 0x60: {
        implied(m);
        stack_dummy(m);
        uint8_t lo = pull(m);
        uint8_t hi = pull(m);
        m->cpu.pc = (uint16_t)(((uint32_t)hi << 8) | lo);
        last_fetch(m);
        break;
    }

    /* RTI: a dummy read at PC, a dummy stack read, then P, PCL and PCH
       pulled. */
    case 0x40: {
        implied(m);
        stack_dummy(m);
        pull_p(m);
        uint8_t lo = pull(m);
        poll_mark(m);
        uint8_t hi = pull(m);
        m->cpu.pc = (uint16_t)(((uint32_t)hi << 8) | lo);
        break;
    }

    /* BRK: the byte after the opcode is read and skipped, the return
       address and P | 0x30 are pushed, I is set and D is left alone, and PC
       loads from 0xFFFE/0xFFFF. Its deciding cycle is cycle 4, the push of
       PCL: an NMI latched by then takes the vector at 0xFFFA instead, with B
       still set in the pushed P, and is consumed. A later one waits for the
       handler's first instruction (docs/hardware/sally.md, NMI). */
    case 0x00: {
        fetch(m);
        push(m, (uint8_t)(m->cpu.pc >> 8));
        push(m, (uint8_t)m->cpu.pc);
        poll_mark(m);
        push_p(m);
        set_flag(m, FLAG_I, 1);
        uint16_t vector = 0xFFFEu;
        if (nmi_due(m)) {
            m->bus.nmi_phi2 = 0u;
            vector = 0xFFFAu;
        }
        m->cpu.pc = read_addr(m, vector, (uint16_t)(vector + 1u));
        break;
    }

    /* JSR: the low address byte, a dummy stack read, the return address
       (the last operand byte) pushed high byte first, then the high address
       byte, read after the pushes so a push over the operand is seen. */
    case 0x20: {
        uint8_t lo = fetch(m);
        stack_dummy(m);
        push(m, (uint8_t)(m->cpu.pc >> 8));
        push(m, (uint8_t)m->cpu.pc);
        uint8_t hi = last_fetch(m);
        m->cpu.pc = (uint16_t)(((uint32_t)hi << 8) | lo);
        break;
    }

    /* Unofficial NOPs (docs/hardware/sally.md, Unofficial opcodes). Each
       makes the bus accesses of its addressing mode and changes nothing; the
       memory forms read the operand, and abs,X makes the dummy read only on a
       page cross, as a read does. */
    case 0x1A:
    case 0x3A:
    case 0x5A:
    case 0x7A:
    case 0xDA:
    case 0xFA:
        last_implied(m);
        break;
    case 0x80:
    case 0x82:
    case 0x89:
    case 0xC2:
    case 0xE2:
        last_fetch(m);
        break;
    case 0x04:
    case 0x44:
    case 0x64:
        read_at(m, ea_zp(m));
        break;
    case 0x14:
    case 0x34:
    case 0x54:
    case 0x74:
    case 0xD4:
    case 0xF4:
        read_at(m, ea_zpx(m));
        break;
    case 0x0C:
        read_at(m, ea_abs(m));
        break;
    case 0x1C:
    case 0x3C:
    case 0x5C:
    case 0x7C:
    case 0xDC:
    case 0xFC:
        read_at(m, ea_absi(m, m->cpu.x, 0));
        break;

    /* LAX: load A and X. */
    case 0xA7: /* LAX zp */
        lax(m, read_at(m, ea_zp(m)));
        break;
    case 0xB7: /* LAX zp,Y */
        lax(m, read_at(m, ea_zpy(m)));
        break;
    case 0xAF: /* LAX abs */
        lax(m, read_at(m, ea_abs(m)));
        break;
    case 0xBF: /* LAX abs,Y */
        lax(m, read_at(m, ea_absi(m, m->cpu.y, 0)));
        break;
    case 0xA3: /* LAX (zp,X) */
        lax(m, read_at(m, ea_izx(m)));
        break;
    case 0xB3: /* LAX (zp),Y */
        lax(m, read_at(m, ea_izy(m, 0)));
        break;

    /* SAX: store A AND X; no flag changes. */
    case 0x87: /* SAX zp */
        last_write(m, ea_zp(m), (uint8_t)(m->cpu.a & m->cpu.x));
        break;
    case 0x97: /* SAX zp,Y */
        last_write(m, ea_zpy(m), (uint8_t)(m->cpu.a & m->cpu.x));
        break;
    case 0x8F: /* SAX abs */
        last_write(m, ea_abs(m), (uint8_t)(m->cpu.a & m->cpu.x));
        break;
    case 0x83: /* SAX (zp,X) */
        last_write(m, ea_izx(m), (uint8_t)(m->cpu.a & m->cpu.x));
        break;

    /* Combined read-modify-writes. abs,X, abs,Y and (zp),Y always make the
       dummy read, as a read-modify-write does. */
    case 0x03: /* SLO (zp,X) */
        rmw(m, ea_izx(m), slo);
        break;
    case 0x07: /* SLO zp */
        rmw(m, ea_zp(m), slo);
        break;
    case 0x0F: /* SLO abs */
        rmw(m, ea_abs(m), slo);
        break;
    case 0x13: /* SLO (zp),Y */
        rmw(m, ea_izy(m, 1), slo);
        break;
    case 0x17: /* SLO zp,X */
        rmw(m, ea_zpx(m), slo);
        break;
    case 0x1B: /* SLO abs,Y */
        rmw(m, ea_absi(m, m->cpu.y, 1), slo);
        break;
    case 0x1F: /* SLO abs,X */
        rmw(m, ea_absi(m, m->cpu.x, 1), slo);
        break;
    case 0x23: /* RLA (zp,X) */
        rmw(m, ea_izx(m), rla);
        break;
    case 0x27: /* RLA zp */
        rmw(m, ea_zp(m), rla);
        break;
    case 0x2F: /* RLA abs */
        rmw(m, ea_abs(m), rla);
        break;
    case 0x33: /* RLA (zp),Y */
        rmw(m, ea_izy(m, 1), rla);
        break;
    case 0x37: /* RLA zp,X */
        rmw(m, ea_zpx(m), rla);
        break;
    case 0x3B: /* RLA abs,Y */
        rmw(m, ea_absi(m, m->cpu.y, 1), rla);
        break;
    case 0x3F: /* RLA abs,X */
        rmw(m, ea_absi(m, m->cpu.x, 1), rla);
        break;
    case 0x43: /* SRE (zp,X) */
        rmw(m, ea_izx(m), sre);
        break;
    case 0x47: /* SRE zp */
        rmw(m, ea_zp(m), sre);
        break;
    case 0x4F: /* SRE abs */
        rmw(m, ea_abs(m), sre);
        break;
    case 0x53: /* SRE (zp),Y */
        rmw(m, ea_izy(m, 1), sre);
        break;
    case 0x57: /* SRE zp,X */
        rmw(m, ea_zpx(m), sre);
        break;
    case 0x5B: /* SRE abs,Y */
        rmw(m, ea_absi(m, m->cpu.y, 1), sre);
        break;
    case 0x5F: /* SRE abs,X */
        rmw(m, ea_absi(m, m->cpu.x, 1), sre);
        break;
    case 0x63: /* RRA (zp,X) */
        rmw(m, ea_izx(m), rra);
        break;
    case 0x67: /* RRA zp */
        rmw(m, ea_zp(m), rra);
        break;
    case 0x6F: /* RRA abs */
        rmw(m, ea_abs(m), rra);
        break;
    case 0x73: /* RRA (zp),Y */
        rmw(m, ea_izy(m, 1), rra);
        break;
    case 0x77: /* RRA zp,X */
        rmw(m, ea_zpx(m), rra);
        break;
    case 0x7B: /* RRA abs,Y */
        rmw(m, ea_absi(m, m->cpu.y, 1), rra);
        break;
    case 0x7F: /* RRA abs,X */
        rmw(m, ea_absi(m, m->cpu.x, 1), rra);
        break;
    case 0xC3: /* DCP (zp,X) */
        rmw(m, ea_izx(m), dcp);
        break;
    case 0xC7: /* DCP zp */
        rmw(m, ea_zp(m), dcp);
        break;
    case 0xCF: /* DCP abs */
        rmw(m, ea_abs(m), dcp);
        break;
    case 0xD3: /* DCP (zp),Y */
        rmw(m, ea_izy(m, 1), dcp);
        break;
    case 0xD7: /* DCP zp,X */
        rmw(m, ea_zpx(m), dcp);
        break;
    case 0xDB: /* DCP abs,Y */
        rmw(m, ea_absi(m, m->cpu.y, 1), dcp);
        break;
    case 0xDF: /* DCP abs,X */
        rmw(m, ea_absi(m, m->cpu.x, 1), dcp);
        break;
    case 0xE3: /* ISC (zp,X) */
        rmw(m, ea_izx(m), isc);
        break;
    case 0xE7: /* ISC zp */
        rmw(m, ea_zp(m), isc);
        break;
    case 0xEF: /* ISC abs */
        rmw(m, ea_abs(m), isc);
        break;
    case 0xF3: /* ISC (zp),Y */
        rmw(m, ea_izy(m, 1), isc);
        break;
    case 0xF7: /* ISC zp,X */
        rmw(m, ea_zpx(m), isc);
        break;
    case 0xFB: /* ISC abs,Y */
        rmw(m, ea_absi(m, m->cpu.y, 1), isc);
        break;
    case 0xFF: /* ISC abs,X */
        rmw(m, ea_absi(m, m->cpu.x, 1), isc);
        break;

    /* Unofficial immediate ALU operations. */
    case 0x0B: /* ANC */
    case 0x2B:
        anc(m, last_fetch(m));
        break;
    case 0x4B: /* ALR: AND, then LSR of A */
        m->cpu.a = lsr(m, (uint8_t)(m->cpu.a & last_fetch(m)));
        break;
    case 0x6B: /* ARR */
        arr(m, last_fetch(m));
        break;
    case 0xCB: /* SBX */
        sbx(m, last_fetch(m));
        break;
    case 0xEB: /* SBC immediate */
        sbc(m, last_fetch(m));
        break;

    /* ANE and LXA OR A with a chip-dependent constant from the machine
       profile (docs/hardware/sally.md, Unstable opcodes). */
    case 0x8B: /* ANE */
        assign(m, &m->cpu.a,
               (uint8_t)((m->cpu.a | m->profile.ane_lxa_constant) & m->cpu.x & last_fetch(m)));
        break;
    case 0xAB: /* LXA */
        m->cpu.x = (uint8_t)((m->cpu.a | m->profile.ane_lxa_constant) & last_fetch(m));
        assign(m, &m->cpu.a, m->cpu.x);
        break;

    case 0xBB: { /* LAS abs,Y: S, A and X all take the value read AND S. */
        uint8_t r = (uint8_t)(read_at(m, ea_absi(m, m->cpu.y, 0)) & m->cpu.s);
        m->cpu.s = r;
        m->cpu.x = r;
        assign(m, &m->cpu.a, r);
        break;
    }

    /* The unstable stores, all through store_sh. */
    case 0x9C: /* SHY abs,X */
        store_sh_abs(m, m->cpu.y, m->cpu.x);
        break;
    case 0x9E: /* SHX abs,Y */
        store_sh_abs(m, m->cpu.x, m->cpu.y);
        break;
    case 0x9F: /* SHA abs,Y */
        store_sh_abs(m, (uint8_t)(m->cpu.a & m->cpu.x), m->cpu.y);
        break;
    case 0x9B: /* TAS abs,Y: S = A AND X first */
        m->cpu.s = (uint8_t)(m->cpu.a & m->cpu.x);
        store_sh_abs(m, m->cpu.s, m->cpu.y);
        break;
    case 0x93: { /* SHA (zp),Y: H is the pointer's high byte */
        uint8_t p = fetch(m);
        uint8_t lo = muhtari__bus_read(m, p);
        uint8_t hi = muhtari__bus_read(m, (uint8_t)(p + 1u));
        store_sh(m, (uint8_t)(m->cpu.a & m->cpu.x), lo, hi, m->cpu.y);
        break;
    }

    case 0x02:
    case 0x12:
    case 0x22:
    case 0x32:
    case 0x42:
    case 0x52:
    case 0x62:
    case 0x72:
    case 0x92:
    case 0xB2:
    case 0xD2:
    case 0xF2:
        /* A jammed CPU takes no NMI: only reset recovers it
           (docs/hardware/sally.md, JAM). */
        jam(m);
        return;
    }
    if (nmi_due(m)) {
        nmi(m);
    }
}
