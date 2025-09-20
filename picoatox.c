#include "picoatox.h"
#include <math.h> // pow()


// auto-detects base (if not forced) using c/c++ rules
int pico_atoi(const char *pStr, int force_base) {
    long long result = 0;
    int base = 10;
    int sign = 1;

    if ('-' == *pStr) {
        sign = -1;
        pStr++;
    }
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
    return sign * result;
}


float pico_atof(const char* pStr) {
    bool still_valid = true;
    bool seen_period = false;
    bool seen_number = false;
    bool seen_e = false;
    bool negative = false;
    bool negative_exponent = false;

    int whole = 0;
    int decimal = 0;
    int decimal_digits = 0;
    int exponent = 0;

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
                    decimal = decimal * 10 + *pCur - '0';
                    decimal_digits++;
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
