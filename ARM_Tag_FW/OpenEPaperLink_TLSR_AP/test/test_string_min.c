// Tests of src/tlsr/string_min.c on a PC: make -C test
//
// The file under test defines memcpy(), memmove(), memset(), memcmp() and strlen(), so the Makefile
// renames them with -D before including it here; every result is checked against the host's own C
// library and against a reference copy written the slowest possible way. memcpy() is the one with
// something to get wrong: it copies a word at a time when source and destination share their
// residue mod 4 and there are at least 16 bytes, so the cases that matter are every combination of
// the two alignments, every length around the threshold and around a multiple of four, and the
// bytes just outside the destination, which must not be touched.
//
// Built under AddressSanitizer and UBSan with -fstrict-aliasing, which is the point: reading a byte
// buffer through a uint32_t* is what the real compiler is being asked to do at -O2.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define memcpy smMemcpy
#define memmove smMemmove
#define memset smMemset
#define memcmp smMemcmp
#define strlen smStrlen
#include "../src/tlsr/string_min.c"
#undef memcpy
#undef memmove
#undef memset
#undef memcmp
#undef strlen

void *smMemcpy(void *out, const void *in, size_t n);
void *smMemmove(void *dest, const void *src, size_t n);
void *smMemset(void *dest, int val, size_t n);
int smMemcmp(const void *m1, const void *m2, size_t n);
size_t smStrlen(const char *s);

static int failures = 0;
static int checks = 0;

static void fail(const char *what, size_t da, size_t sa, size_t n) {
    if (failures < 20) printf("FAIL %s: dstAlign %zu srcAlign %zu n %zu\n", what, da, sa, n);
    failures++;
}

#define PAD 8
#define BUFMAX 600

// Fills a buffer with a pattern that makes a one-byte slip visible wherever it happens
static void pattern(unsigned char *p, size_t n, unsigned seed) {
    for (size_t i = 0; i < n; i++) p[i] = (unsigned char)(seed * 31u + i * 7u + (i >> 3));
}

// --- memcpy -------------------------------------------------------------------------------------
// Every alignment pair, with guard bytes either side of the destination.
static void testMemcpy(void) {
    static unsigned char src[BUFMAX + 2 * PAD + 8];
    static unsigned char dst[BUFMAX + 2 * PAD + 8];
    static unsigned char want[BUFMAX + 2 * PAD + 8];

    for (size_t da = 0; da < 8; da++) {
        for (size_t sa = 0; sa < 8; sa++) {
            for (size_t n = 0; n <= 80; n++) {
                pattern(src, sizeof(src), (unsigned)(da * 13 + sa));
                memset(dst, 0xA5, sizeof(dst));
                memcpy(want, dst, sizeof(dst));
                unsigned char *d = dst + PAD + da;
                const unsigned char *s = src + PAD + sa;
                memcpy(want + PAD + da, s, n);  // the host's memcpy is the reference
                void *r = smMemcpy(d, s, n);
                checks++;
                if (r != d) fail("memcpy return value", da, sa, n);
                if (memcmp(dst, want, sizeof(dst)) != 0) fail("memcpy", da, sa, n);
            }
        }
    }
    // longer blocks, including the 99-byte block part and the 127-byte frame of the radio
    static const size_t lens[] = {13, 14, 15, 16, 17, 18, 19, 20, 31, 32, 33, 63, 64, 65, 98, 99,
                                  100, 127, 128, 129, 254, 255, 256, 257, 511, 512, 513, 599};
    for (size_t da = 0; da < 4; da++) {
        for (size_t sa = 0; sa < 4; sa++) {
            for (size_t i = 0; i < sizeof(lens) / sizeof(lens[0]); i++) {
                const size_t n = lens[i];
                pattern(src, sizeof(src), (unsigned)(da * 101 + sa * 7 + n));
                memset(dst, 0x5A, sizeof(dst));
                memcpy(want, dst, sizeof(dst));
                unsigned char *d = dst + PAD + da;
                const unsigned char *s = src + PAD + sa;
                memcpy(want + PAD + da, s, n);
                smMemcpy(d, s, n);
                checks++;
                if (memcmp(dst, want, sizeof(dst)) != 0) fail("memcpy long", da, sa, n);
            }
        }
    }
    // the same buffer as source and destination: a copy onto itself must leave it alone
    for (size_t n = 0; n <= 64; n++) {
        pattern(src, sizeof(src), 3u);
        memcpy(want, src, sizeof(src));
        smMemcpy(src + PAD, src + PAD, n);
        checks++;
        if (memcmp(src, want, sizeof(src)) != 0) fail("memcpy self", 0, 0, n);
    }
}

// --- memmove ------------------------------------------------------------------------------------
// Overlapping in both directions, at every offset through the 16-byte threshold and past it.
static void testMemmove(void) {
    static unsigned char buf[600], want[600];
    for (size_t n = 0; n <= 70; n++) {
        for (size_t gap = 0; gap <= 20; gap++) {
            for (int forward = 0; forward < 2; forward++) {
                const size_t base = 100;
                const size_t dOff = forward ? base : base + gap;
                const size_t sOff = forward ? base + gap : base;
                pattern(buf, sizeof(buf), (unsigned)(n * 3 + gap));
                memcpy(want, buf, sizeof(buf));
                memmove(want + dOff, want + sOff, n);  // the host's memmove is the reference
                void *r = smMemmove(buf + dOff, buf + sOff, n);
                checks++;
                if (r != buf + dOff) fail("memmove return value", dOff, sOff, n);
                if (memcmp(buf, want, sizeof(buf)) != 0) fail(forward ? "memmove down" : "memmove up", dOff, sOff, n);
            }
        }
    }
    // no overlap at all, both directions, long enough that a word copy would be tempting
    for (size_t n = 0; n <= 200; n += 7) {
        for (int forward = 0; forward < 2; forward++) {
            const size_t dOff = forward ? 10 : 310;
            const size_t sOff = forward ? 310 : 10;
            pattern(buf, sizeof(buf), (unsigned)n);
            memcpy(want, buf, sizeof(buf));
            memmove(want + dOff, want + sOff, n);
            smMemmove(buf + dOff, buf + sOff, n);
            checks++;
            if (memcmp(buf, want, sizeof(buf)) != 0) fail("memmove apart", dOff, sOff, n);
        }
    }
}

// --- memset, memcmp, strlen ---------------------------------------------------------------------
static void testRest(void) {
    static unsigned char buf[200], want[200];
    for (size_t a = 0; a < 8; a++) {
        for (size_t n = 0; n <= 70; n++) {
            memset(buf, 0x11, sizeof(buf));
            memcpy(want, buf, sizeof(buf));
            memset(want + 10 + a, 0xC3, n);
            void *r = smMemset(buf + 10 + a, 0xC3, n);
            checks++;
            if (r != buf + 10 + a) fail("memset return value", a, 0, n);
            if (memcmp(buf, want, sizeof(buf)) != 0) fail("memset", a, 0, n);
            // the value is truncated to a byte, as the C library does
            memset(buf, 0, sizeof(buf));
            smMemset(buf, 0x1C3, n);
            for (size_t i = 0; i < n; i++) {
                if (buf[i] != 0xC3) fail("memset truncation", a, 0, n);
            }
        }
    }

    for (size_t n = 0; n <= 40; n++) {
        for (size_t at = 0; at < n; at++) {
            unsigned char a[64], b[64];
            pattern(a, sizeof(a), 5u);
            memcpy(b, a, sizeof(b));
            b[at] = (unsigned char)(a[at] ^ 0x80);
            const int ref = memcmp(a, b, n);
            const int got = smMemcmp(a, b, n);
            checks++;
            if ((ref < 0) != (got < 0) || (ref > 0) != (got > 0) || (ref == 0) != (got == 0)) {
                fail("memcmp sign", at, 0, n);
            }
            if (smMemcmp(a, a, n) != 0) fail("memcmp equal", at, 0, n);
        }
    }

    const char *strs[] = {"", "a", "ab", "abc", "0123456789", "0123456789abcdefghij"};
    for (size_t i = 0; i < sizeof(strs) / sizeof(strs[0]); i++) {
        checks++;
        if (smStrlen(strs[i]) != strlen(strs[i])) fail("strlen", i, 0, 0);
    }
}

int main(void) {
    testMemcpy();
    testMemmove();
    testRest();
    printf("string_min: %d checks, %d failures\n", checks, failures);
    return failures != 0;
}
