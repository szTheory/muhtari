---
title: "Browser emulator player fit for Playstead"
summary: "Assesses a separate embeddable browser/libretro host beginning with muhtari against current Playstead and muhtari seams."
read_when: "Revisiting SEED-008, browser play, or a Playstead web-player integration."
updated: 2026-10-30
---

# Browser emulator player fit for Playstead

## Scope and inspected snapshots

- muhtari source: revision `26c8268f9c56db39308287eaceb935e97eaf3a77`, worktree dirty.
- Playstead source: revision `bcd93b6c7750b913c7b94613d167304d0cfbed5e`, worktree dirty. Findings describe that working-tree snapshot; no dirty-file list is reproduced.
- Read project instructions and product/host notes, then inspected Playstead's product page, browser routes, save/preview surfaces, Mac adapter pin/installer/host, and relevant state support. No emulator implementation source or game/BIOS/save bytes were read or acquired.

## Recommendation

A separate browser player is a coherent future layer: it can own canvas, audio output, browser input, pacing and page lifecycle while muhtari remains a core, runner and libretro deliverable. Do not add a fourth muhtari product or fold a generic host into the core. The muhtari owner ranks browser execution highly, but Playstead's current product contract is Mac-first and treats browser play as a separate readiness gate. Playstead already has a real Phoenix LiveView console, but it currently presents library, preview and save-management surfaces only. Its own preview tells the player to use the Mac app for supported play, and its save console explicitly says it has no emulator or playhead.

The smallest useful proof is a browser-targeted build of muhtari with a narrow harness, first standalone and then exercised at the intended embedding boundary. Emscripten is the existing muhtari recommendation, subject to validation. Keep it one-core and local-content focused; a thin harness may use muhtari's existing libretro seam without promising arbitrary core loading. Generalize to a multi-core host only when a second core or an explicit Playstead web-play requirement demonstrates a consumer. A real Playstead integration requires its own scope. This is sizing guidance, not a phase plan or authorization to create another project.

The trigger recorded by dormant SEED-008 (v1 shipped) has been met. Reweigh the seed at the next milestone boundary; this broader player idea remains distinct from the seed's inexpensive WASI hash lane.

## What exists now

### Playstead

- `Playstead/PRODUCT.md` describes an implemented SwiftUI macOS client alongside a Phoenix LiveView web console. The first proof is Mac-to-server. The same document marks browser play as a separate readiness gate and says LiveView is not the durable client protocol; clients converge through `/api/v1` after disconnects.
- `playstead-server/lib/playstead_web/router.ex:559-564` exposes web library, collection and preview routes. `playstead-server/lib/playstead_web/live/game_preview_live.ex:217-233` says “Use Playstead on Mac for supported play”; `saves_live.ex:9-15` states that the console has no emulator/playhead. There is no browser player route or libretro host in this inspected snapshot.
- The current Mac adapter is pinned to standalone mGBA 0.10.5 for GBA (`playstead-mac/Playstead/Adapter/AdapterPin.json:1-24`). Its installer verifies and copies one macOS app. `AdapterHost.swift:541-576,632-737` launches a child process with a materialized ROM and app-managed save directory; `GameRowView.swift:1050-1115,1161-1206` gates readiness, save preparation, session recording and exit handling around that seam. This is a useful trust/lifecycle precedent, not a browser host seam.
- Mac manual save-state support has two tightly bound Game Boy rows (`playstead-mac/Playstead/Adapter/StateSupportMatrix.swift` and `playstead-mac/docs/STATE-SUPPORT.md`). It does not qualify 7800 or browser states. Playstead's product keeps imported originals, verified local cache, canonical server data and independent backups distinct; browser-local storage alone cannot replace those custody and conflict contracts.

### muhtari

- muhtari already separates emulation from host devices. Its core receives image/configuration bytes, uses an allocator supplied by its caller, returns video/audio/battery spans, and has no core threads, file I/O or mutable globals (`.planning/preparation/ARCHITECTURE.md`, `.planning/preparation/HOSTS-AND-INTEGRATION.md` §6). That makes it a plausible small browser port.
- The existing `wasm32` preset is **WASI preview 1**, executed under Wasmtime. It sets `MUHTARI_BUILD_LIBRETRO=OFF` (`CMakePresets.json`), and `.github/workflows/ci.yml` labels its lane report-only. It checks portable runner behavior and hash equality. It does not build Emscripten output, exercise browser JavaScript, or test canvas, browser audio, gamepad, iframe or lifecycle behavior.
- No Emscripten toolchain, browser bundle, AudioWorklet harness or browser gameplay route was found in the reviewed source. `include/muhtari.h`, `docs/libretro.md` and the current requirements also show that complete state serialization is not shipped yet. The core's raw battery interface is not an exact Continue point; browser Playstead must not promise state restore until a supported muhtari state contract and 7800 consumer exist.
- The seed's small-harness direction is a better first test than a generic web frontend: Emscripten, a canvas, Web Audio output, keyboard/Gamepad input, a test card and project-owned diagnostics. The seed's RetroArch-web alternative is tied to a GPL-3 distribution; it does not fit muhtari's no-GPL-source/reuse policy.

## Fit and scope choices

| Choice | Fit | Trade-off |
|---|---|---|
| Keep the current Mac-first route | Best immediate fit for Playstead today; it supports the accepted first proof without a new browser product. | Does not satisfy the separate browser reach goal. |
| Emscripten muhtari harness | Best first browser evidence. Reuses the deterministic C core and tests actual browser presentation/audio/input with low host scope. | One-system player; not a general core catalogue or Playstead save/state integration by itself. |
| Separate generic libretro browser host | Good longer-term boundary if multiple permissively licensed cores and consumers share it. Playstead could consume it without making its web console the emulator implementation. | Adds WebAssembly core packaging/loading, artifact provenance and compatibility policy, browser A/V/input/pacing, session recovery and embedding security. The current muhtari WASI build proves none of these. |

Do not make Playstead's LiveView process the frame loop or treat its current Mac adapter as the browser adapter. A browser player should be independently usable and embeddable, with Playstead integrating through its durable client/server boundary. Any core, binary or data asset still needs its own licensing/provenance and verification; the host must not silently broaden the supported core matrix.

## Browser-specific constraints

- **Execution and responsiveness:** A dedicated Worker is a reasonable place to run frame work so it does not monopolize the UI thread; the HTML standard describes Workers as background scripts for long tasks that would otherwise interrupt UI responsiveness. A Worker cannot directly update the DOM, so pixels and control results cross an explicit host boundary. [WHATWG Workers](https://html.spec.whatwg.org/multipage/workers.html)
- **Audio and start gesture:** Web Audio has a distinct rendering thread and callback deadline; an AudioWorklet is the browser mechanism for script-based audio processing on that thread. It still needs careful buffering/resampling and real-browser underrun checks. Browsers commonly block audible autoplay until user interaction, so the Play action must unlock audio and handle a rejected/suspended context visibly. [Web Audio API](https://www.w3.org/TR/webaudio-1.1/), [MDN autoplay guidance](https://developer.mozilla.org/en-US/docs/Web/Media/Guides/Autoplay)
- **Input:** The W3C Gamepad document is still a Working Draft. It specifies browser-visible axes/buttons and connect/disconnect events, while allowing user agents to reduce device identity. Do not treat a browser gamepad ID as the native controller attestation Playstead uses. Keep keyboard and accessible controls complete; add touch controls only with an explicit mobile scope. [W3C Gamepad API](https://www.w3.org/TR/gamepad/)
- **Tab and iframe lifecycle:** A hidden page may be frozen or discarded, and discard may produce no final callback. On mobile, unload-style callbacks are especially unreliable. Pause emulation on loss of visibility, persist progress before that boundary where possible, and recover from already durable state; do not make `unload` the save protocol. Removing an iframe destroys its child document without unload events. [Chrome Page Lifecycle guidance](https://developer.chrome.com/docs/web-platform/page-lifecycle-api/), [HTML iframe standard](https://html.spec.whatwg.org/multipage/iframe-embed-object.html)
- **Storage and saves:** Browser origin storage is best-effort by default and may be evicted under storage pressure; persistent-storage requests are not a substitute for an export or independent backup. Keep the local-only player honest about this. A Playstead embed must use Playstead's save revision/conflict/custody flow for server-owned progress, with ordinary battery saves kept distinct from emulator states. [Storage Standard](https://storage.spec.whatwg.org/)
- **Embedding boundary:** A separate-origin iframe can reduce direct access to Playstead UI state, but cross-origin communication must be explicit. If using `postMessage`, pin the intended `targetOrigin`, validate the sender origin/source and message shape, never send bearer credentials in messages, and limit transferred ROM/save bytes to the selected session. Sandbox and Permissions Policy need to allow only the gameplay features actually required. Current Playstead routes, authentication and frame policy have not qualified an iframe seam. [HTML iframe standard](https://html.spec.whatwg.org/multipage/iframe-embed-object.html), [HTML messaging standard](https://html.spec.whatwg.org/multipage/web-messaging.html)
- **Cross-origin isolation:** The current core is single-threaded, so SharedArrayBuffer and COOP/COEP are not needed for the first proof. If future multithreaded WebAssembly needs SharedArrayBuffer, cross-origin isolation adds COOP/COEP and cross-origin resource constraints that can complicate embedding. Revisit only with measured need. [Chrome cross-origin isolation guidance](https://developer.chrome.com/docs/modern-web-guidance/guides/security/security), [SharedArrayBuffer deployment guidance](https://developer.chrome.com/blog/enabling-shared-array-buffer/)
- **Mobile:** Mobile is a later qualification, not an automatic consequence of a responsive canvas. It needs actual performance/power measurements plus touch input, audio-resume, foreground/background and storage-recovery checks. Playstead currently has no shipped mobile client and its first supported experience is Mac.

## Candidate first proof and evidence

For a first player proof, keep the host to one muhtari core, user-selected local content, keyboard/gamepad input, an accessible pause/exit surface, visible startup/audio-unlock feedback, and safe battery export. Include a small embedded mode to prove that Playstead can consume the host, but do not claim Playstead-managed Continue, cloud save custody or mobile support yet. Use only the test card, project-owned diagnostics and CC0 BIOS fixture already allowed by `ASSET_POLICY.md`; never ship user ROMs or proprietary BIOS bytes.

If promoted, seek evidence such as the following through existing automation where it has recurring value. These are research recommendations, not new active CI gates:

1. An Emscripten artifact runs in actual browsers; the existing WASI/Wasmtime equality lane remains a useful core portability check, not its substitute.
2. Test-card and owned-diagnostic frame/audio hashes agree with native outputs before device rendering, and the browser UI reports unsupported input/audio failures cleanly.
3. Keyboard/gamepad press, release, reconnect and focus loss behave as declared; audio begins only after a real Play gesture and the browser audio path does not underrun within its measured target.
4. Hiding, freezing, discarding, closing or removing the embedded player does not silently claim a clean save; fresh-session recovery and export work from durable bytes.
5. The player makes no ROM/BIOS upload or third-party telemetry request by default, and embedded messages cannot cross the declared origin/data boundary.
6. `.wasm` size, per-frame cost and battery cost have explicit measured budgets. Check current desktop Chrome, Firefox and Safari; qualify phone browsers separately before advertising them.

Choose numeric size, timing, audio-underrun and battery budgets from measured target devices before implementation; current research does not provide values. A browser automation check is useful only if it loads the actual Emscripten core and exercises browser APIs. A WASI test passing under Wasmtime does not prove a browser runtime, audio device, gamepad, tab lifecycle or iframe integration.

## Unknowns and revisit conditions

- Does the owner want a standalone public player, in-browser Playstead play, third-party embeddability, or all three? They imply different distribution and save-custody work.
- Which Playstead experience should consume the player first: the Phoenix web console or a future browser client? The current console is not yet a player, and its exact authentication/iframe policy is unqualified.
- Which permissively licensed cores are committed consumers after muhtari? If none, keep the host specific and small; promote the generic libretro contract after a second real core proves reuse.
- What is the target browser/device matrix, especially iOS/Safari and mobile controllers? Do not infer it from desktop support.
- When muhtari state serialization ships, how will states bind ROM, firmware, core/build and execution configuration, and how will Playstead preserve newer battery data before loading an older point? This is a later state/custody decision, not an MVP shortcut.

Revisit this recommendation if Playstead explicitly prioritizes browser gameplay, if a second sibling core wants the same browser host, if the external Mac route cannot meet a now-required web workflow, or if measured mobile devices meet a defined performance/power target. Until then, the next useful action is to weigh SEED-008 at a milestone boundary and keep current Playstead 7800 work on its accepted Mac/RetroArch route.
