#include "radio.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "RFQueue.h"
#include "ieee_rf_config.h"
#include <ti/drivers/rf/RF.h>
#include "ti_drivers_config.h"
#include DeviceFamily_constructPath(driverlib/rf_prop_mailbox.h)

char the_msg[1000];
#define  max_rf_rx_buffer  20
volatile uint8_t got_rx = 0;
volatile uint8_t worked_got_rx = 0;
volatile uint8_t packetDataPointer[max_rf_rx_buffer+1][1000];

#define CMD_CLEAR_RX                                            0x0008

/* Packet RX Configuration */
#define DATA_ENTRY_HEADER_SIZE 8  /* Constant header size of a Generic Data Entry */
#define MAX_LENGTH             256 /* Max length byte the radio will accept */
#define NUM_DATA_ENTRIES       2  /* NOTE: Only two data entries supported at the moment */
#define MAC_PHY_PHR_IEEE_LEN            1
#define MAC_RSSI_LEN                    1
#define MAC_TS_LEN                      4
#define MAC_FCS2_FIELD_LEN              2       /* 2-byte FCS field */
#define NUM_APPENDED_BYTES     MAC_PHY_PHR_IEEE_LEN + MAC_RSSI_LEN + MAC_TS_LEN + MAC_FCS2_FIELD_LEN


/***** Variable declarations *****/
static RF_Object rfObject;
static RF_Handle rfHandle;
static RF_ScheduleCmdParams scheduleParams;
static RF_ScheduleCmdParams rxParam;
static RF_CmdHandle cmd;
static RF_Params rfParams;

static uint8_t rxRadioBufferPtr[RF_QUEUE_DATA_ENTRY_BUFFER_SIZE(NUM_DATA_ENTRIES, MAX_LENGTH, NUM_APPENDED_BYTES)] __attribute__((aligned(4)));

static rfc_ieeeRxOutput_t macRxOutputIeee;
static dataQueue_t   macRxDataEntryQueue;

static rfc_dataEntryGeneral_t* currentDataEntry;

rfc_CMD_CLEAR_RX_t clearCMD  =
{
    .commandNo = CMD_CLEAR_RX,
    .pQueue = 0                 //!<        Pointer to the queue structure to be cleared
};

static void radioRxCallback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e)
{
  currentDataEntry = RFQueue_getDataEntry();
  RFQueue_nextEntry();
  if (e & RF_EventRxOk)
    {
        if(*(uint8_t*)(&currentDataEntry->data) > 0){
            memcpy((uint8_t *)packetDataPointer[got_rx], &currentDataEntry->data, *(uint8_t*)(&currentDataEntry->data));
            got_rx++;
            got_rx %= max_rf_rx_buffer;
        }
    }

}

void initRadio(uint8_t channel)
{
    static RF_Mode RF_prop_ieee154_inner;
    static rfc_CMD_RADIO_SETUP_t RF_cmdRadioSetup_inner;
    static rfc_CMD_FS_t RF_cmdFs_ieee154_inner;
    static rfc_CMD_IEEE_RX_t RF_cmdIEEERx_inner;
    RF_cancelCmd(rfHandle, cmd, 0);
    RF_pendCmd(rfHandle, cmd, RF_EventLastCmdDone);
    if (rfHandle != NULL)
    {
        RF_yield(rfHandle);
        RF_close(rfHandle);
        rfHandle = NULL;
    }
    uint16_t frequency = 2405 + (5 * (channel - 11));

    memcpy(&RF_prop_ieee154_inner,&RF_prop_ieee154,sizeof(RF_Mode));
    memset(&rfObject,0x00,sizeof(RF_Object));
    memcpy(&RF_cmdRadioSetup_inner,&RF_cmdRadioSetup,sizeof(RF_RadioSetup));
    memset(&rfParams,0x00,sizeof(RF_Params));

    RF_Params_init(&rfParams);
    rfParams.nID = RF_STACK_ID_154;
    clearCMD.pQueue = (dataQueue_t *) &macRxDataEntryQueue;
    RF_cmdRadioSetup_inner.status = IDLE;
    RF_cmdRadioSetup_inner.pNextOp = NULL;
    RF_cmdRadioSetup_inner.condition.rule = COND_NEVER;
    RF_cmdRadioSetup_inner.startTrigger.pastTrig = 1;

    rfHandle = RF_open((RF_Object *)&rfObject, &RF_prop_ieee154_inner, (RF_RadioSetup*) &RF_cmdRadioSetup_inner, &rfParams);

    RF_TxPowerTable_Value power_val = RF_TxPowerTable_DEFAULT_PA_ENTRY(23, 0, 0, 57);
    RF_setTxPower(rfHandle, power_val);

    memcpy(&RF_cmdFs_ieee154_inner,&RF_cmdFs_ieee154,sizeof(rfc_CMD_FS_t));
    RF_cmdFs_ieee154_inner.frequency = frequency;
    RF_postCmd(rfHandle, (RF_Op*)&RF_cmdFs_ieee154_inner, RF_PriorityNormal, NULL, 0);

    if(RFQueue_defineQueue(&macRxDataEntryQueue, rxRadioBufferPtr, sizeof(rxRadioBufferPtr), NUM_DATA_ENTRIES, MAX_LENGTH + NUM_APPENDED_BYTES))
    {
         printf("QUEUE ERROR\r\n");
         while(1);
    }
    memset(&rxParam,0x00,sizeof(RF_ScheduleCmdParams));
    RF_ScheduleCmdParams_init(&rxParam);

    rxParam.duration = 0;
    rxParam.startTime = 0;
    rxParam.startType = RF_StartNotSpecified;
    rxParam.endTime = 0;
    rxParam.endType = RF_EndInfinit;

    memcpy(&RF_cmdIEEERx_inner,&RF_cmdIEEERx,sizeof(rfc_CMD_IEEE_RX_t));
    RF_cmdIEEERx_inner.status = IDLE;

    RF_cmdIEEERx_inner.pRxQ = (dataQueue_t*)&macRxDataEntryQueue;
    RF_cmdIEEERx_inner.pOutput = (rfc_ieeeRxOutput_t *)&macRxOutputIeee;
    RF_cmdIEEERx_inner.rxConfig.bAutoFlushCrc     = 0x1;
    RF_cmdIEEERx_inner.rxConfig.bAutoFlushIgn     = 0x1;
    RF_cmdIEEERx_inner.rxConfig.bIncludePhyHdr    = 0x1;
    RF_cmdIEEERx_inner.rxConfig.bIncludeCrc       = 0x0;
    RF_cmdIEEERx_inner.rxConfig.bAppendRssi       = 0x1;
    RF_cmdIEEERx_inner.rxConfig.bAppendCorrCrc    = 0x0;
    RF_cmdIEEERx_inner.rxConfig.bAppendSrcInd     = 0x1;
    RF_cmdIEEERx_inner.rxConfig.bAppendTimestamp  = 0x0;

    RF_cmdIEEERx_inner.startTime = 0;
    RF_cmdIEEERx_inner.startTrigger.triggerType = TRIG_NOW;
    RF_cmdIEEERx_inner.startTrigger.pastTrig = 1;
    RF_cmdIEEERx_inner.ccaOpt.ccaEnEnergy = 0;
    RF_cmdIEEERx_inner.ccaOpt.ccaEnCorr = 0;
    RF_cmdIEEERx_inner.ccaOpt.ccaEnSync = 0;
    RF_cmdIEEERx_inner.ccaOpt.ccaCorrOp = 0;
    RF_cmdIEEERx_inner.ccaOpt.ccaSyncOp = 0;
    RF_cmdIEEERx_inner.ccaOpt.ccaCorrThr = 3;
    RF_cmdIEEERx_inner.ccaRssiThr = 0x64;
    RF_cmdIEEERx_inner.channel = channel;
    cmd = RF_scheduleCmd(rfHandle, (RF_Op*)&RF_cmdIEEERx_inner, &rxParam, radioRxCallback, RF_EventRxOk | RF_EventRxNOk | RF_EventRxBufFull | RF_EventTXAck | RF_EventCmdPreempted);
}

void radioSendData(uint8_t *out_buff, int len)
{
    RF_ScheduleCmdParams_init(&scheduleParams);
    scheduleParams.startTime    = 0;
    scheduleParams.startType    = RF_StartNotSpecified;
    scheduleParams.allowDelay   = RF_AllowDelayAny;
    scheduleParams.duration     = ~(0);
    scheduleParams.endTime      = ~(0);
    scheduleParams.endType      = RF_EndNotSpecified;
    RF_cmdIeeeTx_ieee154.startTrigger.triggerType = TRIG_NOW;
    RF_cmdIeeeTx_ieee154.payloadLen = len;
    RF_cmdIeeeTx_ieee154.pPayload = out_buff;
    RF_runScheduleCmd(rfHandle, (RF_Op*)&RF_cmdIeeeTx_ieee154,&scheduleParams, NULL, 0);
    RF_yield(rfHandle);
}
