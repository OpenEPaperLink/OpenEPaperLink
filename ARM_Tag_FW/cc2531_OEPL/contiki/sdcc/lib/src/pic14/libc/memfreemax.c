/*-------------------------------------------------------------------------
   memfreemax.c - return size of maximum unallocated heap block

   Copyright (C) 2005, Vangelis Rokas <vrokas at otenet.gr>

   Modifications for PIC14 by
   Copyright (C) 2019 Gonzalo Pérez de Olaguer Córdoba <salo@gpoc.es>

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

#include <pic14_malloc.h>

size_t memfreemax (void)
{
    _malloc_rec __data *head;
    size_t size = 1;
    size_t len;
 
    if (_malloc_heap == NULL)
        return 0;
 
      head = _malloc_heap;
      
      while ((len = head->bits.count) != 0)
      {
          if (!head->bits.alloc && (len > size))
                size = len;
          
          head = NEXTREC(head);
      }
 
    /* do not count the block header */
    return size - 1;
}
