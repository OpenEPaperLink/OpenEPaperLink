uzlib - Deflate/Zlib-compatible LZ77 compression/decompression library
======================================================================

uzlib is a library which can decompress any valid Deflate, Zlib, and Gzip
(further called just "Deflate") bitstream, and compress data to Deflate-
compatible bitstream, albeit with lower compression ratio than Zlib Deflate
algorithm (very basic LZ77 compression algorithm is used instead, static
Deflate Huffman tree encoding is used for bitstream).

uzlib aims for minimal code size and runtime memory requirements, and thus
suitable for (deeply) embedded systems.

uzlib is based on:

* tinf library by Joergen Ibsen (Deflate decompression)
* Deflate Static Huffman tree routines by Simon Tatham
* LZ77 compressor by Paul Sokolovsky

Library integrated and maintained by Paul Sokolovsky.

(c) 2014-2020 Paul Sokolovsky

uzlib library is licensed under Zlib license.


Decompressor features
---------------------

Handling of input (compressed) stream:

* Can reside (fully) in memory.
* Can be received, byte by byte, from an application-defined callback
  function (which e.g. can read it from file or another I/O device).
* Combination of the above: a chunk of input is buffered in memory,
  when buffer is exhausted, the application callback is called to refill
  it.

Handling of output (decompressed) stream:

* In-memory decompression, where output stream fully resides in memory.
* Streaming decompression, which allows to process arbitrary-sized streams
  (longer than available memory), but requires in-memory buffer for Deflate
  dictionary window.
* Application specifies number of output bytes it wants to decompress,
  which can be as high as UINT_MAX to decompress everything into memory
  at once, or as low as 1 to decompress byte by byte, or any other value
  to decompress a chunk of that size.

Note that in regard to input stream handling, uzlib employs callback-based,
"pull-style" design. The control flow looks as follows:

1. Application requests uzlib to decompress given number of bytes.
2. uzlib performs decompression.
3. If more input is needed to decompress given number of bytes, uzlib
   calls back into application to provide more input bytes. (An
   implication of this is that uzlib will always return given number of
   output bytes, unless end of stream (or error) happens).

The original Zlib library instead features "push-style" design:

1. An application prepares arbitrary number of input bytes in a buffer,
   and free space in output buffer, and calls Zlib with these buffers.
2. Zlib tries to decode as much as possible input and produce as much
   as possible output. It returns back to the application if input
   buffer is exhausted, or output buffer is full, whatever happens
   first.

Currently, uzlib doesn't support push-style operation a-la Zlib.

Compressor features
-------------------

Compressor uses very basic implementation of LZ77 algorithm using hash
table to find repeating substrings. The size of the hash table (on which
compression efficiency depends), pointer to the hashtable memory, and
the size of LZ77 dictionary should be configured in `struct uzlib_comp`.

Currently, compressor doesn't support streaming operation, both input and
output must reside in memory. Neither it supports incremental operation,
entire input buffer is compressed at once with a single call to uzlib.

API and configuration
---------------------

The API is defined in the file [uzlib.h](src/uzlib.h) and should be largely
self-describing. There are also examples implementing gzip-compatible
compression and decompression applications in [examples/](examples/) for
further reference. (You may also refer to the original `tinf` README
below for additional information, but it's mostly provided for
historical reference, and `uzlib` largely evolved beyond it).

There are some compile-time options for the library, defined in
the file [uzlib_conf.h](src/uzlib_conf.h). They can be altered directly
in the file, or passed as the compiler options (`-DXXX=YYY`) when
building library.

Binary sizes
------------

To give an impression of code/data sizes of uzlib, the following figures
are provided. Numbers for *.o files are code sizes of individual
components (tinflate.o is decompressor, genlz77.o and defl_static.o -
compressor), and TINF_DATA is the size of the corresponding data
structure. These numbers are provided for different architectures,
with default uzlib configuration, and with compilers/their options
as specified.

```
gcc -m32 -Os
gcc (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0
2891 src/tinflate.o
381 src/genlz77.o
1685 src/defl_static.o
1284 TINF_DATA

arm-none-eabi-gcc -mthumb -mcpu=cortex-m4 -Os
arm-none-eabi-gcc (GNU Tools for Arm Embedded Processors 9-2019-q4-major) 9.2.1 20191025 (release) [ARM/arm-9-branch revision 277599]
1620 src/tinflate.o
180 src/genlz77.o
1131 src/defl_static.o
1284 TINF_DATA
```

---

Original tinf library README
============================

*For historical reference and to provide proper credit, the original `tinf`
library README follows. NOTE: Many parts no longer apply to `uzlib`. In
particular, API is different, features supported are largely extended,
and during decompression, there are checks to avoid erroneous/undefined
behavior on incorrect Deflate bitstreams.*

tinf - tiny inflate library
===========================

Version 1.00

Copyright (c) 2003 Joergen Ibsen

<http://www.ibsensoftware.com/>


About
-----

tinf is a small library implementing the decompression algorithm for the
deflate compressed data format (called 'inflate'). Deflate compression is
used in e.g. zlib, gzip, zip and png.

I wrote it because I needed a small in-memory zlib decompressor for a self-
extracting archive, and the zlib library added 15k to my program. The tinf
code added only 2k.

Naturally the size difference is insignificant in most cases. Also, the
zlib library has many more features, is more secure, and mostly faster.
But if you have a project that calls for a small and simple deflate
decompressor, give it a try :-)

While the implementation should be fairly compliant, it does assume it is
given valid compressed data, and that there is sufficient space for the
decompressed data.

Simple wrappers for decompressing zlib streams and gzip'ed data in memory
are supplied.

tgunzip, an example command-line gzip decompressor in C, is included.

The inflate algorithm and data format are from 'DEFLATE Compressed Data
Format Specification version 1.3' ([RFC 1951][1]).

The zlib data format is from 'ZLIB Compressed Data Format Specification
version 3.3' ([RFC 1950][2]).

The gzip data format is from 'GZIP file format specification version 4.3'
([RFC 1952][3]).

Ideas for future versions:

- the fixed Huffman trees could be built by `tinf_decode_trees()`
  using a small table
- memory for the `TINF_DATA` object should be passed, to avoid using
  more than 1k of stack space
- wrappers for unpacking zip archives and png images
- implement more in x86 assembler
- more sanity checks
- in `tinf_uncompress`, the (entry value of) `destLen` and `sourceLen`
  are not used
- blocking of some sort, so everything does not have to be in memory
- optional table-based huffman decoder

[1]: http://www.rfc-editor.org/rfc/rfc1951.txt
[2]: http://www.rfc-editor.org/rfc/rfc1950.txt
[3]: http://www.rfc-editor.org/rfc/rfc1952.txt


Functionality
-------------

    void tinf_init();

Initialise the global uninitialised data used by the decompression code.
This function must be called once before any calls to the decompression
functions.

    int tinf_uncompress(void *dest,
                        unsigned int *destLen,
                        const void *source,
                        unsigned int sourceLen);

Decompress data in deflate compressed format from `source[]` to `dest[]`.
`destLen` is set to the length of the decompressed data. Returns `TINF_OK`
on success, and `TINF_DATA_ERROR` on error.

    int tinf_gzip_uncompress(void *dest,
                             unsigned int *destLen,
                             const void *source,
                             unsigned int sourceLen);

Decompress data in gzip compressed format from `source[]` to `dest[]`.
`destLen` is set to the length of the decompressed data. Returns `TINF_OK`
on success, and `TINF_DATA_ERROR` on error.

    int tinf_zlib_uncompress(void *dest,
                             unsigned int *destLen,
                             const void *source,
                             unsigned int sourceLen);

Decompress data in zlib compressed format from `source[]` to `dest[]`.
`destLen` is set to the length of the decompressed data. Returns `TINF_OK`
on success, and `TINF_DATA_ERROR` on error.

    unsigned int tinf_adler32(const void *data,
                              unsigned int length);

Computes the Adler-32 checksum of `length` bytes starting at `data`. Used by
`tinf_zlib_uncompress()`.

    unsigned int tinf_crc32(const void *data,
                            unsigned int length);

Computes the CRC32 checksum of `length` bytes starting at `data`. Used by
`tinf_gzip_uncompress()`.


Source Code
-----------

The source code is ANSI C, and assumes that int is 32-bit. It has been
tested on the x86 platform under Windows and Linux.

The decompression functions should be endian-neutral, and also reentrant
and thread-safe (not tested).

In src/nasm there are 32-bit x86 assembler (386+) versions of some of the
files.

Makefiles (GNU Make style) for a number of compilers are included.


Frequently Asked Questions
--------------------------

Q: Is it really free? Can I use it in my commercial ExpenZip software?

A: It's open-source software, available under the zlib license (see
   later), which means you can use it for free -- even in commercial
   products. If you do, please be kind enough to add an acknowledgement.

Q: Did you just strip stuff from the zlib source to make it smaller?

A: No, tinf was written from scratch, using the RFC documentation of
   the formats it supports.

Q: What do you mean by: 'the zlib library .. is more secure'?

A: The zlib decompression code checks the compressed data for validity
   while decompressing, so even on corrupted data it will not crash.
   The tinf code assumes it is given valid compressed data.

Q: I'm a Delphi programmer, can I use tinf?

A: Sure, the object files produced by both Borland C and Watcom C should
   be linkable with Delphi.

Q: Will tinf work on UltraSTRANGE machines running WhackOS?

A: I have no idea .. please try it out and let me know!

Q: Why are all the makefiles in GNU Make style?

A: I'm used to GNU Make, and it has a number of features that are missing
   in some of the other Make utilities.

Q: This is the first release, how can there be frequently asked questions?

A: Ok, ok .. I made the questions up ;-)


License
-------

tinf - tiny inflate library

Copyright (c) 2003 Joergen Ibsen

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
   not claim that you wrote the original software. If you use this
   software in a product, an acknowledgment in the product
   documentation would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must
   not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
   distribution.
