/* The flat test bus for the CPU test programs: 64 KiB of RAM and a log of
   every bus cycle. It defines muhtari__bus_read and muhtari__bus_write in
   place of the core's bus, so a program that links it links the CPU object,
   never the library. */
#ifndef MUHTARI_VECTOR_BUS_H
#define MUHTARI_VECTOR_BUS_H

#include <stdint.h>

#include "internal.h"
#include "n65v.h"

#define VECTOR_LOG_CAPACITY 32u

/* m must stay the first member: the bus functions receive &vm->m and
   convert it back to the machine. */
struct vector_machine {
    struct muhtari m;
    uint8_t ram[65536];
    struct n65v_cycle log[VECTOR_LOG_CAPACITY];
    uint32_t log_count;
    uint8_t log_overflow; /* set when a cycle arrived with the log full */
};

/* Empties the cycle log and clears its overflow flag. */
void vector_machine_reset_log(struct vector_machine *vm);

#endif
