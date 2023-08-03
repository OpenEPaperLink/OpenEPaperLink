/*-------------------------------------------------------------------------
   rand.c - random number generation routines

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

/*
 * derived from the book "The C Programming Language," by Kernighan and Ritchie
 */

#include <stdlib.h>

static long do_rand(unsigned long *ctx)
{
  return ((*ctx = *ctx * 1103515245UL + 12345UL) % ((unsigned long)RAND_MAX + 1));
}

long rand_r(unsigned long *ctx)
{
  unsigned long val = (unsigned long) *ctx;
  
    *ctx = do_rand(&val);
    return (long) *ctx;
}

static unsigned long next = 1;

long rand(void)
{
  return do_rand(&next);
}

void srand(unsigned long seed)
{
  next = seed;
}

#ifdef TEST

main()
{
  int i;
  unsigned long myseed;

  stdout = STREAM_GPSIM;

  printf("seeding rand with 0x19610910: \n");
  srand(0x19610910);

  printf("generating three pseudo-random numbers:\n");
  for (i = 0; i < 10; i++)
  {
    printf("next random number = %ld\n", rand());
  }

  printf("generating the same sequence with rand_r:\n");
  myseed = 0x19610910;
  for (i = 0; i < 10; i++)
  {
    printf("next random number = %ld\n", rand_r(&myseed));
  }

  return 0;
}

#endif /* TEST */

