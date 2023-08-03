/*-------------------------------------------------------------------------
   memmisc.c - heap handling functions

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
#include <string.h>

_malloc_rec __data *_malloc_heap = NULL;

void _initHeap (void __data *ptr, size_t size)
{
    _malloc_rec __data *head;
    size_t used_size = 0;
    size_t block_size;

    if (ptr == NULL || size == 0)
    {
        _malloc_heap = NULL;
        return;
    }

    _malloc_heap = (_malloc_rec __data *)ptr;
    head = _malloc_heap;
    /* we need one byte as the end of block list marker */
    size--;

    while (used_size < size) {
        /* a guess of the next block size */
        block_size = (size - used_size); /* thus: block_size > 0 */
        if (block_size > MAX_BLOCK_SIZE)
	    block_size = MAX_BLOCK_SIZE;
 
        /* now we can create the block */
        head->datum = block_size;
        head = NEXTREC(head);
 
        used_size += block_size;
    }

    /* mark end of block list */
    head->datum = 0;
}

/* try to allocate a block from the heap of at least size bytes,
 * merging adjacent blocks if necessary.
 * if ptr is not NULL, it may be included into the merged blocks.
 */
void __data *_allocateHeap (void __data *ptr, size_t size)
{
    _malloc_rec __data *current_head, *next_head, *realloc_head;
    size_t block_size;

    if (_malloc_heap == NULL)
        return ((void __data *)NULL);

    if (size >= MAX_BLOCK_SIZE)
        return ((void __data *)NULL);

    if (ptr)
        realloc_head = PTR2REC(ptr);
    else
        realloc_head = NULL;

    current_head = next_head = _malloc_heap;
    block_size = 0;
    size++;
    while (next_head->datum != 0)
    {
        if (next_head->bits.alloc == 0 || next_head == realloc_head)
        {
            block_size += next_head->bits.count;
            if (block_size >= size)
            {
                /* block found */
                if (realloc_head)
                {
                    memmove (REC2PTR(current_head), ptr, realloc_head->bits.count - 1);
                    if (next_head != realloc_head)
                        realloc_head->bits.alloc = 0;
                }
                current_head->datum = ALLOC_FLAG + size;
                if (block_size > size)
                {
                    next_head = NEXTREC(current_head);
                    next_head->datum = block_size - size;
                }
                return REC2PTR(current_head);
            }
            next_head = NEXTREC(next_head);
        }
        else
        {
            current_head = next_head = NEXTREC(next_head);
            block_size = 0;
        }
    }
    return ((void __data *)NULL);
}
