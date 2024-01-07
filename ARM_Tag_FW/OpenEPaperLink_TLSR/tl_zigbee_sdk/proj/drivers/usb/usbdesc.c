/********************************************************************************************************
 * @file    usbdesc.c
 *
 * @brief   This is the source file for usbdesc
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

#include "usbdesc.h"
#include "usb_common.h"

#if (MODULE_USB_ENABLE)

// request parameters
/** Language descriptor structure. This descriptor, located in FLASH memory, is returned when the host requests
 *  the string descriptor with index 0 (the first index). It is actually an array of 16-bit integers, which indicate
 *  via the language ID table available at USB.org what languages the device supports for its string descriptors.
 */
const USB_Descriptor_String_t language_desc = {
	{sizeof(USB_Descriptor_Header_t) + 2, DTYPE_String}, // Header
	{LANGUAGE_ID_ENG}
};

/** Manufacturer descriptor string. This is a Unicode string containing the manufacturer's details in human readable
 *  form, and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
const USB_Descriptor_String_t vendor_desc = {
	{sizeof(USB_Descriptor_Header_t) + sizeof(STRING_VENDOR) - 2, DTYPE_String}, // Header
	{STRING_VENDOR}
};

/** Product descriptor string. This is a Unicode string containing the product's details in human readable form,
 *  and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
const USB_Descriptor_String_t product_desc = {
	{sizeof(USB_Descriptor_Header_t) + sizeof(STRING_PRODUCT) - 2, DTYPE_String}, // Header
	{STRING_PRODUCT}
};

/** Serial number string. This is a Unicode string containing the device's unique serial number, expressed as a
 *  series of uppercase hexadecimal digits.
 */
const USB_Descriptor_String_t serial_desc = {
	{sizeof(USB_Descriptor_Header_t) + sizeof(STRING_SERIAL) - 2, DTYPE_String}, // Header
	{STRING_SERIAL}
};

/** Device descriptor.
 */
#if (__PROJECT_TL_SNIFFER__)
USB_Descriptor_Device_t device_desc = SNIFFER_DEVICE_DESC;
#else
const USB_Descriptor_Device_t device_desc = {
	{sizeof(USB_Descriptor_Device_t), DTYPE_Device}, // Header

	0x0110, // USBSpecification, USB 1.1

#if (USB_CDC_ENABLE)
	USB_CSCP_IADDeviceClass, 	// Class
    USB_CSCP_IADDeviceSubclass, // SubClass
    USB_CSCP_IADDeviceProtocol, // Protocol
#else
    USB_CSCP_NoDeviceClass,		// Class
    USB_CSCP_NoDeviceSubclass,  // SubClass
    USB_CSCP_NoDeviceProtocol,  // Protocol
#endif

	8, 			// Endpoint0Size, Maximum Packet Size for Zero Endpoint. Valid Sizes are 8, 16, 32, 64
	ID_VENDOR, 	// VendorID
    ID_PRODUCT, // ProductID
    ID_VERSION, // ReleaseNumber
	USB_STRING_VENDOR, 	// .ManufacturerStrIndex
	USB_STRING_PRODUCT, // .ProductStrIndex
	USB_STRING_SERIAL, 	// .SerialNumStrIndex
	1					// .NumberOfConfigurations
};
#endif

/** Configuration descriptor.
 */
const USB_Descriptor_Configuration_t configuration_desc = {
	{
		{sizeof(USB_Descriptor_Configuration_Header_t), DTYPE_Configuration}, // Header
		sizeof(USB_Descriptor_Configuration_t), // TotalLength: variable
		USB_INTF_MAX, // NumInterfaces
		1, // Configuration index
		NO_DESCRIPTOR, // Configuration String
		USB_CONFIG_ATTR_RESERVED, // Attributes
		USB_CONFIG_POWER_MA(50) // MaxPower = 100mA
	},

#if (USB_CDC_ENABLE)
	{
		// iad0
		{sizeof(USB_Descriptor_Interface_Association_t), DTYPE_InterfaceAssociation}, // Header
		USB_INTF_CDC_CCI, // FirstInterfaceIndex
		2, // TotalInterface
		CDC_CSCP_CDCClass, // Class
		CDC_CSCP_ACMSubclass, // Subclass
		CDC_CSCP_ATCommandProtocol, // protocol
		NO_DESCRIPTOR  // IADStrIndex
	},

	{
		// cdc_interface
		{sizeof(USB_Descriptor_Interface_t), DTYPE_Interface}, // Header
		USB_INTF_CDC_CCI, // InterfaceNumber
		0, // AlternateSetting
		1, // TotalEndpoints
		CDC_CSCP_CDCClass, // Class
		CDC_CSCP_ACMSubclass, // SubClass
		CDC_CSCP_ATCommandProtocol, // Protocol
		NO_DESCRIPTOR //InterfaceStrIndex
	},

	{
		// cdc_descriptor
		// CDC_Functional_Header
		{
			{sizeof(USB_CDC_Descriptor_FunctionalHeader_t), DTYPE_CSInterface}, // Header
			CDC_DSUBTYPE_CSInterface_Header, // Subtype
			0x0110 // CDCSpecification
		},

		// CDC_Functional_ACM =
		{
			{sizeof(USB_CDC_Descriptor_FunctionalACM_t), DTYPE_CSInterface}, // Header
			CDC_DSUBTYPE_CSInterface_ACM, // Subtype
			0x02 // Capabilities
		},

		// CDC_Functional_Union =
		{
			{sizeof(USB_CDC_Descriptor_FunctionalUnion_t), DTYPE_CSInterface}, // Header
			CDC_DSUBTYPE_CSInterface_Union, // Subtype
			USB_INTF_CDC_CCI, // MasterInterfaceNumber
			USB_INTF_CDC_DCI, // SlaveInterfaceNumber
		},

		// CDC_CallManagement =
		{
			{sizeof(USB_CDC_Descriptor_FunctionalUnion_t), DTYPE_CSInterface}, // Header
			CDC_DSUBTYPE_CSInterface_CallManagement, // Subtype
			0, // MasterInterfaceNumber
			USB_INTF_CDC_DCI, // SlaveInterfaceNumber
		},

		// CDC_NotificationEndpoint =
		{
			{sizeof(USB_Descriptor_Endpoint_t), DTYPE_Endpoint}, // Header
			(ENDPOINT_DIR_IN | CDC_NOTIFICATION_EPNUM), // EndpointAddress
			(EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA), // Attributes
			CDC_NOTIFICATION_EPSIZE, // EndpointSize
			0x40 // PollingIntervalMS
		},

		// CDC_DCI_Interface =
		{
			{sizeof(USB_Descriptor_Interface_t), DTYPE_Interface}, // Header
			USB_INTF_CDC_DCI, // InterfaceNumber
			0, // AlternateSetting
			2, // TotalEndpoints
			CDC_CSCP_CDCDataClass, // Class
			CDC_CSCP_NoDataSubclass, // SubClass
			CDC_CSCP_NoDataProtocol, // Protocol
			NO_DESCRIPTOR // InterfaceStrIndex
		},

		// CDC_DataOutEndpoint =
		{
			{sizeof(USB_Descriptor_Endpoint_t), DTYPE_Endpoint}, // Header
			(ENDPOINT_DIR_OUT | CDC_RX_EPNUM), // EndpointAddress
			(EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA), // Attributes
			CDC_TXRX_EPSIZE, // EndpointSize
			0x00 // PollingIntervalMS
		},

		// CDC_DataInEndpoint =
		{
			{sizeof(USB_Descriptor_Endpoint_t),  DTYPE_Endpoint}, // Header
			(ENDPOINT_DIR_IN | CDC_TX_EPNUM), // EndpointAddress
			(EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA), // Attributes
			CDC_TXRX_EPSIZE, // EndpointSize
			0x00 // PollingIntervalMS
		},
	},
#endif /* USB_CDC_ENABLE */

#if (USB_MOUSE_ENABLE)
	{
		// mouse_interface
		{sizeof(USB_Descriptor_Interface_t), DTYPE_Interface}, // Header
		USB_INTF_MOUSE, // InterfaceNumber
		0, // AlternateSetting
		1, // bNumEndpoints
		HID_CSCP_HIDClass, // Class
		HID_CSCP_BootSubclass, // SubClass
		HID_CSCP_MouseBootProtocol, // Protocol
		NO_DESCRIPTOR //InterfaceStrIndex
	},

	{
		// mouse_hid
		{
			{sizeof(USB_HID_Descriptor_HID_t), HID_DTYPE_HID}, // Header
			0x0111, // BCDEncodedVersion
			USB_HID_COUNTRY_US, // CountryCode
			1, // NumberOfDescriptors
			HID_DTYPE_Report, // DescriptorType
			{sizeof(mouse_report_desc), 0x00}, // DescriptorLength
		},
		// mouse_in_endpoint
		{
			{sizeof(USB_Descriptor_Endpoint_t), DTYPE_Endpoint}, // Header
			ENDPOINT_DIR_IN | USB_EDP_MOUSE, // EndpointAddress
			EP_TYPE_INTERRUPT, // Attributes
			0x0008, // EndpointSize
			4 // PollingIntervalMS
		}
	},
#endif /* USB_MOUSE_ENABLE */
#if (USB_KEYBOARD_ENABLE)
	{
		// keyboardInterface
		{sizeof(USB_Descriptor_Interface_t), DTYPE_Interface}, // Header
		USB_INTF_KEYBOARD, // InterfaceNumber
		0, // AlternateSetting
		1, // bNumEndpoints
		HID_CSCP_HIDClass, // Class
		HID_CSCP_BootSubclass, // SubClass
		HID_CSCP_KeyboardBootProtocol, // Protocol
		NO_DESCRIPTOR //InterfaceStrIndex
	},

	{
		// keyboard_hid
		{
			{sizeof(USB_HID_Descriptor_HID_t), HID_DTYPE_HID}, // Header
			0x0111, // BCDEncodedVersion
			USB_HID_COUNTRY_US, // CountryCode
			1, // NumberOfDescriptors
			HID_DTYPE_Report, // DescriptorType
			{sizeof(keyboard_report_desc), 0x00}, // DescriptorLength
		},
		// keyboard_in_endpoint
		{
			{sizeof(USB_Descriptor_Endpoint_t), DTYPE_Endpoint}, // Header
			ENDPOINT_DIR_IN | USB_EDP_KEYBOARD_IN, // EndpointAddress
			EP_TYPE_INTERRUPT, // Attributes
			0x0008, // EndpointSize
			10 // PollingIntervalMS
		},
	},
#endif /* USB_KEYBOARD_ENABLE */

#if (USB_VENDOR_ENABLE)
	{
		// vendor_interface
		{sizeof(USB_Descriptor_Interface_t), DTYPE_Interface}, // Header
		USB_INTF_HID_VENDOR, // InterfaceNumber
		0, // AlternateSetting
		2, // TotalEndpoints
		HID_CSCP_HIDClass, // Class
		HID_CSCP_NonBootSubclass, // SubClass
		HID_CSCP_NonBootProtocol, // Protocol
		NO_DESCRIPTOR //InterfaceStrIndex
	},

	{
		// vendor_descriptor
		// vendor_hid =
		{
			{sizeof(USB_HID_Descriptor_HID_t), HID_DTYPE_HID}, // Header
			0x0111, // BCDEncodedVersion
			USB_HID_COUNTRY_US, // CountryCode
			1,	// NumberOfDescriptors
			HID_DTYPE_Report,	// DescriptorType
			{sizeof(vendor_report_desc), 0x00} // DescriptorLength
		},

		// vendor_in_endpoint =
		{
			{sizeof(USB_Descriptor_Endpoint_t), DTYPE_Endpoint}, // Header
			(ENDPOINT_DIR_IN | USB_EDP_HID_VENDOR_IN), // EndpointAddress
			EP_TYPE_INTERRUPT, // Attributes
			HID_VENDOR_TXRX_EPSIZE, // EndpointSize
			3 // PollingIntervalMS
		},

		// vendor_out_endpoint =
		{
			{sizeof(USB_Descriptor_Endpoint_t),  DTYPE_Endpoint}, // Header
			(ENDPOINT_DIR_OUT | USB_EDP_HID_VENDOR_OUT), // EndpointAddress
			EP_TYPE_INTERRUPT, // Attributes
			HID_VENDOR_TXRX_EPSIZE, // EndpointSize
			3 // PollingIntervalMS
		},
	},
#endif /* USB_VENDOR_ENABLE */

#if (__PROJECT_TL_SNIFFER__)
	SNIFFER_INTERFACE,
	SNIFFER_ENDPOINT,
#endif
};

u8 *usbdesc_get_language(void){
	return (u8 *)(&language_desc);
}

u8 *usbdesc_get_vendor(void){
	return (u8 *)(&vendor_desc);
}

u8 *usbdesc_get_product(void){
	return (u8 *)(&product_desc);
}

u8 *usbdesc_get_serial(void){
	return (u8 *)(&serial_desc);
}

u8 *usbdesc_get_device(void){
	return (u8 *)(&device_desc);
}

u8 *usbdesc_get_configuration(void){
	return (u8 *)(&configuration_desc);
}

#if (USB_MOUSE_ENABLE)
u8 *usbdesc_get_mouse(void){
	return (u8 *)(&configuration_desc.mouse_descriptor);
}
#endif /* USB_MOUSE_ENABLE */

#if (USB_KEYBOARD_ENABLE)
u8 *usbdesc_get_keyboard(void){
	return (u8 *)(&configuration_desc.keyboard_descriptor);
}
#endif /* USB_KEYBOARD_ENABLE */

#if (USB_VENDOR_ENABLE)
u8 *usbvendor_get_vendor_desc(void){
	return (u8 *)(&configuration_desc.vendor_descriptor);
}
#endif /* USB_VENDOR_ENABLE */

#endif
