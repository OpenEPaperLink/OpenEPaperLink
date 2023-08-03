/*-------------------------------------------------------------------------
   at89c51snd1c.h - Register Declarations for the Atmel AT89C51SND1C Processor

   Copyright (C) 2005, Weston Schmidt <weston_schmidt@alumni.purdue.edu>

   This document is based on the AT8xC51SND1C document
       4109H-8051-01/05

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


#ifndef __AT89C51SND1_H__
#define __AT89C51SND1_H__


/*  BYTE Registers */
__sfr __at (0xE0) ACC     ; /* C51 Core SFRs */
__sfr __at (0xF0) B       ;
__sfr __at (0xD0) PSW     ;
__sfr __at (0x81) SP      ;
__sfr __at (0x82) DPL     ;
__sfr __at (0x83) DPH     ;
__sfr __at (0x87) PCON    ; /* System Management SFRs */
__sfr __at (0x8E) AUXR0   ;
__sfr __at (0xA2) AUXR1   ;
__sfr __at (0xFB) NVERS   ;
__sfr __at (0x8F) CKCON   ; /* PLL and System Clock SFRs */
__sfr __at (0xE9) PLLCON  ;
__sfr __at (0xEE) PLLNDIV ;
__sfr __at (0xEF) PLLRDIV ;
__sfr __at (0xA8) IEN0    ; /* Interrupt SFRs */
__sfr __at (0xB1) IEN1    ;
__sfr __at (0xB7) IPH0    ;
__sfr __at (0xB8) IPL0    ;
__sfr __at (0xB3) IPH1    ;
__sfr __at (0xB2) IPL1    ;
__sfr __at (0x80) P0      ; /* Port SFRs */
__sfr __at (0x90) P1      ;
__sfr __at (0xA0) P2      ;
__sfr __at (0xB0) P3      ;
__sfr __at (0xC0) P4      ;
__sfr __at (0xD8) P5      ;
__sfr __at (0xD1) FCON    ; /* Flash Memory SFR */
__sfr __at (0x88) TCON    ; /* Timer SFRs */
__sfr __at (0x89) TMOD    ;
__sfr __at (0x8A) TL0     ;
__sfr __at (0x8C) TH0     ;
__sfr __at (0x8B) TL1     ;
__sfr __at (0x8D) TH1     ;
__sfr __at (0xA6) WDTRST  ;
__sfr __at (0xA7) WDTPRG  ;
__sfr __at (0xAA) MP3CON  ; /* MP3 Decoder SFRs */
__sfr __at (0xC8) MP3STA  ;
__sfr __at (0xAF) MP3STA1 ;
__sfr __at (0xAC) MP3DAT  ;
__sfr __at (0xAD) MP3ANC  ;
__sfr __at (0x9E) MP3VOL  ;
__sfr __at (0x9F) MP3VOR  ;
__sfr __at (0xB4) MP3BAS  ;
__sfr __at (0xB5) MP3MED  ;
__sfr __at (0xB6) MP3TRE  ;
__sfr __at (0xEB) MP3CLK  ;
__sfr __at (0xAE) MP3DBG  ;
__sfr __at (0x9A) AUDCON0 ; /* Audio Interface SFRs */
__sfr __at (0x9B) AUDCON1 ;
__sfr __at (0x9C) AUDSTA  ;
__sfr __at (0x9D) AUDDAT  ;
__sfr __at (0xEC) AUDCLK  ;
__sfr __at (0xBC) USBCON  ; /* USB Controller SFRs */
__sfr __at (0xC6) USBADDR ;
__sfr __at (0xBD) USBINT  ;
__sfr __at (0xBE) USBIEN  ;
__sfr __at (0xC7) UEPNUM  ;
__sfr __at (0xD4) UEPCONX ;
__sfr __at (0xCE) UEPSTAX ;
__sfr __at (0xD5) UEPRST  ;
__sfr __at (0xF8) UEPINT  ;
__sfr __at (0xC2) UEPIEN  ;
__sfr __at (0xCF) UEPDATX ;
__sfr __at (0xE2) UBYCTX  ;
__sfr __at (0xBA) UFNUML  ;
__sfr __at (0xBB) UFNUMH  ;
__sfr __at (0xEA) USBCLK  ;
__sfr __at (0xE4) MMCON0  ; /* MMC Controller SFRs */
__sfr __at (0xE5) MMCON1  ;
__sfr __at (0xE6) MMCON2  ;
__sfr __at (0xDE) MMSTA   ;
__sfr __at (0xE7) MMINT   ;
__sfr __at (0xDF) MMMSK   ;
__sfr __at (0xDD) MMCMD   ;
__sfr __at (0xDC) MMDAT   ;
__sfr __at (0xED) MMCLK   ;
__sfr __at (0xF9) DAT16H  ; /* IDE Interface SFR */
__sfr __at (0x98) SCON    ; /* Serial I/O Port SFRs */
__sfr __at (0x99) SBUF    ;
__sfr __at (0xB9) SADEN   ;
__sfr __at (0xA9) SADDR   ;
__sfr __at (0x92) BDRCON  ;
__sfr __at (0x91) BRL     ;
__sfr __at (0xC3) SPCON   ; /* SPI Controller SFRs */
__sfr __at (0xC4) SPSTA   ;
__sfr __at (0xC5) SPDAT   ;
__sfr __at (0x93) SSCON   ; /* Two Wire Controller SFRs */
__sfr __at (0x94) SSSTA   ;
__sfr __at (0x95) SSDAT   ;
__sfr __at (0x96) SSADR   ;
__sfr __at (0xA3) KBCON   ; /* Keyboard Interface SFRs */
__sfr __at (0xA4) KBSTA   ;
__sfr __at (0xF3) ADCON   ; /* A/D Controller SFRs */
__sfr __at (0xF4) ADDL    ;
__sfr __at (0xF5) ADDH    ;
__sfr __at (0xF2) ADCLK   ;


/*  BIT Registers */
/*  PSW */
__sbit __at (0xD7) CY     ;
__sbit __at (0xD6) AC     ;
__sbit __at (0xD5) F0     ;
__sbit __at (0xD4) RS1    ;
__sbit __at (0xD3) RS0    ;
__sbit __at (0xD2) OV     ;
__sbit __at (0xD1) F1     ;
__sbit __at (0xD0) P      ;

/*  IEN0    */
__sbit __at (0xAF) EA     ;
__sbit __at (0xAE) EAUD   ;
__sbit __at (0xAD) EMP3   ;
__sbit __at (0xAC) ES     ;
__sbit __at (0xAB) ET1    ;
__sbit __at (0xAA) EX1    ;
__sbit __at (0xA9) ET0    ;
__sbit __at (0xA8) EX0    ;

/*  IPLO */
__sbit __at (0xBE) IPLAUD ;
__sbit __at (0xBD) IPLMP3 ;
__sbit __at (0xBC) IPLS   ;
__sbit __at (0xBB) IPLT1  ;
__sbit __at (0xBA) IPLX1  ;
__sbit __at (0xB9) IPLT0  ;
__sbit __at (0xB8) IPLX0  ;

/*  P0  */
__sbit __at (0x87) P0_7   ;
__sbit __at (0x86) P0_6   ;
__sbit __at (0x85) P0_5   ;
__sbit __at (0x84) P0_4   ;
__sbit __at (0x83) P0_3   ;
__sbit __at (0x82) P0_2   ;
__sbit __at (0x81) P0_1   ;
__sbit __at (0x80) P0_0   ;

/*  P1  */
__sbit __at (0x97) P1_7   ;
__sbit __at (0x96) P1_6   ;
__sbit __at (0x95) P1_5   ;
__sbit __at (0x94) P1_4   ;
__sbit __at (0x93) P1_3   ;
__sbit __at (0x92) P1_2   ;
__sbit __at (0x91) P1_1   ;
__sbit __at (0x90) P1_0   ;

__sbit __at (0x97) SDA    ;
__sbit __at (0x96) SCL    ;
__sbit __at (0x93) KIN3   ;
__sbit __at (0x92) KIN2   ;
__sbit __at (0x91) KIN1   ;
__sbit __at (0x90) KIN0   ;

/*  P2  */
__sbit __at (0xA7) P2_7   ;
__sbit __at (0xA6) P2_6   ;
__sbit __at (0xA5) P2_5   ;
__sbit __at (0xA4) P2_4   ;
__sbit __at (0xA3) P2_3   ;
__sbit __at (0xA2) P2_2   ;
__sbit __at (0xA1) P2_1   ;
__sbit __at (0xA0) P2_0   ;

/*  P3  */
__sbit __at (0xB7) P3_7   ;
__sbit __at (0xB6) P3_6   ;
__sbit __at (0xB5) P3_5   ;
__sbit __at (0xB4) P3_4   ;
__sbit __at (0xB3) P3_3   ;
__sbit __at (0xB2) P3_2   ;
__sbit __at (0xB1) P3_1   ;
__sbit __at (0xB0) P3_0   ;

__sbit __at (0xB7) RD     ;
__sbit __at (0xB6) WR     ;
__sbit __at (0xB5) T1     ;
__sbit __at (0xB4) T0     ;
__sbit __at (0xB3) INT1   ;
__sbit __at (0xB2) INT0   ;
__sbit __at (0xB1) TXD    ;
__sbit __at (0xB0) RXD    ;

/*  P4  */
__sbit __at (0xC7) P4_7   ;
__sbit __at (0xC6) P4_6   ;
__sbit __at (0xC5) P4_5   ;
__sbit __at (0xC4) P4_4   ;
__sbit __at (0xC3) P4_3   ;
__sbit __at (0xC2) P4_2   ;
__sbit __at (0xC1) P4_1   ;
__sbit __at (0xC0) P4_0   ;

__sbit __at (0xC3) SS_    ;
__sbit __at (0xC2) SCK    ;
__sbit __at (0xC1) MOSI   ;
__sbit __at (0xC0) MISO   ;

/*  P5  */
__sbit __at (0xDB) P5_3   ;
__sbit __at (0xDA) P5_2   ;
__sbit __at (0xD9) P5_1   ;
__sbit __at (0xD8) P5_0   ;

/*  TCON    */
__sbit __at (0x8F) TF1    ;
__sbit __at (0x8E) TR1    ;
__sbit __at (0x8D) TF0    ;
__sbit __at (0x8C) TR0    ;
__sbit __at (0x8B) IE1    ;
__sbit __at (0x8A) IT1    ;
__sbit __at (0x89) IE0    ;
__sbit __at (0x88) IT0    ;

/*  MP3STA  */
__sbit __at (0xCF) MPANC  ;
__sbit __at (0xCE) MPREQ  ;
__sbit __at (0xCD) ERRLAY ;
__sbit __at (0xCC) ERRSYN ;
__sbit __at (0xCB) ERRCRC ;
__sbit __at (0xCA) MPFS1  ;
__sbit __at (0xC9) MPFS0  ;
__sbit __at (0xC8) MPVER  ;

/*  UEPINT  */
__sbit __at (0xFA) EP2INT ;
__sbit __at (0xF9) EP1INT ;
__sbit __at (0xF8) EP0INT ;

/*  SCON  */
__sbit __at (0x9F) SM0    ;
__sbit __at (0x9F) FE     ;
__sbit __at (0x9E) SM1    ;
__sbit __at (0x9D) SM2    ;
__sbit __at (0x9C) REN    ;
__sbit __at (0x9B) TB8    ;
__sbit __at (0x9A) RB8    ;
__sbit __at (0x99) TI     ;
__sbit __at (0x98) RI     ;


/* BIT definitions for bits that are not directly accessible */
/* PCON bits */
#define MSK_SMOD1   0x80
#define MSK_SMOD0   0x40
#define MSK_GF1     0x08
#define MSK_GF0     0x04
#define MSK_PD      0x02
#define MSK_IDL     0x01

/* AUXR0 bits */
#define MSK_EXT16   0x40
#define MSK_M0      0x20
#define MSK_DPHDIS  0x10
#define MSK_XRS     0x0C
#define MSK_EXTRAM  0x02
#define MSK_AO      0x01

/* AUXR1 bits */
#define MSK_ENBOOT  0x20
#define MSK_GF3     0x08
#define MSK_DPS     0x01

/* CKCON bits */
#define MSK_X2      0x01

/* PLLCON bits */
#define MSK_PLL_R   0xC0
#define MSK_PLLRES  0x08
#define MSK_PLLEN   0x02
#define MSK_PLOCK   0x01

/* PLLNDIV bits */
#define MSK_PLL_N   0x7F

/* IEN1 bits */
#define MSK_EUSB    0x40
#define MSK_EKB     0x10
#define MSK_EADC    0x08
#define MSK_ESPI    0x04
#define MSK_EI2C    0x02
#define MSK_EMMC    0x01

/* IPHO bits */
#define MSK_IPHAUD  0x40
#define MSK_IPHMP3  0x20
#define MSK_IPHS    0x10
#define MSK_IPHT1   0x08
#define MSK_IPHX1   0x04
#define MSK_IPHT0   0x02
#define MSK_IPHX0   0x01

/* IPH1 bits */
#define MSK_IPHUSB  0x40
#define MSK_IPHKB   0x10
#define MSK_IPHADC  0x08
#define MSK_IPHSPI  0x04
#define MSK_IPHI2C  0x02
#define MSK_IPHMMC  0x01

/* IPL1 bits */
#define MSK_IPLUSB  0x40
#define MSK_IPLKB   0x10
#define MSK_IPLADC  0x08
#define MSK_IPLSPI  0x04
#define MSK_IPLI2C  0x02
#define MSK_IPLMMC  0x01

/* TMOD bits */
#define MSK_GATE1   0x80
#define MSK_C_T1    0x40
#define MSK_MO1     0x30
#define MSK_GATE0   0x08
#define MSK_C_T0    0x04
#define MSK_MO0     0x03

/* MP3CON bits */
#define MSK_MPEN    0x80
#define MSK_MPBBST  0x40
#define MSK_CRCEN   0x20
#define MSK_MSKANC  0x10
#define MSK_MSKREQ  0x08
#define MSK_MSKLAY  0x04
#define MSK_MSKSYN  0x02
#define MSK_MSKCRC  0x01

/* MP3STA1 bits */
#define MSK_MPFREQ  0x10
#define MSK_MPBREQ  0x08

/* MP3VOL bits */
#define MSK_VOL     0x1F

/* MP3VOR bits */
#define MSK_VOR     0x1F

/* MP3BAS bits */
#define MSK_BAS     0x1F

/* MP3MED bits */
#define MSK_MED     0x1F

/* MP3TRE bits */
#define MSK_TRE     0x1F

/* MP3CLK bits */
#define MSK_MPCD    0x1F

/* MP3DBG bits */
#define MSK_MPFULL  0x08

/* AUDCON0 bits */
#define MSK_JUST    0xF8
#define MSK_POL     0x04
#define MSK_DSIZ    0x02
#define MSK_HLR     0x01

/* AUDCON1 bits */
#define MSK_SRC     0x80
#define MSK_DRQEN   0x40
#define MSK_MSREQ   0x20
#define MSK_MUDRN   0x10
#define MSK_DUP     0x06
#define MSK_AUDEN   0x01

/* AUDSTA bits */
#define MSK_SREQ    0x80
#define MSK_UDRN    0x40
#define MSK_AUBUSY  0x20

/* AUDCLK bits */
#define MSK_AUCD    0x1F

/* USBCON bits */
#define MSK_USBE    0x80
#define MSK_SUSPCLK 0x40
#define MSK_SDRMWUP 0x20
#define MSK_UPRSM   0x08
#define MSK_RMWUPE  0x04
#define MSK_CONFG   0x02
#define MSK_FADDEN  0x01

/* USBADDR bits */
#define MSK_FEN     0x80
#define MSK_UADD    0x7F

/* USBINT bits */
#define MSK_WUPCPU  0x20
#define MSK_EORINT  0x10
#define MSK_SOFINT  0x08
#define MSK_SPINT   0x01

/* USBIEN bits */
#define MSK_EWUPCPU 0x20
#define MSK_EEORINT 0x10
#define MSK_ESOFINT 0x08
#define MSK_ESPINT  0x01

/* UEPNUM bits */
#define MSK_EPNUM   0x03

/* UEPCONX bits */
#define MSK_EPEN    0x80
#define MSK_NAKIEN  0x40
#define MSK_NAKOUT  0x20
#define MSK_NAKIN   0x10
#define MSK_DTGL    0x08
#define MSK_EPDIR   0x04
#define MSK_EPTYPE  0x03

/* UEPSTAX bits */
#define MSK_DIR     0x80
#define MSK_RXOUTB1 0x40
#define MSK_STALLRQ 0x20
#define MSK_TXRDY   0x10
#define MSK_STLCRC  0x08
#define MSK_RXSETUP 0x04
#define MSK_RXOUTB0 0x02
#define MSK_TXCMP   0x01

/* UEPRST bits */
#define MSK_EPRST   0x07
#define MSK_EP2RST  0x04
#define MSK_EP1RST  0x02
#define MSK_EP0RST  0x01

#define MSK_EPINT   0x07
#define MSK_EP2INT  0x04
#define MSK_EP1INT  0x02
#define MSK_EP0INT  0x01

/* UEPIEN bits */
#define MSK_EPINTE  0x07
#define MSK_EP2INTE 0x04
#define MSK_EP1INTE 0x02
#define MSK_EP0INTE 0x01

/* UBYCTX bits */
#define MSK_BYCT    0x7F

/* UFNUMH bits */
#define MSK_CRCOK   0x20
#define MSK_CRCERR  0x10
#define MSK_FNUM    0x07

/* USBCLK bits */
#define MSK_USBCD   0x03

/* MMCON0 bits */
#define MSK_DRPTR   0x80
#define MSK_DTPTR   0x40
#define MSK_CRPTR   0x20
#define MSK_CTPTR   0x10
#define MSK_MBLOCK  0x08
#define MSK_DFMT    0x04
#define MSK_RFMT    0x02
#define MSK_CRCDIS  0x01

/* MMCON1 bits */
#define MSK_BLEN    0xf0
#define MSK_DATDIR  0x08
#define MSK_DATEN   0x04
#define MSK_RESPEN  0x02
#define MSK_CMDEN   0x01

/* MMCON2 bits */
#define MSK_MMCEN   0x80
#define MSK_DCR     0x40
#define MSK_CCR     0x20
#define MSK_DATD    0x06
#define MSK_FLOWC   0x01

/* MMSTA bits */
#define MSK_CBUSY   0x20
#define MSK_CRC16S  0x10
#define MSK_DATFS   0x08
#define MSK_CRC7S   0x04
#define MSK_RESPFS  0x02
#define MSK_CFLCK   0x01

/* MMINT bits */
#define MSK_MCBI    0x80
#define MSK_EORI    0x40
#define MSK_EOCI    0x20
#define MSK_EOFI    0x10
#define MSK_F2FI    0x08
#define MSK_F1FI    0x04
#define MSK_F2EI    0x02
#define MSK_F1EI    0x01

/* MMMSK bits */
#define MSK_MCBM    0x80
#define MSK_EORM    0x40
#define MSK_EOCM    0x20
#define MSK_EOFM    0x10
#define MSK_F2FM    0x08
#define MSK_F1FM    0x04
#define MSK_F2EM    0x02
#define MSK_F1EM    0x01

/* BDRCON bits */
#define MSK_BRR     0x10
#define MSK_TBCK    0x08
#define MSK_RBCK    0x04
#define MSK_SPD     0x02
#define MSK_M0SRC   0x01

/* SPCON bits */
#define MSK_SPR     0x83
#define MSK_SPEN    0x40
#define MSK_SSDIS   0x20
#define MSK_MSTR    0x10
#define MSK_MODE    0x0C
#define MSK_CPOL    0x08
#define MSK_CPHA    0x04

/* SPSTA bits */
#define MSK_SPIF    0x80
#define MSK_WCOL    0x40
#define MSK_MODF    0x10

/* SSCON bits */
#define MSK_SSCR    0x83
#define MSK_SSPE    0x40
#define MSK_SSSTA   0x20
#define MSK_SSSTO   0x10
#define MSK_SSI     0x08
#define MSK_SSAA    0x04

/* SSSTA bits */
#define MSK_SSC     0xf8

/* SSADR bits */
#define MSK_SSA     0xfe
#define MSK_SSGC    0x01

/* KBCON bits */
#define MSK_KINL    0xf0
#define MSK_KINM    0x0f

/* BKSTA bits */
#define MSK_KPDE    0x80
#define MSK_KINF    0x0f

/* ADCON bits */
#define MSK_ADIDL   0x40
#define MSK_ADEN    0x20
#define MSK_ADEOC   0x10
#define MSK_ADSST   0x80
#define MSK_ADCS    0x01

/* ADCLK bits */
#define MSK_ADCD    0x1f

/* ADDL bits */
#define MSK_ADAT    0x03

/* Interrupt numbers: address = (number * 8) + 3 */
#define IE0_VECTOR      0   /* 0x03 External Interrupt 0 */
#define TF0_VECTOR      1   /* 0x0b Timer 0 */
#define IE1_VECTOR      2   /* 0x13 External Interrupt 1 */
#define TF1_VECTOR      3   /* 0x1b Timer 1 */
#define SIO_VECTOR      4   /* 0x23 Serial port */
#define MP3_VECTOR      5   /* 0x2b MP3 Decoder */
#define AUDIO_VECTOR    6   /* 0x33 Audio Interface */
#define MMC_VECTOR      7   /* 0x3b MMC Interface */
#define TWI_VECTOR      8   /* 0x43 Two Wire Controller */
#define SPI_VECTOR      9   /* 0x4b SPI Controller */
#define ADC_VECTOR      10  /* 0x53 A to D Contverter */
#define KBD_VECTOR      11  /* 0x5b Keyboard */
                            /* 0x63 Reserved */
#define USB_VECTOR      13  /* 0x6b USB */
                            /* 0x73 Reserved */

#endif
