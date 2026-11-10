# Project Research Summary

Project: muhtari. Milestone: v2.0 The whole library. Researched: 2026-10-21.
Status: requirements and roadmap approved on 2026-10-21; work performed inline under the Codex skill adapter. Prior v1 research is archived in milestones/v1.0-research/.

## Key Findings

The existing C17 stack is sufficient. New work is hardware behavior and integration, not a dependency change. Existing parsed board/device/region identifiers do not implement those capabilities. Extend switch-based boards, pin-level ports, the region descriptor and caller-owned memory contracts.

SaveKey must model a real EEPROM protocol and time: 64-byte page writes wrap inside the page, with write completion and busy polling. Microchip's maximum 5 ms is a documented bound, not measured hardware latency. Libretro lightgun coordinates and off-screen semantics must be mapped separately from relative mouse input.

Battery data is raw layout 1 and host-owned persistence: HSC 2048, SaveKey 32768, or HSC then SaveKey 34816 bytes. The runner .sav and frontend .srm share bytes. A ROM-free HSC signature stub supports direct SRAM users, not the full HSC interactive firmware.

## Implications for Roadmap

Start with the approved tune-up at phase 5, then release vertical capabilities for retail boards, battery saves, controllers, PAL, homebrew/expansion memory and broader library evidence. Exact grouping follows requirements approval. Extend current in-process and diagnostic checks, with only the bounded second RetroArch save case added to hosted e2e.

## Scope Dispositions

The owner approved keeping complete Souper/BupChip with the later accuracy work on 2026-10-21, as the detailed hardware plan specifies; SEED-001 contradicts that placement. XBoard/XM scope here is RAM/POKEY/HSC, excluding YM2151. AtariVox scope is EEPROM, excluding speech. Full OpenHSC remains deferred. The milestone name is directional; all pinned library entries must be accounted for without labeling untested titles as passing.

## Confidence and Phase Research Flags

HIGH: existing architecture, raw save sizes, EEPROM page protocol, libretro input abstraction. MEDIUM: board edge cases and fitted analog/light-sensor timing. Further research required for BankSet HALT windows, PAL hand-off, keypad RC fits, XG-1 calibration and explicit device adapters. No GPL/LGPL/unlicensed emulator source was opened.

## Sources

- Repository preparation index and DECISIONS; ARCHITECTURE sections 2–7; A7800-CARTRIDGES-AND-PERIPHERALS sections 2–5/7; SYSTEM sections 1/7; HOSTS sections 1/3/4; CONFORMANCE section 5; ENGINEERING sections 7/8/12.
- Inspected current public header, cart/ports implementation, runner CLI and libretro adapter on b55a3f3.
- Microchip DS21203R, sections 6–8, https://ww1.microchip.com/downloads/en/devicedoc/21203r.pdf (accessed 2026-10-21).
- Libretro Input API, https://docs.libretro.com/development/input-api/ (accessed 2026-10-21).
