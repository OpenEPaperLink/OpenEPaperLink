/********************************************************************************************************
 * @file    drv_keyboard.h
 *
 * @brief   This is the header file for drv_keyboard
 *
 * @author  Zigbee Group
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



#define KB_RETURN_KEY_MAX					6
#define	KB_NUMLOCK_STATUS_INVALID			BIT(7)

typedef struct{
	u8 cnt;
	u8 ctrl_key;
	u8 keycode[KB_RETURN_KEY_MAX];
	//u8 padding[2];	//for 32bit padding, if KB_RETURN_KEY_MAX change, this should be changed
}kb_data_t;

typedef struct{
	kb_data_t *last_send_data;
	u32 last_send_time;
	u8 resent_no_ack_cnt;
	u8 key_pressed;
	u8 frame_no;
	u8 ack_frame_no;
}rf_kb_ctrl_t;

typedef struct{
	u8 val_org;
	u8 val_chg;
	u8 this_pressed;
	u8 last_pressed;
	u32 longpress_th;
	u32 press_start_time;
	u8 longpressed;
	u8 processed;
}kb_longpress_ctrl_t;

enum{
    KB_NOT_LONGPRESS,
	KB_IS_LONGPRESS,
    KB_KEY_RELEASE,
};

extern kb_data_t kb_event;



static inline unsigned int gpio_read_cache(u32 pin, u8 *p){
	return p[pin>>8] & (pin & 0xff);
}

static inline int kb_is_key_valid(kb_data_t *p){
	return (p->cnt || p->ctrl_key);
}
static inline void kb_set_key_invalid(kb_data_t *p){
	p->cnt = p->ctrl_key = 0;
}


int kb_is_data_same(kb_data_t *a, kb_data_t *b);

u32 kb_scan_key(int numlock_status, int read_key);
