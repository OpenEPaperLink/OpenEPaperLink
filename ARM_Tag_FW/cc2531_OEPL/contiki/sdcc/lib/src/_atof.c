/*-------------------------------------------------------------------------
   atof.c - converts an ASCII string to float

   Copyright (C) 2003, Jesus Calvino-Fraga, jesusc@ieee.org

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

#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

float atof(const char * s)
{
    float value, fraction;
    signed char iexp;
    bool sign;

    //Skip leading blanks
    while (isspace(*s)) s++;

    //Get the sign
    if (*s == '-')
    {
        sign=1;
        s++;
    }
    else
    {
        sign=0;
        if (*s == '+') s++;
    }

    //Get the integer part
    for (value=0.0; isdigit(*s); s++)
    {
        value=10.0*value+(*s-'0');
    }

    //Get the fraction
    if (*s == '.')
    {
        s++;
        for (fraction=0.1; isdigit(*s); s++)
        {
            value+=(*s-'0')*fraction;
            fraction*=0.1;
        }
    }

    //Finally, the exponent (not very efficient, but enough for now*/
    if (toupper(*s)=='E')
    {
        s++;
        iexp=(signed char)atoi(s);
        {
            while(iexp!=0)
            {
                if(iexp<0)
                {
                    value*=0.1;
                    iexp++;
                }
                else
                {
                    value*=10.0;
                    iexp--;
                }
            }
        }
    }

    if(sign) value*=-1.0;
    return (value);
}

