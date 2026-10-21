# Feature research — v2.0 The whole library

Researched: 2026-10-21. Sources: preparation/CARTRIDGES sections 2–5, SYSTEM sections 1/7, HOSTS sections 1/3/4, CONFORMANCE section 5; current cart.c, ports.c, boot.c, frame.c, public header, runner and libretro adapter.

## Existing capabilities are not new scope

Linear/SuperGame including SG9, bank-6 and banked RAM, declared POKEY windows, ProLine/joysticks, NTSC and deterministic input replay already ship. The parser recognizes additional board/device/region identifiers without emulating them. Acceptance must exercise gameplay-visible behavior, not merely probe recognition.

## Proposed committed categories

- Tune-up: evidence-based timeout/flake and CI-cost fixes, report-only performance gate arming controls, quality.json ratchet, scheduled digest, docs and budget reconciliation. Six pending todos and the compatibility output race are candidates for this first phase, not individually forced requirements.
- Boards: Activision including both known image layouts, Absolute, Fractalus mirrored RAM, BankSet CPU/MARIA views and HALT transitions, XBoard and the POKEY/RAM/HSC subset of XM. High integration complexity for HALT-banked boards, modest for retail banking.
- Battery: HSC SRAM with supplied ROM and an honestly limited signature-only stub, SaveKey/AtariVox EEPROM, stable battery span, versioned layout, runner atomic persistence and host save round trip. Full OpenHSC and AtariVox speech are separate features.
- Controllers: XG-1; paddles; keypad; driving/CX22/ST and Amiga mice; SNES2Atari/Mega7800. Input choices must be exposed consistently through the API, runner and libretro abstractions where applicable. High complexity for light-sensor timing and analog RC behavior.
- PAL: full timing, visible height, palettes, audio rate, boot hand-off and frontend AV reporting. Existing PAL enum is insufficient.
- Compatibility: every entry of the pinned CC0 library accounted for, public fixtures in CI and private library runs isolated. Preserve unknown/unsupported entries and distinguish metadata coverage from tested compatibility.

## Conflict resolved by owner

SEED-001 says Souper in v2.0; preparation/CARTRIDGES section 2.3 assigns Souper/BupChip to v4.0. Souper memory maps alone do not make Rikki & Vikki play with its music. The owner approved deferring the complete board/audio combination together on 2026-10-21; if promoted now it requires explicit additional requirements, source/licence work and integer audio proof. Do not claim all expansion hardware or 204/204 working games as a consequence of the milestone name.

## Deferred features

Browser player; save states/rewind/run-ahead/movie recording; YM2151, BupChip, Covox/ADPCM and ACE; full clean-room OpenHSC; AtariVox speech; 2600 support; ports, debugger and AI bindings. Maintain deterministic seams and explicit unsupported statuses.
