# TODO

- [ ] Bump version 2.2.0
- [ ] Add hostname to format options
- [ ] More functions that have lineage directly to sir.c need:
  - [ ]  to be guarded with `_sir_sanity()`
  - [ ] clear last error on entry

- [ ] move timer functionality from tests into libsir itself
- [ ] `getopt?`
- [ ] Calculate `SIR_MAXSTYLE` for 256-color and RGB modes, then ditch the old 16-color size.
- [ ] Conditional logging (debug_if(), etc.)
- [ ] Implement `clang-format` (see [here](https://github.com/nullromo/doxygen-example/blob/main/.clang-format))
- [ ] Normalize `/* !_DEF */` / `// !_DEF`
- [ ] Finish comment in sir_syslog_dest once it’s been resolved which option(s) can be utilized for os_log/syslog.
- [ ] Refactor _sir_syslog_init: it’s a clusterF. Should probably just reject initialization if levels are set but ident/cat are not.
  - [ ] Rename sirinit::processName
- [ ] Implement “last message repeated n times”
- [ ] Move documentation from `sir.h` to `sir.c`
  - [x] Remove all internal documentation / stash it away better
  - [ ] Include sir_geterror in docs somehow without including errors.h
- [ ] sir_syslogopts
  - [ ] Document
  - [ ] Add tests
- [ ] *A single call to update one or more properties;  separating them is creating clutter, redundancy, and unnecessary performance losses (by reconfiguring on each call)*
- [ ] *Add a `--leave-logs` option to sirtests to leave the logs it generates behind so they can be examined for correctness.*
- [ ] Document sirfilesystem.h
- [ ] Tests:
  - [ ] Roll a log file w/ no extension
  - [x] Better level/option validation testing
  - [ ] init w/ bad / uninitialized config
  - [ ] Filesystem
    - [ ] Test too-small buffer on every platform (getappfilename)
      - [ ] macOS
      - [ ] Windows
      - [ ] Linux
      - [ ] FreeBSD
    - [ ] Path as long as can fit in the PATH_MAX/MAX_PATH
    - [ ] Invalid paths of all kinds:
      - [x] Malformed
      - [ ] Deleted/moved from underneath the operations
      - [x] Invalid characters
      - [ ] Test file existence of files in directories we don’t have permission to examine and differentiate that from actually not existing
      - [x] “C:\” and “/”
      - [x] Mix-and-match slashes on windows
      - [ ] Symlinks
- [ ] Go through sirfilecache and replace any code possible with the new filesystem calls
  - [ ] Implement 256-color and RGB.
  - [ ] Pull styling up into its own module for reusability in other projects?
- [ ] New preprocessor defines–add to Makefile and docs
  - [ ] SIR_ASSERT_ENABLED
  - [ ] SIR_NO_SYSTEM_LOGGERS
  - [ ] On platforms != windows (that have syslog):
    - [ ] Disable syslog at initialization time:

    - [ ] Disable os_log and syslog at initialization time: **SIR_NO_SYSTEM_LOGGERS**
- [ ] Add to index.md (and rebuild Docs):
  - [ ] Check for unresolved :: and @ref and \a and \p etc, and normalize the usage.
  - [ ] That there’s `syslog` and `os_log` support now
- [ ] Try again to figure out why in 2023 there’s no `stdatomic.h`support in VS2022… maybe clang will compile it?
- [ ] Move this to README.md, or create another .md and put it there:

 1. A plugin system or public interface for registering custom adapters, for things like:
    - Posting high-priority messages to a REST API endpoint.
 2. Compressing archived logs with zlib or similar.
 3. Deleting archived logs older than n days.
 4. A project file for Xcode.
