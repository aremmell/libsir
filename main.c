#include "sir.h"


int main(int argc, char** argv)
{
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
  ss.opts = SIRO_CRLF | SIRO_TIMESTAMP | SIRO_FILES;

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
 
  if(0 != Sir_AddCallback(SirCallback, SIRT_SCREEN, 0U)) {
    printf("Failed to add callback to Sir system!\n");
  } else {
    printf("Successfully associated SirCallback() with SIRT_SCREEN"
           " output type.\n");
  }
*/
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

/*
  Free allocated resources and reset
  the system's state
 */

  Sir_Cleanup ();

  return 0;
}
