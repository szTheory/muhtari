/* The runner's monotonic clock for --bench. Runner-only: the core reads no
   clock. */
#ifndef MUHTARI_RUNNER_CLOCK_H
#define MUHTARI_RUNNER_CLOCK_H

#include <stdint.h>

/* A monotonic reading in nanoseconds, or 0 when the clock cannot be read.
   Only the difference between two readings means anything. */
uint64_t muhtari__clock_ns(void);

#endif /* MUHTARI_RUNNER_CLOCK_H */
