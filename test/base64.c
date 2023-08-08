#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../base64.h"

#define TEST_BASE64_MAX_LEN 500
#define TEST_BASE64_REPS 100

int test_base64(void) {
    printf(
        "%s: TEST_BASE64_MAX_LEN = %d\n"
        "%s: TEST_BASE64_REPS = %d\n",
        __FILE__, TEST_BASE64_MAX_LEN,
        __FILE__, TEST_BASE64_REPS
    );

    int overall_error = 0;
    uint8_t *decode = calloc(TEST_BASE64_MAX_LEN+1, sizeof(uint8_t));
    uint8_t *new_decode = calloc(TEST_BASE64_MAX_LEN+1, sizeof(uint8_t));
    char *encode = calloc(base64_encode_len(TEST_BASE64_MAX_LEN)+1, sizeof(char));

    for (size_t i = 0; i < TEST_BASE64_MAX_LEN; ++i) {
        decode[i] = 'Z';
        new_decode[i] = 'Z';
    }
    for (size_t i = 0; i < base64_encode_len(TEST_BASE64_MAX_LEN); ++i) {
        encode[i] = 'Z';
    }

    for (int i = 0; i < TEST_BASE64_REPS; ++i) {
        int error = 0;
        size_t decode_len = rand() % TEST_BASE64_MAX_LEN;
        size_t encode_len = base64_encode_len(decode_len);

        for (size_t i = 0; i < decode_len; ++i) {
            decode[i] = rand();
            if (!decode[i]) decode[i] = 'Z';
        }
        decode[decode_len] = '\0';
        encode[encode_len] = '\0';

        base64_encode(decode_len, decode, encode);
        char enc_extra = encode[encode_len];
        encode[encode_len] = '\0';
        if (enc_extra) {
            printf("%s: FAILED: encode buffer overwritten\n", __FILE__);
            error |= 1;
        }

        base64_decode(encode_len, encode, new_decode);
        char dec_extra = decode[decode_len];
        new_decode[decode_len] = '\0';
        if (dec_extra) {
            printf("%s: FAILED: new_decode buffer overwritten\n", __FILE__);
            error |= 1;
        }

        if (strcmp(decode, new_decode)) {
            printf("%s: FAILED: bad decode\n", __FILE__);
            error |= 1;
        }

        size_t new_decode_len = base64_decode_len(encode_len, encode);
        if (new_decode_len != decode_len) {
            printf(
                "%s:%d: FAILED: base64_decode_len() return incorrect value: "
                "got %d wanted %d\n",
                __FILE__, __LINE__, new_decode_len, decode_len);
            error |= 1;
        }

        if (error) {
            printf(
                "%s: Offending raw data:\n%s\n%s: END DATA\n",
                __FILE__, decode, __FILE__
            );
            printf(
                "%s: Offending encoded data:\n%s\n%s: END DATA\n",
                __FILE__, encode, __FILE__
            );
            if (enc_extra)
                printf("%s: Extra char: %c\n", __FILE__, enc_extra);
            printf(
                "%s: Offending decoded data:\n%s\n%s: END DATA\n",
                __FILE__, new_decode, __FILE__
            );
            if (dec_extra)
                printf("%s: Extra char: %c\n", __FILE__, dec_extra);
        }

        overall_error |= error;
    }

    free(decode);
    free(new_decode);
    free(encode);

    return overall_error;
}
