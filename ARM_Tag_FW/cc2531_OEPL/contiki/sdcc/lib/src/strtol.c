/*---------------------------------------------------------------------
   strtol() - convert a string to a long int and return it

   Copyright (C) 2018, Philipp Klaus Krause . krauseph@informatik.uni-freiburg.de

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
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

#include <stdlib.h>

#include <stdbool.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>

#pragma disable_warning 196

long int strtol(const char *nptr, char **endptr, int base)
{
  const char *ptr = nptr;
  const char *rptr;
  unsigned long int u;
  bool neg;

  while (isblank (*ptr))
    ptr++;

  neg = (*ptr == '-');

  if (*ptr == '-')
    {
      neg = true;
      ptr++;
    }
  else
    neg = false;

  // strtoul() would accept leading blanks or signs (that might come after '-' handled above)
  if (neg && (isblank (*ptr) || *ptr == '-' || *ptr == '+'))
    {
      if (endptr)
        *endptr = nptr;
      return (0);
    }

  u = strtoul(ptr, &rptr, base);

  // Check for conversion error
  if (rptr == ptr)
    {
      if (endptr)
        *endptr = nptr;
      return (0);
    }

  if (endptr)
    *endptr = rptr;

  // Check for range error
  if (!neg && u > LONG_MAX)
    {
      errno = ERANGE;
      return (LONG_MAX);
    }
  else if (neg && u > -LONG_MIN)
    {
      errno = ERANGE;
      return (LONG_MIN);
    }

  return (neg ? -u : u);
}

