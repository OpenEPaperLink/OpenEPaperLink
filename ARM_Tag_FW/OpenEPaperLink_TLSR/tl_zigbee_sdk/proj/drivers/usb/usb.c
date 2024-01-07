/********************************************************************************************************
 * @file    usb.c
 *
 * @brief   This is the source file for usb
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
#include "usb.h"
#include "usb_common.h"

#if (MODULE_USB_ENABLE)

static USB_Request_Header_t control_request;
static u8 *g_response = 0;
static u16 g_response_len = 0;
static u8 g_stall = 0;
u8 g_rate = 0; //default 0 for all report
u8 usb_mouse_report_proto = 0; //default 1 for report proto
u8 host_keyboard_status;


static void usb_send_response(void){
	u16 n;
	if(g_response_len < 8){
		n = g_response_len;
	} else {
		n = 8;
	}
	g_response_len -= n;

	usbhw_reset_ctrl_ep_ptr();
	while(n-- > 0){
		usbhw_write_ctrl_ep_data(*g_response);
		++g_response;
	}
}

static void usb_prepare_desc_data(void){
	u8 value_l = (control_request.wValue) & 0xff;
	u8 value_h = (control_request.wValue >> 8) & 0xff;

	g_response = 0;
	g_response_len = 0;

	switch(value_h){
		case DTYPE_Device:
			g_response = usbdesc_get_device();
			g_response_len = sizeof(USB_Descriptor_Device_t);
			break;
		case DTYPE_Configuration:
			g_response = usbdesc_get_configuration();
			g_response_len = sizeof(USB_Descriptor_Configuration_t);
			break;
		case DTYPE_String:
			if(USB_STRING_LANGUAGE == value_l){
				g_response = usbdesc_get_language();
				g_response_len = sizeof(LANGUAGE_ID_ENG);
			}else if(USB_STRING_VENDOR == value_l){
				g_response = usbdesc_get_vendor();
				g_response_len = sizeof(STRING_VENDOR);
			}else if(USB_STRING_PRODUCT == value_l){
				g_response = usbdesc_get_product();
				g_response_len = sizeof(STRING_PRODUCT);
			}else if(USB_STRING_SERIAL == value_l){
				g_response = usbdesc_get_serial();
				g_response_len = sizeof(STRING_SERIAL);
			}else{
				g_stall = 1;
			}
			break;
		default:
			g_stall = 1;
			break;
	}

	if(control_request.wLength < g_response_len){
		g_response_len = control_request.wLength;
	}
}

static void usb_handle_std_intf_req(void){
	g_response = 0;
	g_response_len = 0;

	u8 value_h = (control_request.wValue >> 8) & 0xff;
#if (USB_MOUSE_ENABLE || USB_KEYBOARD_ENABLE || USB_VENDOR_ENABLE || USB_SOMATIC_ENABLE)
	u8 index_l = (control_request.wIndex) & 0xff;
#endif

	switch(value_h){
		case HID_DTYPE_HID://HID Descriptor
			//index_l is the interface number
#if (USB_MOUSE_ENABLE )
			if(index_l == USB_INTF_MOUSE){
				//mouse
				g_response = usbdesc_get_mouse();
				g_response_len = sizeof(USB_HID_Descriptor_HID_Mouse_t);
			}
#endif /* USB_MOUSE_ENABLE */
#if (USB_KEYBOARD_ENABLE)
			if(index_l == USB_INTF_KEYBOARD){
				//keyboard
				g_response = usbdesc_get_keyboard();
				g_response_len = sizeof(USB_HID_Descriptor_HID_Keyboard_t);
			}
#endif /* USB_KEYBOARD_ENABLE */
#if (USB_VENDOR_ENABLE)
			if(index_l == USB_INTF_HID_VENDOR){
				g_response = usbvendor_get_vendor_desc();
				g_response_len = sizeof(USB_HID_Descriptor_HID_Vendor_t);
			}
#endif /* USB_VENDOR_ENABLE */
			break;
		case HID_DTYPE_Report://Report Descriptor
#if (USB_MOUSE_ENABLE)
			if(index_l == USB_INTF_MOUSE){
				//mouse
				g_response = usbmouse_get_report_desc();
				g_response_len = usbmouse_get_report_desc_size();
			}
#endif /* USB_MOUSE_ENABLE */
#if (USB_KEYBOARD_ENABLE)
			if(index_l == USB_INTF_KEYBOARD){
				//keyboard
				g_response = usbkb_get_report_desc();
				g_response_len = usbkb_get_report_desc_size();
			}
#endif /* USB_KEYBOARD_ENABLE */
#if (USB_VENDOR_ENABLE)
			if(index_l == USB_INTF_HID_VENDOR){
				g_response = usbvendor_get_report_desc();
				g_response_len = usbvendor_get_report_desc_size();
			}
#endif /* USB_VENDOR_ENABLE */
			break;
		case HID_DTYPE_Physical://Physical Descriptor
			break;
		default:
			break;
	}

	if(control_request.wLength < g_response_len){
		g_response_len = control_request.wLength;
	}
}

static void usb_handle_out_class_intf_req(u8 data_request){
	u8 property = control_request.bRequest;
	u8 value_l = (control_request.wValue) & 0xff;
	u8 value_h = (control_request.wValue >> 8) & 0xff;
//	u8 Entity = (control_request.wIndex >> 8) & 0xff;

	switch(property){
		case HID_REQ_SetReport:
			switch(value_h){
				case HID_REPORT_ITEM_In:
					break;
				case HID_REPORT_ITEM_Out:
					break;
				case HID_REPORT_ITEM_Feature:
					if(data_request){
						host_keyboard_status = usbhw_read_ctrl_ep_data();
					}
					break;
				case HID_REPORT_CUSTOM:
					break;
				default:
					g_stall = 1;
					break;
			}
			break;
		case HID_REQ_SetIdle:
			if(data_request){
				g_rate = usbhw_read_ctrl_ep_data();
			}
			g_rate = value_h;
			break;
		case HID_REQ_SetProtocol:
			if(data_request){
				usb_mouse_report_proto = usbhw_read_ctrl_ep_data();
			}
			usb_mouse_report_proto = value_l;
			break;
#if (USB_CDC_ENABLE)
	    case CDC_REQ_SetControlLineState:
		case CDC_REQ_SetLineEncoding:
			usbcdc_processControlRequest(control_request.bRequest, control_request.wValue, control_request.wIndex, control_request.wLength);
			break;
#endif /* USB_CDC_ENABLE */
		default:
			g_stall = 1;
			break;
	}
}

static void usb_handle_in_class_intf_req(void){
	u8 property = control_request.bRequest;
//	u8 value_h = (control_request.wValue >> 8);
//	u8 Entity = (control_request.wIndex >> 8);

	switch(property){
		case CDC_REQ_SendEncapsulatedCommand:
			usbhw_write_ctrl_ep_data(0x00);
			break;
		case HID_REQ_GetReport:
			break;
		case HID_REQ_GetIdle:
			usbhw_write_ctrl_ep_data(g_rate);
			break;
		case HID_REQ_GetProtocol:
			usbhw_write_ctrl_ep_data(usb_mouse_report_proto);
			break;
#if (USB_CDC_ENABLE)
		case CDC_REQ_GetLineEncoding:
			usbcdc_processControlRequest(control_request.bRequest, control_request.wValue, control_request.wIndex, control_request.wLength);
			break;
#endif /* USB_CDC_ENABLE */
		default:
			g_stall = 1;
			break;
	}
}

static void usb_handle_in_class_endp_req(void){
	//u8 property = control_request.bRequest;
	//u8 ep_ctrl = control_request.wValue >> 8;
}

static void usb_handle_out_class_endp_req(u8 data_request){

}

static void usb_handle_set_intf(void){

}

static void usb_handle_get_intf(void){

}

static void usb_handle_request(u8 data_request){
	u8 bmRequestType = control_request.bmRequestType;
	u8 bRequest = control_request.bRequest;

	usbhw_reset_ctrl_ep_ptr();

	switch(bmRequestType){
		case (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE):
			if(REQ_GetDescriptor == bRequest){
				if(USB_IRQ_SETUP_REQ == data_request){
					usb_prepare_desc_data();
				}
				usb_send_response();
			}
			break;
	case (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_INTERFACE):
		if(REQ_GetDescriptor == bRequest){
			if(USB_IRQ_SETUP_REQ == data_request){
				usb_handle_std_intf_req();
			}
			usb_send_response();
		}else if(REQ_GetInterface == bRequest){
			usb_handle_get_intf();
		}
		break;
	case (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE):
		usb_handle_out_class_intf_req(data_request);
		break;
	case (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_ENDPOINT):
		usb_handle_out_class_endp_req(data_request);
		break;
	case (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE):
		usb_handle_in_class_intf_req();
		break;
	case (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_ENDPOINT):
		usb_handle_in_class_endp_req();
		break;
	case (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_INTERFACE):
		if(REQ_SetInterface == bRequest){
			usb_handle_set_intf();
		}
		break;
#if (__PROJECT_TL_SNIFFER__)
	case (REQDIR_DEVICETOHOST | REQTYPE_VENDOR | REQREC_DEVICE):
		usbSniffer_processControlRequest(bmRequestType, data_request, bRequest, 0);
		break;
	case (REQDIR_HOSTTODEVICE | REQTYPE_VENDOR | REQREC_DEVICE):
		usbSniffer_processControlRequest(bmRequestType, data_request, bRequest, control_request.wIndex);
		break;
#endif
	default:
		g_stall = 1;
		break;
	}
}

static void usb_handle_ctl_ep_setup(void){
	usbhw_reset_ctrl_ep_ptr();
	control_request.bmRequestType = usbhw_read_ctrl_ep_data();
	control_request.bRequest = usbhw_read_ctrl_ep_data();
	control_request.wValue = usbhw_read_ctrl_ep_u16();
	control_request.wIndex = usbhw_read_ctrl_ep_u16();
	control_request.wLength = usbhw_read_ctrl_ep_u16();
	g_stall = 0;
	usb_handle_request(USB_IRQ_SETUP_REQ);
	if(g_stall){
		usbhw_write_ctrl_ep_ctrl(FLD_EP_DAT_STALL);
	}else{
		usbhw_write_ctrl_ep_ctrl(FLD_EP_DAT_ACK);
	}
}

static void usb_handle_ctl_ep_data(void){
	usbhw_reset_ctrl_ep_ptr();
	g_stall = 0;
	usb_handle_request(USB_IRQ_DATA_REQ);
	if(g_stall){
		usbhw_write_ctrl_ep_ctrl(FLD_EP_DAT_STALL);
	}else{
		usbhw_write_ctrl_ep_ctrl(FLD_EP_DAT_ACK);
	}
}

static void usb_handle_ctl_ep_status(void){
	if(g_stall){
		usbhw_write_ctrl_ep_ctrl(FLD_EP_STA_STALL);
	}else{
		usbhw_write_ctrl_ep_ctrl(FLD_EP_STA_ACK);
	}
}

/**
 * @brief This function handles the USB related irq.
 * @param   none
 * @return none
 */
void usb_handle_irq(void){
	u32 irq = usbhw_get_ctrl_ep_irq();

	if(irq & FLD_CTRL_EP_IRQ_SETUP){
		usbhw_clr_ctrl_ep_irq(FLD_CTRL_EP_IRQ_SETUP);
		usb_handle_ctl_ep_setup();
	}
	if(irq & FLD_CTRL_EP_IRQ_DATA){
		usbhw_clr_ctrl_ep_irq(FLD_CTRL_EP_IRQ_DATA);
		usb_handle_ctl_ep_data();
	}
	if(irq & FLD_CTRL_EP_IRQ_STA){
		usbhw_clr_ctrl_ep_irq(FLD_CTRL_EP_IRQ_STA);
		usb_handle_ctl_ep_status();
	}
	if(usb_is_irq_reset()){//USB reset
		usb_mouse_report_proto = 1;
		usb_clear_irq_reset();//Clear USB reset flag
		for(int i = 0; i < 8; i++){
			reg_usb_ep_ctrl(i) = 0;
		}
#if (USB_CDC_ENABLE)
		usbhw_data_ep_ack(CDC_RX_EPNUM);
#endif
	}

#if (__PROJECT_TL_SNIFFER__)
	g_stall = 0;
#endif

#if (USB_CDC_ENABLE)
	g_stall = 0;
	usbcdc_dataHandler();
#endif /* USB_CDC_ENABLE */

#if (USB_VENDOR_ENABLE)
	hidvendor_dataHandler();
#endif /* USB_VENDOR_ENABLE */
}

static void usb_init_interrupt(void){
	usbhw_enable_manual_interrupt(FLD_CTRL_EP_AUTO_STD | FLD_CTRL_EP_AUTO_DESC);

#if (USB_CDC_ENABLE)
    //BM_CLR(reg_usb_mask, BIT(CDC_TX_EPNUM & 0x07) | BIT(CDC_RX_EPNUM & 0x07));
    usbhw_clr_eps_irq_mask(BIT(CDC_TX_EPNUM & 0x07) | BIT(CDC_RX_EPNUM & 0x07));
    usbhw_data_ep_ack(CDC_RX_EPNUM);
#endif /* USB_CDC_ENABLE */

#if (USB_VENDOR_ENABLE)
    //BM_CLR(reg_usb_mask, BIT(USB_EDP_HID_VENDOR_IN & 0x07) | BIT(USB_EDP_HID_VENDOR_OUT & 0x07));
    usbhw_clr_eps_irq_mask(BIT(USB_EDP_HID_VENDOR_IN & 0x07) | BIT(USB_EDP_HID_VENDOR_OUT & 0x07));
    usbhw_data_ep_ack(USB_EDP_HID_VENDOR_OUT);
#endif /* USB_VENDOR_ENABLE */
}

/**
 * @brief This function initializes the USB interface
 * @param   none
 * @return none
 */
void usb_init(void){
	//must switch to manual mode.
	usb_init_interrupt();
}


#endif
