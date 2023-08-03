/*-------------------------------------------------------------------------
   delay.h - delay functions header file

   Copyright (C) 2005, Vangelis Rokas <vrokas AT otenet.gr>

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License 
   along with this library; see the file COPYING. If not, write to the
   Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.

   As a special exception, if you link this library with other files,
   some of which are compiled with SDCC, to produce an executable,
   this library does not by itself cause the resulting executable to
   be covered by the GNU General Public License. This exception does
   not however invalidate any other reasons why the executable file
   might be covered by the GNU General Public License.
-------------------------------------------------------------------------*/

#ifndef __DELAY_H__
#define __DELAY_H__

#pragma library c

/*
 * the delayNNtcy family of functions performs a
 * delay of NN cycles. Possible values for NN are:
 *   10  10*n cycles delay
 *  100  100*n cycles delay
 *   1k  1000*n cycles delay
 *  10k  10000*n cycles delay
 * 100k  100000*n cycles delay
 *   1m  1000000*n cycles delay
 */
 
void delay10tcy(unsigned char) __wparam;
void delay100tcy(unsigned char) __wparam;
void delay1ktcy(unsigned char) __wparam;
void delay10ktcy(unsigned char) __wparam;
void delay100ktcy(unsigned char) __wparam;
void delay1mtcy(unsigned char) __wparam;

#endif
