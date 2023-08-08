#ifndef BASE64_H
#define BASE64_H

#include <stdint.h>
#include <stddef.h>

int base64_encode(size_t, const uint8_t *, char *);
int base64_decode(size_t, const char *, uint8_t *);
size_t base64_encode_len(size_t);
size_t base64_decode_len(size_t, const char *);

#endif // BASE64_H
