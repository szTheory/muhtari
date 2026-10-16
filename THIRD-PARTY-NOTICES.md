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
