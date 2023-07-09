# libsir

libsir &mdash; a cross-platform, thread-safe logging library

<!-- SPDX-License-Identifier: MIT -->
<!-- Copyright (c) 2018-current Ryan M. Lederman <lederman@gmail.com> -->

 [![Build Status](https://app.travis-ci.com/aremmell/libsir.svg?branch=master)](https://app.travis-ci.com/aremmell/libsir) ![REUSE Compliance](https://img.shields.io/reuse/compliance/github.com%2Faremmell%2Flibsir?label=REUSE3&color=2340b911)
 ![GitHub](https://img.shields.io/github/license/aremmell/libsir?color=%2340b911)

## Overview

<!-- toc -->

- [Synopsis](#synopsis)
- [Notables](#notables)
- [Cross-platform compatibility](#cross-platform-compatibility)
- [A sample](#a-sample)
- [Getting started](#getting-started)
  * [Visual Studio Code](#visual-studio-code)
  * [Visual Studio 2022](#visual-studio-2022)
  * [Unix Makefile](#unix-makefile)
  * [Further reading](#further-reading)

<!-- tocstop -->

## Synopsis

libsir is a cross-platform, thread-safe logging library written in C that is designed to simplify and streamline the generation and distribution of human-readable information in software.

Using libsir, you can make a single call that simultaneously sends information to multiple destinations, _each with their own individual preference for format and levels_.

Each 'level' or 'priority' of output can be visually styled however you wish for `stdio` destinations. This allows for immediate visual confirmation that a message appeared in the terminal that you should pay attention to, while less pressing information is more subtle in appearance.

![libsir visual graph](./docs/res/libsir-alpha.png)

## Notables

- No dependencies (other than `libc` and `pthreads`). On Windows, libsir uses the native synchronization API.
- Accompanied by a robust test suite to ensure dependable behavior, even if you make modifications to the source.
- Won't fail silently &mdash; C library or platform level errors are captured and stored for posterity, on a per-thread basis. The _function, file, and line number from which it originated_ are also captured. Additionally, libsir defines its own set of error messages to aid in troubleshooting.
- Hardened &mdash; every function contains sanity checks for internal state as well as arguments passed in through the external interfaces. libsir's make recipes use _&ndash;Wall &ndash;Wextra &ndash;Wpedantic_, too.
- [Full documentation](https://libsir.rml.dev), thanks to Doxygen.
- Lightweight &mdash; the shared library comes in at around 76&nbsp;KiB, and the static library around 70&nbsp;KiB.

## Cross-platform compatibility

At this time, the libsir supports (*compiles and passes the tests*) on at least the following operating system and toolchain combinations:

| System | Toolchain |
| ------:|:--------- |
| **Linux**&nbsp;2.6.32+<br>(glibc&nbsp;2.18+, musl&nbsp;1.2.3+, uClibc‑ng&nbsp;1.0.43+, Bionic&nbsp;19+) | **GCC** (4.8.4 - 13.1.1),&nbsp; **Clang** (6 - 16.0.6),&nbsp; **Oracle Studio C/C++** (12.6),&nbsp; **IBM Advance Toolchain** (14 - 16),&nbsp; **IBM Open XL C/C++** (17.1.1),&nbsp; **NVIDIA HPC SDK C/C++** (23.5),&nbsp; **Arm HPC C/C++** (22.1),&nbsp; **AMD Optimizing C/C++** (4.0.0),&nbsp; **Intel oneAPI DPC++/C++** (2023.1),&nbsp; **Intel C++ Compiler Classic** (2021.9),&nbsp; **Android NDK** (r25c) |
| **macOS**&nbsp;10.15+ | **Xcode** (12.4 - 15),&nbsp; **GCC** (10.4 - 13.1),&nbsp; **Clang** (12.0.1 - 16.0.6),&nbsp; **Intel C++ Compiler Classic** (2021.9) |
| **Windows**&nbsp;10+ | **Microsoft Visual C/C++** (17.6),&nbsp; **GCC-MinGW** (12.2.1 - 13.1.1),&nbsp; **LLVM-MinGW** (15.0 - 16.0.6) |
| **Cygwin**&nbsp;3.4+ | **GCC** (11.2 - 11.4) |
| **FreeBSD**&nbsp;11.4+ | **GCC** (11.4 - 12.2),&nbsp; **Clang** (10.0.0 - 14.0.5) |
| **NetBSD**&nbsp;9.2+ | **GCC** (7.5 - 13.1),&nbsp; **Clang** (15.0.7) |
| **DragonFly**&nbsp;**BSD**&nbsp;6.4+ | **GCC** (8.3 - 13),&nbsp; **Clang** (10.0.1 - 14.0.6) |
| **Haiku**&nbsp;R1b4 | **GCC** (11.2),&nbsp; **Clang** (12.0.1) |
| **Solaris**&nbsp;11.4 | **GCC** (10.3 - 11.2),&nbsp; **Clang** (6 - 11),&nbsp; **Oracle Studio C/C++** (12.6) |
| **illumos** | **GCC** (7.5 - 11.3),&nbsp; **Clang** (15.0.7) |
| *Coming soon* | *Support planned for* **AIX** *and* **OpenBSD** |

* libsir is known to work on most common (and many uncommon) architectures.
  * It has actually been built and tested on **Intel** (x86\_64, i686), **ARM** (ARMv6, ARMv7HF, ARMv8-A), **POWER** (PowerPC, PPC64, PPC64le), **MIPS** (MIPS64, MIPS32, 74Kc), **z/Architecture** (S390X), **SuperH** (SH‑4A), **RISC‑V** (RV64), **OpenRISC** (OR1200), and **m68k** (68020+).
* If it doesn't work on your machine, it's probably a bug.

## A sample

Some sample terminal output from libsir demonstrating the default configuration:

![sample terminal output](./docs/res/sample-terminal.png)

## Getting started

Clone or download this repository to your machine, then choose how you'd like to integrate libsir into your project:

### Visual Studio Code

There is already a code-workspace in the repository (and my primary means of compiling and debugging the library across platforms). As of right now, the build and launch tasks are not fully functional for every platform, but that's easy enough to figure out, right?.

### Visual Studio 2022

A very recent addition is an sln and some vcxproj files in the [msvc](./msvc) directory. They are confirmed to work correctly on Windows 11 (x64 &amp; arm64) with Visual Studio 17.6.

### Unix Makefile

| Recipe Type    |    Command          |       Output file(s)                                          |
| :------------: | :-----------------: | :-----------------------------------------------------------: |
| Test suite     |  `make tests`       |                  _build/sirtests[.exe]_                       |
| Example app    | `make example`      |                  _build/sirexample[.exe]_                     |
| Static library |    `make static`    |                   _build/lib/libsir_s.a_                      |
| Shared library |    `make shared`    |                    _build/lib/libsir.so_                      |
|  Install[^1]   | `sudo make install` |    _`$(INSTALLLIB)`/libsir.so  &amp; `$(INSTALLINC)`/sir.h_   |

[^1]: This recipe isn't really ready for production use. Use it at your own risk &mdash; currently it manually copies files; that’s it. `$(INSTALLLIB)` and `$(INSTALLINC)` are `/usr/local/lib` and `/usr/local/include`, respectively. It's on my TODO list to utilize the `install` tool _(or just ditch the Makefile and use CMake)_.

### Further reading

If you are genuinely interested in utilizing libsir, you are encouraged to read the [full online documentation](https://libsir.rml.dev) to get a better understanding of the library's capabilities and interfaces.
