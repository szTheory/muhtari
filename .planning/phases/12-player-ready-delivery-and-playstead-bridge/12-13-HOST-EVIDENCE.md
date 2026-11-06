# Phase 12 Plan 12-13: Host Evidence Ledger

**Disposition: halted.** The frozen candidate archive loaded in pinned RetroArch and its DMA screenshot matched the runner reference. The required held/released input step failed at its acknowledgment deadline; no input images or battery results were produced, and CI required failed.

## Owner authorization and scope

On 2026-11-06 the owner wrote: “Okay, so I want you to remember all the takeaways from this. And then, yes, I authorize you to get us back on track. The pounded host evidence scope, anything you need to do to get this back on track, I authorize.”

Applied to the reviewed Plan 12-13 scope only: create refs/heads/phase/12-host-evidence-20261106-postformat at the frozen source, submit at most one ordinary full workflow_dispatch with every optional input false, and allow at most one ordinary cancellation of that exact run if the plan's stop condition is reached. No PR, merge, release, force push, retry, source repair, or Phase 14 host action was authorized by this plan.

## Frozen source and local qualification

SOURCE is a8ef07f07017668bb8ce5e205d0b5b7135cb6531; its Git tree is f0212da821284b1eb38ac08e3d6137c86f1a491c. The root checkout was clean on branch phase/12-player-ready-delivery-and-playstead-bridge, the Git identity used the repository noreply address, hooks were configured, and formatting commit cc3f18db1a3be429c9fb5abe53f15ffbb9dc531c is an ancestor of SOURCE. The source SHA and all raw evidence are under ignored build/phase12-gap13-evidence.

| Preflight | Command or comparison | Exit |
|---|---|---:|
| Exact formatter consumer | Formatter 21.1.8 and the plan's repository-wide tracked C/header clang-format dry-run with --Werror | 0 |
| Repository tree hygiene | sh scripts/hygiene.sh --tree | 0 |
| Offline RetroArch policy consumer | The plan's cmake -E chdir invocation, using absolute repository and ignored scratch paths | 0 |
| Protected workflow, pins, formatter and policy consumers unchanged since run 42227540379 source | git diff --exit-code from d22dca50abea335aefcb917cf428d32f5324f93c on the five planned protected paths | 0 |
| Ref syntax, push-trigger scope, and unchanged frozen checkout | git check-ref-format; inspect ci.yml push branches; git diff --exit-code SOURCE -- | 0 |

The formatter resolved to /opt/homebrew/opt/llvm@21/bin/clang-format, version 21.1.8. qualification.exit is 0. No CTest, full local workflow, title-pin test, process inspection, Docker, local RetroArch, or old-log sweep was run.

## Ref publication and run binding

The dedicated ref and a PR for it were absent before publication. GitHub authentication and the workflow file were available; branch pushes in ci.yml target main only.

The one publication command was:

    git push origin "${SOURCE}:refs/heads/phase/12-host-evidence-20261106-postformat"

The command returned exit 0 and GitHub reported the new branch at SOURCE. Git then reported that it could not update the local origin tracking ref because it could not create a lock file. A read-only git ls-remote check returned exit 0 and advertised the exact SOURCE at the remote ref. The push was not repeated.

The one submission was:

    gh workflow run ci.yml --repo szTheory/muhtari --ref phase/12-host-evidence-20261106-postformat -f keyboard_preflight_only=false -f keyboard_probe_only=false -f input_trace_diagnostic=false -f phase14_host=false

T0 was 2026-11-06T15:01:11Z. Submission exit was 0 and produced [run 42239124226](https://github.com/szTheory/muhtari/actions/runs/42239124226). The saved candidate list contained exactly one matching run. Both binding and final receipt assertions passed: source, ref, workflow_dispatch event, attempt 1, run ID, and creation time all matched. identity.exit is 0.

The terminal receipt is completed/failure. The exact-run audit command, cmake -DINPUT=build/phase12-gap13-evidence/run-terminal.json -DEXPECTED_SHA=<SOURCE> -P tools/check_ci_run.cmake, exited 1 because CI required did not succeed. The run had already completed by the next 60-second observation; no cancellation was issued.

## Job outcomes

| Job | Result | Notes |
|---|---|---|
| changes | success | Heavy workflow selected |
| hygiene | success | Repaired formatter gate passed |
| title | skipped | Expected for workflow_dispatch |
| library | success | |
| asan | success | |
| be32 | success | |
| nofp (ubuntu-26.04) | success | |
| nofp (ubuntu-26.04-arm) | success | |
| build (linux-arm64) | success | |
| build (linux-x64) | success | |
| build (macos-arm64) | success | Candidate package producer |
| build (macos-x64) | success | |
| build (windows-arm64) | success | |
| build (windows-x64) | success | |
| perf | success | |
| hash-equality | success | |
| quality | success | |
| wasm32 | failure | Advisory job, excluded from CI required. Test inventory omitted registered input.history_cart and policy.docker_contract.controls. |
| retroarch | failure | Required host input step failed; see predicates below. |
| CI required | failure | Its receipt identifies retroarch as the required failure; wasm32 is not in its dependency set. |

No job was cancelled. The run's full 20-job receipt, terminal JSON, full logs, and downloaded artifacts remain in ignored scratch. The first gh run view --log retrieval failed with exit 1 because the sandbox denied the default user cache path; setting XDG_CACHE_HOME to an ignored directory and repeating that read-only retrieval returned exit 0 and saved the 36,503-line log. This did not submit or rerun CI.

## Candidate package and host predicates

The hashes-macos-arm64 artifact's commit.txt matches SOURCE. The host log printed the same SHA-256 as the downloaded muhtari-libretro-macos-arm64.zip: 264f82a618c268c67b08d4766744815c8e6d77a8951e2f6dde9301666822b143. The archive contains cores/muhtari_libretro.dylib and info/muhtari_libretro.info; their extracted SHA-256 values are 4dc0a57112a180eb7b1280a83e969701c8089a249b6a140d49f9770b3998aede and 7d84d2f4269d8cf11e724932e68bb1901521987f938a740a1ad965cb6883fe50. The host log shows RetroArch loading the core from the directory extracted from this archive.

The pinned DMG step passed with the configured RetroArch 1.22.2 SHA-256 81b79121ba26d539064ae13b4d0419a120c3d165afbe656cf5f5412b15fdb434 and observed frontend version 1.22.2 (Git 69a4f0ea).

| Predicate | Result | Evidence |
|---|---|---|
| Frozen source, dedicated ref, unique attempt and ordinary mode | pass | Exact ref and attempt-1 receipt; all four optional inputs false |
| Required full CI aggregate | fail | Exact-run audit exit 1; CI required completed/failure |
| Candidate macOS ARM64 archive, core and info identity | pass | SOURCE-bound commit.txt, host-logged archive digest, extracted member paths and digests |
| Pinned RetroArch host launch | pass | Pinned DMG hash and observed version 1.22.2 |
| DMA screenshot and independent comparison | pass | The run reports 320x242, every pixel equal; both retained DMA images were inspected |
| Held and released input | fail / not demonstrated | The input wrapper stopped at acknowledgment-deadline after 5013 ms with capture=0 and child_status=unknown. RetroArch exited 1. No held/released screenshot or comparator verdict exists. |
| Battery fresh-process controls | missing | The workflow skipped the battery step after the input step failed |
| Input negative control | missing | The workflow skipped the dependent negative-control step |
| PLAY-03 candidate-host acceptance | not met | The required input images, battery predicates, and successful CI required aggregate are absent |

The RetroArch log also contains failed_to_start_audio_driver. The retained evidence does not establish whether that message caused the input acknowledgment failure. The historical input cause remains unknown; no causal claim is made.

## Runtime and efficiency review

The run was created at 15:01:12Z, its first job started at 15:01:14Z, and CI required completed at 15:37:20Z: about 36 minutes from T0. All 20 jobs reached a terminal result. The Windows ARM64 job was the critical path at 35m15s; RetroArch began about three seconds after it completed and ran for 34 seconds; CI required completed three seconds later. No work was cancelled. The statically reviewed allocation remains 329 configured runner-minutes, 324 eligible on this ordinary dispatch.

This single run supports no structural, timeout, cache, concurrency, or matrix change. None was made. The Windows baseline requirement remains separate; a passing Windows build is not a measured improvement.

## Scope disposition

The Plan 12-13 publication and one dispatch are spent; cancellation was unused. There was no second submission, source repair, PR, merge, release, pin change, or Phase 14 host action. The prior Plan 12-12 attempt remains separate and unchanged. Phase 12 acceptance stays 0/4, PLAY-03 remains open, Plan 12-01 remains halted, and Plans 12-02/03/04 remain blocked. This candidate result does not prove released-byte replay, paired Playstead behavior, TUNE-06, final TUNE-07 disposition, or Phase 14 acceptance.
