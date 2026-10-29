# BankSet diagnostic

`diag.bankset` derives transient cases only from the SHA-256-checked owned
32 KiB-per-set template. Its source/config commit precedes its signed image;
`diag.rebuild` uses the same pinned tool recipe as the other diagnostics. The
canonical generated case must equal the admitted image byte for byte. Larger
cases preserve the upper CPU code at identical addresses, and change only
declared size/mapper/RAM/region fields, the four-byte selector and ROM bank tags.
Every variant is size/probe-checked. Altered template identity, header/body
mismatch, odd truncation and RAM on 48/52 KiB sets refuse at their actual consumers.

The executable matrix is 32/48/52/128/256/512 KiB per set without RAM, plus shared
16 KiB and HALT 32 KiB RAM for 32/128/256/512. RAM cases also declare write-only
POKEY at $4000, proving that its store does not consume the RAM write. Representative
32 KiB HALT and 512 KiB shared cases repeat in PAL. Each case makes 12 bounded
calls; the CPU measures and computes the CRC by call 4, then eight complete frames
are independently judged. The separate `diag.bankset_tracer` retains the narrow
canonical case through the same owned source and real hosts.

The CSV values follow the documented mapper and the owned literal tags. A switched
bank starts with $40 plus its index and ends with $80 plus its index, except the
last bank, whose fixed CPU code starts with $D0 and ends with the $C1 vector high
byte. For N banks the two sweep sums are 256/N times the sum of those N literals;
no production mapping helper supplies an expected offset or number. Linear starts
are $8000/$4000/$3000. Initial RAM is zero; measured stores yield $40/$80 in the
CPU window while $C000 stays $D0 after redirected stores.

DLL zone 0 has OFFSET 15. DMA line 32 reads OFFSET 0, and raster 33 plays it at
native row 17. Two objects read ROM $E000/$E001 ($C0/$C8 in the MARIA set versus
$40/$80 in the CPU set), or RAM $4000/$7FFF. Shared RAM yields palette codes
$16/$27; redirected HALT RAM yields $38 and the second object's $27 pair. The
library and loaded module judge these native samples separately. The actual runner
dumps call 5 through its normal PNG option, and the existing independent PNG reader
judges the same samples in both supported heights before hash comparison.

Every host's mailbox must pass count/order/identity/flags/reserved bytes and CRC
plus all 17 CSV values before native frame, canonical raw audio, PCM and sample-count
equality. Missing/truncated/extra reports, duplicate/reordered records, wrong identity,
CRC corruption and CRC-valid wrong values fail the actual judge. Substituting CPU
graphics into the MARIA set fails the native judge. Ordinary PNG malformed-input
controls remain, with actual NTSC/PAL height, checksum and CRC-valid nonzero-row-filter
faults also exercised here.
Exact HALT edges and four-pair arithmetic remain bus tests. Shared RAM and the larger
256/512 KiB geometries are selected producer/emulator extensions; this evidence
establishes model behavior, without a new physical-board capacity or timing fit.

## Ledger rows

The local `mailbox@12` row of `diag/bankset` records the BankSet32 no-RAM case that `diag.bankset` runs; the
test also covers all fourteen geometries. These are model-conformance observations, not gameplay results. The
7800basic `banksets` sample stays skipped in the ledger because its pinned recipe does not assemble; diagnostic
evidence for a board changes no other row's status or predicate.
