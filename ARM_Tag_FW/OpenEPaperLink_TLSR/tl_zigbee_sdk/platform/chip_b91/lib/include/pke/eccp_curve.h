/********************************************************************************************************
 * @file	eccp_curve.h
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
/********* pke version:1.1 *********/
#ifndef ECCP_CURVE_H
#define ECCP_CURVE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "string.h"

//sample ecc curve
#define SUPPORT_BRAINPOOLP160R1
#define SUPPORT_SECP192R1
#define SUPPORT_SECP224R1
#define SUPPORT_SECP256R1
//#define SUPPORT_SECP384R1
//#define SUPPORT_BRAINPOOLP512R1
//#define SUPPORT_SECP521R1


//#define PKE_HP


#define POINT_NOT_COMPRESSED      (0x04)


// eccp curve struct
#ifdef PKE_HP
typedef struct
{
    unsigned int eccp_p_bitLen;        //bit length of prime p
    unsigned int eccp_n_bitLen;        //bit length of order n
    unsigned int *eccp_p;
    unsigned int *eccp_p_h;
    unsigned int *eccp_a;
    unsigned int *eccp_b;
    unsigned int *eccp_Gx;
    unsigned int *eccp_Gy;
    unsigned int *eccp_n;
    unsigned int *eccp_n_h;
    unsigned int *eccp_half_Gx;
    unsigned int *eccp_half_Gy;
} eccp_curve_t;
#else
typedef struct
{
    unsigned int eccp_p_bitLen;        //bit length of prime p
    unsigned int eccp_n_bitLen;        //bit length of order n
    unsigned int *eccp_p;              //prime p
    unsigned int *eccp_p_h;
    unsigned int *eccp_p_n1;
    unsigned int *eccp_a;
    unsigned int *eccp_b;
    unsigned int *eccp_Gx;
    unsigned int *eccp_Gy;
    unsigned int *eccp_n;              //order of curve or point(Gx,Gy)
    unsigned int *eccp_n_h;
    unsigned int *eccp_n_n1;
} eccp_curve_t;
#endif



#ifdef SUPPORT_BRAINPOOLP160R1
extern eccp_curve_t brainpoolp160r1[1];
#endif

#ifdef SUPPORT_SECP192R1
extern eccp_curve_t secp192r1[1];
#endif

#ifdef SUPPORT_SECP224R1
extern eccp_curve_t secp224r1[1];
#endif

#ifdef SUPPORT_SECP256R1
extern eccp_curve_t secp256r1[1];
#endif

#ifdef SUPPORT_SECP384R1
extern eccp_curve_t secp384r1[1];
#endif

#ifdef SUPPORT_BRAINPOOLP512R1
extern eccp_curve_t brainpoolp512r1[1];
#endif

#ifdef SUPPORT_SECP521R1
extern eccp_curve_t secp521r1[1];
#endif



/********* Curve25519 struct *********/
typedef struct
{
    unsigned int p_bitLen;        //bit length of prime p
    unsigned int *p;
    unsigned int *p_h;
    unsigned int *p_n1;
    unsigned int *a24;            //(A-2)/4
    unsigned int *u;
    unsigned int *v;
    unsigned int *n;              //order of curve or point(Gx,Gy)
    unsigned int *n_h;
    unsigned int *n_n1;
    unsigned int *h;
} mont_curve_t;


/********* Edward Curve 25519 struct *********/
typedef struct
{
    unsigned int p_bitLen;        //bit length of prime p
    unsigned int *p;
    unsigned int *p_h;
    unsigned int *p_n1;
    unsigned int *d;
    unsigned int *Gx;
    unsigned int *Gy;
    unsigned int *n;              //order of curve or point(Gx,Gy)
    unsigned int *n_h;
    unsigned int *n_n1;
    unsigned int *h;
} edward_curve_t;


#ifdef __cplusplus
}
#endif

#endif
