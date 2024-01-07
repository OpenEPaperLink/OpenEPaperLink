/********************************************************************************************************
 * @file	ecdh.h
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
#ifndef _ECDH_H_
#define _ECDH_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "pke.h"


//ECDH return code
enum ECDH_RET_CODE
{
	ECDH_SUCCESS = PKE_SUCCESS,
	ECDH_POINTOR_NULL = PKE_SUCCESS+0x60,
	ECDH_INVALID_INPUT,
};


//APIs

/**
 * @brief		ECDH compute key.
 * @param[in]	curve			- ecc curve struct pointer, please make sure it is valid.
 * @param[in]	local_prikey	- local private key, big-endian.
 * @param[in]	peer_pubkey		- peer public key, big-endian.
 * @param[out]	key				- output key.
 * @param[in]	keyByteLen		- byte length of output key.
 * @param[in]	KDF				- KDF function to get key.
 * @Return		0(success), other(error)
 */
unsigned char ecdh_compute_key(eccp_curve_t *curve, unsigned char *local_prikey, unsigned char *peer_pubkey, unsigned char *key,
		unsigned int keyByteLen, KDF_FUNC kdf);


#ifdef __cplusplus
}
#endif

#endif
