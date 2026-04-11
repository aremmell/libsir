# Configuration         {#config}

## Build-time Configuration

The preprocessor macros in the following table are evaluated at build-time when compiling libsir from source. The libsir Makefile will automatically self-configure based on the absence or preseence of each variable known to the Makefile. Compiler and linker options are massaged, and preprocessor macros will affect the code generated, depending on the values provided: either in variables set explicitly as part of the make command (e.g., `env MY_VAR=1 make ...`), or previously defined variables in the current shell's environment.

The following are the most common libsir preprocessor macros, as well as a description of the effect of each variable upon the build process for each. All variables have a default value of zero (`0`):

| Variable Name        | Default Behavior | Behavior When Set to one (`1`) |
| :----------------------------- | :--------------------------------- | :--------------------------------- |
|     `SIR_DEBUG`            | `-g3 -00 -DDEBUG -U_FORTIFY_SOURCE` : No optimizations are applied, source fortification is disabled, and full debugging symbols are included. | `-DSIR_DEBUG=1 -O3 -DNDEBUG -D_FORTIFY_SOURCE=2` : Full optimization is applied; source fortification is set to level 2, and no debugging symbols are included. |
|    `SIR_SELFLOG`           | Internal diagnostics are not included in the build. | `-DSIR_SELFLOG=1` : Diagnostic information is sent to stderr to report certain events that may aid in debugging. Includes the function name, source file, and line where the event took place. If `SIR_DEBUG` and `SIR_SELFLOG` are enabled, each error handled internally will be logged in real time as it is captured. |
| `SIR_ASSERT_ENABLE`           | `assert` will never be called. If `SIR_SELFLOG` is set to one (`1`), where asserts normally would take place, libsir will instead produce output to stderr such as '`somefunc (file.c:123): !!! would be asserting (NULL != ptr)`'. | `-DSIR_ASSERT_ENABLED=1` : `assert` will be used. Note that assert has no effect if `NDEBUG` is defined, so in order for this to be useful, you will also need `-DSIR_DEBUG=1` (_or manually change `-DNDEBUG` to `-DDEBUG` in the Makefile_). |
|    `SIR_NO_SYSTEM_LOGGERS`       | If the current platform has a system logger facility (_virtually all platforms do by default, but on Windows, you must execute `msvs/compile_man.ps1` as Administrator before compiling libsir in order to use the Windows Event Viewer functionality_), you can utilize it as a destination in libsir. | `-DSIR_NO_SYSTEM_LOGGERS` : Even if the current platform has a system logger facility, the functionality will be disabled (_and most of it compiled out_). |
|    `SIR_NO_PLUGINS`           | The plugin system is available for use. Call ::sir_loadplugin to load a plugin, and ::sir_unloadplugin to unload one. | `-DSIR_NO_PLUGINS=1` : The plugin system's functionality will be disabled (_and most of it compiled out_). |
|    `SIR_NO_SHARED`            | Shared libraries are created when building the *`all`* target, and installed with *`make install`*. | `-DSIR_NO_PLUGINS=1 -DSIR_NO_SHARED=1` : Shared libraries are not created when building the *`all`* target, and are not installed with *`make install`*. |
|    `SIR_USE_EOL_CRLF`         | The end of line sequence will be `SIR_EOL_LF`. | `-DSIR_USE_EOL_CRLF=1` : The end of line sequence will be `SIR_EOL_CR` followed by `SIR_EOL_LF`. |

---

@note If you plan to use the Visual Studio 2022 solution, see `README.md` under `Help` once the solution is loaded.

## sir/config.h

All of the fine-grained compile-time constants, such as (but not limited to):

1. Date/time format strings
2. Default values
3. Miscellaneous format strings
4. Buffer sizes
5. Thresholds

can be found in [config.h](https://github.com/aremmell/libsir/blob/master/include/sir/config.h), and may be modified to fit your specific needs.

@remark All format strings containing specifiers such as `%%d` are `printf`-style. The man page or documentation for `printf` can be used as a guide.

@attention Don't make changes until you've examined how these values are utilized elsewhere in the source code! You _will_ break something.
