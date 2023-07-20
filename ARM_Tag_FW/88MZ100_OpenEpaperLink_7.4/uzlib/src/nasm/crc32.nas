;;
;; NASM assembler crc32
;;
;; Copyright (c) 1998-2003 by Joergen Ibsen / Jibz
;; All Rights Reserved
;;
;; http://www.ibsensoftware.com/
;;
;; This software is provided 'as-is', without any express
;; or implied warranty.  In no event will the authors be
;; held liable for any damages arising from the use of
;; this software.
;;
;; Permission is granted to anyone to use this software
;; for any purpose, including commercial applications,
;; and to alter it and redistribute it freely, subject to
;; the following restrictions:
;;
;; 1. The origin of this software must not be
;;    misrepresented; you must not claim that you
;;    wrote the original software. If you use this
;;    software in a product, an acknowledgment in
;;    the product documentation would be appreciated
;;    but is not required.
;;
;; 2. Altered source versions must be plainly marked
;;    as such, and must not be misrepresented as
;;    being the original software.
;;
;; 3. This notice may not be removed or altered from
;;    any source distribution.
;;

; CRC32 algorithm taken from the zlib source, which is
; Copyright (C) 1995-1998 Jean-loup Gailly and Mark Adler

cpu 386

bits 32

%include "nasmlcm.inc"

section lcmtext

lcmglobal tinf_crc32,8

lcmexport tinf_crc32,8

; =============================================================

lcmlabel tinf_crc32,8
    ; tinf_crc32(const void *data,
    ;            unsigned int length);

    .len$  equ 2*4 + 4 + 4
    .dat$  equ 2*4 + 4

    push   esi
    push   edi

    mov    esi, [esp + .dat$] ; esi -> buffer
    mov    ecx, [esp + .len$] ; ecx =  length

    sub    eax, eax           ; crc = 0

    test   esi, esi
    jz     short .c_exit

    test   ecx, ecx
    jz     short .c_exit

    dec    eax                ; crc = 0xffffffff

%ifdef _OBJ_
    mov    edi, tinf_crc32tab wrt FLAT ; edi -> crctab
%else
    mov    edi, tinf_crc32tab ; edi -> crctab
%endif

  .c_next_byte:
    xor    al, [esi]
    inc    esi

    mov    edx, 0x0f
    and    edx, eax

    shr    eax, 4

    xor    eax, [edi + edx*4]

    mov    edx, 0x0f
    and    edx, eax

    shr    eax, 4

    xor    eax, [edi + edx*4]

    dec    ecx
    jnz    short .c_next_byte

    not    eax

  .c_exit:
    pop    edi
    pop    esi

    lcmret 8

; =============================================================

section lcmdata

tinf_crc32tab dd 0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac, 0x76dc4190
              dd 0x6b6b51f4, 0x4db26158, 0x5005713c, 0xedb88320, 0xf00f9344
              dd 0xd6d6a3e8, 0xcb61b38c, 0x9b64c2b0, 0x86d3d2d4, 0xa00ae278
              dd 0xbdbdf21c

; =============================================================
