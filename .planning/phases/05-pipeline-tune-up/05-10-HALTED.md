---
phase: 05-pipeline-tune-up
plan: "10"
status: halted
blocked_task: 2
implementation_head: 96dc316427cbcb686b06b650d4cc16d729c419d3
---

# Recovery stopped at the hosted equality boundary

The existing PowerPC ABI failures are corrected, but the full producer fails
its final equality command. The plan explicitly requires stopping when a full
inventory/equality run fails. No comparator repair, second gap round, later
plan, ship, merge or owner verification was performed.

[Release run 39659029223](https://github.com/szTheory/muhtari/actions/runs/39659029223),
job 119426862308, builds head `96dc316427cbcb686b06b650d4cc16d729c419d3`.
[Nightly run 39659029340](https://github.com/szTheory/muhtari/actions/runs/39659029340),
job 119426862666, builds merge `aebf09ffb8e5cd6937202edc1831de2b37c40f1e`.
Each executes native and PowerPC 607 normal and six release checks successfully,
including the unchanged ABI controls. Each JUnit judge passes. Both producers
then exit 1 at `lane_equality: members of NATIVE:` with an empty member list.

`gh api --allow-escape-sequences repos/szTheory/muhtari/actions/jobs/JOB/logs`
and `gh run download RUN -R szTheory/muhtari -n lane-equality -D build/<directory>`
returned exit 0 for both jobs/runs. Logs are temporary; authentic JUnit and pair
artifacts remain ignored build output.

Read-only replay on the release artifacts reproduces exit 1 with relative
`-DNATIVE=build/recovery-full-failed/evidence/native` and corresponding BE path.
The unchanged comparator with absolute versions of those paths and the exact
producer SHA exits 0: `hashes: 2 identical files`. The Nightly pair likewise
passes absolute-path replay with its own merge SHA. This isolates a caller/path
contract defect; it does not turn the failed hosted callers into successes.

Partial commits: `f4ce072` exposes actual target RED; `96dc316` restores bounded
unsigned rounding and target-only read-only constant placement with equivalence
tests/docs. Temporary diagnosis remains in `be-equality.yml` because the stop
condition fired before cleanup/final-head acceptance. No successful SUMMARY
status or whole-phase requirement completion is recorded. Progress remains 2/9.

The fixed table remains count400/BE300/digest650/quality450/fixes100/reserve70,
N1970 and C14853. Current charged count is 13451, including all 11 temporary
diagnostic lines and the two target toolchain lines; BE use is 199/300.

Next action is an explicit scoped recovery decision using this new evidence.
Do not blindly rerun execution or launch another gap round. Any authorized
continuation must address the relative-path caller contract, remove diagnostic
work, and prove final immutable-head complete callers before unlocking 05-04.
