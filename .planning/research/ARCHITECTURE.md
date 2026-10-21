# Architecture research — v2.0 integration

Researched: 2026-10-21. Canonical decisions: DEC.22–27, 31–33, 38, 42, 55–60 and preparation/ARCHITECTURE sections 2–7. Repository inspection: src/cart.c and ports.c, include/muhtari.h, runner/main.c, libretro/libretro.c.

## Data flow and module boundaries

Validated image/header/hash -> existing cart selection -> checked board geometry -> load-time allocation -> switch-based board dispatch. Activision layout normalization belongs at load and must preserve separately reported raw/canonical identities. CPU and MARIA accesses must carry explicit ownership; no global DMA flag. BankSet/XM mapping changes follow the existing HALT ownership timeline and cannot infer DMA from the address alone.

Host input -> size-tagged pending input -> VBLANK latch -> device state -> RIOT/TIA pin levels at access tick. Extend the input ABI append-only for axes, key masks and pointer information; preserve old callers. SaveKey consumes effective open-drain RIOT output/DDR transitions including mirrors, repeated START, STOP and busy polling. TIA/RIOT retain responsibility for dump/latch effects.

HSC SRAM and SaveKey EEPROM -> one load-stable battery span -> host persistence. Layout 1 is raw HSC 2048 bytes, SaveKey 32768 bytes, or concatenation 34816 bytes. Version is metadata, not an inserted file header. Core does no file I/O. A generation counter changes on actual byte changes. Power cycle retains battery data; error paths preserve the instance. Runner writes same bytes as .sav with temporary-file/flush/atomic-replace handling; libretro exposes SAVE_RAM and delegates persistence to RetroArch.

Region metadata -> one resolved region descriptor -> dispatcher line/frame limits, MARIA height, boot defaults, synthesiser tick rate, runner hash identity, libretro geometry/fps/aspect/palette. PAL is 313 lines and 142102 ticks/frame with the documented rational clock. Avoid NTSC constants leaking into FIFO sizing, boundaries and sample counts. Existing hashes must remain stable unless a justified behavior revision changes them.

## Suggested order

5 tune-up; 6 retail banking and mirrored RAM; 7 battery media across all three deliverables; 8 pin-level controllers and lightgun; 9 PAL; 10 HALT-banked homebrew/expansion memory; 11 full-library accounting and bounded hosted acceptance. The owner approved this seven-phase order on 2026-10-21; detailed planning may split dependency-aware execution waves within each phase. Each phase releases a usable capability; no study-only phase.

## Verification strategy

Synthetic licensed diagnostics for address decode and pin protocols; property/fuzz checks for geometry and I2C; battery fixtures generated during tests; fresh-process persistence; in-process libretro ABI tests; deterministic frame/raw-audio/PCM hashes on existing platforms; only one additional hosted RetroArch save case under the existing 300-line budget. No commercial bytes committed or required by public CI.
