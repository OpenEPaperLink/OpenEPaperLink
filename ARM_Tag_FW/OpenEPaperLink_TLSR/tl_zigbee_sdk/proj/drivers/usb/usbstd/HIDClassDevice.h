/********************************************************************************************************
 * @file    HIDClassDevice.h
 *
 * @brief   This is the header file for HIDClassDevice
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

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
	extern "C" {
#endif

/* Public Interface - May be used in end-application: */
/* Type Defines: */
/** \brief HID Class Device Mode Configuration and State Structure.
 *
 *  Class state structure. An instance of this structure should be made for each HID interface
 *  within the user application, and passed to each of the HID class driver functions as the
 *  \c HIDInterfaceInfo parameter. This stores each HID interface's configuration and state information.
 *
 *  \note Due to technical limitations, the HID device class driver does not utilize a separate OUT
 *        endpoint for host->device communications. Instead, the host->device data (if any) is sent to
 *        the device via the control endpoint.
 */
typedef struct usbhid_config_t
{
	u8  InterfaceNumber; /**< Interface number of the HID interface within the device. */

	u8  ReportINEndpointNumber; /**< Endpoint number of the HID interface's IN report endpoint. */
	u16 ReportINEndpointSize; /**< Size in bytes of the HID interface's IN report endpoint. */
	bool ReportINEndpointDoubleBank; /**< Indicates if the HID interface's IN report endpoint should use double banking. */

	void* PrevReportINBuffer; /**< Pointer to a buffer where the previously created HID input report can be
	                              *  stored by the driver, for comparison purposes to detect report changes that
	                              *  must be sent immediately to the host. This should point to a buffer big enough
	                              *  to hold the largest HID input report sent from the HID interface. If this is set
								  *  to \c NULL, it is up to the user to force transfers when needed in the
								  *  \ref CALLBACK_HID_Device_CreateHIDReport() callback function.
								  *
								  *  \note Due to the single buffer, the internal driver can only correctly compare
								  *        subsequent reports with identical report IDs. In multiple report devices,
								  *        this buffer should be set to \c NULL and the decision to send reports made
								  *        by the user application instead.
	                              */
	u8  PrevReportINBufferSize; /**< Size in bytes of the given input report buffer. This is used to create a
	                                  *  second buffer of the same size within the driver so that subsequent reports
	                                  *  can be compared. If the user app is to determine when reports are to be sent
	                                  *  exclusively (i.e. \ref PrevReportINBuffer is \c NULL) this value must still be
									  *  set to the size of the largest report the device can issue to the host.
	                                  */
} usbhid_config_t; /**< Config data for the USB class interface within the device. All elements in this section
           *   <b>must</b> be set or the interface will fail to enumerate and operate correctly.
           */
typedef struct usbhid_state_t
{
	bool UsingReportProtocol; /**< Indicates if the HID interface is set to Boot or Report proto mode. */
	u16 PrevFrameNum; /**< Frame number of the previous HID report packet opportunity. */
	u16 IdleCount; /**< Report idle period, in milliseconds, set by the host. */
	u16 IdleMSRemaining; /**< Total number of milliseconds remaining before the idle period elapsed - this
							   *   should be decremented by the user application if non-zero each millisecond. */
} usbhid_state_t; /**< State data for the USB class interface within the device. All elements in this section
          *   are reset to their defaults when the interface is enumerated.
          */
typedef struct
{
	
} USB_ClassInfo_HID_Device_t;


/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
	}
#endif



