# Changelog

## 2.2.0

### July 17, 2023

- Many new platforms and toolsets are now supported. See [the main page](https://libsir.rml.dev) for details.
- Plugin loader and sample plugin module implemented! This gives libsir virtually unlimited potential, and in the near future, some standard plugins will be available as part of the library. See documentation for `sir_loadplugin` and `sir_unloadplugin`. The sample plugin is located at `plugins/sample/sample_plugin.c`
- *Big performance gains* through optimization and refactoring *(estimated 30% faster than the previous version–try the perf test for yourself)*
- *Much improved* support for syslog, if available, and added support for macOS' centralized logging facility (os_log).
- Support for 256-color and RGB color modes.
- Windows got a lot of love, and support for that platform is better than ever
  - A Visual Studio 2022 solution can now be found in the [msvs](https://github.com/aremmell/libsir/tree/master/msvs) directory with both `x64` and `arm64` configurations
- New macros in [siransimacros.h](https://github.com/aremmell/libsir/blob/master/include/sir/ansimacros.h) that can be used with libsir or anything
  that writes to stdio *(printf, puts, etc.)*: `printf(RED("Oh lawd, something " ULINE("awful") " has happened!\n");`
- Test suite:
  - New and more vigorous tests
  - Now supports command-line arguments:
    - `--list` : lists all the available tests
    - `--only`: only run the specified tests
    - `--leave-logs`: leave log files behind for examination
    - `--wait` : wait for keypress before exiting
    - `--perf` : runs the performance benchmark test (normally skipped)
- Eliminated several bugs
- Potential memory leaks squashed
- More specific error codes and better error reporting
- New preprocessor defines:
  - `SIR_ASSERT_ENABLED`: if defined, libsir calls `assert` (*note that if `-DNDEBUG` is present, this becomes a NOOP*). If not defined and `SIR_SELFLOG=1`, libsir writes a red message to stderr containing the condition that would have asserted, along with the file, line, and function. This allows for fine-grain control over assertions.
  - `SIR_NO_SYSTEM_LOGGERS`: if defined, system logger support is completely disabled/compiled out. The entry points still exist, but they will return an error that the feature is disabled or unavailable.
  - `SIR_NO_PLUGINS`: The same as `SIR_NO_SYSTEM_LOGGERS`, but for the plugin system.
- Better and more thorough documentation.
