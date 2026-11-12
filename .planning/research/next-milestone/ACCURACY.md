# 7800 accuracy candidate

**Inspection basis:** 2026-11-11, repository HEAD `12953bc`. The working tree
already contained unrelated milestone-planning edits; this note is the only
file written for this research task.

**Unresolved question:** is there a reproduced, user-visible Atari 7800
hardware mismatch that supports a bounded accuracy correction now? The
inspected evidence does not name one. Current contracts include fitted and
source-derived behavior, but a model-only expectation is not evidence that the
emulator is wrong.

## Decision comparison

| Choice | Evidence and player value | Cost / risk | Assessment |
|---|---|---|---|
| Make no accuracy correction now | Avoids changing supported timing without a counterexample. Existing tests cover several hardware-referenced values, but broad retail playability remains unproved. | No core work; leaves the known evidence gaps open. | **Recommended.** There is no evidence-supported correction to ship. |
| Discriminate one MARIA timing contract | The smallest relevant in-tree oracle is the owned `tests/diag/dli.a78`: `zone1`/`zone2` currently expect 117 from RTL, while the Atari guide describes DLI one tick after shutdown. The CPU samples handler timing against WSYNC; this could identify a real timing difference affecting DLI-sensitive games. `diag.dma` already covers empty-list 16/24-tick totals and H8/H16 hole cases. | The current test compares the emulator with committed expectations; it cannot establish which timing matches a console. A physical result needs a known console/flash-cart setup and authorization, absent here. A timing change without that result risks destabilizing games. | Best *conditional* accuracy investigation, not a correction or runnable milestone today. |
| Add one gameplay/compatibility slice | More direct player value than microtiming. `docs/compatibility.md` records 201 pinned-list titles as not evaluated and 3 skipped; owned samples are title-screen/boot evidence, while owned diagnostics prove model conformance. | The pinned retail list has digests, not ROM bytes. Existing 7800basic sample content includes unclear per-file asset terms. No lawful, ready-to-run gameplay fixture plus independent expected outcome was found in this tree. A new fixture needs a rights audit and a discriminating input/result oracle. | Strongest product-facing alternative, but it lacks a present fixture/oracle and does not currently identify a core defect. Revisit when a lawful fixture and independent gameplay expectation are available. |

## What the current evidence says

- **HALT and MARIA holes:** `src/bus.c:83-88` calls the two-cycle SALLY response fitted, and `docs/hardware/maria.md:134-150` fits the `T0 = fall + 6` handshake. The owned DMA diagnostic already includes empty-DL hardware-reference expectations (`tests/diag/expected.csv:8-9`) and direct hole cases (`:25-28`); the notes attribute the 16/24 tick empty-DL totals and hole costs to RevEng's real-console measurements. These are source-backed expected values, not a new measurement or a demonstrated mismatch. The remaining DLI disagreement is explicit: the Software Guide says one tick after DMA shutdown, while this implementation follows MiSTer's second-φ1 rule (`docs/hardware/maria.md:236-255`, `tests/diag/expected.csv:39-48`). Existing `diag.dli` cannot settle that source conflict by itself.
- **POKEY timing:** `src/pokey.c` advances POKEY on actual bus half-edges, including held cycles; `tests/audio/test_pokey.c` covers the deterministic timer/audio model. `docs/hardware/pokey.md:1-50` derives the digital model from the Altirra Hardware Reference Manual and explicitly leaves cold-silicon state, analog loading, STIMER polarity and sub-cycle races confidence-qualified. No reproduced mismatch or licensed independent hardware trace was identified for a correction.
- **Controllers:** `docs/hardware/controllers.md` and the owned diagnostics cover pin mapping and deterministic fitted behavior. For example, paddle comparator/charge timing is expressly unmeasured (`controllers.md:342-362`); motion and optical timing are also fits. The tests validate those model policies, not calibrated physical device response. Host device mapping is outside the core's deliverable boundary.
- **Gameplay evidence:** `docs/compatibility.md:7-14, 38-57` cautions that diagnostics are not gameplay and reports the pinned-title evidence as not evaluated. The actual list is `tests/compat/ledger.csv`; admitted committed ROM fixtures are restricted by `tests/roms/manifest.txt`. The preparation inventory identifies commercial dumps as local-only and says current 7800basic sample assets have unresolved terms (`.planning/preparation/CONFORMANCE.md:21-37`). None supplies a ready, redistributable gameplay oracle.

## Smallest useful proof and limits

If a bounded measurement is later authorized, reuse the existing `dli` ROM and
record its numeric `zone1`/`zone2` mailbox results on a named NTSC console and
flash cart. Preserve the observation as source-specific evidence and change the
expected value only if the hardware observation differs. Then the existing
`ctest --test-dir build/ci --output-on-failure -R '^(diag\.dli|diag\.dma)$'`
would check the updated CPU/DMA expectations locally, followed by
`cmake --workflow --preset ci`. These are proposed existing commands, not runs
from this research. Their exit statuses establish emulator regression
behavior, not physical accuracy; only the recorded console observation would
establish that premise. The current authorization does not include a console
run, so this proof is not presently runnable as an agent-owned acceptance gate.

For a gameplay alternative, the smallest meaningful acceptance would require a
rights-cleared fixture, fixed input sequence, and independent gameplay result
(for example a score/state milestone), then the same CI workflow plus a
compatibility-ledger row. A frame hash at a title screen or agreement between
two emulator models would not establish gameplay compatibility. Fixture
acquisition and rights are unresolved, so no such acceptance is available now.

## Provenance, confidence, and revisit triggers

- **Primary document:** [Atari 7800 Software Guide, Appendix 3](https://www.atarihq.com/danb/files/7800%20Software%20Guide.pdf) describes DMA timing ranges and explicitly says the extra startup/shutdown cycles are uncertain; it says DLI follows shutdown by one 7.16 MHz cycle. This is an Atari-era guide hosted by AtariHQ.
- **Firsthand hardware report:** [RevEng, “7800 hardware facts”](https://forums.atariage.com/topic/224025-7800-hardware-facts/) reports the NULL-DL totals as 16 ticks for ordinary lines and 24 for zone-last lines (May 4, 2014), and describes the hole-cycle observations. The repository classifies this community source as T4; it supports a test expectation, not a fresh or multi-console calibration.
- **Implementation/reference source:** `src/bus.c`, `src/maria.c`, `src/pokey.c`, `src/ports.c`; contracts and limitations in `docs/hardware/{bus,maria,pokey,controllers}.md`; tests and evidence inventory in `tests/diag/{dli.s,dma.s,expected.csv,stats.csv}`, `tests/audio/test_pokey.c`, and `tests/compat/ledger.csv`. No third-party emulator implementation was opened for this report.
- **Confidence:** high that the inspected repository does not establish a current supported defect; medium that the cited guide and firsthand report justify their respective documented expectations; low that this read-only inspection settles hardware DLI timing, analog controls, or POKEY cold/analog behavior. No code, test, build, CI, app, ROM, or console was run.
- **Revisit if:** a specific game or homebrew sequence fails with a reproducible input and lawful fixture; an authorized real-console DLI observation contradicts the current `diag.dli` reference; or an independently licensed oracle/recording establishes a POKEY or controller divergence. Until then, keep fitted contracts labeled and do not convert unknown rows into compatibility claims.

**Recommendation:** do not select a core-accuracy correction milestone from the evidence currently available. Keep the DLI case as the narrowest conditional discriminator, and prefer a lawful gameplay oracle if one becomes available; either would need evidence that identifies a defect before implementation scope is justified.
