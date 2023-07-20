/*
 * tgzip  -  gzip compressor example
 *
 * Copyright (c) 2003 by Joergen Ibsen / Jibz
 * Copyright (c) 2014-2018 by Paul Sokolovsky
 *
 * http://www.ibsensoftware.com/
 *
 * This software is provided 'as-is', without any express
 * or implied warranty.  In no event will the authors be
 * held liable for any damages arising from the use of
 * this software.
 *
 * Permission is granted to anyone to use this software
 * for any purpose, including commercial applications,
 * and to alter it and redistribute it freely, subject to
 * the following restrictions:
 *
 * 1. The origin of this software must not be
 *    misrepresented; you must not claim that you
 *    wrote the original software. If you use this
 *    software in a product, an acknowledgment in
 *    the product documentation would be appreciated
 *    but is not required.
 *
 * 2. Altered source versions must be plainly marked
 *    as such, and must not be misrepresented as
 *    being the original software.
 *
 * 3. This notice may not be removed or altered from
 *    any source distribution.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "uzlib.h"

void exit_error(const char *what)
{
   printf("ERROR: %s\n", what);
   exit(1);
}

int main(int argc, char *argv[])
{
    FILE *fin, *fout;
    unsigned int len;
    unsigned char *source;

    printf("tgzip - example from the uzlib library\n\n");

    if (argc < 3)
    {
       printf(
          "Syntax: tgunzip <source> <destination>\n\n"
          "Both input and output are kept in memory, so do not use this on huge files.\n");

       return 1;
    }

    /* -- open files -- */

    if ((fin = fopen(argv[1], "rb")) == NULL) exit_error("source file");

    if ((fout = fopen(argv[2], "wb")) == NULL) exit_error("destination file");

    /* -- read source -- */

    fseek(fin, 0, SEEK_END);

    len = ftell(fin);

    fseek(fin, 0, SEEK_SET);

    source = (unsigned char *)malloc(len);

    if (source == NULL) exit_error("memory");

    if (fread(source, 1, len, fin) != len) exit_error("read");

    fclose(fin);

    /* -- compress data -- */

    struct uzlib_comp comp = {0};
    comp.dict_size = 32768;
    comp.hash_bits = 12;
    size_t hash_size = sizeof(uzlib_hash_entry_t) * (1 << comp.hash_bits);
    comp.hash_table = malloc(hash_size);
    memset(comp.hash_table, 0, hash_size);

    zlib_start_block(&comp);
    uzlib_compress(&comp, source, len);
    zlib_finish_block(&comp);

    printf("compressed to %u raw bytes\n", comp.outlen);

    /* -- write output -- */

    putc(0x1f, fout);
    putc(0x8b, fout);
    putc(0x08, fout);
    putc(0x00, fout); // FLG
    int mtime = 0;
    fwrite(&mtime, sizeof(mtime), 1, fout);
    putc(0x04, fout); // XFL
    putc(0x03, fout); // OS

    fwrite(comp.outbuf, 1, comp.outlen, fout);

    unsigned crc = ~uzlib_crc32(source, len, ~0);
    fwrite(&crc, sizeof(crc), 1, fout);
    fwrite(&len, sizeof(len), 1, fout);

    fclose(fout);

    return 0;
}
