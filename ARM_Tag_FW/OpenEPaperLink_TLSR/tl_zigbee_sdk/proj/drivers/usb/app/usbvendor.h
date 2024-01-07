/********************************************************************************************************
 * @file    usbvendor.h
 *
 * @brief   This is the header file for usbvendor
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

#pragma once

#include "usbvendor_i.h"

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
    extern "C" {
#endif


typedef void (*hidvendor_handlerFn_t)(u8 *pData);

typedef struct {
    u16 len;
    u8  data[1];
} hidvendor_txBuf_t;

typedef struct {
    u8 *rxBuf;
    hidvendor_txBuf_t *txBuf;

	/* Following variables are used in the RX more than CDC_TXRX_EPSIZE */
    ev_timer_event_t *timer;

	hidvendor_handlerFn_t rxCb;
	hidvendor_handlerFn_t txCb;

	u16 lenToSend;
	u16 lastSendIndex;
	u8  lastIndex;
} hidvendor_ctrl_t;

typedef enum hidvendor_sts_e {
	HID_VENDOR_SUCCESS  = 0,
	HID_VENDOR_BUSY 	= 1,
	HID_VENDOR_MULTIBLOCK,
} hidvendor_sts_t;

/**
 * @brief This function used to send or receive data as the USB vendor device
 * @param   none
 * @return  none
 */
void hidvendor_dataHandler(void);

/**
 * @brief This function initializes the USB vendor device
 * @param   none
 * @return none
 */
void hidvendor_init(void);

/**
 * @brief This function sets the USB vendor tx and rx callback function
 * @param[in]   rxFunc rx callback function
 * @param[in]   txCb tx callback function
 * @return none
 */
void hidvendor_setCb(hidvendor_handlerFn_t rxFunc, hidvendor_handlerFn_t txCb);

/**
 * @brief This function sets the USB vendor rx buffer
 * @param[in]   buf pointer to the rx buffer
 * @return none
 */
void hidvendor_setRxBuf(u8 *buf);

/**
 * @brief This function sends data to host as the USB vendor device
 * @param[in]   buf pointer to the data need to send
 * @param[in]   len length in byte of the data need to send
 * @return none
 */
hidvendor_sts_t hidvendor_reportData(hidvendor_txBuf_t *txBuf);

/**
 * @brief This function determines whether the USB interface is available
 * @param   none
 * @return 1: the USB interface is available 0: the USB interface is busy
 */
u8 hidvendor_isAvailable(void);

/**
 * @brief This function used to check whether the Tx of the control endpoint is busy
 * @param   none
 * @return 1: the control endpoint is busy 0: the control endpoint is idle
 */
u8 hidvendor_isTxBusy(void);

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
    }
#endif
