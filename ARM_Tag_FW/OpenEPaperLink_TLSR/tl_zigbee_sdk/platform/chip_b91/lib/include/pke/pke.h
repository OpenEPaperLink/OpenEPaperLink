/********************************************************************************************************
 * @file	pke.h
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
#ifndef _PKE_H_
#define _PKE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "reg_include/register.h"
#include "pke_common.h"
#include "eccp_curve.h"



#ifndef NULL
#define NULL  0
#endif

#define GET_MAX_LEN(a,b)         (((a)>(b))?(a):(b))
#define GET_WORD_LEN(bitLen)     ((bitLen+31)/32)
#define GET_BYTE_LEN(bitLen)     ((bitLen+7)/8)


/********* pke microcode *********/
#define MICROCODE_PDBL                (0x04)
#define MICROCODE_PADD                (0x08)
#define MICROCODE_PVER                (0x0C)
#define MICROCODE_PMUL                (0x10)
#define MICROCODE_MODMUL              (0x18)
#define MICROCODE_MODINV              (0x1C)
#define MICROCODE_MODADD              (0x20)
#define MICROCODE_MODSUB              (0x24)
#define MICROCODE_CAL_PRE_MON         (0x28)
#define MICROCODE_C25519_PMUL         (0x34)
#define MICROCODE_Ed25519_PMUL        (0x38)
#define MICROCODE_Ed25519_PADD        (0x3C)


/********* pke action *********/
#define PKE_START_CALC                (0x01)
#define PKE_EXE_CFG_DEFAULT           (0x2A)
#define PKE_EXE_CFG_ALL_NON_MONT      (0x15)
#define PKE_EXE_CFG_ALL_MONT          PKE_EXE_CFG_DEFAULT


/********* oprand length *********/
#define PKE_OPERAND_MAX_WORD_LEN      (0x08)
#define PKE_OPERAND_MAX_BIT_LEN       (0x100)
#define ECC_MAX_WORD_LEN              PKE_OPERAND_MAX_WORD_LEN
#define ECC_MAX_BIT_LEN               PKE_OPERAND_MAX_BIT_LEN
#define C25519_WORD_LEN               ECC_MAX_WORD_LEN
#define C25519_BYTE_LEN               (C25519_WORD_LEN<<2)
#define Ed25519_WORD_LEN              C25519_WORD_LEN
#define Ed25519_BYTE_LEN              C25519_BYTE_LEN

/*********** some PKE algorithm operand length ************/
#define ECCP_MAX_BIT_LEN                      (256)  //ECC256
#define ECCP_MAX_BYTE_LEN                     (GET_BYTE_LEN(ECCP_MAX_BIT_LEN))
#define ECCP_MAX_WORD_LEN                     (GET_WORD_LEN(ECCP_MAX_BIT_LEN))


//PKE return code
enum PKE_RET_CODE
{
    PKE_SUCCESS = 0,
	PKE_ACTIVE_STOP,
    PKE_MOD_INV_NOT_EXIST,
    PKE_NOT_ON_CURVE,
    PKE_INVALID_MICRO_CODE,
	PKE_POINTOR_NULL,
	PKE_INVALID_INPUT,
};


//KDF define
typedef void *(*KDF_FUNC)(const void *input, unsigned int byteLen, unsigned char *key, unsigned int keyByteLen);


//API

/**
 * @brief		get rand.
 * @param[in] 	rand	- the buffer rand.
 * @param[in] 	byteLen	- the length of rand.
 * @return		0:TRNG_SUCCESS,   1:TRNG_ERROR.
 */
unsigned char rand_get(unsigned char *rand, unsigned int byteLen);

/**
 * @brief       get real bit length of big number a of wordLen words.
 * @param[in]   a			- the buffer a.
 * @param[in]   wordLen		- the length of a.
 * @return      the real bit length of big number a of wordLen words.
 */
unsigned int valid_bits_get(const unsigned int *a, unsigned int wordLen);

/**
 * @brief		get real word lenth of big number a of max_words words.
 * @param[in]   a			- the buffer a.
 * @param[in]   max_words	- the length of a.
 * @return		get real word lenth of big number a.
 */
unsigned int valid_words_get(unsigned int *a, unsigned int max_words);

/**
 * @brief		reverse byte array.
 * @param[in]   in			- need to reverse the array.
 * @param[in]   out			- inverted array.
 * @param[in]   byteLen		- the length of array.
 * @return		none.
 */
void reverse_byte_array(unsigned char *in, unsigned char *out, unsigned int byteLen);

/**
 * @brief		compare big integer a and b.
 * @param[in] 	a 			- value.
 * @param[in] 	aWordLen  	- the length of a.
 * @param[in] 	b			- value.
 * @param[in] 	bWordLen	- the length of b.
 * @return		0:a=b,   1:a>b,   -1: a<b.
 */
int big_integer_compare(unsigned int *a, unsigned int aWordLen, unsigned int *b, unsigned int bWordLen);

/**
 * @brief		This function is used to determine whether the array is all 0s.
 * @param[in]	data	- the buffer data.
 * @param[in]	len		- the length of data.
 * @return		1: all 0, 0: not all 0.
 */
int is_mem_zero4(unsigned int a[], unsigned int wordLen);

/**
 * @brief       out = (a+b) mod modulus.
 * @param[in]   modulus - modulus.
 * @param[in]   a 		- integer a.
 * @param[in]   b 		- integer b.
 * @param[in]   wordLen - word length of modulus, a, b.
 * @param[out]  out 	- out = a+b mod modulus.
 * @return      PKE_SUCCESS(success), other(error).
 */
unsigned char pke_mod_add(const unsigned int *modulus, const unsigned int *a, const unsigned int *b,
				   unsigned int *out, unsigned int wordLen);

/**
 * @brief       out = (a-b) mod modulus.
 * @param[in]   modulus	- input, modulus.
 * @param[in]  	a		- input, integer a.
 * @param[in]   b		- input, integer b.
 * @param[in]   wordLen - input, word length of modulus, a, b.
 * @param[out]  out		- output, out = a-b mod modulus.
 * @return      PKE_SUCCESS(success), other(error).
 */
unsigned char pke_mod_sub(const unsigned int *modulus, const unsigned int *a, const unsigned int *b,
				   unsigned int *out, unsigned int wordLen);

/**
 * @brief		load the pre-calculated mont parameters H(R^2 mod modulus) and
 * 				n1( - modulus ^(-1) mod 2^w ).
 * @param[in] 	H 	  	- R^2 mod modulus.
 * @param[in] 	n1 	  	- modulus ^(-1) mod 2^w, here w is 32 acutally.
 * @param[in] 	wordLen - word length of modulus or H.
 * @return: 	none.
 */
void pke_load_pre_calc_mont(unsigned int *H, unsigned int *n1, unsigned int wordLen);

/**
 * @brief		out = a*b mod modulus.
 * @param[in] 	modulus - input, modulus.
 * @param[in] 	a 	  	- input, integer a.
 * @param[in] 	b 	  	- input, integer b.
 * @param[in] 	wordLen - input, word length of modulus, a, b.
 * @param[out] 	out 	- output, out = a*b mod modulus.
 * @return      PKE_SUCCESS(success), other(error).
 */
unsigned char pke_modmul_internal(const unsigned int *modulus, const unsigned int *a, const unsigned int *b,
							unsigned int *out, unsigned int wordLen);

/**
 * @brief       out = a*b mod modulus.
 * @param[in]   modulus	- modulus.
 * @param[in]   a 		- integer a.
 * @param[in]   b 		- integer b.
 * @param[in]   wordLen	- word length of modulus, a, b.
 * @param[out]  out		- out = a*b mod modulus.
 * @return      PKE_SUCCESS(success), other(error).
 */
unsigned char pke_mod_mul(const unsigned int *modulus, const unsigned int *a, const unsigned int *b,
				   unsigned int *out, unsigned int wordLen);

/**
 * @brief       ainv = a^(-1) mod modulus.
 * @param[in]   modulus		- modulus.
 * @param[in]   a 			- integer a.
 * @param[in]   modWordLen	- word length of modulus, ainv.
 * @param[in]   aWordLen 	- word length of integer a.
 * @param[out]	ainv 		- ainv = a^(-1) mod modulus.
 * @return: 	PKE_SUCCESS(success), other(inverse not exists or error).
 */
unsigned char pke_mod_inv(const unsigned int *modulus, const unsigned int *a, unsigned int *ainv, unsigned int modWordLen,
				   unsigned int aWordLen);

/**
 * @brief		a = a/(2^n).
 * @param[in]   a			- big integer a.
 * @param[in]   aWordLen	- word length of a.
 * @param[in]   n			- exponent of 2^n.
 * @return		word length of a = a/(2^n).
 * @attention:	1. make sure aWordLen is real word length of a.
 *     			2. a may be 0, then aWordLen is 0, to make sure aWordLen-1 is available, so data
 *        		   type of aWordLen is int32_t, not uint32_t.
 */
unsigned int div2n_u32(unsigned int a[], int aWordLen, unsigned int n);

/**
 * @brief		c = a - b.
 * @param[in]   a 		- integer a.
 * @param[in]   b 		- integer b.
 * @param[in]   wordLen	- the length of a and b.
 * @param[out]  c 		- integer c = a - b.
 * @return		none.
 */
void sub_u32(unsigned int *a, unsigned int *b, unsigned int *c, unsigned int wordLen);

/**
 * @brief		c = a mod b.
 * @param[in]   a 		 	- integer a.
 * @param[in]   b 		 	- integer b.
 * @param[in]   aWordLen	- word length of a.
 * @param[in]   bWordLen	- word length of b.
 * @param[in]   b_h			- parameter b_h.
 * @param[in]   b_n1		- parameter b_n1.
 * @param[out]  c			- c = a mod b.
 * @return		PKE_SUCCESS(success), other(error).
 */
unsigned char pke_mod(unsigned int *a, unsigned int aWordLen, unsigned int *b, unsigned int *b_h, unsigned int *b_n1,
				unsigned int bWordLen, unsigned int *c);

/**
 * @brief       ECCP curve point mul(random point), Q=[k]P.
 * @param[in]   curve	- ECCP_CURVE struct pointer.
 * @param[in]   k	 	- scalar.
 * @param[in]   Px 		- x coordinate of point P.
 * @param[in]   Py 		- y coordinate of point P.
 * @param[out]  Qx 		- x coordinate of point Q=[k]P.
 * @param[out]  Qy 		- y coordinate of point Q=[k]P.
 * @return      PKE_SUCCESS(success), other(error).
 */
unsigned char pke_eccp_point_mul(eccp_curve_t *curve, unsigned int *k, unsigned int *Px, unsigned int *Py,
					  unsigned int *Qx, unsigned int *Qy);

/**
 * @brief       ECCP curve point del point, Q=2P.
 * @param[in]   curve	- ECCP_CURVE struct pointer.
 * @param[in]   Px 		- x coordinate of point P.
 * @param[in]   Py 		- y coordinate of point P.
 * @param[out]  Qx 		- x coordinate of point Q=2P.
 * @param[out]  Qy 		- y coordinate of point Q=2P.
 * @return      PKE_SUCCESS(success), other(error).
 */
unsigned char pke_eccp_point_del(eccp_curve_t *curve, unsigned int *Px, unsigned int *Py,
					  unsigned int *Qx, unsigned int *Qy);

/**
 * @brief       ECCP curve point add, Q=P1+P2.
 * @param[in]   curve	- eccp curve struct pointer.
 * @param[in]   P1x 	- x coordinate of point P1.
 * @param[in]   P1y 	- y coordinate of point P1.
 * @param[in]   P2x 	- x coordinate of point P2.
 * @param[in]   P2y 	- y coordinate of point P2.
 * @param[out]  Qx 		- x coordinate of point Q=P1+P2.
 * @param[out]  Qy 		- y coordinate of point Q=P1+P2.
 * @return      PKE_SUCCESS(success), other(error).
 */
unsigned char pke_eccp_point_add(eccp_curve_t *curve, unsigned int *P1x, unsigned int *P1y, unsigned int *P2x, unsigned int *P2y,
					  unsigned int *Qx, unsigned int *Qy);

//#define ECCP_POINT_DOUBLE
#ifdef ECCP_POINT_DOUBLE
unsigned char eccp_pointDouble(eccp_curve_t *curve, unsigned int *Px, unsigned int *Py, unsigned int *Qx, unsigned int *Qy);
#endif

/**
 * @brief       check whether the input point P is on ECCP curve or not.
 * @param[in]   curve	- ECCP_CURVE struct pointer.
 * @param[in]   Px		- x coordinate of point P.
 * @param[in]   Py		- y coordinate of point P.
 * @return      PKE_SUCCESS(success, on the curve), other(error or not on the curve).
 */
unsigned char pke_eccp_point_verify(eccp_curve_t *curve, unsigned int *Px, unsigned int *Py);

/**
 * @brief		get ECCP key pair(the key pair could be used in ECDSA/ECDH).
 * @param[in]	curve	- eccp curve struct pointer.
 * @param[out]	priKey	- private key, big-endian.
 * @param[out]	pubKey	- public key, big-endian.
 * @return		PKE_SUCCESS(success), other(error).
 */
unsigned char eccp_getkey(eccp_curve_t *curve, unsigned char *priKey, unsigned char *pubKey);

/**
 * @brief       c25519 point mul(random point), Q=[k]P.
 * @param[in]   curve	- c25519 curve struct pointer.
 * @param[in]  	k		- scalar.
 * @param[in]   Pu		- u coordinate of point P.
 * @param[out]  Qu		- u coordinate of point Q=[k]P.
 * @return      PKE_SUCCESS(success), other(error).
 */
unsigned char pke_x25519_point_mul(mont_curve_t *curve, unsigned int *k, unsigned int *Pu, unsigned int *Qu);

/**
 * @brief       edwards25519 curve point mul(random point), Q=[k]P.
 * @param[in]   curve	- edwards25519 curve struct pointer.
 * @param[in]  	k		- scalar.
 * @param[in]   Px 		- x coordinate of point P.
 * @param[in]   Py 		- y coordinate of point P.
 * @param[out]  Qx 		- x coordinate of point Q=[k]P.
 * @param[out]  Qy 		- y coordinate of point Q=[k]P.
 * @return      PKE_SUCCESS(success), other(error).
 */
unsigned char pke_ed25519_point_mul(edward_curve_t *curve, unsigned int *k, unsigned int *Px, unsigned int *Py,
					  unsigned int *Qx, unsigned int *Qy);

/**
 * @brief       edwards25519 point add, Q=P1+P2.
 * @param[in]   curve	- edwards25519 curve struct pointer.
 * @param[in]   P1x 	- x coordinate of point P1.
 * @param[in]   P1y 	- y coordinate of point P1.
 * @param[in]   P2x 	- x coordinate of point P2.
 * @param[in]   P2y 	- y coordinate of point P2.
 * @param[out]  Qx 		- x coordinate of point Qx=P1x+P2x.
 * @param[out]  Qy 		- y coordinate of point Qy=P1y+P2y.
 * @return      PKE_SUCCESS(success), other(error).
 */
unsigned char pke_ed25519_point_add(edward_curve_t *curve, unsigned int *P1x, unsigned int *P1y, unsigned int *P2x, unsigned int *P2y,
					  unsigned int *Qx, unsigned int *Qy);



#ifdef __cplusplus
}
#endif

#endif
