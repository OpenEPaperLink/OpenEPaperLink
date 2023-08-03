/*-------------------------------------------------------------------------
   at89x051.h - Register Declarations for Atmel AT89C1051, AT89C2051 and AT89C4051 Processors
   based on 8051.h (8051.h must be in mcs51 subdirectory)
   KEIL C compatible definitions are included

   Copyright (C) 2000, Bela Torok / bela.torok@kssg.ch

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

#ifndef AT89Cx051_H
#define AT89Cx051_H

#include <8051.h>     /* load difinitions for the 8051 core */

#ifdef REG8051_H
#undef REG8051_H
#endif

/* remove non existing registers */

#ifdef P0				  /* P0 is defined in <8051.h> */
#undef P0				  /* AT89Cx051 has no P0 */
#undef P0_0				  /* undefine bit addressable registers in P0 */
#undef P0_1
#undef P0_2
#undef P0_3
#undef P0_4
#undef P0_5
#undef P0_6
#undef P0_7
#endif

#ifdef P2				  /* P2 is defined in <8051.h> */
#undef P2				  /* AT89Cx051 has no P2 */
#undef P2_0				  /* undefine bit addressable registers in P2 */
#undef P2_1
#undef P2_2
#undef P2_3
#undef P2_4
#undef P2_5
#undef P2_6
#undef P2_7
#endif

#endif

