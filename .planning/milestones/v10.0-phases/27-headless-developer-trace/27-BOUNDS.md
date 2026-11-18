# Trace capture bounds and measurements

The trace selects one invocation frame and keeps capture in a runner-owned
array. The product objective is a useful all-address capture of a full PAL
frame, including the maximum instruction/interrupt overshoot, with a few MiB
of runner-only memory and a finite JSONL file. Capture never enters the
instance state or save format (D-07).

## Source-grounded event bound

`src/frame.c` runs CPU steps until `bus.ticks` reaches the absolute frame
deadline, then stops at an instruction boundary. The loop has no unbounded
wait: every instruction step includes at least one bus cycle of four ticks or
more, including a jammed CPU repeatedly reading `$FFFF`. The normal loop and
the traced loop use the same deadline and stop condition. BIOS rejection ends
both early. Thus a hostile or jammed cartridge cannot make one selected frame
run forever.

The largest supported frame is PAL at 142,102 ticks
(`src/internal.h`, `muhtari__timing`). The existing conservative
instruction/interrupt overshoot is 20,730 ticks:

```
(456 RDY + 2 * 454 DMA + 12 rounding + 6 access ticks)
    * (8 opcode + 7 interrupt accesses) = 20,730 ticks
```

These terms are defined by `MUHTARI__SYNTH_*` in `src/internal.h` and
explained in `docs/hardware/audio.md` (instruction-boundary overshoot). CPU
reads and writes pass through the trace producers in `src/bus.c`. A read
emits one bus event, and an opcode-fetch read can additionally emit one
instruction event; a write emits one bus event. Tracing adds no CPU access.
This gives a conservative full-frame access ceiling of
`ceil((142,102 + 20,730) / 4) = 40,708`, or at most 81,416 trace events.
The hard cap of 131,072 is above that source-derived ceiling. A selected first
post-load frame may be shorter; selection still uses invocation numbers.

## Chosen finite limits

The event stores a 64-bit tick, four 16-bit values, and eight byte values.
Its measured native `sizeof` is 24 bytes, enforced by the compile-time
`_Static_assert` in `src/internal.h`. The default 65,536-event allocation is
1,572,864 bytes (1.5 MiB); the 131,072-event hard maximum is 3,145,728 bytes
(3 MiB), below the 4 MiB capture objective. Allocation multiplication and the
capture/output ceilings are checked before allocation.

The serializer measures a maximum record of 149 bytes including its newline
when the tick is `UINT64_MAX` and every other integer field is at its maximum.
The 150-byte stack buffer also holds the terminating NUL. The record ceiling
is checked at compile time and the maximum-value line is exercised by
`runner.trace.memory`. At the hard cap, record lines occupy at most
`149 * 131,072 = 19,529,728` bytes, below the 19 MiB body objective
(19,922,944 bytes). The header and terminal footer are bounded by a combined
192 bytes, so the file is at most 19,529,920 bytes. Serialization streams
header, records, and footer through one bounded line buffer into an owned
sibling temporary; it does not allocate a second full-file buffer.

The candidate 32-byte event, 192-byte line, 2/4 MiB capture and 24 MiB body
were evaluated rather than assumed. The actual event is 24 bytes; synthetic
maximum fields require 149 bytes per record; and the chosen capture/output
limits remain below the stated objectives. A default cap remains useful for
ordinary captures but may truncate a denser valid frame; truncation is sticky
only after an additional matching event arrives, so exactly-full output is
complete and emulation continues.

## Comparable generated-workload evidence

`runner.trace.bounds` generates a build-directory cartridge that repeatedly
executes NOPs on an address bus and loops. It runs the selected second frame
under the same `ci` build for NTSC and PAL, at the default and hard maximum.
The first invocation is only the fast-boot tail; invocation two is a full
frame. The test reads the resulting JSONL with the trace test consumer,
measures actual record count, status, event allocation, largest serialized
line and file length, and times the actual runner's header-through-footer
write interval with a monotonic clock.

| Region | Limit | Records | Status | Capture bytes | Largest line | File bytes | Serialization write time |
| --- | ---: | ---: | --- | ---: | ---: | ---: | ---: |
| NTSC | 65,536 | 44,775 | complete | 1,572,864 | 120 | 5,343,524 | 16.060 ms |
| NTSC | 131,072 | 44,775 | complete | 3,145,728 | 120 | 5,343,525 | 15.621 ms |
| PAL | 65,536 | 53,287 | complete | 1,572,864 | 121 | 6,416,184 | 18.342 ms |
| PAL | 131,072 | 53,287 | complete | 3,145,728 | 121 | 6,416,185 | 18.809 ms |

The limit comparison records the same full-frame event count and output
length, confirming that the measured workload fits both caps. The separate
synthetic maximum-field record reaches exactly the 149-byte ceiling; the
ordinary workload's smaller lines do not substitute for that check.

## Reproduction

Command, run in the pinned worktree:

```sh
git diff --check && cmake --preset ci && \
  cmake --build --preset ci --target muhtari-run muhtari-test-trace muhtari-test-save-runner && \
  ctest --test-dir build/ci -V --output-on-failure \
    -R '^runner[.]trace[.]|^core[.]trace[.]edges$'
```

Exit status: `0` (7 focused tests passed). The verbose output reported the
four measurements above; `runner.trace.memory` reported
`trace_event_size=24 maximum_record_line_bytes=149 record_line_ceiling=149`.
Run URL: N/A-local; this was a local CMake/CTest run and no logs or generated
fixtures are committed.
