#ifndef __picoprintf_h_INCLUDED__
#define __picoprintf_h_INCLUDED__

#include <stddef.h>  // size_t
#include <stdarg.h>  // va_*



int pico_vsnprintf(char *pDest, size_t cbDest, const char *pFormat, va_list vl);
int pico_snprintf(char *pDest, size_t cbDest, const char *pFormat, ...);



//
// IMPORTANT!!!
//  uncomment the following lines corresponding to the features you need
//
// #define PICOFORMAT_HANDLE_FILL          // uncomment this line to handle "%6i" and "%04d" -- the fill with zeroes or spaces
// #define PICOFORMAT_HANDLE_FORCEDSIGN    // uncomment this line to handle "%+d" -- the forced sign placement
// #define PICOFORMAT_HANDLE_BIN           // uncomment this line to handle "%b" -- binary representation
// #define PICOFORMAT_HANDLE_OCT           // uncomment this line to handle "%o"
// #define PICOFORMAT_HANDLE_HEX           // uncomment this line to handle "%x" and "%X"
// #define PICOFORMAT_HANDLE_FLOATS        // uncomment this line to handle the "%f"
// #define PICOFORMAT_HANDLE_WCHAR_T       // not implemented yet. MS Visual Studio compiler uses "%S" to render strings of opposite bittness (8-bit vs. 16-bit)



// by default, the debug builds (determined by `#define _DEBUG`) will real-time print errors when a feature is used that is not enabled above
#ifndef FORMAT_ERROR_DELEGATE
    #ifdef _DEBUG
        #define FORMAT_ERROR_DELEGATE(__message, __arg) printf(__message, __arg); printf("\n");
    #else // _DEBUG
        #define FORMAT_ERROR_DELEGATE(__message, __arg)
    #endif // _DEBUG
#endif // FORMAT_ERROR_DELEGATE

#endif // __picoprintf_h_INCLUDED__
