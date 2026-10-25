---
phase: 08-controllers-at-the-pins
plan: "12"
status: complete
subsystem: input
tags: [libretro, snes2atari, mega7800, diagnostics, replay]
requires:
  - phase: 08-11
    provides: Finite SNES and Mega pin protocols with opposite EEPROM programming
provides:
  - Conventional complete adapter mappings without console collisions
  - Six independently judged adapter diagnostic groups in the existing motion ROM
  - Actual library, runner and module numeric, native frame, raw audio and PCM equality
affects: [08-13, 08-14]
tech-stack:
  added: []
  patterns:
    - Base JOYPAD polling with separate supplemental console controls
    - Fresh diagnostic groups preserve the existing27-record result limit
key-files:
  created: []
  modified:
    - libretro/libretro.c
    - tests/libretro/host.c
    - docs/libretro.md
    - tests/diag/controller-motion.s
    - tests/diag/controller-motion.a78
    - tests/diag/driver.c
    - tests/diag/expected.csv
    - tests/diag/tests.cmake
    - tests/roms/manifest.txt
    - tests/runner/test_runner.c
    - tests/runner/tests.cmake
    - docs/hardware/controllers.md
    - docs/emulation-tests.md
    - docs/runner.md
key-decisions:
  - Use JOYPAD subclasses5/6 and conventional native mappings with supplemental L2/R2/L3 console inputs.
  - Split each adapter into three fresh startup-selected groups of26records rather than enlarge the mailbox or judge.
  - Commit owned diagnostic source before admitting its rebuilt fixture so the manifest names an exact source revision.
requirements-completed: []
requirements-contributed: [INPUT-01, INPUT-05]
coverage:
  - id: conventional-mappings
    description: All native SNES and Mega controls reach real loaded-module CPU scans on both ports without console or difficulty collisions.
    requirement: INPUT-01
    verification:
      - kind: integration
        ref: libretro.host.input,libretro.host.devices;individual controls,17bits,six pairs,directions,descriptors and provider changes;exit0;N/A-local
        status: pass
    human_judgment: false
  - id: independent-adapter-numbers
    description: Six fresh owned executable groups independently establish every adapter button, both ports, detection tail, identification, complete pairs and normal pins.
    requirement: INPUT-05
    verification:
      - kind: integration
        ref: diag.controllers;motion4a..c,motion5a..c,26records each,real CRC-valid fault consumers;exit0;N/A-local
        status: pass
    human_judgment: false
  - id: fixture-identity
    description: The rebuilt admitted motion fixture names its exact owned source and retains all earlier numeric and output observations.
    requirement: INPUT-05
    verification:
      - kind: integration
        ref: diag.rebuild,policy.quality.controls and independent source/manifest/historical-report comparison;exit0;N/A-local
        status: pass
    human_judgment: false
  - id: actual-host-equality
    description: Library extended input, actual movie2 runner subprocesses and actual loaded modules agree after independent numeric judgment on12native frames,raw transitions and exact PCM/counts.
    requirement: INPUT-01
    verification:
      - kind: integration
        ref: diag.controllers;all earlier families and six adapter groups,missing/truncated/wrong report consumers;exit0;N/A-local
        status: pass
    human_judgment: false
  - id: release-and-storage
    description: Complete adapter EOF release and first-frame selection retain old replay behavior and frozen opposite SAVE_RAM.
    requirement: INPUT-01
    verification:
      - kind: integration
        ref: runner.movie.owner,runner.movie.v2_edges,libretro.host.input,libretro.host.battery_order,input.savekey.pins;exit0;N/A-local
        status: pass
    human_judgment: false
  - id: complete-local-evidence
    description: Exact committed-source CI and release inventories,hygiene and authenticated actual three-component ZIP observations pass.
    requirement: INPUT-05
    verification:
      - kind: integration
        ref: ci648+6,hygiene24,explicit package/release and quality_evidence.cmake at03b7f96;exit0;N/A-local
        status: pass
    human_judgment: false
actuals:
  tokens: 15853
  tasks: 3
  commits: 7
plan_head_before: 3d8889e481f3804e120d8b9195a9573f087bd61e
plan_head_after: 03b7f961f07f3be7a5d8571c369e53707b4dcb64
duration: 14min
started: 2026-10-25T01:08:28Z
completed: 2026-10-25
---

# Phase 8 Plan 12: Complete adapter delivery Summary

**Conventional SNES/Mega RetroPad mappings and156 independent adapter records now execute through library, actual runner replay and loaded modules without console collisions or SAVE_RAM changes.**

## Accomplishments

JOYPAD subclasses5/6 advertise SNES2Atari/Mega7800 while retaining every
earlier identity. SNES maps native names identically; Mega maps Y→A,
B→B,A→C,L→X,X→Y,R→Z,Select→Mode,Start→Start. The native Mega B bit is
the normal primary button; A is the extended-only second primary bit.
All polling uses base JOYPAD index0. Device-specific labels refresh with
selection and Auto activates supported headers. An ordinary port retains
console priority; otherwise adapters use supplemental L2/R2/L3 for
Pause/Select/Reset and options for difficulties. Provider selection does
not synthesize shoulder edges. Unknown IDs retain visible safe fallback;
late storage changes retain the installed attachment and report reload.

Loaded-module owned CPU loops observe every individual native button,
directions, all17 SNES bits and all six Mega pairs on both connectors.
They verify descriptor labels/base IDs, absence of console double effects,
provider changes, supplemental switches and opposite storage. With an
opposite EEPROM, its pointer/32768-byte length and all32768 marker bytes
remain fixed through scans and refused late replacement. Existing storage
tests retain detailed pending-programming and ACK evidence from08-11.

The existing motion ROM adds scenario4=SNES and5=Mega. Each has three
fresh groups a/b/c selected by existing startup difficulty positions,
with26records: scenario,group,six scans×four records. Frames3..6 exercise
four single controls, reversed between ports;7 releases;8 holds all twelve.
Startup momentary selectors release while group difficulty positions stay
fixed. This preserves the existing27-record screen/judge limit and the
mailbox, rather than expanding a protocol. All earlier motion records stay.

SNES words pack the first16 observations, retaining high13..16; separate
17th-bit records remain low. Individual words are65535 XOR(32768>>i),
released65535,all-held15. Partial scans followed by a latch restart, DDR-only
clocks advance and duplicate DDR writes do not. Mega words preserve00/00
class/ID followed by C/B,Start/A,Y/Z,Mode/X; a single button yields
255 XOR(128>>i),released255,all-held0. Normal SWCHA and all six TIA pins
are separately packed, including B/C and extended-only A. Duplicate
low/high writes and DDR release/rearm execute in the ROM. Fine-grained
extra/partial-edge and EEPROM chronology remain in existing input tests.

Expected numbers were written before the ROM implementation and output
hashes, using documented connector equations. The actual report judge
rejects CRC-valid wrong values/IDs/counts, damaged17th tail or identification,
swapped serial words/Mega pairs or pins and omitted scans. Missing and
truncated reports fail through the real consumer. Only after numeric
judgment do all12 native frames, raw transitions and exact PCM/counts compare
across the library, actual runner movie2 subprocess and loaded test module.
Module public pixels and stereo copies are checked independently. All
seven preceding controller reference reports are byte-identical to their
retained pre-change reports, including frame/raw/PCM/count observations.

Runner owner and subprocess expansions check4095 masks, both placements,
complete EOF release against explicit neutral rows, retained difficulties
and modern no-controls first-frame selection. Existing movie1 predicates,
digests, inventory names and three deliverables remain intact. Public docs
describe mappings, physical frontend binding limitations, numeric models
and replay tags without retail or calibration claims. No INPUT requirement
is completed before final acceptance.

## Task Commits

1. Task1 RED `c0070fb`, GREEN `d56f4a3`: adapter advertisement, mappings, real CPU scans and frontend documentation.
2. Task2 RED `6d14bea`, owned source `7638c2e`, GREEN `eeae539`: independent expectations, finite executable scans, rebuilt fixture and real numeric fault consumers.
3. Task3 RED `aada6b3`, GREEN `03b7f96`: actual module/runner equality, typed replay EOF and complete local verification.

The fixture manifest names exact source commit7638c2e90bd060440f0508a7f017616c70d75056.
Its admitted ROM SHA-256 is8a5686a6f313ff3261f0399d754dcbcb90a17ca7af6662a01f1129b76a17a83c.
An independent Git comparison confirms that commit's assembly equals the
current source and its actual rebuilt bytes match the manifest. Only this
fixture's manifest row changes. Driver facts came from the already pinned
CC0 assembly files and official libretro input API/vendored MIT header;
no outside expression or excluded emulator source was imported. Context7
tools/CLI were unavailable; no package was installed.

Normal hooks and GitHub noreply identity were retained. Supplied-root and
protected-branch assertions passed before every commit. No tracked deletion
or unexpected untracked output remains. Fourteen implementation/test/doc
files changed. The parent owns STATE.md,ROADMAP.md,REQUIREMENTS.md; this
executor did not edit,stage or commit them.

## TDD Gate Compliance

All three tasks executed meaningful behavioral RED before implementation.
Unchanged actual JUnit reports and evidence JSON remain ignored build
artifacts. Every classifier invocation returned RED_EVIDENCE_OK,exit0.
All evidence below is local, run URL **N/A-local**:

- Task1 `ctest --preset ci -R '^libretro[.]host[.]devices$' --output-junit ../adapter-map-red.xml --output-on-failure`,exit8: the loaded module advertised13 devices instead of15.
- Task2 `ctest --preset ci -R '^diag[.]controllers$' --output-junit ../adapter-diag-red.xml --output-on-failure`,exit8: after earlier scenarios passed,the admitted ROM completed motion4a with1record instead of26. This is missing executable behavior,not a missing-file/configure failure.
- Task3 `ctest --preset ci -R '^diag[.]controllers$' --output-junit ../adapter-host-red.xml --output-on-failure`,exit8: the actual loaded module produced19driving records instead of26adapter records and9201 output mismatches after library numeric judgment passed.
- Classifier commands: `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/adapter-map-red.json --raw` and corresponding adapter-diag-red.json/adapter-host-red.json,exit0 each. Two record timestamp fields were corrected from unchanged actual JUnit local timestamps to UTC; report bodies were not edited and reclassification passed.

Runner EOF/first-frame coverage added in Task3 was initially green against
the existing parser/runner behavior; it is test expansion,not another RED.
No refactor commit was necessary. Source7638c2e is a separate test-source
commit so fixture provenance can name a real immutable source revision.

## Verification Evidence

Every final command below exits0; run URL **N/A-local**:

- Task1 and its post-commit tracer rerun: `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^libretro[.]host[.](devices|input|battery|battery_order|reset)' --no-tests=error --output-on-failure`,6/6 including the input fixture.
- Task2: `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(diag[.]rebuild|diag[.]controllers)$' --no-tests=error --output-on-failure`,2/2. The actual assembler/sign/header recipe rebuilt the admitted ROM.
- Task3 final focused check: `ctest --preset ci -R '^(docs[.]budgets|diag[.]controllers|runner[.]movie[.](v2_edges|owner))$' --no-tests=error --output-on-failure`,5/5 including fixture.
- `cmake --workflow --preset ci`,648/648 plus actual package production and release6/6. It passed before GREEN and again at exact implementation03b7f961f07f3be7a5d8571c369e53707b4dcb64.
- At the same committed source: `cmake --build --preset ci --target package && ctest --preset ci-release --output-on-failure`,exit0,release6/6.
- `cmake --workflow --preset hygiene`,24/24,including actual formatter,analyzer,integer-only,tree and firmware checks. `git diff --check`,exit0.
- `cmake -DROOT=. -DBUILD=build/ci -DOUT=build/phase08-12-quality.json -DSHA=$(git rev-parse HEAD) -DCONFIG=ci -DSOURCE=local -P tests/cmake/quality_evidence.cmake`,exit0. The actual producer confirms exact648+6 inventories,source/environment/manifest/sample identity and three ZIPs.
- `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check evaluation-scope --plan 08-12 --commits-only --raw`,exit0,resolves all seven reachable task commits.
- Independent Python/Git checks: every changed file exists,all seven commits are ancestors,all old numeric rows and unrelated fixture pins are unchanged,all seven historical native/raw/PCM reports match,source/fixture identity and actual ZIP count/sum match. Added-line stub scan is clear.

## Package Observations

Actual native macOS arm64 packages at03b7f96:

| Component | Bytes |
|---|---:|
| library | 80625 |
| libretro | 85066 |
| runner | 93802 |
| Total | 259493 |

The actual producer reports native/local/measured with exact source and
environment identity. Preceding08-11 total258624 at70465c9 is869bytes smaller.
These are attributed local observations,not a fabricated candidate/base
or six-native acceptance. Final genuine event-base comparison and six-native
measurements remain08-14. Contents,notices,firmware exclusion and platform
floor checks pass. DEC.61 retains required measurements/integrity without
restoring the retired archive cutoff or inventing a replacement quota.
No hosted run was dispatched solely for intermediate sizes.

## Deviations from Plan

**1. Rule2 — preserve documentation budget and source-qualified derivation.**
The new hardware diagnostic explanation belongs in docs/hardware/controllers.md,
in addition to listed host/testing docs. The testing guide initially exceeded
its40960-byte bound; its controller section now points to the full hardware
derivation and is40882bytes. The required bound is unchanged. Commits7638c2e/
03b7f96; actual docs.budgets and full CI pass.

**2. Rule3 — split executable groups and anchor fixture source.** The
existing judge/screen accepts27records,so each adapter uses three fresh
26-record groups selected through existing switches. This is the plan's
explicit split alternative,without a new ROM or protocol. A separate source
commit makes the fixture manifest's revision truthful. Commit7638c2e/
eeae539; exact-source rebuild,admission and numeric consumers pass.

## Issues Encountered

Initial Task1 GREEN checks exposed an inverted native Mega B/A mapping;
the real CPU pair expectations caught it and the mapping was corrected
before GREEN. New Auto adapters also required their correct modern pin
expectations; legacy client expectations were preserved. A test-only SNES
released sentinel was incorrectly treated as position12 and was corrected.
Task2's first swap mutant changed only equal low bytes; the actual consumer
accepted that no-op,so it was corrected to swap complete words and later
to exchange actual Mega pairs. All final fault consumers reject their faults.
Task3's first test build found shadowed argument variables and a const-dropping
cast; these test declarations were corrected before execution. None of these
compile/setup corrections is presented as behavioral RED.

The first complete CI exited8 solely for docs.budgets at42138bytes; local
condensations still measured41358 and41005 before the final40882. Those
failed experiments are retained here rather than claimed as passes. Fresh
complete CI passed after the final edit and at the committed implementation.
No skipped test,unrun required check,remaining blocker,known stub,auth gate
or new security trust-boundary surface remains. Physical undocumented adapter
behavior stays explicitly unmeasured.

## Next Plan Readiness

This completes08-12 only. The parent owns shared position and acceptance;
08-13 is the next dependency-ready plan. All INPUT requirements remain
pending final phase acceptance. No additional GSD step,PR,ship,merge or
release was invoked. Exact next separate command: `$gsd-execute-phase 8`.

## Self-Check: PASSED

All14 declared changed files exist; all seven task commits are ancestors
of HEAD and resolve through evaluation-scope. The persisted plan ledger
measures7commits from3d8889e through03b7f96. The realized diff measures15853
estimate-scale tokens,characters/4 floored,excluding this metadata summary.
Committed-source648+6 CI/package evidence,hygiene24,actual numeric/host/storage
consumers and authenticated three-ZIP measurements pass. No tracked deletion,
unexpected untracked output,unrelated fixture change or inventory adjustment
remains. Duration14min is measured from01:08:28Z through summary closeout;
context loading before that start is not separately timed.
