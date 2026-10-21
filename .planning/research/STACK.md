# Stack research — v2.0 The whole library

Researched: 2026-10-21. Scope: new capabilities only. Approved milestone scope; phase-level implementation research remains separate.

## Recommendation

Keep C17, CMake/CTest, the existing runner, libretro header and integer synthesiser. Add no runtime dependency. Boards, EEPROM and pin protocols are small own modules written from documented hardware facts; tests reuse existing diagnostic ROM construction, mailbox checks, seeded property checks and fuzz replay infrastructure.

Core additions: HSC storage and mapping; a bounded 24LC256 state machine; per-device pin state; board-specific reads/writes with CPU/MARIA ownership; region parameters in the existing dispatcher. Host additions: battery-file persistence in the runner, SAVE_RAM exposure and lightgun/analog/mouse mapping in libretro. Device files and clocks remain outside the core.

Pipeline tune-up uses POSIX sh, gh and existing CMake JSON parsing; no reporting service or test framework. A weekly triage digest and monthly efficiency section are already proposed in ENGINEERING sections 7/12. The quality ratchet must consume measured existing outputs and stay within the gate-code budget; do not manufacture coverage values or duplicate per-metric gates.

## New source verification

Microchip DS21203R documents a 32K x 8 EEPROM, a 64-byte page buffer and a maximum 5 ms write cycle. Model busy time with emulated ticks; 5 ms is a deterministic model choice based on a maximum, not a measured typical value. See https://ww1.microchip.com/downloads/en/devicedoc/21203r.pdf (sections 6–8, accessed 2026-10-21).

Libretro's current input documentation distinguishes relative mouse deltas from screen-space pointer and lightgun coordinates, including off-screen lightgun input. Use the existing vendored header as the ABI and verify individual IDs against it: https://docs.libretro.com/development/input-api/ (accessed 2026-10-21).

## Exclusions and confidence

No new graphics/audio/device backend, browser harness, state serializer or speech synthesiser. Existing tool versions remain pinned; no dependency upgrade is required by these features. HIGH confidence in stack reuse and data boundaries; board and sensor fits need phase research.
