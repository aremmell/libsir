# TODO

- [ ] Keep woring on perf–run in VS profiler w/ debug symbols.
- [ ]  When adding log files, they go into the working directory; not necessarily the directory of the executable
  - [ ]  Add to the documentation that this is the case.
  - [ ] Look for slashes in the path leading up to the filename–does it start with `/` or an ASCII letter and `:\`;
  - [ ]  <u>Use the `get_working_directory` function from in `tests/cwd.c`</u>

- [ ] Write the following  functions:
  - [x] Grab the `get_working_directory` from `tests/cwd.c` -> it’s now called `_sir_getcwd()` in `sirhelpers.h`.
  - [ ] `path_compare`
  - [ ] `path_split` (path/filename/extension)
  - [ ] `get_program_directory`: returns the directory that the binary is located in, not the working directory.

- [ ] https://en.cppreference.com/w/c/error/set_constraint_handler_s
  
- [ ] Implement “last message repeated n times”
- [ ] Tests: add some more tests, including:
  - [x] Better level/option validation testing
  - [ ] Super long long file name/path
- [ ] You gotta be jokin’ my ass:

  - [ ] [Console Virtual Terminal Sequences - Windows Console](https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences)
  - [ ] [SetConsoleMode function - Windows Console](https://learn.microsoft.com/en-us/windows/console/setconsolemode)
  - [ ] If Windows can really do escape sequences, implement that, and 256, R,G,B coloring.
  - [ ] Pull styling up into its own module for reusability in other projects
- [ ] Add a note about SIR_ASSERT_ON_ERROR, and add it to the makefile.
- [ ] Add a note about sir.hh and the C++ wrapper/boost::format adapter
- [ ] Conditional logging (debug_if(), etc.)
