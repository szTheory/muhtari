# PAL diagnostic tests

`diag.pal` executes the owned `pal.a78` (mailbox identity19). Its eight
records measure initial blank, visible start, raster307 visible, raster308
blank, raster309 blank, frame-wrap blank, progress600 and final visible.
`expected.csv` labels documented raster facts `guide` and the selected
fast-start/progress convention `fit`; these are emulator-policy evidence,
not hardware calibration. Existing `core.pal.*` tests remain authority for
single-tick MSTAT/DMA boundaries and exact boot snapshots.

The first call starts at fitted tick140016 and ends at281976:141960 ticks,
960 samples and remainder336960. Exactly600 further intervals each span
142102 ticks. Their carried samples are281×961+319×962=576919; combined
601-call output is577879 samples with remainder47029 at tick85543176.
The separate zero-carry arithmetic result is576918 samples/remainder301218;
it is not the actual fast-start sequence.600 full frames exceed two
228-frame TIA/frame-phase recurrences. `MUHTARI_SHORT_RUNS` never reduces
this acceptance workload.

The fixture initializes both TIA channels once to AUDC4/AUDF0/volume15.
After startup the actual ordered source0/1 transitions alternate0/15 at
absolute tick modulo456 equal64 or288, with224/232-tick intervals.
POKEY@$450 uses AUDF100, AUDC$AF and the28-phi2 prescaler. Frame snapshots
check its real CPU-clock advancement; once the completed ROM enters its
fast JMP loop, at least two actual source2 intervals must be11312 ticks
(28×101×4), with alternating0/15. Every full frame must have nonzero PCM.
DMA on raster308 and its absence on309 are observed. VBE16 loads the DLL,
DMA17 first appears on raster18: native rows2..291 contain independently
specified$2F endpoints at x0 and319, including the last PAL row.

The library streams one93440-byte native frame and962 PCM samples. The
actual601-call runner must match every native frame digest and the complete
raw/PCM streams. Loaded test-only modules in both pixel formats compare
all93440 native bytes and each exact stereo sample every call; public
pixels are checked against the PAL palette and independent RGB565 channel
truncation. WASM runs the complete library/runner work without modules.
CRC-valid wrong numbers, wrong protocol/count/CRC, absent/truncated/trailing
reports, wrong sample/call counts and an actual599-full-frame session must
fail the same numeric judge before equality evidence is accepted.

The existing equality writer appends614 `pal.*` rows after its342 unchanged
NTSC rows:601 native frames, raw/PCM digests, audio metadata, mailbox and
five interval counters. It invokes the same numeric judge first; missing,
malformed, shortened and mutated reports also pass through that writer in
fault controls. The existing real producer/join admits the exact owned PAL
manifest row, retains it when present in a later base, and rejects changed
source/license/digest identities and forged evidence. Rebuild and admission
precede hash acceptance. The provisional diagnostic TIMEOUT36/COST18 uses
complete two-format/runner/control execution:17.69s on Windows arm64,
doubled and rounded up for the deadline. This exceeds the normalized ASan
maximum34.29s/scale2 from an independently rebuilt same-workload base.
The expanded equality writer similarly uses TIMEOUT29 from28.91s at scale2.
Existing lane scales and complete workloads remain unchanged. These are
uncensored workload measurements; timeouts establish no duration sample or
acceptance. Hosted portability acceptance is recorded separately when the
complete exact-source lanes actually pass.


