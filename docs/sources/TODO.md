# TODO

Strikethrough = “Nice to have.”
Bold = “Priority”

- [ ] Bump version 2.2.0

- [x] **Add hostname to format options** (partially implemented; it’s no bueno to get the host name on every log call, so for now I will be stashing it on init, and perhaps doing a check to see if it needs an update every 30s or so), or there are these methods to know dynamically that it’s changed:

  - [ ] macOS
    - [ ] /Library/Preferences/SystemConfiguration/preferences.plist | Root->System->System->HostName : <https://developer.apple.com/library/archive/documentation/Darwin/Conceptual/FSEvents_ProgGuide/UsingtheFSEventsFramework/UsingtheFSEventsFramework.html>
    - [ ] <https://developer.apple.com/documentation/foundation/nsprocessinfo>
    - [ ] <https://developer.apple.com/documentation/systemconfiguration/1437828-scdynamicstorecreate>
  - [ ] Lunix: /etc/hostname
  - [ ] Windows: HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\ComputerName\ComputerName

- [ ] Figure out restrict keyword when using C++. Ditto for thread_local.

  - [ ] extern “C”

- [ ] **More functions that have lineage directly to sir.c need:**
  - [ ]  to be guarded with `_sir_sanity()`
  - [ ] clear last error on entry

- [ ] Add ‘policy’ flags to sirinit

  - [ ] Remove on write error? how many?
  - [ ] Roll size that defaults to SIR_FROLLSIZE but can be modified.

- [ ] ~~move timer functionality from tests into libsir itself~~

- [ ] Test adding existing file

- [ ] Test adding file we have no write perms on

- [ ] Calculate `SIR_MAXSTYLE` for 256-color and RGB modes, then ditch the old 16-color size.

- [ ] Conditional logging (debug_if(), etc.)

- [ ] Finish comment in sir_syslog_dest once it’s been resolved which option(s) can be utilized for os_log/syslog.

- [ ] Refactor _sir_syslog_init: it’s a clusterF. Should probably just reject initialization if levels are set but ident/cat are not.

- [ ] **Implement “last message repeated n times”**

- [ ] *Add a `--leave-logs` option to sirtests to leave the logs it generates behind so they can be examined for correctness.*

- [ ] Document

  - [ ] The remainder of sir.h

  - [ ] sirfilesystem.h

  - [ ] Put at least part of sirerrors.h in public, or a separate group

  - [ ] Put at least a brief in every file, or the file listing looks weird.

- [ ] Tests:
  - [ ] Invalid paths of all kinds:
    - [ ] Test file existence of files in directories we don’t have permission to examine and differentiate that from actually not existing
    - [ ] Symlinks

- [ ] **Implement 256-color and RGB.**

- [ ] Pull styling up into its own module for reusability in other projects?

- [ ] Add to Makefile and docs
  - [ ] SIR_ASSERT_ENABLED
  - [ ] SIR_NO_SYSTEM_LOGGERS

  - [ ] That there’s `syslog` and `os_log` support now

- [ ] Try again to figure out why in 2023 there’s no `stdatomic.h`support in VS2022… maybe clang will compile it?

- [ ] Move this to README.md, or create another .md and put it there:

  1 **A plugin system or public interface for registering custom adapters**, for things like:
  - Posting high-priority messages to a REST API endpoint.
  2 **Compressing archived logs with zlib or similar.**
    - Can we just use `logrotate`?
    - Can we rely on installed archivers like tar?
  3 **Deleting archived logs older than n days.**
  4 A project file for Xcode.
