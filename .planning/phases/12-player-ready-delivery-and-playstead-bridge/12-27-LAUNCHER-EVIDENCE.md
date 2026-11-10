# Plan 12-27 Launcher Evidence

**Status: prepared and locally qualified; owner request and diagnostic choices are pending.**

## Frozen identity

| Item | Value |
|---|---|
| Bundle identifier | `org.muhtari.inputdiagnostic.gap27` |
| App bundle | `build/phase12-gap27-evidence/MuhtariInputDiagnostic-gap27.app` |
| Executable | `build/phase12-gap27-evidence/MuhtariInputDiagnostic-gap27.app/Contents/MacOS/MuhtariInputDiagnostic` |
| Executable SHA-256 | `9d8d0b6b404a8ca385c410e7b769c4e9faeabedd33c4a49433d8bd17a6f86aa1` |
| Info.plist SHA-256 | `7949c9c389ab48cb2b960d741db6a60a7cc3ac9a80284526d1b0d480da1a54f4` |
| Signature | Ad-hoc signature verified; CodeDirectory `a69922366f635b9c63afd101cead6428d1d70ccc` |
| Runner SHA-256 | `bfea90231e61394c7b947ce48c037104a7da719569de488ef082e968568d8c35` |
| Owner command SHA-256 | `6edeb75fac677625c24eb0c65dd03c74020975671b81b3b77550e0e659ce41f8` |

The app does not request permission on startup. Its request entry point and the existing keyboard-helper modes are separate. The app identity does not establish TCC attribution.

## Offline preparation results

`python3 build/phase12-gap27-evidence/runner.py prepare-checks` passed twice. Each invocation compiled a fresh mock into a different unique path. In both runs the Mach-O undefined-symbol parser controls ran first; the actual newly compiled mock then passed its `nm -u` gate before any mock process ran. The latest mock SHA-256 is `d78f30dd4ffce56aaa5cca2c187183f46cec713c2d276f3526211cc025272fc6`.

| Control | Result |
|---|---|
| `approval-and-receipt-bypass-rejected` | PASS |
| `changed-identity-and-command` | PASS |
| `delayed-unknown-no-retry` | PASS |
| `denied-request-no-content` | PASS |
| `dispatcher-zero-missing-receipt` | PASS |
| `duplicate-reservation` | PASS |
| `fake-open-only` | PASS |
| `invalid-mode-no-api` | PASS |
| `missing-and-declined-approval` | PASS |
| `mock-api-separation` | PASS |
| `stale-and-wrong-receipt` | PASS |

Parser fault controls use Mach-O-style undefined-symbol lines and the same parser as the real binary gate:

| Symbol input | Result |
|---|---|
| `_printf` normalized to `printf` | PASS |
| `_CGRequestPostEventAccess` rejected | PASS |
| `_AXIsProcessTrusted` rejected | PASS |
| `_CFRelease` rejected | PASS |
| Fresh mock binary undefined-symbol gate | PASS; no `CG*`, `AX*`, or `CFRelease` symbols |

The mock gate receipt was checked before each of the 10 mock executable invocations. The 11 offline control groups passed. Counts remain:

```yaml
permission_request: 0
standalone_preflight: 0
helper_pre_fork_check: 0
probe_reservations: 0
real_launchservices: 0
app_launches: 0
retroarch_launches: 0
content_launches: 0
```

No real permission API, app, actual-context preflight, RetroArch, or content was run.

## Owner-controlled request

The fully expanded command is:

```sh
python3 build/phase12-gap27-evidence/runner.py owner-run --expect-sha256 bfea90231e61394c7b947ce48c037104a7da719569de488ef082e968568d8c35 --bundle build/phase12-gap27-evidence/MuhtariInputDiagnostic-gap27.app
```

The runner checks the literal script hash and exact bundle path before dispatch. The command first asks whether to issue one event-post permission request for this exact app, with a 60-second bound. If the request succeeds, it performs one actual-context, nonprompting preflight with a 15-second bound. Only if that preflight passes does the same command ask a separate yes/no question for one isolated diagnostic. The owner may decline either choice. A denial, timeout, dismissal, unknown result, mismatch, or decline stops the flow without a retry. The active Terminal remains open; no restart is needed.

Bounds are unchanged: one request up to 60 seconds; actual-context preflight up to 15 seconds; the helper's existing internal pre-fork recheck; then at most one diagnostic through `tests/retroarch/run_retroarch.cmake`, capped at 600 frames, 5-second input acknowledgment, 45-second helper, and 50-second wrapper.

The optional diagnostic reuses `tests/retroarch/keyboard_preflight.c`, the private owned cartridge generated from `tests/input/test_input.c`, pinned cached RetroArch 1.22.2, the cached input-trace core and parser, and the existing trace and screenshot outputs. Existing independent pixel consumers include `tests/input/history_pixels.c`, `tests/input/pixels.c`, and `tests/retroarch/compare.c`; the agent reviews any saved screenshot and trace. No owner-run result exists yet.

```yaml
permission request: `pending`
actual-context preflight: `pending`
diagnostic choice: `pending`
diagnostic result: `pending`
```

This preparation and any later local diagnostic can show only current local app/helper behavior. It cannot establish the historical hosted cause, released or published-core acceptance, paired Playstead acceptance, or Windows improvement. The required Windows publication comparison remains with its existing owner plan (12-02); no Mac result is reported and the Windows criterion is unchanged. Phase 12 acceptance and PLAY-03 remain pending.
