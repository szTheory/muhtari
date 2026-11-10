---
phase: 12-player-ready-delivery-and-playstead-bridge
plan: "18"
subsystem: testing
tags:
  - retroarch
  - keyboard-input
  - event-post
requires: []
provides:
  - "An exact-identity consent request and the one permitted follow-up preflight are recorded; access denial stops before RetroArch."
affects:
  - PLAY-03
  - phase-12-host-acceptance
actuals:
  tasks: 3
  commits: 3
commits: 3
tech-stack:
  added: []
  patterns:
    - "Reserve identity-bound permission operations before single invocation; never retry an ambiguous or denied result."
key-files:
  created:
    - .planning/phases/12-player-ready-delivery-and-playstead-bridge/12-18-CONSENT-EVIDENCE.md
    - .planning/phases/12-player-ready-delivery-and-playstead-bridge/12-18-SUMMARY.md
  modified:
    - .planning/STATE.md
    - .planning/ROADMAP.md
key-decisions:
  - "Honor the explicit owner approval for one request and one local diagnostic, bound to the prepared helper and Terminal identities."
  - "Stop after the sole request and sole postflight both return denied; do not reserve or launch RetroArch and do not retry."
requirements-completed: []
coverage:
  - id: exact-owner-decision
    description: "Owner authorization is bound to the exact helper, Terminal session, host, core, cartridge, and one-run scope."
    requirement: PLAY-03
    verification:
      - kind: other
        ref: "build/phase12-gap18-evidence/owner-decision.json; explicit approval recorded before the request"
        status: pass
    human_judgment: true
  - id: event-post-request
    description: "The sole authorized helper request completes once and records its actual outcome."
    requirement: PLAY-03
    verification:
      - kind: other
        ref: "python3 build/phase12-gap18-evidence/consent.py request-approved; helper CGRequestPostEventAccess returned denied, exit 77; reservation count 1"
        status: fail
    human_judgment: false
  - id: nonprompting-postflight
    description: "The same helper performs one separate nonprompting preflight after a normal request completion."
    requirement: PLAY-03
    verification:
      - kind: other
        ref: "python3 build/phase12-gap18-evidence/consent.py postflight; CGPreflightPostEventAccess returned denied, exit 77; driver stopped before probe"
        status: fail
    human_judgment: false
  - id: causal-input-diagnostic
    description: "One RetroArch diagnostic supplies a supported causal finding for the earlier hosted input failure."
    requirement: PLAY-03
    verification:
      - kind: other
        ref: "Not run: denied postflight prevented diagnostic reservation; content children 0; cause remains unknown"
        status: unknown
    human_judgment: false
metrics:
  completed: 2026-11-07
  status: halted
duration: "not measured"
completed: 2026-11-07
status: halted
---

# Phase 12 Plan 12-18: Consent and Diagnostic Summary

**The exact request and one follow-up preflight both returned denied, so the diagnostic stopped before RetroArch.**

## Outcome

The owner explicitly authorized one access request and one bounded local RetroArch diagnostic for the prepared helper and Apple Terminal identities. The helper's `CGRequestPostEventAccess()` call completed normally and returned denied (exit 77). The same helper's sole subsequent `CGPreflightPostEventAccess()` also returned denied (exit 77). The request reservation and preflight reservation were each created once before their operation.

The preflight denial stopped the conditional diagnostic before its invocation reservation. No helper-owned pre-fork check, RetroArch launch, content child, screenshot, or trace occurred. Whether macOS displayed a prompt or changed persistent permission state is unknown. No settings automation, TCC reset, alternate identity, or retry occurred. The detailed identity and command receipts are in [12-18-CONSENT-EVIDENCE.md](12-18-CONSENT-EVIDENCE.md).

## Task Results

| Task | Result |
|---|---|
| 1. Prepare exact consent route and isolated controls | Passed; real-route helper, mock controls, cached host/core/parser and owned cartridge were qualified without real API calls or RetroArch launch. |
| 2. Owner decision | Explicit approval recorded for the exact request and one local hardware diagnostic. |
| 3. Run approved diagnostic | Halted before content: request exit 77 (`denied`), follow-up preflight exit 77 (`denied`), so no diagnostic invocation was reserved. |

## Task Commits

1. **Prepare the exact consent route and controls** — `4f8f75b`.
2. **Record the owner decision checkpoint in project state** — `b41712d`.
3. **Record the authorized request, denied preflight, and halted outcome** — `62f45af`.

## Recorded Results

- `python3 build/phase12-gap18-evidence/consent.py prepare-checks` — exit 0; isolated mocked request/preflight and approval-boundary controls passed.
- `python3 build/phase12-gap18-evidence/consent.py check-decision` — exit 0; exact affirmative decision receipt matched the helper and Terminal identities.
- `python3 build/phase12-gap18-evidence/consent.py request-approved` — driver exit 0; one helper request returned denied, exit 77.
- `python3 build/phase12-gap18-evidence/consent.py postflight` — driver exit 1 after the one helper preflight returned denied, exit 77; no probe followed.
- `python3 build/phase12-gap18-evidence/consent.py audit-receipts` — exit 0; request reservations 1, standalone preflight reservations 1, diagnostic invocation reservations 0, content children 0.
- No diagnostic wrapper command, helper pre-fork recheck, source repair, full CI, host dispatch, or new content launch occurred.

## Acceptance Effect and Next Action

Plan 12-18 is halted without a causal finding. Plan 12-19 remains blocked because its conditional repair requires a supported in-scope finding; Plan 12-16 remains gated on Plans 12-17/18/19. Phase 12 remains accepted 0/4; Plan 12-01 remains halted and Plans 12-02/03/04 remain blocked. Plan 12-17's WASI `tmpfile()` halt and the separate Phase 14 stop remain unchanged. This plan's sole request allowance is spent; no repeat request is authorized.

## Self-Check: PASSED

- The evidence ledger binds the decision and both operation receipts to the exact helper, session, host, core, and cartridge identities.
- The operation counts distinguish the request, standalone preflight, helper recheck, diagnostic reservation, and content child.
- The summary includes the required coverage entries and leaves `requirements-completed` empty.
