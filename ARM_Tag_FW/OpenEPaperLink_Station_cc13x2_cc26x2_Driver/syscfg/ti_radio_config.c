/*
 *  ======== ti_radio_config.c ========
 *  Configured RadioConfig module definitions
 *
 *  DO NOT EDIT - This file is generated for the CC2652P1FSIP
 *  by the SysConfig tool.
 *
 *  Radio Config module version : 1.16
 *  SmartRF Studio data version : 2.28.0
 */

#include "ti_radio_config.h"
#include DeviceFamily_constructPath(rf_patches/rf_patch_cpe_ieee_802_15_4.h)


// *********************************************************************************
//   RF Frontend configuration
// *********************************************************************************
// RF design based on: LP_CC2652PSIP

// TX Power tables
// The RF_TxPowerTable_DEFAULT_PA_ENTRY and RF_TxPowerTable_HIGH_PA_ENTRY macros are defined in RF.h.
// The following arguments are required:
// RF_TxPowerTable_DEFAULT_PA_ENTRY(bias, gain, boost, coefficient)
// RF_TxPowerTable_HIGH_PA_ENTRY(bias, ibboost, boost, coefficient, ldoTrim)
// See the Technical Reference Manual for further details about the "txPower" Command field.
// The PA settings require the CCFG_FORCE_VDDR_HH = 0 unless stated otherwise.

// 2400 MHz, 5 dBm
RF_TxPowerTable_Entry txPowerTable_2400_pa5[TXPOWERTABLE_2400_PA5_SIZE] =
{
    {-20, RF_TxPowerTable_DEFAULT_PA_ENTRY(6, 3, 0, 4) }, // 0x08C6
    {-18, RF_TxPowerTable_DEFAULT_PA_ENTRY(7, 3, 0, 4) }, // 0x08C7
    {-15, RF_TxPowerTable_DEFAULT_PA_ENTRY(9, 3, 0, 0) }, // 0x00C9
    {-12, RF_TxPowerTable_DEFAULT_PA_ENTRY(12, 3, 0, 0) }, // 0x00CC
    {-10, RF_TxPowerTable_DEFAULT_PA_ENTRY(15, 3, 0, 4) }, // 0x08CF
    {-9, RF_TxPowerTable_DEFAULT_PA_ENTRY(16, 3, 0, 4) }, // 0x08D0
    {-6, RF_TxPowerTable_DEFAULT_PA_ENTRY(21, 3, 0, 4) }, // 0x08D5
    {-5, RF_TxPowerTable_DEFAULT_PA_ENTRY(24, 3, 0, 7) }, // 0x0ED8
    {-3, RF_TxPowerTable_DEFAULT_PA_ENTRY(32, 3, 0, 19) }, // 0x26E0
    {0, RF_TxPowerTable_DEFAULT_PA_ENTRY(30, 2, 0, 19) }, // 0x269E
    {1, RF_TxPowerTable_DEFAULT_PA_ENTRY(35, 2, 0, 19) }, // 0x26A3
    {2, RF_TxPowerTable_DEFAULT_PA_ENTRY(35, 1, 0, 28) }, // 0x3863
    {3, RF_TxPowerTable_DEFAULT_PA_ENTRY(27, 0, 0, 44) }, // 0x581B
    {4, RF_TxPowerTable_DEFAULT_PA_ENTRY(42, 0, 0, 69) }, // 0x8A2A
    {5, RF_TxPowerTable_DEFAULT_PA_ENTRY(62, 0, 0, 54) }, // 0x6C3E
    RF_TxPowerTable_TERMINATION_ENTRY
};

// 2400 MHz, 10 dBm
RF_TxPowerTable_Entry txPowerTable_2400_pa10[TXPOWERTABLE_2400_PA10_SIZE] =
{
    {6, RF_TxPowerTable_HIGH_PA_ENTRY(32, 0, 1, 32, 16) }, // 0x104120
    {7, RF_TxPowerTable_HIGH_PA_ENTRY(35, 0, 1, 35, 20) }, // 0x144723
    {8, RF_TxPowerTable_HIGH_PA_ENTRY(42, 0, 1, 44, 20) }, // 0x14592A
    {9, RF_TxPowerTable_HIGH_PA_ENTRY(23, 1, 1, 28, 20) }, // 0x143957
    {10, RF_TxPowerTable_HIGH_PA_ENTRY(30, 1, 1, 35, 20) }, // 0x14475E
    RF_TxPowerTable_TERMINATION_ENTRY
};



//*********************************************************************************
//  RF Setting:   IEEE 802.15.4-2006, 250 kbps, OQPSK, DSSS = 1:8
//
//  PHY:          ieee154p10
//  Setting file: setting_ieee_802_15_4_10_dbm.json
//*********************************************************************************

// PARAMETER SUMMARY
// Channel - Frequency (MHz): 2405
// TX Power (dBm): 5

// TI-RTOS RF Mode Object
RF_Mode RF_prop_ieee154p10_1 =
{
    .rfMode = RF_MODE_AUTO,
    .cpePatchFxn = &rf_patch_cpe_ieee_802_15_4,
    .mcePatchFxn = 0,
    .rfePatchFxn = 0
};

// Overrides for CMD_RADIO_SETUP_PA
uint32_t pOverrides_ieee154p10_1[] =
{
    // override_ieee_802_15_4_10_dbm.json
    // IEEE 15.4: Set IPEAK = 3 and DCDC dither off for TX
    (uint32_t)0x00F388D3,
    // Rx: Set LNA bias current offset to +15 to saturate trim to max (default: 0)
    (uint32_t)0x000F8883,
    (uint32_t)0xFFFFFFFF
};



// CMD_RADIO_SETUP_PA
// Radio Setup Command for Pre-Defined Schemes
rfc_CMD_RADIO_SETUP_PA_t RF_cmdRadioSetup_ieee154p10_1 =
{
    .commandNo = 0x0802,
    .status = 0x0000,
    .pNextOp = 0,
    .startTime = 0x00000000,
    .startTrigger.triggerType = 0x0,
    .startTrigger.bEnaCmd = 0x0,
    .startTrigger.triggerNo = 0x0,
    .startTrigger.pastTrig = 0x0,
    .condition.rule = 0x1,
    .condition.nSkip = 0x0,
    .mode = 0x01,
    .loDivider = 0x00,
    .config.frontEndMode = 0x0,
    .config.biasMode = 0x0,
    .config.analogCfgMode = 0x0,
    .config.bNoFsPowerUp = 0x0,
    .config.bSynthNarrowBand = 0x0,
    .txPower = 0x6C3E,
    .pRegOverride = pOverrides_ieee154p10_1,
    .pRegOverrideTxStd = 0,
    .pRegOverrideTx20 = 0
};

// CMD_FS
// Frequency Synthesizer Programming Command
rfc_CMD_FS_t RF_cmdFs_ieee154p10_1 =
{
    .commandNo = 0x0803,
    .status = 0x0000,
    .pNextOp = 0,
    .startTime = 0x00000000,
    .startTrigger.triggerType = 0x0,
    .startTrigger.bEnaCmd = 0x0,
    .startTrigger.triggerNo = 0x0,
    .startTrigger.pastTrig = 0x0,
    .condition.rule = 0x1,
    .condition.nSkip = 0x0,
    .frequency = 0x0965,
    .fractFreq = 0x0000,
    .synthConf.bTxMode = 0x0,
    .synthConf.refFreq = 0x0,
    .__dummy0 = 0x00,
    .__dummy1 = 0x00,
    .__dummy2 = 0x00,
    .__dummy3 = 0x0000
};

// CMD_IEEE_TX
// IEEE 802.15.4 Transmit Command
rfc_CMD_IEEE_TX_t RF_cmdIeeeTx_ieee154p10_1 =
{
    .commandNo = 0x2C01,
    .status = 0x0000,
    .pNextOp = 0,
    .startTime = 0x00000000,
    .startTrigger.triggerType = 0x0,
    .startTrigger.bEnaCmd = 0x0,
    .startTrigger.triggerNo = 0x0,
    .startTrigger.pastTrig = 0x0,
    .condition.rule = 0x1,
    .condition.nSkip = 0x0,
    .txOpt.bIncludePhyHdr = 0x0,
    .txOpt.bIncludeCrc = 0x0,
    .txOpt.payloadLenMsb = 0x0,
    .payloadLen = 0x1E,
    .pPayload = 0,
    .timeStamp = 0x00000000
};

// CMD_IEEE_RX
// IEEE 802.15.4 Receive Command
rfc_CMD_IEEE_RX_t RF_cmdIeeeRx_ieee154p10_1 =
{
    .commandNo = 0x2801,
    .status = 0x0000,
    .pNextOp = 0,
    .startTime = 0x00000000,
    .startTrigger.triggerType = 0x0,
    .startTrigger.bEnaCmd = 0x0,
    .startTrigger.triggerNo = 0x0,
    .startTrigger.pastTrig = 0x0,
    .condition.rule = 0x1,
    .condition.nSkip = 0x0,
    .channel = 0x00,
    .rxConfig.bAutoFlushCrc = 0x0,
    .rxConfig.bAutoFlushIgn = 0x0,
    .rxConfig.bIncludePhyHdr = 0x0,
    .rxConfig.bIncludeCrc = 0x0,
    .rxConfig.bAppendRssi = 0x1,
    .rxConfig.bAppendCorrCrc = 0x1,
    .rxConfig.bAppendSrcInd = 0x0,
    .rxConfig.bAppendTimestamp = 0x0,
    .pRxQ = 0,
    .pOutput = 0,
    .frameFiltOpt.frameFiltEn = 0x0,
    .frameFiltOpt.frameFiltStop = 0x0,
    .frameFiltOpt.autoAckEn = 0x0,
    .frameFiltOpt.slottedAckEn = 0x0,
    .frameFiltOpt.autoPendEn = 0x0,
    .frameFiltOpt.defaultPend = 0x0,
    .frameFiltOpt.bPendDataReqOnly = 0x0,
    .frameFiltOpt.bPanCoord = 0x0,
    .frameFiltOpt.maxFrameVersion = 0x3,
    .frameFiltOpt.fcfReservedMask = 0x0,
    .frameFiltOpt.modifyFtFilter = 0x0,
    .frameFiltOpt.bStrictLenFilter = 0x0,
    .frameTypes.bAcceptFt0Beacon = 0x1,
    .frameTypes.bAcceptFt1Data = 0x1,
    .frameTypes.bAcceptFt2Ack = 0x1,
    .frameTypes.bAcceptFt3MacCmd = 0x1,
    .frameTypes.bAcceptFt4Reserved = 0x1,
    .frameTypes.bAcceptFt5Reserved = 0x1,
    .frameTypes.bAcceptFt6Reserved = 0x1,
    .frameTypes.bAcceptFt7Reserved = 0x1,
    .ccaOpt.ccaEnEnergy = 0x0,
    .ccaOpt.ccaEnCorr = 0x0,
    .ccaOpt.ccaEnSync = 0x0,
    .ccaOpt.ccaCorrOp = 0x1,
    .ccaOpt.ccaSyncOp = 0x1,
    .ccaOpt.ccaCorrThr = 0x0,
    .ccaRssiThr = 0x64,
    .__dummy0 = 0x00,
    .numExtEntries = 0x00,
    .numShortEntries = 0x00,
    .pExtEntryList = 0,
    .pShortEntryList = 0,
    .localExtAddr = 0x12345678,
    .localShortAddr = 0xABBA,
    .localPanID = 0x0000,
    .__dummy1 = 0x000000,
    .endTrigger.triggerType = 0x1,
    .endTrigger.bEnaCmd = 0x0,
    .endTrigger.triggerNo = 0x0,
    .endTrigger.pastTrig = 0x0,
    .endTime = 0x00000000
};


