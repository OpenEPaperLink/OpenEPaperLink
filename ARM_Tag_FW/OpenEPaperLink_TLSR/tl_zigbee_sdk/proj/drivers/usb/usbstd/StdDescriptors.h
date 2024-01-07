/********************************************************************************************************
 * @file    StdDescriptors.h
 *
 * @brief   This is the header file for StdDescriptors
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
/* Macros: */
/** Indicates that a given descriptor does not exist in the device. This can be used inside descriptors
 *  for string descriptor indexes, or may be use as a return value for GetDescriptor when the specified
 *  descriptor does not exist.
 */
#define NO_DESCRIPTOR                     0

/** Macro to calculate the power value for the configuration descriptor, from a given number of milliamperes.
 *
 *  \param[in] mA  Maximum number of milliamps the device consumes when the given configuration is selected.
 */
#define USB_CONFIG_POWER_MA(mA)           ((mA) >> 1)

/** Macro to calculate the Unicode length of a string with a given number of Unicode characters.
 *  Should be used in string descriptor's headers for giving the string descriptor's byte length.
 *
 *  \param[in] UnicodeChars  Number of Unicode characters in the string text.
 */
#define USB_STRING_LEN(UnicodeChars)      (sizeof(USB_Descriptor_Header_t) + ((UnicodeChars) << 1))

/** String language ID for the English language. Should be used in \ref USB_Descriptor_String_t descriptors
 *  to indicate that the English language is supported by the device in its string descriptors.
 */
#define LANGUAGE_ID_ENG                   0x0409

/** \name USB Configuration Descriptor Attribute Masks */
//@{
/** Mask for the reserved bit in the Configuration Descriptor's \c ConfigAttributes field, which must be set on all
 *  devices for historial purposes.
 */
#define USB_CONFIG_ATTR_RESERVED          0x80
/** Can be masked with other configuration descriptor attributes for a \ref USB_Descriptor_Configuration_Header_t
 *  descriptor's \c ConfigAttributes value to indicate that the specified configuration can draw its power
 *  from the device's own power source.
 */
#define USB_CONFIG_ATTR_SELFPOWERED       0x40

/** Can be masked with other configuration descriptor attributes for a \ref USB_Descriptor_Configuration_Header_t
 *  descriptor's \c ConfigAttributes value to indicate that the specified configuration supports the
 *  remote wakeup feature of the USB standard, allowing a suspended USB device to wake up the host upon
 *  request.
 */
#define USB_CONFIG_ATTR_REMOTEWAKEUP      0x20
//@}

/** \name Endpoint Descriptor Attribute Masks */
//@{
/** Can be masked with other endpoint descriptor attributes for a \ref USB_Descriptor_Endpoint_t descriptor's
 *  \c Attributes value to indicate that the specified endpoint is not synchronized.
 *
 *  \see The USB specification for more details on the possible Endpoint attributes.
 */
#define ENDPOINT_ATTR_NO_SYNC             (0 << 2)

/** Can be masked with other endpoint descriptor attributes for a \ref USB_Descriptor_Endpoint_t descriptor's
 *  \c Attributes value to indicate that the specified endpoint is asynchronous.
 *
 *  \see The USB specification for more details on the possible Endpoint attributes.
 */
#define ENDPOINT_ATTR_ASYNC               (1 << 2)

/** Can be masked with other endpoint descriptor attributes for a \ref USB_Descriptor_Endpoint_t descriptor's
 *  \c Attributes value to indicate that the specified endpoint is adaptive.
 *
 *  \see The USB specification for more details on the possible Endpoint attributes.
 */
#define ENDPOINT_ATTR_ADAPTIVE            (2 << 2)

/** Can be masked with other endpoint descriptor attributes for a \ref USB_Descriptor_Endpoint_t descriptor's
 *  \c Attributes value to indicate that the specified endpoint is synchronized.
 *
 *  \see The USB specification for more details on the possible Endpoint attributes.
 */
#define ENDPOINT_ATTR_SYNC                (3 << 2)
//@}

/** \name Endpoint Descriptor Usage Masks */
//@{
/** Can be masked with other endpoint descriptor attributes for a \ref USB_Descriptor_Endpoint_t descriptor's
 *  \c Attributes value to indicate that the specified endpoint is used for data transfers.
 *
 *  \see The USB specification for more details on the possible Endpoint usage attributes.
 */
#define ENDPOINT_USAGE_DATA               (0 << 4)

/** Can be masked with other endpoint descriptor attributes for a \ref USB_Descriptor_Endpoint_t descriptor's
 *  \c Attributes value to indicate that the specified endpoint is used for feedback.
 *
 *  \see The USB specification for more details on the possible Endpoint usage attributes.
 */
#define ENDPOINT_USAGE_FEEDBACK           (1 << 4)

/** Can be masked with other endpoint descriptor attributes for a \ref USB_Descriptor_Endpoint_t descriptor's
 *  \c Attributes value to indicate that the specified endpoint is used for implicit feedback.
 *
 *  \see The USB specification for more details on the possible Endpoint usage attributes.
 */
#define ENDPOINT_USAGE_IMPLICIT_FEEDBACK  (2 << 4)
//@}

/* Enums: */
/** Enum for the possible standard descriptor types, as given in each descriptor's header. */
enum USB_DescriptorTypes_t
{
	DTYPE_Device                    = 0x01, /**< Indicates that the descriptor is a device descriptor. */
	DTYPE_Configuration             = 0x02, /**< Indicates that the descriptor is a configuration descriptor. */
	DTYPE_String                    = 0x03, /**< Indicates that the descriptor is a string descriptor. */
	DTYPE_Interface                 = 0x04, /**< Indicates that the descriptor is an interface descriptor. */
	DTYPE_Endpoint                  = 0x05, /**< Indicates that the descriptor is an endpoint descriptor. */
	DTYPE_DeviceQualifier           = 0x06, /**< Indicates that the descriptor is a device qualifier descriptor. */
	DTYPE_Other                     = 0x07, /**< Indicates that the descriptor is of other type. */
	DTYPE_InterfacePower            = 0x08, /**< Indicates that the descriptor is an interface power descriptor. */
	DTYPE_InterfaceAssociation      = 0x0B, /**< Indicates that the descriptor is an interface association descriptor. */
	DTYPE_CSInterface               = 0x24, /**< Indicates that the descriptor is a class specific interface descriptor. */
	DTYPE_CSEndpoint                = 0x25, /**< Indicates that the descriptor is a class specific endpoint descriptor. */
};

/** Enum for possible Class, Subclass and Protocol values of device and interface descriptors. */
enum USB_Descriptor_ClassSubclassProtocol_t
{
	USB_CSCP_NoDeviceClass          = 0x00, /**< Descriptor Class value indicating that the device does not belong
	                                         *   to a particular class at the device level.
	                                         */
	USB_CSCP_NoDeviceSubclass       = 0x00, /**< Descriptor Subclass value indicating that the device does not belong
	                                         *   to a particular subclass at the device level.
	                                         */
	USB_CSCP_NoDeviceProtocol       = 0x00, /**< Descriptor Protocol value indicating that the device does not belong
	                                         *   to a particular protocol at the device level.
	                                         */
	USB_CSCP_VendorSpecificClass    = 0xFF, /**< Descriptor Class value indicating that the device/interface belongs
	                                         *   to a vendor specific class.
	                                         */
	USB_CSCP_VendorSpecificSubclass = 0xFF, /**< Descriptor Subclass value indicating that the device/interface belongs
	                                         *   to a vendor specific subclass.
	                                         */
	USB_CSCP_VendorSpecificProtocol = 0xFF, /**< Descriptor Protocol value indicating that the device/interface belongs
	                                         *   to a vendor specific protocol.
	                                         */
	USB_CSCP_IADDeviceClass         = 0xEF, /**< Descriptor Class value indicating that the device belongs to the
	                                         *   Interface Association Descriptor class.
	                                         */
	USB_CSCP_IADDeviceSubclass      = 0x02, /**< Descriptor Subclass value indicating that the device belongs to the
	                                         *   Interface Association Descriptor subclass.
	                                         */
	USB_CSCP_IADDeviceProtocol      = 0x01, /**< Descriptor Protocol value indicating that the device belongs to the
	                                         *   Interface Association Descriptor protocol.
	                                         */
};

/* Type Defines: */
/** \brief Standard USB Descriptor Header (LUFA naming conventions).
 *
 *  Type define for all descriptors' standard header, indicating the descriptor's length and type. This structure
 *  uses LUFA-specific element names to make each element's purpose clearer.
 *
 *  \see \ref USB_StdDescriptor_Header_t for the version of this type with standard element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	u8 Size; /**< Size of the descriptor, in bytes. */
	u8 Type; /**< Type of the descriptor, either a value in \ref USB_DescriptorTypes_t or a value
	               *   given by the specific class.
	               */
}USB_Descriptor_Header_t;

/** \brief Standard USB Descriptor Header (USB-IF naming conventions).
 *
 *  Type define for all descriptors' standard header, indicating the descriptor's length and type. This structure
 *  uses the relevant standard's given element names to ensure compatibility with the standard.
 *
 *  \see \ref USB_Descriptor_Header_t for the version of this type with non-standard LUFA specific element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	u8 bLength; /**< Size of the descriptor, in bytes. */
	u8 bDescriptorType; /**< Type of the descriptor, either a value in \ref USB_DescriptorTypes_t or a value
	                          *   given by the specific class.
	                          */
}USB_StdDescriptor_Header_t;

/** \brief Standard USB Device Descriptor (LUFA naming conventions).
 *
 *  Type define for a standard Device Descriptor. This structure uses LUFA-specific element names to make each
 *  element's purpose clearer.
 *
 *  \see \ref USB_StdDescriptor_Device_t for the version of this type with standard element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	USB_Descriptor_Header_t Header; /**< Descriptor header, including type and size. */

	u16 USBSpecification; /**< BCD of the supported USB specification. */
	u8  Class; /**< USB device class. */
	u8  SubClass; /**< USB device subclass. */
	u8  Protocol; /**< USB device protocol. */

	u8  Endpoint0Size; /**< Size of the control (address 0) endpoint's bank in bytes. */

	u16 VendorID; /**< Vendor ID for the USB product. */
	u16 ProductID; /**< Unique product ID for the USB product. */
	u16 ReleaseNumber; /**< Product release (version) number. */

	u8  ManufacturerStrIndex; /**< String index for the manufacturer's name. The
	                                *   host will request this string via a separate
	                                *   control request for the string descriptor.
	                                *
	                                *   \note If no string supplied, use \ref NO_DESCRIPTOR.
	                                */
	u8  ProductStrIndex; /**< String index for the product name/details.
	                           *
	                           *  \see ManufacturerStrIndex structure entry.
	                           */
	u8  SerialNumStrIndex; /**< String index for the product's globally unique hexadecimal
	                             *   serial number, in uppercase Unicode ASCII.
	                             *
	                             *  \note On some microcontroller models, there is an embedded serial number
	                             *        in the chip which can be used for the device serial number.
	                             *        To use this serial number, set this to USE_INTERNAL_SERIAL.
	                             *        On unsupported devices, this will evaluate to 0 and will cause
	                             *        the host to generate a pseudo-unique value for the device upon
	                             *        insertion.
	                             *
	                             *  \see ManufacturerStrIndex structure entry.
	                             */
	u8  NumberOfConfigurations; /**< Total number of configurations supported by
	                                  *   the device.
	                                  */
}USB_Descriptor_Device_t;

/** \brief Standard USB Device Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard Device Descriptor. This structure uses the relevant standard's given element names
 *  to ensure compatibility with the standard.
 *
 *  \see \ref USB_Descriptor_Device_t for the version of this type with non-standard LUFA specific element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	u8  bLength; /**< Size of the descriptor, in bytes. */
	u8  bDescriptorType; /**< Type of the descriptor, either a value in \ref USB_DescriptorTypes_t or a value
	                              *   given by the specific class.
	                              */
	u16 bcdUSB; /**< BCD of the supported USB specification. */
	u8  bDeviceClass; /**< USB device class. */
	u8  bDeviceSubClass; /**< USB device subclass. */
	u8  bDeviceProtocol; /**< USB device protocol. */
	u8  bMaxPacketSize0; /**< Size of the control (address 0) endpoint's bank in bytes. */
	u16 idVendor; /**< Vendor ID for the USB product. */
	u16 idProduct; /**< Unique product ID for the USB product. */
	u16 bcdDevice; /**< Product release (version) number. */
	u8  iManufacturer; /**< String index for the manufacturer's name. The
	                         *   host will request this string via a separate
	                         *   control request for the string descriptor.
	                         *
	                         *   \note If no string supplied, use \ref NO_DESCRIPTOR.
	                         */
	u8  iProduct; /**< String index for the product name/details.
	                    *
	                    *  \see ManufacturerStrIndex structure entry.
	                    */
	u8 iSerialNumber; /**< String index for the product's globally unique hexadecimal
	                        *   serial number, in uppercase Unicode ASCII.
	                        *
	                        *  \note On some microcontroller models, there is an embedded serial number
	                        *        in the chip which can be used for the device serial number.
	                        *        To use this serial number, set this to USE_INTERNAL_SERIAL.
	                        *        On unsupported devices, this will evaluate to 0 and will cause
	                        *        the host to generate a pseudo-unique value for the device upon
	                        *        insertion.
	                        *
	                        *  \see ManufacturerStrIndex structure entry.
	                        */
	u8  bNumConfigurations; /**< Total number of configurations supported by
	                              *   the device.
	                              */
}USB_StdDescriptor_Device_t;

/** \brief Standard USB Device Qualifier Descriptor (LUFA naming conventions).
 *
 *  Type define for a standard Device Qualifier Descriptor. This structure uses LUFA-specific element names
 *  to make each element's purpose clearer.
 *
 *  \see \ref USB_StdDescriptor_DeviceQualifier_t for the version of this type with standard element names.
 */
typedef struct
{
	USB_Descriptor_Header_t Header; /**< Descriptor header, including type and size. */

	u16 USBSpecification; /**< BCD of the supported USB specification. */
	u8  Class; /**< USB device class. */
	u8  SubClass; /**< USB device subclass. */
	u8  Protocol; /**< USB device protocol. */

	u8  Endpoint0Size; /**< Size of the control (address 0) endpoint's bank in bytes. */
	u8  NumberOfConfigurations; /**< Total number of configurations supported by
	                                  *   the device.
	                                  */
	u8  Reserved; /**< Reserved for future use, must be 0. */
}USB_Descriptor_DeviceQualifier_t;

/** \brief Standard USB Device Qualifier Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard Device Qualifier Descriptor. This structure uses the relevant standard's given element names
 *  to ensure compatibility with the standard.
 *
 *  \see \ref USB_Descriptor_DeviceQualifier_t for the version of this type with non-standard LUFA specific element names.
 */
typedef struct
{
	u8  bLength; /**< Size of the descriptor, in bytes. */
	u8  bDescriptorType; /**< Type of the descriptor, either a value in \ref USB_DescriptorTypes_t or a value
	                              *   given by the specific class.
	                              */
	u16 bcdUSB; /**< BCD of the supported USB specification. */
	u8  bDeviceClass; /**< USB device class. */
	u8  bDeviceSubClass; /**< USB device subclass. */
	u8  bDeviceProtocol; /**< USB device protocol. */
	u8  bMaxPacketSize0; /**< Size of the control (address 0) endpoint's bank in bytes. */
	u8  bNumConfigurations; /**< Total number of configurations supported by
	                              *   the device.
	                              */
	u8  bReserved; /**< Reserved for future use, must be 0. */
}USB_StdDescriptor_DeviceQualifier_t;

/** \brief Standard USB Configuration Descriptor (LUFA naming conventions).
 *
 *  Type define for a standard Configuration Descriptor header. This structure uses LUFA-specific element names
 *  to make each element's purpose clearer.
 *
 *  \see \ref USB_StdDescriptor_Configuration_Header_t for the version of this type with standard element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	USB_Descriptor_Header_t Header; /**< Descriptor header, including type and size. */

	u16 TotalConfigurationSize; /**< Size of the configuration descriptor header,
	                                  *   and all sub descriptors inside the configuration.
	                                  */
	u8  TotalInterfaces; /**< Total number of interfaces in the configuration. */

	u8  ConfigurationNumber; /**< Configuration index of the current configuration. */
	u8  ConfigurationStrIndex; /**< Index of a string descriptor describing the configuration. */

	u8  ConfigAttributes; /**< Configuration attributes, comprised of a mask of \c USB_CONFIG_ATTR_* masks.
	                            *   On all devices, this should include USB_CONFIG_ATTR_RESERVED at a minimum.
	                            */

	u8  MaxPowerConsumption; /**< Maximum power consumption of the device while in the
	                               *   current configuration, calculated by the \ref USB_CONFIG_POWER_MA()
	                               *   macro.
	                               */
}USB_Descriptor_Configuration_Header_t;

/** \brief Standard USB Configuration Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard Configuration Descriptor header. This structure uses the relevant standard's given element names
 *  to ensure compatibility with the standard.
 *
 *  \see \ref USB_Descriptor_Device_t for the version of this type with non-standard LUFA specific element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	u8  bLength; /**< Size of the descriptor, in bytes. */
	u8  bDescriptorType; /**< Type of the descriptor, either a value in \ref USB_DescriptorTypes_t or a value
	                              *   given by the specific class.
	                              */
	u16 wTotalLength; /**< Size of the configuration descriptor header,
	                           *   and all sub descriptors inside the configuration.
	                           */
	u8  bNumInterfaces; /**< Total number of interfaces in the configuration. */
	u8  bConfigurationValue; /**< Configuration index of the current configuration. */
	u8  iConfiguration; /**< Index of a string descriptor describing the configuration. */
	u8  bmAttributes; /**< Configuration attributes, comprised of a mask of \c USB_CONFIG_ATTR_* masks.
	                        *   On all devices, this should include USB_CONFIG_ATTR_RESERVED at a minimum.
	                        */
	u8  bMaxPower; /**< Maximum power consumption of the device while in the
	                     *   current configuration, calculated by the \ref USB_CONFIG_POWER_MA()
	                     *   macro.
	                     */
}USB_StdDescriptor_Configuration_Header_t;

/** \brief Standard USB Interface Descriptor (LUFA naming conventions).
 *
 *  Type define for a standard Interface Descriptor. This structure uses LUFA-specific element names
 *  to make each element's purpose clearer.
 *
 *  \see \ref USB_StdDescriptor_Interface_t for the version of this type with standard element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	USB_Descriptor_Header_t Header; /**< Descriptor header, including type and size. */

	u8 InterfaceNumber; /**< Index of the interface in the current configuration. */
	u8 AlternateSetting; /**< Alternate setting for the interface number. The same
	                           *   interface number can have multiple alternate settings
	                           *   with different endpoint configurations, which can be
	                           *   selected by the host.
	                           */
	u8 TotalEndpoints; /**< Total number of endpoints in the interface. */

	u8 Class; /**< Interface class ID. */
	u8 SubClass; /**< Interface subclass ID. */
	u8 Protocol; /**< Interface protocol ID. */

	u8 InterfaceStrIndex; /**< Index of the string descriptor describing the interface. */
}USB_Descriptor_Interface_t;

/** \brief Standard USB Interface Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard Interface Descriptor. This structure uses the relevant standard's given element names
 *  to ensure compatibility with the standard.
 *
 *  \see \ref USB_Descriptor_Interface_t for the version of this type with non-standard LUFA specific element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	u8 bLength; /**< Size of the descriptor, in bytes. */
	u8 bDescriptorType; /**< Type of the descriptor, either a value in \ref USB_DescriptorTypes_t or a value
	                          *   given by the specific class.
	                          */
	u8 bInterfaceNumber; /**< Index of the interface in the current configuration. */
	u8 bAlternateSetting; /**< Alternate setting for the interface number. The same
	                            *   interface number can have multiple alternate settings
	                            *   with different endpoint configurations, which can be
	                            *   selected by the host.
	                            */
	u8 bNumEndpoints; /**< Total number of endpoints in the interface. */
	u8 bInterfaceClass; /**< Interface class ID. */
	u8 bInterfaceSubClass; /**< Interface subclass ID. */
	u8 bInterfaceProtocol; /**< Interface protocol ID. */
	u8 iInterface; /**< Index of the string descriptor describing the
	                     *   interface.
	                     */
}USB_StdDescriptor_Interface_t;

/** \brief Standard USB Interface Association Descriptor (LUFA naming conventions).
 *
 *  Type define for a standard Interface Association Descriptor. This structure uses LUFA-specific element names
 *  to make each element's purpose clearer.
 *
 *  This descriptor has been added as a supplement to the USB2.0 standard, in the ECN located at
 *  <a>http://www.usb.org/developers/docs/InterfaceAssociationDescriptor_ecn.pdf</a>. It allows composite
 *  devices with multiple interfaces related to the same function to have the multiple interfaces bound
 *  together at the point of enumeration, loading one generic driver for all the interfaces in the single
 *  function. Read the ECN for more information.
 *
 *  \see \ref USB_StdDescriptor_Interface_Association_t for the version of this type with standard element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	USB_Descriptor_Header_t Header; /**< Descriptor header, including type and size. */

	u8 FirstInterfaceIndex; /**< Index of the first associated interface. */
	u8 TotalInterfaces; /**< Total number of associated interfaces. */

	u8 Class; /**< Interface class ID. */
	u8 SubClass; /**< Interface subclass ID. */
	u8 Protocol; /**< Interface protocol ID. */

	u8 IADStrIndex; /**< Index of the string descriptor describing the
	                      *   interface association.
	                      */
}USB_Descriptor_Interface_Association_t;

/** \brief Standard USB Interface Association Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard Interface Association Descriptor. This structure uses the relevant standard's given
 *  element names to ensure compatibility with the standard.
 *
 *  This descriptor has been added as a supplement to the USB2.0 standard, in the ECN located at
 *  <a>http://www.usb.org/developers/docs/InterfaceAssociationDescriptor_ecn.pdf</a>. It allows composite
 *  devices with multiple interfaces related to the same function to have the multiple interfaces bound
 *  together at the point of enumeration, loading one generic driver for all the interfaces in the single
 *  function. Read the ECN for more information.
 *
 *  \see \ref USB_Descriptor_Interface_Association_t for the version of this type with non-standard LUFA specific
 *       element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	u8 bLength; /**< Size of the descriptor, in bytes. */
	u8 bDescriptorType; /**< Type of the descriptor, either a value in \ref USB_DescriptorTypes_t or a value
	                          *   given by the specific class.
	                          */
	u8 bFirstInterface; /**< Index of the first associated interface. */
	u8 bInterfaceCount; /**< Total number of associated interfaces. */
	u8 bFunctionClass; /**< Interface class ID. */
	u8 bFunctionSubClass; /**< Interface subclass ID. */
	u8 bFunctionProtocol; /**< Interface protocol ID. */
	u8 iFunction; /**< Index of the string descriptor describing the
	                    *   interface association.
	                    */
}USB_StdDescriptor_Interface_Association_t;

/** \brief Standard USB Endpoint Descriptor (LUFA naming conventions).
 *
 *  Type define for a standard Endpoint Descriptor. This structure uses LUFA-specific element names
 *  to make each element's purpose clearer.
 *
 *  \see \ref USB_StdDescriptor_Endpoint_t for the version of this type with standard element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	USB_Descriptor_Header_t Header; /**< Descriptor header, including type and size. */

	u8  EndpointAddress; /**< Logical address of the endpoint within the device for the current
	                           *   configuration, including direction mask.
	                           */
	u8  Attributes; /**< Endpoint attributes, comprised of a mask of the endpoint type (EP_TYPE_*)
	                      *   and attributes (ENDPOINT_ATTR_*) masks.
	                      */
	u16 EndpointSize; /**< Size of the endpoint bank, in bytes. This indicates the maximum packet
	                        *   size that the endpoint can receive at a time.
	                        */
	u8  PollingIntervalMS; /**< Polling interval in milliseconds for the endpoint if it is an INTERRUPT
	                             *   or ISOCHRONOUS type.
	                             */
}USB_Descriptor_Endpoint_t;

/** \brief Standard USB Endpoint Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard Endpoint Descriptor. This structure uses the relevant standard's given
 *  element names to ensure compatibility with the standard.
 *
 *  \see \ref USB_Descriptor_Endpoint_t for the version of this type with non-standard LUFA specific
 *       element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	u8  bLength; /**< Size of the descriptor, in bytes. */
	u8  bDescriptorType; /**< Type of the descriptor, either a value in \ref USB_DescriptorTypes_t or a
	                           *   value given by the specific class.
	                           */
	u8  bEndpointAddress; /**< Logical address of the endpoint within the device for the current
	                            *   configuration, including direction mask.
	                            */
	u8  bmAttributes; /**< Endpoint attributes, comprised of a mask of the endpoint type (EP_TYPE_*)
	                        *   and attributes (ENDPOINT_ATTR_*) masks.
	                        */
	u16 wMaxPacketSize; /**< Size of the endpoint bank, in bytes. This indicates the maximum packet size
	                          *   that the endpoint can receive at a time.
	                          */
	u8  bInterval; /**< Polling interval in milliseconds for the endpoint if it is an INTERRUPT or
	                     *   ISOCHRONOUS type.
	                     */
}USB_StdDescriptor_Endpoint_t;

/** \brief Standard USB String Descriptor (LUFA naming conventions).
 *
 *  Type define for a standard string descriptor. Unlike other standard descriptors, the length
 *  of the descriptor for placement in the descriptor header must be determined by the \ref USB_STRING_LEN()
 *  macro rather than by the size of the descriptor structure, as the length is not fixed.
 *
 *  This structure should also be used for string index 0, which contains the supported language IDs for
 *  the device as an array.
 *
 *  This structure uses LUFA-specific element names to make each element's purpose clearer.
 *
 *  \see \ref USB_StdDescriptor_String_t for the version of this type with standard element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
#ifndef __GNUC__
#pragma warning(push)
#pragma warning(disable:4200)	// none standard zero length array
#endif

typedef struct
{
	USB_Descriptor_Header_t Header; /**< Descriptor header, including type and size. */
	wchar_t UnicodeString[]; /**< String data, as unicode characters (alternatively,
	                           *   string language IDs). If normal ASCII characters are
	                           *   to be used, they must be added as an array of characters
	                           *   rather than a normal C string so that they are widened to
	                           *   Unicode size.
	                           *
	                           *   Under GCC, strings prefixed with the "L" character (before
	                           *   the opening string quotation mark) are considered to be
	                           *   Unicode strings, and may be used instead of an explicit
	                           *   array of ASCII characters on little endian devices with
	                           *   UTF-16-LE \c wchar_t encoding.
	                           */
}USB_Descriptor_String_t;

/** \brief Standard USB String Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard string descriptor. Unlike other standard descriptors, the length
 *  of the descriptor for placement in the descriptor header must be determined by the \ref USB_STRING_LEN()
 *  macro rather than by the size of the descriptor structure, as the length is not fixed.
 *
 *  This structure should also be used for string index 0, which contains the supported language IDs for
 *  the device as an array.
 *
 *  This structure uses the relevant standard's given element names to ensure compatibility with the standard.
 *
 *  \see \ref USB_Descriptor_String_t for the version of this type with with non-standard LUFA specific
 *       element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	u8 bLength; /**< Size of the descriptor, in bytes. */
	u8 bDescriptorType; /**< Type of the descriptor, either a value in \ref USB_DescriptorTypes_t
	                          *   or a value given by the specific class.
	                          */
	wchar_t bString[]; /**< String data, as unicode characters (alternatively, string language IDs).
	                     *   If normal ASCII characters are to be used, they must be added as an array
	                     *   of characters rather than a normal C string so that they are widened to
	                     *   Unicode size.
	                     *
	                     *   Under GCC, strings prefixed with the "L" character (before the opening string
	                     *   quotation mark) are considered to be Unicode strings, and may be used instead
	                     *   of an explicit array of ASCII characters.
	                     */
}USB_StdDescriptor_String_t;

#ifndef __GNUC__
#pragma warning(pop)
#endif

/** \brief HID class-specific HID Descriptor (LUFA naming conventions).
 *
 *	Type define for the HID class-specific HID descriptor, to describe the HID device's specifications. Refer to the HID
 *	specification for details on the structure elements.
 *
 *	\see \ref USB_HID_StdDescriptor_HID_t for the version of this type with standard element names.
 *
 *	\note Regardless of CPU architecture, these values should be stored as little endian.
 */

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
	}
#endif



