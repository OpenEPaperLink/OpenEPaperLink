/********************************************************************************************************
 * @file    types.h
 *
 * @brief   This is the header file for types
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

#if 0
typedef signed char int8_t;
typedef unsigned char uint8_t;

typedef signed short int16_t;
typedef unsigned short uint16_t;

typedef signed int int32_t;
typedef unsigned int uint32_t;

typedef int16_t coffee_page_t;
#endif

typedef unsigned char u8;
typedef signed char s8;

typedef unsigned short u16;
typedef signed short s16;

typedef unsigned int u32;
typedef signed int s32;


typedef long long s64;
typedef unsigned long long u64;

typedef void (*fn_ptr)(u8*);

typedef u32  u24;
typedef s32	 s24;

// Generic Status return
typedef u8 status_t;
typedef u32 UTCTime;

typedef u32 arg_t;

enum {
  ZB_FALSE = 0,
  ZB_TRUE = 1
};

typedef u8 cId_t;

/**
 *  @brief Type definition for extended address
 */
typedef u8 addrExt_t[8];
typedef u8 extPANId_t[8];

typedef addrExt_t extAddr_t;

typedef union {
	u16  shortAddr;
	addrExt_t extAddr;
}tl_zb_addr_t;

typedef union {
	u32	srcId;
	addrExt_t gpdIeeeAddr;
}gpdId_t;

/**
 *  @brief Type definition for combined short/extended device address
 */
typedef struct {
    union {
        u16         shortAddr;         //!< Short address
        addrExt_t   extAddr;           //!< Extended address
    } addr;
    u8  addrMode;                      //!< Address mode
} addr_t;

typedef struct {
	u16 id;
    u8  mode;                      //!< Address mode
} pan_id_t;

#ifdef WIN32
#ifndef FALSE
#define FALSE	0
#endif
#ifndef TRUE
#define TRUE	1
#endif

#define PACK_1
#define CODE
#define __no_init
#define GENERIC

#define NULL_OK
#define INP
#define OUTP
#define UNUSED
#define ONLY
#define READONLY
#define SHARED
#define KEEP
#endif

#ifndef NULL
#define NULL	0
#endif

#ifndef __cplusplus

#ifndef FALSE
#define FALSE 	0
#endif
#ifndef TRUE
#define TRUE 	(!FALSE)
#endif

#ifndef bool
#define bool	u8
#endif

#ifndef false
#define false	FALSE
#endif

#ifndef true
#define true	TRUE
#endif


#elif defined WIN32

#ifndef FALSE
#define FALSE	0
#endif
#ifndef TRUE
#define TRUE	1
#endif

#endif

// There is no way to directly recognise whether a typedef is defined
// http://stackoverflow.com/questions/3517174/how-to-check-if-a-datatype-is-defined-with-typedef
#ifdef __GNUC__
typedef	u16	wchar_t;		
#endif

#ifndef WIN32
//typedef u32 size_t;
#endif

#define U32_MAX ((u32)0xffffffff)
#define U16_MAX ((u16)0xffff)
#define U8_MAX  ((u8)0xff)
#define U31_MAX ((u32)0x7fffffff)
#define U15_MAX ((u16)0x7fff)
#define U7_MAX  ((u8)0x7f)


/*** Generic Status Return Values ***/
#define SUCCESS                   0x00
#define FAILURE                   0x01
#define INVALIDPARAMETER          0x02
#define INVALID_TASK              0x03
#define MSG_BUFFER_NOT_AVAIL      0x04
#define INVALID_MSG_POINTER       0x05
#define INVALID_EVENT_ID          0x06
#define INVALID_INTERRUPT_ID      0x07
#define NO_TIMER_AVAIL            0x08
#define NV_ITEM_UNINIT            0x09
#define NV_OPER_FAILED            0x0A
#define INVALID_MEM_SIZE          0x0B
#define NV_BAD_ITEM_LEN           0x0C



