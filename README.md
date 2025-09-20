**pico-printf** implements `sprintf`, `snprintf`, and `vsnprintf` for embedded platforms.  Its goal is to be very small (under 1 kB in minimal config), avoid using dynamic memory, and be customizable.

**pico-printf** implements basic subset of format specifiers: **strings**, **chars**, **decimal** integers.  Optionally (enabled by compile-time macro declarations) it also supports **octal** and **hex** integers, **binary** integers via `"%b"`, as well as **floats**.

# Benchmarks and Alternatives
**picoprintf** is compile-time customizable.  This lets you control the module's footprint and only use what you need.  While the table below reflects the smallest possible (chars, strings, ints) and the largest possible (simple floats, hex, oct) footprints, the actual size will be somewhere in between, depending on features enabled via the compile-time macros.

Please note the actual values depend on the exact compiler used, build-time parameters, alignment, and other variables.  Thus, the values below should be treated as relative.

The code size of this and similar libraries, as reported in .map file, adding up `.text` and `.rodata` segments in bytes:

|    target      | **picoprintf** min | **picoprintf** full | <a href="https://github.com/mpaland/printf">mpaland</a> | <a href="https://github.com/cjlano/tinyprintf">tinyprintf</a> | <a href="https://github.com/charlesnicholson/nanoprintf">nanoprintf</a> |
| ---: | :---: | :---: | :---: | :---: | :---: |
| ARM thumb gcc 11.3.0 |  504 | 1320 | 4438 | 1528 | 2388 |
| ARM 32 gcc 11.3.0    |  832 | 2192 | 6962 | 2372 | 3764 |
| ARM 64 gcc 11.3.0    | 1140 | 2160 | 6582 | 3200 | 4612 |
| ARM 64 clang 14.0.3  |  744 | 1954 | 5846 | 2401 | 4066 |
| x86 gcc 9.4.0        |  638 | 1563 | 5022 | 3649 | 3681 |
| x64 gcc 9.4.0        |  909 | 1726 | 5974 | 3782 | 2676 |

all values are collected by compiling with `-Os` flag

# <a href="usage">Usage</a>
1. copy `picoprint.h` and `picoprintf.c` into your project
1. `#include "picoprintf.h"` wherever you need it
1. (optional) adjust the `#define`s in `picoprintf.h` to include options you need (this affects the footprint in your flash, ROM, or OTP)
1. (optional) `#define snprintf pico_snprintf`
1. enjoy!

# Building This Project
this chapter discusses how this project is built for benchmarking.  For instructions on using **picoprintf** in your project, see [Usage](#usage) above

## embedded
copy the header and the .c file into any project, build it and notice the .map file

## macOS
to build for native CPU:

`clang picoprintf.c picoatox.c picotest.c -Os -Wl,-map,pico.map`

calculate the size of the module:

`cat pico.map | grep 1\] | grep -o -E '\t0x[0-9A-F]*' | sed 's/\t0x[^0-9A-F]*//' | awk '{ printf "%d\n", "0x" $1 }' | awk '{s+=$1} END {print s}'`

## Linux

`gcc picoprintf.c picoatox.c picotest.c -lm -Os -Wl,-Map,pico.map`

calculate the size of the module:

`cat pico.map | grep pico_v -B 1 | grep -o -E '0x[0-9a-fA-F]{2,4} ' | sed 's/0x[^0-9a-fA-F]*//' | awk '{ printf "%d\n", "0x" $1}' | awk '{s+=$1} END {print s}'`
