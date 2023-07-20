;;
;; tinfzlib  -  tiny zlib uncompress
;;
;; Copyright (c) 2003 by Joergen Ibsen / Jibz
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

TINF_OK         equ  0
TINF_DATA_ERROR equ (-3)

cpu 386

bits 32

%include "nasmlcm.inc"

section lcmtext

lcmglobal tinf_zlib_uncompress,16

lcmexport tinf_zlib_uncompress,16

lcmextern tinf_uncompress,16
lcmextern tinf_adler32,8

; =============================================================

lcmlabel tinf_zlib_uncompress,16
    ; tinf_zlib_uncompress(void *dest,
    ;                      unsigned int *destLen,
    ;                      const void *source,
    ;                      unsigned int sourceLen)

    .slen$ equ 2*4 + 4 + 12
    .src$  equ 2*4 + 4 + 8
    .dlen$ equ 2*4 + 4 + 4
    .dst$  equ 2*4 + 4

    push   esi
    push   ebx

    mov    esi, [esp + .src$]  ; esi -> source

    ; -- get header bytes --

    movzx  eax, word [esi]    ; al = cmf, ah = flg,

    ; -- check format --

    ; check method is deflate
    ; if ((cmf & 0x0f) != 8) return TINF_DATA_ERROR;
    mov    cl, 0x0f
    and    cl, al
    cmp    cl, 8
    jne    short .return_error

    ; check window size is valid
    ; if ((cmf >> 4) > 7) return TINF_DATA_ERROR;
    mov    ch, al
    shr    ch, 4
    cmp    ch, cl             ; cl = 8 from above
    jae    short .return_error

    ; check there is no preset dictionary
    ; if (flg & 0x20) return TINF_DATA_ERROR;
    test   ah, 0x20
    jnz    short .return_error

    ; check checksum
    ; if ((256*cmf + flg) % 31) return TINF_DATA_ERROR;
    xchg   al, ah
    xor    edx, edx
    lea    ebx, [edx + 31]
    div    ebx
    test   edx, edx
    jnz    short .return_error

    ; -- get adler32 checksum --

    mov    ecx, [esp + .slen$] ; ecx =  sourceLen
    mov    ebx, [esi + ecx - 4]

  %ifdef BSWAP_OK
    bswap  ebx
  %else ; BSWAP_OK
    xchg   bl, bh
    rol    ebx, 16
    xchg   bl, bh
  %endif ; BSWAP_OK

    ; -- inflate --

    ; res = tinf_uncompress(dst, destLen, src + 2, sourceLen - 6);
    lea    eax, [ecx - 6]
    push   eax
    lea    eax, [esi + 2]
    push   eax
    push   dword [esp + 8 + .dlen$]
    push   dword [esp + 12 + .dst$]
    call   tinf_uncompress
    add    esp, byte 16

    ; if (res != TINF_OK) return TINF_DATA_ERROR;
    test   eax, eax
    jnz    short .return_error

    ; -- check adler32 checksum --

    ; if (a32 != tinf_adler32(dst, *destLen)) return TINF_DATA_ERROR;
    mov    eax, [esp + .dlen$];
    push   dword [eax]
    push   dword [esp + 4 + .dst$]
    call   tinf_adler32
    add    esp, byte 8

    sub    eax, ebx
    jz     short .return_eax

  .return_error:
    mov    eax, TINF_DATA_ERROR

  .return_eax:
    pop    ebx
    pop    esi

    lcmret 16

; =============================================================

%ifdef _OBJ_
  section lcmdata
%endif

; =============================================================
