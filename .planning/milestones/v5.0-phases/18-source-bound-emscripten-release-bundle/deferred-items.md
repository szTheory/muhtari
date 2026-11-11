# Phase 18 optional tooling seed

The README link checker recognizes the expected Markdown-link syntax and rejects the exercised missing, wrong-target, plain-text and backtick-fenced cases. It does not interpret inline-code spans or tilde-fenced blocks, so those wrappers can still contain a matching link token. The current README contains correct clickable download and guide links; no integrator instruction is missing or broken.

This is a limited tooling-coverage seed, not accepted browser/device evidence or an open package requirement. Keep the reviewer observation visible in `18-REVIEW.md` (WR-02). Revisit if the portable README section gains code examples or a real documentation regression demonstrates the need. Prefer a narrow check of that actual format over a new Markdown parser or dependency. The two code-review fix rounds remain used; original host/remote stops remain closed.
