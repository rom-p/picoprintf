**pico-printf** implements `sprintf`, `snprintf`, and `vsnprintf` for embedded platforms.  Its main goal is to be very small (under 1 kB in its minimal config), avoid using dynamic memory, and be customizable.

**pico-printf** implements basic subset of format specifiers: **strings**, **chars**, **decimal** integers.  Optionally (enabled by compile-time macro declarations) it also supports **octal** and **hex** integers, **binary** integers via `"%b"`, as well as **floats**.

# Benchmarks and Alternatives
**picoprintf** is compile-time customizable.  This enables tight control over the function's footprint and using only what you need.  While the table below reflects the smallest possible (chars, strings, ints) and the largest possible (simple floats, hex, oct) footprints, the real footprint size will be somewhere in the middle depending of features enabled via the compile-time macros

Please note the actual values depend on the exact compiler used, build-time parameters and other variables.  Thus, the values below are better treated as relative and not absolute.

https://github.com/mpaland/printf
https://github.com/cjlano/tinyprintf
https://github.com/charlesnicholson/nanoprintf

|    target      | **picoprintf** minimum | **picoprintf** full | mpaland | tinyprintf | nanoprintf |
| ---: | :---: | :---: | :---: | :---: | :---: |
 ARM Thumb gcc  |  526 | 1084 |
 ARM 32 gcc     |
 ARM 64 gcc     |
 ARM 64 clang   |  844 | 2039 | 5846 | 2401 | 4066 |
 x86 gcc        |
 x86 msvc       |
 x64 gcc        |
 x64 msvc       |

 all values are collected by compiling with `-Os` flag

# <a href="usage">Usage</a>
1. copy `picoprint.h` and `picoprintf.cpp` into your project
1. `#include "picoprintf.h"` wherever you need it
1. (optional) adjust the `#define`s in `picoprintf.h` to include options you need (this changes the footprint in your flash or ROM)
1. enjoy!

# Building This Project
this chapter discusses how this project is built.  For instructions on using **picoprintf** in your project, see [Usage](#usage) above

## embedded
copy the header and the .cpp file into any project, build it and notice the .map file

## macOS
to build for native CPU:

`clang picoprintf.cpp picotest.cpp -Wl,-map,pico.map -Os`

to build for other CPUs:

`clang picoprintf.cpp picotest.cpp -Os -Wl,-Map,pico.map -march=ARCH`

where ARCH is from

`clang -print-targets`

To calculate the size of the module:

`cat pico.map | grep 1\] | grep -o -E '\t0x[0-9A-F]*' | sed 's/\t0x[^0-9A-F]*//' | awk '{ printf "%d\n", "0x" $1 }' | awk '{s+=$1} END {print s}'`

## Linux

`gcc picoprintf.cpp picotest.cpp -Os -Wl,-Map,pico.map`

## Windows

# DELETEME
// possible names:
// tiny
// micro
// nano
// pico