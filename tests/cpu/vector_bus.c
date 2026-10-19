/* Test definitions of the CPU's two bus functions: flat RAM, a cycle log,
   and the phi2 count the core's bus keeps. */
#include "vector_bus.h"

/* A pointer to a structure, suitably converted, points to its first member,
   and back (C17 6.7.2.1p15); m is the first member of vector_machine. */
static struct vector_machine *machine(muhtari *m)
{
    return (struct vector_machine *)(void *)m;
}

static void log_cycle(struct vector_machine *vm, uint16_t addr, uint8_t value, uint8_t kind)
{
    vm->m.bus.phi2++;
    if (vm->log_count >= VECTOR_LOG_CAPACITY) {
        vm->log_overflow = 1u;
        return;
    }
    struct n65v_cycle *c = &vm->log[vm->log_count];
    c->addr = addr;
    c->value = value;
    c->kind = kind;
    vm->log_count++;
}

uint8_t muhtari__bus_read(muhtari *m, uint16_t addr)
{
    struct vector_machine *vm = machine(m);
    uint8_t v = vm->ram[addr];
    log_cycle(vm, addr, v, N65V_KIND_READ);
    return v;
}

void muhtari__bus_write(muhtari *m, uint16_t addr, uint8_t v)
{
    struct vector_machine *vm = machine(m);
    vm->ram[addr] = v;
    log_cycle(vm, addr, v, N65V_KIND_WRITE);
}

void vector_machine_reset_log(struct vector_machine *vm)
{
    vm->log_count = 0u;
    vm->log_overflow = 0u;
}
