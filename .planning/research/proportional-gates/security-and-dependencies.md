---
title: "Proportional gates: security, dependencies and adversarial governance"
summary: "Preserve concrete safety boundaries while replacing the structural LOC allocation mechanism with bounded engineering judgment."
read_when: "Deciding the replacement for the gate-code line cap or reviewing a new dependency or privileged check."
updated: 2026-10-23
---

# Security and dependency recommendation

**Recommend outcome-based safety gates plus a short, bounded review of maintenance cost. Remove the hard gate-code LOC allocation mechanism; do not replace it with a complexity score, dependency quota, approval service or new policy framework.** Existing product, provenance, evidence-integrity and release checks remain independent. Source size can inform review if already cheap to obtain; it should neither grant permission nor block a correct change by itself.

This is a proposed policy change, not an implementation or grant. It does not authorize the Phase 6 N654 proposal, loosen archive or performance limits, prove board correctness, or resume blocked execution.

## Local evidence and scope

Read together: [AGENTS.md](../../../AGENTS.md), [preparation index](../../preparation/README.md), [ENGINEERING §§8, 10–13](../../preparation/ENGINEERING.md), [DEC.10, DEC.15, DEC.43, DEC.52](../../preparation/DECISIONS.md), [PROVENANCE](../../../PROVENANCE.md), [quality contract](../../../docs/quality.md), [halt summary](../../phases/06-retail-cartridge-boards/06-01-SUMMARY.md) and [redesign](../../phases/06-retail-cartridge-boards/06-REDESIGN.md).

The installed gate baseline is 15,287 lines. The selected disposable redesign measures 15,941 against the authorized 15,479 ceiling. Its 654-line addition includes 139 lines of manifest/inventory glue and 91 lines of grant logic/controls. Its fixture and host checks protect real evidence; its prospective N654 selector protects the allocation rule itself. This separates genuine safety obligations from accounting machinery without declaring every control unnecessary. The candidate uses controlled observations; real board behavior and future six-native package results remain unproved. The halt is evidence of a delivery obstruction, not evidence that safety checks can be removed.

The repository already has SHA-pinned Actions, downloaded-tool digests, least-privilege job permissions, unmodified vendor hashes, source notices, archive identity and content checks. These are distinct from a gate-code line count. The lack of linked C packages does not eliminate the build/release supply chain.

## Primary evidence and limits

Sources accessed 2026-10-23. Recommendations below are project-specific judgments, not requirements claimed on behalf of these sources.

| Source | Supported finding | Limit on the inference |
|---|---|---|
| [NIST SSDF 1.1](https://nvlpubs.nist.gov/nistpubs/SpecialPublications/NIST.SP.800-218.pdf), executive summary; PO.4; PW.1–2; PW.4 | SSDF focuses on outcomes, leaves implementation mechanisms open, and calls for risk-based applicability. It describes defined check criteria and safeguarded evidence, risk modeling/design review, reuse of well-secured components, provenance, and lifecycle evaluation of third-party software. | It supplies neither a LOC ceiling nor a mandate for an SBOM platform or commercial scanner here. It supports a risk-based judgment, not certification of this proposal. |
| [OpenSSF dependency evaluation guide](https://best.openssf.org/Concise-Guide-for-Evaluating-Open-Source-Software.html) | Consider necessity and authenticity, suitability, maintenance/security response, known vulnerabilities, installation behavior and unexpected indirect dependencies. Its examples are optional and even good projects may score poorly on individual questions. | Use relevant questions rather than a badge threshold, freshness cutoff or numeric score. Few dependencies alone do not prove security. |
| [Google review standard](https://google.github.io/eng-practices/review/reviewer/standard.html) | Favor a change that improves overall code health without requiring perfection; facts take priority over preferences. | This is Google's process, not empirical proof that review prevents every regression. |
| [Google review content](https://google.github.io/eng-practices/review/reviewer/looking-for.html) | Review design, user effect, complexity and over-engineering. Tests must make useful assertions and detect broken behavior; test code also has maintenance cost. | This does not require a new test framework or testing every test helper. |
| [SQLite self-contained design](https://www.sqlite.org/selfcontained.html) and [testing](https://www.sqlite.org/testing.html), §§7.6–7.7 | SQLite demonstrates an embedded C library with few external requirements. Its mutation testing asks whether branches affect observable results; it explicitly warns that full MC/DC effort may not be cost-effective for typical applications. | A precedent for small dependencies and meaningful fault tests, not a mandate to copy SQLite's enormous test estate or coverage target. |
| [GitHub secure use](https://docs.github.com/en/actions/reference/security/secure-use), third-party Actions and permissions | Third-party Actions can compromise workflows; minimize token privileges, audit their behavior, and lock identities. The current page prefers dependency lockfiles including transitives, with full commit SHAs as a fallback; mutable tags carry risk. | Existing pinning prevents certain unexpected changes, not malicious behavior already at the selected pin. Newly documented lockfiles are a future option to evaluate if relevant, not a prerequisite for this policy change. |
| [Cargo build scripts](https://doc.rust-lang.org/cargo/reference/build-scripts.html) | Cargo compiles and executes package build scripts before the package is built; scripts receive environment inputs and can perform build tasks. | Ecosystem precedent only: a build-only dependency can execute code. This is not a proposal to introduce Rust or Cargo. |

## Comparing replacement strategies

| Strategy | Product/security effect | Maintainer/automation effect | Judgment |
|---|---|---|---|
| Absolute LOC cap with advance allocations | Forces a visible stop and may discourage needless machinery, but a one-line unsafe permission change fits; a necessary diagnostic or comment can exceed it. | Deterministic accounting also requires inventories, exception rules, allocation admission and controls. It rewards compression, relocation or externalization unless policed. | Strongest alternative when enforcing an explicit finite maintenance allowance is itself the owner's objective. It is poorly aligned with security outcomes here. |
| Advisory size metric | Flags growth for investigation; has no safety meaning on its own. | Cheap only if it does not retain an elaborate audited count or demand explanations for every delta. | Optional review context, never acceptance authority. Remove collection too if unused. |
| Qualitative review alone | Can judge scope, readability and duplication, but can miss stale evidence, skipped tests or a compromised dependency. | Flexible, vulnerable to correlated agent judgment and rubber-stamping. | Insufficient alone. Preserve deterministic semantic checks. |
| Concrete safety checks plus bounded review | Directly rejects invalid input handling, wrong artifacts and unsafe privilege changes; review judges maintenance tradeoffs. | Existing automation carries repeatable checks; a short explanation covers new complexity. No recursive control framework required. | Recommended. |
| Targeted resource bounds | A ROM-size limit, bounded emulation call, package target or measured CI timeout has an observable cost/failure. | Useful only with the workload, units and consequence stated; revisable when needs change. | Retain applicable existing bounds. Do not silently repurpose them into another structural quota. |

## Compact proposed replacement rule

> Gates protect a named fault or user constraint and use the simplest existing mechanism that demonstrates it. Review added tooling for necessity, readability, duplication, dependencies and recurring cost; size is context, not an allocation or approval gate. Preserve relevant safety and evidence checks when simplifying. Stop the affected work for a demonstrated contract violation, missing required evidence, unknown provenance, or an unapproved expansion of privileged behavior; report the concrete fault and smallest remedy. Put speculative tooling improvements in seeds. Keep existing review-round limits and automation-first verification; do not build new governance machinery to enforce this rule.

This replaces the structural budget, its advance allocations and their admission/exception machinery only if adopted. Keep one authoritative rule and cross-reference it; do not append conflicting copies across AGENTS, ENGINEERING, plans and quality docs. A migration must explicitly identify which allocation-only controls retire and which semantic controls remain. A recommendation document cannot grant itself authority to edit the accepted policy or continue Phase 6.

### Minimal stops tied to real faults

| Stop condition | Fault/user cost | Sufficient next action |
|---|---|---|
| Untrusted cartridge/state/config input can overrun memory, hang beyond the API bound, corrupt state on error, or change deterministic output unexpectedly | Host crash, resource denial, corrupted saves or divergent play | Fix the path and run the relevant boundary/regression tests; do not use structural-budget exceptions. |
| Required tests/judges failed, were omitted, or supply the wrong SHA/configuration/fixture identity | Green status would claim evidence that does not establish the shipped behavior | Restore trustworthy evidence. Keep fail-closed handling at the evidence consumer; unrelated work need not stop. |
| Imported code/data lacks an allowed exact-pin/file license, required notices, authentic source or verified download identity | Unlawful redistribution or substitution into build/runtime | Reject or replace that input. Existing provenance rules remain binding. |
| A change introduces broader token/secret exposure, new download-and-execute behavior or a new privileged release path without a justified scoped change | Credential loss or publication of attacker-controlled artifacts | Remove the added capability or review it as an explicit trust-boundary change with relevant evidence. Ordinary dependency use is not automatically an owner-consent event. |
| A claimed simplification demonstrably loses a required fault check or turns its failure into success | Known regression becomes undetectable | Preserve the check or provide the simpler equivalent and run the distinguishing fault case. |

An observed defect is different from a suggestion that a test helper could be more elegant. Do not relabel every tooling finding a safety blocker to evade the existing seed and review-round rules. A stop must point to the affected accepted requirement, reachable failure or invalid acceptance claim. New evidence can justify a scoped re-plan; speculative scenarios do not automatically create phase requirements.

## Dependency selection without absolutism

Use this decision order: remove the need; reuse what is already here; use the C standard library; use an existing platform/tool facility in the host/build layer; consider a narrow permissive seed; compare a real package with the minimum own code. Platform reuse must preserve the core's portability, determinism and lack of host-device code. For a small, stable operation, another understandable copy can cost less than a new abstraction or dependency. Prefer the established simple mechanism over novel code merely to avoid a dependency count.

Choose a seed when the imported surface is small and understandable, the license is valid at the exact pin, upstream updates are infrequent/manageable, and owning the local maintenance is credible. Follow PROVENANCE: unchanged vendored files retain their hashes; a modified/ported seed follows the derived-code route and notice/parity rules. Vendoring does not remove the dependency's vulnerabilities. Copying the same upstream code into several places can multiply patch work; use one local module when those copies must change together.

Choose a package when it avoids a substantial correctness or security burden and its total lifecycle cost is lower: for example, complex standardized functionality where a hand implementation is riskier. Neither package popularity nor fewer authored lines settles the choice. A mature package can be safer than an amateur replacement; an oversized framework can be worse than a few obvious lines. No package is needed merely to implement this governance change.

Record the decision in the existing notice or change description, without a new register or scoring system:

1. What behavior is needed, and why existing facilities or a small seed are insufficient.
2. Authentic upstream, exact version/commit and integrity identity; file-level license and required release notices.
3. Direct and transitive scope: what runs in production, what executes during build/tests, and what reaches release artifacts or credentials. Disable unnecessary features and downloads where practical.
4. Relevant known vulnerabilities and maintenance/security-response evidence; who will update the local copy/package, how upstream fixes will be noticed, and what would trigger replacement. A vulnerability matters according to affected version, reachable use and impact; an absent advisory is not a safety proof.
5. Appropriate integration evidence, including portability/determinism for code entering the core and execution privileges for build tools.

An exact pin gives repeatability and limits substitution; freezing forever can retain known bugs. A digest is not a code audit. A build-only package is not harmless merely because it is absent from release ZIPs. Maintain a small, flat tree by eliminating unnecessary dependencies and features, not by denying their transitive or copied origins. Reuse existing pins/notices and tune-up routines; adopt an analyzer, SBOM service or update platform only if a concrete dependency estate makes the existing approach insufficient.

## Adversarial pass: test the rule before adopting it

These are review scenarios and candidate acceptance checks, not a request to add a test for each row or a policy engine.

| Attack or failure | Expected response |
|---|---|
| Delete an ASan job, drop a fixture, or accept stale hashes to make the workflow shorter | Reject the lost behavior/evidence; a smaller diff earns no safety credit. |
| Add a few lines that grant a broad write token or execute an unverified download | Investigate the concrete new authority, even if LOC shrinks overall. |
| Hide logic inside YAML, generated C, a package, or a copied script | Review the actual executed behavior and maintenance/trust surface; no budget can be earned through relocation. |
| Add ordinary numeric board expectations or improve explanatory comments | Accept on behavior and maintenance merit, with relevant tests; no allocation negotiation. |
| Call every refactor a security improvement and introduce a generic policy DSL | Require the present fault, simpler alternative and evidence; defer speculative infrastructure. |
| Say “reviewed” without discussing changed inputs, callers, failures or dependencies | Request a short concrete explanation and relevant executed checks. An independent agent can challenge the author where the authorized workflow already provides one; do not claim independence from identical context or add owner UAT. |
| Change an exception and its validator together so the candidate validates itself | Compare against the trusted policy and authorized scope. Preserve independent authority for remaining real resource limits; deleting LOC allocations does not authorize other grants. |
| Add a permissive snippet with no upstream identity, or claim a vendor copy is no dependency | Apply existing provenance/notice requirements and lifecycle ownership. |
| Use urgency or exhausted review rounds to skip a demonstrated failing product/safety check | Keep the affected work incomplete and name the defect; round limits bound review churn, not permission to ship known failure. |
| Expand “preserve safety” into tests asserting filenames, exact regex structure or every internal helper call | Test the actual bad input/evidence and expected verdict. Reject brittle structure checks unless that structure itself defines a necessary public/trust contract. |

For hostile public inputs, keep malformed/truncated/oversized input tests, integer-bound cases, deterministic state/error behavior and bounded execution checks. For a security-relevant gate, exercise representative wrong identity, missing evidence and actual rejection behavior through the consumer. Use a narrowly targeted mutation or fault fixture when it demonstrates the check is live. Do not require broad mutation infrastructure, a meta-test per assertion, or exhaustive testing of harmless internal rearrangements. SQLite's testing precedent supports fault sensitivity; it does not establish an affordable universal test quantity for this project.

## Residual risks and revisit triggers

The replacement cannot mechanically prevent slow tooling growth. Reviewers can agree on an overbuilt design; agents can share blind spots. Keep findings concrete, prefer small changes and existing mechanisms, and inspect recurring cost at the existing tune-up. Do not compensate by adding an ever-growing mandatory checklist.

Revisit if actual tooling growth repeatedly obstructs product work, repeated review misses cause invalid green CI, a new runtime dependency crosses a meaningful trust boundary, update ownership fails, build tools gain secret-bearing privileges, or dependency breadth makes manual provenance/update tracking unreliable. Select a remedy for the observed failure: simplify a workflow, isolate privileges, add a missing fault test, replace an unmaintained component, or then evaluate automated dependency inventory. None of these triggers justifies reinstalling an arbitrary LOC ceiling by default.

Research stopped after convergent primary guidance plus local halt evidence identified the same tradeoff: meaningful boundaries and maintainability judgment are complementary. More ecosystem checklists would add breadth without changing this recommendation. No product code, accepted policy, allocation or execution state was changed for this note.
