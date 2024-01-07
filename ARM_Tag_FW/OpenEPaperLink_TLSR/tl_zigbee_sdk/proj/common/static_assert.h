/********************************************************************************************************
 * @file    static_assert.h
 *
 * @brief   This is the header file for static_assert
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

// static assertion. evaluate at compile time. It is very useful like,  STATIC_ASSERT(sizeof(a) == 5);

// #define STATIC_ASSERT(expr)   			{ char static_assertion[(expr) ? 1 : -1]; ((void) static_assertion); }	// (void) array;  to remove compiler unused variable warning

// more complicated version canbe used anywhere in the source
#define STATIC_ASSERT_M(COND,MSG) typedef char static_assertion_##MSG[(!!(COND))*2-1] 
// token pasting madness: 
#define STATIC_ASSERT3(X,L) 	STATIC_ASSERT_M(X,static_assertion_at_line_##L) 
#define STATIC_ASSERT2(X,L) 	STATIC_ASSERT3(X,L) 

#define STATIC_ASSERT(X)    	STATIC_ASSERT2(X,__LINE__) 

#define STATIC_ASSERT_POW2(expr)		STATIC_ASSERT(!((expr) & ((expr)-1)))					//  assert  expr  is  2**N
#define STATIC_ASSERT_EVEN(expr)		STATIC_ASSERT(!((expr) & 1))
#define STATIC_ASSERT_ODD(expr)			STATIC_ASSERT(((expr) & 1))
#define STATIC_ASSERT_INT_DIV(a, b) 	STATIC_ASSERT((a) / (b) * (b) == (a))

