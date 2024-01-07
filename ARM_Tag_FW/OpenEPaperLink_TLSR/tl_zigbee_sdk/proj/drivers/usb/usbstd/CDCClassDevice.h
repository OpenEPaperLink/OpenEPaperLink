/********************************************************************************************************
 * @file    CDCClassDevice.h
 *
 * @brief   This is the header file for CDCClassDevice
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

/* Includes: */
#include "types.h"
#include "CDCClassCommon.h"

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
    extern "C" {
#endif

/* Public Interface - May be used in end-application: */
/* Type Defines: */
/** \brief CDC Class Device Mode Configuration and State Structure.
 *
 *  Class state structure. An instance of this structure should be made for each CDC interface
 *  within the user application, and passed to each of the CDC class driver functions as the
 *  CDCInterfaceInfo parameter. This stores each CDC interface's configuration and state information.
 */
typedef struct
{
	struct
	{
		u8       ControlInterfaceNumber; /**< Interface number of the CDC control interface within the device. */

		u8       DataINEndpointNumber; /**< Endpoint number of the CDC interface's IN data endpoint. */
		u16      DataINEndpointSize; /**< Size in bytes of the CDC interface's IN data endpoint. */
		bool     DataINEndpointDoubleBank; /**< Indicates if the CDC interface's IN data endpoint should use double banking. */

		u8       DataOUTEndpointNumber; /**< Endpoint number of the CDC interface's OUT data endpoint. */
		u16      DataOUTEndpointSize;  /**< Size in bytes of the CDC interface's OUT data endpoint. */
		bool     DataOUTEndpointDoubleBank; /**< Indicates if the CDC interface's OUT data endpoint should use double banking. */

		u8       NotificationEndpointNumber; /**< Endpoint number of the CDC interface's IN notification endpoint, if used. */
		u16      NotificationEndpointSize;  /**< Size in bytes of the CDC interface's IN notification endpoint, if used. */
		bool     NotificationEndpointDoubleBank; /**< Indicates if the CDC interface's notification endpoint should use double banking. */
	} Config; /**< Config data for the USB class interface within the device. All elements in this section
			   *   <b>must</b> be set or the interface will fail to enumerate and operate correctly.
			   */
	struct
	{
		struct
		{
			u16      HostToDevice; /**< Control line states from the host to device, as a set of \c CDC_CONTROL_LINE_OUT_*
									*   masks. This value is updated each time \ref CDC_Device_USBTask() is called.
									*/
			u16      DeviceToHost; /**< Control line states from the device to host, as a set of \c CDC_CONTROL_LINE_IN_*
									*   masks - to notify the host of changes to these values, call the
									*   \ref CDC_Device_SendControlLineStateChange() function.
									*/
		} ControlLineStates; /**< Current states of the virtual serial port's control lines between the device and host. */

		CDC_LineEncoding_t LineEncoding; /** Line encoding used in the virtual serial port, for the device's information.
										  *  This is generally only used if the virtual serial port data is to be
										  *  reconstructed on a physical UART.
										  */
	} State; /**< State data for the USB class interface within the device. All elements in this section
			  *   are reset to their defaults when the interface is enumerated.
			  */
} USB_ClassInfo_CDC_Device_t;


/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
	}
#endif


