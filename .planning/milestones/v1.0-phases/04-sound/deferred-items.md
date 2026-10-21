## Deferred Items

- Tooling seed: `policy.compat_doc_fresh` and its self-test share
  status: acknowledged
  `build/ci/policy/compatibility.md` as output. One full CI invocation reported
  a stale page while both ran concurrently; the generated and committed pages
  were identical afterward, the isolated freshness check passed, and a complete
  workflow retry passed all 588 regular and six archive tests. The compatibility
  source, ledger and tests were unchanged by this plan. Give the self-test a
  separate output during the later tooling tune-up; this is a test/tooling seed
  under AGENTS rule 7, not a sound requirement or an additional review round.
