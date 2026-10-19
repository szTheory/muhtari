---
created: 2026-10-16T18:30:00.000Z
title: Add the release firmware scan with the first ROM code
area: release
severity: minor
files:
  - ASSET_POLICY.md
  - .github/workflows/release.yml
  - scripts/publish.sh
---

## Problem

`release.firmware_scan` from ASSET_POLICY §5 hashes every member of every release zip and every tracked binary and
fails on a known BIOS or ROM digest. It is not built in phase 1: no ROM-handling code or fixture exists yet, so a
scan would have nothing to find, and `scripts/hygiene.sh` already rejects unlisted ROM, BIOS and save files and any
file with the A78 magic. ASSET_POLICY.md keeps its "(untested)" marker until the scan exists.

## Solution

In the phase that first adds ROM-handling code or a ROM fixture, add `release.firmware_scan` as ASSET_POLICY §5
describes, with its negative control, a synthetic A78 file whose digest the scan must report, and run it on the
release zips before `publish.sh` uploads them. Then drop "(untested)" from ASSET_POLICY.md.
