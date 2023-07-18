/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*            (c) 2006 - 2019 SEGGER Microcontroller GmbH             *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
* All rights reserved.                                               *
*                                                                    *
* Redistribution and use in source and binary forms, with or         *
* without modification, are permitted provided that the following    *
* condition is met:                                                  *
*                                                                    *
* o Redistributions of source code must retain the above copyright   *
*   notice, this condition and the following disclaimer.             *
*                                                                    *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND             *
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,        *
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF           *
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
* DISCLAIMED. IN NO EVENT SHALL SEGGER Microcontroller BE LIABLE FOR *
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT  *
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;    *
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF      *
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE  *
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
* DAMAGE.                                                            *
*                                                                    *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File    : JLINKDCC_Process.c
Purpose : Data handler for ARM J-Link type communication via DCC (IAR version, using intrinsics)
Changes : 08-10-08
          Support for "channels"added, where
          channel 0 is reserved for terminal
          channel 1 is reserved for OS communication such as embOSView
          11-11-15
          Cortex A/R defines modified.

Notes   : (1) How to use
              In order to use the DCC communication to read / write memory, the
                following needs to be done:
              * Add this file to the project / make-file
              * Make sure this data handler is called regularly
              * Add the JLINKDCC data abort handler (optional)
                For details, refer to the documentation or see file JLINKDCC_HandleDataAbort.s79.
          (2) Compatibility
              The J-Link ARM DCC handler is compatible to the DCC communication
              protocol used by IAR in the embedded workbench for ARM and allows
              using the live data window in C-Spy

Protocol
  Following response packets from target will be possible:
  00 00 00 XX - reading a byte XX
  00 00 XX XX - reading a half word XXXX
  XX XX XX XX - reading a word, except words starting with 93zX XXXX (10010011-1xxxxxxx-xxxxxxx-xxxxxxx)

  93 8z 00 XX - terminal I/O one byte XX to channel z=0-15
  93 9z 00 XX - terminal I/O one byte XX to channel z=16-31
  93 Az XX XX - terminal I/O two bytes XX XX to channel z=0-15
  93 Bz XX XX - terminal I/O two bytes XX XX to channel z=16-31

  93 C0 XX XX - escape sequence for words starting with 93XX, the lower 16-bit part is in XX XX
  93 C1 XX XX - escape sequence for words starting with 93XX, the upper 16-bit part is in XX XX
  93 C2 XX XX - data abort for reading
  91 CA XX XX - signature (same as before)

  There is a new capability flag. C-SPY will use the new protocol when this is set.
  #define DCC_CAP_TERM_OUT   0x80

Sequence for U8 write:
  H->T    Addr & 0xFFFFFFFE
  H->T    ((Addr & 1) << 31) | (Data << 2) | 0x40000001

Sequence for U16 write:
  H->T    Addr & 0xFFFFFFFE
  H->T    ((Addr & 1) << 31) | (Data << 2) | 0x20000001

Sequence for U32 write:
  H->T    Addr & 0xFFFFFFFE
  H->T    (Data & 0xFFFF) << 2
  H->T    ((Addr & 1) << 31) | ((Data >> 14) & 0xFFFF) | 0x10000001

Sequence for U8 Read:
  H->T    Addr & 0xFFFFFFFE
  H->T    ((Addr & 1) << 31) | (NumItems << 2) | 0x04000001
  if (Aborted) {
    T->H    0x93c20000
  } else {
    T->H    Data
  }

Sequence for U16 Read:
  H->T    Addr & 0xFFFFFFFE
  H->T    ((Addr & 1) << 31) | (NumItems << 2) | 0x02000001
  if (Aborted) {
    T->H    0x93c20000
  } else {
    T->H    Data
  }

Sequence for U32 Read:
  H->T    Addr & 0xFFFFFFFE
  H->T    ((Addr & 1) << 31) | (NumItems << 2) | 0x01000001
  if (Aborted) {
    T->H    0x93c20000
  } else if ((Data >> 24) != 0x93) {
    T->H    Data
  } else {
    T->H    0x93c0.0000 | (Data & 0xFFFF)
    T->H    0x93c1.0000 | (Data >> 16)
  }

Terminal IN:  (target receives data)
H->T   0x93000000 + (Channel << 19) + (Data8 << 1) + DCC_OP_COMMAND

Terminal OUT: (target sends data)
T->H   0x93800000 + (Channel << 16) + (Data8)

*/

#include "JLINKDCC.h"

#ifdef __ICCARM__
  /* With IAR workbench we use intrinsics for CP14 communication */
  #include <intrinsics.h>
#else
  #define __ARM7A__ 1
  #define __ARM7R__ 2
  #define __CORE__  0    // Default is: DCC module is designed for ARM7/9. In order to support Cortex-A/R, __CORE__ has to be set to __ARM7A__ or __ARM7R__
  /* We use external functions from assembly module JLINKDCC_Process_ASM.s */
  extern unsigned long CP14_ReadDCCStat(void);
  extern unsigned long CP14_ReadDCC(void);
  extern void CP14_WriteDCC(unsigned long Data);
#endif

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/

#ifndef JLINKDCC_BUFFER_SIZE
  #define JLINKDCC_BUFFER_SIZE    1024          // Used for channel 0 (terminal out buffer)
#endif

#define NUM_CHANNELS              2

/*********************************************************************
*
*       Defines, non- configurable
*
**********************************************************************
*/

#define U8  unsigned char
#define U16 unsigned short
#define U32 unsigned long

#define DCC_OP_READ_U32   0x01000000
#define DCC_OP_READ_U16   0x02000000
#define DCC_OP_READ_U8    0x04000000
#define DCC_OP_GET_CAPS   0x08000000
#define DCC_OP_WRITE_U32  0x10000000
#define DCC_OP_WRITE_U16  0x20000000
#define DCC_OP_WRITE_U8   0x40000000
#define DCC_OP_ODD_ADDR   0x80000000
#define DCC_OP_COMMAND    0x00000001

#define DCC_SIGNATURE     0x91CA0000
#define DCC_CONFIG        0xFF       // All features are supported

#define DCC_CHANNEL_TERMINAL  0
#define DCC_CHANNEL_OS        1

//
// The bit positions for DCC RX and TX ready are different for ARM7/9 and Cortex-A/R,
// so we have to distinguish here.
//
#if (defined (__ARM7A__) && (__CORE__ == __ARM7A__)) || (defined (__ARM7R__) && (__CORE__ == __ARM7R__))
  #define RX_FULL_FLAG_SHIFT 30
  #define TX_FULL_FLAG_SHIFT 29
#else
  #define RX_FULL_FLAG_SHIFT 0
  #define TX_FULL_FLAG_SHIFT 1
#endif

/*********************************************************************
*
*       Global data
*
**********************************************************************
*/

U8 JLINKDCC_IsInHandler;
U8 JLINKDCC_AbortOccurred;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

void (* _pfOnRx[2]) (unsigned Channel, U8 Data);
void (* _pfOnTx[2]) (unsigned Channel);


static U16  _NumReadItems;
static U32  _Command;
static U32  _Addr;

static char _acBuffer[JLINKDCC_BUFFER_SIZE];
static U16  _RdPos;
static U16  _WrPos;

static U8   _ActiveChannel   = 0;

static char _WritePendingNB  = 0;
static U32  _PendingPacketNB = 0;

static char _WritePending;
static U32  _Data;

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

#ifdef __ICCARM__
/****** Use intrinsics for CP14 communication ***********************/

#if (defined (__ARM7A__) && (__CORE__ == __ARM7A__)) || (defined (__ARM7R__) && (__CORE__ == __ARM7R__))

/*********************************************************************
*
*       _ReadDCCStat
*
*  IAR macro:     unsigned long __MRC(coproc, opcode_1, CRn, CRm, opcode_2);
*  Inst:          MRC<c> <coproc>,<opc1>,<Rt>,<CRn>,<CRm>{,<opc2>}  Move from coproc to ARM reg
*  ARMv4 / ARMv5: MRC p14,0,<Rt>,c0,c0,0
*  ARMv7-AR:      MRC p14,0,<Rt>,c0,c1,0 DBGDSCR
*/
static int _ReadDCCStat(void) {
  return __MRC(14, 0, 0, 1, 0);
}

/*********************************************************************
*
*       _ReadDCC
*
*  IAR macro:     unsigned long __MRC(coproc, opcode_1, CRn, CRm, opcode_2);
*  Inst:          MRC<c> <coproc>,<opc1>,<Rt>,<CRn>,<CRm>{,<opc2>}  Move from coproc to ARM reg
*  ARMv4 / ARMv5: MRC p14,0,<Rt>,c1,c0,0
*  ARMv7-AR:      MRC p14,0,<Rt>,c0,c5,0 DTRRX
*/
static U32 _ReadDCC(void) {
  return __MRC(14, 0, 0, 5, 0);
}

/*********************************************************************
*
*       _WriteDCC
*
*  IAR macro:     void __MCR(coproc, opcode_1, Data, CRn, CRm, opcode_2);
*  Inst:          MCR<c> <coproc>,<opc1>,<Rt>,<CRn>,<CRm>{,<opc2>}  Move to coproc
*  ARMv4 / ARMv5: MCR P14,0,<Rt>,C1,C0,0
*  ARMv7-AR:      MCR p14,0,<Rt>,c0,c5,0 DTRTX
*/
static void _WriteDCC(U32 Data) {
  __MCR(14, 0, Data, 0, 5, 0);
}

#else // (defined (__ARM7A__) && (__CORE__ == __ARM7A__)) || (defined (__ARM7R__) && (__CORE__ == __ARM7R__))

/*********************************************************************
*
*       _ReadDCCStat
*
*  IAR macro:     unsigned long __MRC(coproc, opcode_1, CRn, CRm, opcode_2);
*  Inst:          MRC<c> <coproc>,<opc1>,<Rt>,<CRn>,<CRm>{,<opc2>}  Move from coproc to ARM reg
*  ARMv4 / ARMv5: MRC p14,0,<Rt>,c0,c0,0
*  ARMv7-AR:      MRC p14,0,<Rt>,c0,c1,0 DBGDSCR
*/
static __interwork __arm int _ReadDCCStat(void) {
  return __MRC( 14, 0, 0, 0, 0 );       // __asm("mrc P14,0,R0,C0,C0,0");
}

/*********************************************************************
*
*       _ReadDCC
*
*  IAR macro:     unsigned long __MRC(coproc, opcode_1, CRn, CRm, opcode_2);
*  Inst:          MRC<c> <coproc>,<opc1>,<Rt>,<CRn>,<CRm>{,<opc2>}  Move from coproc to ARM reg
*  ARMv4 / ARMv5: MRC p14,0,<Rt>,c1,c0,0
*  ARMv7-AR:      MRC p14,0,<Rt>,c0,c5,0 DTRRX
*/
static __interwork __arm U32 _ReadDCC(void) {
  return __MRC( 14, 0, 1, 0, 0 );       // __asm("mrc P14,0,R0,C1,C0,0");
}

/*********************************************************************
*
*       _WriteDCC
*
*  IAR macro:     void __MCR(coproc, opcode_1, Data, CRn, CRm, opcode_2);
*  Inst:          MCR<c> <coproc>,<opc1>,<Rt>,<CRn>,<CRm>{,<opc2>}  Move to coproc
*  ARMv4 / ARMv5: MCR P14,0,<Rt>,C1,C0,0
*  ARMv7-AR:      MCR p14,0,<Rt>,c0,c5,0 DTRTX
*/
static __interwork __arm void _WriteDCC(U32 Data) {
  __MCR( 14, 0, Data, 1, 0, 0 );        // __asm("mcr P14,0,R0,C1,C0,0");
}

#endif // (defined (__ARM7A__) && (__CORE__ == __ARM7A__)) || (defined (__ARM7R__) && (__CORE__ == __ARM7R__))
#else  //__ICCARM__

/****** Map externel CP14 communication routines ********************/
#define _ReadDCCStat     CP14_ReadDCCStat
#define _ReadDCC         CP14_ReadDCC
#define _WriteDCC(Data)  CP14_WriteDCC(Data)

#endif // __ICCARM__

/*********************************************************************
*
*       _HandleRead
*
*  Function description
*    Performs Command / data read from host
*/
static void _HandleRead(void) {
  U32 Data;

  if (((_ReadDCCStat() >> RX_FULL_FLAG_SHIFT)  & 1) == 0) {       // Data or command received ?
    return;      // Nothing received
  }

  Data = _ReadDCC();
  //
  // If item received does not have the command-flag set, it is the new addr.
  //
  if ((Data & DCC_OP_COMMAND) == 0) {
    _Addr = Data;
    return;
  }
  //
  // If item received is a terminal out command,
  //
  if ((Data & 0xFF000000) == 0x93000000) {
    unsigned Channel;
    Channel = (Data >> 19) & 0x1F;
    if (_pfOnRx[Channel]) {
      _pfOnRx[Channel](Channel, (Data >> 1) & 0xFF);
    }
    return;
  }
  //
  // We received a new command.
  //
  _Command = Data;
  if (_Command & DCC_OP_ODD_ADDR) {
    _Addr |= 1;
  }
  if (_Command & (DCC_OP_READ_U32 | DCC_OP_READ_U16 | DCC_OP_READ_U8 | DCC_OP_GET_CAPS)) {
    _NumReadItems = (_Command >> 2) & 0xffff;
  } else {
    // Clear before write
    JLINKDCC_AbortOccurred = 0;

    if (_Command & DCC_OP_WRITE_U32) {
      _Data |= (_Command << 14) & 0xffff0000;
    } else {
      _Data = (_Command >> 2) & 0xffff;
    }
    if (_Command & DCC_OP_WRITE_U8) {
      *(U8*)_Addr = _Data;
      _Addr += 1;
    }
    if (_Command & DCC_OP_WRITE_U16) {
      *(U16*)_Addr = _Data;
      _Addr += 2;
    }
    if (_Command & DCC_OP_WRITE_U32) {
      *(U32*)_Addr =_Data;
      _Addr += 4;
    }
  }
}

/*********************************************************************
*
*       _HandleWrite
*
*  Function description
*    Sends data back to host if there is space in DCC buffer and data to be send.
*/
static void _HandleWrite(void) {
  U32 Data;
  int DCCBusy;
  int NumBytes;

  Data = 0;
  DCCBusy = (_ReadDCCStat() >> TX_FULL_FLAG_SHIFT) & 1;
  if (DCCBusy) {                  // Can we send data ?
    return;                       // If not, we are done.
  }

  if (_ActiveChannel) {
    U32 Channel;

    Channel = _ActiveChannel;
    _ActiveChannel = 0;
    if (_WritePendingNB) {
      _WriteDCC(_PendingPacketNB);
      _WritePendingNB = 0;
    }
    if (_pfOnTx[Channel]) {
      _pfOnTx[Channel](Channel);
    }
    return;
  }

  //
  // Check if a data item is pending
  //
  if (_WritePending) {
    _WriteDCC(_Data);
    _WritePending = 0;
    return;
  }
  //
  // Check if a read command is pending
  //
  if (_NumReadItems) {
    if (_Command & DCC_OP_GET_CAPS) {
      Data = (DCC_CONFIG | DCC_SIGNATURE);
      Data |= (JLINKDCC_AbortOccurred << 8);  // write abort status
      JLINKDCC_AbortOccurred = 0;
    } else {

      // Clear before next read
      JLINKDCC_AbortOccurred = 0;

      if (_Command & DCC_OP_READ_U8) {
        Data = *(U8*)_Addr;
        _Addr += 1;
      } else if (_Command & DCC_OP_READ_U16) {
        Data = *(U16*)_Addr;
        _Addr += 2;
      } else if (_Command & DCC_OP_READ_U32) {
        Data = *(U32*)_Addr;
        _Addr += 4;
        if ((Data & 0xFF800000) == 0x93800000) {    // Do we need to use the escape sequence and split it up into 2 transfers ?
          _Data = 0x93c10000 | (Data >> 16);
          Data  = 0x93c00000 | (Data & 0xFFFF);
          _WritePending = 1;
        }
      }
      if (JLINKDCC_AbortOccurred) {
        Data = 0x93c20000;          // read abort status
        _WritePending = 0;
        JLINKDCC_AbortOccurred = 0; // clear it
      }
    }

    _WriteDCC(Data);
    _NumReadItems--;
    return;
  }
  //
  // Handle terminal out. Up to 2 bytes in one 32-bit unit
  //
  NumBytes = _WrPos - _RdPos;
  if (NumBytes < 0) {
    NumBytes += sizeof(_acBuffer);
  }
  if (NumBytes) {
    int i;
    if (NumBytes > 2) {
      NumBytes = 2;
    }
    if (NumBytes == 2) {
      Data = 0x93A00000;
    } else {
      Data = 0x93800000;
    }
    for (i = 0; i < NumBytes; i++) {
      Data |= _acBuffer[_RdPos] << (8*i);
      _RdPos++;
      if (_RdPos == sizeof(_acBuffer)) {
        _RdPos = 0;
      }
    }
    _WriteDCC(Data);
  }
}


/*********************************************************************
*
*       Public code
*
**********************************************************************
*/

/*********************************************************************
*
*       JLINKDCC_Process
*
*  Function description
*    This function should be called more or less regularily to allow
*    memory reads while the application progam is running.
*    The more often it is called, the higher the transfer speed.
*/
void JLINKDCC_Process(void) {
  //
  // Avoid problems if this code is called from multiple threads or interrupts
  //
  if (JLINKDCC_IsInHandler) {
    return;
  }
  JLINKDCC_IsInHandler = 1;
  _HandleRead();
  _HandleWrite();
   JLINKDCC_IsInHandler = 0;
}

/*********************************************************************
*
*       JLINKDCC_SendChar
*
*  Function description
*   Sends a single char to terminal out.
*/
void JLINKDCC_SendChar(char c) {
  int Pos;

  Pos = _WrPos + 1;
  if (Pos == sizeof(_acBuffer)) {
    Pos = 0;
  }
  if (Pos == _RdPos) {
    return;
  }
  _acBuffer[_WrPos] = c;
  _WrPos = Pos;
}

/*********************************************************************
*
*       JLINKDCC_SendString
*
*  Function description
*   Sends a NUL- terminated string to terminal out.
*/
void JLINKDCC_SendString(const char * s) {
  char c;
  while (1) {
    c = *s++;
    if (c == 0) {
      break;
    }
    JLINKDCC_SendChar(c);
  }
}

/*********************************************************************
*
*       JLINKDCC_SendCharOnChannelNB
*
*  Function description
*    Send data to the host on selected channel.
*    This function is non-blocking.
*    If data can not be send it is stored in a buffer
*    and sent later, when the DCC Handler is called.
*/
void JLINKDCC_SendCharOnChannelNB(unsigned Channel, U8 Data) {
  U32 DCCPacket;
  int DCCBusy;

  DCCPacket = 0x93800000 | (Channel << 16);
  DCCPacket |= Data;
  DCCBusy = _ReadDCCStat() & 2;
  if (DCCBusy == 0) {
    _WriteDCC(DCCPacket);
  } else {
    _WritePendingNB = 1;
    _PendingPacketNB = DCCPacket;
  }
  _ActiveChannel = Channel;
}

/*********************************************************************
*
*       JLINKDCC_SendOnChannel
*
*  Function description
*    Send data to the host.
*    32 channels are available, channel 0 is used for terminal output and is buffered,
*    all other channels are unbuffered.
*/
void JLINKDCC_SendOnChannel(unsigned Channel, U8 * pData, int NumItems) {
  U32 Data;

  if (Channel == DCC_CHANNEL_TERMINAL)  {
    while (NumItems-- > 0) {
      Data = *pData++;
      JLINKDCC_SendChar(Data);
    }
  } else {
    while (NumItems-- > 0) {
      Data = *pData++;
      if  (NumItems > 0) {
        Data |= *pData++ << 8;
        NumItems--;
        Data |= 0x200000;
      }
      Data |= 0x93800000;
      Data |= Channel << 16;
      while (_ReadDCCStat() & 2);       // Wait until we can send data
      _WriteDCC(Data);
    }
  }
}

/*********************************************************************
*
*       JLINKDCC_SetpfOnRx
*
*/
void JLINKDCC_SetpfOnRx(unsigned Channel, void (* pf)(unsigned Channel, U8 Data)) {
  _pfOnRx[Channel] = pf;
}

/*********************************************************************
*
*       JLINKDCC_SetpfOnTx
*
*/
void JLINKDCC_SetpfOnTx(unsigned Channel, void (* pf)(unsigned Channel)) {
  _pfOnTx[Channel] = pf;
}


/*************************** end of file ****************************/
