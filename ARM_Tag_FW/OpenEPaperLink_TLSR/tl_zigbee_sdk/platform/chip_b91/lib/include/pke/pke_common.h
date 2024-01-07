/********************************************************************************************************
 * @file	pke_common.h
 *
 * @brief	This is the header file for B91
 *
 * @author	Driver Group
 * @date	2019
 *
 * @par     Copyright (c) 2019, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
/********* pke version:1.0 *********/
#ifndef PKE_COMMON_H
#define PKE_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

//TRNG return code
enum TRNG_RET_CODE
{
	TRNG_SUCCESS = 0,
	TRNG_ERROR,
};

/**
 * @brief       copy uint32 buffer.
 * @param[out]	dst     - output word buffer.
 * @param[in]   src   	- input word value.
 * @param[in]   wordLen - word length of buffer dst or src.
 * @return   	none.
 */
void uint32_copy(unsigned int *dst, unsigned int *src, unsigned int wordLen);

/**
 * @brief       	clear uint32 buffer.
 * @param[in&out]   a   	 - word buffer a.
 * @param[in]   	aWordLen - word length of buffer a.
 * @return   		none.
 */
void uint32_clear(unsigned int *a, unsigned int wordLen);

/**
 * @brief       reverse byte order in every unsigned int word.
 * @param[in]	in     	- input byte buffer.
 * @param[out]  out   	- output word buffer.
 * @param[in]   byteLen - byte length of buffer in or out.
 * @return   	none.
 * @caution		byteLen must be multiple of 4.
 */
void reverse_word(unsigned char *in, unsigned char *out, unsigned int bytelen);


#ifdef __cplusplus
}
#endif

#endif
