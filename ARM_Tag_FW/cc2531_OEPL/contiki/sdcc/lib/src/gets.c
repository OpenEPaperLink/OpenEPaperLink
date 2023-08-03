/*-------------------------------------------------------------------------
   gets.c

   Copyright (C) 2000, Johan Knol <johan.knol AT iduna.nl>

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

char *
gets (char *s)
{
  char c;
  unsigned int count = 0;

  while (1)
    {
      c = getchar ();
      switch(c)
        {
        case '\b': /* backspace */
          if (count)
            {
              putchar ('\b');
              putchar (' ');
	      putchar ('\b');
	      --s;
	      --count;
            }
          break;

        case '\n':
        case '\r': /* CR or LF */
          putchar ('\r');
          putchar ('\n');
          *s = 0;
          return s;

        default:
          *s++ = c;
          ++count;
          putchar (c);
          break;
        }
    }
}
