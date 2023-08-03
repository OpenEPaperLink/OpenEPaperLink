/*-------------------------------------------------------------------------
   memmisc.c - heap handling functions

   Copyright (C) 2005, Vangelis Rokas <vrokas at otenet.gr>

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

void _initHeap(unsigned char _MALLOC_SPEC *dheap, unsigned int heapsize)
{
  _malloc_rec _MALLOC_SPEC *pHeap;
  unsigned int hsize=0;
  int bsize;
  
    pHeap = (_malloc_rec _MALLOC_SPEC *)dheap;
    if (heapsize == 0) return;
    /* we need one byte as the end of block list marker */
    heapsize--;
    
    while (hsize < heapsize) {
      /* a guess of the next block size */
      bsize = (heapsize - hsize); /* thus: bsize > 0 */
      if(bsize > MAX_BLOCK_SIZE) bsize = MAX_BLOCK_SIZE;
      
      /* now we can create the block */
      pHeap->datum = bsize;
      pHeap = (_malloc_rec _MALLOC_SPEC *)((unsigned int)pHeap + bsize);

      hsize += bsize;
    }

    /* mark end of block list */
    pHeap->datum = 0;
}

/* search heap starting from sBlock for a block of size bSize, merging
 * adjacent blocks if necessery */
_malloc_rec _MALLOC_SPEC *_mergeHeapBlock(_malloc_rec _MALLOC_SPEC *sBlock, unsigned char bSize)
{
  _malloc_rec _MALLOC_SPEC *temp;
  unsigned char bLen;
  unsigned char eLen;
  unsigned char dat;
  
    bLen = sBlock->bits.count;
  
    /* current block is not enough, see if we can merge some adjacent memory
     * blocks to make it fit */
    temp = (_malloc_rec _MALLOC_SPEC *)((unsigned int)sBlock + bLen);	//sBlock->bits.count);
    eLen = bLen;
    while((temp->datum) && (!temp->bits.alloc) && (eLen <= bSize)) {
      eLen += (dat=temp->bits.count);
      temp = (_malloc_rec _MALLOC_SPEC *)((unsigned int)temp + dat);
    }

    if(eLen > bSize) {
      unsigned char i;
            
        /* yes, there are some free blocks that can be merged, so merge them... */
        temp = sBlock;
        while(eLen > 0) {
          if(eLen > MAX_BLOCK_SIZE)i = MAX_BLOCK_SIZE;
            else i = eLen;
          temp->datum = i;
          temp = (_malloc_rec _MALLOC_SPEC *)((unsigned int)temp + i);
          eLen -= i;
        }

        /* return block starts at the old block start address */
        return (sBlock);
    } else {
      
      /* no, there are no free blocks after sBlock, so return NULL */
      return ((_malloc_rec _MALLOC_SPEC *)0);
    }
}
