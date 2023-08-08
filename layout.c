#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "base64.h"

int parse_commandline(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Expected 1 argument, found %d\n", argc);
        return 1;
    }

    return 0;
}

int main(int argc, char **argv) {
    if (parse_commandline(argc, argv)) {
        return 1;
    }

    const size_t len = strlen(argv[1]);
    const size_t out_len = encode_len(len);
    printf("len = %d, out_len = %d, %%3 = %d, %%4 = %d\n", len, out_len, len%3, out_len%4);
    char *out = calloc(out_len+1, sizeof(char));
    out[out_len] = '\0';
    base64_encode(len, argv[1], out);
    if (out[out_len]) {
        fputs("out over-written\n", stderr);
        return 1;
    }
    puts(out);
    putchar('\n');

    if (len != decode_len(out_len, out)) {
        fprintf(stderr, "Bad decode_len=%d vs. %d\n", decode_len(out_len, out), len);
        return 1;
    }

    char *x = argv[1];
    while (*x) *x++ = 'Z';

    base64_decode(out_len, out, argv[1]);
    if (argv[1][len]) {
        fputs("argv over-written\n", stderr);
        return 1;
    }
    puts(argv[1]);
    putchar('\n');

    free(out);

    return 0;
}
