/*
 *  ======== ti_radio_config.h ========
 *  Configured RadioConfig module definitions
 *
 *  DO NOT EDIT - This file is generated for the CC2652P1FSIP
 *  by the SysConfig tool.
 *
 *  Radio Config module version : 1.16
 *  SmartRF Studio data version : 2.28.0
 */
#ifndef _TI_RADIO_CONFIG_H_
#define _TI_RADIO_CONFIG_H_

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/rf_mailbox.h)
#include DeviceFamily_constructPath(driverlib/rf_common_cmd.h)
#include DeviceFamily_constructPath(driverlib/rf_ieee_cmd.h)
#include <ti/drivers/rf/RF.h>

/* SmartRF Studio version that the RF data is fetched from */
#define SMARTRF_STUDIO_VERSION "2.28.0"

// *********************************************************************************
//   RF Frontend configuration
// *********************************************************************************
// RF design based on: LP_CC2652PSIP
#define LP_CC2652PSIP

// High-Power Amplifier supported
#define SUPPORT_HIGH_PA

// RF frontend configuration
#define FRONTEND_24G_DIFF_RF
#define FRONTEND_24G_INT_BIAS

// Supported frequency bands
#define SUPPORT_FREQBAND_2400

// TX power table size definitions
#define TXPOWERTABLE_2400_PA5_SIZE 16 // 2400 MHz, 5 dBm
#define TXPOWERTABLE_2400_PA10_SIZE 6 // 2400 MHz, 10 dBm

// TX power tables
extern RF_TxPowerTable_Entry txPowerTable_2400_pa5[]; // 2400 MHz, 5 dBm
extern RF_TxPowerTable_Entry txPowerTable_2400_pa10[]; // 2400 MHz, 10 dBm



//*********************************************************************************
//  RF Setting:   IEEE 802.15.4-2006, 250 kbps, OQPSK, DSSS = 1:8
//
//  PHY:          ieee154p10
//  Setting file: setting_ieee_802_15_4_10_dbm.json
//*********************************************************************************

// PA table usage
#define TX_POWER_TABLE_SIZE_ieee154p10_0 TXPOWERTABLE_2400_PA5_SIZE

#define txPowerTable_ieee154p10_0 txPowerTable_2400_pa5

// TI-RTOS RF Mode object
extern RF_Mode RF_prop_ieee154p10_1;

// RF Core API commands
extern rfc_CMD_RADIO_SETUP_PA_t RF_cmdRadioSetup_ieee154p10_1;
extern rfc_CMD_FS_t RF_cmdFs_ieee154p10_1;
extern rfc_CMD_IEEE_TX_t RF_cmdIeeeTx_ieee154p10_1;
extern rfc_CMD_IEEE_RX_t RF_cmdIeeeRx_ieee154p10_1;

// RF Core API overrides
extern uint32_t pOverrides_ieee154p10_1[];

#endif // _TI_RADIO_CONFIG_H_
