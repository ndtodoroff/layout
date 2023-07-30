#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

int parse_commandline(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Expected 1 argument, found %d\n", argc);
        return 1;
    }

    return 0;
}


const unsigned int uncode_width = 8, encode_width = 6;
const size_t uncode_step = 3, encode_step = 4;

// POSIX standard says char is 8 bits
void base64_encode(size_t len, const char *data, char *out) {
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
    for (; i <= len - uncode_step; i += uncode_step, j += encode_step) {
        word  = ((uint32_t) data[i+2]) << 2*uncode_width;
        word |= ((uint32_t) data[i+1]) << uncode_width;
        word |= ((uint32_t) data[i  ]);

        out[j] = table[word & mask];
        word >>= encode_width;
        out[j+1] = table[word & mask];
        word >>= encode_width;
        out[j+2] = table[word & mask];
        word >>= encode_width;
        out[j+3] = table[word];
    }

    const size_t rem = len - i;

    if (rem == 0) return;

    word = 0;
    for (int k = rem - 1; 0 <= k; --k) {
        word |= ((uint32_t) data[i + k]) << k*uncode_width;
    }

    size_t k = 0;
    for (; k < rem+1; ++k) {
        out[j+k] = table[word & mask];
        word >>= encode_width;
    }
    for (; k < encode_step; ++k) {
        out[j+k] = pad;
    }
}

void base64_decode(size_t len, const char *encode, char *uncode) {
    const size_t bias = '+';
    const static char table[256] = {
    };

    const uint32_t mask = 0xff;
    uint32_t word;

    for (size_t i = 0, j = 0; i < len;
         i += encode_step, j += uncode_step)
    {
        word  = encode[i]   << 3*encode_width;
        word |= encode[i+1] << 2*encode_width;
        word |= encode[i+2] <<   encode_width;
        word |= encode[i+3];

        out[j + 2] = table[word & mask];
        word >>= decode_width;
        out[j + 1] = table[word & mask];
        word >>= decode_width;
        out[j] = table[word];
    }
}

size_t encode_len(size_t uncode_len) {
    return ((uncode_len-1)/3+1)*4
}
size_t uncode_len(size_t encode_len, const char *encode) {
    const size_t padding =
        (encode[encode_len-1] == '=') + (encode[encode_len-2] == '=');
    
    return encode_len/4*3 + padding;
}

int main(int argc, char *argv[]) {
    if (parse_commandline(argc, argv)) {
        return 1;
    }

    const size_t len = strlen(argv[1]);
    const size_t out_len = encode_len(len) + 1;
    char *out = calloc(out_len, sizeof(char));
    base64(strlen(argv[1]), argv[1], out);
    out[out_len-1] = '\0';
    puts(out);
    putchar('\n');

    free(out);

    return 0;
}
