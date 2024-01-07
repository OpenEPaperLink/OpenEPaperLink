/********************************************************************************************************
 * @file    usbdesc.h
 *
 * @brief   This is the header file for usbdesc
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
#include "tl_common.h"
#include "usbstd/usbstd.h"

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

#ifndef STRING_VENDOR
#define STRING_VENDOR       		L"Telink"
#endif
#ifndef STRING_PRODUCT
#define STRING_PRODUCT      		L"TelinkUSBDongle"
#endif
#ifndef STRING_SERIAL
#define STRING_SERIAL       		L"TLSR82xx"
#endif
#ifndef ID_VENDOR
#define ID_VENDOR					0x248A
#endif
#ifndef ID_VERSION
#define ID_VERSION					0x0100
#endif
#ifndef ID_PRODUCT_BASE
#define ID_PRODUCT_BASE				0x8800
#endif
#ifndef USB_PRINTER_ENABLE
#define USB_PRINTER_ENABLE			0
#endif
#ifndef USB_SPEAKER_ENABLE
#define USB_SPEAKER_ENABLE			0
#endif
#ifndef USB_MIC_ENABLE
#define USB_MIC_ENABLE				0
#endif
#ifndef USB_MOUSE_ENABLE
#define USB_MOUSE_ENABLE			0
#endif
#ifndef USB_KEYBOARD_ENABLE
#define USB_KEYBOARD_ENABLE			0
#endif
#ifndef USB_SOMATIC_ENABLE
#define USB_SOMATIC_ENABLE			0
#endif
#ifndef USB_CDC_ENABLE
#define USB_CDC_ENABLE				0
#endif
#ifndef USB_VENDOR_ENABLE
#define USB_VENDOR_ENABLE			0
#endif
#ifndef ID_PRODUCT
#define ID_PRODUCT					(ID_PRODUCT_BASE | (USB_PRINTER_ENABLE?(1<<0):0) | (USB_SPEAKER_ENABLE?(1<<1):0) | (USB_MIC_ENABLE?(1<<2):0) |	\
									(USB_MOUSE_ENABLE?(1<<3):0) | (USB_KEYBOARD_ENABLE?(1<<4):0) | (USB_SOMATIC_ENABLE?(1<<5):0) | (USB_CDC_ENABLE?(1<<6):0) | \
									(USB_VENDOR_ENABLE?(1<<7):0))
#endif

// interface id
typedef enum {
#if (USB_CDC_ENABLE)
	USB_INTF_CDC_CCI,
	USB_INTF_CDC_DCI,
#endif /* USB_CDC_ENABLE */

#if (USB_KEYBOARD_ENABLE)
	USB_INTF_KEYBOARD,
#endif /* USB_KEYBOARD_ENABLE */

#if (USB_MOUSE_ENABLE)
	USB_INTF_MOUSE,
#endif /* USB_MOUSE_ENABLE */

#if (USB_VENDOR_ENABLE)
	USB_INTF_HID_VENDOR,
#endif /* USB_VENDOR_ENABLE */

#if (__PROJECT_TL_SNIFFER__)
	USB_INTF_SNIFFER,
#endif

	USB_INTF_MAX,
} USB_INTF_ID_E;

#if (USB_CDC_ENABLE)
/** Endpoint number of the CDC device-to-host notification IN endpoint. */
#ifndef CDC_NOTIFICATION_EPNUM
#define CDC_NOTIFICATION_EPNUM         2
#endif
/** Endpoint number of the CDC device-to-host data IN endpoint. */
#ifndef CDC_TX_EPNUM
#define CDC_TX_EPNUM                   USB_EDP_CDC_IN//4
#endif
/** Endpoint number of the CDC host-to-device data OUT endpoint. */
#ifndef CDC_RX_EPNUM
#define CDC_RX_EPNUM                   USB_EDP_CDC_OUT//5
#endif
/** Size in bytes of the CDC device-to-host notification IN endpoint. */
#define CDC_NOTIFICATION_EPSIZE        8
/** Size in bytes of the CDC data IN and OUT endpoints. */
#define CDC_TXRX_EPSIZE                64
#endif /* USB_CDC_ENABLE */

#if (USB_VENDOR_ENABLE)
/** Endpoint number of the HID Vendor device-to-host data IN endpoint. */
#define USB_EDP_HID_VENDOR_IN          4
/** Endpoint number of the HID Vendor host-to-device data OUT endpoint. */
#define USB_EDP_HID_VENDOR_OUT         5
/** Size in bytes of the HID Vendor data IN and OUT endpoints. */
#define HID_VENDOR_TXRX_EPSIZE         64
#endif /* USB_VENDOR_ENABLE */

#if (__PROJECT_TL_SNIFFER__)
#define SNIFFER_TX_EPNUM               3
#define SNIFFER_TX_EPSIZE         	   64
#endif

enum {
	USB_AUDIO_FORMAT_UNKNOWN 				= 0,
	USB_AUDIO_FORMAT_PCM,
	USB_AUDIO_FORMAT_ADPCM,
	USB_AUDIO_FORMAT_IEEE_FLOAT,
	USB_AUDIO_FORMAT_IBM_CVSD,
	USB_AUDIO_FORMAT_ALAW,
	USB_AUDIO_FORMAT_MULAW,
	USB_AUDIO_FORMAT_WMAVOICE9,
	USB_AUDIO_FORMAT_OKI_ADPCM,
	USB_AUDIO_FORMAT_DVI_ADPCM,
	USB_AUDIO_FORMAT_IMA_ADPCM,
	USB_AUDIO_FORMAT_MEDIASPACE_ADPCM,
	USB_AUDIO_FORMAT_SIERRA_ADPCM,
	USB_AUDIO_FORMAT_G723_ADPCM,
	USB_AUDIO_FORMAT_DIGISTD,
	USB_AUDIO_FORMAT_DIGIFIX,
	USB_AUDIO_FORMAT_DIALOGIC_OKI_ADPCM,
	USB_AUDIO_FORMAT_MEDIAVISION_ADPCM,
	USB_AUDIO_FORMAT_YAMAHA_ADPCM,
	USB_AUDIO_FORMAT_SONARC,
	USB_AUDIO_FORMAT_DSPGROUP_TRUESPEECH,
	USB_AUDIO_FORMAT_ECHOSC1,
	USB_AUDIO_FORMAT_AUDIOFILE_AF36,
	USB_AUDIO_FORMAT_APTX,
	USB_AUDIO_FORMAT_AUDIOFILE_AF10,
	USB_AUDIO_FORMAT_DOLBY_AC2,
	USB_AUDIO_FORMAT_GSM610,
	USB_AUDIO_FORMAT_MSNAUDIO,
	USB_AUDIO_FORMAT_ANTEX_ADPCME,
	USB_AUDIO_FORMAT_CONTROL_RES_VQLPC,
	USB_AUDIO_FORMAT_DIGIREAL,
	USB_AUDIO_FORMAT_DIGIADPCM,
	USB_AUDIO_FORMAT_CONTROL_RES_CR10,
	USB_AUDIO_FORMAT_NMS_VBXADPCM,
	USB_AUDIO_FORMAT_CS_IMAADPCM,
	USB_AUDIO_FORMAT_ECHOSC3,
	USB_AUDIO_FORMAT_ROCKWELL_ADPCM,
	USB_AUDIO_FORMAT_ROCKWELL_DIGITALK,
	USB_AUDIO_FORMAT_XEBEC,
	USB_AUDIO_FORMAT_G721_ADPCM,
	USB_AUDIO_FORMAT_G728_CELP,
	USB_AUDIO_FORMAT_MPEG,
	USB_AUDIO_FORMAT_MPEGLAYER3,
	USB_AUDIO_FORMAT_CIRRUS,
	USB_AUDIO_FORMAT_ESPCM,
	USB_AUDIO_FORMAT_VOXWARE,
	USB_AUDIO_FORMAT_WAVEFORMAT_CANOPUS_ATRAC,
	USB_AUDIO_FORMAT_G726_ADPCM,
	USB_AUDIO_FORMAT_G722_ADPCM,
	USB_AUDIO_FORMAT_DSAT,
	USB_AUDIO_FORMAT_DSAT_DISPLAY,
	USB_AUDIO_FORMAT_SOFTSOUND,
	USB_AUDIO_FORMAT_RHETOREX_ADPCM,
	USB_AUDIO_FORMAT_MSAUDIO1,
	USB_AUDIO_FORMAT_WMAUDIO2,
	USB_AUDIO_FORMAT_WMAUDIO3,
	USB_AUDIO_FORMAT_WMAUDIO_LOSSLESS,
	USB_AUDIO_FORMAT_CREATIVE_ADPCM,
	USB_AUDIO_FORMAT_CREATIVE_FASTSPEECH8,
	USB_AUDIO_FORMAT_CREATIVE_FASTSPEECH10,
	USB_AUDIO_FORMAT_QUARTERDECK,
	USB_AUDIO_FORMAT_FM_TOWNS_SND,
	USB_AUDIO_FORMAT_BTV_DIGITAL,
	USB_AUDIO_FORMAT_OLIGSM,
	USB_AUDIO_FORMAT_OLIADPCM,
	USB_AUDIO_FORMAT_OLICELP,
	USB_AUDIO_FORMAT_OLISBC,
	USB_AUDIO_FORMAT_OLIOPR,
	USB_AUDIO_FORMAT_LH_CODEC,
	USB_AUDIO_FORMAT_NORRIS,
};

enum {
	USB_HID_COUNTRY_NONE = 0,
	USB_HID_COUNTRY_ARABIC,
	USB_HID_COUNTRY_BELGIAN,
	USB_HID_COUNTRY_CANADA_BI,
	USB_HID_COUNTRY_CANADA_FR,
	USB_HID_COUNTRY_CZECH_REPUBLIC,
	USB_HID_COUNTRY_DANISH,
	USB_HID_COUNTRY_FINNISH,
	USB_HID_COUNTRY_FRENCH,
	USB_HID_COUNTRY_GERMAN,
	USB_HID_COUNTRY_GREEK,
	USB_HID_COUNTRY_HEBREW,
	USB_HID_COUNTRY_HUNGARY,
	USB_HID_COUNTRY_INTERNATIONAL_ISO,
	USB_HID_COUNTRY_ITALIAN,
	USB_HID_COUNTRY_JAPAN_KATAKANA,
	USB_HID_COUNTRY_KOREAN,
	USB_HID_COUNTRY_LATIN_AMERICAN,
	USB_HID_COUNTRY_NETHERLANDS,
	USB_HID_COUNTRY_NORWEGIAN,
	USB_HID_COUNTRY_PERSIAN,
	USB_HID_COUNTRY_POLAND,
	USB_HID_COUNTRY_PORTUGUESE,
	USB_HID_COUNTRY_RUSSIA,
	USB_HID_COUNTRY_SLOVAKIA,
	USB_HID_COUNTRY_SPANISH,
	USB_HID_COUNTRY_SWEDISH,
	USB_HID_COUNTRY_SWISS_FRENCH,
	USB_HID_COUNTRY_SWISS_GERMAN,
	USB_HID_COUNTRY_SWITZERLAND,
	USB_HID_COUNTRY_TAIWAN,
	USB_HID_COUNTRY_TURKISH_Q,
	USB_HID_COUNTRY_UK,
	USB_HID_COUNTRY_US,
	USB_HID_COUNTRY_YUGOSLAVIA,
	USB_HID_COUNTRY_TURKISH_F,
};

enum {
	USB_STRING_LANGUAGE 			   = 0,
	USB_STRING_VENDOR,
	USB_STRING_PRODUCT,
	USB_STRING_SERIAL,

	USB_STRING_MS_OS 				   = 0xee,
};

typedef struct {
	USB_HID_Descriptor_HID_t audio_hid;
	USB_Descriptor_Endpoint_t audio_in_endpoint;
} USB_HID_Descriptor_HID_Audio_t;

typedef struct {
	USB_HID_Descriptor_HID_t mouse_hid;
	USB_Descriptor_Endpoint_t mouse_in_endpoint;
} USB_HID_Descriptor_HID_Mouse_t;

typedef struct {
	USB_HID_Descriptor_HID_t keyboard_hid;
	USB_Descriptor_Endpoint_t keyboard_in_endpoint;
} USB_HID_Descriptor_HID_Keyboard_t;

typedef struct {
	USB_HID_Descriptor_HID_t somatic_hid;
	USB_Descriptor_Endpoint_t somatic_in_endpoint;
	USB_Descriptor_Endpoint_t somatic_out_endpoint;
} USB_HID_Descriptor_HID_Somatic_t;

typedef struct {
	USB_HID_Descriptor_HID_t vendor_hid;
	USB_Descriptor_Endpoint_t vendor_in_endpoint;
	USB_Descriptor_Endpoint_t vendor_out_endpoint;
} USB_HID_Descriptor_HID_Vendor_t;

typedef struct {
	// CDC Control Interface
	USB_CDC_Descriptor_FunctionalHeader_t    CDC_Functional_Header;
	USB_CDC_Descriptor_FunctionalACM_t       CDC_Functional_ACM;
	USB_CDC_Descriptor_FunctionalUnion_t     CDC_Functional_Union;
	USB_CDC_Descriptor_FunctionalUnion_t     CDC_Functional_CallManagement;
	USB_Descriptor_Endpoint_t                CDC_NotificationEndpoint;

	// CDC Data Interface
	USB_Descriptor_Interface_t               CDC_DCI_Interface;
	USB_Descriptor_Endpoint_t                CDC_DataOutEndpoint;
	USB_Descriptor_Endpoint_t                CDC_DataInEndpoint;
} USB_CDC_Descriptor_t;


typedef struct {
	USB_Descriptor_Configuration_Header_t 	Config;

#if (USB_CDC_ENABLE)
	// IAD0
	USB_Descriptor_Interface_Association_t 	cdc_iad;
    // CDC Interface
    USB_Descriptor_Interface_t 				cdc_interface;
    USB_CDC_Descriptor_t 					cdc_descriptor;
#endif /* USB_CDC_ENABLE */

#if (USB_MOUSE_ENABLE)
	// Mouse HID Interface
	USB_Descriptor_Interface_t 				mouse_interface;
	USB_HID_Descriptor_HID_Mouse_t 			mouse_descriptor;
#endif /* USB_MOUSE_ENABLE */

#if (USB_KEYBOARD_ENABLE)
	// Keyboard HID Interface
	USB_Descriptor_Interface_t 				keyboard_interface;
	USB_HID_Descriptor_HID_Keyboard_t 		keyboard_descriptor;
#endif /* USB_KEYBOARD_ENABLE */

#if (USB_VENDOR_ENABLE)
    //Vendor HID Interface
    USB_Descriptor_Interface_t 				vendor_interface;
    USB_HID_Descriptor_HID_Vendor_t 		vendor_descriptor;
#endif /* USB_VENDOR_ENABLE */

#if (__PROJECT_TL_SNIFFER__)
	USB_Descriptor_Interface_t 				sniffer_interface;
	USB_Descriptor_Endpoint_t 				sniffer_endpoint;
#endif
} USB_Descriptor_Configuration_t;



u8 *usbdesc_get_language(void);
u8 *usbdesc_get_vendor(void);
u8 *usbdesc_get_product(void);
u8 *usbdesc_get_serial(void);
u8 *usbdesc_get_device(void);
u8 *usbdesc_get_configuration(void);

#if (USB_MOUSE_ENABLE)
u8 *usbdesc_get_mouse(void);
#endif /* USB_MOUSE_ENABLE */
#if (USB_KEYBOARD_ENABLE)
u8 *usbdesc_get_keyboard(void);
#endif /* USB_KEYBOARD_ENABLE */
#if (USB_VENDOR_ENABLE)
u8 *usbvendor_get_vendor_desc(void);
#endif /* USB_VENDOR_ENABLE */

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
