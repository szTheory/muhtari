# Phase 29 Windows path acceptance

This matrix records evidence by execution environment. A test implementation or a passing portable test
does not accept Windows behavior. Windows rows remain pending until the actual runner, SMB share lifecycle
and required cases pass on that architecture.

| Requirement / case group | Local | Windows x64 | Windows ARM64 |
|---|---|---|---|
| WINPATH-01: cartridge/probe, movie, BIOS, HSC firmware, state input, save directory, state output, frame PNG and trace through drive and extended UNC paths; non-ACP Unicode, literal dot/space suffixes and a path beyond `MAX_PATH` | Pending local focused and configured CI commands | Pending source-qualified `runner.trace.paths` job and run URL | Pending source-qualified `runner.trace.paths` job and run URL |
| WINPATH-02: suffix-distinct destinations, existing-file and hard-link identity, parent-junction and mixed ordinary/extended aliases, input/output and output/output overlaps | Pending local focused and configured CI commands | Pending actual-process alias matrix and run URL | Pending actual-process alias matrix and run URL |
| WINPATH-03: state, PNG, trace and derived battery success bytes; blocked create/write/replace preserves previous bytes and removes owned temporary files | Pending local focused and configured CI commands | Pending actual-process output matrix and run URL | Pending actual-process output matrix and run URL |
| WINPATH-04: ordinary-path controls, real temporary SMB share create/read/write/remove, filesystem/component measurements, full/derived/temp path lengths, and user documentation | Pending local focused and configured CI commands | Pending SMB lifecycle, measurements and run URL | Pending SMB lifecycle, measurements and run URL |

## Evidence record

- Local product source, command, exit status and run artifact: pending the final local gate.
- Windows x64 built source, exact test command, exit status, measurements and job/run URL: pending.
- Windows ARM64 built source, exact test command, exit status, measurements and job/run URL: pending.
- The Windows driver reports UTF-16 code-unit counts, filesystem and reported component limit, the full test-directory and longest final-output lengths, and conservative maximum sibling-temp candidate lengths. Its success line is surfaced by `runner.trace.paths`; the PowerShell wrapper reports the completed SMB create/read/write/remove lifecycle only after share cleanup succeeds.
- The Windows test must also prove no test-owned files or share remain. A missing SMB capability, self-skip, incomplete job inventory, failed cleanup or source mismatch leaves the affected requirement pending.
- CI queue, job duration, critical path and cancelled work: pending the existing current-milestone PR runs. No topology, cache or deadline change is proposed by this plan.

No requirement is accepted by this initial record.
