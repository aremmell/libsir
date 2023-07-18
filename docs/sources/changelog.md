# Changelog

## 2.2.0

### July 17, 2023

- Many new platforms and toolsets are now supported. See [the main page](@ref index) for details.
- [Plugin](@ref plugins) loader and sample plugin module implemented! In the near future, some standard plugins will be available as part of the library.
- Big performance gains through optimization and refactoring.
- Much improved support for syslog and added support for macOS' centralized logging facility (os_log). See ::sir_syslog_dest.
- Support for 256-color and RGB color modes:
  - ::sir_setcolormode
  - ::sir_makergb
  - ::sir_settextstyle
- Windows got a lot of love, and support for that platform is better than ever
  - A Visual Studio 2022 solution can now be found in the [msvs](https://github.com/aremmell/libsir/tree/master/msvs) directory with both `x64` and `arm64` configurations
- New macros in [sir/ansimacros.h](https://github.com/aremmell/libsir/blob/master/include/sir/ansimacros.h) that can be used with libsir or anything
  that writes to stdio (*printf, puts, etc.*): `printf(RED("Oh lawd, something " ULINE("awful") " has happened!\n");`
- Test suite:
  - New and more vigorous tests
  - Now supports command-line arguments:
    - `--list` : lists all the available tests
    - `--only`: only run the specified tests
    - `--leave-logs`: leave log files behind for examination
    - `--wait` : wait for keypress before exiting
    - `--perf` : runs the performance benchmark test (normally skipped)
    - `--version`: shows the version of libsir the test suite was built with.
- Eliminated several bugs and potential memory leaks (*Flawfinder, Valgrind, PVS Studio, clang-analyzer, cppcheck*)
- More specific error codes and better error reporting
- New preprocessor defines:
  - `SIR_ASSERT_ENABLED`: if defined, libsir calls `assert` (*note that if `-DNDEBUG` is present, this becomes a NOOP*). If not defined and `SIR_SELFLOG=1`, libsir writes a red message to stderr containing the condition that would have asserted, along with the file, line, and function. This allows for fine-grain control over assertions.
  - `SIR_NO_SYSTEM_LOGGERS`: if defined, system logger support is completely disabled/compiled out. The entry points still exist, but they will return an error that the feature is disabled or unavailable.
  - `SIR_NO_PLUGINS`: The same as `SIR_NO_SYSTEM_LOGGERS`, but for the plugin system.
- Better and more thorough documentation.

---

## 2.1.1

### May 17, 2023

- Fixed crash on macOS when handling OS errors
- Added FreeBSD support
- Fixed -Wstrict-prototype, -Wformat warnings
- Fixed bug in Makefile (make clean was not deleting all intermediate files)
- Disallow root/sudo running the test suite; some of the tests depend on file permissions being unavailable.
