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
 * @param   si   Pointer to a ::sirinit structure to receive default values.
 * @returns bool `true` if `si` is not `NULL`, `false` otherwise.
 */
bool sir_makeinit(sirinit* si);

/**
 * @brief Initializes libsir for use.
 *
 * Must be called before making any other calls into libsir (with the exception
 * of ::sir_makeinit).
 *
 * For every call to ::sir_init, there must be a corresponding call to
 * ::sir_cleanup. All exported libsir functions are thread-safe, so you may
 * initialize and cleanup on whichever thread you wish.
 *
 * @see ::sir_makeinit
 * @see ::sir_cleanup
 *
 * @param si Pointer to a ::sirinit structure containing the desired settings
 * and configuration. libsir makes a copy of this structure, so its lifetime
 * is not a concern.
 *
 * @returns bool `true` if initialization was successful, `false` otherwise. Call
 * ::sir_geterror to obtain information about any error that may have occurred.
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
 * @returns bool `true` if cleanup was successful, `false otherwise`. Call
 * ::sir_geterror to obtain information about any error that may have occurred.
 */
bool sir_cleanup(void);

/**
 * @brief Retrieves information about the last error that occurred.
 *
 * libsir maintains errors on a per-thread basis, so it's important that the
 * same thread that encountered a failed library call be the one to get the
 * error information.
 *
 * @param   message A buffer of ::SIR_MAXERROR chars to receive an error message.
 * @returns uint16_t The error code. Possible error codes are listed in
 * ::sir_errorcode.
 */
uint16_t sir_geterror(char message[SIR_MAXERROR]);

/**
 * @brief Dispatches a ::SIRL_DEBUG level message.
 *
 * The message will be delivered to all destinations registered to receive debug
 * level messages, each with their own formatting and styling options.
 *
 * If no destinations are registered for this level, the return value will be
 * false, and the error returned from ::sir_geterror will be ::SIR_E_NODEST.
 *
 * This is not a fatal error–it simply means your message was not dispatched to
 * any destination.
 *
 * @remark For stdio (stdout/stderr) destinations, you may find the default
 * formatting options and text styling for each in ::Defaults.
 *
 * @remark To change options or level registrations for stdout/stderr, call
 * ::sir_stdoutopts/::sir_stdoutlevels and ::sir_stderropts/::sir_stderrlevels,
 * respectively. To change the text styling on a per-level basis, call
 * ::sir_settextstyle.
 *
 * @see ::sir_level
 * @see ::sir_option
 *
 * @param  format A printf-style format string, representing the template for the
 *                message to dispatch.
 * @param  ...    Arguments whose type and position align with the format specifiers
 *                in @param format
 * @returns bool  `true` if the message was dispatched succcessfully to at least one
 * destination, `false` otherwise. Call ::sir_geterror to obtain information about
 * any error that may have occurred.
 */
bool sir_debug(const char* format, ...);
bool sir_info(const char* format, ...);
bool sir_notice(const char* format, ...);
bool sir_warn(const char* format, ...);
bool sir_error(const char* format, ...);
bool sir_crit(const char* format, ...);
bool sir_alert(const char* format, ...);
bool sir_emerg(const char* format, ...);

/**
 * @brief Adds a log file and registeres it to receive log output.
 *
 * The file at `path` will be created if it does not exist; otherwise it
 * be appended to.
 *
 * The levels for which the file will receive output are specified in the
 * `levels` bitmask. If you wish to use the default levels, pass
 * ::SIRL_DEFAULT.
 *
 * The formatting options for the log output sent to the file are specified in
 * the `opts` bitmask. If you wish to use the default options, pass
 * ::SIRO_DEFAULT. By default, log files receive all available information.
 *
 * @remark Take note of the ::SIR_FROLLSIZE compile-time constant. When any log
 * file reaches that size in bytes, it will be archived to a date-stamped file
 * in the same directory, and logging will resume at the path of the original file.
 *
 * @remark If `path` is a relative path, it shall be treated as relative
 * to the _current working directory_. This is not necessarily the same path
 * that your process' binary file resides in.
 *
 * @remark The return value from this function is only valid for the lifetime of
 * the process. If a crash or restart occurs, you will no longer be able to refer
 * to the file by that identifier, and will have to add the file again (libsir does
 * not persist its list of log files).
 *
 * @remark To change the file's level registrations or options after adding it,
 * call ::sir_filelevels and ::sir_fileopts, respectively.
 *
 * @see ::sir_remfile
 *
 * @param path        Either a relative or absolute path to a file name which shall
 *                    become a log file managed by libsir.
 * @param levels      Levels of output to register the file for.
 * @param opts        Formatting options for the output sent to the file.
 * @returns sirfileid If successful, a unique identifier that can later be used
 * to modify level registrations, options, or remove the file from libsir. Upon
 * failure, returns `NULL`. Use ::sir_geterror to obtain information about any
 * error that may have occurred.
 */
sirfileid sir_addfile(const char* path, sir_levels levels, sir_options opts);

/**
 * @brief Removes a file previously added to libsir.
 *
 * Immediately causes the file represented by `id` (the value returned from
 * ::sir_addfile) to be removed from libsir, and its file descriptor closed.
 *
 * Its contents will remain intact, and any pending writes will be flushed to
 * the file before it is closed.
 *
 * @see ::sir_filelevels
 * @see ::sir_fileopts
 *
 * @param   id   The ::sirfileid obtained when the file was added to libsir.
 * @returns bool `true` if the file is known to libsir, and was successfully
 *               removed, `false` otherwise. Use ::sir_geterror to obtain
 *               information about any error that may have occurred.
 */
bool sir_remfile(sirfileid id);

/**
 * @brief Set new level registrations for a log file already managed by libsir.
 *
 * @see ::sir_fileopts
 *
 * @param   id     The ::sirfileid obtained when the file was added to libsir.
 * @param   levels New bitmask of ::sir_level to register for.
 * @returns bool   `true` if the file is known to libsir and was succcessfully
 *                 updated, `false` otherwise. Use ::sir_geterror to obtain
 *                 information about any error that may have occurred.
 */
bool sir_filelevels(sirfileid id, sir_levels levels);

/**
 * @brief Set new formatting options for a log file already managed by libsir.
 *
 * @see ::sir_filelevels
 *
 * @param   id    The ::sirfileid obtained when the file was added to libsir.
 * @param   opts  New bitmask of ::sir_option for the file.
 * @returns bool `true` if the file is known to libsir and was succcessfully
 *                updated, `false` otherwise. Use ::sir_geterror to obtain
 *                information about any error that may have occurred.
 */
bool sir_fileopts(sirfileid id, sir_options opts);

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
