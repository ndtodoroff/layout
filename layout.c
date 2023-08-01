#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

int parse_commandline(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Expected 1 argument, found %d\n", argc);
        return 1;
    }

    return 0;
}

const unsigned int decode_width = 8, encode_width = 6;
const size_t decode_step = 3, encode_step = 4;

int base64_encode(size_t len, const uint8_t *data, char *out) {
    const static char table[64] = {
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
           'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
           'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
      '+', '/'
    };
    const char pad = '=';

    const uint32_t mask = 0x3f;
    uint32_t word;

    size_t i = 0, j = 0;
    for (; i + decode_step <= len; i += decode_step, j += encode_step) {
        word  = ((uint32_t) data[i])   << 2*decode_width;
        word |= ((uint32_t) data[i+1]) << decode_width;
        word |= ((uint32_t) data[i+2]);

        out[j+3] = table[word & mask];
        word >>= encode_width;
        out[j+2] = table[word & mask];
        word >>= encode_width;
        out[j+1] = table[word & mask];
        word >>= encode_width;
        out[j]   = table[word];
    }
    printf("i = %d, j = %d\n", i, j);

    const size_t rem = len - i;

    switch (rem) {
        case 0:
            break;
        case 1:
            word  = ((uint32_t) data[i])   << 2*decode_width;
            word >>= 2*encode_width;

            out[j+3] = pad;
            out[j+2] = pad;
            out[j+1] = table[word & mask];
            word >>= encode_width;
            out[j]   = table[word];

            break;
        case 2:
            word  = ((uint32_t) data[i])   << 2*decode_width;
            word |= ((uint32_t) data[i+1]) << decode_width;
            word >>= encode_width;

            out[j+3] = pad;
            out[j+2] = table[word & mask];
            word >>= encode_width;
            out[j+1] = table[word & mask];
            word >>= encode_width;
            out[j]   = table[word];

            break;
#ifdef DEBUG
        default:
            fprintf(stderr,
                "base64_encode: IMPOSSIBLE: bad remainder: %d\n",
                rem
            );
            return 1;
#endif
    }

    return 0;
}

int base64_decode(size_t len, const char *encode, uint8_t *decode) {
    const char pad = '=';
    const size_t bias = '+';
    const size_t max = 'z';
#ifdef DEBUG
    const char bad_value = 64;
#endif
    const static char table['z' - '+' + 1] = {
        62, 64, 64, 64, 63,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61,
        64, 64, 64,
         0,
        64, 64, 64,
         0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17,
            18, 19, 20, 21, 22, 23, 24, 25,
        64, 64, 64, 64, 64, 64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 
            38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
    };

    const uint32_t mask = 0xff;
#ifdef DEBUG
    char buf[5];
#endif
    size_t i = 0, j = 0;
    for (; i + encode_step < len; i += encode_step, j += decode_step) {
        uint32_t word;
#ifdef DEBUG
        for (size_t k = 0; k < 4; ++k) {
            if (encode[i+k] < bias || max < encode[i+k] ||
                table[encode[i+k]-bias] == bad_value)
            {
#ifdef DEBUG
                snprintf(buf, 5, encode + i);
                fprintf(stderr,
                    "base64_decode: bad encode \"%s\" at index %d\n",
                    buf, i
                );
                for (size_t k = 0; k < 4; ++k)
                    fprintf(stderr,
                        "bias = %d, enc = %c %3d, enc-bias = %d, max = %d, val = %d\n",
                        bias, encode[i+k], encode[i+k], encode[i+k]-bias, max,
                        table[encode[i+k]-bias]
                    );
#endif
                return 1;
            }
        }
#endif
        word  = table[encode[i]  -bias] << 3*encode_width;
        word |= table[encode[i+1]-bias] << 2*encode_width;
        word |= table[encode[i+2]-bias] <<   encode_width;
        word |= table[encode[i+3]-bias];

        decode[j + 2] = word & mask;
        word >>= decode_width;
        decode[j + 1] = word & mask;
        word >>= decode_width;
        decode[j]     = word & mask;

    }

    uint32_t word;
    word  = table[encode[i]  -bias] << 3*encode_width;
    word |= table[encode[i+1]-bias] << 2*encode_width;
    word |= table[encode[i+2]-bias] <<   encode_width;
    word |= table[encode[i+3]-bias];

    const int padding = (encode[len-1] == pad) + (encode[len-2] == pad);
    switch (padding) {
        case 2:
            word >>= 2*decode_width;
            decode[j]   = word & mask;
            break;
        case 1:
            word >>= decode_width;
            decode[j+1] = word & mask;
            word >>= decode_width;
            decode[j]   = word & mask;
            break;
        case 0:
            decode[j+2] = word & mask;
            word >>= decode_width;
            decode[j+1] = word & mask;
            word >>= decode_width;
            decode[j]   = word & mask;
            break;
#ifdef DEBUG
        default:
            fprintf(stderr,
                "base64_decode: IMPOSSIBLE: too much padding: %d\n",
                padding
            );
            return 2;
#endif
    }

    return 0;

}

size_t encode_len(size_t decode_len) {
    const size_t mod = decode_len%3;
    return ((decode_len - mod)/3 + !!mod)*4;
}
size_t decode_len(size_t encode_len, const char *encode) {
    const size_t padding =
        (encode[encode_len-1] == '=') + (encode[encode_len-2] == '=');

    return encode_len/4*3 - padding;
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
