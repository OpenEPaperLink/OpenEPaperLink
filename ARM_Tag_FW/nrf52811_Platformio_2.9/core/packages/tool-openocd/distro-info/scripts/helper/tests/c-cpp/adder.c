/* ----------------------------------------------------------------------------
 * This file is part of the xPack distribution.
 *   (https://xpack.github.io)
 * Copyright (c) 2020 Liviu Ionescu.
 *
 * Permission to use, copy, modify, and/or distribute this software 
 * for any purpose is hereby granted, under the terms of the MIT license.
 * -------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>

extern int
#if defined(_WIN32)
__declspec(dllimport)
#endif
add(int a, int b);

int
main(int argc, char* argv[])
{
  int sum = atoi(argv[1]) + atoi(argv[2]);
  printf("%d\n", sum);

  return 0;
}
