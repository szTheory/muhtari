---
title: "Proportional gates: architecture and maintenance economics"
date: 2026-10-23
status: research-only
---

# Recommendation

Replace the blocking maintenance-line allowance with **existing executable contracts plus a bounded, evidence-based review of each changed boundary**. Keep physical size as optional diagnostic context; do not install a replacement complexity score or dashboard. Block concrete contract failures, unsupported scope/dependencies and demonstrated unnecessary machinery. Do not block solely on line growth, a tool's default complexity threshold, or a reviewer's preference for a different architecture.

This recommendation concerns the maintenance proxy. It neither approves the proposed N654 allocation nor changes installed policy, archive bounds, performance authorities, Phase 6 status or board acceptance. A separate implementation step would reconcile the accepted replacement with their current consumers.

# What the repository establishes

| Observation | Evidence | What it does and does not establish |
|---|---|---|
| The current policy counts physical lines, including comments, controls, shared helpers and embedded glue; additions need an advance allocation. | `ENGINEERING.md:249–275`; `gate_count.cmake:18–25, 33–80` | Auditable size measurement, not a measurement of architecture or maintenance effort. Counting the counter makes policy overhead visible. |
| Phase 6's accepted 192-line estimate failed a complete disposable construction by 473 lines. | `06-01-SUMMARY.md:87–114` | The estimate did not fit this construction. It does not prove all fitting constructions impossible or all checks necessary. |
| Reusing the existing dispatch and consolidating report parsing produced a construction of 654 net lines, still 462 over the allowance. | `06-REDESIGN.md:10–37` | Concrete simplification exists; saving 11 lines does not establish that the remaining architecture is optimal. |
| The redesigned proposal spends 91 lines on narrow admission and controls; another 123 concern fixture comparability and controls. | `06-REDESIGN.md:47–65` | Some work implements authority over the proxy itself. Fixture comparability separately prevents genuine false evidence; it must not be deleted merely because the size proxy goes away. The 91-line bucket is not a proven removal saving. |
| Whole-file admission brings 125 expectation rows into the count, including 73 inherited rows. | `06-REDESIGN.md:27–33`; `06-01-SUMMARY.md:116–134` | Accounting growth can differ from newly authored behavior. Honest whole-file counting does not remove this mismatch. |
| The halted construction tested controlled observations, not real boards or complete host equality. | `06-REDESIGN.md:101–109`; `STATE.md:76–91` | Policy fit and passing controls are insufficient evidence of product delivery. |
| Current quality evidence binds artifacts to commits, configurations and comparable environments before judging results. | `docs/quality.md:3–34`; `quality_join.cmake:16–41, 55–69` | There are real contracts worth preserving independently of a size metric. |

References above are repository-relative: preparation documents are under `.planning/preparation/`, Phase 6 documents under `.planning/phases/06-retail-cartridge-boards/`, state under `.planning/`, and scripts under `tests/cmake/`. These are inspected repository records, not independently rerun experiments in this research. No hosted acceptance or performance result is claimed.

The demonstrated failure is a blocked construction and additional admission work. It is **not** evidence that the owner should verify software manually, that all test tooling is waste, or that any particular replacement will reduce hosted CI cost. Those would require other evidence.

# Candidate measures and their failure modes

The assessments below are architectural inference applied to this repository, not universal empirical rankings.

| Measure | Useful question | False positives, gaming and missed architecture | Appropriate role here |
|---|---|---|---|
| Physical LOC / source ratio | How much text would a maintainer inspect? Where did growth occur? | Formatting, comments and data affect the count. Compression, moving predicates and generated wrappers can improve the number without improving understanding. Ratios grant headroom when unrelated core/data grows. A small script can still orchestrate many coupled systems. | Optional diff context; no acceptance ceiling or advance line allocation. |
| Cyclomatic complexity | Where are many decision paths concentrated? Which cases need tests? | A straightforward opcode decoder has many branches for a legitimate finite instruction set (`src/cpu.c:661` onward). Splitting it into forwarding functions moves complexity between functions; dispatch tables move it into data. It misses distributed protocol/state complexity. | Targeted investigation of changed control flow; no project-wide numerical cap. |
| Cognitive complexity / nesting | Is nested control flow making a changed function difficult to follow? | Does not establish correctness or module quality. LLVM's implementation ignores preprocessor conditionals and recursion cycles; its default 25 is a configurable diagnostic threshold, not a locally validated acceptance bound. Extraction can lower a function's score while increasing navigation and coupling. [LLVM documentation](https://clang.llvm.org/extra/clang-tidy/checks/readability/function-cognitive-complexity.html) | A review aid if already available; no new dependency or custom parser. |
| Coupling, fanout and dependency topology | Does this feature create new knowledge that several modules must duplicate? Does a helper reach across ownership boundaries? | Include/call counts conflate standard-library use, stable leaf helpers and unstable domain knowledge. Existing `internal.h` sharing makes raw include fanout uninformative. A dependency graph cannot decide whether two consumers truly share a contract. | Inspect actual changed interfaces and knowledge ownership; preserve the flat dependency preference. |
| Change locality / churn | Must the same behavior be edited in many places? Is a boundary repeatedly implicated in regressions? | One broad portability change may correctly touch many files. Young modules lack useful history. Relocation creates artificial locality; a central god-module can look local. | Investigate observed repeated change or drift; do not cap file counts or assume co-change proves causation. |
| Executable contracts | Does the deliverable obey its promises, and does the test reject a representative violation? | Green tests can share the implementation's mistaken assumption, test the wrong artifact, or miss integration. Coverage alone does not establish assertions. | Primary hard acceptance, including identity, missing-output and domain-boundary evidence where relevant. |
| Actual resource budgets | Does the product fit a real deployment or operating limit? | A baseline ratchet can become another arbitrary freeze. Compiler/ZIP variation and unmatched environments can create false alarms. Instruction counts on the test card cannot prove retail performance. | Hard bounds when tied to an explicit capacity or accepted regression policy and comparable measurements; preserve current independent authorities pending any separately authorized revision. |

The core necessarily contains behavior complexity: opcode rules, address decoding, chip timing and hardware modes. Tooling contains different complexity: evidence transport, schema interpretation, trust boundaries, process invocation and failure propagation. Both need maintainable code, but they should not share one numerical ceiling. A table-driven hardware fact and a generic policy interpreter can have similar length with very different change costs. The source-cited MARIA constants and assertion (`src/maria.c:5–28`) illustrate useful explanatory text and executable relationships that a physical-line cap charges equally.

# External evidence and limits

Google's review guidance asks reviewers to assess design, functionality, complexity and useful tests, including whether assertions catch broken code. It warns against speculative generality and says test code also needs maintainability. This supports reviewing actual boundaries rather than treating all new automation as valuable. It is an engineering practice guide, not a causal study or a mandate to import Google's process. [What to look for in a code review](https://google.github.io/eng-practices/review/reviewer/looking-for.html)

Google's review standard balances progress with code health, distinguishes optional polish, gives facts precedence over preferences and permits the author's choice among equally valid approaches. That is useful protection against an architecture review becoming an unlimited redesign exercise. [The standard of code review](https://google.github.io/eng-practices/review/reviewer/standard.html)

A 2005 empirical study of five Microsoft systems found relationships between code complexity and post-release failures, but no universally best set of complexity predictors. This supports rejecting a universal replacement number; it does not prove metrics are useless or validate the policy proposed here. [Mining Metrics to Predict Component Failures](https://www.microsoft.com/en-us/research/publication/mining-metrics-to-predict-component-failures/)

A study revised in July 2024 used 216 developers with one to four years of experience rating 12 Java classes. Cyclomatic and cognitive complexity were modest predictors of perceived complexity, but did not predict perceived severity. Its language, population and task limit transfer to an agent-maintained C emulator and CMake/sh tooling. [Early Career Developers' Perceptions of Code Understandability](https://arxiv.org/abs/2303.07722)

SQLite documents mutation testing as a stronger check that branches affect observable outcomes, while also describing false positives from performance-only branches. It retains defensive code rather than deleting it to improve coverage. This supports representative fault controls and resisting metric-driven deletion; it does not justify copying SQLite's exhaustive testing scale here. [How SQLite Is Tested, sections 7.2 and 7.6](https://www.sqlite.org/testing.html)

All external sources above were fetched on 2026-10-23. LLVM's page describes development documentation, not necessarily the installed tool version. No prohibited emulator source was opened. No outside code or dependency is proposed.

# Minimal policy usable by agents

Proposed replacement text for later synthesis and adoption:

> Keep repository checks in CMake `-P` or POSIX sh and reuse existing checks before adding machinery. Each added or materially changed check must identify its scoped fault, the existing judge it extends or why none suffices, the evidence it consumes, and a representative passing case and fault control. Reuse the ordinary plan or PR explanation; do not create a new policy database.
>
> Acceptance blocks on a violated documented contract, missing required evidence, a demonstrated false pass/failure, unsupported scope or dependency, or a concrete maintainability defect in the changed code. A maintainability objection must identify the code, the present failure/change burden, and a smaller sufficient remedy. Numerical size/complexity results and personal architecture preferences alone do not block. Keep existing review-round limits and defer unrelated tooling findings as seeds.
>
> No line allowance, composite score, mandatory complexity service, generic budget interpreter or new dependency is required. Resource bounds remain separate from maintenance proxies and require their own measurement and authority.

“Concrete maintainability defect” includes two changed consumers duplicating the same evolving protocol predicate, a helper hiding required inputs through global/process state, unrelated responsibilities entangled so that a scoped change alters another contract, or a framework adding unused modes that increase the current test surface. It does not mean “the reviewer would organize it differently.” Reviewers should show the duplication/coupling or trace the failure; they should not demand a speculative full alternate implementation before accepting ordinary work.

Before adding a helper, the agent should answer, briefly in the existing change rationale: **What present responsibility becomes easier to understand or keep consistent? Why does extending the current owner not suffice? What callers and failure behavior will this helper have?** There is no arbitrary minimum caller count. A single-use helper can clarify a real parsing boundary; a short repeated expression may remain clearer inline. Copying a small, stable permissive seed can be preferable to a dependency; copying evolving domain rules into multiple judges usually requires a shared owner or an equivalence check.

For additional recurring checks, evidence can be a reproduced regression, an existing required contract with an uncovered credible failure, or an integration boundary newly introduced by the feature. A previous incident is not mandatory: that would prohibit preventive testing. One-off observations with no recurring value remain recorded commands and exit statuses under rule 6. Controls should reject the named fault rather than prove that every hypothetical corruption has its own separate test. Trust boundaries may require several distinct controls; this is an argument about failure modes, not a numerical test budget.

This is a small review contract, not an independently enforceable architecture theorem. Its weakness is review judgment. The required code/fault/remedy evidence, existing limited rounds, scope limits and nonblocking status of preferences constrain that weakness without another metrics subsystem. It adds no permanent dashboard and does not make every helper await owner approval.

# Decision matrix

| Choice | Delivery and maintenance tradeoff | Decision / would change if |
|---|---|---|
| Existing contracts + constrained review; optional local metrics | Accepts understandable necessary growth. Can catch distributed coupling and false evidence; requires disciplined judgment. | **Recommended.** Reconsider if repeated accepted changes produce attributable maintainability regressions that this specific review contract failed to identify. First improve the missed contract or boundary, rather than inventing a global number. |
| Hard absolute line cap with scoped grants | Auditable restraint; makes unchecked growth impossible. Grants consume implementation/review work and cap estimates can halt useful contracts without proving waste. | **Strongest alternative** when a fixed text surface is itself the explicit product constraint. Prefer only if the owner intentionally values that ceiling over otherwise correct feature delivery, and accepts its admission overhead. |
| Complexity or coupling cap | More direct signal for some local structures; still misses distributed behavior, language differences and legitimate decoders. Requires tooling and exception maintenance. | Do not adopt now. Consider a narrow check only after representative local cases show a repeatable defect and a threshold separates useful findings from acceptable code, using existing tooling. |
| Advisory metrics alone | Low friction; no arbitrary blocker. Does not stop speculative frameworks or false evidence. | Insufficient by itself; retain executable contracts and constrained review. |
| Only runtime/package/CI resource budgets | Connects cost to an observable outcome. A small, fast binary can still be difficult to maintain; hosted duration is noisy. | Complement, not replacement for maintenance review. Establish product/resource rationale and comparable measurements before adding another hard ceiling. |

Do not claim a percentage cost reduction, a maintenance optimum, an exact removable line total, or Phase 6 readiness from this research. The practical next policy implementation should remove only the obsolete proxy and its exclusive admission machinery, preserve domain/evidence contracts, and verify their positive and negative behavior through the existing CI workflow. That is a bounded migration, not authorization for a broad tooling rewrite.
