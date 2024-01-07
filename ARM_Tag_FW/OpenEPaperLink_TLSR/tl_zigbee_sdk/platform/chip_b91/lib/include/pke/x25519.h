/********************************************************************************************************
 * @file	x25519.h
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
#ifndef _X25519_H_
#define _X25519_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "pke.h"



extern mont_curve_t c25519[1];



//X25519 return code
enum {
	X25519_SUCCESS = PKE_SUCCESS,
	X25519_POINTOR_NULL = PKE_SUCCESS+0x70,
	X25519_INVALID_INPUT,
	X25519_INVALID_OUTPUT,
};


//APIs

/**
 * @brief		get X25519 public key from private key.
 * @param[in]	priKey		- private key, 32 bytes, little-endian.
 * @param[out]	pubKey		- public key, 32 bytes, little-endian.
 * @return		X25519_SUCCESS(success), other(error).
 */
unsigned char x25519_get_pubkey_from_prikey(unsigned char prikey[32], unsigned char pubkey[32]);

/**
 * @brief		get x25519 random key pair.
 * @param[out]	priKey		- private key, 32 bytes, little-endian.
 * @param[out]	pubKey		- public key, 32 bytes, little-endian.
 * @return		X25519_SUCCESS(success), other(error).
 */
unsigned char x25519_getkey(unsigned char prikey[32], unsigned char pubkey[32]);

/**
 * @brief		X25519 key agreement.
 * @param[in]	local_prikey	- local private key, 32 bytes, little-endian.
 * @param[in]	peer_pubkey		- peer Public key, 32 bytes, little-endian.
 * @param[in]	keyByteLen		- byte length of output key.
 * @param[in]	kdf				- KDF function.
 * @param[out]	key				- derived key.
 * @return		X25519_SUCCESS(success), other(error).
 * @Caution		if no KDF function, please set kdf to be NULL.
 */
unsigned char x25519_compute_key(unsigned char local_prikey[32], unsigned char peer_pubkey[32], unsigned char *key, unsigned int keyByteLen,
						   KDF_FUNC kdf);


#ifdef __cplusplus
}
#endif

#endif
