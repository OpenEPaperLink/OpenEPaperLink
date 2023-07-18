/*
 * Copyright (c) 2018 Martin Storsjo
 *
 * This file is part of llvm-mingw.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include <math.h>

jmp_buf jmp;

int fibonacci(int val) {
    char buf[100];
    if (val <= 1) {
        longjmp(jmp, 1);
        return val;
    }
    snprintf(buf, sizeof(buf), "fibonacci(%d)", val);
    printf("%s\n", buf);
    return fibonacci(val - 1) + fibonacci(val - 2);
}

double static_sin, static_cos;

int main(int argc, char* argv[]) {
    int val = 10, ret;
    double val2 = 3.14;
    if (argc > 1)
        val = atoi(argv[1]);
    if (argc > 2)
        val2 = atof(argv[2]);
    double local_sin = sin(val2);
    printf("sin = %f\n", local_sin);
    printf("cos = %f\n", cos(val2));
    printf("size = %d, %p\n", (int) sizeof(jmp), jmp);
    static_sin = local_sin;
    static_cos = cos(val2);
    if ((ret = setjmp(jmp)) != 0) {
        printf("setjmp returned %d\n", ret);
        printf("sin = %f\n", local_sin);
        printf("cos = %f\n", cos(val2));
        double epsilon = 0.000001;
        if (fabs(static_sin - local_sin) > epsilon || fabs(static_cos - cos(val2)) > epsilon) {
            printf("check d = %f delta = %f\n", static_sin, static_sin - local_sin);
            printf("check cos = %f delta = %f\n", static_cos, static_cos - cos(val2));
            printf("local variables were clobbered\n");
            return 1;
        }
        return 0;
    }
    printf("fibonacci(%d) = %d\n", val, fibonacci(val));
    return 1;
}
