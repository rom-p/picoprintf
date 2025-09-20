#ifndef __picoatox_h_INCLUDED__
#define __picoatox_h_INCLUDED__

// this file provides an alternative to `atof()`,
// which causes inclusion of dependency tree of ~12 kB

#include "picobool.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifdef __cplusplus
    int pico_atoi(const char *pStr, int force_base = 0);
#else
    int pico_atoi(const char *pStr, int force_base);
#endif
float pico_atof(const char *pStr);

bool pico_isboolean(const char *pStr);
bool pico_atob(const char *pStr);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __picoatox_h_INCLUDED__
