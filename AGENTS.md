# muhtari

An Atari 7800 emulator core in C. This repository ships three things: a C library, a headless
command-line runner (`muhtari-run`), and a libretro core. RetroArch or another host provides the window.

Project state lives in `.planning/` (PROJECT.md, REQUIREMENTS.md, ROADMAP.md, STATE.md). Reference material
with sources is indexed in `.planning/preparation/README.md`; open the file it points to before designing or
building a subsystem. Settled choices are in `.planning/preparation/DECISIONS.md`; do not reopen one without new
evidence.

## Rules

1. **Reuse permissively, never GPL.** MIT, BSD, zlib and CC0 code may seed a module, with its notice kept as
   `PROVENANCE.md` describes. GPL, LGPL and unlicensed emulator source is never opened and no copy of it is kept
   in the workspace; reference emulators run only as released binaries.
2. **Three deliverables.** Library, runner, libretro core. No window, audio-device or input-device code.
3. **Clean tree.** No ROM, BIOS or save bytes except fixtures listed in `tests/roms/manifest.txt`. No personal
   paths, e-mail addresses or names. Commits use the GitHub noreply identity. `scripts/hygiene.sh` checks every
   commit and push (enable the hooks with `git config core.hooksPath .githooks`).
4. **Deterministic, bounded core.** Integer-only. All state lives in the instance. The same inputs give the same
   frame and audio hashes on every platform. Every call returns within a bounded amount of emulated time, even
   for a hostile or jammed ROM.
5. **Tested and documented together, within budgets.** Every behaviour change lands with a test that
   `cmake --workflow --preset ci` runs. The README stays user-facing and short; testing, CI and maintainer docs
   live in `docs/`; the public header stays terse and puts behaviour prose in `docs/`.
6. **Verification is automation-first; the owner verifies nothing by default.** This is settled; do not
   re-propose it. Deterministic checks go in tests; integration, end-to-end and smoke checks go in CI when they
   catch recurring regressions; a check with no recurring value is run once by the agent and recorded as a
   command and an exit status. The agent makes the judgement calls itself: it reads docs against the code, views
   images and screenshots, and runs RetroArch. What only a merge can show is confirmed at ship (rule 9), not in
   UAT. Ask the owner only for identity-bound consent (the App key, admin settings, a merge) or a run on their
   own hardware, and say why.
7. **Proportional gates.** Repository checks use CMake `-P` or POSIX sh and reuse existing mechanisms.
   A new or materially changed check names its concrete fault and exercises representative pass/fault behavior;
   consolidation preserves required behavior and fault detection. For architecture review and new resource
   bounds, follow `.planning/preparation/ENGINEERING.md` §11. At most two code-review fix rounds and one gap
   round per phase. Optional test/tooling improvements become seeds; violations of an existing required
   contract remain blocking wherever they live. Re-plan a gap only with new evidence.
   When workflow retries or metadata repairs repeat, follow
   `.planning/preparation/GSD-HANDOFF.md` §3.1 and its bounded fan-out prompt.
8. **One GSD step per session, then stop.** Report what finished and the exact next command. Never run
   `/gsd-next`, `/gsd-autonomous`, `/gsd-manager`, `/gsd-progress --next` or `--do`, any GSD command whose
   arguments contain `--auto` or `--chain`, or `/gsd-profile-user`, and never set the hygiene bypasses `MUHTARI_ALLOW_CHAIN` or
   `MUHTARI_ALLOW_MAIN_COMMIT`. Inside a step, proceed without routine
   confirmations.
9. **Branch, then pull request.** Never commit on `main` once a remote exists. Before the first GSD step that
   commits, branch off `origin/main`: `phase/NN-slug` for a phase, `milestone/vX.Y-slug` for milestone steps,
   `gsd/roadmap` for the kickoff. After `/gsd-ship`, give the pull request a Conventional Commit title, run
   `gh pr merge --auto --squash` in a shell (never inside GSD command text), and confirm a CI run exists on the
   head commit; after the merge, confirm the release pull request is green.
10. **Evidence is a command, an exit status and a run URL**, never a committed log.
11. **Lessons.** Inbound lessons in `.planning/LESSONS.md` are hypotheses until reproduced here. When the owner
    asks to "export lessons", produce a pasteable block from that file's template.

## GSD summaries

Every plan SUMMARY carries `requirements-completed:` and a block-style `coverage:` list. Each entry has `id`,
`description`, `requirement`, `verification` (kind one of unit, integration, e2e, automated_ui,
manual_procedural, other; `ref`; `status`) and `human_judgment` (true only for what rule 6 leaves to the
owner). No flow style `{}` and no inline `#` comments in that block.

## Style

Small modules, plain control flow, fixed-width integer types. Public names start with `muhtari_`; each module
prefixes its own names. A hardware comment says what the hardware does and cites its source (a document, or a
`.planning/preparation` section). No planning IDs in code, test names or user docs. To decide how to do
something, follow the ladder at ponytail.dev: remove the need, reuse what is here, use the standard library, use
the platform, and only then write the minimum new code. Another copy and paste beats another dependency: when
outside code is needed, a permissive seed with its notice is preferred to a new package, so the dependency tree
stays small and flat.
