/*-------------------------------------------------------------------------
   strmputchar.c - stream putchar dispatch function

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

#include <stdio.h>

void
__stream_putchar (FILE *stream, char c)
{
  unsigned char deref;

  deref = (unsigned char) (((unsigned long) stream) >> 16);

#if _DEBUG
  io_str ("__stream_putchar: ");
  io_long ((unsigned long) stream);
#endif

  if (deref == 0x80)
    {
      /* this is a data/near memory pointer */
      *(*(char **) stream) = c;
      *(char **) stream += 1;
    }
  else if (deref & 0x20)
    {
      deref ^= 0x20;
      if (deref == USART_DEREF)
        __stream_usart_putchar (c);
      else if (deref == MSSP_DEREF)
        __stream_mssp_putchar (c);
      else if (deref == USER_DEREF)
        putchar (c);
      else if (deref == GPSIM_DEREF)    /* see stdio.h for info on this */
        __stream_gpsim_putchar (c);     /* feature */
    }
}
