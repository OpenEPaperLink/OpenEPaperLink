/*-------------------------------------------------------------------------
   realloc.c - dynamic memory allocation

   Copyright (C) 2004, Vangelis Rokas <vrokas at otenet.gr>

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

#include <malloc.h>

extern unsigned char _MALLOC_SPEC *heap;

unsigned char _MALLOC_SPEC *realloc(unsigned char _MALLOC_SPEC *mblock, unsigned char len)
{
  _malloc_rec _MALLOC_SPEC *pHeap;			/* pointer to block header */
  _malloc_rec _MALLOC_SPEC *temp;
  unsigned char bLen;			/* size of block  */

  if(len >= MAX_BLOCK_SIZE)
    return ((unsigned char _MALLOC_SPEC *)0);

  /* if mblock is NULL, then same as malloc */
  if(!mblock)
    return (malloc(len));

  /* if len is 0 */
  if(len == 0) {
    free(mblock);
    return ((unsigned char _MALLOC_SPEC *)0);
  }
  
  pHeap = (_malloc_rec _MALLOC_SPEC *)((unsigned int)mblock - 1);
  bLen = pHeap->bits.count;

  /* block too small for len bytes + 1 byte header <===> bLen < len + 1 <===> blen <= len */
  if (bLen <= len) {
    /* so, new segment has size bigger than the old one, we can return a
     * valid pointer only when after the block there is an empty block that
     * can be merged to produce a new block of the requested size, otherwise
     * we return NULL */
    temp = _mergeHeapBlock(pHeap, len);

    if(!temp) {
      /* no could not find a valid block, return NULL */
      return ((unsigned char _MALLOC_SPEC *)0);
    }

    //pHeap = temp; /* temp == pHeap */
    bLen = pHeap->bits.count;
  }

  len++; /* increase to also count the header */
  
  if(bLen > len) {
    /* new segment is smaller than the old one (or the merged one), that's easy! */
    pHeap->bits.count = len;
    temp = (_malloc_rec _MALLOC_SPEC *)((unsigned int)pHeap + len);
    temp->bits.alloc = 0;
    temp->bits.count = bLen - len;
  }

  return (mblock);
}
