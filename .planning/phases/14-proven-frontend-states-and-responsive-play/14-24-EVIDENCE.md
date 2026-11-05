# Phase 14 Plan 24: Runtime Evidence

**Outcome:** blocked before qualification by a malformed local-input digest embedded in the generated controller. The fresh qualification command was not invoked; no active claim, Docker contact, container, build, CTest case, or protected workload start occurred. This scope is ineligible and will not be retried.

## Scope and bounds

- Run URL: `N/A-local`.
- Frozen source expected: `3e31b9be45e287f58d3e02b8d2b3ab73120cc704`.
- Image identity expected for this diagnostic: ID `sha256:10ca2cfc3a29b70e13fe0a2a9244fe7e5d24fbd7350ac4205028335c9541f926`; digest `catthehacker/ubuntu@sha256:c58e2b364da03b0c804c7d660f2ecbedf2f221a382b9baa0b344b0144780ff43`; source label `https://github.com/catthehacker/catthehacker/docker_images`; revision label `e2f8efe464c82732f78e967ee709c00b6af53643`.
- The image, endpoint, daemon/client versions, VM resources, HostConfig, cgroups, namespaces, and effective container observations were not queried because the exact local-input gate failed first.
- Declared limits remained 300 seconds for preparation/configure/targeted build, 90 seconds per CTest side, 10 seconds for cleanup, and 490 seconds total. The qualification clock was never started.
- The expected workload closure is the fixed 15-case set; neither qualification build nor any fixture/target was run.

## Commands and outcomes

| Command | Exit | Result |
| --- | ---: | --- |
| `sh -n build/phase14-gap24-evidence/container-driver.sh && sh -n build/phase14-gap24-evidence/docker-diagnostic.sh && python3 -c 'import pathlib; p=pathlib.Path("build/phase14-gap24-evidence/pair-runner.py"); compile(p.read_text(),str(p),"exec")' && GAP24_CONTROL_ROOT="$PWD/build/phase14-gap24-evidence/controls-v8" sh build/phase14-gap24-evidence/docker-diagnostic.sh controls` | 0 | Fresh offline controls passed, including JSON admission/fault cases, saved-receipt validation, exact-owned cleanup behavior and production supervisor reaping. The control reported that Docker, build, and CTest workloads were not started. |
| `python3 -c "import hashlib; l=next(x for x in open('build/phase14-gap24-evidence/container-driver.sh') if 'build/pins/mame/a7800.xml' in x); e=l.split(':\"',1)[1].split('\"',1)[0]; a=hashlib.sha256(open('build/pins/mame/a7800.xml','rb').read()).hexdigest(); p=next(x.split()[-1] for x in open('pins.txt') if x.startswith('mame-a7800-xml ')); print(f'controller expected length={len(e)}; local SHA-256={a}; pins.txt SHA-256={p}'); raise SystemExit(0 if len(e)==64 and e==a==p else 1)"` | 1 | The controller expects a 62-character value for `build/pins/mame/a7800.xml`; `pins.txt` and the local file agree on SHA-256 `5343714eedd715f7f1f080f1e283d5ed56e6f6509ea3eff2f8f7c8c53ee066b9`. This prevents the exact input contract from being met. |
| `cmake -DROOT=. -DWORK=build/phase14-gap24-controls-v6 -P tests/policy/docker_contract.cmake` | 0 | Focused offline production-consumer and preservation-checker controls passed after the literal traversal-pattern correction. No daemon was contacted. |
| `cmake -DROOT=. -DMODE=check -DMANIFEST=build/phase14-gap24-evidence/preservation.json -DMANIFEST_SHA=7245b0986ed19b7240329a26a15b6472ef2ea72385f34eb50e9b887858f6a9d6 -P tests/cmake/check_diagnostic_preservation.cmake` | 0 | The saved manifest, historical roots, and exact six-path task-stage set matched. |
| `sh build/phase14-gap24-evidence/docker-diagnostic.sh qualify` | N/A | Invocation count: 0. Not run because the exact local-input gate failed; no qualification claim was created. |
| Real Docker daemon/client-server queries, image inspect, container creation, project build/CTest, cleanup | 0 | Not contacted or started. Offline fake-transport controls only. |

The passing v8 suite used fresh ignored `controls-v8` and `build/phase14-gap24-controls-v5` scratch roots. Failed `controls-v7` and earlier `controls-v4` artifacts remain preserved. The checker correction changed only its literal `..` path-segment match; its focused policy controls and the final preservation command passed. The v3 controller-hash receipt remains untouched and describes the controller set used by the v8 run before that closeout-only correction; it was not used to authorize qualification. The saved historical preservation manifest is `build/phase14-gap24-evidence/preservation.json`, SHA-256 `7245b0986ed19b7240329a26a15b6472ef2ea72385f34eb50e9b887858f6a9d6`; all five historical evidence roots remain unchanged.

## Accounting and cleanup

| Item | Reserved | Started | Unknown |
| --- | ---: | ---: | ---: |
| Qualification | 0 | 0 | 0 |
| Default side | 0 | 0 | 0 |
| Serial side | 0 | 0 | 0 |

The Plan 14-24 active namespace and claim are absent. No container ID or owner label was issued, so cleanup was not applicable. The qualification is `N/A` and the conditional pair is ineligible. No retry, alternate image/input, rcheevos acquisition, Wasmtime fetch, or old Plan 14-23 command was used.

## Preserved acceptance status

`requirements-completed: []`. FRONT-02, FRONT-03, and FRONT-04 remain pending. This local offline control result does not provide actual pinned-host evidence, resolve the parent CI timeout, establish native macOS behavior, or qualify RetroArch behavior. Phase 14 remains blocked by its existing acceptance requirements.
