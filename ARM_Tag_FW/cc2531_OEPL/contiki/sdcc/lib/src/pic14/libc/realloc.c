/*-------------------------------------------------------------------------
   realloc.c - dynamic memory allocation

   Copyright (C) 2004, Vangelis Rokas <vrokas at otenet.gr>

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

void __data *realloc (void __data *ptr, size_t size)
{
    _malloc_rec __data *current_head, *next_head;
    size_t current_size;

    if (ptr)
    {
        current_head = PTR2REC(ptr);
        current_size = current_head->bits.count - 1;
        /* not changing the size */
        if (size == current_size)
            return ptr;
        /* reduce the size of the block */
        if (size < current_size)
        {
            current_head->bits.count = size + 1;
            next_head = NEXTREC(current_head);
            next_head->datum = current_size - size;
            return ptr;
        }
        /* increase the size of the block */
        next_head = NEXTREC(current_head);
        while (next_head->bits.alloc == 0 && next_head->bits.count > 0)
        {
            current_size += next_head->bits.count;
            if (size <= current_size)
            {
                current_head->bits.count = size + 1;
                if (size < current_size)
                {
                    next_head = NEXTREC(current_head);
                    next_head->datum = current_size - size;
                }
                return ptr;
            }
            next_head = NEXTREC(next_head);
        }
    }

    /* reallocation needed */
    return _allocateHeap (ptr, size);
}

