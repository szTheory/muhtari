; SPDX-License-Identifier: MIT
; The diagnostics' common code: the 7800's registers, the result mailbox at
; $2200 with its CRC-16, and a results screen drawn in our own hex glyphs.
; A diagnostic includes this file right after its ORG $C000 and ends with
; LIB_TAIL, which places the font at $F800-$FFFF and the vectors. It defines
; reset; IRQ returns at once, and so does NMI unless it defines diag_nmi.

; The registers (Software Guide; docs/hardware/maria.md, Registers;
; docs/hardware/riot.md, Registers). Pages 0 and 1 below $40 are zero page,
; so the TIA and MARIA registers assemble as zero-page operands.
INPTCTRL = $01
BACKGRND = $20
P0C1     = $21
P0C2     = $22
P0C3     = $23
WSYNC    = $24
MSTAT    = $28
DPPH     = $2C
DPPL     = $30
CHARBASE = $34
CTRL     = $3C
INTIM    = $0284
TIM1T    = $0294

; CTRL values: DMA mode 10 (on) or 11 (off), 160A, 1-byte characters, no
; colour kill and no kangaroo mode; CTRL_ON_CW2 has 2-byte characters.
CTRL_ON     = $40
CTRL_ON_CW2 = $50
CTRL_OFF    = $60

; The mailbox (CONFORMANCE section 4): magic "M7DG", version 1, status
; ($80 running, $00 done), records written, ROM id, the CRC-16 of
; $2210-$22FF stored low byte first, then records of four bytes: test id,
; flags (bit 0 valid), value low, value high.
MB         = $2200
MB_VERSION = $2204
MB_STATUS  = $2205
MB_COUNT   = $2206
MB_ROMID   = $2207
MB_CRC     = $2208
MB_REC     = $2210

; Zero-page work bytes, RAM at $2080-$2084.
CRC_LO = $80
CRC_HI = $81
TMP_LO = $82
TMP_HI = $83
SHOW_N = $84

; The results screen: one row of 7 characters per record, "NN VVVV", the
; id and the value in hex, and the CRC on the last row; 27 records at most.
; The character maps are RAM at $1800.
SHOW_MAP  = $1800
SHOW_ROWS = 28
SHOW_RECS = SHOW_ROWS - 1
SHOW_BLANK = 16
FONT      = $F800

; mb_init: clears $2200-$22FF and opens the mailbox for ROM id A.
mb_init SUBROUTINE
        pha
        lda #0
        tax
.clear  sta MB,x
        inx
        bne .clear
        lda #'M
        sta MB
        lda #'7
        sta MB+1
        lda #'D
        sta MB+2
        lda #'G
        sta MB+3
        lda #1
        sta MB_VERSION
        lda #$80
        sta MB_STATUS
        pla
        sta MB_ROMID
        rts

; mb_record: appends a valid record with test id Y and value X:A.
mb_record SUBROUTINE
        sta TMP_LO
        stx TMP_HI
        lda MB_COUNT
        asl
        asl
        tax
        tya
        sta MB_REC,x
        lda #1
        sta MB_REC+1,x
        lda TMP_LO
        sta MB_REC+2,x
        lda TMP_HI
        sta MB_REC+3,x
        inc MB_COUNT
        rts

; mb_finish: stores the CRC-16 of $2210-$22FF, then status $00, last, so a
; reader that sees $00 sees the whole mailbox.
mb_finish SUBROUTINE
        jsr crc16
        lda CRC_LO
        sta MB_CRC
        lda CRC_HI
        sta MB_CRC+1
        lda #0
        sta MB_STATUS
        rts

; crc16: CRC-16 with polynomial $1021, initial value $FFFF, most significant
; bit first and no final XOR, over the 240 bytes $2210-$22FF, into
; CRC_HI:CRC_LO.
crc16 SUBROUTINE
        lda #$FF
        sta CRC_LO
        sta CRC_HI
        ldy #$10
.each   lda MB,y
        eor CRC_HI
        sta CRC_HI
        ldx #8
.shift  asl CRC_LO
        rol CRC_HI
        bcc .next
        lda CRC_HI
        eor #$10
        sta CRC_HI
        lda CRC_LO
        eor #$21
        sta CRC_LO
.next   dex
        bne .shift
        iny
        bne .each
        rts

; wait_vblank returns once MSTAT shows VBLANK, wait_visible once it does not.
wait_vblank SUBROUTINE
.wait   bit MSTAT
        bpl .wait
        rts

wait_visible SUBROUTINE
.wait   bit MSTAT
        bmi .wait
        rts

; show: draws the mailbox and never returns. Row n holds record n's id, a
; blank and its value; the last row holds the CRC.
show SUBROUTINE
        ldx #SHOW_ROWS*7-1
        lda #SHOW_BLANK
.fill   sta SHOW_MAP,x
        dex
        cpx #$FF
        bne .fill
        lda MB_COUNT
        cmp #SHOW_RECS
        bcc .count
        lda #SHOW_RECS
.count  sta SHOW_N
        ldx #0
        ldy #0
        lda SHOW_N
        beq .crc
.row    lda MB_REC,x
        jsr show_byte
        iny
        lda MB_REC+3,x
        jsr show_byte
        lda MB_REC+2,x
        jsr show_byte
        inx
        inx
        inx
        inx
        dec SHOW_N
        bne .row
.crc    ldy #SHOW_RECS*7+3
        lda MB_CRC+1
        jsr show_byte
        lda MB_CRC
        jsr show_byte
        jsr wait_vblank
        lda #>show_dll
        sta DPPH
        lda #<show_dll
        sta DPPL
        lda #>FONT
        sta CHARBASE
        lda #$00
        sta BACKGRND
        lda #$0F
        sta P0C1
        sta P0C2
        sta P0C3
        lda #CTRL_ON
        sta CTRL
.forever
        jmp .forever

; show_byte: A's two hex digits as glyph numbers at SHOW_MAP+Y, Y += 2.
show_byte SUBROUTINE
        pha
        lsr
        lsr
        lsr
        lsr
        sta SHOW_MAP,y
        iny
        pla
        and #$0F
        sta SHOW_MAP,y
        iny
        rts

lib_nmi SUBROUTINE
        rti

; A blank zone's DL: a terminator.
lib_blank_dl
        .byte $00, $00, $00, $00

; One DL per row: a 5-byte indirect header, write mode 0, palette 0, width 7
; (W = 25), at hpos 8, over the row's character map; then a terminator.
show_dl
LIB_R   SET 0
        REPEAT SHOW_ROWS
        .byte <(SHOW_MAP + LIB_R*7), $60, >(SHOW_MAP + LIB_R*7), $19, 8
        .byte $00, $00, $00, $00
LIB_R   SET LIB_R + 1
        REPEND

; The screen's DLL: an 8-line blank zone, one 8-line zone (OFFSET 7) per row
; and two 16-line blank zones, past line 258.
show_dll
        .byte $07, >lib_blank_dl, <lib_blank_dl
LIB_R   SET 0
        REPEAT SHOW_ROWS
        .byte $07, >(show_dl + LIB_R*9), <(show_dl + LIB_R*9)
LIB_R   SET LIB_R + 1
        REPEND
        .byte $0F, >lib_blank_dl, <lib_blank_dl
        .byte $0F, >lib_blank_dl, <lib_blank_dl

; LIB_TAIL: the font and the vectors, after the diagnostic's own code.
; Character c's graphics for a zone line with OFFSET o are at FONT + o * 256
; + c, and OFFSET counts down from 7, so glyph row r (0 the top) sits in page
; 7 - r. The 16 hex glyphs and a blank are 3 by 5 pixels in rows 1-5, in
; 160A with colour 3 for a lit pixel and the fourth pixel a gap.
        MAC LIB_TAIL
        IF * > FONT
        ERR
        ENDIF
        ORG FONT
        ds 2*256, 0
        ;     0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F    blank
        .byte $FC, $FC, $FC, $FC, $0C, $FC, $FC, $0C, $FC, $FC, $CC, $F0, $FC, $F0, $FC, $C0, $00
        ds 256-17, 0
        .byte $CC, $30, $C0, $0C, $0C, $0C, $CC, $0C, $CC, $0C, $CC, $CC, $C0, $CC, $C0, $C0, $00
        ds 256-17, 0
        .byte $CC, $30, $FC, $FC, $FC, $FC, $FC, $0C, $FC, $FC, $FC, $F0, $C0, $CC, $FC, $FC, $00
        ds 256-17, 0
        .byte $CC, $F0, $0C, $0C, $CC, $C0, $C0, $0C, $CC, $CC, $CC, $CC, $C0, $CC, $C0, $C0, $00
        ds 256-17, 0
        .byte $FC, $30, $FC, $FC, $CC, $FC, $FC, $FC, $FC, $FC, $FC, $F0, $FC, $F0, $FC, $FC, $00
        ds 256-17, 0
        ds $FFF8-*, 0
        ; $FFF8: region and signature flags; $FFF9: the ROM starts at $C000.
        .byte $FF, $C7
        IFCONST diag_nmi
        .word diag_nmi, reset, lib_nmi
        ELSE
        .word lib_nmi, reset, lib_nmi
        ENDIF
        ENDM
