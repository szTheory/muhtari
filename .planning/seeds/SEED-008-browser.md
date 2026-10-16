---
id: SEED-008
status: dormant
planted: 2026-10-15
planted_during: kickoff of milestone v1.0
trigger_when: milestone v1.0 has shipped (weigh it against v2.0 at that new-milestone step), or the owner asks for a web build sooner
scope: medium
---

# SEED-008: muhtari in the browser — an efficient WebAssembly build

## Why This Matters

The owner ranks running efficiently in the browser through WebAssembly as very important (2026-10-15). A web page
reaches players who will not install a frontend, and it was filed under v6.0 with the ports (SEED-005); this seed
brings it forward. Efficiency counts twice here: a browser tab on a laptop or phone runs on battery, and the core's
work per frame sets both its speed and its power draw.

## When to Surface

**Trigger:** v1.0 shipped (games play with sound), so there is something worth playing; weigh it against v2.0 at
that new-milestone step. Sooner if the owner asks. The cheap half, a wasm32 hash lane in CI, is a phase 1 discuss
question (the pending todo in `.planning/todos/pending/`).

## Scope Estimate

**Medium** — one or two phases: an Emscripten build of the core and a ~150-line JS harness (canvas 320×242,
AudioWorklet at 48 kHz, keyboard and Gamepad API) on a static page; the player picks a cartridge file locally and
the page ships no ROM or BIOS (ASSET_POLICY); the test card, the project's own diagnostic ROMs and the CC0 OpenBIOS
as built-in content; frame and audio hashes equal to native (the WASI lane); a size budget for the `.wasm`, and
frame-time and power checks in Chrome, Firefox and Safari, including on a phone.

## Breadcrumbs

- `.planning/preparation/HOSTS-AND-INTEGRATION.md` section 6: the recommended build, harness and alternative.
- `.planning/preparation/ARCHITECTURE.md`: portability row (no threads, files or mutable globals; 32-bit `size_t`).
- `.planning/preparation/A7800-ECOSYSTEM-AND-USERS.md` section 4: work per frame and device budgets.
- `.planning/preparation/ENGINEERING.md` sections 8 and 9: the instructions-per-frame ratchet and portability lanes.
- `.planning/seeds/SEED-005-everywhere.md`: the other ports.

## Notes

No threads means no SharedArrayBuffer, so no COOP/COEP headers, and any static host works (HOSTS 6). Web 7800
emulators already exist (JS7800, test7800's WASM build; both GPL, ECOSYSTEM 1); muhtari's would be the accurate,
permissive one that other sites can embed.
