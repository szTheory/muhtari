---
title: "Successor scope: delivery and evidence boundaries"
summary: "What shipping already proves, how to keep the next slice economical, and where new claims need new evidence."
read_when: "When selecting the successor milestone, planning its tune-up, or transferring sibling lessons."
updated: 2026-11-11
---

# Delivery and evidence boundaries

## Baseline, provenance and confidence

- **Verified receipt, high confidence:** v0.13.1 publication, downloaded-byte
  checks/attestations and latest installation finished successfully. Non-release
  fixture maintenance merged and current-main CI closed. Read
  [the final shipping receipt](../../milestones/v5.0-SHIP.md#final-publication-and-current-main-closure--reconciled-2026-11-11),
  including failed-source history; do not create another release-recovery task.
- **Inspected implementation, high confidence:** the required quality producer
  consumes existing six native, hygiene and sanitizer artifacts. The measured
  instruction-rate gate uses the test card, not a representative new sound chip
  or every game. [Quality guide](../../../docs/quality.md),
  [test inventory/commands](../../../docs/testing.md), `quality.json` and
  `.github/workflows/ci.yml` own the actual checks. A future new subsystem needs
  relevant workload measurement rather than a performance claim from an
  unchanged test-card result.
- **Owner direction:** use small permissive seeds with notices before adding a
  dependency; exact license scope and update duty still apply. Runtime C17,
  integer-only bounded work, instance-owned state and no post-load allocation
  remain required. [Provenance](../../../PROVENANCE.md),
  [asset policy](../../../ASSET_POLICY.md), [engineering](../../preparation/ENGINEERING.md)
  and [settled decisions](../../preparation/DECISIONS.md) are binding.
- **Evidence boundary:** no new execution evidence was created in this scoping
  exercise. Source inspection and retained receipts do not establish a new
  behavior pass. Future command suggestions below are acceptance design only.

## Recommendations

1. Retain one small recurring tune-up at milestone start. Reconcile release,
   documentation and existing measurements; repair a demonstrated defect at its
   owner. Do not select caching, sharding, retries, new dependencies or a broad
   timeout audit without measurements that change the recommendation. Windows
   ARM64 is the observed longest shipping/main job; no safe structural remedy
   follows from that observation alone.
2. Select one concrete output, its consumer and its failure boundary. An
   unmeasured timing fit is not a bug; untested games are not failures; a parsed
   sound flag is not emulated audio; a source comparison is not hardware truth.
   State the strongest claim the proposed oracle can support before implementation.
3. Use existing unit/diagnostic/public C, runner/movie and loaded-libretro
   consumers. Add recurring checks only where they protect new behavior. Keep
   expected numbers independent before digest equality. Test source, extracted
   package and installed consumer at the boundary actually changed.
4. Check affected configure-time inventories and genuine artifact-transfer
   layouts before spending a full hosted matrix. Use existing CMake/sh facilities;
   no new test framework or evidence service is justified.
5. For new hardware state, decide public API/behavior/native-format compatibility
   explicitly during phase discussion. Do not assume the current API 8/ABI 1/
   behavior 6/format 1 tuple admits extra state or that LibFuturo's exact-build
   save/snapshot envelopes migrate. Host revision, core media generation and
   machine snapshot are distinct contracts. Preserve old refusal/continuation
   semantics while defining any approved compatibility change.

## Acceptance design and resource bounds

`cmake --workflow --preset ci` remains the ordinary full native behavior gate.
Affected focused tests must use `--no-tests=error` and their own report paths.
The existing `sh scripts/emscripten.sh build/ci` route qualifies the libretro
Wasm artifact; LibFuturo qualifies its own C-adapter/browser path. Neither is a
substitute for the other's exact consumer/source gate.

Reuse the current CI topology and exact-source release path. A new subsystem's
per-call work, integer width/headroom, state extent and host-buffer requirements
must be justified under ENGINEERING §11. Do not invent runtime/LOC targets
upstream of measurement. Small scope and early narrow checks reduce duplicate
work without weakening fault coverage.

No Phase 12/14 denial, spent attempt, expired observation window or specific
no-retry condition is reopened. Standing local preauthorization does not override
those exact stops. A new remote, identity-bound or host action requires its own
applicable authority. No workload, sibling mutation, dispatch or publication is
authorized by this research document.

## Alternatives and revisit triggers

| Choice | Benefit | Cost / when to change |
|---|---|---|
| Leave delivery mechanisms unchanged; add only changed-behavior coverage | Preserves already qualified package/native/Wasm paths and shortens the next change's critical path | Revisit after comparable runs reproduce a specific preventable bottleneck or failure. |
| Consolidate all evidence/CI machinery before a product slice | Could reduce repeated bookkeeping | No demonstrated net benefit or multi-run basis; broadens ownership and can delay useful behavior. Defer. |
| Rebuild a host or shared ABI in Muhtari | More direct control of a whole session | Duplicates implemented LibFuturo routes and violates the existing three-deliverable scope. Reject. |

## Reusable lessons, not automatic imports

Read the locally reproduced entries in [LESSONS](../../LESSONS.md), particularly
shipping consumer coverage, clean package setup, downloaded-layout validation,
inventory registration and Windows deadline calibration. LibFuturo's PAL
report/input identity fix is a useful receiving-project hypothesis: hash the
actual submitted buffer, bind each route independently, and avoid assuming PAL
and NTSC inputs have equal extents. Do not mark that mechanism reproduced in
Muhtari until a concrete local fault/control demonstrates it here.
