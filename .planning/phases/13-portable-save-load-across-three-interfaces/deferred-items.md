# Deferred Items

- The preexisting `.planning/WINDOWS.md` frontmatter counts do not match its rows (17 open and 35 fixed in frontmatter; 16 open and 36 fixed in rows). The best-effort append for the unavailable full CI verification was rejected with this count mismatch, and no ledger change was made. Reconcile the existing counts before a later append or ship audit.
- `muhtari-test-rcheevos` could not be built from its actual target because the pinned `build/pins/rcheevos` source is absent. Its archive fallback is wired consistently with the compiled libretro testdb targets, but this consumer's build remains unverified until the pin is present; no pin was fetched.
- Task 3's `tdd="true"` cases and implementation were included in one task commit, so there is no separate RED commit/evidence. The scoped identity assertions pass, but the planned test-first commit cadence was not followed.
