# libsir

libsir&mdash;a cross-platform, thread-safe logging library

<!-- SPDX-License-Identifier: MIT -->
<!-- Copyright (c) 2018-current Ryan M. Lederman <lederman@gmail.com> -->

[![Release](https://img.shields.io/github/v/release/aremmell/libsir?color=%2340b900?cacheSeconds=60)](releases/latest)
[![Commits](https://img.shields.io/github/commits-since/aremmell/libsir/latest?cacheSeconds=60&color=%2340b900)](commits/master)
[![License](https://img.shields.io/github/license/aremmell/libsir?color=%2340b900&cacheSeconds=60)](LICENSE)
[![OpenHub](https://img.shields.io/badge/openhub-libsir-ok.svg?color=%2340b900&cacheSeconds=60)](https://www.openhub.net/p/libsir/)
[![Build](https://img.shields.io/travis/com/aremmell/libsir?cacheSeconds=60&color=%2340b900)](https://app.travis-ci.com/aremmell/libsir)
[![Coverity](https://img.shields.io/coverity/scan/28843.svg?color=%2340b900&cacheSeconds=60)](https://scan.coverity.com/projects/aremmell-libsir)
[![Codacy](https://img.shields.io/codacy/grade/c631edd919a94bafa21c98dfcf72f2ad?color=%2340b900&cacheSeconds=60)](https://app.codacy.com/gh/aremmell/libsir/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)
[![Coverage](https://img.shields.io/coverallsCoverage/github/aremmell/libsir?color=%2340b900&cacheSeconds=60)](https://coveralls.io/github/aremmell/libsir)
[![REUSE](https://img.shields.io/reuse/compliance/github.com%2Faremmell%2Flibsir?label=REUSE3&color=%2340b900&cacheSeconds=60)](https://api.reuse.software/info/github.com/aremmell/libsir)

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
    - [Make targets](#make-targets)
    - [Make variables](#make-variables)
    - [Make options](#make-options)
- [Dig in](#dig-in)

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
- Hardened&mdash;every function contains sanity checks for internal state as well as arguments passed in through the external interfaces. libsir's make recipes use *‑Wall&nbsp;‑Wextra&nbsp;‑Wpedantic*, too.
- [Full documentation](https://libsir.rml.dev), thanks to Doxygen.
- Lightweight&mdash;the shared library comes in at around 64&nbsp;KiB, and the static library around 60&nbsp;KiB.

## <a id="cross-platform-compatibility" /> Cross-platform compatibility

At this time, libsir is supported (*that is, it compiles and passes the test suite*) on at least the following operating system and toolchain combinations:

| System | Toolchain |
| ------:|:--------- |
| **Linux**&nbsp;≳2.6.32<br>(glibc&nbsp;2.18, musl&nbsp;≳1.2.3, uClibc‑ng&nbsp;1.0.43, Bionic&nbsp;19) | **GCC**&nbsp;(4.8.4&nbsp;‑&nbsp;13.1.1),&nbsp; **Clang**&nbsp;(3.8&nbsp;‑&nbsp;16.0.6),&nbsp; **Oracle&nbsp;Studio&nbsp;C/C++**&nbsp;(≳12.6),&nbsp; **IBM&nbsp;Advance&nbsp;Toolchain**&nbsp;(14&nbsp;‑&nbsp;16),&nbsp; **IBM&nbsp;XL&nbsp;C/C++**&nbsp;(16.1),&nbsp; **IBM&nbsp;Open&nbsp;XL&nbsp;C/C++**&nbsp;(17.1.1),&nbsp; **NVIDIA&nbsp;HPC&nbsp;SDK&nbsp;C/C++**&nbsp;(23.5),&nbsp; **Arm&nbsp;HPC&nbsp;C/C++**&nbsp;(22.1),&nbsp; **AMD&nbsp;Optimizing&nbsp;C/C++**&nbsp;(4.0.0),&nbsp; **Intel&nbsp;oneAPI&nbsp;DPC++/C++**&nbsp;(2023.1),&nbsp; **Intel&nbsp;C++&nbsp;Compiler&nbsp;Classic**&nbsp;(2021.9),&nbsp; **Android&nbsp;NDK**&nbsp;(r25c),&nbsp; **Chibicc**&nbsp;(2020.12.6) |
| **AIX**&nbsp;7.2 | **GCC**&nbsp;(8.3&nbsp;‑&nbsp;11.3),&nbsp; **IBM&nbsp;XL&nbsp;C/C++**&nbsp;(16.1),&nbsp; **IBM&nbsp;Open&nbsp;XL&nbsp;C/C++**&nbsp;(17.1) |
| **macOS**&nbsp;≳10.15 | **Xcode**&nbsp;(12.4&nbsp;‑&nbsp;15),&nbsp; **GCC**&nbsp;(10.4&nbsp;‑&nbsp;13.1),&nbsp; **Clang**&nbsp;(12.0.1&nbsp;‑&nbsp;16.0.6),&nbsp; **Intel&nbsp;C++&nbsp;Compiler&nbsp;Classic**&nbsp;(2021.9) |
| **Windows**&nbsp;≳10, 11 | **Microsoft&nbsp;Visual&nbsp;C/C++**&nbsp;(17.6),&nbsp; **GCC‑MinGW**&nbsp;(12.2.1&nbsp;‑&nbsp;13.1.1),&nbsp; **LLVM‑MinGW**&nbsp;(15.0&nbsp;‑&nbsp;16.0.6) |
| **Cygwin**&nbsp;3.4 | **GCC**&nbsp;(11.2&nbsp;‑&nbsp;11.4) |
| **FreeBSD**&nbsp;≳11.3 | **GCC**&nbsp;(11.4&nbsp;‑&nbsp;12.2),&nbsp; **Clang**&nbsp;(10&nbsp;‑&nbsp;14.0.5) |
| **NetBSD**&nbsp;9.2 | **GCC**&nbsp;(7.5&nbsp;‑&nbsp;13.1),&nbsp; **Clang**&nbsp;(15.0.7) |
| **OpenBSD**&nbsp;7.3 | **GCC**&nbsp;(11.2),&nbsp; **Clang**&nbsp;(13) |
| **DragonFly**&nbsp;**BSD**&nbsp;6.4 | **GCC**&nbsp;(8.3&nbsp;‑&nbsp;13),&nbsp; **Clang**&nbsp;(10.0.1&nbsp;‑&nbsp;16.0.6) |
| **Haiku**&nbsp;R1b4 | **GCC**&nbsp;(11.2),&nbsp; **Clang**&nbsp;(12.0.1) |
| **Solaris**&nbsp;11.4 | **GCC**&nbsp;(10.3&nbsp;‑&nbsp;11.2),&nbsp; **Clang**&nbsp;(6&nbsp;‑&nbsp;11),&nbsp; **Oracle&nbsp;Studio&nbsp;C/C++**&nbsp;(≳12.6) |
| **illumos** | **GCC**&nbsp;(7.5&nbsp;‑&nbsp;11.3),&nbsp; **Clang**&nbsp;(15.0.7) |
| **SerenityOS** | **GCC**&nbsp;(13.1) |

libsir is known to work on most common (and many uncommon) architectures.
It has actually been built and tested on **Intel** (x86\_64, i686), **ARM** (ARMv6, ARMv7HF, ARMv8‑A), **POWER** (PowerPC, PPC64, PPC64le), **MIPS** (MIPS64, MIPS32, 74Kc), **z/Architecture** (S390X), **SuperH** (SH‑4A), **RISC‑V** (RV64), **OpenRISC** (OR1200), and **m68k** (68020+).
If it doesn't work on your machine, it's probably a bug.

## <a id="an-example" /> An example

Some sample terminal output from libsir demonstrating the default configuration:

![sample terminal output](./docs/res/sample-terminal.png)

This output is from the `example` application, whose source code can be located in the [example](https://github.com/aremmell/libsir/tree/master/example) directory. If you're curious about a basic implementation of libsir in a practical context, that's a good place to start.

## <a id="building-from-source" /> Building from source

There are several options if you would like to build libsir from source:

### <a id="visual-studio-code" /> Visual Studio Code

There is a `code‑workspace` in the repository. As of right now, the build and launch tasks are not fully functional for every platform, but everything else should be good to go.

### <a id="visual-studio-2022" /> Visual Studio 2022

A very recent addition is an `sln` and some `vcxproj` files in the [msvc](https://github.com/aremmell/libsir/tree/master/msvs) directory. They are confirmed to work correctly on Windows 11 (x64 &amp; Arm64) with Visual Studio 17.6.

### <a id="unix-makefile" /> Unix Makefile

The included *GNU* *Makefile* supports both native and cross-compilation on a wide variety of platforms using [**GNU Make**](https://www.gnu.org/software/make/) or [**Remake**](http://bashdb.sf.net/remake) version **3.81** (*or later*). On *macOS* systems, [**Apple Make**](https://github.com/apple-oss-distributions/gnumake) version **199** or later (derived from **GNU Make** version **3.81**) as bundled with [**Apple Xcode**](https://developer.apple.com/xcode/), is sufficient.

*Simply executing `make` will do the right thing for the vast majority of users on any supported platform.*

Developers, package builders, or advanced users should also review the [variables](#make-variables) and [options](#make-options) that influence the build.

#### <a id="make-targets" /> Make targets

|  <ins>**Build** **action**</ins> | <ins>***Make*** **target**</ins>  | <ins>**Output** **file**(**s**)</ins>          |
|---------------------------------:|:---------------------------------:|:-----------------------------------------------|
|                     **Clean up** |            `clean`                | <ul><li>*(removes build/\*)*</li></ul>         |
|                   **Test suite** |            `tests`                | <ul><li>`build/bin/sirtests(.exe)`</li></ul>   |
|                  **Example app** |            `example`              | <ul><li>`build/bin/sirexample(.exe)`</li></ul> |
|               **Static library** |            `static`               | <ul><li>`build/lib/libsir_s.(a,lib)`</li></ul> |
|               **Shared library** |            `shared`               | <ul><li>`build/lib/libsir.(so,dll)`</li></ul>  |
|                 **Installation** |            `install`              | <ul><li>`$PREFIX/lib/libsir_s.(a,lib)`</li><li>`$PREFIX/lib/libsir.(so,dll)`</li><li>`$PREFIX/include/sir.h`</li><li>`$PREFIX/include/sir/*.h`</li></ul> |

#### <a id="make-variables" /> Make variables

The following variables influence the *default build configuration*, and should be set in the shell environment when executing `make` (*e.g.* `env CC=clang make`). Some developers or users may wish to configure one or more of these options.

| <ins>**Environment** **Variable**</ins> | <ins>**Description**</ins>                                                                                                                    | <ins>**Default**</ins>                                        |
|----------------------------------------:|:---------------------------------------------------------------------------------------------------------------------------------------------:|:--------------------------------------------------------------|
| **`CC`**                                | Compiler driver front-end to execute for source code compilation and object linking                                                           | *System specific, usually `cc`*                               |
| **`CFLAGS`**                            | Default flags to be passed to the compiler driver during compilation                                                                          | *System specific, usually unset*                              |
| **`LDFLAGS`**                           | Default flags to be passed to the compiler driver during linking                                                                              | *System specific, usually unset*                              |
| **`LIBDL`**                             | Compiler driver flags to request [dynamic linking functions](https://pubs.opengroup.org/onlinepubs/9699919799/) be included during linking    | *System specific, usually `-ldl`*                             |
| **`NO_DEFAULT_CFLAGS`**                 | Prevents additional flags (*i.e.* `-Wall -Wextra -Wpedantic -std=c11 -Iinclude -fPIC`) from being appended to the user-supplied `CFLAGS`      | *`0` (disabled; use `1` to enable)*                           |
| **`FORTIFY_FLAGS`**                     | Compiler driver flags to request [function call fortifications](https://www.gnu.org/software/libc/manual/html_node/Source-Fortification.html) | *`-D_FORTIFY_SOURCE=2`                                        |
| **`LDCONFIG`**                          | Path to an `ldconfig` utility, used to index shared object names and refresh the dynamic linker cache, used during installation               | *`ldconfig`*                                                  |
| **`RANLIB`**                            | Path to an `ranlib` utility, used to index object names within a static archive, used after linking and during installation                   | *`ranlab`*                                                    |
| **`SIR_SHARED`**                        | Compiler driver flags to be passed to the compiler driver when linking a shared library                                                       | *`-qmkshrobj* for IBM XL C/C++ for AIX, *`-shared`* otherwise |

Review the [**`Makefile`**](Makefile) for additional details.

#### <a id="make-options" /> Make options

The following options influence the *compilation and installation of libsir*, and should be passed as arguments to `make` (*e.g.* `make install DESTDIR=$HOME/staging PREFIX=/usr` *or* `make SIR_NO_SYSTEM_LOGGERS=1`).  Most developers and users will never need to use any of these options.

| <ins>**Make** **Option**</ins> | <ins>**Description**</ins>                                                                                                                                                 | <ins>**Default**</ins>                                      |
|-------------------------------:|:---------------------------------------------------------------------------------------------------------------------------------------------------------------------------|:------------------------------------------------------------|
| **`SHELL`**                    | Path to a [*POSIX-conforming* shell interpreter](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html) to be used during compilation and installation | *Auto-detected, normally `/bin/sh`*                         |
| **`PREFIX`**                   | Path value used to construct the default destination paths used during installation                                                                                        | *`/usr/local`*                                              |
| **`DESTDIR`**                  | Path value used to support [staged installations](https://www.gnu.org/prep/standards/html_node/DESTDIR.html), normally used when building redistributable binary packages  | *(Unset)*                                                   |
| **`INSTALLSH`**                | Path to an *X11-compatible* `install` program to be used during installation                                                                                               | *`build-aux/install-sh`*                                    |
| **`SIR_DEBUG`**                | Configures the build system to produce a build with no optimizations enabled and full debugging symbols included&mdash;not intended for production                         | *`0` (disabled; use `1` to enable)*                         |
| **`SIR_SELFLOG`**              | Configures the build system to produce a build including internal diagnostic routines that report certain events that may aid in debugging and troubleshooting             | *`0` (disabled; use `1` to enable)*                         |
| **`SIR_ASSERT_ENABLED`**       | Configures the build system to produce a build that calls `assert`; in order for this to be useful, you will also need to `SIR_DEBUG=1`&mdash;not intended for production  | *`0` (disabled; use `1` to enable)*                         |
| **`SIR_NO_SYSTEM_LOGGERS`**    | Configures the build system to produce a build without support for any platform-specific logging facilities                                                                | *`0` (disabled; use `1` to enable)*                         |
| **`SIR_MSVCRT_MINGW`**         | Configures the build system allowing MSVCRT-specific [MinGW-w64](https://www.mingw-w64.org/) toolchains to produce binaries depending *only* on the *legacy* MSVC runtime  | *`0` (disabled; use `1` to enable)*                         |
| **`SIR_NO_PLUGINS`**           | Configures the build system to produce a build without support the dynamic loading and unloading of plugins                                                                | *`0` (disabled; use `1` to enable)*                         |
| **`MINGW`**                    | Configures the build system to use the [MinGW-w64](https://www.mingw-w64.org/) toolchain                                                                                   | *Auto-detected, normally `0` (disabled; use `1` to enable)* |
| **`HAIKU`**                    | Configures the build system to use the [Haiku](https://www.haiku-os.org/) toolchain                                                                                        | *Auto-detected, normally `0` (disabled; use `1` to enable)* |
| **`OPENBSD`**                  | Configures the build system to use the [OpenBSD](https://www.openbsd.org/) toolchain                                                                                       | *Auto-detected, normally `0` (disabled; use `1` to enable)* |
| **`NETBSD`**                   | Configures the build system to use the [NetBSD](https://www.netbsd.org/) toolchain                                                                                         | *Auto-detected, normally `0` (disabled; use `1` to enable)* |
| **`SUNPRO`**                   | Configures the build system to use the [Oracle Developer Studio](https://www.oracle.com/application-development/developerstudio/) toolchain                                | *Auto-detected, normally `0` (disabled; use `1` to enable)* |
| **`INTELC`**                   | Configures the build system to use the [(legacy) Intel C++ Compiler Classic](https://en.wikipedia.org/wiki/Intel_C%2B%2B_Compiler) toolchain                               | *Auto-detected, normally `0` (disabled; use `1` to enable)* |
| **`NVIDIAC`**                  | Configures the build system to use the [NVIDIA HPC SDK](https://developer.nvidia.com/hpc-sdk) (*or the legacy Portland Group PGI C/C++*) toolchain                         | *Auto-detected, normally `0` (disabled; use `1` to enable)* |
| **`IBMXLC`**                   | Configures the build system to use the [(legacy) IBM XL C/C++ for AIX](https://www.ibm.com/docs/en/xl-c-and-cpp-aix/16.1) toolchain                                        | *Auto-detected, normally `0` (disabled; use `1` to enable)* |

Review the [**`sirplatform.mk**`](sirplatform.mk) file for additional details.

## <a id="dig-in" /> Dig in

If you are genuinely interested in utilizing libsir, you are encouraged to read the [full online documentation](https://libsir.rml.dev) to get a better understanding of the library's capabilities and interfaces.

Suggested initial sections:

- [Public Functions](https://libsir.rml.dev/group__publicfuncs.html)
- [Public Types](https://libsir.rml.dev/group__publictypes.html)
- [Configuration](https://libsir.rml.dev/config.html)
- [Test suite](https://libsir.rml.dev/testsuite.html)
