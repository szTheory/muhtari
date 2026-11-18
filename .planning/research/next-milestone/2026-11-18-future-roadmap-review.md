# Future roadmap review after v10.0

**Snapshot:** origin/main at f709d493e6660b3b53a98b49b693ab91a714dcb0  
**Researched:** 2026-11-18  
**Status:** Selected by the owner on 2026-11-18 for v11.0.

## Recommendation

Recommend **v11.0 — Windows extended-path behavior**: finish the known
muhtari-run Windows output-path follow-up from v10.0. Define and qualify
explicit `\\?\` and `\\?\UNC\` paths end to end, preserving the namespace's
literal naming semantics while retaining collision protection, atomic output,
and clear failures. Keep the change in the existing runner; do not change the
public C API, ABI, state formats, emulated behavior, or deliverables. Start with
the required measured tune-up, including correction of the Phase 27 deferred
register's count mismatch.

This is a bounded known usability problem, not a claim of a data-loss defect:
the current guard may reject two distinct outputs when one uses the explicit
extended namespace and ends in a dot or space; it fails before either write.
Supporting the namespace is broader than skipping suffix trimming. The current
Windows safety path includes narrow routines, and Microsoft's documentation
requires Unicode APIs for long paths. Requirements must cover the actual
argument-to-open/replace path chain and Windows CI must exercise real runner
outputs before docs claim support. Restrict any claim to the exact namespace
forms and path lengths proven by those controls.

## Strongest alternative

Keep current behavior fail-closed: reject `\\?\` and `\\?\UNC\` paths before
emulation with a clear diagnostic, retain ordinary Windows path behavior, and
use the milestone only for the tune-up and ledger correction. This is the
lowest-risk path and closes the ambiguity, but it does not support explicit
extended-path destinations. Recommend support because the owner carried the
Windows path issue forward as a requested follow-up and the source identifies a
concrete, reproducible edge.

The strongest player-centered future direction remains a **finite gameplay
replay set**. It is not ready to promote: no exact rights-cleared titles and
independent gameplay outcomes are currently identified. The compatibility list
has 201 unevaluated entries and three skipped entries; those are unknowns, not
known defects. The 49 owned sample/diagnostic rows contain 46 passes and three
skips, mostly boot-only, title-screen or model-conformance evidence. Do not
mass-promote these rows or infer gameplay from a movie hash alone.

## Evidence register

| Finding | Evidence class / confidence | Source and consequence |
|---|---|---|
| v10.0 is shipped and archived; stable v0.17.0 is current | Exact-source project record; high | [v10.0 audit](../../milestones/v10.0-MILESTONE-AUDIT.md), [archive](../../milestones/v10.0-ROADMAP.md), [STATE](../../STATE.md). PR #80 merged. PR CI [44229018798](https://github.com/szTheory/muhtari/actions/runs/44229018798), main CI [44233889699](https://github.com/szTheory/muhtari/actions/runs/44233889699), and Release [44233889579](https://github.com/szTheory/muhtari/actions/runs/44233889579) passed. No new release PR was needed because the package version did not change. |
| The Windows comparison can reject distinct extended-namespace destinations | Source-backed finding; high | [Deferred follow-up](../../todos/pending/2026-11-17-define-extended-windows-output-path-support.md), [review fix record](../../milestones/v10.0-phases/27-headless-developer-trace/27-REVIEW-FIX.md), runner/main.c Windows trace_normalized_path. The path comparator trims trailing dot/space under ordinary Win32 assumptions; the audit found no overwrite case. |
| Full long-path support needs an end-to-end Unicode path chain | Primary platform documentation plus source inspection; high | Microsoft [Naming Files, Paths, and Namespaces](https://learn.microsoft.com/en-us/windows/win32/fileio/naming-a-file), [Maximum Path Length Limitation](https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation), and CRT [Path Field Limits](https://learn.microsoft.com/en-us/cpp/c-runtime-library/path-field-limits). \\?\ disables normal parsing; \\?\UNC\ is the extended UNC form; paths beyond MAX_PATH require Unicode APIs. The current runner comparison uses narrow routines (_fullpath, CreateFileA, _stat64) and must be audited before claiming support. |
| The Phase 27 register frontmatter counts disagree with entries | Direct ledger count noted by the accepted audit; high | [v10.0 audit](../../milestones/v10.0-MILESTONE-AUDIT.md) records 17 open/35 fixed in frontmatter versus 16 open/36 fixed entries (52 total). Reconcile before another append; no verdict needs to change. |
| Muhtari has no newly demonstrated browser/API requirement | Saved consumer boundary; medium | [Consumer review](CONSUMERS.md), together with v10.0's runner-only delivery. LibFuturo owns browser/host paths; Playstead owns player integration. No sibling operation or new shared contract is needed for this recommendation. |
| Broader gameplay coverage is not ready to promote | Current ledger and project policy; high | [Compatibility ledger](../../../docs/compatibility.md), [CONFORMANCE §1](../../preparation/CONFORMANCE.md#1-test-rom-inventory). Rights and independent gameplay outcomes must be qualified per title; unevaluated rows are not failures. |
| General tool expansion has lower current evidence | Owner direction and saved product review; medium | The v10 trace shipped as one deliberately bounded SEED-004 slice. No direct demand supports stepping, symbols, overlays, scripting, AI/TAS or a second host. |

## Scope and acceptance boundary

- Keep the existing tune-up first. Use measured current release/CI inputs and
  correct the deferred-register counts without changing their verdicts.
- Define precisely which command-line path options accept explicit local and
  UNC extended namespaces. Do not silently normalize their literal names.
- Preserve refusals when two destinations name the same file, including
  existing-file identity, parent junctions, aliases, and input/output overlap.
- Exercise successful distinct destinations and collision/failure controls
  through the actual runner in Windows CI, including the supported path-length
  and character cases. Keep old ordinary Win32 alias tests.
- Make no universal MAX_PATH or arbitrary Unicode claim unless the complete
  path conversion and filesystem operation chain is verified.
- Keep public API/ABI, state identity, core timing/audio, fixture policy and
  platform safety stops unchanged. No owner hardware or real-host run is needed.

These boundaries inform the selected v11.0 requirements. The implementation
must audit the full runner path chain and demonstrate the supported forms in
Windows CI; if a specific operation cannot safely preserve the selected path
semantics, it needs a clear bounded failure and recorded evidence.

## Rejected or deferred candidates

- Do not add broad compatibility work from the 201 unevaluated rows: no known
  failure, rights-cleared finite title set or independent gameplay oracle is
  present.
- Keep GameDrive/BupChip NO-GO until a lawful normative profile, independent
  output oracle and finite resource facts exist.
- Keep browser host, package consumer and Playstead UI work with LibFuturo and
  Playstead. No current evidence moves it into Muhtari's three deliverables.
- Do not grow SEED-004 beyond the shipped trace without demonstrated users.
- Do not change CI topology, cache, deadlines or required inventory from one
  CI/Release observation pair; preserve the existing measured-evidence rule.

## Confidence and revisit triggers

**Confidence:** High that the limitation and safe refusal exist, high that the
Windows namespace semantics require specific tests, medium that full support is
the best next milestone because demand is bounded to the carried follow-up.

Revisit the recommendation if the complete Windows path audit shows support
requires broad changes outside runner I/O, if the host CI environment cannot
produce representative namespace controls, or if the owner prefers a clear
refusal over increased compatibility. Revisit gameplay evidence when a small
rights-cleared title set and independent outcomes are available. Revisit
GameDrive only when its missing source, oracle and resource conditions change.

## Memory and lesson checks

Reconciled v10.0 shipping, the deferred audit items, the 2026-11-16 future-roadmap
review, CONSUMERS.md, the latest LESSONS.md entries and the project fan-out
brief. Relevant reproduced process lessons are to run cheap source-specific
consistency checks before long suites and to correct active-plan/ledger counts
from the actual records. The count mismatch is a direct known follow-up; no new
technical lesson was inferred without reproduction.
