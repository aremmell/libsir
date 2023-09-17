# libsir

libsir&mdash;a cross-platform, thread-safe logging library

<!-- SPDX-License-Identifier: MIT -->
<!-- Copyright (c) 2018-current Ryan M. Lederman <lederman@gmail.com> -->

[![Release](https://img.shields.io/github/v/release/aremmell/libsir?color=%2340b900&cacheSeconds=60)](https://github.com/aremmell/libsir/releases/latest)
[![Commits](https://img.shields.io/github/commits-since/aremmell/libsir/latest?cacheSeconds=60&color=%2340b900)](https://github.com/aremmell/libsir/commits/master)
[![License](https://img.shields.io/github/license/aremmell/libsir?color=%2340b900&cacheSeconds=60)](https://github.com/aremmell/libsir/blob/master/LICENSE)
[![OpenHub](https://img.shields.io/badge/openhub-libsir-ok.svg?color=%2340b900&cacheSeconds=60)](https://www.openhub.net/p/libsir/)
[![GitLab](https://gitlab.com/libsir/libsir/badges/master/pipeline.svg?)](https://gitlab.com/libsir/libsir/pipelines/master/latest)
[![Coverage](https://img.shields.io/coverallsCoverage/github/aremmell/libsir?color=%2340b900&cacheSeconds=60)](https://coveralls.io/github/aremmell/libsir)
[![Coverity](https://img.shields.io/coverity/scan/28843.svg?color=%2340b900&cacheSeconds=60)](https://scan.coverity.com/projects/aremmell-libsir)
[![REUSE](https://img.shields.io/reuse/compliance/github.com%2Faremmell%2Flibsir?label=REUSE3&color=%2340b900&cacheSeconds=60)](https://api.reuse.software/info/github.com/aremmell/libsir)
[![Maintainability](https://sonarcloud.io/api/project_badges/measure?project=aremmell_libsir&metric=sqale_rating)](https://sonarcloud.io/component_measures?id=aremmell_libsir&metric=new_maintainability_rating&view=list)
[![Reliability](https://sonarcloud.io/api/project_badges/measure?project=aremmell_libsir&metric=reliability_rating)](https://sonarcloud.io/component_measures?id=aremmell_libsir&metric=new_reliability_rating&view=list)
[![Security](https://sonarcloud.io/api/project_badges/measure?project=aremmell_libsir&metric=security_rating)](https://sonarcloud.io/component_measures?id=aremmell_libsir&metric=new_security_rating&view=list)

## <a id="overview" /> Overview

<!-- toc -->

- [Synopsis](#synopsis)
- [Notables](#notables)
- [Cross-platform compatibility](#cross-platform-compatibility)
- [An example](#an-example)
- [Building from source](#building-from-source)
  - [Visual Studio Code](#visual-studio-code)
  - [Visual Studio 2022](#visual-studio-2022)
  - [Unix Makefile](#unix-makefile)
- [Dig in](#dig-in)
- [SAST Tools](#sast-tools)

<!-- tocstop -->

## <a id="synopsis" /> Synopsis

libsir is a cross-platform, thread-safe logging library written in C (*ISO/IEC 9899:2011 C11*) that is designed to simplify and streamline the generation and distribution of human-readable information in software.

Using libsir, you can make a single call that simultaneously sends information to multiple destinations, *each with their own individual preference for format and levels*.

Each 'level' or 'priority' of output can be visually styled however you wish for `stdio` destinations. This allows for immediate visual confirmation that a message appeared in the terminal that you should pay attention to, while less pressing information is more subtle in appearance.

![libsir visual graph](./docs/res/libsir-alpha.png)

## <a id="notables" /> Notables

- No dependencies (other than `libc` and `pthreads`). On Windows, libsir uses the native synchronization API.
- Accompanied by a robust test suite to ensure dependable behavior, even if you make modifications to the source.
- Won't fail silently&mdash;C library or platform level errors are captured and stored for posterity, on a per-thread basis. The *function, file, and line number from which it originated* are also captured. Additionally, libsir defines its own set of error messages to aid in troubleshooting.
- Hardened&mdash;every function contains sanity checks for internal state as well as arguments passed in through the external interfaces. libsir's standard make recipes use *‑Wall ‑Wextra ‑Wpedantic* (and pass with *‑Wmissing‑prototypes ‑Wdouble‑promotion ‑Wfloat‑conversion ‑Wmissing‑declarations ‑Wswitch‑enum ‑Wformat‑nonliteral ‑Wformat‑truncation ‑Wbad‑function‑cast ‑Wextra‑semi‑stmt*, too.)
- [Full documentation](https://libsir.rml.dev), thanks to Doxygen.
- Lightweight&mdash;the shared library comes in at around 70&nbsp;KiB, and the static library around 50&nbsp;KiB.

## <a id="cross-platform-compatibility" /> Cross-platform compatibility

At this time, libsir is supported (*that is, it compiles and passes the test suites*) on at least the following operating system and toolchain combinations:

| System | Toolchain |
| ------:|:--------- |
| **Linux**&nbsp;≳2.6.32<br>(glibc&nbsp;≳2.4, musl&nbsp;≳1.2.3, uClibc‑ng&nbsp;1.0.43, Bionic&nbsp;19) | **GCC**&nbsp;(4.8.4&nbsp;‑&nbsp;13.2.1),&nbsp; **Clang**&nbsp;(3.8&nbsp;‑&nbsp;17.0.0),&nbsp; **Oracle&nbsp;Studio&nbsp;C/C++**&nbsp;(≳12.6),&nbsp; **Circle**&nbsp;(C++&nbsp;1.0.0‑200),&nbsp; **IBM&nbsp;Advance&nbsp;Toolchain**&nbsp;(14&nbsp;‑&nbsp;16),&nbsp; **IBM&nbsp;XL&nbsp;C/C++**&nbsp;(16.1),&nbsp; **IBM&nbsp;Open&nbsp;XL&nbsp;C/C++**&nbsp;(17.1.1),&nbsp; **NVIDIA&nbsp;HPC&nbsp;SDK&nbsp;C/C++**&nbsp;(23.5&nbsp;‑&nbsp;23.7),&nbsp; **Arm&nbsp;HPC&nbsp;C/C++**&nbsp;(22.1&nbsp;‑&nbsp;23.04.1),&nbsp; **DMD&nbsp;ImportC**&nbsp;(2.104.2&nbsp;‑&nbsp;2.105.2),&nbsp; **AMD&nbsp;Optimizing&nbsp;C/C++**&nbsp;(4&nbsp;‑&nbsp;4.1),&nbsp; **Intel&nbsp;oneAPI&nbsp;DPC++/C++**&nbsp;(2023.1&nbsp;‑&nbsp;2023.2.1),&nbsp; **Open64**&nbsp;(5,&nbsp;AMD&nbsp;4.5.2.1),&nbsp; **Intel&nbsp;C++&nbsp;Compiler&nbsp;Classic**&nbsp;(2021.9.0&nbsp;‑&nbsp;2021.10.0),&nbsp; **Android&nbsp;NDK**&nbsp;(r25c),&nbsp; **Portable&nbsp;C&nbsp;Compiler**&nbsp;(1.2.0.DEVEL‑20230730),&nbsp; **Kefir**&nbsp;(≳0.3.0),&nbsp; **Chibicc**&nbsp;(2020.12.6) |
| **AIX**&nbsp;7.2,&nbsp;7.3 | **GCC**&nbsp;(8.3&nbsp;‑&nbsp;11.3),&nbsp; **IBM&nbsp;XL&nbsp;C/C++**&nbsp;(16.1),&nbsp; **IBM&nbsp;Open&nbsp;XL&nbsp;C/C++**&nbsp;(17.1) |
| **macOS**&nbsp;≳10.15 | **Xcode**&nbsp;(11.7&nbsp;‑&nbsp;15),&nbsp; **GCC**&nbsp;(10.4&nbsp;‑&nbsp;13.2.1),&nbsp; **Clang**&nbsp;(11.0.3&nbsp;‑&nbsp;16.0.6),&nbsp; **DMD&nbsp;ImportC**&nbsp;(2.105.0&nbsp;‑&nbsp;2.105.2),&nbsp; **Intel&nbsp;C++&nbsp;Compiler&nbsp;Classic**&nbsp;(2021.9.0&nbsp;‑&nbsp;2021.10.0) |
| **Windows**&nbsp;≳10,&nbsp;11 | **Microsoft&nbsp;Visual&nbsp;C/C++**&nbsp;(17.6&nbsp;‑&nbsp;17.7.4),&nbsp; **Clang‑CL**&nbsp;(16.0.6),&nbsp; **GCC‑MinGW**&nbsp;(12.2.1&nbsp;‑&nbsp;13.2.1),&nbsp; **LLVM‑MinGW**&nbsp;(15.0&nbsp;‑&nbsp;16.0.6),&nbsp; **Embarcadero&nbsp;C++**&nbsp;(7.20&nbsp;‑&nbsp;7.60),&nbsp; **OrangeC**&nbsp;(≳6.0.71.10) |
| **Cygwin**&nbsp;3.4 | **GCC**&nbsp;(11.2&nbsp;‑&nbsp;13.2) |
| **FreeBSD**&nbsp;≳11.3 | **GCC**&nbsp;(11.4&nbsp;‑&nbsp;13.1),&nbsp; **Clang**&nbsp;(10&nbsp;‑&nbsp;17.0.0),&nbsp; **DMD&nbsp;ImportC**&nbsp;(2.105.0&nbsp;‑&nbsp;2.105.2) |
| **NetBSD**&nbsp;9.2 | **GCC**&nbsp;(7.5&nbsp;‑&nbsp;13.2),&nbsp; **Clang**&nbsp;(15.0.7) |
| **OpenBSD**&nbsp;7.3 | **GCC**&nbsp;(11.2),&nbsp; **Clang**&nbsp;(13) |
| **DragonFly**&nbsp;**BSD**&nbsp;6.4 | **GCC**&nbsp;(8.3&nbsp;‑&nbsp;13),&nbsp; **Clang**&nbsp;(10.0.1&nbsp;‑&nbsp;16.0.6) |
| **WebAssembly/JS** | **Emscripten**&nbsp;(3.1.45‑&nbsp;3.1.46),&nbsp; **Node.js**&nbsp;(20.5) |
| **GNU/Hurd** | **GCC**&nbsp;(9.5&nbsp;‑&nbsp;13.2),&nbsp; **Clang**&nbsp;(9.0.1&nbsp;‑&nbsp;13.0.1) |
| **Haiku**&nbsp;R1b4 | **GCC**&nbsp;(11.2&nbsp;‑&nbsp;13.2),&nbsp; **Clang**&nbsp;(12.0.1&nbsp;‑&nbsp;16.0.6) |
| **Solaris**&nbsp;11.4 | **GCC**&nbsp;(10.3&nbsp;‑&nbsp;11.2),&nbsp; **Clang**&nbsp;(6&nbsp;‑&nbsp;11),&nbsp; **Oracle&nbsp;Studio&nbsp;C/C++**&nbsp;(≳12.6) |
| **illumos** | **GCC**&nbsp;(7.5&nbsp;‑&nbsp;11.4),&nbsp; **Clang**&nbsp;(15.0.7) |
| **SerenityOS** | **GCC**&nbsp;(13.1) |

* libsir is known to work on most common (and many uncommon) architectures. It has actually been built and tested on **Intel** (x86\_64, i686, x32), **ARM** (ARMv6, ARMv7HF, ARMv8‑A), **POWER** (PowerPC, PPC64, PPC64le), **MIPS** (MIPS64, MIPS32, 74Kc), **SPARC** (SPARC64, V8, LEON3), **z/Architecture** (S390X), **SuperH** (SH‑4A), **RISC‑V** (RV64), **OpenRISC** (OR1200), and **m68k** (68020+).
* This table only lists toolchains that have actually been tested and is by no means exhaustive&mdash;newer (or older) versions are likely to work without fanfare. In fact, if it *doesn't* work on your machine, it's probably a bug.

## <a id="an-example" /> An example

Some sample terminal output from libsir demonstrating the default configuration:

![sample terminal output](./docs/res/sample-terminal.png)

This output is from the `example` application, whose source code can be located in the [example](https://github.com/aremmell/libsir/tree/master/example) directory. If you're curious about a basic implementation of libsir in a practical context, that's a good place to start.

## <a id="building-from-source" /> Building from source

There are several options if you would like to build libsir from source:

### <a id="visual-studio-code" /> Visual Studio Code

There is a `code‑workspace` in the repository. As of right now, the build and launch tasks are not fully functional for every platform, but everything else should be good to go.

### <a id="visual-studio-2022" /> Visual Studio 2022

A very recent addition is an `sln` and some `vcxproj` files in the [msvc](https://github.com/aremmell/libsir/tree/master/msvs) directory. They are confirmed to work correctly on Windows 11 (x86, x64, Arm64) with Visual Studio 17.6+.

### <a id="unix-makefile" /> Unix Makefile

| Recipe Type      |    Command          |       Output file(s)                                          |
| ----------------:| :-----------------: |:------------------------------------------------------------- |
| Test suite (C)   |  `make tests`       |                  *build/bin/sirtests[.exe]*                   |
| Test suite (C++) |  `make tests++`     |                  *build/bin/sirtests++[.exe]*                 |
| Example app      |  `make example`     |                  *build/bin/sirexample[.exe]*                 |
| Static library   |  `make static`      |                   *build/lib/libsir_s.a*                      |
| Shared library   |  `make shared`      |                    *build/lib/libsir.so*                      |
| Installation     |  `make install`     | <ul><li>*$PREFIX/lib/libsir_s.a*</li><li>*$PREFIX/lib/libsir.so*</li><li>*$PREFIX/include/sir.h*</li><li>*$PREFIX/include/sir/\*.h*</li></ul> |

## <a id="dig-in" /> Dig in

If you are genuinely interested in utilizing libsir, you are encouraged to read the [full online documentation](https://libsir.rml.dev) to get a better understanding of the library's capabilities and interfaces.

- Suggested initial sections:

  * [Public Functions](https://libsir.rml.dev/group__publicfuncs.html)
  * [Public Types](https://libsir.rml.dev/group__publictypes.html)
  * [Configuration](https://libsir.rml.dev/config.html)
  * [Test suite](https://libsir.rml.dev/testsuite.html)

## <a id="sast-tools" /> SAST Tools

- [PVS-Studio](https://pvs-studio.com/pvs-studio/?utm_source=website&utm_medium=github&utm_campaign=open_source) - static analyzer for C, C++, C#, and Java code.
- [Clang Static Analyzer](https://clang-analyzer.llvm.org/) - static analysis tool for C, C++, and Objective-C.
- [Coveralls](https://coveralls.io/github/aremmell/libsir) - line-by-line test coverage reports and statistics.
- [Coverity® Scan](https://scan.coverity.com/projects/aremmell-libsir) - find defects in Java, C/C++, C#, JavaScript, Ruby, or Python code.
- [Cppcheck](https://cppchecksolutions.com/) - static analysis tool for C/C++ code.
- [DUMA](https://github.com/johnsonjh/duma) - Detect Unintended Memory Access, a memory debugger.
- [Flawfinder](https://dwheeler.com/flawfinder/) - scan C/C++ source code for security weaknesses.
- [GCC Static Analyzer](https://gcc.gnu.org/onlinedocs/gcc/Static-Analyzer-Options.html) - coverage-guided symbolic execution analyzer for C.
- [Oracle Developer Studio](https://www.oracle.com/application-development/developerstudio/) - performance, security, and thread analysis tools.
- [REUSE](https://api.reuse.software/info/github.com/aremmell/libsir) - verifies compliance with [REUSE](https://reuse.software/) licensing recommendations.
- [SonarCloud](https://sonarcloud.io/project/overview?id=aremmell_libsir) - cloud-based code quality and security analysis.
- [Valgrind](https://valgrind.org/) - tools for memory debugging, memory leak detection, and profiling.
- [Visual Studio Code Analyzer](https://learn.microsoft.com/en-us/cpp/code-quality/) - tools to analyze and improve C/C++ code quality.
