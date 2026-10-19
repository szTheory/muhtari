# Third-party notices

muhtari is MIT-licensed ([LICENSE](LICENSE)). Some files are derived from other projects under
permissive licences. Each section below names those files, where they came from, what changed, and
the licence text the upstream requires to travel with source and binaries.

## a7800 palette

- **Applies to:** `host/palette.c`
- **Upstream:** https://github.com/7800-devtools/a7800, file `src/mame/drivers/a7800.cpp` at commit
  `7a2afdc1ea08fc331b16b750d8c1f02d4ef62fc8`. The file's own header licenses it BSD-3-Clause.
- **Upstream file SHA-256:** `504b221d6dd1726cabd600b795a0243a6353d4642827f7d1bc438ea1d8ce31c2`
- **Changes:** warm NTSC table only (the 16 `ntsc_hue*_warm` rows of the block "26.7 PHASE SHIFT -
  LUM3 from TREBORS PRO PALETTES V7"), as a C array of 0x00RRGGBB values.
- **Licence:** BSD-3-Clause

```
Copyright (c) Dan Boris, Fabio Priuli, Mike Saarna, Robert Tuccitto

Redistribution and use in source and binary forms, with or without modification, are permitted
provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions
   and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of
   conditions and the following disclaimer in the documentation and/or other materials provided
   with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to
   endorse or promote products derived from this software without specific prior written
   permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```

## libretro.h

- **Applies to:** `libretro/libretro.h`
- **Upstream:** https://github.com/libretro/RetroArch, file `libretro-common/include/libretro.h` at
  commit `69a4f0ea1e8aaf442ae4858f2e7f2b31a1776576` (v1.22.2). The file's own header licenses it MIT.
- **SHA-256:** `bd3398d29c3763d18617087020ff56b5450a48a63123403a4b674f8e79947acb`
- **Changes:** none. The file is vendored unmodified.
- **Licence:** MIT

```
Copyright (C) 2010-2024 The RetroArch team

Permission is hereby granted, free of charge,
to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```

## singlesteptests

- **Applies to:** `tests/vectors/6502-sample.n65v`, `tests/vectors/fixtures/a9.json`
- **Upstream:** https://github.com/SingleStepTests/65x02, directory `6502/v1` at commit
  `2f6980a2d95757486c7bee24355c360e40e2a224`. The repository's `LICENSE` licenses it MIT.
- **Changes:** the first 100 tests of each of the 256 files `6502/v1/00.json` to `ff.json`,
  converted to the N65V binary form (`tests/vectors/n65v.h`) and concatenated in opcode order. Test
  names are dropped; every state, RAM pair and bus cycle is kept.
  The fixtures in `tests/vectors/fixtures` are byte prefixes of upstream files, cut where its
  `README.md` says.
- **Licence:** MIT

```
MIT License

Copyright (c) 2024 Thomas Harte et al

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
