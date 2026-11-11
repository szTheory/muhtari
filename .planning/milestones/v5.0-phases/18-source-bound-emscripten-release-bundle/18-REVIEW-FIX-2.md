# Phase 18 review fix round 2

The README install checker now requires the exact portable archive and guide as Markdown links in the portable section. It removes fenced examples from that section before checking, so plain text and displayed code cannot pass. The native install command and extraction assertions are unchanged.

Focused check command (valid README, exit 0):

```sh
cmake -DREADME=README.md -DBASE_URL=file:///tmp/ -DPORTABLE_LINK_CHECK_ONLY=ON -P tests/cmake/install_line.cmake
```

Isolated README copies under ignored `build/phase18-install-links/` produced these expected results through the same checker:

| Case | Exit | Expected diagnostic |
|---|---:|---|
| Missing archive link | 1 | `missing the clickable portable archive link` |
| URL shown as plain text | 1 | `portable archive URL must be a clickable Markdown link` |
| Wrong ZIP target | 1 | `portable archive link must target muhtari-libretro-emscripten.zip` |
| Guide path shown as plain text | 1 | `integrator guide path must be a clickable Markdown link` |
| Both links only in a fenced example | 1 | `missing the clickable portable archive link` |

The end-to-end native install-line execution remains in the normal macOS CI path and is part of the pending final local native gate.
