; -----------------------------------------------
; --- extern declared SFRs
; -----------------------------------------------
	extern _STATUS
	extern _FSR0L
	extern _FSR0H
	extern _INDF0
	extern _PCLATH
	extern _PCL

C	EQU	0
DC	EQU	1
Z	EQU	2
FSR0    EQU     0x04 ; for MOVWI FSR0++
FSR1    EQU     0x06
	
; -----------------------------------------------
; --- generic pointer access helpers
; -----------------------------------------------

GPTRTAG_DATA	EQU	0x00
GPTRTAG_CODE	EQU	0x80

; setup FSR0 to point to (STK00:STK01)
setup_fsr	macro
	movf	STK01, W
	movwf	_FSR0L
	movf	STK00, W
	movwf	_FSR0H
	endm

; dispatch according to gptr type
select_routine	macro	dataptr, codeptr
	; __data pointer tag: 0x00
	xorlw	GPTRTAG_DATA
	btfsc	_STATUS, Z
	goto	dataptr
	; __code pointer tag: 0x80
	xorlw	(GPTRTAG_DATA ^ GPTRTAG_CODE)
	btfsc	_STATUS, Z
	goto	codeptr
	endm

check_data	macro	dataptr
	; __data pointer tag: 0x00
	xorlw	GPTRTAG_DATA
	btfsc	_STATUS, Z
	goto	dataptr
	endm

