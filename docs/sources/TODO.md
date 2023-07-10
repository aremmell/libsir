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
- [ ] Add ‘policy’ flags to sirinit

  - [ ] Remove on write error? how many?
  - [ ] Roll size that defaults to SIR_FROLLSIZE but can be modified.
- [ ] Add emphasis, underline, etc. to SIRS_
- [ ] Test adding existing file
- [ ] Conditional logging (debug_if(), etc.)
- [ ] Finish comment in sir_syslog_dest once it’s been resolved which option(s) can be utilized for os_log/syslog.
- [ ] **Implement “last message repeated n times”**
- [ ] *Add a `--leave-logs` option to sirtests to leave the logs it generates behind so they can be examined for correctness.*
- [ ] Document

  - [ ] Put at least a brief in every file, or the file listing looks weird.
- [ ] Tests:
  - [ ] Invalid paths of all kinds:
    - [ ] Test file existence of files in directories we don’t have permission to examine and differentiate that from actually not existing
    - [ ] Symlinks
- [ ] **Implement 256-color and RGB.**
  - [ ] Calculate `SIR_MAXSTYLE` for 256-color and RGB modes, then ditch the old 16-color size.
- [ ] Pull styling up into its own module for reusability in other projects?
- [ ] Try again to figure out why in 2023 there’s no `stdatomic.h`support in VS2022… maybe clang will compile it?
- [ ] **A plugin system or public interface for registering custom adapter**, for things like:
  - [ ] Posting high-priority messages to a REST API endpoint.
  - [ ] Compressing archived logs with zlib or similar.
  - [ ] Can we just use `logrotate`?
  - [ ] Can we rely on installed archivers like tar?
  - [ ] Deleting archived logs older than n days.
- [ ] A project file for Xcode.
