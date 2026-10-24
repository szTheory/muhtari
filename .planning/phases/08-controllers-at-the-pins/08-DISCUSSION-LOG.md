# Phase 8: Controllers at the pins - Discussion Log

> Audit trail only. Planning, research and execution consume `08-CONTEXT.md`.
> This log records alternatives and delegated recommendations, not interview answers.

**Date:** 2026-10-24
**Phase:** 08-controllers-at-the-pins
**Areas discussed:** selection/API, input lifecycle, replay, emitted-light timing,
analog/matrix timing, motion/protocols, frontend mappings, acceptance and resources.

The owner requested consideration of all relevant areas, specialist fan-out,
high-quality primary sources, adversarial review and autonomous adoption of a
coherent recommendation set. The owner also preferred small flat dependency trees
and permissive copying over a package when appropriate. No routine preference or
approval questions were needed. Three read-only specialist reviews ran concurrently;
a final cross-area pass closed transient-state, event-ordering and raster-boundary
risks. No additional GSD step or implementation was performed.

## Legacy and extended input

| Option | Pros and tradeoffs | Selected |
|---|---|---|
| Enlarge old input and change its setter | Fewer names, but breaks frozen size/refusal expectations and old automatic behavior | |
| Separate additive extended packet/setter | One extra public entry point preserves old callers and supports complete device payloads | Yes |
| Generic event/configuration framework | Potential future flexibility, with unnecessary current ownership and state burden | |

**Decision basis:** `tests/api/v3.c` asserts a 24-byte input and paddle refusal.
Preserve that entry point; extended Auto resolves per-frame override, successful
load override, then header. Explicit new load devices begin neutral; metadata stays
descriptive. Validate complete packets transactionally. D-01–02 capture the contract.

## Held fields, motion and switching

| Option | Pros and tradeoffs | Selected |
|---|---|---|
| Hold every field across frames | Simple, but repeats relative motion forever without another host call | |
| Hold absolute controls; consume deltas once | Small finite state separates held intent from motion already accepted | Yes |
| Queue every host submission/subframe event | Preserves more host history but changes scope, bounds and replay semantics | |

**Decision basis:** Latest accepted pending packet wins at VBLANK. Same-device
updates retain legitimate hardware state; replacement resets the old device's
transients. EEPROM topology and battery storage remain load-frozen. D-03–04 apply.

## Light-gun sensing and confidence

| Option | Pros and tradeoffs | Selected |
|---|---|---|
| Geometry-only sensor | Small, but reports hits on dark targets | |
| Inspect final framebuffer/current palette | Convenient, but loses temporal order and fails without video buffers | |
| Sample emitted native light, then delay response | Preserves causal brightness and headless equivalence; needs a narrow MARIA/ports connection | Yes |

**Decision basis:** MARIA currently computes pixels only when caller rows exist.
Gun hardware cannot depend on that buffer. Bright DMA-off BACKGRND differs from
blanking. Preserve emission raster/palette through response latency, including the
rightmost pixel at the next line origin. The 64/30/9 window is fitted; threshold and
anchoring require explicit research, not private-game calibration. D-05–06 apply.

## TIA, paddles and keypad

| Option | Pros and tradeoffs | Selected |
|---|---|---|
| Instant register shortcuts | Small, but omits dump/latch and settling behavior | |
| Per-game pot range and executed-cycle waits | May fit individual polling loops, but couples electrical time to software | |
| Integer elapsed-time pins, TIA-owned D6/D7 | Deterministic through slow cycles and stalls; uncertain transfer/ghost details must be qualified | Yes |

**Decision basis:** The primary guide's keypad wait is 400 microseconds, about
2,864 NTSC master ticks, not 716 actual bus cycles. Matrix, pot and digital columns
need separate behavior. D-07–09 require primary truth tables and independently
derived expectations before freezing constants. Hardware corrections follow the
existing narrow behavior-version policy.

## Quadrature and modern adapters

| Option | Pros and tradeoffs | Selected |
|---|---|---|
| Advance movement/transactions on reads | Cheap, but polling changes device behavior and peeks can mutate it | |
| Compact timed axes and finite pin-driven protocols | Fixed storage and bounded work; requires correct wiring, PA7 events and edge tables | Yes |
| Precomputed generic edge queue | Explicit scheduling, with additional storage and machinery absent a demonstrated need | |

**Decision basis:** CX22 direction/toggle signals differ from quadrature. ST/Amiga
tables need explicit axis signs. Even unpolled PA7 transitions must reach RIOT.
ORA and DDRA can both create protocol edges; unchanged writes cannot. Original CC0
SNES2Atari/Mega7800 drivers support both ports and establish useful scan evidence,
but do not constitute physical timing measurements. Exact remaining edge/class
tables are research obligations. D-10–12 apply; cadence and bounds need engineering
justification rather than an invented numerical limit.

## Replay and memory bounds

| Option | Pros and tradeoffs | Selected |
|---|---|---|
| Silently extend movie 1 | Less parser surface but changes released syntax and failure behavior | |
| Movie 2 plus unchanged movie 1 | Explicit new payloads preserve existing replay and diagnostics | Yes |
| Allocate a million extended packets | Straightforward but multiplies memory independently of the text-file cap | |
| Validate immutable bytes, then decode one packet at a time | Preserves full preflight with bounded compact storage | Yes |

**Decision basis:** Preserve file/frame/header limits, identities and CLI conflicts.
Derive a sufficient version-2 record bound and reject malformed late records before
emulation. No recorder or external streaming input is added. D-13–14 apply.

## Frontend controls

| Option | Pros and tradeoffs | Selected |
|---|---|---|
| One universal ProSystem descriptor/mapping table | Stable old layout but adapter buttons also reset/pause/change console switches | |
| Device-specific subclasses/descriptors and base-ID polling | Fits libretro abstractions and permits nonconflicting mappings | Yes |
| New physical input/UI dependency | Unneeded host responsibility and maintenance scope | |

**Decision basis:** Absolute gun coordinates need explicit off-screen state;
mouse deltas are sampled once per frame. Preserve old layouts and give extended
devices complete reachable console controls without double actions. Research must
publish the exact applicable mapping. Existing difficulty options remain. D-15–16 apply.

## Acceptance, provenance and size

| Option | Pros and tradeoffs | Selected |
|---|---|---|
| Private titles, owner UAT or more hosted cases | Broader anecdotes, but violates settled public-automation/hosted-scope contracts | |
| Independent vectors and owned numeric diagnostics through existing hosts/lanes | Legal reproducible evidence; claims remain limited to actual tested behavior | Yes |
| Assume package headroom or rebaseline automatically | Faster apparent progress, without lawful acceptance under the independent guard | |
| Measure comparable packages early and all six native lanes at acceptance | Exposes the narrow headroom before later work depends on an assumption | Yes |

**Decision basis:** Exactly two hosted RetroArch cases remain. Named controller
fixtures require narrow authenticated comparability extension; old rows/identities
remain protected. The 399,937-byte ceiling and performance/compatibility contracts
remain unchanged. Historical Phase 7 worst size leaves 5,985 bytes, not a fit proof.
No new dependency, maintenance-line quota or scoring system is selected. D-17–20 apply.

## Agent's Discretion

Names, internal placement, device button tables, diagnostic grouping and plan
ordering are delegated. Research resolves physical/model uncertainties and
justifies numerical bounds. Decisions above were selected by the agent under the
owner's explicit delegation, with no claim that future implementation tests passed.
Source links and their qualifications are recorded in `08-CONTEXT.md`.

## Deferred Ideas

PAL completion, expansion boards, speech, measured hardware calibration, recording,
save states and general input tooling retain their existing later scope. Four
pending broad-match CI/tooling todos were reviewed and not folded; their paths and
reasons are in the context. Optional tools do not become phase acceptance gates.
