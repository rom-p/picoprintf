#include "picoatox.h"
#include <math.h> // pow()

#define MAX_DECIMAL_DIGITS 9


// auto-detects base (if not forced) using c/c++ rules
unsigned pico_atou(const char *pStr, int force_base) {
    unsigned result = 0;
    int base = 10;

    if (0 != force_base) {
        base = force_base;
    } else if ('0' == *pStr) { // bin, oct, or hex
        pStr++;
        switch (*pStr) {
        case 'b': case 'B':
            base = 2;
            pStr++;
            break;
        case 'x': case 'X':
            base = 16;
            pStr++;
            break;
        default:
            base = 8;
            break;
        }
    }
    for (const char *pCur = pStr; *pCur; pCur++) {
        if ((base == 2 && *pCur >= '0' && *pCur <= '1')
         || (base == 8 && *pCur >= '0' && *pCur <= '7')
         || (base == 10 && *pCur >= '0' && *pCur <= '9')
         || (base == 16 && ((*pCur >= '0' && *pCur <= '9') || (*pCur >= 'a' && *pCur <= 'f') || (*pCur >= 'A' && *pCur <= 'F')))) {
            if (base == 16 && *pCur > '9') {
                result = result * base + (*pCur & 0xdf) - 'A' + 10;
            } else {
                result = result * base + *pCur - '0';
            }
        } else {
            break;
        }
    }
    return result;
}


// auto-detects base (if not forced) using c/c++ rules
int pico_atoi(const char *pStr, int force_base) {
    int sign = 1;

    if ('-' == *pStr) {
        sign = -1;
        pStr++;
    }

    return sign * pico_atou(pStr, force_base);
}


float pico_atof(const char* pStr) {
    bool still_valid = true;
    bool seen_period = false;
    bool seen_number = false;
    bool seen_e = false;
    bool negative = false;
    bool negative_exponent = false;

    unsigned whole = 0;
    unsigned decimal = 0;
    unsigned decimal_digits = 0;
    unsigned exponent = 0;

    for (const char *pCur = pStr; still_valid && *pCur; pCur++) {
        switch (*pCur) {
        case '-':
            if (!seen_number && !seen_period) {
                if (seen_e) {
                    negative_exponent = true;
                } else {
                    negative = true;
                }
                seen_number = true;
            } else {
                still_valid = false;
                // unexpected '-' after numbers
            }
            break;
        case '.':
            seen_period = true;
            break;
        case 'e':
        case 'E':
            seen_e = true;
            seen_number = seen_period = false;
            break;
        default:
            if (*pCur >= '0' && *pCur <= '9') {
                if (seen_e) {
                    exponent = exponent * 10 + *pCur - '0';
                } else if (seen_period) {
                    if (decimal_digits < MAX_DECIMAL_DIGITS) { // still room in `decimal`
                        decimal = decimal * 10 + *pCur - '0';
                        decimal_digits++;
                    } else {  // `decimal` saturated: not accepting any more digits
                    }
                } else {
                    whole = whole * 10 + *pCur - '0';
                }
            } else {
                still_valid = false;
            }
            break;
        }
    }
    return (negative ? -1.f : 1.f) * (whole + decimal / pow(10.f, decimal_digits)) * pow(10.f, (negative_exponent ? -1.f: 1.f) * exponent);
}
