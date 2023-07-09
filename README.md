# libsir

<!-- SPDX-License-Identifier: MIT -->
<!-- Copyright (c) 2018-current Ryan M. Lederman <lederman@gmail.com> -->

[![Build Status](https://app.travis-ci.com/aremmell/libsir.svg?branch=master)](https://app.travis-ci.com/aremmell/libsir) ![REUSE Compliance](https://img.shields.io/reuse/compliance/github.com%2Faremmell%2Flibsir?label=REUSE3&color=2340b911)
 ![GitHub](https://img.shields.io/github/license/aremmell/libsir?color=%2340b911)

## Synopsis

libsir is a cross-platform, thread-safe logging library written in C that is designed to simplify and streamline the generation and distribution of human-readable information in software.

Using libsir, you can make a single call that simultaneously sends information to multiple destinations, _each with their own individual preference for format and levels_.

Each 'level' or 'priority' of output can be visually styled however you wish for `stdio` destinations. This allows for immediate visual confirmation that a message appeared in the terminal that you should pay attention to, while less pressing information is more subtle in appearance.

![libsir visual graph](./docs/res/libsir-alpha.gif)

## Notables

- No dependencies (other than `libc` and `pthreads`). On Windows, libsir uses the native synchronization API.
- Accompanied by a robust test suite to ensure dependable behavior, even if you make modifications to the source.
- Won't fail silently &mdash; C library or platform level errors are captured and stored for posterity, on a per-thread basis. The _function, file, and line number from which it originated_ are also captured. Additionally, libsir defines its own set of error messages to aid in troubleshooting.
- Hardened &mdash; every function contains sanity checks for internal state as well as arguments passed in through the external interfaces. libsir's make recipes use _&ndash;Wall &ndash;Wextra &ndash;Wpedantic_, too.
- [Full documentation](https://libsir.rml.dev), thanks to Doxygen.
- Lightweight &mdash; the shared library comes in at around 76KB, and the static library around 70KB.

## Cross-platform compatibility

At this time, the test suite has been compiled and tested on (at minimum) these configurations:

| Toolset   | Versions            | Operating system                     | Architecture                 |
| :-------- | :-----------------: | :----------------------------------: | ---------------------------: |
| gcc       | 5.4-7.5             |    Ubuntu, Mint, Fedora, FreeBSD     |   x64, armhf, arm64, ppc64le |
| clang     | 6.0-14.0            | macOS, Ubuntu, Mint, FreeBSD, Fedora |   x64, arm64                 |
| MSVC (cl) | 17.6                |               Windows                |   x64, arm64                 |

## A sample

Some sample terminal output from libsir demonstrating the default configuration:

![sample terminal output](./docs/res/sample-terminal.gif)

## Getting started

Clone or download this repository to your machine, then choose how you'd like to integrate libsir into your project:

### Visual Studio Code

There is already a code-workspace in the repository (and  my primary means of compiling and debugging the library across platforms). As of right now, the build and launch tasks are not fully functional for every platform, but that's easy enough to figure out, right?.

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
