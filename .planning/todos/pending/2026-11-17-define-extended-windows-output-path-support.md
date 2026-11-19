---
created: 2026-11-17T00:07:48.000Z
title: Define extended Windows output path support
area: runner
severity: minor
resolves_phase: 29
files:
  - runner/main.c
  - tests/runner/test_save.c
  - tests/runner/trace.cmake
source:
  phase: 27
  review: 27-REVIEW.md
  finding: "WR-01: Extended Windows paths are compared with ordinary path semantics"
confidence: high
audit_acknowledged:
  milestone: v10.0
  at: 2026-11-17
---

## Problem

The Windows trace-output collision check removes trailing ASCII dots and spaces to match ordinary Win32 path handling. An explicit `\\?\` extended namespace path preserves those suffixes, so the comparator can reject two distinct output destinations as a conflict. The failure is conservative: the runner refuses the pair before writing either output. This path form is not documented as part of the current runner contract, and Windows execution was unavailable during Phase 27 review.

Microsoft documents that `\\?\` disables ordinary path-string parsing and enables names that ordinary Win32 paths cannot address: [Naming Files, Paths, and Namespaces](https://learn.microsoft.com/en-us/windows/win32/fileio/naming-a-file). The final Phase 27 review found no overwrite case, only this false-positive rejection.

## Follow-up

Before claiming extended namespace support, define whether the runner supports explicit `\\?\` and `\\?\UNC\` inputs. If supported, preserve the suffix under that namespace and add Windows runner controls that accept distinct destinations while still rejecting ordinary-path aliases. If unsupported, reject the namespace with a clear diagnostic. Verify the chosen contract in Windows CI.

## Revisit trigger

Revisit when Windows output path semantics are next changed, or before documenting extended namespace paths as supported. The local review could not test this path form; the finding is based on the Windows API namespace rules and source inspection.
