/*---------------------------------------------------------------------
   strtoul() - convert a string to a unsigned long int and return it

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
#include <string.h>
#include <limits.h>
#include <errno.h>

static signed char _isdigit(const char c, unsigned char base)
{
  unsigned char v;

  if (c >= '0' && c <= '9')
    v = c - '0';
  else if (c >= 'a' && c <='z')
    v = c - 'a' + 10;
  else if (c >= 'A' && c <='Z')
    v = c - 'A' + 10;
  else
    return (-1);

  if (v >= base)
    return (-1);

  return (v);
}

unsigned long int strtoul(const char *nptr, char **endptr, int base)
{
  const char *ptr = nptr;
  unsigned long int ret;
  bool range_error = false;
  bool neg = false;

  while (isblank (*ptr))
    ptr++;

  // Handle sign.
  switch(*ptr)
    {
    case '-':
      neg = true;
    case '+':
      ptr++;
    }

  // base not specified.
  if (!base)
    {
      if (!strncmp (ptr, "0x", 2) || !strncmp (ptr, "0X", 2))
        {
          base = 16;
          ptr += 2;
        }
      else if (*ptr == '0')
        {
          base = 8;
          ptr++;
        }
      else
        base = 10;
    }
  // Handle optional hex prefix.
  else if (base == 16 && (!strncmp (ptr, "0x", 2) || !strncmp (ptr, "0X", 2)))
    ptr += 2;


  // Empty sequence conversion error
  if (_isdigit (*ptr, base) < 0)
    {
      if (endptr)
        *endptr = (char*)nptr;
      return (0);
    }

  for (ret = 0;; ptr++)
    {
      unsigned long int oldret;
      signed char digit = _isdigit (*ptr, base);

      if (digit < 0)
        break;

      oldret = ret;
      ret *= base;
      if (ret < oldret)
        range_error = true;

      ret += (unsigned char)digit;
    }

  if (endptr)
    *endptr = (char*)ptr;

  if (range_error)
    {
      errno = ERANGE;
      return (ULONG_MAX);
    }

  return (neg ? -ret : ret);
}

