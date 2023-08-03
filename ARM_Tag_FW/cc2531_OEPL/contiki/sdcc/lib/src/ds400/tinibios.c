/*-------------------------------------------------------------------------
   tinibios.c - startup and serial routines for the DS80C400 (tested on TINIM400)

   Copyright (C) 2003, Johan Knol <johan.knol AT iduna.nl>

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

#include <tinibios.h>
#include <ds400rom.h>

#include <stdio.h>

#define TIMED_ACCESS(sfr,value) { TA=0xaa; TA=0x55; sfr=value; }

unsigned char _sdcc_external_startup(void)
{
  IE = 0; // Disable all interrupts.

  PSW = 0;
  
  __asm
  ; save the 24-bit return address
  pop ar2; msb
  pop ar1
  pop ar0; lsb


  mov _ESP,#0x00; reinitialize the stack
  mov _SP,#0x00

  ; restore the 24-bit return address
  push ar0; lsb
  push ar1
  push ar2; msb
  __endasm;    
  
  // Stub: call rom_init here, then fixup IVT.
  // 
  
  Serial0Init(1, 0); // baud argument ignored.
  
  IE = 0x80; // Enable interrupts.
  
  return 0;
}

// now the serial0 stuff

// just to make the code more readable 
#define S0RBS SERIAL_0_RECEIVE_BUFFER_SIZE

// this is a ring buffer and can overflow at anytime!
static volatile unsigned char receive0Buffer[S0RBS];
static volatile int receive0BufferHead=0;
static volatile int receive0BufferTail=0;
// no buffering for transmit
static volatile char transmit0IsBusy=0;

static __data unsigned char serial0Buffered;

/* Initialize serial0.

   Available baudrates are from 110 upto 115200 (using 16-bit timer 2)
   If baud==0, the port is disabled.

   If buffered!=0, characters received are buffered using an interrupt
*/


#define TIMER_RELOAD (65536 - ((OSCILLATOR) / (32 * SERIAL_0_BAUD)))

void Serial0Init (unsigned long baud, unsigned char buffered)
{
  ES0 = 0; // disable serial channel 0 interrupt

#if 0
  // Need no port setup, done by boot rom.
  baud;
#else
  SCON0 = 0x5A; // 10 bit serial 0, use timer baud rate, enable recieving
  RCAP2H = (TIMER_RELOAD >> 8) & 0xff;
  RCAP2L = TIMER_RELOAD & 0xff;
  T2CON = 0x30; // Enable timer 2 for serial port
  TR2 = 1; // Set timer 2 to run

  baud;
#endif

  serial0Buffered=buffered;
 
 if (buffered) {
    installInterrupt(Serial0IrqHandler, 0x23);
    RI_0=TI_0=0; // clear "pending" interrupts
    ES0 = 1; // enable serial channel 0 interrupt
  } else {
    RI_0=0; // receive buffer empty
    TI_0=1; // transmit buffer empty
  }
}

void Serial0SwitchToBuffered(void)
{
  IE &= ~0x80;
  
  serial0Buffered = 1;
  installInterrupt(Serial0IrqHandler, 0x23);
  RI_0=TI_0=0; // clear "pending" interrupts
  ES0 = 1; // enable serial channel 0 interrupt
  
  IE |= 0x80;
}

void Serial0IrqHandler (void) __interrupt 4
{
  if (RI_0) {
    receive0Buffer[receive0BufferHead]=SBUF0;
    receive0BufferHead=(receive0BufferHead+1)&(S0RBS-1);
    if (receive0BufferHead==receive0BufferTail) {
      /* buffer overrun, sorry :) */
      receive0BufferTail=(receive0BufferTail+1)&(S0RBS-1);
    }
    RI_0=0;
  }
  if (TI_0) {
    TI_0=0;
    transmit0IsBusy=0;
  }
}

char Serial0CharArrived(void)
{
  if (serial0Buffered) {
    if (receive0BufferHead!=receive0BufferTail)
      return receive0Buffer[receive0BufferTail];
  } else {
    if (RI_0)
      return SBUF0;
  }
  return 0;
}

void Serial0PutChar (char c)
{
  if (serial0Buffered) {
    while (transmit0IsBusy)
      ;
    transmit0IsBusy=1;
    SBUF0=c;
  } else {
    while (!TI_0)
      ;
    TI_0 = 0;
    SBUF0=c;
  }
}

char Serial0GetChar (void)
{
  char c;
  if (serial0Buffered) {
    while (receive0BufferHead==receive0BufferTail)
      ;
    c=receive0Buffer[receive0BufferTail];
    ES0=0; // disable serial interrupts
    receive0BufferTail=(receive0BufferTail+1)&(S0RBS-1);
    ES0=1; // enable serial interrupts
  } else {
    while (!RI_0)
      ;
    c=SBUF0;
    RI_0=0;
  }
  return c;
}

void Serial0SendBreak()
{
  P3 &= ~0x02;
  ClockMilliSecondsDelay(2);
  P3 |= 0x02;
}

void Serial0Flush()
{
  ES0=0; // disable interrupts
  receive0BufferHead=receive0BufferTail=0;
  RI_0=0;
  if (serial0Buffered) {
    TI_0=0;
    ES0=1; // enable interrupts
  } else {
    TI_0=1;
  }
}

// now let's go for the clock stuff - on the DS400, we can just
// use the ROM's millisecond timer, running off timer 0.
// 
// for now, this timer runs too fast by about 20%. We need an implementation of
// task_settickreload to fix this.

void ClockInit()
{
  // nada, all done by DSS_rom_init
}

// we can't just use milliSeconds
unsigned long ClockTicks(void)
{
  return task_gettimemillis_long();
}

void ClockMilliSecondsDelay(unsigned long delay)
{
  unsigned long ms = task_gettimemillis_long() + delay;

  while (ms > task_gettimemillis_long())
    ;
}

// Return the start of the XI_SEG. Really just a workaround for the
// fact that the linker defined symbol (s_XISEG) isn't directly accessible
// from C due to the lack of a leading underscore, and I'm too lazy to hack 
// the linker.
static void __xdata *_xisegStart(void) __naked
{
  __asm    
  mov dptr, #(s_XISEG)
  ret
  __endasm;
}

// Return the length of the XI_SEG. Really just a workaround for the
// fact that the linker defined symbol (l_XISEG) isn't directly accessible
// from C due to the lack of a leading underscore, and I'm too lazy to hack 
// the linker.
static unsigned  _xisegLen(void) __naked
{
  __asm
  mov dptr, #(l_XISEG)
  ret
  __endasm;
}

// Returns the address of the first byte available for heap memory, 
// i.e. the first byte following the XI_SEG.
static void __xdata *_firstHeapByte(void)
{
  unsigned char __xdata *start;
  
  start = (unsigned char __xdata *) _xisegStart();
  start += _xisegLen();

  return (void __xdata *)start;
}

// TINIm400 specific startup.

// The last addressible byte of the CE0 area. 
#define CE0_END 0xfffff

unsigned char romInit(unsigned char noisy, char speed)
{
  void __xdata *heapStart;
  void __xdata *heapEnd;
  unsigned long heapLen; 
  unsigned char rc;

  if (speed == SPEED_2X)
  {
    PMR = 0x82;
    PMR = 0x92;

    while (!(EXIF & 8))
      ;

    PMR = 0x12;
  }
  else if (speed == SPEED_4X)
  {
    // Hangs on TINIm400!
    PMR = 0x82;
    PMR = 0x8a;
    PMR = 0x9a;

    while (!(EXIF & 8))
      ;

    PMR = 0x1a;
  }

  heapStart = _firstHeapByte();
  heapEnd = (void __xdata *)CE0_END;

  rc = init_rom(heapStart, heapEnd);

  if (noisy)
  {
    if (rc)
    {
      printf("error: rom_init returns %d\n", (int)rc);
      return rc;
    }
    else
    {
      heapLen = CE0_END - (unsigned long)heapStart;
      printf("Heap starts at %p, length %luK\n", heapStart, heapLen / 1024);
    }
  }

  task_settickreload(RELOAD_14_746);

  // Switch to interrupt driven serial I/O now that the rom is initialized.
  Serial0SwitchToBuffered();

  P5 &= ~4; // LED on.

  return 0;
}

// Install an interrupt handler.
void installInterrupt(void (*isrPtr)(void), unsigned char offset)
{
  unsigned char __xdata * vectPtr = (unsigned char __xdata *) offset;
  unsigned long isr = (unsigned long)isrPtr;

  *vectPtr++ = 0x02;
  *vectPtr++ = (unsigned char)(isr >> 16);
  *vectPtr++ = (unsigned char)(isr >> 8);
  *vectPtr = (unsigned char)isr;
}
