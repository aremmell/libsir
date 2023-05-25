# TODO

Perf: Made huge improvement by using binary search when looking up styles, etc. (~30s -> 9sf)!

```
printf:  1000000 lines in 9.00sec (111091.3 lines/sec)
```
> Surprisingly, snprintf was somewhat slower than strncat; even reusing components of the last message that haven’t changed did not affect the perf in any meaningful way. It looks like the bottleneck is `vsnprintf`, and I’m going to have to find a way to:
> a.) predict its output and determine if it’s the same message I’m about to print or
> b.) find something to replace it.

- [ ] When adding log files, they go into the working directory; not necessarily the directory of the executable:
         1.) Add to the documentation that this is the case, unless the file is prefixed with “./”–automatically prefix it;~~
         2.) Look for slashes in the path leading up to the filename–assume it is an absolute path;
         3.) <u>Use the `get_working_directory` function from in `tests/cwd.c` if “./” doesn’t work everywhere.</u>

- [ ] Implement “last message repeated n times”

- [ ] Tests: add some more tests, including:
  - [x] Better level/option validation testing
  - [ ] Super long long file name/path

- [ ] You gotta be jokin’ my ass:

  - [ ] [Console Virtual Terminal Sequences - Windows Console](https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences)
  - [ ] [SetConsoleMode function - Windows Console](https://learn.microsoft.com/en-us/windows/console/setconsolemode)
  - [ ] If Windows can really do escape sequences, implement that, and 256, R,G,B coloring.
  - [ ] Pull styling up into its own module for reusability in other projects

- [ ] Manually test sirtests –wait
  - [x] Ubuntu
  - [x] Fedora
  - [x] FreeBSD

- [ ] Add a note about SIR_ASSERT_ON_ERROR, and add it to the makefile.

- [ ] Conditional logging (debug_if(), etc.)

