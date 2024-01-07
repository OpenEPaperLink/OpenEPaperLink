/********************************************************************************************************
 * @file	ecdsa.h
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
#ifndef _ECDSA_H_
#define _ECDSA_H_

#ifdef __cplusplus
extern "C" {
#endif



#include "pke.h"



//ECDSA return code
enum ECDSA_RET_CODE
{
	ECDSA_SUCCESS = PKE_SUCCESS,
	ECDSA_POINTOR_NULL = PKE_SUCCESS+0x50,
	ECDSA_INVALID_INPUT,
	ECDSA_ZERO_ALL,
	ECDSA_INTEGER_TOO_BIG,
	ECDSA_VERIFY_FAILED,
};


//APIs

/**
 * @brief		Generate ECDSA Signature in octet string style.
 * @param[in]	curve		- ecc curve struct pointer, please make sure it is valid.
 * @param[in]	E			- hash value, big-endian.
 * @param[in]	EByteLen	- byte length of E.
 * @param[in]	priKey		- private key, big-endian.
 * @param[out]	signature	- signature r and s, big-endian.
 * @return		ECDSA_SUCCESS(success), other(error).
 */
unsigned char ecdsa_sign(eccp_curve_t *curve, unsigned char *E, unsigned int EByteLen, unsigned char *rand_k, unsigned char *priKey,
				   unsigned char *signature);

/**
 * @brief		Verify ECDSA Signature in octet string style.
 * @param[in]	curve		- ecc curve struct pointer, please make sure it is valid.
 * @param[in]	E			- hash value, big-endian.
 * @param[in]	EByteLen	- byte length of E.
 * @param[in]	pubKey		- public key, big-endian.
 * @param[in]	signature	- signature r and s, big-endian.
 * @return		ECDSA_SUCCESS(success), other(error).
 */
unsigned char ecdsa_verify(eccp_curve_t *curve, unsigned char *E, unsigned int EByteLen, unsigned char *pubKey, unsigned char *signature);



#ifdef __cplusplus
}
#endif

#endif
