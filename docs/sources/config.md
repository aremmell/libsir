Configuration         {#config}
=============

Compile-time configuration of libsir.

Preprocessor macros
-------------------

The following are applied at compile-time when executing `make`:

```sh
env SIR_PREPROCESSOR_FOO=1 make ...
```

The Makefile takes care of the rest; you may also feel free to edit the Makefile
to suit your needs, avoiding this step entirely.

Available preprocessor macros:

| Macro (default)      | State | Compiler settings            | Description |
| :------------------: | :---: | :--------------------------: | :---------- |
| `SIR_DEBUG (0)`      | `0`   | `-O3 -DNDEBUG`               | Full optimization is applied, and no debugging symbols are included. |
| ^                    | `1`   | `-g -00 -DDEBUG`             | No optimizations are applied, and debugging symbols are included. |
| `SIR_SELFLOG (0)`    | `0`   | `N/A`                        | NOOP |
| ^                    | `1`   | `-DSIR_SELFLOG`              | Diagnostic information is sent to `stderr` to report certain events that may aid in debugging. Includes the function name, source file, and line where the event took place. If `SIR_DEBUG` and `SIR_SELFLOG` are enabled, each error handled internally will be logged in real time as it is captured. |
| `SIR_ASSERT_ENABLED (0)` | `0` | `N/A` | `assert` will never be called. If `SIR_SELFLOG` is enabled, where asserts normally would take place, a `_sir_selflog` call is made, producing output to stderr such as '`somefunc (file.c:123): !!! would be asserting (NULL != ptr)`'. |
| ^                    | `1`   | `-DSIR_ASSERT_ENABLED`       | `assert` will be used. Note that assert has no effect if `NDEBUG` is defined, so in order for this to be useful, you will also need `SIR_DEBUG=1` (_or manually add `-DNDEBUG` in the Makefile_). |
| `SIR_NO_SYSTEM_LOGGERS (0)` | `0` | `N/A`                   | If the current platform has a system logger facility (_currently all platforms do by default except Windows_), you can utilize it as a destination in libsir. |
| ^ | `1`     | `-DSIR_NO_SYSTEM_LOGGERS`                     | Even if the current platform has a system logger facility, the functionality will be disabled (_and most of it compiled out_). |
| `SIR_NO_PLUGINS (0)` | `0`   | `N/A`                        | The plugin system is available for use. Call ::sir_loadplugin to load a plugin, and ::sir_unloadplugin to unload one.
| ^                    | `1`   | `-DSIR_NO_PLUGINS`           | The plugin system's functionality will be disabled (_and most of it compiled out_).
---

@note If you plan to use the Visual Studio solution, see `README.md` under `Help` once the solution is loaded.

sirconfig.h
-----------

All of the fine-grained compile-time constants, such as (but not limited to):

1. Date/time format strings
2. Default values
3. Miscellaneous format strings
4. Buffer sizes
5. Thresholds

can be found in [sirconfig.h](https://github.com/aremmell/libsir/blob/master/sirconfig.h), and may be modified to fit your specific needs.

@remark All format strings containing specifiers such as `%%d` are `printf`-style. The man page or documentation for `printf` can be used as a guide.

@attention Don't make changes until you've examined how these values are utilized elsewhere in the source code! You _will_ break something.
