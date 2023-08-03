; -----------------------------------------------
; --- extern declared SFRs
; -----------------------------------------------
	extern _STATUS
	extern _FSR
	extern _INDF
	extern _PCLATH
	extern _PCL

C	EQU	0
DC	EQU	1
Z	EQU	2
IRP	EQU	7
	
; -----------------------------------------------
; --- generic pointer access helpers
; -----------------------------------------------

GPTRTAG_DATA	EQU	0x00
GPTRTAG_CODE	EQU	0x80

; setup FSR to point to (STK00:STK01)
setup_fsr	macro
	movf	STK01, W
	movwf	_FSR
	; select indirectly addressed bank
	bcf	_STATUS, IRP
	btfsc	STK00, 0
	bsf	_STATUS, IRP
	endm

; increment FSR, update IRP on bank crossings
; FIXME: update IRP if FSR is reset to 0...
inc_fsr		macro
	incf	_FSR, F
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

