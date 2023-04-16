#include "picoprintf.h"

#include <stdlib.h>  // abs()
#include <math.h>    // fabs()


static void flip(char *pLeft, char *pRight) {
    pRight--;
    while (pLeft < pRight) {
        char tmp = *pLeft;
        *pLeft = *pRight;
        *pRight = tmp;
        pLeft++;
        pRight--;
    }
}

// using #define over `inline` for enabling porting to old C
#if !defined(MIN)
#define MIN(left, right) (((left) < (right)) ? (left) : (right))
#endif
#if !defined(MAX)
#define MAX(left, right) (((left) > (right)) ? (left) : (right))
#endif

// returns the pointer to the null-terminating character of the filled string
int pico_vsnprintf(char *pDest, size_t cbDest, const char *pFormat, va_list vl) {
    char *pStart = pDest;
    char *pEnd = pDest + cbDest - 1;  // `pEnd` is the last _writable_ character of the string
                                      // `pDest` will never point past `pEnd`, meaning it will always point to writable chars
    for (char *pEnd = pDest + cbDest - 1; *pFormat && pDest < pEnd; ) {
        if (*pFormat != '%') {
            *pDest++ = *pFormat++;
        } else {                            // format starts here
            pFormat++;                      // skipping the '%'
            if (*pFormat == '%') {          // unless it's indeed a '%'
                *pDest++ = *pFormat++;
            } else {                        // first, collect the format
                char format = '\0';
                bool force_sign = false;
                bool fill_zeros = false;
                int bits_per_digit = 0;     // valid in 'b', 'o', 'p' and 'x'/'X' modes only
                int whole_chars = 0;
                int decimal_chars = -1;     // if not specified, %f are rendered with 6, while %g are rendered with 0
                bool seen_period = false;
                bool seen_numbers = false;
                bool treat_as_unsigned = false;
                bool treat_as_long = false;
                bool render_in_lowercase = false;

                for (; *pFormat && '\0' == format; pFormat++) {
                    switch (*pFormat) {
                #ifdef PICOFORMAT_HANDLE_FORCEDSIGN
                    case '+':
                        force_sign = true;
                        break;
                #endif // PICOFORMAT_HANDLE_FORCEDSIGN
                    case '0':
                        if (!seen_numbers) {
                            fill_zeros = true;
                            break;
                        }
                        // fall through
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        *(seen_period ? &decimal_chars : &whole_chars ) *= 10;
                        *(seen_period ? &decimal_chars : &whole_chars) += *pFormat - '0';
                        seen_numbers = true;
                        break;
                    case '.':
                        seen_numbers = seen_period = true;
                        decimal_chars = 0;
                        break;
                    case 'l':    // long modifier
                        treat_as_long = true;
                    case 'u':    // unsigned modifier
                        treat_as_unsigned = true;
                        break;
                #ifdef PICOFORMAT_HANDLE_WCHAR_T
                    case 'C':    // string of opposite encoding char (i.e., `wchar_t`s)
                        break;
                #endif // PICOFORMAT_HANDLE_WCHAR_T
                #ifdef PICOFORMAT_HANDLE_BIN
                    case 'b':
                        bits_per_digit = 0;
                        treat_as_unsigned = true;
                        format = 'b';
                        break;
                #endif // PICOFORMAT_HANDLE_BIN
                #ifdef PICOFORMAT_HANDLE_OCT
                    case 'o':    // octal integer
                        bits_per_digit = 3;
                        treat_as_unsigned = true;
                        format = 'b';
                        break;
                #endif // PICOFORMAT_HANDLE_OCT
                #ifdef PICOFORMAT_HANDLE_HEX
                    case 'x':    // hexadecimal integer
                    case 'p':    // pointer
                        bits_per_digit = 4;
                        treat_as_unsigned = true;
                        render_in_lowercase = true;
                        format = 'b';
                        break;
                    case 'X':    // hexadecimal integer, uppercase
                        bits_per_digit = 4;
                        treat_as_unsigned = true;
                        format = 'b';
                        break;
                #endif // PICOFORMAT_HANDLE_HEX
                #ifdef PICOFORMAT_HANDLE_FLOATS
                    case 'a':
                #ifdef PICOFORMAT_HANDLE_EXPONENTS
                    case 'e':   // floating point, exponent format
                #endif // PICOFORMAT_HANDLE_EXPONENTS
                    case 'f':
                    case 'g':
                        render_in_lowercase = true;
                        // fall through
                    case 'F': 
                #endif
                #ifdef PICOFORMAT_HANDLE_WCHAR_T
                    case 'S':    // string of opposite encoding chars (i.e., `wchar_t`s)
                #endif // PICOFORMAT_HANDLE_WCHAR_T
                    case 'c':  // single char: always supported
                    case 'd':  // integer: always supported
                    case 'i':  // integer: always supported
                    case 's':  // string of native `char`s: always supported
                        format = *pFormat;
                        break;
                    default:
                        FORMAT_ERROR_DELEGATE("detected unhandled format specifier: %c", *pFormat);
                        break;
                    }
                }

                // format is parsed, now render the value
                char *pParamStarts = pDest;
                switch (format) {
                case 'c':           // single char
                    *pDest++ = va_arg(vl, int) & 0xff;
                    break;
                case 's':           // null-terminated string
                    for (const char *pStr = va_arg(vl, const char*); *pStr && pDest < pEnd; ) {
                        *pDest++ = *pStr++;
                    }
                    break;
            #if defined(PICOFORMAT_HANDLE_HEX) || defined(PICOFORMAT_HANDLE_OCT) || defined(PICOFORMAT_HANDLE_BIN)
                case 'b': {          // binary, oct, or hex integer, always unsigned
                        long long int val = 0;
                        if (treat_as_long) {
                            val = va_arg(vl, unsigned long long int);
                        } else {
                            val = va_arg(vl, unsigned);
                        }
                        unsigned mask = bits_per_digit == 4 ? 0x0f : bits_per_digit == 3 ? 0x07 : 0x01;
                #if defined(PICOFORMAT_HANDLE_HEX)
                        const char* chars = render_in_lowercase ? "0123456789abcdef" : "0123456789ABCDEF";
                #elif defined(PICOFORMAT_HANDLE_OCT)
                        const char* chars = "01234567";
                #else
                        const char* chars = "01";
                #endif // individual non-decimal formats
                        while (pDest < pEnd
                            && (pDest == pParamStarts
                             || val
                             || pDest - pParamStarts < whole_chars)) {
                            char ch;
                            if (pDest == pParamStarts || 0 != val) { // if first char or there's still meaningful digits
                                ch = chars[val & mask];
                                val >>= bits_per_digit;
                            } else {
                                ch = fill_zeros ? '0' : ' ';
                            }
                            *pDest++ = ch;
                        }
                        flip(pParamStarts, pDest);
                    }
                    break;
            #endif // defined(PICOFORMAT_HANDLE_BIN) || defined(PICOFORMAT_HANDLE_OCT) || defined(PICOFORMAT_HANDLE_HEX)
                case 'd':           // decimal integer
                case 'i': {
                        long long int val = 0;
                        if (treat_as_long) {
                            if (treat_as_unsigned) {
                                val = va_arg(vl, unsigned long long int);
                            } else {
                                val = va_arg(vl, long long int);
                            }
                        } else {
                            if (treat_as_unsigned) {
                                val = va_arg(vl, unsigned);
                            } else {
                                val = va_arg(vl, int);
                            }
                        }
                        char chSign = '\0';
                        if ((force_sign || val < 0) && pDest < pEnd) {
                            chSign = val < 0 ? '-' : '+';
                            val = abs(val);
                            whole_chars--;
                        }
                        while (pDest < pEnd
                            && (pDest == pParamStarts
                             || val
                             || pDest - pParamStarts < whole_chars)) {
                            char ch;
                            if (pDest == pParamStarts || 0 != val) {// if first digit (i.e. zero) or there's still non-zero digits to write
                                ch = val % 10 + '0';
                                val /= 10;
                            } else {
                                if ('\0' != chSign && !fill_zeros) {// write a sign if filling with spaces, not with zeros
                                    *pDest++ = chSign;
                                    whole_chars++;
                                    chSign = '\0';                  // prevent the sign from placing down twice
                                }
                        #ifdef PICOFORMAT_HANDLE_FILL
                                ch = fill_zeros ? '0' : ' ';
                        #else // PICOFORMAT_HANDLE_FILL
                                    break;
                        #endif // PICOFORMAT_HANDLE_FILL
                            }
                            *pDest++ = ch;
                        }
                        if ('\0' != chSign) { // write a sign if it wasn't written before
                            *pDest++ = chSign;
                        }
                        flip(pParamStarts, pDest);
                    }
                    break;
            #ifdef PICOFORMAT_HANDLE_FLOATS
                case 'f': case 'F': case 'g': {
                        double val = va_arg(vl, float);
                        if (force_sign || val < 0.f) {
                            *pDest++ = val < 0.f ? '-' : '+';
                            val = fabs(val);
                            pParamStarts = pDest;
                        }
                        if (val == INFINITY || val == -INFINITY || isnan(val)) {
                            for (const char *pszVal = isnan(val) ? render_in_lowercase ? "nan" : "NAN": render_in_lowercase ? "inf" : "INF"
                               ; pDest < pEnd && *pszVal
                               ; *pDest++ = *pszVal++);
                        } else {
                            if (decimal_chars == -1) {
                                if (format == 'g') {
                                    decimal_chars = 0;
                                } else {
                                    decimal_chars = 6;
                                }
                            }
                            whole_chars = MAX(0, whole_chars - decimal_chars);
                            whole_chars = MAX(whole_chars, pDest - pParamStarts + 1); // at least sign (if present) and first char
                            for (float digit = 1.f
                               ; pDest < pEnd && (val >= digit || pDest - pParamStarts < whole_chars)
                               ; digit *= 10.f) {
                                char ch = (int)(val / digit) % 10 + '0';
                                *pDest++ = ch;
                            }
                            flip(pParamStarts, pDest);
                            if (decimal_chars && pDest < pEnd) {
                                *pDest++ = '.';
                                val += .5 * pow(10, -decimal_chars); // compensating for rounding error
                                for (size_t digit = 0; pDest < pEnd && digit < decimal_chars; digit++) {
                                    val = (val - (int)(val)) * 10.f;
                                    *pDest++ = (int)(val) + '0';
                                }
                            }
                        }
                    }
                    break;
            #endif // PICOFORMAT_HANDLE_FLOATS
                }
            }
        }
    }
    *pDest = '\0';
    return pDest - pStart;
}


int pico_snprintf(char *pDest, size_t cbDest, const char *pFormat, ...) {
    va_list vl;
    va_start(vl, pFormat);
    int result = pico_vsnprintf(pDest, cbDest, pFormat, vl);
    va_end(vl);
    return result;
}
