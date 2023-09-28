#ifndef _ZBS243_H_
#define _ZBS243_H_

#include <stdint.h>

//P2 selects XPAGE which sucks for us
//IRAM is not at all mapped into XRAM
//id page is not in xram either


static __idata __at (0x00) unsigned char R0;
static __idata __at (0x01) unsigned char R1;
static __idata __at (0x02) unsigned char R2;
static __idata __at (0x03) unsigned char R3;
static __idata __at (0x04) unsigned char R4;
static __idata __at (0x05) unsigned char R5;
static __idata __at (0x06) unsigned char R6;
static __idata __at (0x07) unsigned char R7;
__sfr __at (0xf0) B;
__sfr __at (0xe0) ACC;
__sfr __at (0x82) DPL;
__sfr __at (0x83) DPH;
__sfr __at (0x84) DPL1;
__sfr __at (0x85) DPH1;
__sfr __at (0x92) DPS;	//low bit switched DPTR
__sfr __at (0x87) PCON;	//lower 4 bits as per 8051 spec, bit4 set disables xram

__sfr __at (0xb2) PERFMON0;
__sfr __at (0xb3) PERFMON1;
__sfr __at (0xb4) PCH;	//current PC...yes...
__sfr __at (0xb5) PCL;
__sfr __at (0xb6) PERFMON4;



__sbit __at (0x80) P0_0;
__sbit __at (0x81) P0_1;
__sbit __at (0x82) P0_2;
__sbit __at (0x83) P0_3;
__sbit __at (0x84) P0_4;
__sbit __at (0x85) P0_5;
__sbit __at (0x86) P0_6;
__sbit __at (0x87) P0_7;
__sbit __at (0x90) P1_0;
__sbit __at (0x91) P1_1;
__sbit __at (0x92) P1_2;
__sbit __at (0x93) P1_3;
__sbit __at (0x94) P1_4;
__sbit __at (0x95) P1_5;
__sbit __at (0x96) P1_6;
__sbit __at (0x97) P1_7;
__sbit __at (0xa0) P2_0;
__sbit __at (0xa1) P2_1;
__sbit __at (0xa2) P2_2;
__sbit __at (0xa3) P2_3;
__sbit __at (0xa4) P2_4;
__sbit __at (0xa5) P2_5;
__sbit __at (0xa6) P2_6;
__sbit __at (0xa7) P2_7;

__sbit __at (0xA8) IEN_UART0;
__sbit __at (0xA9) IEN_TMR0;
__sbit __at (0xAB) IEN_TMR1;
__sbit __at (0xAC) IEN_RF1;	//enable for RF interrupt #1 (irq #2)
__sbit __at (0xAD) IEN_RF2;	//enable for RF interrupt #2 (irq #5)
__sbit __at (0xAF) IEN_EA;

//gpio configs (all in cfg page 0 only)
__sfr __at (0x80) P0;		//RO for input pins, WO for output pins (written state not read back)
__sfr __at (0x90) P1;
__sfr __at (0xA0) P2;		//disabled ot avoid accidental use
__sfr __at (0xA3) P0LVLSEL;	//pin change interrupt edge desired. 0 - rising 1 - falling
__sfr __at (0xA4) P1LVLSEL;
__sfr __at (0xA5) P2LVLSEL;
__sfr __at (0xA6) P0INTEN;	//pin change interrupt enable (1 = enabled). will wake device from sleep
__sfr __at (0xA7) P1INTEN;
__sfr __at (0xA9) P2INTEN;
__sfr __at (0xAA) P0CHSTA;	//pin change status 1 = changed (remember to clear in ISR & before enabling ISR)
__sfr __at (0xAB) P1CHSTA;
__sfr __at (0xAC) P2CHSTA;
__sfr __at (0xAD) P0FUNC;	//1 = func, 0 = gpio
__sfr __at (0xAE) P1FUNC;
__sfr __at (0xAF) P2FUNC;
__sfr __at (0xB9) P0DIR;	//1 = in, 0 = out
__sfr __at (0xBA) P1DIR;
__sfr __at (0xBB) P2DIR;
__sfr __at (0xBC) P0PULL;	//1 = pull up
__sfr __at (0xBD) P1PULL;
__sfr __at (0xBE) P2PULL;

__sfr __at (0xA0) XPAGE;	//sadly this is accurate! same as P2

//these are both avail in both config pages
__sfr __at (0xA8) IEN0;
__sfr __at (0xA1) IEN1;		//low 1, bits unknown, top 7 bits eable irqs (7..13)


__sfr __at (0x8E) CLKSPEED;	//top bit likely selects osc (1 = crystal??), next 3 set divisor FOR CPU, not periphs, bottom 4 unknown

__sfr __at (0xB7) CLKEN;	//one bit per periph to enable clocks. 0x08 - spi, 0x10 - i2c, 0x20 - uart, 0x80 temp sensor, 0x01 - in-cpu-timers(T0 & T1)

__sfr __at (0x8F) RESET;	//resets periphs AND stores reset cause.  bit 0x01 - we were reset due to WDT, 0x04 - resets radio (Active high), 0x02 resets cpu core

//wdt (accessible in CFG page 4), runs at about 62KHz. counter counts up, overflow causes reset
__sfr __at (0xBA) WDTENA;		//low bit is used for WDT off. same SFR addr used as P1DIR when cfg page 0
__sfr __at (0xBB) WDTPET;		//write 0 to pet, write 0xff to reset immediately. same SFR addr used as P2DIR when cfg page 0
__sfr __at (0xBC) WDTRSTVALL;	//wdt reset value low
__sfr __at (0xBD) WDTRSTVALM;	//wdt reset value mid
__sfr __at (0xBE) WDTRSTVALH;	//wdt reset value high
__sfr __at (0xBF) WDTCONF;		//top bit enable, 0x08 to hang instead of reset. top bit only accessible in CFG page 4


//SPI (accessible in CFG page 4)
__sfr __at (0xEB) SPIUNKNOWN;	//existing code sets 0x08 after a byte tx. no idea why. not doing it works
__sfr __at (0xEC) SPICFG;		//0x80 - 1, 0x40 - 0, 0x20 - do TX (cleared on done), {0x10,0x08}) - clock div {4mhz,2mhz,1mhz,500khz}, 0x04 - CPHA, 0x02 - CPOL, 0x01 ?? (breaks rx, maybe bidirectional or rx off)
__sfr __at (0xED) SPIENA;		//0x80 - 1, 0x01 - unit enable. stock code also enables/disables after every byte but i do not think we need to
__sfr __at (0xEE) SPITX;		//tx buffer
__sfr __at (0xEF) SPIRX;		//rx buffer

//I2C (accessible in CFG page 0)
//it is in theory possible to drive it without IRQs but there seem to be hw bugs in figuring out when handling is needed, so i wouldn't. stock also doesn't
__sfr __at (0x91) I2CSTATE;		//top 5 bits are current state: {???, START_SENT, RESTART_SENT, WR_ADDR_ACKED, WR_ADDR_NAKED, TX_BYTE_ACKED, TX_BYTE_NAKED, unused, RD_ADDR_ACKED, RD_ADDR_NAKED, RXED_BYTE}
//0x93 written by 0xe0 by stock but seems not needed
__sfr __at (0x94) I2CBUF;		//data for RX/TX
__sfr __at (0x95) I2CCTL;		//0x80 = irqOn (#8), 0x40 = master(?), 0x20 = start/restart, 0x10 = stop, 0x08 = irqReqd, 0x04 = sendAck (to RXed btes)
__sfr __at (0x96) I2CSPEED;		//bottom 3 bits = dividerA. next 4 are dividerB. clock = 16MHz / ((dividerB ? 10 *(1 + dividerB) : 12) << dividerA)
__sfr __at (0xA2) I2CUNKNOWN;	//4 ORRED at init or it will not work


//uart is basically hard wired to be a uart. clock is divided from 16MHz. only bottom 4 bits of BRGH are used. divisor is (BRGH:BRGL + 1)
__sbit __at (0x98) UART_RXF;
__sbit __at (0x99) UART_TXE;
__sfr __at (0x98) UARTSTA;	//0x10 needed. 0x02 - have tx space, 0x01 - have rx byte. both can cause interrupts. both need to be cleared in irq. flag bits do NOT work if accessed using "jbc"
__sfr __at (0x99) UARTBUF;	//fifo access
__sfr __at (0x9A) UARTBRGL;
__sfr __at (0x9B) UARTBRGH;	//divisor for uart

//timers (same as 8051-classic)
__sfr __at (0x88) TCON;
__sfr __at (0x89) TMOD;
__sfr __at (0x8A) TL0;
__sfr __at (0x8B) TL1;
__sfr __at (0x8C) TH0;
__sfr __at (0x8D) TH1;
__sfr16 __at (0x8C8A) T0;
__sfr16 __at (0x8D8B) T1;	//used by timer code for storage

//flash writing (must be done with CFGPAGE == 4). each page is 0x400 bytes
__sfr __at (0xD8) FPGNO;	//page number (0x00..0x3f for main flash, 0x80 for info block)
__sfr __at (0xD9) FWRSRCL;	//(flash source data for write).lo for write, (flash start offset in page).lo for read
__sfr __at (0xDA) FWRSRCH;	//(flash source data for write).hi for write, (flash start offset in page).hi for read
__sfr __at (0xDB) FWRDSTL;	//(flash start offset in page).lo for write, (dest xram addr).lo for read
__sfr __at (0xDC) FWRDSTH;	//(flash start offset in page).hi for write, (dest xram addr).hi for read
__sfr __at (0xDD) FWRLENL;	//(num bytes to write minus 1).lo
__sfr __at (0xDE) FWRLENH;	//(num bytes to write minus 1).hi
__sfr __at (0xDF) FWRTHREE;	//always written to three


//temp sensor (CLKEN bit 0x80). these only accessible in config page 4
__sfr __at (0xE6) TEMPCAL1;	//purpose guessed
__sfr __at (0xE7) TEMPCAL2;	//purpose guessed

__sfr __at (0xF7) TEMPCFG;	//purpose guessed

__sfr __at (0xF8) TEMPRETH;	//has high 8 bits of result. bitswapped
__sfr __at (0xF9) TEMPRETL;	//has low two bits of result in lower 2 bits. bitswapped

__sfr __at (0xFB) TEMPCAL3;	//purpose guessed
__sfr __at (0xFC) TEMPCAL4;	//purpose guessed
__sfr __at (0xFD) TEMPCAL5;	//purpose guessed
__sfr __at (0xFE) TEMPCAL6;	//purpose guessed

//radio regs (cfg page 4)
__sfr __at (0xC9) RADIO_TXPTRL;	//xram pointer to tx buffer's "length" byte. low byte
__sfr __at (0xCA) RADIO_TXPTRH;	//xram pointer to tx buffer's "length" byte. high byte
__sfr __at (0xCB) RADIO_INITSEQ1;
__sfr __at (0xCC) RADIO_INITSEQ2;
__sfr __at (0xCD) RADIO_TXLEN;	//data bytes to send (not incl "len" byte or crc)
__sfr __at (0xCE) RADIO_INITSEQ0;
__sfr __at (0xD1) RADIO_INITSEQ4;
__sfr __at (0xD2) RADIO_INITSEQ5;
__sfr __at (0xD3) RADIO_RXPTRL;	//xram pointer to tx buffer's "length" byte. low byte
__sfr __at (0xD4) RADIO_RXPTRH;	//xram pointer to tx buffer's "length" byte. high byte
__sfr __at (0xD5) RADIO_RXLEN;	//data bytes to RX (not incl "len" byte or crc) 2 more than this will be RXed
__sfr __at (0xD6) RADIO_INITSEQ3;

//radio regs (any cfg page)
__sfr __at (0xFA) RADIO_GOTLEN;	//size packet we GOT (incl len byte)



//shared regs
__sfr __at (0xC1) UNK_C1;	//in cfg page 4 only, purpose unknown

__sfr __at (0xD7) TRIGGER;	//in both cfg page, seach bit starts something. 0x08 starts flash op, 0x04 - start radio rx fifo dma, 0x02 - start radio tx fifo dma


//in cfg page 4
__sfr __at (0xC7) SETTINGS;	//misc settings incl flasg access. 0x18 is flash write, 0x08 is flash read, 
__sfr __at (0xCF) TCON2;	//interrupt flags (0x08 is flash write)



__sfr __at (0xFF) CFGPAGE;	//partially swaps SFR set to another. only 0x04 bit is used


/*
	84 85 92 B2 B3 B4 B5 B6 B8 are visible on both pages
	
	93 98 9D 9E 9F B9 C1-C7 E0-E9 are swapped
	
	86 9C B0 B1 C0 C8 EA F1-F6 ??????

*/



//radio MMIO

static __xdata __at (0xdf48) unsigned char RADIO_command;
static __xdata __at (0xdf70) unsigned char RADIO_calibration_70;
static __xdata __at (0xdf71) unsigned char RADIO_calibration_71;
static __xdata __at (0xdf80) unsigned char RADIO_FLAGS;
static __xdata __at (0xdf81) unsigned char RADIO_calibration_81;
static __xdata __at (0xdf83) unsigned char RADIO_unk_83;
static __xdata __at (0xdf84) unsigned char RADIO_currentRSSI;
static __xdata __at (0xdf86) unsigned char RADIO_calibration_86;
static __xdata __at (0xdf88) unsigned char RADIO_ownMac_7;
static __xdata __at (0xdf89) unsigned char RADIO_ownMac_6;
static __xdata __at (0xdf8a) unsigned char RADIO_ownMac_5;
static __xdata __at (0xdf8b) unsigned char RADIO_ownMac_4;
static __xdata __at (0xdf8c) unsigned char RADIO_ownMac_3;
static __xdata __at (0xdf8d) unsigned char RADIO_ownMac_2;
static __xdata __at (0xdf8e) unsigned char RADIO_ownMac_1;
static __xdata __at (0xdf8f) unsigned char RADIO_ownMac_0;
static __xdata __at (0xdf90) unsigned char RADIO_PANID_Hi;
static __xdata __at (0xdf91) unsigned char RADIO_PANID_Lo;
static __xdata __at (0xdf92) unsigned char RADIO_ownShortAddress_Hi;
static __xdata __at (0xdf93) unsigned char RADIO_ownShortAddress_Lo;
static __xdata __at (0xdf94) unsigned char RADIO_calibration_94;
static __xdata __at (0xdf95) unsigned char RADIO_calibration_95;
static __xdata __at (0xdf96) unsigned char RADIO_calibration_96;
static __xdata __at (0xdf98) unsigned char RADIO_rxFirstByte;		// first byte of the message actualyl RXed
static __xdata __at (0xdf9b) unsigned char RADIO_curRfState;		// RFSTATUS ?
static __xdata __at (0xdf9d) unsigned char RADIO_calibration_9D;
static __xdata __at (0xdfa1) unsigned char RADIO_calibration_A1;
static __xdata __at (0xdfa5) unsigned char RADIO_unk_rxAckSta;	//in irqh4, when RADIO_IRQ4_pending has 0x40 set, this is 0x04 if we got ack, and 0x02 if not
static __xdata __at (0xdfa6) unsigned char RADIO_unk_A6;
static __xdata __at (0xdfad) unsigned char RADIO_IRQ4_pending;
static __xdata __at (0xdfaf) unsigned char RADIO_unk_AF;
static __xdata __at (0xdfc0) unsigned char RADIO_channel;
static __xdata __at (0xdfc1) unsigned char RADIO_unk_C1;
static __xdata __at (0xdfc2) unsigned char RADIO_calibration_C2;
static __xdata __at (0xdfc3) unsigned char RADIO_calibration_C3;
static __xdata __at (0xdfc4) unsigned char RADIO_calibration_C4;
static __xdata __at (0xdfc5) unsigned char RADIO_calibration_C5;
static __xdata __at (0xdfc6) unsigned char RADIO_calibration_C6;
static __xdata __at (0xdfc7) unsigned char RADIO_calibration_C7;
static __xdata __at (0xdfc8) unsigned char RADIO_unk_C8;
static __xdata __at (0xdfc9) unsigned char RADIO_txPower;	// 0..7 for -5..+8 dBm
static __xdata __at (0xdfca) unsigned char RADIO_unk_CA;
static __xdata __at (0xdfcb) unsigned char RADIO_perChannelSetting2;	//relevant for tx
static __xdata __at (0xdfcd) unsigned char RADIO_unk_CD;
static __xdata __at (0xdfce) unsigned char RADIO_unk_CE;
static __xdata __at (0xdfcf) unsigned char RADIO_calibration_CF;
static __xdata __at (0xdfd0) unsigned char RADIO_calibration_D0;
static __xdata __at (0xdfd1) unsigned char RADIO_calibration_D1;
static __xdata __at (0xdfd2) unsigned char RADIO_calibration_D2;
static __xdata __at (0xdfd3) unsigned char RADIO_calibration_D3;
static __xdata __at (0xdfd4) unsigned char RADIO_calibration_D4;
static __xdata __at (0xdfd5) unsigned char RADIO_calibration_D5;
static __xdata __at (0xdfd7) unsigned char RADIO_unk_D7;
static __xdata __at (0xdfd8) unsigned char RADIO_unk_D8;
static __xdata __at (0xdfd9) unsigned char RADIO_SleepTimerHi;	//only bottom 4 bits implemented, counts down to wake
static __xdata __at (0xdfda) unsigned char RADIO_SleepTimerMid;
static __xdata __at (0xdfdb) unsigned char RADIO_SleepTimerLo;
static __xdata __at (0xdfe2) unsigned char RADIO_unk_E2;
static __xdata __at (0xdff0) unsigned char RADIO_unk_F0;
static __xdata __at (0xdff3) unsigned char RADIO_SleepTimerSettings;	//0x16 for one second tick, 0x56 for 1/32k second tick
static __xdata __at (0xdff4) unsigned char RADIO_RadioPowerCtl;
static __xdata __at (0xdffb) unsigned char RADIO_Wake_Reason; // 0x04 ext interrupt, 0x08 timer, 0x02 RF wake
static __xdata __at (0xdffd) unsigned char RADIO_perChannelSetting1;	//relevant fo rRX

#define RADIO_CMD_RECEIVE			0xc2	//tx always goes to RX anyways
#define RADIO_CMD_FLUSH_RX_FIFO		0xc6
#define RADIO_CMD_FLUSH_TX_FIFO		0xc7
#define RADIO_CMD_LOAD_TX_FIFO		0xcb


#define RADIO_WAKE_REASON_RF 0x02
#define RADIO_WAKE_REASON_EXT 0x04
#define RADIO_WAKE_REASON_TIMER 0x08


#endif
