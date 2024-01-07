/********************************************************************************************************
 * @file    USBController.h
 *
 * @brief   This is the header file for USBController
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

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

/* Defines: */
/** \name Endpoint Direction Masks */
//@{
/** Endpoint direction mask, for masking against endpoint addresses to retrieve the endpoint's
 *	direction for comparing with the \c ENDPOINT_DIR_* masks.
 */
#define ENDPOINT_DIR_MASK				   0x80

/** Endpoint address direction mask for an OUT direction (Host to Device) endpoint. This may be ORed with
 *	the index of the address within a device to obtain the full endpoint address.
 */
#define ENDPOINT_DIR_OUT				   0x00

/** Endpoint address direction mask for an IN direction (Device to Host) endpoint. This may be ORed with
 *	the index of the address within a device to obtain the full endpoint address.
 */
#define ENDPOINT_DIR_IN 				   0x80
//@}

/** \name Endpoint/Pipe Type Masks */
//@{
/** Mask for determining the type of an endpoint from an endpoint descriptor. This should then be compared
 *	with the \c EP_TYPE_* masks to determine the exact type of the endpoint.
 */
#define EP_TYPE_MASK					   0x03

/** Mask for a CONTROL type endpoint or pipe.
 *
 *	\note See \ref Group_EndpointManagement and \ref Group_PipeManagement for endpoint/pipe functions.
 */
#define EP_TYPE_CONTROL 				   0x00

/** Mask for an ISOCHRONOUS type endpoint or pipe.
 *
 *	\note See \ref Group_EndpointManagement and \ref Group_PipeManagement for endpoint/pipe functions.
 */
#define EP_TYPE_ISOCHRONOUS 			   0x01

/** Mask for a BULK type endpoint or pipe.
 *
 *	\note See \ref Group_EndpointManagement and \ref Group_PipeManagement for endpoint/pipe functions.
 */
#define EP_TYPE_BULK					   0x02

/** Mask for an INTERRUPT type endpoint or pipe.
 *
 *	\note See \ref Group_EndpointManagement and \ref Group_PipeManagement for endpoint/pipe functions.
 */
#define EP_TYPE_INTERRUPT				   0x03

/**If Isochronous endpoint,
Bits 3..2 = Synchronisation Type (Iso Mode)
00 = No Synchonisation
01 = Asynchronous
10 = Adaptive
11 = Synchronous
Bits 5..4 = Usage Type (Iso Mode)
00 = Data Endpoint
01 = Feedback Endpoint
10 = Explicit Feedback Data Endpoint
11 = Reserved
*/
#define EP_SYNC_TYPE_NO_SYNC                0
#define EP_SYNC_TYPE_ASYN                   1
#define EP_SYNC_TYPE_ADAPTIVE               2
#define EP_SYNC_TYPE_SYNC                   3

#define EP_USAGE_TYPE_DATA                  0
#define EP_USAGE_TYPE_FEEDBACK              1
#define EP_USAGE_TYPE_FEEDBACK_DATA         2
#define EP_USAGE_TYPE_RSV                   3

//@}

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif

