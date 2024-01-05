#ifndef __picomath_h_INCLUDED__
#define __picomath_h_INCLUDED__

// this file is created to provide an alternative to `atof()`,
// which causes inclusion of dependency tree of ~12 kB

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int pico_atoi(const char *pStr);
float pico_atof(const char* pStr);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __picomath_h_INCLUDED__
