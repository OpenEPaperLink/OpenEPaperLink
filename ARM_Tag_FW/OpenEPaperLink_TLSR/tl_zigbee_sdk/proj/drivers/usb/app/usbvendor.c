/********************************************************************************************************
 * @file    usbvendor.c
 *
 * @brief   This is the source file for usbvendor
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

#include "usbvendor.h"

#if (USB_VENDOR_ENABLE)

hidvendor_ctrl_t hidvendor_vs;
hidvendor_ctrl_t *hidvendor_v = &hidvendor_vs;


/**
 * @brief This function initializes the USB vendor device
 * @param   none
 * @return none
 */
void hidvendor_init(void)
{
    hidvendor_v->lastIndex = 0;
}

u8 hidvendor_isAvailable(void)
{
	return (hidvendor_v->txBuf == NULL);
}

u8 hidvendor_isTxBusy(void)
{
    return usbhw_is_ep_busy(USB_EDP_HID_VENDOR_IN);
}

/**
 * @brief This function sets the USB vendor rx buffer
 * @param[in]   buf pointer to the rx buffer
 * @return none
 */
void hidvendor_setRxBuf(u8 *buf)
{
	hidvendor_v->rxBuf = buf;
}

/**
 * @brief This function sets the USB vendor tx and rx callback function
 * @param[in]   rxFunc rx callback function
 * @param[in]   txCb tx callback function
 * @return none
 */
void hidvendor_setCb(hidvendor_handlerFn_t rxFunc, hidvendor_handlerFn_t txCb)
{
	hidvendor_v->rxCb = rxFunc;
    hidvendor_v->txCb = txCb;
}

static s32 hidvendor_recvTimeoutCb(void *arg)
{
	u8 *p;

	hidvendor_v->lastIndex = 0;

	/* Clear the buffer */
	p = hidvendor_v->rxBuf;
	hidvendor_v->rxBuf = NULL;

	/* Callback */
	if(hidvendor_v->rxCb){
		hidvendor_v->rxCb(p);
	}

	hidvendor_v->timer = NULL;
	return -1;
}

static void hidvendor_recvData(void)
{
	u8 i;
	u8 *p;
	u8 len;
	u8 fEnd = 0;

	/* No buffer */
	if(!hidvendor_v->rxBuf){
		while(1);
	}

	if(hidvendor_v->timer){
		TL_ZB_TIMER_CANCEL(&hidvendor_v->timer);
	}

	len = reg_usb_ep_ptr(USB_EDP_HID_VENDOR_OUT & 0x07);
	fEnd = (len == HID_VENDOR_TXRX_EPSIZE) ? 0 : 1;
	usbhw_reset_ep_ptr(USB_EDP_HID_VENDOR_OUT);

	for(i = 0; i < len; i++){
		hidvendor_v->rxBuf[hidvendor_v->lastIndex++] = usbhw_read_ep_data(USB_EDP_HID_VENDOR_OUT);
	}

	if(fEnd){
		hidvendor_v->lastIndex = 0;

		/* Clear the buffer */
		p = hidvendor_v->rxBuf;
		hidvendor_v->rxBuf = NULL;

		/* Callback */
		if(hidvendor_v->rxCb){
			hidvendor_v->rxCb(p);
		}
	}else{
		hidvendor_v->timer = TL_ZB_TIMER_SCHEDULE(hidvendor_recvTimeoutCb, NULL, 20);
	}
}

/**
 * @brief This function sends bulk data to host as the USB vendor device
 * @param[in]   none
 * @return the length of the data send out
 */
static u8 hidvendor_sendData(void)
{
	u16 len;

	/* Wait until not busy */
	if(usbhw_is_ep_busy(USB_EDP_HID_VENDOR_IN)){
        /* Return to wait IRQ come again */
        return 0;
    }

    /* Get the length to send in this bulk transaction */
	len = (hidvendor_v->lenToSend > HID_VENDOR_TXRX_EPSIZE) ? HID_VENDOR_TXRX_EPSIZE : hidvendor_v->lenToSend;
	hidvendor_v->lenToSend -= len;

	if(len == 0){
		return 0;
	}

    reg_usb_ep_ptr(USB_EDP_HID_VENDOR_IN) = 0;

	/* Write data to USB fifo */
    foreach(i, len){
        reg_usb_ep_dat(USB_EDP_HID_VENDOR_IN) = hidvendor_v->txBuf->data[hidvendor_v->lastSendIndex++];
    }

	/* Write ACK */
    reg_usb_ep_ctrl(USB_EDP_HID_VENDOR_IN) = FLD_EP_DAT_ACK;        // ACK
    u16 t = 0;
    while(usbhw_is_ep_busy(USB_EDP_HID_VENDOR_IN)){
        if(t++ > 10000){
            reg_usb_ep_ctrl(USB_EDP_HID_VENDOR_IN) &= 0xfe; // clear bit(0)
        }
    };

    /* TX transaction finish */
	if(hidvendor_v->lenToSend == 0){
		hidvendor_v->lenToSend = 0;
		hidvendor_v->lastSendIndex = 0;

        if(hidvendor_v->txCb){
            //EV_SCHEDULE_TASK(hid_v->txCb, hid_v->txBuf);
            hidvendor_v->txCb((u8 *)hidvendor_v->txBuf);
        }

		hidvendor_v->txBuf = NULL;
	}

	return len;
}

/**
 * @brief This function sends data to host as the USB vendor device
 * @param[in]   buf pointer to the data need to send
 * @param[in]   len length in byte of the data need to send
 * @return none
 */
hidvendor_sts_t hidvendor_reportData(hidvendor_txBuf_t *txBuf)//u8 *buf, u8 len)
{
	if(hidvendor_v->txBuf){
		return HID_VENDOR_BUSY;
	}

	/* Init the bulk transfer */
    hidvendor_v->lenToSend = txBuf->len;
	hidvendor_v->txBuf = txBuf;
	hidvendor_v->lastSendIndex = 0;

    /* Send first bulk */
	hidvendor_sendData();

	usbhw_data_ep_ack(USB_EDP_HID_VENDOR_OUT);

	return HID_VENDOR_SUCCESS;
}

void hidvendor_dataHandler(void)
{
	u32 irq = usbhw_get_eps_irq();// data irq

	if(irq & BIT((USB_EDP_HID_VENDOR_OUT & 0x07))){
		usbhw_clr_eps_irq(BIT((USB_EDP_HID_VENDOR_OUT & 0x07))); // clear ime

		hidvendor_recvData();

		usbhw_data_ep_ack(USB_EDP_HID_VENDOR_OUT);
	}

	if(irq & BIT((USB_EDP_HID_VENDOR_IN & 0x07))){
		usbhw_clr_eps_irq(BIT((USB_EDP_HID_VENDOR_IN & 0x07))); // clear ime

		if(hidvendor_sendData()){
			usbhw_data_ep_ack(USB_EDP_HID_VENDOR_OUT);
		}
	}
}

#endif
