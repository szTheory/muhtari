; SPDX-License-Identifier: MIT
; Owned PAL raster and sustained-source diagnostic, mailbox identity19.
; Raster coordinates and DMA pipeline: docs/hardware/maria.md, Rendering.
; TIA/POKEY source clocks: docs/hardware/tia.md and pokey.md.
; The host separately judges startup and600 complete hardware intervals.
; Record7 counts visible starts after the deliberately ignored startup
; visible interval; a done mailbox by itself does not prove600 full frames.
        processor 6502
        ORG $C000
        include "lib.s"

FULL_LO = $85
FULL_HI = $86
START_M = $87
LINE_LO = $88
LINE_HI = $89

        MAC RECORD
        ldx #0
        ldy #{1}
        jsr mb_record
        ENDM
        MAC MRECORD
        lda MSTAT
        and #$80
        RECORD {1}
        ENDM

reset SUBROUTINE
        sei
        cld
        ldx #$FF
        txs
        lda MSTAT
        and #$80
        sta START_M
        lda #7
        sta INPTCTRL
        lda #CTRL_OFF
        sta CTRL
        lda #19
        jsr mb_init
        lda START_M
        RECORD 1
        lda #0
        sta FULL_LO
        sta FULL_HI
        ; Pure TIA tones, no later audio writes or horizontal resync.
        sta $17                 ; AUDF0
        sta $18                 ; AUDF1
        lda #4
        sta $15                 ; AUDC0
        sta $16                 ; AUDC1
        lda #15
        sta $19                 ; AUDV0
        sta $1A                 ; AUDV1
        ; Supported POKEY@$450, channel1 pure tone at the64k prescaler.
        lda #0
        sta $0458               ; AUDCTL
        lda #3
        sta $045F               ; SKCTL: running
        lda #100
        sta $0450               ; AUDF1
        lda #$AF
        sta $0451               ; AUDC1
        lda #0
        sta $0459               ; STIMER
        lda #3
        sta BACKGRND
        lda #$2F
        sta P0C3
        lda #>pal_dll
        sta DPPH
        lda #<pal_dll
        sta DPPL
        ; First visible interval belongs to startup and is not progress1.
        jsr wait_visible
        MRECORD 2
        ; DPP loads at VBE. Turning DMA on after VBE would walk the old
        ; startup DL until the following frame, so arm only in VBLANK.
        jsr wait_vblank
        lda #CTRL_ON
        sta CTRL
        jsr wait_visible
        inc FULL_LO             ; first complete host interval has begun
        ; WSYNC counts physical lines, independently of the PAL CPU/TIA
        ; phase drift. From raster16 to307 is291 line boundaries.
        lda #<291
        sta LINE_LO
        lda #>291
        sta LINE_HI
.last   sta WSYNC
        lda LINE_LO
        bne .dec
        dec LINE_HI
.dec    dec LINE_LO
        lda LINE_LO
        ora LINE_HI
        bne .last
        MRECORD 3
        jsr wait_vblank         ; terminal raster308
        MRECORD 4
        sta WSYNC               ; raster309: no further DMA request
        MRECORD 5
        REPEAT 4                ;309 ->310 ->311 ->312 ->0
        sta WSYNC
        REPEND
        MRECORD 6
.full   jsr wait_visible
        inc FULL_LO
        bne .count
        inc FULL_HI
.count  lda FULL_HI
        cmp #>600
        bne .next
        lda FULL_LO
        cmp #<600
        beq .done
.next   jsr wait_vblank
        jmp .full
.done   lda FULL_LO
        ldx FULL_HI
        ldy #7
        jsr mb_record
        MRECORD 8
        jsr pal_finish
        ; Keep the same native image and clocks during the rest of the
        ;600th interval and thereafter; no results-screen reconfiguration.
.forever
        jmp .forever

; Same CRC-16/1021/FFFF protocol as lib.s. A byte lookup keeps final
; publication inside the600th interval, leaving continued source output
; after status00 for the host's independent cadence observation.
pal_finish SUBROUTINE
        lda #$FF
        sta CRC_LO
        sta CRC_HI
        ldy #$10
.byte   lda MB,y
        eor CRC_HI
        tax
        lda CRC_LO
        eor pal_crc_hi,x
        sta CRC_HI
        lda pal_crc_lo,x
        sta CRC_LO
        iny
        bne .byte
        lda CRC_LO
        sta MB_CRC
        lda CRC_HI
        sta MB_CRC+1
        lda #0
        sta MB_STATUS
        rts

; Tables generated from the stated polynomial, not copied from a tool.
        MAC CRC_TABLE
CRC_I   SET 0
        REPEAT 256
CRC_N   SET CRC_I*256
        REPEAT 8
        IF CRC_N & $8000
CRC_N   SET ((CRC_N*2) ^ $1021) & $FFFF
        ELSE
CRC_N   SET (CRC_N*2) & $FFFF
        ENDIF
        REPEND
        IF {1}
        .byte >CRC_N
        ELSE
        .byte <CRC_N
        ENDIF
CRC_I   SET CRC_I+1
        REPEND
        ENDM
pal_crc_hi
        CRC_TABLE 1
pal_crc_lo
        CRC_TABLE 0

;160A direct bytes at each horizontal endpoint. Each FF fills four160
; cells, doubled to x0..7 and312..319. DMA fills the next raster's line
; RAM. VBE16 loads the zone; DMA17 first supplies raster18, so rows0/1
; hold background and rows2..291 contain both endpoint patterns.
pal_dl
        .byte <pal_gfx, $40, >pal_gfx, $1F, 0
        .byte <pal_gfx, $1F, >pal_gfx, 156
        .byte 0, 0, 0, 0
pal_dll
        REPEAT 19               ;304 lines of zones cover all PAL DMA
        .byte $0F, >pal_dl, <pal_dl
        REPEND
        ds $E000-*, $FF
pal_gfx
        REPEAT 16
        .byte $FF
        ds 255, 0
        REPEND
        LIB_TAIL
