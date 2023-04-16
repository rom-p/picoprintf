#include "picoprintf.h"

#include <stdbool.h>
#include <stdio.h>  // printf() for messages and snprintf() for comparison
#include <string.h> // strcmp()
#include <stdlib.h> // rand() and srand()
#include <time.h>   // time
#include <math.h>   // INFINITY, NaN, etc.

// TODO: add the following failure or near-failure tests:
// * too small of a buffer
// * incorrect formats, like "%4. "
// * non-existing formats, like "%q"


char pFullBuf[0x200];
char pPicoBuf[0x200];

bool g_verbose = false;


#define RUN_TEST(format, ...) \
    snprintf(pFullBuf, 0x200, format, __VA_ARGS__); \
    pico_snprintf(pPicoBuf, 0x200, format, __VA_ARGS__); \
    failed = strcmp(pFullBuf, pPicoBuf); \
    if (g_verbose || failed) { \
        printf("picoprintf %s  \"%s\", -- %s, %s\n", failed ? "FAILED" : "passed", format, pFullBuf, pPicoBuf); \
    } \
    if (failed) { \
        picofailures++; \
    } else { \
        picopasses++; \
    }


#ifdef __RUN_COMPARISON_TESTS__
// comparison tests require cloning the following projects into the same directory
#include "mpaland.h"
#include "tinyprintf.h"
#include "nanoprintf.h"
// mpaland's printf requires this function:
void _putchar(char character) {}
#endif //__RUN_COMPARISON_TESTS__


const char* g_pIntegerFormats[] = {
    "%d", "%i",
#ifdef PICOFORMAT_HANDLE_OCT
    "%o",
#endif // PICOFORMAT_HANDLE_OCT
#ifdef PICOFORMAT_HANDLE_HEX
    "%x", "%X",
#endif // PICOFORMAT_HANDLE_HEX
#ifdef PICOFORMAT_HANDLE_FILL
    "%6i", "%05d",
#ifdef PICOFORMAT_HANDLE_OCT
    "%10d", "%3o",
#endif // PICOFORMAT_HANDLE_OCT
#ifdef PICOFORMAT_HANDLE_HEX
    "%8x", "%08x", "0x%08x",
#endif // PICOFORMAT_HANDLE_HEX
#endif // PICOFORMAT_HANDLE_FILL
#ifdef PICOFORMAT_HANDLE_FORCEDSIGN
    "%+d",
#endif // PICOFORMAT_HANDLE_FORCEDSIGN

    NULL  // keep it last
};

int g_testIntegers[] = {
    0, 1, -1, 17, -17, 44, -44, 126, -126, 127, -127, 128, -128, 255, -255, 256, -256, 999, -999,
    1000, -1000, 65535, -65535, 65536, -65536, 1000 * 1000, -1000 * 1000,
    (int)((long long)(2) * 1024 * 1024 * 1024 - 1), -(int)((long long)(2) * 1024 * 1024 * 1024 - 1)
};


const char* g_pFloatFormats[] = {
    "%f", "%3.2f", "%F", "text %.2f here",
    NULL  // keep it last
};

float g_testFloats[] = {
    0, 1, -1, 3.14, -M_PI, 13.07, 1000 * 1000 * 42 + 0.582, INFINITY, -INFINITY, NAN, -NAN
};


int main(int argc, const char ** argv) {
    unsigned picopasses = 0, picofailures = 0;
    unsigned mpalandpasses = 0, mpalandfailures = 0;
    int failed;
    // RUN_TEST("hello, world!");  // can't do that: limitation of RUN_TEST variadic expansion
    RUN_TEST("%s", "");
    RUN_TEST("%s", "x");
    RUN_TEST("%s, world!", "hello");
    RUN_TEST("%s, %s!", "hello", "world");
    RUN_TEST("%s%c %s%c", "hello", ',', "world", '!');

    srand((unsigned)time(NULL));

    for (const char **ppFormat = g_pIntegerFormats; NULL != *ppFormat; ppFormat++) {
        for (size_t ii = 0; ii < sizeof(g_testIntegers) / sizeof(g_testIntegers[0]); ii++) {
            RUN_TEST(*ppFormat, g_testIntegers[ii]);
        }
        for (size_t ii = 0; ii < 20; ii++) {
            int val = (rand() % 2 ? 1 : -1 ) * rand();
            RUN_TEST(*ppFormat, val);
        }
    }

#ifdef PICOFORMAT_HANDLE_FLOATS
    for (const char **ppFormat = g_pFloatFormats; NULL != *ppFormat; ppFormat++) {
        for (size_t ii = 0; ii < sizeof(g_testFloats) / sizeof(g_testFloats[0]); ii++) {
            RUN_TEST(*ppFormat, g_testFloats[ii]);
        }
        for (size_t ii = 0; ii < 20; ii++) {
            float val = (rand() % 2 ? 1 : -1 ) * rand() + 1.f / rand();
            RUN_TEST(*ppFormat, val);
        }
    }
#endif // PICOFORMAT_HANDLE_FLOATS

#ifdef PICOFORMAT_HANDLE_WCHAR_T
    RUN_TEST("%S", "should fail");
#endif // PICOFORMAT_HANDLE_WCHAR_T

    RUN_TEST("%s", "last one");  // keep it last to detect buffer overruns

    printf("\n\n Passed: %u\n Failed: %u\n\n", picopasses, picofailures);

#ifdef __RUN_COMPARISON_TESTS__
    // calling them in order to force linking in order to measure the footprint
    snprintf_(pPicoBuf, 0x200, "I'm mpaland's printf\n");
    tfp_snprintf(pPicoBuf, 0x200, "I'm tiny printf\n");
    npf_snprintf(pPicoBuf, 0x200, "I'm nanoprintf\n");
#endif // __RUN_COMPARISON_TESTS__

    return picofailures;
}
