; SPDX-License-Identifier: MIT
; The read-mode diagnostic: five zones drawn in 160B, 320B, 320D, kangaroo
; mode and 2-byte indirect characters, each entered by a DLI that writes its
; CTRL. It records the byte at $4000 and the DLIs counted in one frame, then
; keeps drawing. See docs/hardware/maria.md, The modes ROM, which derives
; every modes row of tests/diag/expected.csv. Test n (from 1) is the n-th
; modes row. A plain SuperGame board of 4 banks; the code runs from the last
; bank, fixed at $C000.

        processor 6502
BANKS   = 4

; Banks 0 to BANKS - 2: their number, then zeros.
BANK    SET 0
        REPEAT BANKS - 1
        ORG BANK * $4000
        RORG $8000
        .byte BANK
        ds $3FFF, 0
BANK    SET BANK + 1
        REPEND

        ORG (BANKS - 1) * $4000
        RORG $C000
        .byte BANKS - 1         ; the last bank's number
        include "lib.s"

ROM_ID  = 7
; Zero-page byte after lib.s's: the DLIs taken since VBLANK.
COUNT   = $85
; The graphics: 16 pages, one per line of a 16-line zone, and the character
; base of the indirect zone.
GFX     = $D000
; A 5-byte header's second byte: write mode 1 or 0, direct or indirect.
WM1     = $C0
WM0     = $40
WM0_IND = $60
; Palette p and width 16 (W = 32 - 16), a 5-byte header's fourth byte.
        MAC PW16
        .byte ({1} << 5) | $10
        ENDM

reset SUBROUTINE
        sei
        cld
        lda #$07                ; INPTCTRL: lock, MARIA on, BIOS off
        sta INPTCTRL
        lda #CTRL_OFF
        sta CTRL
        ldx #$FF
        txs
        lda #ROM_ID
        jsr mb_init
        lda $4000               ; at_4000
        ldx #0
        ldy #1
        jsr mb_record
        ldx #31                 ; colours 1-3 of palettes 0-7
.pal    txa
        and #3
        beq .skip
        lda colours,x
        sta BACKGRND,x
.skip   dex
        bpl .pal
        lda #$00
        sta BACKGRND
        lda #>GFX
        sta CHARBASE
        lda #>modes_dll
        sta DPPH
        lda #<modes_dll
        sta DPPL
        jsr wait_vblank
        jsr frame_start
        jsr wait_visible
        jsr wait_vblank
        lda COUNT               ; zones
        ldx #0
        ldy #2
        jsr mb_record
        jsr mb_finish
.draw   jsr wait_vblank
        jsr frame_start
        jsr wait_visible
        jmp .draw

; frame_start: in VBLANK, no DLIs counted yet and CTRL 160A with DMA on.
frame_start SUBROUTINE
        lda #0
        sta COUNT
        lda #CTRL_ON
        sta CTRL
        rts

; The DLI: counts it and writes the next drawn zone's CTRL.
diag_nmi SUBROUTINE
        pha
        txa
        pha
        inc COUNT
        ldx COUNT
        cpx #6
        bcs .done
        lda zone_ctrl,x
        sta CTRL
.done   pla
        tax
        pla
        rti

; CTRL for drawn zones 1-5: 160B and 320B come from write mode 1 headers.
zone_ctrl
        .byte CTRL_ON           ; unused
        .byte CTRL_ON           ; 160B: read mode 0
        .byte CTRL_ON | $02     ; 320B: read mode 2
        .byte CTRL_ON | $02     ; 320D: read mode 2
        .byte CTRL_ON | $04     ; kangaroo mode, read mode 0
        .byte CTRL_ON_CW2       ; 2-byte characters, read mode 0

; Colour c of palette p at index 4p + c: hue 2p + 1, luminance 4c + 2.
colours
COL     SET 0
        REPEAT 32
        .byte ((COL >> 2) * $20 + $10) | ((COL & 3) * 4 + 2)
COL     SET COL + 1
        REPEND

; The drawn zones' DLs: 5-byte headers (low, flags, high, palette and
; width, hpos), then a terminator.
dl_160b .byte <GFX, WM1, >GFX
        PW16 1
        .byte 8
        .byte <(GFX + $40), WM1, >GFX
        PW16 2
        .byte 72
        .byte $00, $00, $00, $00
dl_320b .byte <GFX, WM1, >GFX
        PW16 1
        .byte 8
        .byte <(GFX + $40), WM1, >GFX
        PW16 6
        .byte 72
        .byte $00, $00, $00, $00
dl_320d .byte <GFX, WM0, >GFX
        PW16 2
        .byte 8
        .byte <(GFX + $40), WM0, >GFX
        PW16 7
        .byte 88
        .byte $00, $00, $00, $00
; The second object overlaps the first's right half; kangaroo mode writes
; its 00 pairs too.
dl_kang .byte <GFX, WM0, >GFX
        PW16 3
        .byte 8
        .byte <(GFX + $80), WM0, >GFX
        PW16 4
        .byte 40
        .byte $00, $00, $00, $00
dl_ind  .byte <charmap, WM0_IND, >charmap
        PW16 5
        .byte 16
        .byte $00, $00, $00, $00
; 16 characters of 2 bytes each: c and c + 1 at CHARBASE:c.
charmap
CHR     SET 0
        REPEAT 16
        .byte CHR * 2
CHR     SET CHR + 1
        REPEND

; The DLL: a 16-line empty zone; five 16-line drawn zones, each with DLI
; set and followed by an 8-line empty zone; seven 16-line empty zones, past
; line 258.
modes_dll
        .byte $0F, >lib_blank_dl, <lib_blank_dl
        .byte $8F, >dl_160b, <dl_160b
        .byte $07, >lib_blank_dl, <lib_blank_dl
        .byte $8F, >dl_320b, <dl_320b
        .byte $07, >lib_blank_dl, <lib_blank_dl
        .byte $8F, >dl_320d, <dl_320d
        .byte $07, >lib_blank_dl, <lib_blank_dl
        .byte $8F, >dl_kang, <dl_kang
        .byte $07, >lib_blank_dl, <lib_blank_dl
        .byte $8F, >dl_ind, <dl_ind
        .byte $07, >lib_blank_dl, <lib_blank_dl
        REPEAT 7
        .byte $0F, >lib_blank_dl, <lib_blank_dl
        REPEND

; The graphics: page r, byte i holds i's low 4 bits over r XOR i's, so every
; data pair and every D3 D2 appear.
        IF * > GFX
        ERR
        ENDIF
        ds GFX - *, 0
GR      SET 0
        REPEAT 16
GI      SET 0
        REPEAT 256
        .byte ((GI << 4) & $F0) | ((GR ^ GI) & $0F)
GI      SET GI + 1
        REPEND
GR      SET GR + 1
        REPEND

        LIB_TAIL
