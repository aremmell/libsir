# libsir

libsir&mdash;a lightweight, cross-platform library for information distribution

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
[![Scorecard](https://img.shields.io/ossf-scorecard/github.com/aremmell/libsir?label=openssf%20scorecard&style=flat&color=%2340b900&cacheSeconds=60)](https://securityscorecards.dev/viewer/?uri=github.com/aremmell/libsir)
[![Practices](https://img.shields.io/cii/level/7861?label=openssf%20best%20practices&color=2340b900&cacheSeconds=60)](https://www.bestpractices.dev/en/projects/7861)

## <a id="overview" /> Overview

<!-- toc -->

- [Synopsis](#synopsis)
- [Notables](#notables)
- [Cross-platform compatibility](#cross-platform-compatibility)
  - [C++ compatibility](#c---compatibility)
  - [Language Bindings](#language-bindings)
- [An example](#an-example)
- [Building from source](#building-from-source)
  - [Unix Makefile](#unix-makefile)
    - [Environment Variables](#unix-makefile-envvars)
    - [Recipes](#unix-makefile-recipes)
  - [Visual Studio Code](#visual-studio-code)
    - [Build Tasks](#vs-code-build-tasks)
  - [Visual Studio 2022](#visual-studio-2022)

- [Dig in](#dig-in)
- [SAST Tools](#sast-tools)
- [Other Tools](#other-tools)

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
| **Linux**&nbsp;≳2.6.32<br>(glibc&nbsp;≳2.4, musl&nbsp;≳1.2.3, uClibc‑ng&nbsp;1.0.43, Bionic&nbsp;19) | **GCC**&nbsp;(4.8.4&nbsp;‑&nbsp;13.2.1),&nbsp; **Clang**&nbsp;(3.8&nbsp;‑&nbsp;17.0.1),&nbsp; **Oracle&nbsp;Studio&nbsp;C/C++**&nbsp;(≳12.6),&nbsp; **Circle**&nbsp;(*C++*,&nbsp;1.0.0‑200),&nbsp; **IBM&nbsp;Advance&nbsp;Toolchain**&nbsp;(14&nbsp;‑&nbsp;16),&nbsp; **IBM&nbsp;XL&nbsp;C/C++**&nbsp;(16.1),&nbsp; **IBM&nbsp;Open&nbsp;XL&nbsp;C/C++**&nbsp;(17.1.1),&nbsp; **NVIDIA&nbsp;HPC&nbsp;SDK&nbsp;C/C++**&nbsp;(23.5&nbsp;‑&nbsp;23.7),&nbsp; **Arm&nbsp;HPC&nbsp;C/C++**&nbsp;(22.1&nbsp;‑&nbsp;23.04.1),&nbsp; **DMD&nbsp;ImportC**&nbsp;(2.104.2&nbsp;‑&nbsp;2.105.2),&nbsp; **AMD&nbsp;Optimizing&nbsp;C/C++**&nbsp;(4&nbsp;‑&nbsp;4.1),&nbsp; **Intel&nbsp;oneAPI&nbsp;DPC++/C++**&nbsp;(2023.1&nbsp;‑&nbsp;2023.2.1),&nbsp; **Open64**&nbsp;(5,&nbsp;AMD&nbsp;4.5.2.1),&nbsp; **Intel&nbsp;C++&nbsp;Compiler&nbsp;Classic**&nbsp;(2021.9.0&nbsp;‑&nbsp;2021.10.0),&nbsp; **Android&nbsp;NDK**&nbsp;(r25c),&nbsp; **Portable&nbsp;C&nbsp;Compiler**&nbsp;(1.2.0.DEVEL‑20230730),&nbsp; **Kefir**&nbsp;(≳0.3.0),&nbsp; **Chibicc**&nbsp;(2020.12.6) |
| **AIX**&nbsp;7.2,&nbsp;7.3 | **GCC**&nbsp;(8.3&nbsp;‑&nbsp;11.3),&nbsp; **IBM&nbsp;XL&nbsp;C/C++**&nbsp;(16.1),&nbsp; **IBM&nbsp;Open&nbsp;XL&nbsp;C/C++**&nbsp;(17.1) |
| **macOS**&nbsp;≳10.15 | **Xcode**&nbsp;(11.7&nbsp;‑&nbsp;15),&nbsp; **GCC**&nbsp;(10.4&nbsp;‑&nbsp;13.2.1),&nbsp; **Clang**&nbsp;(11.0.3&nbsp;‑&nbsp;16.0.6),&nbsp; **DMD&nbsp;ImportC**&nbsp;(2.105.0&nbsp;‑&nbsp;2.105.2),&nbsp; **Intel&nbsp;C++&nbsp;Compiler&nbsp;Classic**&nbsp;(2021.9.0&nbsp;‑&nbsp;2021.10.0) |
| **Windows**&nbsp;≳10,&nbsp;11 | **Microsoft&nbsp;Visual&nbsp;C/C++**&nbsp;(17.6&nbsp;‑&nbsp;17.7.4),&nbsp; **Clang‑CL**&nbsp;(16.0.6),&nbsp; **GCC‑MinGW**&nbsp;(12.2.1&nbsp;‑&nbsp;13.2.1),&nbsp; **LLVM‑MinGW**&nbsp;(15.0&nbsp;‑&nbsp;16.0.6),&nbsp; **Embarcadero&nbsp;C++**&nbsp;(7.20&nbsp;‑&nbsp;7.60),&nbsp; **OrangeC**&nbsp;(≳6.0.71.10) |
| **Cygwin**&nbsp;3.4 | **GCC**&nbsp;(11.2&nbsp;‑&nbsp;13.2) |
| **FreeBSD**&nbsp;≳11.3 | **GCC**&nbsp;(11.4&nbsp;‑&nbsp;13.1),&nbsp; **Clang**&nbsp;(10&nbsp;‑&nbsp;17.0.1),&nbsp; **DMD&nbsp;ImportC**&nbsp;(2.105.0&nbsp;‑&nbsp;2.105.2) |
| **NetBSD**&nbsp;9.2 | **GCC**&nbsp;(7.5&nbsp;‑&nbsp;13.2),&nbsp; **Clang**&nbsp;(15.0.7) |
| **OpenBSD**&nbsp;7.3 | **GCC**&nbsp;(11.2),&nbsp; **Clang**&nbsp;(13) |
| **DragonFly**&nbsp;**BSD**&nbsp;6.4 | **GCC**&nbsp;(8.3&nbsp;‑&nbsp;13),&nbsp; **Clang**&nbsp;(10.0.1&nbsp;‑&nbsp;16.0.6) |
| **GNU/Hurd** | **GCC**&nbsp;(9.5&nbsp;‑&nbsp;13.2),&nbsp; **Clang**&nbsp;(9.0.1&nbsp;‑&nbsp;13.0.1) |
| **Haiku**&nbsp;R1b4 | **GCC**&nbsp;(11.2&nbsp;‑&nbsp;13.2),&nbsp; **Clang**&nbsp;(12.0.1&nbsp;‑&nbsp;16.0.6) |
| **Solaris**&nbsp;11.4 | **GCC**&nbsp;(10.3&nbsp;‑&nbsp;11.2),&nbsp; **Clang**&nbsp;(6&nbsp;‑&nbsp;11),&nbsp; **Oracle&nbsp;Studio&nbsp;C/C++**&nbsp;(≳12.6) |
| **illumos** | **GCC**&nbsp;(7.5&nbsp;‑&nbsp;12.3),&nbsp; **Clang**&nbsp;(15.0.7) |
| **SerenityOS** | **GCC**&nbsp;(13.1) |
| **WebAssembly/JS** | **Emscripten**&nbsp;(3.1.45&nbsp;‑&nbsp;3.1.46),&nbsp; **Node.js**&nbsp;(20.5&nbsp;‑&nbsp;20.7) |

* libsir is known to work on most common (and many uncommon) architectures. It has actually been built and tested on **Intel** (x86\_64, i686, x32), **ARM** (ARMv6, ARMv7HF, ARMv8‑A), **POWER** (PowerPC, PPC64, PPC64le), **MIPS** (MIPS64, MIPS32, 74Kc), **SPARC** (SPARC64, V8, LEON3), **z/Architecture** (S390X), **SuperH** (SH‑4A), **RISC‑V** (RV64), **OpenRISC** (OR1200), and **m68k** (68020+).
* This table only lists toolchains that have actually been tested and is by no means exhaustive&mdash;newer (or older) versions are likely to work without fanfare. In fact, if it *doesn't* work on your machine, it's probably a bug.

### <a id="c---compatibility" /> C++ compatibility

* A wrapper header is under development which will allow libsir to be used in C++20 projects, optionally integrating with popular formatting libraries:

<table align="center">
 <tbody>
  <tr>
   <td align="center"><center>&nbsp;<a href="https://fmt.dev/"><b>{fmt}</b></a>&nbsp;</center></td>
   <td align="center"><center>&nbsp;<a href="https://en.cppreference.com/w/cpp/utility/format/format"><b>std</b>::<b>format</b></a>&nbsp;</center></td>
   <td align="center"><center>&nbsp;<a href="https://theboostcpplibraries.com/boost.format"><b>Boost</b>.<b>Format</b></a>&nbsp;</center></td>
  </tr>
  <tr>
   <td align="center"><center>&nbsp;<a href="https://cplusplus.com/reference/istream/iostream"><b>std</b>::<b>iostream</b></a>&nbsp;</center></td>
   <td align="center"><center>&nbsp;<a href="https://github.com/c42f/tinyformat"><b>tinyformat</b></a>&nbsp;</center></td>
   <td align="center"><center>&nbsp;<a href="https://github.com/seanbaxter/circle/blob/master/new-circle/README.md#string-constant-formatting"><b>Circle</b>.<b>format</b></a>&nbsp;</center></td>
  </tr>
 </tbody>
</table>

### <a id="language-bindings" /> Language Bindings

* [Bindings](https://github.com/aremmell/libsir/tree/master/bindings) for scripting tools and other high-level programming languages are being developed, including support for:

<table align="center">
 <tbody>
  <tr>
   <td align="center"><center>&nbsp;<a href="https://www.softintegration.com/"><b>Ch</b></a>&nbsp;</center></td>
   <td align="center"><center>&nbsp;<a href="https://www.python.org/"><b>Python</b></a>&nbsp;</center></td>
  </tr>
 </tbody>
</table>

## <a id="an-example" /> An example

Some sample terminal output from libsir demonstrating the default text styling settings for stdio (*note that this is just a sample; libsir supports 4, 8, and 24-bit color modes, so a virtually limitless number of configurations are possible*):

![sample terminal output](./docs/res/sample-terminal.png)

This output is from the `example` application, whose source code can be located in the [example](https://github.com/aremmell/libsir/tree/master/example) directory. If you're curious about a very basic implementation of libsir in a practical context, that's a good place to start.

**Coming soon:** A C++20 version of the example program.

## <a id="building-from-source" /> Building from source

There are several options available for building libsir from source:

### <a id="unix-makefile" /> Unix Makefile

The Makefiles are compatible with **GNU Make** version **3.81** and later (**4.4** recommended) and support most Unix-like operating systems: AIX, Solaris, Linux, illumos, macOS, Cygwin, BSD, GNU/Hurd, Haiku, etc.

#### <a id="unix-makefile-envvars" /> Environment variables

A number of environment variables are available which affect the way in which libsir and its various build artifacts are created (*e.g., turning off optimizations and enabling debug symbols*). Information about these variables and how to use them along with make can be found in the [online documentation](https://libsir.rml.dev/config.html).

#### <a id="unix-makefile-recipes" /> Recipes

|   Recipe Type    |     Command      | &nbsp;&nbsp;&nbsp;&nbsp;Output file(s)         |
|:----------------:|:----------------:|:-----------------------------------------------|
| Test suite (C)   |  `make tests`    | <ul><li>*build/bin/sirtests[.exe]*</li></ul>   |
| Test suite (C++) |  `make tests++`  | <ul><li>*build/bin/sirtests++[.exe]*</li></ul> |
| Example app      |  `make example`  | <ul><li>*build/bin/sirexample[.exe]*</li></ul> |
| Static library   |  `make static`   | <ul><li>*build/lib/libsir_s.a*</li></ul>       |
| Shared library   |  `make shared`   | <ul><li>*build/lib/libsir.so*</il></ul>        |
| Installation     |  `make install`  | <ul><li>*$PREFIX/lib/libsir_s.a*</li><li>*$PREFIX/lib/libsir.so*</li><li>*$PREFIX/include/sir.h*</li><li>*$PREFIX/include/sir/\*.h*</li></ul> |

### <a id="visual-studio-code" /> Visual Studio Code

A VS Code workspace file is located in the root of the repository (`sir.code-workspace`). Build tasks are only configured for macOS and Linux; on other platforms, use the integrated terminal to run GNU [make](#unix-makefile) manually.

#### <a id="vs-code-build-tasks" /> Build Tasks

| Build Task            | Equivalent `make` Command                          |
|:---------------------:|:---------------------------------------------------|
| Clean                 | `make clean`                                       |
| Static Library        | `make static`                                      |
| DBG: Static Library   | `env SIR_DEBUG=1 SIR_SELFLOG=1 make static`        |
| Shared Library        | `make shared`                                      |
| DBG: Shared Library   | `env SIR_DEBUG=1 SIR_SELFLOG=1 make shared`        |
| Test Suite (C)        | `make tests`                                       |
| DBG: Test Suite (C)   | `env SIR_DEBUG=1 SIR_SELFLOG=1 make tests`         |
| Test Suite (C++)      | `make tests++`                                     |
| DBG: Test Suite (C++) | `env SIR_DEBUG=1 SIR_SELFLOG=1 make tests++`       |
| Example App (C)       | `make example`                                     |

### <a id="visual-studio-2022" /> Visual Studio 2022

A Visual Studio 2022 solution containing appropriate project files can be found in the [msvs](https://github.com/aremmell/libsir/tree/master/msvs) directory. Tested on Windows 11 (*x86, x64, and Arm64*) with Visual Studio 17.6+. See the [README](https://github.com/aremmell/libsir/tree/master/msvs/README.md) for more information.

## <a id="dig-in" /> Dig in

If you are genuinely interested in utilizing libsir, you are encouraged to read the [full online documentation](https://libsir.rml.dev) to get a better understanding of the library's capabilities and interfaces.

Suggested initial sections:
 * [Public Functions](https://libsir.rml.dev/group__publicfuncs.html)
 * [Public Types](https://libsir.rml.dev/group__publictypes.html)
 * [Configuration](https://libsir.rml.dev/config.html)
 * [Test suite](https://libsir.rml.dev/testsuite.html)

## <a id="sast-tools" /> SAST Tools

The developers are obsessed with the quality, reliability, and correctness of libsir.

The following **SAST** (*static application*/*analysis security*/*software testing*) and verification tools are used&mdash;on **every commit**&mdash;as part of our comprehensive [continuous integration](https://gitlab.com/libsir/libsir/-/pipelines) testing process:

| Tool | Usage |
|-----:|:------|
| [**PVS-Studio**](https://pvs-studio.com/pvs-studio/?utm_source=website&utm_medium=github&utm_campaign=open_source) | Static analysis tool for C, C++, C#, and Java code                                 |
| [Clang Static Analyzer](https://clang-analyzer.llvm.org/)                                                          | Static analysis tool for C, C++, and Objective-C code                              |
| [Coveralls](https://coveralls.io/github/aremmell/libsir)                                                           | Line-by-line test coverage reporting and statistical analysis                      |
| [Coverity® Scan](https://scan.coverity.com/projects/aremmell-libsir)                                               | Find and fix defects in Java, C/C++, C#, JS, Ruby, and Python code                 |
| [Cppcheck](https://cppchecksolutions.com/)                                                                         | Static analysis tool for C and C++ code                                            |
| [DUMA](https://github.com/johnsonjh/duma)                                                                          | Detect Unintended Memory Access, a memory debugger                                 |
| [Flawfinder](https://dwheeler.com/flawfinder/)                                                                     | Scans C and C++ source code for potential security weaknesses                      |
| [GCC Static Analyzer](https://gcc.gnu.org/onlinedocs/gcc/Static-Analyzer-Options.html)                             | Coverage-guided symbolic execution static analyzer for C code                      |
| [GNU Cppi](https://www.gnu.org/software/cppi/)                                                                     | C preprocessor directive linting, indenting, and regularization                    |
| [Oracle Developer Studio](https://www.oracle.com/application-development/developerstudio/)                         | Performance, security, and thread analysis tools for C, C++, and Fortran           |
| [REUSE](https://api.reuse.software/info/github.com/aremmell/libsir)                                                | Verifies compliance with the [REUSE](https://reuse.software/) licensing guidelines |
| [Semgrep](https://semgrep.dev)                                                                                     | A fast, open-source, static analysis engine for many languages                     |
| [SonarCloud](https://sonarcloud.io/project/overview?id=aremmell_libsir)                                            | Cloud-based code quality and security analysis tools                               |
| [Valgrind](https://valgrind.org/)                                                                                  | Tools for memory debugging, memory leak detection, and profiling                   |
| [Visual Studio Code Analyzer](https://learn.microsoft.com/en-us/cpp/code-quality/)                                 | Tools to analyze and improve C/C++ source code quality                             |

## <a id="other-tools" /> Other Tools

Additional tools used for libsir development and testing include:

| Tool | Usage |
|-----:|:------|
| [CBMC](http://www.cprover.org/cbmc/)                                                                         | Bounded Model Checker for C and C++ programs                        |
| [Frama-C](https://frama-c.com)                                                                               | Framework for Modular Analysis of C programs                        |
| [GNU Global](https://www.gnu.org/software/global/)                                                           | Source code indexing and tagging system                             |
| [Intel® VTune™ Profiler](https://www.intel.com/content/www/us/en/developer/tools/oneapi/vtune-profiler.html) | Performance analysis and profiling tools for Intel systems          |
| [PurifyPlus™](https://www.teamblue.unicomsi.com/products/purifyplus/)                                        | Run-time analysis tools for application reliability and performance |
| [SoftIntegration Ch](https://www.softintegration.com/)                                                       | C/C++ interpreter and interactive platform for scientific computing |
| [Very Sleepy](http://www.codersnotes.com/sleepy/)                                                            | Sampling CPU profiler for Microsoft Windows applications            |
