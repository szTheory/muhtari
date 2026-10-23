---
title: "Replace maintenance line budgets with contract checks and architecture review"
date: 2026-10-23
status: accepted
scope: Repository-check policy and its Phase 6 migration
---

# Recommendation

Retire the hard maintenance-line ratchet, its allocation arithmetic, and the
machinery used exclusively to count or authorize those lines. Do not replace it
with a cyclomatic-complexity cap, weighted score, file-count quota or dashboard.
Use executable checks for concrete requirements, and a short evidence-based
architecture review for the changed design. Reuse the existing review and PR
explanation; add no policy registry or approval service.

Accepted 2026-10-23: the owner authorized this recommendation and its bounded
migration in quick task261023-jkd. ENGINEERING §11 is the authoritative current
review contract, with AGENTS rule7 pointing to it. The migration retires the
line gate; it does not approve N654/C15941 or complete any board requirement.
Historical measurements and acceptance remain historical.

## Why change the policy

The goal was sound: keep support machinery from becoming harder to maintain than
the emulator. Physical lines measure text volume, however, while the decision we
need concerns responsibilities, dependencies, failure behavior and future change.
Those are different things.

The local evidence is unusually clear. The first construction added665 counted
lines against an allowance of192. One bounded redesign reduced this to654, still
462 above the allowance. That count includes125 expectation rows,73 inherited
rows newly brought into the inventory, and91 lines allocated to admission and
its controls. These are not interchangeable units of architecture complexity.
The experiments demonstrate delivery friction and policy overhead; they do not
prove all checks necessary or all fitting designs impossible. See the historical
[halt](../../phases/06-retail-cartridge-boards/06-01-SUMMARY.md) and
[redesign](../../phases/06-retail-cartridge-boards/06-REDESIGN.md).

NASA's measurement guidance starts with the decision a measurement serves and
recommends the minimum useful measurement set. DORA warns against making metrics
the goal, relying on one metric, and investing more in measurement integration
than its improvement value. These are relevant principles, not instructions to
import a space-program process or service-delivery dashboard into an emulator.
[NASA measurement objectives](https://swehb.nasa.gov/spaces/7150/pages/16449970/SWE-090%2B-%2BMeasurement%2BObjectives),
[DORA metric pitfalls](https://dora.dev/guides/dora-metrics/).

## Decisions and tradeoffs

| Decision | Recommendation | Strongest alternative and tradeoff |
|---|---|---|
| Hard LOC ceiling | Remove the active metric and its exclusive counter, inventory and admission special cases | Keeping it report-only is cheaper initially, but retains a counting system without an identified recurring decision |
| A replacement complexity number | None; investigate nesting, branch paths, coupling or churn only when answering a concrete question | A local analyzer warning can help an observed recurring defect; defaults are not validated project acceptance thresholds |
| Architecture acceptance | Review ownership, change locality, state, contracts and dependencies in the changed boundary | A numerical rule is reproducible, but cannot distinguish a legitimate hardware decoder from needless abstraction |
| New recurring automation | Name its fault, show the existing check it extends or why none suffices, and exercise representative pass/fault behavior | Require a prior incident for every check: rejects useful prevention of newly introduced failure modes |
| Tests of test infrastructure | Concentrate adversarial controls on judges, parsers, artifact identity, publication and other important failure boundaries | A separate harness for every ordinary test/helper adds maintenance and can mirror its implementation |
| Shared helper versus duplication | Centralize a genuinely shared changing invariant; allow short stable duplication when it keeps ownership clear | Deduplicate everything: saves text while increasing indirection and generalized modes |
| Dependencies | Prefer existing code, standard facilities or a narrow permissible seed when they meet the need at lower lifecycle cost | A maintained dependency can be safer and cheaper for difficult functionality; zero dependencies is not a security proof |
| Actual resource bounds | Tie each new bound to a product, platform, reliability or operating-cost objective and comparable measurement | Ratchet every baseline maximum forever: sensitive to toolchain variation and can freeze harmless growth |
| Review discipline | Block substantive current defects, label optional improvements, keep existing finite review/gap limits | Unrestricted expert judgment can become perpetual redesign or preference enforcement |
| Further fan-out | Stop when independent relevant perspectives agree and remaining differences have explicit tradeoffs | More roles and more metrics can add volume without changing the recommendation |

Google's review guidance assesses design and unnecessary generality, and explicitly
treats test code as maintainable code. Its review standard favors progress that
improves code health without demanding perfection. Its small-change guidance
defines smallness conceptually rather than as a simple line count. The proposed
review below adapts these principles to this repository.
[Review scope](https://google.github.io/eng-practices/review/reviewer/looking-for.html),
[review standard](https://google.github.io/eng-practices/review/reviewer/standard.html),
[focused changes](https://google.github.io/eng-practices/review/developer/small-cls.html).

## What to block automatically

Keep automated enforcement of documented, reproducible obligations: bounded
hostile-ROM execution, integer and instance-state rules, allocation behavior,
correct output and cross-platform equality, ABI and host contracts, valid
artifacts, provenance, fixture integrity, release integrity, required test
execution, and actual sanitizer/analyzer failures. Missing required evidence is
not a pass. Preserve current independent performance, compatibility and package
consumers during this migration.

The absence of a line cap does not permit deleting tests or weakening a judge
to make CI green. Removing or consolidating an existing check must identify the
same fault still covered by the retained check and demonstrate it where the
boundary warrants a control. A check is not sacred merely because it exists;
neither is its deletion an improvement merely because it saves code.

Distinguish a correctness contract from its proxy. Exact named-test execution
helps detect missing coverage; total test count does not demonstrate good
coverage. Package size is observable, but an old compressed-size peak alone
does not establish the right product limit. Current floors and399937-byte cap
remain active here; their separate review must not be smuggled into LOC removal.
The same reasoning should later be applied to other structural line quotas,
including the hosted UI-test allowance, without silently deleting their tests.

## What architecture review should examine

Use these questions in the ordinary review, with concrete examples from the diff.
They are prompts, not a scored form or a mandatory new document for every change.

| Concern | Useful evidence in this project | Example of a substantive finding |
|---|---|---|
| Ownership and encapsulation | Which module owns a fact, state transition or evidence decision? | Runner and libretro independently reinterpret a cartridge-layout rule and can disagree |
| Change locality | Trace one plausible supported change through its actual callers and schemas | Adding one diagnostic requires manually synchronizing several copies of the same predicate |
| Coupling and interface burden | Required inputs, global state, lifetime, error behavior, direction of dependencies | A quality helper silently relies on process-wide mutable state set by a distant caller |
| State-space and control flow | Hardware modes, combinations, explicit bounds, invariants, error paths | A generic policy interpreter introduces unused modes and interactions that current scope does not need |
| Understandability | Trace the normal and failing paths using the existing code and concise contracts | Repeated forwarding layers hide the one place a bank switch or rejection happens |
| Test independence | Does the expected value come from independent facts, and does failure reach the real caller? | A host test substitutes the same mistaken mapper formula or accepts an empty report |
| Build and operational cost | Required tools, external services, network actions, CI latency and failures | A local deterministic check gains a remote service and a credential without needing either |
| Dependencies and updates | Runtime/build scope, transitive graph, provenance, patch/update path | A small helper imports an unrelated framework; a copied security-sensitive component has no update path |

A blocking review finding must name the affected code and requirement or
demonstrated present change burden, explain the failure or maintenance mechanism,
and state the property a proportionate fix must restore. The reviewer need not
implement a complete competing architecture. Style preference, a metric crossing
a default threshold, hypothetical future extensibility, or a merely different
valid design is not enough. Strong architectural reasoning may be preventive;
an escaped bug is not a prerequisite.

Existing required behavior can block wherever it lives, including tooling.
Unrelated desirable tooling enhancements remain seeds. Review caps never turn a
known correctness or security failure into acceptance; an unresolved material
conflict stops with that specific issue rather than restarting a general audit.

## Dependency rule in practice

Follow the existing ladder: remove the need, reuse this repository, use the
standard library/platform, then choose the smallest sufficient implementation or
component. Prefer a narrow reviewed permissive seed to a package when its license,
portability, update responsibility and total maintenance cost are favorable.
Keep notices and provenance at the pinned revision; do not infer permission from
the repository's top-level license alone. Existing PROVENANCE restrictions on
seeds remain authoritative until separately changed.

Copying a small stable helper can be sensible. Copying a complex parser,
cryptographic implementation or evolving security protocol simply to avoid a
dependency can make us responsible for much harder work. A dependency can be the
correct choice when it supplies a present capability and has a credible trust
and maintenance story. Consider its transitives and build tools as well as what
ships to users. No blanket package-count cap or new scanning service is needed.
NIST SSDF recommends evaluating and maintaining reused software, including its
provenance and vulnerabilities; it does not establish that copied code is always
safer than packages. [SSDF PW.4](https://csrc.nist.gov/pubs/sp/800/218/final).

## Adopted rule and authoritative location

The accepted review contract lives once in [ENGINEERING §11](../../preparation/ENGINEERING.md#11-docs-standards).
AGENTS rule7 is its concise operational pointer. The following is the historical
proposal reviewed during research, retained to explain the adoption:

> **Proportional gates.** Repository checks use CMake `-P` or POSIX sh and reuse
> existing mechanisms. A new or materially changed check names its concrete fault
> and exercises representative passing and failing behavior. Review changed
> ownership, coupling, state, duplication, dependencies and recurring operating
> cost with evidence; line counts and complexity scores alone never block work.
> Do not add speculative frameworks, policy registries or tests-of-tests without
> a current fault boundary. Consolidation preserves required behavior and fault
> detection. New resource bounds need a stated product/platform objective and
> comparable measurements. At most two code-review fix rounds and one gap round
> per phase. Optional test/tooling improvements become seeds; violations of an
> existing required behavior remain blockers. Re-plan a gap only with new
> evidence. Do not turn architecture review into an unbounded redesign loop.

No new language/tool exception is needed to retire the current metric. CMake/sh
remain the default. If later work demonstrates that they force unnecessary
complexity at a particular boundary, evaluate that concrete alternative rather
than adopting an additional ecosystem by convenience.

## Adversarial checks against the recommendation

| Attack or failure | Required response |
|---|---|
| A large generic framework is green in CI | Review its current need and extra modes; passing tests alone do not justify unnecessary architecture |
| A small diff creates a dangerous dependency or evidence bypass | Evaluate the trust boundary and actual failure, regardless of its line count |
| A large opcode table or independent expected-value set grows | Review correctness and its source; size alone is not a maintenance defect |
| An agent claims that every new test needs a separate failure framework | Require a named uncovered failure boundary; reuse meaningful existing assertions and controls |
| Review says only "too complex" | Require the code, mechanism/burden and property to restore; do not demand subjective compliance |
| An agent removes the entire quality job to retire LOC | Reject it: remove only the obsolete metric and exclusive machinery; retain unrelated evidence and domain judges |
| Removing the metric accidentally permits all base-policy deletions | Exercise an unrelated metric deletion and show it still fails |
| Review limits are used to merge a known unsafe change | Stop on the specific unresolved contract; optional polish goes to seeds |
| The recommendation itself becomes another enforced scorecard | Reject that implementation; existing review prose is enough |

## One bounded migration before Phase 6 resumes

1. Reconcile rule7, ENGINEERING§11, DEC.10, current docs and the active Phase6
   instructions in one scoped policy change. Mark the N654 proposal superseded
   by policy retirement rather than approved or historically successful. Preserve
   shipped Phase5 and halted01 evidence; historical records do not need rewriting.
2. Remove `gate_lines` from active policy/evidence and retire the counter,
   whole-file inventory, its dedicated control registration, initial-adoption
   branches and LOC-only reason handling. Do not leave this as advisory baggage
   without a concrete recurring decision it serves.
3. Bridge the trusted-base transition narrowly. The current judge refuses
   deletion of base metrics, so remove only the retired literal metric from the
   effective comparison at the documented boundary. Do not add a generic
   "ignore missing metrics" mode, rewrite base artifacts, or loosen other bounds.
   Exercise legacy-base→current and current-base→current, plus unrelated deletion
   and evidence/identity negative cases. Preserve actual present invariants;
   broad new schema hardening is a separate seed, not a hidden prerequisite.
4. Keep the legitimate Phase6 work: owned fixtures, authenticated comparable
   evidence, numeric diagnostics, real three-host behavior, hostile-input bounds,
   cross-platform equality and existing package acceptance. Drop only allocation
   prototypes and line-admission tasks. Prepare the minimal successor boundary
   while preserving halted01; do not recreate a full disposable budget fit test.
5. Run focused quality controls, inventory/configuration checks and
   `cmake --workflow --preset ci`; verify the affected hosted head/base path when
   implementation reaches CI. Record commands, exits and run URLs. Do not claim
   full boards or six-native acceptance from this research or from migration CI.

The detailed path map and boundary reproductions are in
[CI and migration research](ci-and-migration.md). The original research pass did
not perform the migration; the accepted quick task now implements and verifies
it. It is not a requirement to implement a
dashboard, centralize all existing domain policy, or redesign the whole pipeline.

## Evidence and follow-up

Three independent research passes considered architecture/maintenance economics,
CI/release/test behavior, and security/dependencies. Their artifacts separate
repository observations, fetched primary evidence and recommendations:

- [Architecture and metric alternatives](architecture.md)
- [CI and migration boundaries](ci-and-migration.md)
- [Security and dependency tradeoffs](security-and-dependencies.md)

One independent adversarial review of this synthesis found no material issue.
It checked the current judge, producer, historical redesign and all three
research documents for lost safeguards, surrogate quotas, subjective review
loops and overclaimed acceptance. This verified the proposal's coherence;
executable retirement evidence belongs to the quick-task summary, and future
board behavior remains unproved.

Revisit the review method when a concrete accepted change causes an attributable
maintenance or safety failure that it missed. Investigate that boundary using
existing tools; do not invent a global numeric rule from one incident. Observe
whether delivery stops for proxy accounting cease while actual regression
checks remain effective. No measured maintenance-time or CI-cost improvement is
claimed in advance.
