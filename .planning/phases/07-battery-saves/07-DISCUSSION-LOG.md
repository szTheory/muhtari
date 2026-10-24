# Phase 7: Battery saves across hosts - Discussion Log

> Audit trail only. Do not use as input to planning, research or execution.
> Decisions are captured in 07-CONTEXT.md.

**Date:** 2026-10-23
**Phase:** 07-battery-saves
**Areas discussed:** HSC/EEPROM behavior, battery API/attachment, runner persistence,
frontend interoperability and automated acceptance.

## Authorization and method

The owner invoked `$gsd-discuss-phase 7`, then requested consideration of all relevant
decision points through parallel role reviews, primary-source research and an adversarial
pass, and instructed the agent to adopt coherent recommendations. The owner reiterated
the preference for a small, flat dependency tree and permissive copied code when useful.
No interview selections were fabricated. Recommendations were adopted under that delegation;
no additional scope, chained GSD step, new dependency or implementation was authorized.

Three read-only advisor researchers covered hardware/API, runner storage/control and
libretro/automation. The parent reconciled their findings against settled preparation,
current code, the maintenance digest and the unchanged resource/evidence contracts.

## HSC firmware and EEPROM behavior

| Option | Advantages and costs | Selected |
|---|---|---|
| Exact HSC size plus digest identification and warnings | Matches documented signature-warning policy, supports owned firmware tests, needs honest unknown-firmware reporting | Yes |
| Canonical firmware digest only | Restricts compatibility claims but excludes open/modified firmware and creates a test-only acceptance problem | No |
| Signature-only fallback | Already settled; compatible direct-SRAM software works, firmware-vector calls remain unsupported | Carried forward |
| Full HLE or OpenHSC | Could support interactive firmware calls, but adds a deferred subsystem and provenance burden | Deferred |
| EEPROM persistence at programming completion | Models completed writes and reset interruption explicitly; requires bounded deadline catch-up | Yes |
| EEPROM persistence immediately at STOP | Simpler, but exposes incomplete programming as persistent state | No |

**Agent-adopted result:** D-01–05. Microchip's manufacturer document now verifies facts
previously marked unfetched in preparation. Its 5 ms maximum supplies a labelled fixed
model, not a measured duration. HSC has no reset vectors, so the preparation's combined
BIOS/HSC validation row cannot be applied literally. Unknown explicit firmware is warned,
not silently identified as canonical. No prohibited emulator source was opened or retained.

## Battery ownership, topology and change generation

| Option | Advantages and costs | Selected |
|---|---|---|
| Freeze provisioned storage at load | Stable raw layout/pointer; late attachment needs reload | Yes |
| Grow or reinterpret SAVE_RAM on first use | Convenient late device changes, violates stable host storage contract | No |
| Provision all possible media for every game | Simplifies later attachment but creates phantom saves and extra configuration | No |
| One EEPROM on either port | Fits layout 1; dual requests fail visibly | Yes |
| Two EEPROMs or shared array on both ports | Requires a new layout or falsely aliases distinct physical devices | No |
| Host import as clean initialization, then emulated changes | Reuses the import window and avoids scans/shadow state | Yes |
| Observe arbitrary host stores with shadow comparisons | Broadens mutability but adds memory, query work and ambiguous observation timing | No |

**Agent-adopted result:** D-06–09. Preserve old per-frame joystick behavior and smaller
public structs. Reset retains committed bytes and generation; new load resets the baseline.
Console RESET remains a software input. No success claim is made for arbitrary live host
mutation, dual media or topology changes after load.

## Runner storage and controls

| Option | Advantages and costs | Selected |
|---|---|---|
| Explicit existing save directory, established basename | Predictable opt-in and no recursive filesystem side effects; users separate collisions | Yes |
| Default writes, hashes/sidecars, shared device files | Adds invisible writes, migration or new identity/sharing semantics | No |
| Stage bounded raw import before arming persistence | Preserves old progress on short/read/oversize failures; contents stay opaque | Yes |
| Treat every import error as missing | Convenient startup, can destroy valid progress | No |
| Dirty flush every 60 emulated frames | Small bounded writes, roughly one-second NTSC loss window | Yes |
| Longer 300-frame default | Less I/O, longer loss window; no existing contract requires it | No |
| Exclusive sibling temporary, checked sync/close, replace | Preserves old bytes before commit; needs narrow POSIX/Windows seams | Yes |
| Unlink/copy fallback or fixed shared temporary | Can truncate prior progress or another process's work | No |
| Stop on first flush failure | Visible and simple, ends session on storage failure | Yes |
| Continue with retries | Can recover transient failure, needs more state/reporting and accumulates unsaved work | No |
| Signal/control flags serviced in frame loop | Safe handler responsibility; Windows callbacks need thread synchronization | Yes |
| I/O in handlers or generalized IPC | Unsafe or unnecessary operational surface | No |

**Agent-adopted result:** D-10–14. Size mismatch activates reserved exit 4; other I/O
errors use 1. Repeated POSIX SIGUSR1 and Windows CTRL_BREAK request flush; orderly stop
finishes a frame and flushes. A native force-kill is not an orderly stop. The committed
generation advances only after replacement succeeds. Atomic replacement and universal
power-loss durability are separate properties; no such durability expansion is promised.

## Frontend behavior and automated proof

| Option | Advantages and costs | Selected |
|---|---|---|
| Expose core-owned SAVE_RAM and real HSC descriptors | Raw runner/frontend agreement and honest achievement visibility | Yes |
| Adapter-owned save files or fabricated EEPROM CPU map | Bypasses frontend lifecycle or misrepresents serial hardware | No |
| Two fresh launches, two independently expected counters, exact raw bytes | Proves both media and persistence boundary, requires committed EEPROM writes | Yes |
| Screenshot equality alone or seeded counters | Can pass with both implementations wrong or without persistent writes | No |
| Keep DMA hosted case, retain no-content in-process | Fits settled two-case limit while preserving recurring coverage | Yes |
| Add another hosted smoke/case/framework | Consumes budget without new required signal | No |
| Narrow named-fixture comparability extension | Enables new owned fixtures while retaining raw trusted identity and old rows | Yes |
| Broad rebaseline or ceiling increase | Could conceal regressions; no measurements or authorization justify it | No |

**Agent-adopted result:** D-15–18. Current hosted helper deletes its entire run directory;
two launches must share only an explicit save directory. Current tests/retroarch totals
246 lines, so planning must consolidate within the 300-line contract. Existing fixture
allowlist and package limit are real prerequisites, not assumed successful experiments.
Detailed faults stay in-process; old portability status and independent authorities persist.

## Agent's Discretion and deferred work

Internal storage/source boundaries, exact additive API names, diagnostic names and plan
ordering remain delegated. No test or implementation has been delivered by this discussion.
Shared media, dual EEPROM layout, OpenHSC, speech, extra IPC/locking, save states, PAL and
other assigned later work remain deferred. Four matched pending todos were reviewed and
not folded; 07-CONTEXT.md records their paths and reasons. Optional tooling improvements
remain seeds; existing required-contract violations remain blocking.

## Verification record

Before commit, validate canonical paths and decision numbering, run `git diff --check`,
and run `scripts/hygiene.sh --staged`; record their actual results in the session response.
No behavior changed, so no emulator CI run is claimed or needed for this local discussion.
External evidence URLs and existing host/quality entry points are indexed in 07-CONTEXT.md.
