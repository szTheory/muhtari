# Phase 29: Windows extended-path runner support - Discussion Log

> **Audit trail only.** Do not use as input to planning, research, or execution agents.
> Decisions are captured in CONTEXT.md — this log preserves the alternatives considered.

**Date:** 2026-11-18
**Phase:** 29-windows-extended-path-runner-support
**Areas discussed:** namespace contract and Unicode semantics; collision and safe replacement; Windows CI/UNC acceptance; documentation and dependency scope

---

## Namespace contract and Unicode semantics

| Option | Description | Selected |
|--------|-------------|----------|
| Reject extended paths and keep the current ordinary-path contract | Lowest change and fails closed, but leaves WINPATH-01–04 unmet and does not resolve the Phase 27 follow-up. | |
| Support only explicit absolute drive and extended UNC paths throughout the runner | Meets the selected scope while preserving literal names through a wide Windows argument and file-operation chain. Excludes other namespaces and avoids promising unprefixed long paths. | ✓ |
| Generalize all Windows paths, including unprefixed long-path policy and a new shared path framework | Could broaden Unicode and long-path support, but adds manifest/OS-policy interactions and maintenance beyond the requested contract. | |

**Authorization and adopted recommendation:** The owner authorized the whole v11.0 milestone in autonomous mode and asked that each phase discussion follow the detailed fan-out brief; for ordinary in-scope choices, adopt the synthesized recommendation without routine prompts.
**Notes:** Three specialists independently reviewed Microsoft path semantics, runner alias/write safety, and Windows CI acceptance. The runner currently uses narrow APIs and ACP conversion. Microsoft describes the extended prefix as disabling ordinary path parsing; the prefix must not be normalized away. Source links: [Microsoft naming and namespaces](https://learn.microsoft.com/en-us/windows/win32/fileio/naming-a-file), [Microsoft path-length rules](https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation), [Microsoft wide wmain](https://learn.microsoft.com/en-us/cpp/text/support-for-using-wmain?view=msvc-170), [Microsoft _wfopen](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/fopen-wfopen?view=msvc-170). Revisit if a complete file-operation audit cannot preserve namespace semantics without widening the public contract.

---

## Collision and safe replacement

| Option | Description | Selected |
|--------|-------------|----------|
| Keep existing per-output checks | Smallest change, but current checks differ by output type and frame PNG directly truncates before a successful write. | |
| Check collisions across input/output roles and use handle identity; publish all output types atomically | Preserves aliases while allowing true suffix-distinct names, and satisfies WINPATH-02/03 including frame-image failure behavior. Reuses current sibling-temp design. | ✓ |
| Add a general path/file subsystem with handle-relative operations | Could serve other future consumers or a broader race threat model, but expands interfaces and tests without a current second consumer. | |

**Authorization and adopted recommendation:** Use the specialist recommendation that meets the locked acceptance contract with existing project mechanisms.
**Notes:** File IDs support identity checks; network final-path lookup may fail due to SMB permissions, so required uncertainty must cause refusal. The current ordinary _stat64 identity path is not a sufficient proof on NTFS. PNG direct "wb" open is a reproduced source gap relative to WINPATH-03 and docs/runner.md's atomic publication promise. References: [GetFileInformationByHandle](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfileinformationbyhandle), [GetFinalPathNameByHandleW network limitations](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfinalpathnamebyhandlew), [MoveFileExW](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-movefileexw). Adversarial pass: false-positive normalization could reject distinct user files, while permissive fallback on an unresolved UNC alias could overwrite an input; preserve literal leaves and fail closed when safety identity is unavailable. Keep the identity implementation private and test hard links/junctions where the filesystem supports them.

---

## Windows CI and UNC acceptance

| Option | Description | Selected |
|--------|-------------|----------|
| Test only local extended drive paths | Easier but cannot establish the required extended UNC behavior in WINPATH-04. | |
| Use a temporary local SMB share within the existing Windows CI jobs | No external host or credentials; actual UNC path is exercised on the same VM. Windows hosted runners are documented as administrator-capable, and New-SmbShare exposes a local test folder. | ✓ |
| Depend on an external share or self-hosted Windows runner | Could model remote SMB, but introduces infrastructure, credentials, cost, and a new authority boundary. | |

**Authorization and adopted recommendation:** Use the narrow in-job share path, subject to actual create/access/cleanup evidence before accepting UNC coverage.
**Notes:** GitHub documents each hosted VM as fresh and its Windows VM as administrator-capable: [GitHub-hosted runners reference](https://docs.github.com/en/actions/reference/runners/github-hosted-runners). Microsoft documents local share creation via [New-SmbShare](https://learn.microsoft.com/en-us/powershell/module/smbshare/new-smbshare?view=windowsserver2025-ps). Whether share setup and loopback access work on both existing Windows architectures remains an empirical acceptance question. If it fails, do not silently replace UNC coverage with local drive tests or change CI infrastructure; preserve that gate and continue independent work until a concrete resolution is available.

---

## Documentation and dependency scope

| Option | Description | Selected |
|--------|-------------|----------|
| Document theoretical Windows maximum path length | Risks promising lengths and component rules that the tested filesystem cannot deliver. | |
| Document exactly the supported namespace forms and the longest representative path actually tested; use existing C/Win32 facilities | Matches observable product support and the dependency-minimization ladder. | ✓ |
| Add a path library and broaden support claims | Adds a package and a larger compatibility surface without evidence that current primitives are inadequate. | |

**Authorization and adopted recommendation:** Follow the autonomous recommendation; the owner specifically prefers a small dependency tree and permits a dependency only when its correctness/security value justifies the transitive cost.
**Notes:** The path-length evidence must include namespace prefix, battery-derived name and sibling temporary suffix where relevant. Per-component filesystem limits still apply. Add no dependency; keep tests in existing CMake/CTest/Windows CI. A cheap formatter/consistency preflight should precede the longer CI workflow, following recent lessons.

---

## Fan-out and adversarial synthesis

Three disjoint read-only specialist lenses were used:

- Windows namespace and Unicode/API semantics (Microsoft primary documentation).
- Runner path, collision, alias, and output-write safety (live code and tests plus Microsoft API documentation).
- Windows CI, real-process acceptance, and accessible UNC-share strategy (existing workflow and GitHub/Microsoft primary documentation).

The recommendations converge on the existing runner, existing Windows lanes, and existing sibling-temp pattern. The adversarial review considered partial Unicode conversion, stripping the prefix or suffix, false alias acceptance, false collision rejection, an untested in-process-only entrypoint, PNG truncation, temporary names exceeding component limits, and a UNC share whose final-path query fails. Acceptance therefore covers each file role through the real executable, all alias classes and output failures, measures the longest actual path, and treats share/identity failures as unresolved rather than passing by inference. Further research is unlikely to change scope unless a hosted Windows lane cannot establish local-share access or a complete code audit identifies an API that cannot preserve the selected namespace.

## the agent's Discretion

- Internal UTF-16 representation and small private helper boundaries.
- Exact supported test path length and filesystem-safe generated names.
- Choice of documented handle identity call and Windows fault-injection seam.
- Whether the existing path helper stays in runner/main.c or is extracted narrowly to remove real duplication.

## Deferred Ideas

- NT device and volume-GUID namespaces, relative extended paths, broad unprefixed long-path policy, and theoretical unlimited-length promises.
- External SMB infrastructure or self-hosted runners, only reconsidered if the ephemeral hosted-runner share cannot satisfy actual UNC acceptance.
- CI topology, cache, timeout, public API, emulator core, GUI, device, and dependency changes.
