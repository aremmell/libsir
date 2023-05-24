# TODO

- [x] Run perf before and after using snprintf() in _sir_format instead of strncat

    1.) On Windows 11 x64:
    printf:  100000 lines in 10.67sec (9375.9 lines/sec)

    libsir(stdout): 100000 lines in 31.46sec (3178.5 lines/sec) w/ SIRO_NOMSEC. (3x slower than printf)
    libsir(stdout):  100000 lines in 51.21sec (1952.7 lines/sec) w/ SIRO_MSGONLY (5x slower than printf)
    libsir(stdout):  100000 lines in 31.44sec (3181.1 lines/sec) w/ SIRO_NOMSEC | SIRO_NOLEVEL. Figure out how this can possibly be way faster than SIRO_MSGONLY.

    Surprisingly, snprintf was somewhat slower than strncat; even reusing components of the last message that haven’t changed did not affect the perf in any meaningful way. It looks like the bottleneck is `vsnprintf`, and I’m going to have to find a way to:

    a.) predict its output and determine if it’s the same message I’m about to print or
    b.) find something to replace it.\

- [ ] Use profiler. At least implement binary search for `_sir_getprivstyle` and anything else that does a for 0 .. size loop

- [ ] When adding log files, they go into the working directory; not necessarily the directory of the executable:
         ~~1.) Add to the documentation that this is the case, unless the file is prefixed with “./”–automatically prefix it;~~
         2.) Look for slashes in the path leading up to the filename–assume it is an absolute path;
         3.) <u>Use the `get_working_directory` function from in `tests/cwd.c` if “./” doesn’t work everywhere.</u>

- [x] MSVS: Change output files to somewhere under `build/bin`

- [x] Don’t forget to remove TODO #4 in tests/tests.h

- [ ] Implement “last message repeated n times”

- [ ] Tests: add some more tests, including:

    - [ ] Super long long file name/path
    
- [ ] You gotta be jokin’ my ass:

    - [ ] [Console Virtual Terminal Sequences - Windows Console](https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences)
    - [ ] [SetConsoleMode function - Windows Console](https://learn.microsoft.com/en-us/windows/console/setconsolemode)
