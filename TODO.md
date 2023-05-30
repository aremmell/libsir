# TODO

- [ ] BUG: SIRO_ALL is not effectively SIRO_MSGONLY; it’s the opposite. It should be called SIRO_NONE
- [ ] BUG: if NO_TIME and NO_NAME are set, but TID and PID are present, there’s no space between the message
- [ ] Implement “last message repeated n times”
- [ ] *sir_syslogcat* to update the category
- [ ] *sir_syslogid* to update the identity
    - [ ] Document
    - [ ] Add tests
- [ ] Document sirfilesystem.h
- [ ] Tests:
  - [ ] Better level/option validation testing
  - [ ] Filesystem
    - [ ] Path as long as can fit in the PATH_MAX/MAX_PATH
    - [ ] Invalid paths of all kinds:
      - [ ] Malformed
      - [ ] Deleted/moved from underneath the operations
      - [ ] Invalid characters
      - [ ] Test file existence of files in directories we don’t have permission to examine and differentiate that from actually not existing
      - [ ] “C:\” and “/”
      - [ ] Mix-and-match slashes on windows
      - [ ] Symlinks
  - [ ] filesystem implementation
- [ ] Go through sirfilecache and replace any code possible with the new filesystem calls
  - [ ] Implement 256-color and RGB.
  - [ ] Pull styling up into its own module for reusability in other projects
- [ ] New preprocessor defines–add to Makefile and docs
  - [ ] SIR_ASSERT_ON_ERROR

  - [ ] SIR_NO_SYSTEM_LOGGERS

  - [ ] On platforms != windows (that have syslog):
    - [ ] Disable syslog at initialization time:

    - [ ] Disable os_log and syslog at initialization time: **SIR_NO_SYSTEM_LOGGERS**

  - [ ] Conditional logging (debug_if(), etc.)
- [ ] Add to index.md (and rebuild Docs):
  - [ ] Check for unresolved :: and @ref and \a and \p etc, and normalize the usage.
  - [x] That there’s a VS2022 solution and projects now
  - [ ] That there’s a C++ header w/ boost::format adapter (or delete it and start over)
  - [ ] That there’s `syslog` and `os_log` support now
- [ ] Try again to figure out why in 2023 there’s no `stdatomic.h`support in VS2022… maybe clang will compile it?
- [ ] Go through sir.hh and write Doxygen comments before rebuilding docs (or delete it and start over)
  - [x] Search for “SIR” and replace with “libsir”
  - [x] Same for “SIR_NO_SYSTEM_LOGGERS”
