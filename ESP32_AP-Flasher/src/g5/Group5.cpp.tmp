#include "g5enc.inl"
#include "g5dec.inl"
//
// Group5 1-bit image compression library
// Written by Larry Bank
// Copyright (c) 2024 BitBank Software, Inc.
//
// Use of this software is governed by the Business Source License
// included in the file ./LICENSE.
//
// As of the Change Date specified in that file, in accordance with
// the Business Source License, use of this software will be governed
// by the Apache License, Version 2.0, included in the file
// ./APL.txt.

//
// Decoder C++ wrapper functions
//
int G5DECODER::init(int iWidth, int iHeight, uint8_t *pData, int iDataSize)
{
    return g5_decode_init(&_g5dec, iWidth, iHeight, pData, iDataSize);
} /* init() */

int G5DECODER::decodeLine(uint8_t *pOut)
{
    return g5_decode_line(&_g5dec, pOut);
} /* decodeLine() */

//
// Encoder C++ wrapper functions
//
int G5ENCODER::init(int iWidth, int iHeight, uint8_t *pOut, int iOutSize)
{
    return g5_encode_init(&_g5enc, iWidth, iHeight, pOut, iOutSize);
} /* init() */

int G5ENCODER::encodeLine(uint8_t *pPixels)
{
    return g5_encode_encodeLine(&_g5enc, pPixels);
} /* encodeLine() */

int G5ENCODER::size()
{
    return g5_encode_getOutSize(&_g5enc);
} /* size() */

