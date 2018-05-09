# Etcetera

## Hardening and Compatibility

I have only compiled and tested libsir on the following platforms using `gcc`.

- Ubuntu 16.04 x64
- Windows 10 x64

Other platforms, such as `BSD` and `macOS` remain untested, and probably won't even compile cleanly.

## Nice to have

1. **A plugin system or public interface for registering custom adapters, for things like:**
  * Posting high-priority messages to a REST API endpoint.
  * Sending high-prirority messages via SMS or push notification.
2. **Compressing archived logs with zlib or similar.**
3. **Deleting archived logs older than _n_ days.**
4. **A project file for Visual Studio.**
5. **A project file for Xcode.**
6. **An accompanying C++ wrapper.**
7. **Something I didn't think of yet.**