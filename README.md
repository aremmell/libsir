# SIR
Standard Incident Reporter; simple logging to file, debug and stdout/stderr

> NOTE: This code is ancient; I wrote it in 2003. It may not compile at all. I have placed it here for posterity.

## Description
Sir is a cross-platform ANSI C library that provides developers with much needed, usually repetitive functionality. Any application of moderate size or complexity requires program output to be sent to various locations, namely the console, log files, a debug window, or GUI.  Writing the code to do this is painfully repetitious. Enter Sir.

By adding sir to your application's code base, you can perform information output tasks in no time flat.  Sir provides functionality that allows you to easily send formatted output to one or more of these destinations simultaneously with one function call:

* stdout/stderr
* Log files
* Callback functions
* Debug (Windows)

Sir has the capability to prepend output with a standard format timestamp (time()/ctime()), and append carriage return/line feed.  Also, Sir may be implemented in C++ applications.

### Types and return values
Sir uses the type TCHAR frequently. This type is simply either an 8-bit character when the program being built is ANSI, or a 16-bit character when the program is in UNICODE mode. In addition, Sir makes use of the macro _T, which, on Windows if UNICODE is being used, transforms string literals to L"String". If you're not using Sir on Windows, you do not need to worry about the _T macro.

All Sir functions return type int unless otherwise specified.  The success code is zero, and if an error occurs, -1 will be returned.

### Implementation
Implementing Sir in your application is extremely easy. I have provided it in various forms, including source code, compiled static libraries (debug/release, ANSI/UNICODE builds), and a makefile for a shared library. The following section will show example usage.

### Example
```C
#include "sir.h"

/*
  Sir type callback function
*/
void SirCallback(TCHAR *out, u_long data);

int main(int argc, TCHAR *argv[]) {
  SIRSTRUCT ss = {0}; /* Declare the Sir initialization structure */

/*
  Output of types SIRT_LOG and SIRT_WARNING
  will be sent to stdout.
 */
  ss.f_stdout = SIRT_LOG | SIRT_WARNING;

/*
  Output of type SIRT_WARNING will be sent
  to debug.
 */
  ss.f_debug = SIRT_WARNING;

/*
  Output of types SIRT_ERROR and SIRT_FATAL
  will be sent to stderr.
 */
  ss.f_stderr = SIRT_ERROR | SIRT_FATAL;

/*
  Set the option bits (Sir.h);
  SIRO_CRLF = \r\n will be appended to output,
  SIRO_TIMESTAMP = Time will be prepended to output,
  SIRO_FILES = Sir will handle file output,
  SIRO_CALLBACKS = Sir will handle callback output
 */
  ss.opts = SIRO_CRLF | SIRO_TIMESTAMP | SIRO_FILES | SIRO_CALLBACKS;

/*
  Initialize the Sir system.
 */

  if(0 != Sir_Init(&ss)) {
    printf("Failed to initialize the Sir system!\n");
    return 1;
  } else {
    printf("Successfully initialized the Sir system.\n");
  }

/*
  Add a file for output of types SIRT_WARNING and SIRT_LOG.
 */
  if(0 != Sir_AddFile(_T("sir.log"), SIRT_WARNING | SIRT_LOG)) {
    printf("Failed to add file to Sir system!\n");
  } else {
    printf("Successfully associated 'sir.log' with SIRT_WARNING"
           " and SIRT_LOG output types.\n");
  }

/*
  Add a callback for output types of SIRT_SCREEN.
 */
  if(0 != Sir_AddCallback(SirCallback, SIRT_SCREEN, 0U)) {
    printf("Failed to add callback to Sir system!\n");
  } else {
    printf("Successfully associated SirCallback() with SIRT_SCREEN"
           " output type.\n");
  }

/*
  Send output to all its destinations with one call!
 */

/*
  Sent to all destinations associated with type SIRT_WARNING or SIRT_LOG
  (or both).
 */
  Sir(SIRT_WARNING | SIRT_LOG, _T("This is a test of the Sir system. This is")
      _T(" only a test. %d, %d, %d"), 1, 2, 3);

/*
  Sent to all destinations associated with type SIRT_ERROR or SIRT_SCREEN
  (or both).
 */
  Sir(SIRT_ERROR | SIRT_SCREEN, _T("This is a continuation of the Sir system")
      _T(" test."));

/*
  Remove the file and callback (disassociates them with output types).
 */
  Sir_RemFile(_T("sir.log"));

  Sir_RemCallback (SirCallback);

/*
  Free allocated resources and reset
  the system's state
 */

  Sir_Cleanup ();

  return 0;
}
```

### Constants
#### Options
* `SIRO_TIMESTAMP` : Output will be prepended with a timestamp.
* `SIRO_CRLF`      : Output will be appended with a carriage return and line feed.
* `SIRO_FILES`     : Enables the use of disk files for output.
* `SIRO_CALLBACKS` : Enables the use of callback functions for output.
* `SIRO_DEFAULT`   : Functionally equivalent to SIRO_TIMESTAMP and SIRO_CRLF.  Sir will use this option if none are specified.

#### Logging levels
* `SIRT_DEBUG`   : For debugging information.
* `SIRT_ERROR`   : To report errors (non-fatal).
* `SIRT_WARNING` : To report warnings or notifications.
* `SIRT_FATAL`   : To report fatal errors.
* `SIRT_LOG`     : For outputting logging data.
* `SIRT_SCREEN`  : For outputting information to the user.
