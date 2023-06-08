# TODO

- [ ] Bump version 2.2.0
- [ ] Finish comment in sir_syslog_dest once it’s been resolved which option(s) can be utilized for os_log/syslog.
- [ ] Refactor _sir_syslog_init: it’s a clusterF. Should probably just reject initialization if levels are set but ident/cat are not.
- [ ] Implement “last message repeated n times”
- [ ] Move documentation from `sir.h` to `sir.c`
  - [ ] Remove all internal documentation / stash it away better
- [ ] sir_syslogopts
  - [ ] Document
  - [ ] Add tests
- [ ] What we really need is a single call to update one or more properties;  separating them is creating clutter, redundancy, and unnecessary performance losses (by reconfiguring on each call)
- [ ] Add a `--leave-logs` option to sirtests to leave the logs it generates behind so they can be examined for correctness.
- [ ] Document sirfilesystem.h
- [ ] Tests:
  - [x] Better level/option validation testing
  - [ ] init w/ bad / uninitialized config
  - [ ] Filesystem
    - [ ] Path as long as can fit in the PATH_MAX/MAX_PATH
    - [ ] Invalid paths of all kinds:
      - [x] Malformed
      - [ ] Deleted/moved from underneath the operations
      - [x] Invalid characters
      - [ ] Test file existence of files in directories we don’t have permission to examine and differentiate that from actually not existing
      - [x] “C:\” and “/”
      - [ ] Mix-and-match slashes on windows
      - [ ] Symlinks
  - [ ] filesystem implementation
- [ ] Go through sirfilecache and replace any code possible with the new filesystem calls
  - [ ] Implement 256-color and RGB.
  - [ ] Pull styling up into its own module for reusability in other projects
- [ ] New preprocessor defines–add to Makefile and docs
  - [ ] SIR_ASSERT_ENABLED

  - [ ] SIR_NO_SYSTEM_LOGGERS

  - [ ] On platforms != windows (that have syslog):
    - [ ] Disable syslog at initialization time:

    - [ ] Disable os_log and syslog at initialization time: **SIR_NO_SYSTEM_LOGGERS**

  - [ ] Conditional logging (debug_if(), etc.)
- [ ] Add to index.md (and rebuild Docs):
  - [ ] Check for unresolved :: and @ref and \a and \p etc, and normalize the usage.
  - [ ] That there’s `syslog` and `os_log` support now
- [ ] Try again to figure out why in 2023 there’s no `stdatomic.h`support in VS2022… maybe clang will compile it?
