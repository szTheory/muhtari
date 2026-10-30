---
phase: 05-pipeline-tune-up
plan: "03"
status: halted
blocked_task: 3
implementation_head: d1b443506bac6d20c3f4cc0a1d6e51d87faa7249
---

# Full PowerPC integration halted

Task 3 cannot satisfy its required hosted full-inventory/equality gate. Do not
advance to 05-04 or claim 05-03/Phase 5 complete. The executor's three-fix limit
and D-20 stop further implementation in this run. No ABI allowlist or writable
data control was weakened. No new GSD step, gap plan, merge or production release occurred.

The first full-inventory job exposed `abi.undefined` rejecting `__divdi3`, and
`abi.globals` rejecting these writable symbols:

- `muhtari__cartdb_2600_prefix_count`
- `muhtari__cartdb_7800_entry_count`
- `muhtari__cartdb_7800_key_count`
- `muhtari__cartdb_7800_layout_count`

Its existing undefined/global negative controls passed. The same job also
reproduced direct execution of PowerPC install/embed consumers; the committed
emulator/toolchain/staging fixes address that integration path, subject to the
current hosted result.

Evidence: `gh api --allow-escape-sequences
repos/szTheory/muhtari/actions/jobs/119400329664/logs` — exit 0, contains the
named ABI failures in [run 39651391703](https://github.com/szTheory/muhtari/actions/runs/39651391703).
That obsolete run was cancelled; its failure output is diagnostic evidence,
not a successful full-inventory result. Logs remain outside the repository.

`git diff 3f20455..d1b4435 -- src include tests/cmake/abi.cmake tests/abi
cmake/toolchains/ppc-linux-gnu.cmake` — exit 0, empty: the failing core, ABI
checks and target toolchain are unchanged at the final implementation head.

The most advanced final-head direct equality job was
[run 39652860054](https://github.com/szTheory/muhtari/actions/runs/39652860054),
job 119405429568. It was cancelled after 6m35s at 18:33:58 UTC. Its available
logs directly reproduce the same two PowerPC ABI failures at this head, with
both negative controls passing. The native 607-test and six-release-test
inventories completed first. PowerPC package/equality proof remains absent.
`gh api --allow-escape-sequences
repos/szTheory/muhtari/actions/jobs/119405429568/logs` — exit 0. No cancelled
duration is used as a successful timing sample.
Redundant own runs were cancelled; no run mutates nightly issues on this PR.

All implementation commits remain on `phase/05-pipeline-tune-up`; draft
[PR 20](https://github.com/szTheory/muhtari/pull/20) is retained. The 300-line BE
allocation and fixed C14853 ceiling remain in force. Recovery must use this
new evidence through the existing bounded gap route; it is not an owner UAT.

## Targeted diagnosis commands

On the existing Linux PowerPC build, these isolate the retained failures and
controls without claiming full-inventory coverage:

```sh
ctest --test-dir build/be32 -R '^abi[.](undefined|globals)([.]self_test)?$' -V --output-junit build/be32/junit-abi.xml
powerpc-linux-gnu-nm -P build/be32/libmuhtari.a
powerpc-linux-gnu-objdump -t build/be32/libmuhtari.a
```

`cmake --workflow --preset be32-compile` recreates the configured/built target
after the pinned fixture tools and distribution cross compiler/qemu packages
from `be-equality.yml` are present. The commands above are recovery guidance,
not additional verification executed by this halted run.
