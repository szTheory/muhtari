---
created: 2026-10-16T03:40:27.377Z
title: Consider a wasm32 build and hash lane in phase 1 CI
area: ci
severity: minor
files:
  - .planning/preparation/HOSTS-AND-INTEGRATION.md
  - .planning/seeds/SEED-008-browser.md
---

## Problem

The owner (2026-10-15): running efficiently in the browser through WebAssembly is very important, and power
efficiency matters for battery life on handhelds. The plan ships the browser build later (SEED-008), so until then
nothing would show that a wasm32 build of the library still compiles, or still gives the same frame hash as the six
native platforms.

## Solution

At phase 1 discuss, decide whether phase 1's CI adds a seventh hash-equality lane: the library and `muhtari-run`
built for wasm32 (WASI, run under wasmtime, which the fixture tools already use) printing the same test-card SHA-256
as every other platform (HOSTS 6). The core already links only the C memory functions and has no threads, files or
mutable globals, so the lane should be cheap; its per-PR cost and the toolchain pin (wasi-sdk, or clang with
`--target=wasm32-wasip1`) are the open questions. The browser page itself stays in SEED-008.
