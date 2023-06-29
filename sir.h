/**
 * @file sir.h
 * @brief Public interface to libsir
 *
 * The functions and types defined here comprise the entire set intended for
 * use by an implementor of the library-unless modification is desired.
 *
 * @author    Ryan M. Lederman \<lederman@gmail.com\>
 * @date      2018-2023
 * @version   2.2.0
 * @copyright The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef _SIR_H_INCLUDED
#define _SIR_H_INCLUDED

#include "sirplatform.h"
#include "sirtypes.h"

#if defined(__cplusplus)
extern "C" {
#endif /* !__cplusplus */

/**
 * @defgroup public Public Interface
 *
 * Functions and types that comprise the public interface to libsir.
 *
 * @addtogroup public
 * @{
 *
 * @defgroup publicfuncs Functions
 * @{
 */

/**
 * @brief Fills out a ::sirinit structure with default values.
 *
 * Creates an initialization configuration for libsir essentially using all of
 * the default values (i.e., levels, options, text styling).
 *
 * @note Does not fill in string fields, such as ::sirinit.name.
 *
 * @param si Pointer to a ::sirinit structure to receive default values.
 * @return true if si is not NULL, false otherwise.
 */
bool sir_makeinit(sirinit* si);

/**
 * @brief Initializes libsir for use.
 *
 * Must be called before making any other calls into libsir (with the exception
 * of ::sir_makeinit).
 *
 * For every call to ::sir_init, there must be a corresponding call to
 * ::sir_cleanup. May be called from any thread, and it is not necessary to call
 * ::sir_cleanup from the same thread.
 *
 * @see ::sir_makeinit
 * @see ::sir_cleanup
 *
 * @param si Pointer to a ::sirinit structure containing the desired settings
 * and configuration. libsir makes a copy of this structure, so its lifetime
 * is not a concern.
 *
 * @return true if initialization was successful, false otherwise. Call ::sir_geterror
 * to obtain information about any error that may have occurred if false.
 */
bool sir_init(sirinit* si);

/**
 * @brief Tears down and cleans up libsir after use.
 *
 * Deallocates resources such as memory buffers, file descriptors, etc. and
 * resets the internal state. No calls into libsir will succeed after calling
 * ::sir_cleanup (with the exception of ::sir_makeinit and ::sir_init).
 *
 * May be called from any thread. If you wish to utilize libsir again during the
 * same process' lifetime, simply call ::sir_init again.
 *
 * @return true if cleanup was successful, false otherwise. Call ::sir_geterror
 * to obtain information about any error that may have occurred if false.
 */
bool sir_cleanup(void);

/**
 * @brief Retrieves information about the last error that occurred.
 *
 * libsir maintains errors on a per-thread basis, so it's important that the
 * same thread that encountered a failed library call be the one to get the
 * error information.
 *
 * @param message A buffer of ::SIR_MAXERROR chars to receive an error message.
 * @return uint16_t The error code. Possible error codes can be found in
 * ::sirerrors.h.
 */
uint16_t sir_geterror(char message[SIR_MAXERROR]);

bool sir_debug(const char* format, ...);
bool sir_info(const char* format, ...);
bool sir_notice(const char* format, ...);
bool sir_warn(const char* format, ...);
bool sir_error(const char* format, ...);
bool sir_crit(const char* format, ...);
bool sir_alert(const char* format, ...);
bool sir_emerg(const char* format, ...);

sirfileid_t sir_addfile(const char* path, sir_levels levels, sir_options opts);
bool sir_remfile(sirfileid_t id);

bool sir_filelevels(sirfileid_t id, sir_levels levels);
bool sir_fileopts(sirfileid_t id, sir_options opts);

bool sir_settextstyle(sir_level level, sir_textstyle style);
bool sir_resettextstyles(void);

bool sir_stdoutlevels(sir_levels levels);
bool sir_stdoutopts(sir_options opts);

bool sir_stderrlevels(sir_levels levels);
bool sir_stderropts(sir_options opts);

bool sir_sysloglevels(sir_levels levels);
bool sir_syslogopts(sir_options opts);
bool sir_syslogid(const char* identity);
bool sir_syslogcat(const char* category);

/**
 * @}
 * @}
 */

#if defined(__cplusplus)
}
#endif /* !__cplusplus */

#endif /* !_SIR_H_INCLUDED */
