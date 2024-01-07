/********************************************************************************************************
 * @file    usbcdc.c
 *
 * @brief   This is the source file for usbcdc
 *
 * @author  Driver & Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/

#include "usbcdc.h"

#if (USB_CDC_ENABLE)

cdc_ctrl_t cdc_vs;
cdc_ctrl_t *cdc_v = &cdc_vs;

USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
{
	//.Config =
    {
	    0, //ControlInterfaceNumber

		CDC_TX_EPNUM, // DataINEndpointNumber
		CDC_TXRX_EPSIZE, // DataINEndpointSize
		false, // DataINEndpointDoubleBank

		CDC_RX_EPNUM,  // DataOUTEndpointNumber
		CDC_TXRX_EPSIZE, // DataOUTEndpointSize
		false, // DataOUTEndpointDoubleBank

		CDC_NOTIFICATION_EPNUM, // NotificationEndpointNumber
		CDC_NOTIFICATION_EPSIZE, // NotificationEndpointSize
		false,  // NotificationEndpointDoubleBank
	},
};

USB_ClassInfo_CDC_Device_t *CDCInterfaceInfo = &VirtualSerial_CDC_Interface;


static void usbcdc_write32(u32 value)
{
	usbhw_write_ctrl_ep_data(value & 0xff);
	usbhw_write_ctrl_ep_data((value >> 8) & 0xff);
	usbhw_write_ctrl_ep_data((value >> 16) & 0xff);
	usbhw_write_ctrl_ep_data((value >> 24) & 0xff);
}

static void usbcdc_read32(u32 *value)
{
	u32 temp = 0;
	*value = usbhw_read_ctrl_ep_data();

	temp =  usbhw_read_ctrl_ep_data();
	*value = (temp << 8) | (*value);
	temp = 0;

	temp =  usbhw_read_ctrl_ep_data();
	*value = (temp << 16) | (*value);
	temp = 0;

	temp =  usbhw_read_ctrl_ep_data();
	*value = (temp << 24) | (*value);
}

/**
 * @brief This function handles the USB CDC class-specific requests from host
 * @param[in]   bRequest the request type
 * @param[in]   wValue word-sized field that varies according to request
 * @param[in]   wIndex word-sized field that varies according to request
 *              typically used to pass an index or offset
 * @param[in]   wLength number of bytes to transfer if there is a Data stage
 * @return none
 */
void usbcdc_processControlRequest(u8 bRequest, u16 wValue, u16 wIndex, u16 wLength)
{
	if(wIndex != CDCInterfaceInfo->Config.ControlInterfaceNumber)
	  return;

	switch(bRequest){
		case CDC_REQ_GetLineEncoding:
            usbcdc_write32(CDCInterfaceInfo->State.LineEncoding.BaudRateBPS);
			usbhw_write_ctrl_ep_data(CDCInterfaceInfo->State.LineEncoding.CharFormat);
            usbhw_write_ctrl_ep_data(CDCInterfaceInfo->State.LineEncoding.ParityType);
            usbhw_write_ctrl_ep_data(CDCInterfaceInfo->State.LineEncoding.DataBits);
			break;
		case CDC_REQ_SetLineEncoding:
			usbcdc_read32(&CDCInterfaceInfo->State.LineEncoding.BaudRateBPS);
			CDCInterfaceInfo->State.LineEncoding.CharFormat  = usbhw_read_ctrl_ep_data();
			CDCInterfaceInfo->State.LineEncoding.ParityType  = usbhw_read_ctrl_ep_data();
			CDCInterfaceInfo->State.LineEncoding.DataBits    = usbhw_read_ctrl_ep_data();
			break;
		case CDC_REQ_SetControlLineState:
			CDCInterfaceInfo->State.ControlLineStates.HostToDevice = wValue;
			break;
		case CDC_REQ_SendBreak:
			break;
	}
}


u8 usbcdc_isAvailable(void)
{
	return (cdc_v->txBuf == NULL);
}

u8 usbcdc_isTxBusy(void)
{
    return usbhw_is_ep_busy(CDC_TX_EPNUM);
}

/**
 * @brief This function initializes the USB CDC device
 * @param   none
 * @return none
 */
void usbcdc_init(void)
{
	/* Init UART parameters */
	CDCInterfaceInfo->State.LineEncoding.BaudRateBPS = 115200;
	CDCInterfaceInfo->State.LineEncoding.CharFormat = CDC_LINEENCODING_OneStopBit;
	CDCInterfaceInfo->State.LineEncoding.ParityType = CDC_PARITY_None;
	CDCInterfaceInfo->State.LineEncoding.DataBits = 8;

    cdc_v->lastIndex = 0;
}

/**
 * @brief This function sets the USB CDC rx buffer
 * @param[in]   buf pointer to the rx buffer
 * @return none
 */
void usbcdc_setRxBuf(u8 *buf)
{
	cdc_v->rxBuf = buf;
}

/**
 * @brief This function sets the USB CDC tx and rx callback function
 * @param[in]   rxFunc rx callback function
 * @param[in]   txCb tx callback function
 * @return none
 */
void usbcdc_setCb(cdc_handlerFn_t rxFunc, cdc_handlerFn_t txCb)
{
	cdc_v->rxCb = rxFunc;
    cdc_v->txCb = txCb;
}

static s32 usbcdc_recvTimeoutCb(void *arg)
{
	u8 *p;

	cdc_v->lastIndex = 0;

	/* Clear the buffer */
	p = cdc_v->rxBuf;
	cdc_v->rxBuf = NULL;

	/* Callback */
	if(cdc_v->rxCb){
		cdc_v->rxCb(p);
	}

	cdc_v->timer = NULL;
	return -1;
}

static void usbcdc_recvData(void)
{
	u8 i;
	u8 *p;
	u8 len;
	u8 fEnd = 0;

	/* No buffer */
	if(!cdc_v->rxBuf){
		while(1);
	}

	if(cdc_v->timer){
		TL_ZB_TIMER_CANCEL(&cdc_v->timer);
	}

	len = reg_usb_ep_ptr(CDC_RX_EPNUM & 0x07);
	fEnd = (len == CDC_TXRX_EPSIZE) ? 0 : 1;
	usbhw_reset_ep_ptr(CDC_RX_EPNUM);

	for(i = 0; i < len; i++){
		cdc_v->rxBuf[cdc_v->lastIndex++] = usbhw_read_ep_data(CDC_RX_EPNUM);
	}

	if(fEnd){
		cdc_v->lastIndex = 0;

		/* Clear the buffer */
		p = cdc_v->rxBuf;
		cdc_v->rxBuf = NULL;

		/* Callback */
		if(cdc_v->rxCb){
			cdc_v->rxCb(p);
		}
	}else{
		cdc_v->timer = TL_ZB_TIMER_SCHEDULE(usbcdc_recvTimeoutCb, NULL, 20);
	}
}

/**
 * @brief This function sends bulk data to host as the USB CDC device
 * @param[in]   none
 * @return the length of the data send out
 */
u8 usbcdc_sendBulkData(void)
{
	u16 len;

	/* Wait until not busy */
	if(usbhw_is_ep_busy(CDC_TX_EPNUM)){
        /* Return to wait IRQ come again */
        return 0;
    }

    /* Get the length to send in this bulk transaction */
	len = (cdc_v->lenToSend > CDC_TXRX_EPSIZE) ? CDC_TXRX_EPSIZE : cdc_v->lenToSend;
	cdc_v->lenToSend -= len;

	if(len == 0){
		return 0;
	}

    reg_usb_ep_ptr(CDC_TX_EPNUM) = 0;

	/* Write data to USB fifo */
    foreach(i, len){
        reg_usb_ep_dat(CDC_TX_EPNUM) = cdc_v->txBuf->data[cdc_v->lastSendIndex++];
    }

	/* Write ACK */
    reg_usb_ep_ctrl(CDC_TX_EPNUM) = FLD_EP_DAT_ACK;        // ACK
    u16 t = 0;
    while(usbhw_is_ep_busy(CDC_TX_EPNUM)){
        if(t++ > 10000){
            reg_usb_ep_ctrl(CDC_TX_EPNUM) &= 0xfe; // clear bit(0)
        }
    };

    /* TX transaction finish */
	if(cdc_v->lenToSend == 0){
		cdc_v->lenToSend = 0;
		cdc_v->lastSendIndex = 0;

        if(cdc_v->txCb){
            //EV_SCHEDULE_TASK(cdc_v->txCb, cdc_v->txBuf);
            cdc_v->txCb((u8 *)cdc_v->txBuf);
        }

		cdc_v->txBuf = NULL;
	}

	return len;
}

/**
 * @brief This function sends data to host as the USB CDC device
 * @param[in]   buf pointer to the data need to send
 * @param[in]   len length in byte of the data need to send
 * @return none
 */
usbcdc_sts_t usbcdc_sendData(usbcdc_txBuf_t *txBuf)//u8 *buf, u8 len)
{
	if(cdc_v->txBuf){
		return USB_CDC_BUSY;
	}

	/* Init the bulk transfer */
    cdc_v->lenToSend = txBuf->len;
	cdc_v->txBuf = txBuf;
	cdc_v->lastSendIndex = 0;

    /* Send first bulk */
	usbcdc_sendBulkData();

	usbhw_data_ep_ack(CDC_RX_EPNUM);

	return USB_CDC_SUCCESS;
}

u8 usbWriteByte(u8 byte){
	if(usbhw_is_ep_busy(CDC_TX_EPNUM)){
		/* Return to wait IRQ come again */
		return 0;
	}
	reg_usb_ep_ptr(CDC_TX_EPNUM) = 0;
	reg_usb_ep_dat(CDC_TX_EPNUM) = byte;
	/* Write ACK */
	reg_usb_ep_ctrl(CDC_TX_EPNUM) = FLD_EP_DAT_ACK;        // ACK
	u16 t = 0;
	while(usbhw_is_ep_busy(CDC_TX_EPNUM)){
		if(t++ > 10000){
			reg_usb_ep_ctrl(CDC_TX_EPNUM) &= 0xfe; // clear bit(0)
		}
	}
	return 1;
}

void usbcdc_dataHandler(void)
{
	u32 irq = usbhw_get_eps_irq();

	if(irq & BIT((CDC_RX_EPNUM & 0x07))){
		usbhw_clr_eps_irq(BIT((CDC_RX_EPNUM & 0x07))); // clear ime

		usbcdc_recvData();

		usbhw_data_ep_ack(CDC_RX_EPNUM);
	}

	if(irq & BIT((CDC_TX_EPNUM & 0x07))){
		usbhw_clr_eps_irq(BIT((CDC_TX_EPNUM & 0x07))); // clear ime

		if(usbcdc_sendBulkData()){
			usbhw_data_ep_ack(CDC_RX_EPNUM);
		}
	}
}

#endif
