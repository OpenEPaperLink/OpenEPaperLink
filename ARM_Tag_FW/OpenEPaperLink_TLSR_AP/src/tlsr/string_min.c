// The C library functions the firmware and the linked SDK code use. Replaces proj/common/string.c of
// the SDK: its memcpy()/memset() call is_ev_buf(), which links the SDK's event-buffer pools (about
// 3.7 KB of RAM this firmware never uses). Everything here is the plain C library behaviour;
// memcpy() copies a word at a time where it can, the rest is byte by byte.

#include <stddef.h>
#include <stdint.h>

// A word the compiler must assume may alias anything. Reading a byte buffer through a uint32_t* is
// an aliasing violation, and -fstrict-aliasing is on at -O2: without may_alias the compiler may
// reorder the byte writes before the loop and the word writes inside it. GCC 4.5 has the attribute.
typedef uint32_t aliasWord __attribute__((may_alias));

// Byte at a time costs about 9 cycles a byte on the TC32, a word at a time about 2.25. The TC32 has
// no unaligned word access, so both sides must start at the same residue mod 4 ((d ^ s) & 3). Below
// 16 bytes the run-in and tail cost more than the words save, so short copies stay byte-wise.
void *memcpy(void *out, const void *in, size_t n) {
    unsigned char *d = out;
    const unsigned char *s = in;
    if (n >= 16 && ((((uintptr_t)d) ^ ((uintptr_t)s)) & 3u) == 0u) {
        while (((uintptr_t)d & 3u) != 0u) {  // at most three bytes, and then both sides are aligned
            *d++ = *s++;
            n--;
        }
        aliasWord *dw = (aliasWord *)d;
        const aliasWord *sw = (const aliasWord *)s;
        size_t words = n >> 2;
        n &= 3u;
        while (words--) *dw++ = *sw++;
        d = (unsigned char *)dw;
        s = (const unsigned char *)sw;
    }
    while (n--) *d++ = *s++;
    return out;
}

void *memmove(void *dest, const void *src, size_t n) {
    unsigned char *d = dest;
    const unsigned char *s = src;
    if (d < s) {
        while (n--) *d++ = *s++;
    } else {
        d += n;
        s += n;
        while (n--) *--d = *--s;
    }
    return dest;
}

void *memset(void *dest, int val, size_t n) {
    unsigned char *d = dest;
    while (n--) *d++ = (unsigned char)val;
    return dest;
}

int memcmp(const void *m1, const void *m2, size_t n) {
    const unsigned char *a = m1, *b = m2;
    while (n--) {
        if (*a != *b) return *a - *b;
        a++;
        b++;
    }
    return 0;
}

size_t strlen(const char *s) {
    size_t n = 0;
    while (s[n]) n++;
    return n;
}
