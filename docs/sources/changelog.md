# Changelog

## 2.2.5

### TBA

- TBD

## 2.2.4

### December 23, 2023

- Add language bindings for Ch Professional and Python 3.
- No longer squelch repeating messages when sent to different levels.
- Add support for logging to the Windows Event Log.
- Add support for building as WebAssembly/JS with Emscripten.
- Add support for building via VSCode Tasks.
- Add `tags` target to Makefile.
- Add `sirtests++` test rig for testing the C++ wrapper.
- Add `sir_isinitialized` convenience function.
- Fix bug preventing rolling of `./`-prefixed log files with no extension.
- Add test for rolling `./`-prefixed log files with no extension.
- Add `sir_geterrorinfo` function for fine-grained error information.
- Add a helper script to rebuild all Docker CI images.
- Fix potential null pointer dereference in `_sir_getchar` on Windows.
- Fix compilation on AIX when `SIR_SELFLOG` is enabled.
- Add `_sir_nprocs` function to portably query processor count.
- Generate HTML cross-referenced source listing using GNU Global.
- Add in-place string mangling utilities:
  * `strreplace` - substring replacement
  * `strcreplace` - counting substring replacement
  * `strremove` - substring removal
  * `strsqueeze` - whitespace normalization
  * `strredact` - substring redaction
- Improve test runtime (by reducing test iterations).
- Add support for the Embarcadero Athens (RAD Studio 12, BCC 7.70) compiler.
- Allow overriding keyword for declaring thread local variables (`-D_sir_thread_local=`).
- Add `SIR_NO_SHARED` option to disable building or installing shared libraries.
- Add support for the CompCert-C and Chamois-CompCert compilers.
- Stricter REUSE3/SPDX compliance.
- Print thread IDs in hexadecimal format on NetBSD.
- Annotate shell scripts to appease ShellCheck.
- Add internal `strcasestr` implementation.
- Adjust compiler invocation in GitLab CI configuration.
- Improve CI testing on NetBSD and OpenBSD.
- Fix usage of automatic dependency information generated during the build.
- Update Doxygen to 1.9.8.
- Silence some warnings when building on IBM AIX using IBM XL C/C++.
- Update tooling and tested toolchains in documentation.
- General portability and performance improvements.
- General linter and static analyzer conformance improvements.

## 2.2.3

### September 8, 2023

- Fixed a bug that caused some log file messages to be truncated.
- Changed maximum message length to 4 KiB (4096 bytes).
- Added support for additional compilers and platforms.
- Added Oracle Lint to CI pipeline.
- General portability improvements.
- General linter and static analyzer conformance improvements.
- Now utilizes `stdatomic.h` under MSVC on version 17.4+ using `/experimental:c11atomics`.
- Added two additional tests.
- Optimized the primary code path for some *really impressive* performance gains. Some samples:

| Platform                 | Change vs. 2.2.2 |
| ------------------------ | ---------------- |
| macOS 13.5.2 (x86_64)    |  +361% ± 0.13﹪  |
| Arch 6.4.12 (x86_64)     |  +276% ± 0.07﹪  |
| RHEL 9 (x86_64)          |  +256% ± 0.03﹪  |
| Debian 12 (x86_64)       |  +217% ± 0.09﹪  |
| macOS 13.5.1 (AArch64)   |  +213% ± 0.12﹪  |
| Fedora 38 (x86_64)       |  +121% ± 0.05﹪  |
| AIX 7.3 (POWER9)         |  +119% ± 0.07﹪  |
| Debian 12 (AArch64)      |  +100% ± 0.04﹪  |
| Windows 11 (x64_64)      |   +87% ± 0.03﹪  |
| FreeBSD 13.2 (x86_x64)   |   +76% ± 0.06﹪  |
| Windows 11 (AArch64)     |   +67% ± 0.03﹪  |
| NetBSD 9.3 (ARM/A53)     |   +62% ± 0.18﹪  |
| OpenWrt 23 (MIPS32/74Kc) |    +6% ± 3.14﹪  |

## 2.2.2

### August 18, 2023

- Added support for
  - Embarcadero C++ for Windows
  - GNU/Hurd
  - Older glibc versions
- Updated graphics, fixed typos in Doxygen comments (updated docs)
- Fixed some MSVC analyzer warnings

## 2.2.1

### July 25, 2023

- Many small bug fixes and internal improvements.
- Fixed building for IBM AIX using IBM XL C/C++ V16.1.
- Fixed building for Haiku using Clang.
- Cleaned up all ‑Wmissing‑prototypes&nbsp;‑Wdouble‑promotion&nbsp;‑Wfloat‑conversion warnings.
- Debug builds now use `‑g3` on most platforms.
- Improved the coverage of the test suite.
- Added a new thread pool implementation (not yet used by the library).

## 2.2.0

### July 17, 2023

- Many new platforms and toolsets are now supported. See [the main page](@ref index) for details.
- [Plugin](@ref plugins) loader and sample plugin module implemented! In the near future, some standard plugins will be available as part of the library.
- Big performance gains through optimization and refactoring.
- Much improved support for syslog and added support for macOS' centralized logging facility (os_log). See ::sir_syslog_dest.
- Support for 256‑color and RGB color modes:
  - ::sir_setcolormode
  - ::sir_makergb
  - ::sir_settextstyle
- Windows got a lot of love, and support for that platform is better than ever
  - A Visual Studio 2022 solution can now be found in the [msvs](https://github.com/aremmell/libsir/tree/master/msvs) directory with both `x64` and `arm64` configurations
- New macros in [sir/ansimacros.h](https://github.com/aremmell/libsir/blob/master/include/sir/ansimacros.h) that can be used with libsir or anything
  that writes to stdio (*printf, puts, etc.*): `printf(RED("Oh lawd, something " ULINE("awful") " has happened!\n");`
- Test suite:
  - New and more vigorous tests
  - Now supports command‑line arguments:
    - `‑‑list` : lists all the available tests
    - `‑‑only`: only run the specified tests
    - `‑‑leave‑logs`: leave log files behind for examination
    - `‑‑wait` : wait for keypress before exiting
    - `‑‑perf` : runs the performance benchmark test (normally skipped)
    - `‑‑version`: shows the version of libsir the test suite was built with.
- Eliminated several bugs and potential memory leaks (*Flawfinder, Valgrind, PVS Studio, Clang‑analyzer, Cppcheck*)
- More specific error codes and better error reporting
- New preprocessor defines:
  - `SIR_ASSERT_ENABLED`: if defined, libsir calls `assert` (*note that if `‑DNDEBUG` is present, this becomes a NOOP*). If not defined and `SIR_SELFLOG=1`, libsir writes a red message to stderr containing the condition that would have asserted, along with the file, line, and function. This allows for fine‑grain control over assertions.
  - `SIR_NO_SYSTEM_LOGGERS`: if defined, system logger support is completely disabled/compiled out. The entry points still exist, but they will return an error that the feature is disabled or unavailable.
  - `SIR_NO_PLUGINS`: The same as `SIR_NO_SYSTEM_LOGGERS`, but for the plugin system.
- Better and more thorough documentation.

## 2.1.1

### May 17, 2023

- Fixed crash on macOS when handling OS errors
- Added FreeBSD support
- Fixed ‑Wstrict‑prototype, ‑Wformat warnings
- Fixed bug in Makefile (make clean was not deleting all intermediate files)
- Disallow root/sudo running the test suite; some of the tests depend on file permissions being unavailable.
