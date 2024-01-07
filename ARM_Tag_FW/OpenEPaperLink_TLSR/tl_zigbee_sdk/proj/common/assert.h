/********************************************************************************************************
 * @file    assert.h
 *
 * @brief   This is the header file for assert
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

//for  __DEBUG__
#if (ASSERT_DEBUG_MODE)
#define assert(exp) 	do{ \
                        	if(!(exp)){ 											\
                        		DEBUG_OUT("File:%s,Line:%d\n",__FILE__,__LINE__); 	\
                        		while(1){;} 										\
                        	} 														\
						}while(0)

#define trace_route 	do{ \
                        	DEBUG_OUT("File:%s,Line:%d\n",__FILE__,__LINE__); 		\
                    	}while(0)
#else
#define assert(ignore)  ((void) 0)
#define trace_route
#endif

////////////////////  To do compiler warning  //////////////////
// http://stackoverflow.com/questions/5966594/how-can-i-use-pragma-message-so-that-the-message-points-to-the-filelineno
// http://gcc.gnu.org/ml/gcc-help/2010-10/msg00196.html
// http://stackoverflow.com/questions/3030099/c-c-pragma-in-define-macro

#define _STRINGIFY(x)	#x
#define STRINGIFY(x) 	_STRINGIFY(x)

#ifdef __GNUC__
#define COMPILE_MESSAGE(x) _Pragma (#x)
#endif

#if (__SHOW_TODO__)
#ifdef __GNUC__
#define TODO(x) 		COMPILE_MESSAGE(message ("--TODO-- " #x))
#else
#define TODO(x) 		__pragma(message("--TODO-- "_STRINGIFY(x) " ::function: " __FUNCTION__ "@"STRINGIFY(__LINE__)))
#endif
#else
#define TODO(x)
#endif

#if (__SHOW_WARN__)
#ifdef __GNUC__
#define WARN(x) 		COMPILE_MESSAGE(message ("--WARN-- " #x))
#else
#define WARN(x) 		__pragma(message("--WARN-- "_STRINGIFY(x) " ::function: " __FUNCTION__ "@"STRINGIFY(__LINE__)))
#endif
#else
#define WARN(x)
#endif

#if (__SHOW_WARN__)
#ifdef __GNUC__
#define NOTE(x) 		COMPILE_MESSAGE(message ("--NOTE-- " #x))
#else
#define NOTE(x) 		__pragma(message("--NOTE-- "_STRINGIFY(x) " ::function: " __FUNCTION__ "@"STRINGIFY(__LINE__)))
#endif
#else
#define NOTE(x)
#endif

