# Phase 9: PAL games and audio - Discussion Log

> **Audit trail only.** Do not use as input to planning, research or execution.
> Decisions are captured in `09-CONTEXT.md`; this record preserves alternatives.

**Date:** 2026-10-25
**Phase:** 09-pal-games-and-audio
**Areas discussed:** Region/scheduler/boot; video/palette/hosts; audio/device time/evidence.

## Authorization and method

The owner invoked `$gsd-discuss-phase 9`, selected discussion of all relevant points,
requested specialist fan-out with reputable primary sources and an adversarial pass,
and instructed the agent to synthesize and follow coherent recommendations. Their
dependency preference is a small, flat tree with existing code or understood permissive
seeds preferred where sufficient. No interactive option answers were invented.

Three read-only `gsd-advisor-researcher` reviews owned non-overlapping questions:
region/system/boot, video/host integration, and audio/physical-time/evidence.
Cross-area review followed once. Additional expansion required new evidence capable
of changing a choice. The existing bounded fan-out prompt and settled choices applied.
No `--auto`, `--chain`, lifecycle chaining or next GSD step ran.

## Region, system timing and boot

| Option | Tradeoff | Selected |
|--------|----------|----------|
| Keep NTSC-only execution/refuse PAL | Preserves present behavior but leaves PAL-01 unmet | |
| Extend current scheduler and select a qualified PAL fast-boot profile | Correct region/timing with local changes; unmeasured boot residue remains labelled | Yes |
| Require measured firmware-specific profiles first | Stronger startup evidence but depends on later accuracy work | |

**Selection:** Agent recommendation adopted under the owner's explicit delegation.

Keep existing resolution precedence and transactional loading. PAL becomes available
to existing media layouts; the temporary PAL-refusal test changes deliberately while
numeric statuses/layouts remain. Select 313 lines and the next future VBLANK boundary.
The fitted line308,t184 fast handoff is later than same-origin VBLANK, so NTSC's
first-frame shortcut would underflow. PAL boot registers/RAM follow preparation C10;
unmeasured internal residue does not inherit the NTSC snapshot. Supplied BIOS runs
independently, with synthetic and stripped-OpenBIOS evidence and no Atari bytes.

**Adversarial findings:** Fixed263-line statistics must grow with event limits;
version-size PAL routing would create unnecessary legacy behavior; BIOS16KiB does
not imply PAL and OpenBIOS state is not Atari PAL state.

## PAL video, palette and hosts

| Option | Tradeoff | Selected |
|--------|----------|----------|
| Retain NTSC host metadata/buffers | Simple but fails PAL-02 and can omit lower rows | |
| Use effective region through existing info and host helpers | Preserves ABI and exact native output; requires coordinated direct consumers | Yes |
| Add live region switching and presentation controls | More flexibility but adds AV lifecycle state without a present requirement | |

**Selection:** Agent recommendation adopted under delegation.

Expose320×292, exact FPS/PAR and warm PAL presentation, retaining NTSC helper behavior.
Reuse the approved BSD palette seed with provenance and full-table comparison. Runner
gets resolved dimensions/hashes/PNG rows and a narrow opt-in information report.
libretro options remain load-stable; after-load AV query suffices. No AV-update callback
runs inside load. Test both pixel formats and sequential region/unload/card transitions.

**Adversarial findings:** The public frame has pitch, not allocation length: callers
must requery info and provide storage. Native light sampling/aim/wrap also contain NTSC
limits. Failed library load is transactional; current adapter load releases first and
must instead leave clean fallback metadata. Do not silently strengthen that contract.

## Audio clocks, physical time and evidence

| Option | Tradeoff | Selected |
|--------|----------|----------|
| Keep NTSC sample arithmetic and capacities | Preserves old output but miscounts PAL audio and underbounds storage | |
| Parameterize current synthesis and selected time conversions | Exact carry with fixed storage; needs PAL width/headroom and deadline evidence | Yes |
| Add arbitrary clock/rate or new DSP machinery | Generality without a current consumer, larger regression and maintenance surface | |

**Selection:** Agent recommendation adopted under delegation.

Keep TIA456, real bus-driven POKEY and raw ownership. PAL needs4000/591149 sample
carry,961/962 samples/full frame and a fresh FIFO/phase/headroom proof. A cheap complete
carry sweep and600 actual full frames establish independent numbers before hash equality.
Convert keypad/EEPROM/paddle physical durations, preserving native-line motion and
tick-fit light policies. Scale unrounded paddle durations with safe uint64_t arithmetic.

**Mixer uncertainty:** The original preliminary PAL sheet shows equal6.8kΩ inputs,
suggesting25:38 instead of85:342. One bounded confirmation search found no production
corroboration; its inconsistent crystal label also limits applicability. Retain85:342
as an explicitly unverified PAL compatibility assumption, without claiming analog
calibration. Production schematic/board evidence can reopen it. No gain/kernel redesign
or owner hardware prerequisite is selected.

**Adversarial findings:** An earlier review concern that paddle conversion needed wider
than64-bit arithmetic was corrected by an exact bound: the maximum product is about
1.233e18 and fits uint64_t. No wide-arithmetic dependency/helper is needed. Full frame
carry recurs after591149frames; rendering that entire recurrence is unnecessary when
cheap arithmetic proves it and bounded real execution covers chip/sample phase seams.
Preserve current lane authority and the two hosted cases; do not adopt an advisor's
shorthand about lane roles over the actual workflow.

## Agent's discretion

Helper/profile representation, test names/count partitioning, report key spelling,
plan ordering and implementation arithmetic are delegated within the context's
contracts. Primary facts, code observations, fits and unknowns remain distinguished.
No implementation or hosted PAL pass is claimed by this discussion.

## Deferred ideas

Hardware-calibrated mixer/boot/optics, live AV switching, configurable sample rates,
extra palette families/filters, additional frontend cases, optional tooling seeds and
unearned lane promotion. Expansion boards and whole-library accounting retain their
existing phases. New dependency/framework/mandatory report work was not selected.

## Local discussion evidence

Exact-arithmetic derivation (`python3`, `math.gcd` with integer ratios), exit0,
run URL N/A-local: PAL4000/591149,142102ticks,962maximum current samples,
591149-frame carry recurrence; fitted boot140016→281976 gives141960ticks;
400µs→2838ticks and5ms→35469ticks. These are arithmetic observations, not runtime
acceptance. Canonical source URLs and code entry points are in the context.

Branch starts at released `origin/main` on `phase/09-pal-games-and-audio`.
Read-only `gh pr view 31 --json state,mergedAt,mergeCommit,url`, exit0, confirms
Phase8 merged. `gh pr view 32 --json state,mergedAt,url` and
`gh run view 40201179371 --json conclusion,headSha,url`, exits0, confirm the prior
release PR merged with successful CI:
https://github.com/szTheory/muhtari/actions/runs/40201179371
These resolve the old ship-note's pending snapshot; they are not Phase9 acceptance.

Discussion checks: `git diff --cached --check` and `scripts/hygiene.sh --staged`,
exits0, run URL N/A-local. A one-off Python structure/reference check also exits0:
all six context sections,19 unique decisions and every local canonical reference
are present. No product behavior changed, so these checks make no runtime-test claim.
