/*
 * tgunzip  -  gzip decompressor example
 *
 * Copyright (c) 2003 by Joergen Ibsen / Jibz
 * All Rights Reserved
 *
 * http://www.ibsensoftware.com/
 *
 * Copyright (c) 2014-2016 by Paul Sokolovsky
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
#include <stdio.h>

#include "uzlib.h"

/* produce decompressed output in chunks of this size */
/* default is to decompress byte by byte; can be any other length */
#define OUT_CHUNK_SIZE 1

void exit_error(const char *what)
{
   printf("ERROR: %s\n", what);
   exit(1);
}

int main(int argc, char *argv[])
{
    FILE *fin, *fout;
    unsigned int len, dlen, outlen;
    const unsigned char *source;
    unsigned char *dest;
    int res;

    printf("tgunzip - example from the tiny inflate library (www.ibsensoftware.com)\n\n");

    if (argc < 3)
    {
       printf(
          "Syntax: tgunzip <source> <destination>\n\n"
          "Both input and output are kept in memory, so do not use this on huge files.\n");

       return 1;
    }

    uzlib_init();

    /* -- open files -- */

    if ((fin = fopen(argv[1], "rb")) == NULL) exit_error("source file");

    if ((fout = fopen(argv[2], "wb")) == NULL) exit_error("destination file");

    /* -- read source -- */

    fseek(fin, 0, SEEK_END);

    len = ftell(fin);

    fseek(fin, 0, SEEK_SET);

    source = (unsigned char *)malloc(len);

    if (source == NULL) exit_error("memory");

    if (fread((unsigned char*)source, 1, len, fin) != len) exit_error("read");

    fclose(fin);

    if (len < 4) exit_error("file too small");

    /* -- get decompressed length -- */

    dlen =            source[len - 1];
    dlen = 256*dlen + source[len - 2];
    dlen = 256*dlen + source[len - 3];
    dlen = 256*dlen + source[len - 4];

    outlen = dlen;

    /* there can be mismatch between length in the trailer and actual
       data stream; to avoid buffer overruns on overlong streams, reserve
       one extra byte */
    dlen++;

    dest = (unsigned char *)malloc(dlen);

    if (dest == NULL) exit_error("memory");

    /* -- decompress data -- */

    struct uzlib_uncomp d;
//    uzlib_uncompress_init(&d, malloc(32768), 32768);
    uzlib_uncompress_init(&d, NULL, 0);

    /* all 3 fields below must be initialized by user */
    d.source = source;
    d.source_limit = source + len - 4;
    d.source_read_cb = NULL;

    res = uzlib_gzip_parse_header(&d);
    if (res != TINF_OK) {
        printf("Error parsing header: %d\n", res);
        exit(1);
    }

    d.dest_start = d.dest = dest;

    while (dlen) {
        unsigned int chunk_len = dlen < OUT_CHUNK_SIZE ? dlen : OUT_CHUNK_SIZE;
        d.dest_limit = d.dest + chunk_len;
        res = uzlib_uncompress_chksum(&d);
        dlen -= chunk_len;
        if (res != TINF_OK) {
            break;
        }
    }

    if (res != TINF_DONE) {
        printf("Error during decompression: %d\n", res);
        exit(-res);
    }

    printf("decompressed %lu bytes\n", d.dest - dest);

#if 0
    if (d.dest - dest != gz.dlen) {
        printf("Invalid decompressed length: %lu vs %u\n", d.dest - dest, gz.dlen);
    }

    if (tinf_crc32(dest, gz.dlen) != gz.crc32) {
        printf("Invalid decompressed crc32\n");
    }
#endif

    /* -- write output -- */

    fwrite(dest, 1, outlen, fout);

    fclose(fout);

    return 0;
}
