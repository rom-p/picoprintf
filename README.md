![Language: C](https://img.shields.io/badge/language-C-blue.svg)
![Platform: Embedded](https://img.shields.io/badge/platform-embedded-green.svg)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

**picoprintf** is a minimal, customizable implementation of `sprintf`, `snprintf`, and `vsnprintf` for embedded platforms.

* **Tiny footprint:** <1 kB in minimal configuration on most platforms
* **No dynamic memory allocation**
* **Highly customizable via compile-time macros**
* **Designed for resource-constrained and embedded systems**

# Supported Format Specifiers
* `%c` (char)
* `%s` (string); dynamic precision (e.g., `%.*s`) can be enabled with `PICOFORMAT_HANDLE_DYNAMIC_PRECISION`
* `%d`, `%i` (signed decimal integers), `%u` (unsigned decimal integer), and the `l` length modifier (e.g. `%ld`, `%lu`).  Fill (e.g., `%6d` or `%03i`) and forced signs (e.g., `%+i`) can be enabled at build time by defining `PICOFORMAT_HANDLE_FILL` and `PICOFORMAT_HANDLE_FORCEDSIGN`, respectively
* `%b` (binary) -- can be enabled at build time by defining `PICOFORMAT_HANDLE_BIN`
* `%o` (octal) -- can be enabled at build time by defining `PICOFORMAT_HANDLE_OCT`
* `%x`/`%X` (hexadecimal) -- can be enabled at build time by defining `PICOFORMAT_HANDLE_HEX`
* `%p` (pointer) -- included with `PICOFORMAT_HANDLE_HEX`
* `%f`/`%F`/`%e`/`%a` (floating-point) -- can be enabled at build time by defining `PICOFORMAT_HANDLE_FLOATS`

# Quick Start
1. **Copy files**: add `picoprintf.h`, `picobool.h`, and `picoprintf.c` to your project
2. **Include header**: add `#include "picoprintf.h"` in your source files
3. **Configure (optional)**: adjust the `#define`s in `picoprintf.h` to enable or disable features as needed
4. **Replace standard functions (optional)**: use `#define snprintf pico_snprintf` before including standard headers to override the standard function
5. **Build**: compile and link as usual

# Examples
## Basic Usage
```c
#include "picoprintf.h"

char buf[0x40];
pico_snprintf(buf, sizeof(buf), "Value: %d", 42);
// `buf` now contains "Value: 42"
```

## Advanced Usage with Optional Features
```c
// Uncomment features you need in `picoprinft.h`:
#define PICOFORMAT_HANDLE_HEX
#define PICOFORMAT_HANDLE_FILL
#define PICOFORMAT_HANDLE_FORCEDSIGN
```


```c
// In your code: include the header:
#include "picoprintf.h"

char buf[0x100];

// Hexadecimal with padding
pico_snprintf(buf, sizeof(buf), "Magic: 0x%08X  Answer: 0x%04X", 0xCAFE, 66);
// `buf`: "Magic: 0x0000CAFE  Answer: 0x0042"

// Decimal with forced sign and padding
pico_snprintf(buf, sizeof(buf), "Temperature: %+6d°C", 23);
// `buf`: "Temperature:    +23°C"

// Multiple format specifiers
pico_snprintf(buf, sizeof(buf), "User: %s, ID: %04d", "Alice", 42);
// `buf`: "User: Alice, ID: 0042"
```

## Replacing Standard Library Functions
```c
#define snprintf pico_snprintf
#define sprintf  pico_sprintf
// Now `snprintf()` uses picoprintf instead of the standard library

snprintf(buf, sizeof(buf), "Using picoprintf: %d", 123);
```

# Configuration
Control the code size and feature set by commenting and uncommenting macros in `picoprintf.h`. Available configuration macros:

| Macro | Features Enabled | Code Size Impact |
|-------|------------------|------------------|
| `PICOFORMAT_HANDLE_FILL` | Width specifiers: `%6d`, `%04d` | Small |
| `PICOFORMAT_HANDLE_DYNAMIC_PRECISION` | Dynamic precision: `%.*s`, `%.*f` | Small |
| `PICOFORMAT_HANDLE_FORCEDSIGN` | Forced signs: `%+d` | Minimal |
| `PICOFORMAT_HANDLE_BIN` | Binary format: `%b` | Small |
| `PICOFORMAT_HANDLE_OCT` | Octal format: `%o` | Small |
| `PICOFORMAT_HANDLE_HEX` | Hex format: `%x`, `%X`, `%p` | Small |
| `PICOFORMAT_HANDLE_FLOATS` | Float formats: `%f`, `%F`, `%e`, `%a` | Large |

# Benchmarks and Alternatives
The table below compares the _code size (in bytes)_ of this and similar libraries:

|    target      | **picoprintf** (min/full) |  [tinyprintf](https://github.com/cjlano/tinyprintf) (min/fll) | [nano-printf](https://github.com/charlesnicholson/nanoprintf) | [mpaland](https://github.com/mpaland/printf) |
| ---: | :---: | :---: | :---: | :---: |
| ARM thumb gcc 15.3.0 |  572 / 1724 |  968 / 1312 | 1760 | 3848 |
| ARM 32 gcc 15.3.0    | 1040 / 2272 | 1480 / 1928 | 2748 | 5740 |
| ARM 64 gcc 15.3.0    | 1508 / 3068 | 2380 / 3136 | 3892 | 6440 |
| ARM 64 clang 17.0.0  |  876 / 2487 | 1736 / 2192 | 3288 | 2444 |
| x86 gcc 13.3.0       |  825 / 1921 | 1379 / 1829 | 2701 | 4652 |
| x64 gcc 13.3.0       | 1066 / 2096 | 1961 / 2486 | 3120 | 5669 |

**Notes:**
* "min": minimal config (chars, strings, decimal ints only)
* "full": all optional features enabled
* all values collected with `-Os` flag; actual results depend on toolchain version and configuration

# Building This Project for Benchmarking
## embedded
Copy the header and C file into your embedded project, build, and inspect the `.map` file for code size.

## macOS
```sh
clang picoprintf.c picoatox.c picotest.c -Os -Wl,-map,pico.map
```

Calculate size:
```sh
cat pico.map | grep 1\] | grep -o -E '\t0x[0-9A-F]*' | sed 's/\t0x[^0-9A-F]*//' | awk '{ printf "%d\n", "0x" $1 }' | awk '{s+=$1} END {print s}'
```

## Linux
```sh
gcc picoprintf.c picoatox.c picotest.c -lm -Os -Wl,-Map,pico.map
```

Calculate size:
```sh
cat pico.map | grep pico_v -B 1 | grep -o -E '0x[0-9a-fA-F]{2,4} ' | sed 's/0x[^0-9a-fA-F]*//' | awk '{ print strtonum("0x" $1) }' | awk '{s+=$1} END {print s}'
```

# Testing
## Running Tests
```sh
# Compile and run all tests (select the features you want through the `-D` flags or by uncommening the `#define`s in `picoprintf.h`)
gcc picoprintf.c picoatox.c picotest.c -lm -DPICOFORMAT_HANDLE_FLOATS -DPICOFORMAT_HANDLE_HEX -o picotest
./picotest

# Test with verbose output
./picotest -v
```

## Features Tested
* all format specifiers (`%c`, `%s`, `%d`, `%x`, `%f`, etc.)
* edge cases (negative infinity, NaN, buffer boundaries)
* comparison with standard library `snprintf()`
* platform-specific behavior verification

# API Reference
## Functions
```c
int pico_snprintf(char *output_buffer, size_t size, const char *format, ...);
int pico_vsnprintf(char *output_buffer, size_t size, const char *format, va_list args);
int pico_sprintf(char *output_buffer, const char *format, ...);  // risk of buffer overflow, use `pico_snprintf()` or `pico_vsnprintf()` instead
```

## Return Value
Returns the number of characters written (excluding null terminator), or negative on error.

## Safety Notes
* **always use `pico_snprintf()`** instead of `pico_sprintf()` to prevent buffer overflows
* check return value to verify the string is written in full
* this library does not allocate memory dynamically

# Contributing
Contributions, bug reports, and feature requests are welcome!  Please open an issue or submit a pull request.

# License
This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.
