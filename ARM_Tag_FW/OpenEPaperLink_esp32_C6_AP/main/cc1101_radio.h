// Large portions of this code was copied from:
//  https://github.com/nopnop2002/esp-idf-cc1101 with the following copyright

/*
 * Copyright (c) 2011 panStamp <contact@panstamp.com>
 * Copyright (c) 2016 Tyler Sommer <contact@tylersommer.pro>
 * 
 * This file is part of the CC1101 project.
 * 
 * CC1101 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * any later version.
 * 
 * CC1101 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with CC1101; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA   02110-1301 
 * USA
 * 
 * Author: Daniel Berenguer
 * Creation date: 03/03/2011
 */

#ifndef __CC1101_RADIO_H_
#define __CC1101_RADIO_H_

// Log to all
#define LOGA(format, ... ) \
   uart_printf(format "\r",## __VA_ARGS__)

// Error log to all
#define LOGE(format, ... ) \
   uart_printf("%s#%d: " format "\r",__FUNCTION__,__LINE__,## __VA_ARGS__)

/**
 * CC1101 configuration registers
 */
#define CC1101_IOCFG2      0x00  // GDO2 Output Pin Configuration
#define CC1101_IOCFG1      0x01  // GDO1 Output Pin Configuration
#define CC1101_IOCFG0      0x02  // GDO0 Output Pin Configuration
#define CC1101_FIFOTHR     0x03  // RX FIFO and TX FIFO Thresholds
#define CC1101_SYNC1       0x04  // Sync Word, High Byte
#define CC1101_SYNC0       0x05  // Sync Word, Low Byte
#define CC1101_PKTLEN      0x06  // Packet Length
#define CC1101_PKTCTRL1    0x07  // Packet Automation Control
#define CC1101_PKTCTRL0    0x08  // Packet Automation Control
#define CC1101_ADDR        0x09  // Device Address
#define CC1101_CHANNR      0x0A  // Channel Number
#define CC1101_FSCTRL1     0x0B  // Frequency Synthesizer Control
#define CC1101_FSCTRL0     0x0C  // Frequency Synthesizer Control
#define CC1101_FREQ2       0x0D  // Frequency Control Word, High Byte
#define CC1101_FREQ1       0x0E  // Frequency Control Word, Middle Byte
#define CC1101_FREQ0       0x0F  // Frequency Control Word, Low Byte
#define CC1101_MDMCFG4     0x10  // Modem Configuration
#define CC1101_MDMCFG3     0x11  // Modem Configuration
#define CC1101_MDMCFG2     0x12  // Modem Configuration
#define CC1101_MDMCFG1     0x13  // Modem Configuration
#define CC1101_MDMCFG0     0x14  // Modem Configuration
#define CC1101_DEVIATN     0x15  // Modem Deviation Setting
#define CC1101_MCSM2       0x16  // Main Radio Control State Machine Configuration
#define CC1101_MCSM1       0x17  // Main Radio Control State Machine Configuration
#define CC1101_MCSM0       0x18  // Main Radio Control State Machine Configuration
#define CC1101_FOCCFG      0x19  // Frequency Offset Compensation Configuration
#define CC1101_BSCFG       0x1A  // Bit Synchronization Configuration
#define CC1101_AGCCTRL2    0x1B  // AGC Control
#define CC1101_AGCCTRL1    0x1C  // AGC Control
#define CC1101_AGCCTRL0    0x1D  // AGC Control
#define CC1101_WOREVT1     0x1E  // High Byte Event0 Timeout
#define CC1101_WOREVT0     0x1F  // Low Byte Event0 Timeout
#define CC1101_WORCTRL     0x20  // Wake On Radio Control
#define CC1101_FREND1      0x21  // Front End RX Configuration
#define CC1101_FREND0      0x22  // Front End TX Configuration
#define CC1101_FSCAL3      0x23  // Frequency Synthesizer Calibration
#define CC1101_FSCAL2      0x24  // Frequency Synthesizer Calibration
#define CC1101_FSCAL1      0x25  // Frequency Synthesizer Calibration
#define CC1101_FSCAL0      0x26  // Frequency Synthesizer Calibration
#define CC1101_RCCTRL1     0x27  // RC Oscillator Configuration
#define CC1101_RCCTRL0     0x28  // RC Oscillator Configuration
#define CC1101_FSTEST      0x29  // Frequency Synthesizer Calibration Control
#define CC1101_PTEST       0x2A  // Production Test
#define CC1101_AGCTEST     0x2B  // AGC Test
#define CC1101_TEST2       0x2C  // Various Test Settings
#define CC1101_TEST1       0x2D  // Various Test Settings
#define CC1101_TEST0       0x2E  // Various Test Settings

/**                              
 * Status registers              
 */                              
#define CC1101_PARTNUM     0x30  // Chip ID
#define CC1101_VERSION     0x31  // Chip ID
#define CC1101_FREQEST     0x32  // Frequency Offset Estimate from Demodulator
#define CC1101_LQI         0x33  // Demodulator Estimate for Link Quality
#define CC1101_RSSI        0x34  // Received Signal Strength Indication
#define CC1101_MARCSTATE   0x35  // Main Radio Control State Machine State
#define CC1101_WORTIME1    0x36  // High Byte of WOR Time
#define CC1101_WORTIME0    0x37  // Low Byte of WOR Time
#define CC1101_PKTSTATUS   0x38  // Current GDOx Status and Packet Status
#define CC1101_VCO_VC_DAC  0x39  // Current Setting from PLL Calibration Module
#define CC1101_TXBYTES     0x3A  // Underflow and Number of Bytes
#define CC1101_RXBYTES     0x3B  // Overflow and Number of Bytes
#define CC1101_RCCTRL1_STATUS 0x3C  // Last RC Oscillator Calibration Result
#define CC1101_RCCTRL0_STATUS 0x3D  // Last RC Oscillator Calibration Result 

typedef struct {
   uint16_t Reg;
   uint8_t  Value;
} RfSetting;

extern spi_device_handle_t gSpiHndl;

void CC1101_SetConfig(const RfSetting *pConfig);
int  CC1101_Rx(uint8_t *RxBuf,size_t RxBufLen,uint8_t *pRssi,uint8_t *pLqi);
bool CC1101_Tx(uint8_t *TxData);
bool CC1101_Present(void);
void CC1101_DumpRegs(void);
void CC1101_reset(void);
void CC1101_logState(void);
void CC1101_setRxState(void);
int CC1101_WaitMISO(const char *Func,int Line,int level);

#endif   // __CC1101_RADIO_H_

