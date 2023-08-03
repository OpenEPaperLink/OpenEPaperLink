/*-------------------------------------------------------------------------
   malloc.h - dynamic memory allocation header

   Copyright (C) 2004, Vangelis Rokas <vrokas AT otenet.gr>

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

/*
 * Structure of memory block header:
 * bit 7 (MSB): allocated flag
 * bits 0-6: pointer to next block (max length: 126)
 *
 */


#ifndef __MALLOC_H__
#define __MALLOC_H__


/* set EMULATION to 1 to enable native Linux malloc emulation layer. This is
 * for debugging purposes only */
 
#ifndef EMULATION
#define EMULATION	0
#endif

#if EMULATION
//#define malloc	pic16_malloc
//#define free	pic16_free
//#define realloc	pic16_realloc
//#define calloc	pic16_calloc

//#define lmalloc		pic16_lmalloc
//#define lfree		pic16_lfree
//#define lrealloc	pic16_lrealloc
//#define lcalloc		pic16_lcalloc
#define _MALLOC_SPEC

#else

#pragma library c

#define _MALLOC_SPEC	__data

#endif

/* when MALLOC_MAX_FIRST is 1, the memory allocator tries to find a block
 * that fits the requested size without merging (initially), if this block
 * is not found, then tries to merge adjacent blocks. If MALLOC_MAX_FIRST is
 * set 0, then the allocator tries to merge adjacent blocks in the first
 * place.  Both behaviours may give better results when used in certain
 * circumstancs. I.e. if realloc is to be used, leaving some space after the
 * block, will allow realloc to allocate it, otherwise it may result in much
 * more memory fragmentation. An algorithm can be implemented to allow small
 * fragments to be allocated but this is much too complicated for the PIC18F's
 * architecture */
#define MALLOC_MAX_FIRST	0

#define MAX_BLOCK_SIZE	0x7f		/* 127 bytes */
#define	MAX_HEAP_SIZE	0x200		/* 512 bytes */
#define _MAX_HEAP_SIZE	(MAX_HEAP_SIZE-1)

#define ALLOC_FLAG		0x80
#define HEADER_SIZE		1

/* memory block header, max size 127 bytes, 126 usable */
typedef union {
  unsigned char datum;
  struct {
    unsigned count: 7;
    unsigned alloc: 1;
  } bits;
} _malloc_rec;


/* initialize heap, should be called before any call to malloc/realloc/calloc */
void _initHeap(unsigned char _MALLOC_SPEC *dHeap, unsigned int heapsize);


/* start searching for a block of size at least bSize, merge adjacent blocks
 * if necessery */
_malloc_rec _MALLOC_SPEC *_mergeHeapBlock(_malloc_rec _MALLOC_SPEC *sBlock, unsigned char bSize);


/* allocate a memory block */
unsigned char _MALLOC_SPEC *malloc(unsigned char len);


/* same as malloc, but clear memory */
unsigned char _MALLOC_SPEC *calloc(unsigned char len);


/* expand or reduce a memory block, if mblock is NULL, then same as malloc */
unsigned char _MALLOC_SPEC *realloc(unsigned char _MALLOC_SPEC *mblock, unsigned char len);


/* free a memory block */
void free(unsigned char _MALLOC_SPEC *);


/* returns the size of all the unallocated memory */
unsigned int memfree(void);


/* return the size of the maximum unallocated memory block */
unsigned int memfreemax(void);

#endif /* __MALLOC_H__ */
