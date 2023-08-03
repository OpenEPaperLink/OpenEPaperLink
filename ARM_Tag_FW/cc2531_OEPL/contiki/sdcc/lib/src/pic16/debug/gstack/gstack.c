/*-------------------------------------------------------------------------
   gstack.c - debug stack tracing support function

   Copyright (C) 2004, Vangelis Rokas <vrokas at otenet.gr>

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License 
   along with this library; see the file COPYING. If not, write to the
   Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.

   As a special exception, if you link this library with other files,
   some of which are compiled with SDCC, to produce an executable,
   this library does not by itself cause the resulting executable to
   be covered by the GNU General Public License. This exception does
   not however invalidate any other reasons why the executable file
   might be covered by the GNU General Public License.
-------------------------------------------------------------------------*/

extern int WREG;
extern int FSR1L;
extern int FSR1H;
extern int FSR0L;
extern int FSR0H;
extern int STATUS;
extern int POSTINC0;
extern int POSTDEC1;
extern int PREINC1;
extern int TOSL;
extern int TOSH;
extern int TOSU;
extern int PCL;
extern int PCLATH;
extern int PCLATU;
extern int stack;
extern int stack_end;

#pragma udata access _wreg_store _status_store _fsr0_store
#pragma udata access _gstack_begin _gstack_end _init_ok

static char _init_ok=0;
static char _wreg_store;
static char _status_store;
static unsigned int _fsr0_store;
static unsigned int _gstack_begin;
static unsigned int _gstack_end;

char _gstack_fail_str[]="Stack overflow\n";
char _gstack_succ_str[]="Stack ok\n";


static
void _gstack_overflow_default(void) __naked
{
  __asm
    lfsr	0, __gstack_fail_str
;    incf	_FSR0L, f

@0:
    movf	_POSTINC0, w
    movff	_WREG, 0xf7f
    bnz		@0
    
;    sleep
@00:
    goto	@00
    
  __endasm ;
}

void (* _gstack_overflow)(void)=_gstack_overflow_default;

    
void _gstack_init(void) __naked
{
  __asm
    
    movlw	LOW(_stack)
    movwf	__gstack_begin
    
    movlw	HIGH(_stack)
    movwf	__gstack_begin+1

    movlw	LOW(_stack_end)
    movwf	__gstack_end
    
    movlw	HIGH(_stack_end)
    movwf	__gstack_end+1

    ; load default handler
;    movlw	LOW(__gstack_overflow_default)
;    movwf	__gstack_overflow
    
;    movlw	HIGH(__gstack_overflow_default)
;    movwf	__gstack_overflow+1
    
;    movlw	UPPER(__gstack_overflow_default)
;    movwf	__gstack_overflow+2
    

    movlw	1
    movwf	__init_ok
    
    return;    
  __endasm ;
}


void _gstack_test(void) __naked
{
  __asm
    movff	_WREG, __wreg_store
    movff	_STATUS, __status_store

    ; if first time, initialize boundary variables
    movf	__init_ok, w
    bnz		@1
    call	__gstack_init
    
@1:
    movf	__gstack_begin, w
    cpfslt	_FSR1L
    bra		@2
    bra		@3

@2:
    movf	__gstack_begin+1, w
    cpfslt	_FSR1H
    bra		@4
    bra		@3

@4:
    movf	__gstack_end, w
    cpfsgt	_FSR1L
    bra		@5
    bra		@3

@5:
    movf	__gstack_end+1, w
    cpfsgt	_FSR1H
    bra		@6

    ; fail

@3:

    push
    movlw	LOW(ret_lab)
    movwf	_TOSL

    movlw	HIGH(ret_lab)
    movwf	_TOSH

    movlw	UPPER(ret_lab)
    movwf	_TOSU

    movff	__gstack_overflow+2, _PCLATU
    movff	__gstack_overflow+1, _PCLATH
    movf	__gstack_overflow, w
    
    ; execute 
    movwf	_PCL
    
ret_lab:
    bra		@10

    ; success
@6:
    movff	_FSR0L, __fsr0_store
    movff	_FSR0H, __fsr0_store+1
    lfsr	0, __gstack_succ_str

    ; print corresponding string
@8:
    movf	_POSTINC0, w
    movff	_WREG, 0xf7f
    bnz		@8

@9:
    movff	__fsr0_store+1, _FSR0H
    movff	__fsr0_store, _FSR0L

@10:
    movff	__status_store, _STATUS
    movff	__wreg_store, _WREG
    
    return
    
    __endasm ;
}
