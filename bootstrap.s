;
;	FIXME: load from the end of the disk not the start.
;
	.segment "BOOT"

ptr1	=	$FE

	; At 0x0200

	sei

	lda #<hello
	ldx #>hello
	jsr print

	lda #0
	tay
	sta ptr1
	sta $FE30	;	disk 0
	lda #$FF
	sta $FE31	;	block high
	lda #$80	;	block low (last 64K of our 32MB disk)
	sta $FE32
	lda #1
	sta $FE33

	ldx #$DE	; going to load 56K - 512 bytes
	lda #$20
	sta ptr1+1	; at 0x2000-0xFDFF
diskload_2:
	lda $FE35
	cmp #0
	bne diskfault
diskload_1:
	lda $FE34
	sta (ptr1),Y
	iny
	bne diskload_1
	lda #'*'
	sta $FE20
	inc ptr1+1
	dex
	bne diskload_2
	lda #13
	sta $FE20
	lda #10
	sta $FE20
	lda $2000
	cmp #$65
	bne badcode
	lda $2001
	cmp #$02
	bne badcode
	lda #<booting
	ldx #>booting
	jmp $2002

print:
	sta ptr1
	stx ptr1+1
	ldy #0
print_loop:
	lda (ptr1),y
	cmp #0
	beq printed
	sta $FE20
	iny
	jmp print_loop
printed:lda #13
	sta $FE20
	lda #10
	sta $FE20
	rts

diskfault:
	lda #<disk
	ldx #>disk
	jsr print
spin:	jmp spin

badcode:
	lda #<bad
	ldx #>bad
	jsr print
	jmp spin


hello:
	.byte "Disk Loader 0.1",13,10,"Loading...",0
booting:
	.byte "Booting...",0
bad:
	.byte "No loadable image found.",0
disk:
	.byte "Disk error.",0
