/********************************************************************************************************
 * @file    pke.h
 *
 * @brief   This is the header file for B87
 *
 * @author  Driver & Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *			All rights reserved.
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

#include "register.h"




#define GET_WORD_LEN(bitLen)     ((bitLen+31)/32)
#define GET_BYTE_LEN(bitLen)     ((bitLen+7)/8)

#define PKE_BASE             	 (0X802000)
#define reg_pke_a_ram(a)		 ((volatile unsigned long *)(PKE_BASE+0x0400+(a)*(0x24)))
#define reg_pke_b_ram(b)		 ((volatile unsigned long *)(PKE_BASE+0x1000+(b)*(0x24)))

/********* oprand length *********/
#define PKE_OPERAND_MAX_WORD_LEN      (0x08)
#define PKE_OPERAND_MAX_BIT_LEN       (0x100)
#define ECC_MAX_WORD_LEN              PKE_OPERAND_MAX_WORD_LEN
#define ECC_MAX_BIT_LEN               PKE_OPERAND_MAX_BIT_LEN

#ifndef NULL
#define NULL  0
#endif


/********* eccp curve struct *********/
typedef struct
{
    unsigned int eccp_p_bitLen;        //bit length of prime p
    unsigned int eccp_n_bitLen;        //bit length of order n
    unsigned int *eccp_p;
    unsigned int *eccp_p_h;
    unsigned int *eccp_p_n1;
    unsigned int *eccp_a;
    unsigned int *eccp_b;
    unsigned int *eccp_Gx;
    unsigned int *eccp_Gy;
    unsigned int *eccp_n;
    unsigned int *eccp_n_h;
    unsigned int *eccp_n_n1;
} eccp_curve_t;

typedef struct
{
	unsigned int mont_p_bitLen;        //bit length of prime p
	unsigned int *mont_p;
	unsigned int *mont_p_h;
	unsigned int *mont_p_n1;
	unsigned int *mont_a24;
	unsigned int *mont_u;
	unsigned int *mont_v;
	unsigned int *mont_n;
	unsigned int *mont_n_h;
	unsigned int *mont_n_n1;
	unsigned int *mont_h;
} mont_curve_t;

typedef struct
{
    unsigned int edward_p_bitLen;        //bit length of prime p
    unsigned int *edward_p;
    unsigned int *edward_p_h;
    unsigned int *edward_p_n1;
    unsigned int *edward_d;
    unsigned int *edward_Gx;
    unsigned int *edward_Gy;
    unsigned int *edward_n;
    unsigned int *edward_n_h;
    unsigned int *edward_n_n1;
    unsigned int *edward_h;

} edward_curve_t;


//PKE return code
typedef enum
{
    PKE_SUCCESS = 0,
	PKE_ACTIVE_STOP,
    PKE_MOD_INV_NOT_EXIST,
    PKE_POINT_NOT_ON_CURVE,
    PKE_INVALID_MICRO_CODE,
	PKE_POINTOR_NULL,
	PKE_INVALID_INPUT,
}pke_ret_code_e;

typedef enum{
	PKE_EXE_CFG_ALL_NON_MONT = 0x15,
	PKE_EXE_CFG_ALL_MONT	 = 0x2A,
}pke_exe_cfg_e;

typedef enum{
	PKE_MICROCODE_PDBL = 0x04,
	PKE_MICROCODE_PADD = 0x08,
	PKE_MICROCODE_PVER = 0x0C,
	PKE_MICROCODE_PMUL = 0x10,
	PKE_MICROCODE_MODMUL = 0x18,
	PKE_MICROCODE_MODINV = 0x1C,
	PKE_MICROCODE_MODADD = 0x20,
	PKE_MICROCODE_MODSUB = 0x24,
	PKE_MICROCODE_CAL_PRE_MON = 0x28,
	PKE_MICROCODE_C25519_PMUL = 0x34,
	PKE_MICROCODE_Ed25519_PMUL = 0x38,
	PKE_MICROCODE_Ed25519_PADD = 0x3C,

}pke_microcode_e;

/**
 * @brief		compare big integer a and b
 * @param[in] 	a 			- value
 * @param[in] 	aWordLen  	- the length of a
 * @param[in] 	b			- value
 * @param[in] 	bWordLen	- the length of b
 * @return		0:a=b,   1:a>b,   -1: a<b
 */
//signed int uint32_big_num_cmp(unsigned int *a, unsigned int aWordLen, unsigned int *b, unsigned int bWordLen);
signed int big_integer_compare(unsigned int *a, unsigned int aWordLen, unsigned int *b, unsigned int bWordLen);
/**
 * @brief		c = a - b
 * @param[in]   a -  integer a
 * @param[in]   b -  integer b
 * @param[out]  c -  integer c = a - b
 * @return		none
 */
//void uint32_big_sub(unsigned int *a, unsigned int *b, unsigned int *c, unsigned int wordLen);
void sub_u32(unsigned int *a, unsigned int *b, unsigned int *c, unsigned int wordLen);
/**
 * @brief		a = a/(2^n)
 * @param[in]   a - big integer a
 * @param[in]   aWordLen - word length of a
 * @param[in]   n - exponent of 2^n
 * @return		word length of a = a/(2^n)
 * @caution:	1. make sure aWordLen is real word length of a
 *     			2. a may be 0, then aWordLen is 0, to make sure aWordLen-1 is available, so data
 *        		   type of aWordLen is int32_t, not uint32_t
 */
//unsigned int uint32_big_div2n(unsigned int a[], signed int aWordLen, unsigned int n);
unsigned int div2n_u32(unsigned int a[], signed int aWordLen, unsigned int n);
/**
 * @brief      This function serves to get pke status.
 * @param[in]  none.
 * @return     pke status.
 */
static inline unsigned int pke_get_irq_status(void)
{
    return reg_pke_stat & FLD_PKE_STAT_DONE;
}
/**
 * @brief      This function serves to clear pke status.
 * @param[in]  none.
 * @return     none.
 */
static inline void pke_clr_irq_status(void)
{
	BM_CLR(reg_pke_stat, FLD_PKE_STAT_DONE);
}
/**
 * @brief      This function serves to enable pke interrupt function.
 * @param[in]  none.
 * @return     none.
 */
static inline void pke_set_irq_mask(void)
{
	BM_SET(reg_pke_conf, FLD_PKE_CONF_IRQ_EN);
}
/**
 * @brief      This function serves to disable PKE interrupt function.
 * @param[in]  none.
 * @return     none.
 */
static inline void pke_clr_irq_mask(void)
{
	BM_CLR(reg_pke_conf, FLD_PKE_CONF_IRQ_EN);
}
/**
 * @brief      set operation micro code.
 * @param[in]  none.
 * @return     none.
 */
static inline void pke_set_microcode(pke_microcode_e addr)
{
	reg_pke_mc_ptr = addr;
}
/**
 * @brief      set exe config.
 * @param[in]  none.
 * @return     none.
 */
static inline void pke_set_exe_cfg(pke_exe_cfg_e cfg)
{
	reg_pke_exe_conf = cfg;
}
/**
 * @brief      start pke calculate.
 * @param[in]  none.
 * @return     none.
 */
static inline void pke_opr_start(void)
{
	BM_SET(reg_pke_ctrl, FLD_PKE_CTRL_START);
}
/**
 * @brief      	This is used to indicate the reason when the pke stopped.
 * @param[in]  	none
 * @return     	0 - normal stop.
 * 				1 - received a termination request(CTRL.STOP is high)
 * 				2 - no valid modulo inverse
 * 				3 - point is not on the curve(CTRL.CMD:PVER)
 * 				4 - invalid microcode
 */
static inline unsigned char pke_check_rt_code(void)
{
    return (unsigned char)(reg_pke_rt_code & FLD_PKE_RT_CODE_STOP_LOG);
}
/**
 * @brief      set operand width please make sure 0 < bitLen <= 256.
 * @param[in]  none.
 * @return     none.
 */
static inline void pke_set_operand_width(unsigned int bitLen)
{
	BM_CLR(reg_pke_conf, FLD_PKE_CONF_PARTIAL_RADIX);
	BM_SET(reg_pke_conf, GET_WORD_LEN(bitLen)<<16);
	BM_CLR(reg_pke_conf, FLD_PKE_CONF_BASE_RADIX);
	BM_SET(reg_pke_conf, 2<<24);
}
/**
 * @brief		load the pre-calculated mont parameters H(R^2 mod modulus) and
 * 				n1( - modulus ^(-1) mod 2^w )
 * @param[in] 	H 	  	- R^2 mod modulus
 * @param[in] 	n1 	  	-  modulus ^(-1) mod 2^w, here w is 32 acutally
 * @param[in] 	wordLen - word length of modulus or H
 * @return: 	none
 */
void pke_load_pre_calc_mont(unsigned int *H, unsigned int *n1, unsigned int wordLen);
/**
 * @brief       calc h(R^2 mod modulus) and n1( - modulus ^(-1) mod 2^w ) for modmul, pointMul. etc.
 * 				here w is bit width of word, i,e. 32.
 * @param[in]   modulus - input, modulus
 * @param[in]   wordLen - input, word length of modulus or H
 * @param[out]  PKE_A3 	-  R^2 mod modulus
 * @param[out]  PKE_B4	-  modulus ^(-1) mod 2^w, here w is 32 acutally
 * @return      PKE_SUCCESS(success), other(error)
 */
unsigned char pke_calc_pre_mont(const unsigned int *modulus, unsigned int wordLen);
/**
 * @brief       ECCP curve point del point, Q=2P
 * @param[in]   curve	- ECCP_CURVE struct pointer
 * @param[in]   Px 		- x coordinate of point P
 * @param[in]   Py 		- y coordinate of point P
 * @param[out]  Qx 		- x coordinate of point Q
 * @param[out]  Qy 		- y coordinate of point Q
 * @return      PKE_SUCCESS(success), other(error)
 */
unsigned char pke_eccp_point_del(eccp_curve_t *curve, unsigned int *k, unsigned int *Px, unsigned int *Py,
					  unsigned int *Qx, unsigned int *Qy);
/**
 * @brief       ECCP curve point add, Q=P1+P2
 * @param[in]   eccp_curve 	- ECCP_CURVE struct pointer
 * @param[in]   P1x 		- x coordinate of point P1
 * @param[in]   P1y 		- y coordinate of point P1
 * @param[in]   P2x 		- x coordinate of point P2
 * @param[in]   P2y 		- y coordinate of point P2
 * @param[out]  Qx 			- x coordinate of point Q=P1+P2
 * @param[out]  Qy 			- y coordinate of point Q=P1+P2
 * @return      PKE_SUCCESS(success), other(error)
 */
unsigned char pke_eccp_point_add(eccp_curve_t *curve, unsigned int *P1x, unsigned int *P1y, unsigned int *P2x, unsigned int *P2y,
					  unsigned int *Qx, unsigned int *Qy);
/**
 * @brief       check whether the input point P is on ECCP curve or not
 * @param[in]   curve	- ECCP_CURVE struct pointer
 * @param[in]   Px		- x coordinate of point P
 * @param[in]   Py		- y coordinate of point P
 * @return      PKE_SUCCESS(success, on the curve), other(error or not on the curve)
 */
unsigned char pke_eccp_point_verify(eccp_curve_t *curve, unsigned int *Px, unsigned int *Py);
/**
 * @brief       ECCP curve point mul(random point), Q=[k]P
 * @param[in]   curve	- ECCP_CURVE struct pointer
 * @param[in]   k	 	- scalar
 * @param[in]   Px 		- x coordinate of point P
 * @param[in]   Py 		- y coordinate of point P
 * @param[out]  Qx 		- x coordinate of point Q
 * @param[out]  Qy 		- y coordinate of point Q
 * @return      PKE_SUCCESS(success), other(error)
 */
unsigned char pke_eccp_point_mul(eccp_curve_t *curve, unsigned int *k, unsigned int *Px, unsigned int *Py,
					  unsigned int *Qx, unsigned int *Qy);
/**
 * @brief       out = a*b mod modulus
 * @param[in]   modulus	- modulus
 * @param[in]   a 		- integer a
 * @param[in]   b 		- integer b
 * @param[in]   wordLen	- word length of modulus, a, b
 * @param[out]  out		- out = a*b mod modulus
 * @return      PKE_SUCCESS(success), other(error)
 */
unsigned char pke_mod_mul(const unsigned int *modulus, const unsigned int *a, const unsigned int *b,
 							unsigned int *out, unsigned int wordLen);
/**
 * @brief       ainv = a^(-1) mod modulus
 * @param[in]   modulus		- modulus
 * @param[in]   a 			- integer a
 * @param[in]   ainv 		- ainv = a^(-1) mod modulus
 * @param[in]   modWordLen	- word length of modulus, ainv
 * @param[in]   aWordLen 	- word length of integer a
 * @return: 	PKE_SUCCESS(success), other(inverse not exists or error)
 */
 unsigned char pke_mod_inv(const unsigned int *modulus, const unsigned int *a, unsigned int *ainv, unsigned int modWordLen,
 				   unsigned int aWordLen);
/**
 * @brief       out = (a+b) mod modulus
 * @param[in]   modulus - modulus
 * @param[in]   a 		- integer a
 * @param[in]   b 		- integer b
 * @param[in]   wordLen - word length of modulus, a, b
 * @param[out]  out 	- out = a+b mod modulus
 * @return      PKE_SUCCESS(success), other(error)
 */
unsigned char pke_mod_add(const unsigned int *modulus, const unsigned int *a, const unsigned int *b,
				   unsigned int *out, unsigned int wordLen);
/**
 * @brief       out = (a-b) mod modulus
 * @param[in]   modulus	- input, modulus
 * @param[in]  	a		- input, integer a
 * @param[in]   b		- input, integer b
 * @param[in]   wordLen - input, word length of modulus, a, b
 * @param[out]  out		- output, out = a-b mod modulus
 * @return      PKE_SUCCESS(success), other(error)
 */
unsigned char pke_mod_sub(const unsigned int *modulus, const unsigned int *a, const unsigned int *b,
 				   unsigned int *out, unsigned int wordLen);
/**
 * @brief       c25519 point mul(random point), Q=[k]P
 * @param[in]   curve	- c25519 curve struct pointer
 * @param[in]  	k		- scalar
 * @param[in]   Pu		- u coordinate of point P
 * @param[out]  Qu		- u coordinate of point Q
 * @return      PKE_SUCCESS(success), other(error)
 */
unsigned char pke_x25519_point_mul(mont_curve_t *curve, unsigned int *k, unsigned int *Pu, unsigned int *Qu);
/**
 * @brief       edwards25519 curve point mul(random point), Q=[k]P
 * @param[in]   curve	- edwards25519 curve struct pointer
 * @param[in]  	k		- scalar
 * @param[in]   Px 		- x coordinate of point P
 * @param[in]   Py 		- y coordinate of point P
 * @param[out]  Qx 		- x coordinate of point Q
 * @param[out]  Qy 		- y coordinate of point Q
 * @return      PKE_SUCCESS(success), other(error)
 */
unsigned char pke_ed25519_point_mul(edward_curve_t *curve, unsigned int *k, unsigned int *Px, unsigned int *Py,
					  unsigned int *Qx, unsigned int *Qy);
/**
 * @brief       edwards25519 point add, Q=P1+P2
 * @param[in]   curve	- edwards25519 curve struct pointer
 * @param[in]  	k		- scalar
 * @param[in]   P1x 		- x coordinate of point P1
 * @param[in]   P1y 		- y coordinate of point P1
 * @param[in]   P2x 		- x coordinate of point P2
 * @param[in]   P2y 		- y coordinate of point P2
 * @param[out]  Qx 			- x coordinate of point Q=P1+P2
 * @param[out]  Qy 			- y coordinate of point Q=P1+P2
 * @return      PKE_SUCCESS(success), other(error)
 */
unsigned char pke_ed25519_point_add(edward_curve_t *curve, unsigned int *P1x, unsigned int *P1y, unsigned int *P2x, unsigned int *P2y,
					  unsigned int *Qx, unsigned int *Qy);
/**
 * @brief		c = a mod b
 * @param[in]   modulus  - modulus
 * @param[in]   a 		 - integer a
 * @param[in]   b 		 - integer b
 * @param[in]   aWordLen - word length of a
 * @param[in]   bWordLen - word length of b
 * @return		PKE_SUCCESS(success), other(error)
 */
unsigned char pke_mod(unsigned int *a, unsigned int aWordLen, unsigned int *b, unsigned int *b_h, unsigned int *b_n1,
				unsigned int bWordLen, unsigned int *c);




