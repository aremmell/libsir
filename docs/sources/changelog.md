# Changelog

## 2.2.0

### July 15, 2023

- *Vastly improved* support for syslog, if available, and macOS' system logger (os_log) as their own independent logging destination.
- Support for 256-color and RGB color modes.
- Many new platforms and toolsets are now supported. See [the main page](https://libsir.rml.dev) for details.
- <u>Massive performance gains</u> for both stdio and log files via optimization/refactoring (estimated 30%+)
- *Lots* of work to get things going smoothly on Windows
  - A Visual Studio 2022 solution in `msvs`
- New macros in siransimacros.h that can be used with libsir or anything
  that writes to stdio (*printf, puts, etc.): `printf(RED("Oh lawd, something " ULINE("awful") " has happened!\n");`
- Many new/more vigorous tests
- The test rig now supports command-line arguments:
  - `--list` : lists all the available tests
  - `--only`: only run the specified tests
  - `--leave-logs`: leave log files behind for examination
  - `--wait` : wait for keypress before exiting
  - `--perf` : runs the performance benchmark test (normally skipped)
- Bug fixes
- Potential memory leaks squashed
- Additional errors with better error reporting
- New preprocessor defines:
  - `SIR_ASSERT_ENABLED`: if defined, libsir actually uses assert() (*if `-DNDEBUG` is present, this means nothing*), if not defined, `_sir_selflog` outputs a red message containing the condition that would have asserted, along with the file/line/func (*if `-DSIR_SELFLOG` is present, otherwise it’s a NOOP*).
  - `SIR_NO_SYSTEM_LOGGERS`: if defined, system logger support is completely disabled/not compiled in. The entry points still exist, but they will return an error that the feature is disabled or unavailable.
- rename `thread_local` – this is a C++ keyword
- renamed all typedefs ending in `_t`
- Better and more thorough documentation.
