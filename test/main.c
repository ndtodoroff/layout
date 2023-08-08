#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <libgen.h>

int test_base64(void);

#define NTESTS 1
int (*const tests[NTESTS])(void) = {
    test_base64,
};

int main(int argc, char **argv) {
    unsigned seed = time(NULL);
    printf("Random seed: %d\n", seed);
    srand(seed);

    int failed = 0;
    for (int i = 0; i < NTESTS; ++i) {
        failed |= (*(tests[i]))();
    }
    if (!failed)
        puts("All tests passed successfully\n");

    return failed;
}
