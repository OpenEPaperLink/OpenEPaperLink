/* ----------------------------------------------------------------------------
 * This file is part of the xPack distribution.
 *   (https://xpack.github.io)
 * Copyright (c) 2020 Liviu Ionescu.
 *
 * Permission to use, copy, modify, and/or distribute this software 
 * for any purpose is hereby granted, under the terms of the MIT license.
 * -------------------------------------------------------------------------- */

#if defined(_WIN32)
__declspec(dllexport)
#endif
int
add(int a, int b)
{
  return a + b;
}
