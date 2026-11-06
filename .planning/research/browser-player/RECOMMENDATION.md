---
title: Browser player project — initial recommendation
summary: Existing browser hosts, a possible separate permissive player, Playstead fit and conditions for deeper exploration.
read_when: Before promoting the browser-player seed or starting a separate player project.
updated: 2026-10-30
---

# A browser player is an existing category; a smaller host is a hypothesis

Owner direction, 2026-10-30: bank the idea, inspect existing products and the
actual sibling, use bounded primary-source fan-out, and stop before a full spec.
The [seed](../../seeds/SEED-261030-tt4-browser-libretro-player.md) is dormant.
No implementation, new project, milestone promotion or deployment is authorized.

**Yes, RetroArch already runs in browsers.** Its
[official Web Player](https://web.libretro.com/) and
[user guide](https://docs.libretro.com/guides/web-player/) describe core selection
and local content loading. EmulatorJS, Nostalgist/RetroAssembly and WebRetro are
other relevant products. Their existence establishes the category, not their
fitness for Playstead or a wholly permissive runtime distribution.

The [source-pinned landscape](landscape.md) separates wrapper licenses from
RetroArch and core licenses. MIT wrappers can still depend on GPL runtimes.
Libretro's API header is permissive; libretro compatibility does not require
copying RetroArch. An unlicensed small-host project is documentation to learn
from, not code to reuse under this repository's policy. No prohibited emulator
implementation source, runtime package or game content was acquired.

## Coherent recommendation

Keep a **separate, small, permissive browser host** as the candidate to evaluate,
with muhtari as its first core. Compare its actual value against an existing
player before committing to owning one. The differentiation worth testing is
calm embedding, explicit session/save ownership, predictable input, actionable
failures and a pinned/reproducible distribution. Feature count is not the goal.

| Decision | Initial recommendation | Cost or reason to change |
|---|---|---|
| Reuse vs build | Compare existing products first; only build a thin host if the fit/license gap matters. Reuse small permitted pieces with notices when justified. | A maintained, appropriately licensed host that meets the embed/save contract would beat new infrastructure. Copied code still needs maintenance. |
| First supported core | One browser-targeted muhtari build, a declared libretro subset, and owned diagnostic content; user games stay local. | Native shared libraries and today's headless WASI runner are not browser cores. Build/toolchain/ABI selection remains unvalidated. |
| Consumer | A standalone demo and an embeddable view using the same small player surface; Playstead is a prospective consumer. | Playstead has a web console, but current play remains native. Embedding and progress custody require a Playstead-owned change. |
| Progress | Preserve/export battery data first where supported; expose save states only when the core supports them and identity/compatibility checks pass. | A page close is not a reliable save event; browser storage can be evicted. Cloud sync and exact cross-device Continue are separate product work. |
| Architecture | Browser facilities for rendering, audio and input; an isolated execution/lifecycle boundary; measure simple non-shared-memory delivery before adding shared-memory/thread requirements. | Audio scheduling, latency and iframe deployment may favor another design after measurements. Do not generalize a single-core result to heavy/GPU cores. |
| Scope | Local file selection, start/pause/stop, controls, sound/picture, progress export/recovery and useful errors. | Universal cores, shaders, netplay, accounts, cloud library, AI/TAS and a plugin marketplace wait for demonstrated demand. |

The [current-source Playstead/browser fit](playstead-fit.md) records sibling
revision and dirty-worktree qualifications, platform sources and quality risks.
Muhtari remains a library, runner and libretro core; a frontend owns browser
devices/UI. The existing native Playstead/RetroArch quick route remains separate.

## Adversarial pass and promotion evidence

The strongest objection is that this duplicates mature hosts while inheriting
their hardest problems: sound timing, input focus, mobile constraints, save loss,
core packaging and continuing browser support. A smaller dependency tree alone
does not establish security, low latency or lower ownership cost. An iframe also
does not automatically isolate code or grant durable storage/fullscreen/audio.

If later promoted, the first experiment should compare one real, appropriately
licensed core/session in a plain page and the intended embed. Observe startup,
audio/underruns, input/release on focus loss, pause/resume, bounded teardown and
durable-progress recovery under failures. Reuse owned diagnostics and automated
browser checks; qualify actual Safari/mobile evidence separately from WebKit
automation. Record commands/exits and artifacts without asking the owner to judge
ordinary UI or screenshots. Agree concrete performance/resource criteria from
measurements rather than importing historical guesses as acceptance thresholds.

Stop the custom-host direction if an existing permissible solution meets these
needs, Playstead has no useful browser-play surface, or the maintenance cost
outweighs the product gain. No superiority, browser performance or successful
integration has been measured in this initial read.

## Resume in another session

Start with the seed, this recommendation and its two linked research notes, then
recheck moving upstream licenses/builds and Playstead's current dirty sources.
Suggested prompt: “Explore the browser-player seed at bounded research/spike
depth. Compare existing hosts with a small permissive libretro-compatible player
for muhtari and Playstead. Identify the smallest discriminating experiment and
ask before starting a project or implementing it.”

The [interface-project initial read](../emulator-interface/RECOMMENDATION.md) is
a related idea; a new emulator standard is not a browser-player prerequisite.
