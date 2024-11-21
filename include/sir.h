/**
 * @file sir.h
 *
 * @brief Public interface to libsir
 *
 * The functions and types defined here comprise the entire set intended for
 * use by an implementer of the library-unless modification is desired.
 *
 * @version 2.2.6
 *
 * -----------------------------------------------------------------------------
 *
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2018-2024 Ryan M. Lederman <lederman@gmail.com>
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
 *
 * -----------------------------------------------------------------------------
 */

#ifndef _SIR_H_INCLUDED
# define _SIR_H_INCLUDED

# include "sir/platform.h"
# include "sir/version.h"
# include "sir/types.h"

# if defined(SWIG)
#  define PRINTF_FORMAT
#  define PRINTF_FORMAT_ATTR(fmt_p, va_p)
# endif

# if defined(__cplusplus)
extern "C" {
# endif

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
 * the default values (i.e., level registrations, formatting options, and text
 * styling).
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
 * ::sir_cleanup (with the exception of ::sir_makeinit, ::sir_init,
 * ::sir_isinitialized,::sir_geterror, and ::sir_geterrorinfo).
 *
 * May be called from any thread. If you wish to utilize libsir again during the
 * same process' lifetime, simply call ::sir_init again.
 *
 * @returns bool `true` if cleanup was successful, `false otherwise`. Call
 * ::sir_geterror to obtain information about any error that may have occurred.
 */
bool sir_cleanup(void);

/**
 * @brief Determines whether or not libsir is in the initialized state.
 *
 * Provided as a convenience method to detect whether libsir requires initial-
 * ization or cleanup at any given time.
 *
 * @remark Calling ::sir_init after libsir is initialized produces an error.
 * Similarly, ::sir_cleanup behaves the same way if libsir is not initialized.
 *
 * @returns bool `true` if ::sir_init has been called and libsir is initialized;
 * `false` if ::sir_init has not yet been called, or a corresponding call to
 * ::sir_cleanup has already been made.
 */
bool sir_isinitialized(void);

/**
 * @brief Retrieves a formatted message for the last error that occurred on the
 * calling thread and returns the associated error code.
 *
 * To retrieve more granular information about an error, or to customize the
 * error message format, use ::sir_geterrorinfo.
 *
 * @remark libsir maintains errors on a per-thread basis, so it's important that
 * the same thread that encountered a failed library call be the one to retrieve
 * the error message.
 *
 * @param   message  A buffer of ::SIR_MAXERROR chars into which the error message
 *                   is placed.
 * @returns uint16_t An error code (see ::sir_errorcode). Returns ::SIR_E_NOERROR
 *                   if no error has occurred.
 */
uint16_t sir_geterror(char message[SIR_MAXERROR]);

/**
 * @brief Retrieves granular information about the last error that occurred on
 * the calling thread.
 *
 * To retrieve just an error code and a formatted message, use ::sir_geterror.
 *
 * @remark libsir maintains errors on a per-thread basis, so it's important that
 * the same thread that encountered a failed library call be the one to retrieve
 * the error message.
 *
 * @param err Pointer to a ::sir_errorinfo structure into which the error infor-
 *            mation is placed.
 */
void sir_geterrorinfo(sir_errorinfo* err);

/**
 * @brief Dispatches a ::SIRL_DEBUG level message.
 *
 * The message will be delivered to all destinations registered to receive
 * debug-level messages, each with their own formatting and styling options.
 *
 * @remark To change options or level registrations for `stdout`/`stderr`, call
 * ::sir_stdoutopts/::sir_stdoutlevels and ::sir_stderropts/::sir_stderrlevels,
 * respectively. To change the text styling on a per-level basis, call
 * ::sir_settextstyle.
 *
 * @see ::sir_level
 * @see ::sir_option
 * @see ::default
 *
 * @param   format A printf-style format string, representing the template for
 *                 the message to dispatch.
 * @param   ...    Arguments whose type and position align with the format
 *                 specifiers in `format`.
 * @returns bool   `true` if the message was dispatched successfully to all
 *                 registered destinations, `false` otherwise. Call ::sir_geterror
 *                 to obtain information about any error that may have occurred.
 */
PRINTF_FORMAT_ATTR(1, 2)
bool sir_debug(PRINTF_FORMAT const char* format, ...);

/**
 * @brief Dispatches a ::SIRL_INFO level message.
 *
 * The message will be delivered to all destinations registered to receive
 * information-level messages, each with their own formatting and styling options.
 *
 * @remark To change options or level registrations for `stdout`/`stderr`, call
 * ::sir_stdoutopts/::sir_stdoutlevels and ::sir_stderropts/::sir_stderrlevels,
 * respectively. To change the text styling on a per-level basis, call
 * ::sir_settextstyle.
 *
 * @see ::sir_level
 * @see ::sir_option
 * @see ::default
 *
 * @param   format A printf-style format string, representing the template for
 *                 the message to dispatch.
 * @param   ...    Arguments whose type and position align with the format
 *                 specifiers in `format`.
 * @returns bool   `true` if the message was dispatched successfully to all
 *                 registered destinations, `false` otherwise. Call ::sir_geterror
 *                 to obtain information about any error that may have occurred.
 */
PRINTF_FORMAT_ATTR(1, 2)
bool sir_info(PRINTF_FORMAT const char* format, ...);

/**
 * @brief Dispatches a ::SIRL_NOTICE level message.
 *
 * The message will be delivered to all destinations registered to receive
 * notice-level messages, each with their own formatting and styling options.
 *
 * @remark To change options or level registrations for `stdout`/`stderr`, call
 * ::sir_stdoutopts/::sir_stdoutlevels and ::sir_stderropts/::sir_stderrlevels,
 * respectively. To change the text styling on a per-level basis, call
 * ::sir_settextstyle.
 *
 * @see ::sir_level
 * @see ::sir_option
 * @see ::default
 *
 * @param   format A printf-style format string, representing the template for
 *                 the message to dispatch.
 * @param   ...    Arguments whose type and position align with the format
 *                 specifiers in `format`.
 * @returns bool   `true` if the message was dispatched successfully to all
 *                 registered destinations, `false` otherwise. Call ::sir_geterror
 *                 to obtain information about any error that may have occurred.
 */
PRINTF_FORMAT_ATTR(1, 2)
bool sir_notice(PRINTF_FORMAT const char* format, ...);

/**
 * @brief Dispatches a ::SIRL_WARN level message.
 *
 * The message will be delivered to all destinations registered to receive
 * warning-level messages, each with their own formatting and styling options.
 *
 * @remark To change options or level registrations for `stdout`/`stderr`, call
 * ::sir_stdoutopts/::sir_stdoutlevels and ::sir_stderropts/::sir_stderrlevels,
 * respectively. To change the text styling on a per-level basis, call
 * ::sir_settextstyle.
 *
 * @see ::sir_level
 * @see ::sir_option
 * @see ::default
 *
 * @param   format A printf-style format string, representing the template for
 *                 the message to dispatch.
 * @param   ...    Arguments whose type and position align with the format
 *                 specifiers in `format`.
 * @returns bool   `true` if the message was dispatched successfully to all
 *                 registered destinations, `false` otherwise. Call ::sir_geterror
 *                 to obtain information about any error that may have occurred.
 */
PRINTF_FORMAT_ATTR(1, 2)
bool sir_warn(PRINTF_FORMAT const char* format, ...);

/**
 * @brief Dispatches a ::SIRL_ERROR level message.
 *
 * The message will be delivered to all destinations registered to receive
 * error-level messages, each with their own formatting and styling options.
 *
 * @remark To change options or level registrations for `stdout`/`stderr`, call
 * ::sir_stdoutopts/::sir_stdoutlevels and ::sir_stderropts/::sir_stderrlevels,
 * respectively. To change the text styling on a per-level basis, call
 * ::sir_settextstyle.
 *
 * @see ::sir_level
 * @see ::sir_option
 * @see ::default
 *
 * @param   format A printf-style format string, representing the template for
 *                 the message to dispatch.
 * @param   ...    Arguments whose type and position align with the format
 *                 specifiers in `format`.
 * @returns bool   `true` if the message was dispatched successfully to all
 *                 registered destinations, `false` otherwise. Call ::sir_geterror
 *                 to obtain information about any error that may have occurred.
 */
PRINTF_FORMAT_ATTR(1, 2)
bool sir_error(PRINTF_FORMAT const char* format, ...);

/**
 * @brief Dispatches a ::SIRL_CRIT level message.
 *
 * The message will be delivered to all destinations registered to receive
 * critical-level messages, each with their own formatting and styling options.
 *
 * @remark To change options or level registrations for `stdout`/`stderr`, call
 * ::sir_stdoutopts/::sir_stdoutlevels and ::sir_stderropts/::sir_stderrlevels,
 * respectively. To change the text styling on a per-level basis, call
 * ::sir_settextstyle.
 *
 * @see ::sir_level
 * @see ::sir_option
 * @see ::default
 *
 * @param   format A printf-style format string, representing the template for
 *                 the message to dispatch.
 * @param   ...    Arguments whose type and position align with the format
 *                 specifiers in `format`.
 * @returns bool   `true` if the message was dispatched successfully to all
 *                 registered destinations, `false` otherwise. Call ::sir_geterror
 *                 to obtain information about any error that may have occurred.
 */
PRINTF_FORMAT_ATTR(1, 2)
bool sir_crit(PRINTF_FORMAT const char* format, ...);

/**
 * @brief Dispatches a ::SIRL_ALERT level message.
 *
 * The message will be delivered to all destinations registered to receive
 * alert-level messages, each with their own formatting and styling options.
 *
 * @remark To change options or level registrations for `stdout`/`stderr`, call
 * ::sir_stdoutopts/::sir_stdoutlevels and ::sir_stderropts/::sir_stderrlevels,
 * respectively. To change the text styling on a per-level basis, call
 * ::sir_settextstyle.
 *
 * @see ::sir_level
 * @see ::sir_option
 * @see ::default
 *
 * @param   format A printf-style format string, representing the template for
 *                 the message to dispatch.
 * @param   ...    Arguments whose type and position align with the format
 *                 specifiers in `format`.
 * @returns bool   `true` if the message was dispatched successfully to all
 *                 registered destinations, `false` otherwise. Call ::sir_geterror
 *                 to obtain information about any error that may have occurred.
 */
PRINTF_FORMAT_ATTR(1, 2)
bool sir_alert(PRINTF_FORMAT const char* format, ...);

/**
 * @brief Dispatches a ::SIRL_EMERG level message.
 *
 * The message will be delivered to all destinations registered to receive
 * emergency-level messages, each with their own formatting and styling options.
 *
 * @remark To change options or level registrations for `stdout`/`stderr`, call
 * ::sir_stdoutopts/::sir_stdoutlevels and ::sir_stderropts/::sir_stderrlevels,
 * respectively. To change the text styling on a per-level basis, call
 * ::sir_settextstyle.
 *
 * @see ::sir_level
 * @see ::sir_option
 * @see ::default
 *
 * @param   format A printf-style format string, representing the template for
 *                 the message to dispatch.
 * @param   ...    Arguments whose type and position align with the format
 *                 specifiers in `format`.
 * @returns bool   `true` if the message was dispatched successfully to all
 *                 registered destinations, `false` otherwise. Call ::sir_geterror
 *                 to obtain information about any error that may have occurred.
 */
PRINTF_FORMAT_ATTR(1, 2)
bool sir_emerg(PRINTF_FORMAT const char* format, ...);

/**
 * @brief Adds a log file and registers it to receive log output.
 *
 * The file at `path` will be created if it does not exist, otherwise it will
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
 * @remark If `path` is a relative path, it shall be treated as relative to the
 * _current working directory_. This is not necessarily the same directory that
 * your application's binary file resides in.
 *
 * @remark The return value from this function is only valid for the lifetime of
 * the process. If a crash or restart occurs, you will no longer be able to refer
 * to the file by that identifier, and will have to add it again (libsir does not
 * persist its log file cache).
 *
 * @remark To change the file's level registrations or options after adding it,
 * call ::sir_filelevels and ::sir_fileopts, respectively.
 *
 * @see ::sir_remfile
 *
 * @param path        The absolute or relative path of the file to become a
 *                    logging destination for libsir.
 * @param levels      Level registration bitmask.
 * @param opts        Formatting options bitmask.
 * @returns sirfileid If successful, a unique identifier that can later be used
 *                    to modify level registrations, options, or remove the file
 *                    from libsir. Upon failure, returns zero. Use ::sir_geterror
 *                    to obtain information about any error that may have occurred.
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
 * @returns bool `true` if the file is known to libsir and was successfully
 *               removed, `false` otherwise. Use ::sir_geterror to obtain
 *               information about any error that may have occurred.
 */
bool sir_remfile(sirfileid id);

/**
 * @brief Loads a plugin module from disk.
 *
 * Loads, queries, validates, and initializes a libsir plugin module. If loading
 * and validation are successful, the plugin is registered in the internal cache.
 *
 * After that point in time, the plugin will be notified upon the dispatching of
 * log messages on any ::sir_level the plugin registered for when it was loaded.
 *
 * @remark If `path` is a relative path, it shall be treated as relative to the
 * _current working directory_. This is not necessarily the same directory that
 * your application's binary file resides in.
 *
 * @remark The return value from this function is only valid for the lifetime of
 * the process. If a crash or restart occurs, you will no longer be able to refer
 * to the plugin by that identifier, and will have to load it again (libsir does
 * not persist its plugin cache).
 *
 * @see ::sir_unloadplugin
 * @see ::plugins
 *
 * @param  path         The absolute or relative path of the plugin to be loaded
 *                      and registered.
 * @returns sirpluginid If successful, a unique identifier that may later be used
 *                      to unload the plugin module. Upon failure, returns zero.
 *                      Use ::sir_geterror to obtain information about any error
 *                      that may have occurred.
 */
sirpluginid sir_loadplugin(const char* path);

/**
 * @brief Unloads a previously registered plugin module.
 *
 * Cleans up, de-registers, and unloads a plugin represented by `id` (the value
 * returned from ::sir_loadplugin).
 *
 * If the plugin is located in the cache, it is instructed to clean up and prepare
 * to be unloaded. Upon completion of the plugin's clean up routine, it is unloaded.
 *
 * @see ::sir_loadplugin
 * @see ::plugins
 *
 * @param   id   The ::sirpluginid obtained when the plugin was loaded.
 * @returns bool `true` if the plugin was located and successfully unloaded,`false`
 *               otherwise. Use ::sir_geterror to obtain information about any
 *               error that may have occurred.
 */
bool sir_unloadplugin(sirpluginid id);

/**
 * @brief Set new level registrations for a log file already managed by libsir.
 *
 * By default, log files are registered for the following levels:
 *
 * - all levels (SIRL_ALL)
 *
 * @see ::sir_fileopts
 *
 * @param   id     The ::sirfileid obtained when the file was added to libsir.
 * @param   levels New bitmask of ::sir_level to register for. If you wish to use
 *                 the default levels, pass ::SIRL_DEFAULT.
 * @returns bool   `true` if the file is known to libsir and was successfully
 *                 updated, `false` otherwise. Use ::sir_geterror to obtain
 *                 information about any error that may have occurred.
 */
bool sir_filelevels(sirfileid id, sir_levels levels);

/**
 * @brief Set new formatting options for a log file already managed by libsir.
 *
 * By default, log files have the following formatting options:
 *
 * - ::SIRO_ALL
 * - ::SIRO_NOHOST
 *
 * @see ::sir_filelevels
 *
 * @param   id   The ::sirfileid obtained when the file was added to libsir.
 * @param   opts New bitmask of ::sir_option for the file. If you wish to use
 *               the default options, pass ::SIRO_DEFAULT.
 * @returns bool `true` if the file is known to libsir and was successfully
 *               updated, `false` otherwise. Use ::sir_geterror to obtain
 *               information about any error that may have occurred.
 */
bool sir_fileopts(sirfileid id, sir_options opts);

/**
 * @brief Set new text styling for stdio (stdout/stderr) destinations on a
 * per-level basis.
 *
 * @see ::sir_setcolormode
 * @see ::sir_resettextstyles
 * @see ::default
 *
 * @note Use `SIRTC_DEFAULT` to get the default foreground or background color.
 * To set colors in RGB color mode, use ::sir_makergb to create the foreground
 * and background colors.
 *
 * @param   level The ::sir_level for which to set the text styling.
 * @param   attr  The ::sir_textattr attributes to apply to the text.
 * @param   fg    The foreground color to apply to the text.
 * @param   bg    The background color to apply to the text.
 * @returns bool  `true` if successfully updated, `false` otherwise. Use
 *                ::sir_geterror to obtain information about any error that may
 *                have occurred.
 */
bool sir_settextstyle(sir_level level, sir_textattr attr, sir_textcolor fg,
    sir_textcolor bg);

/**
 * @brief Reset text styling for stdio (stdout/stderr) destinations to their
 * default values.
 *
 * @note The text styling will be applied according to the current color mode
 * (as previously set by ::sir_setcolormode, or by default, 16-color mode).
 *
 * @see ::sir_setcolormode
 * @see ::sir_settextstyle
 * @see ::default
 *
 * @returns bool `true` if successfully reset, `false` otherwise. Use
 * ::sir_geterror to obtain information about any error that may have occurred.
 */
bool sir_resettextstyles(void);

/**
 * @brief Creates a ::sir_textcolor from red, green, and blue components.
 *
 * @note Use this function to create colors suitable for ::sir_settextstyle when
 * using RGB color mode.
 *
 * @see ::sir_setcolormode
 * @see ::sir_settextstyle
 * @see ::default
 *
 * @param   r             The red component (0..255)
 * @param   g             The green component (0..255)
 * @param   b             The blue component (0..255)
 * @returns sir_textcolor The color created by combining the r, g, and b components.
 */
sir_textcolor sir_makergb(sir_textcolor r, sir_textcolor g, sir_textcolor b);

/**
 * @brief Sets the ANSI color mode for stdio destinations.
 *
 * @note Some terminals may not support the color modes offered by libsir, so
 * make sure everything looks right after changing the mode.
 *
 * @note When you change the color mode, all previously set text styles will be
 * reset to their defaults. You will have to reapply any text styles set before
 * this call.
 *
 * The available modes are:
 *
 * - `SIRCM_16`:  4-bit, 16-color mode. Colors are defined by the `SIRTC_*` values.
 *                This is the default mode.
 * - `SIRCM_256`: 8-bit, 256-color mode. Colors are defined by numeric value (0..255)
 * - `SIRCM_RGB`: 24-bit RGB color mode. Colors are defined by numeric value, with
 *                red, green, and blue components (0..255) each.
 *
 * @see ::sir_makergb
 * @see ::sir_settextstyle
 * @see ::default
 *
 * @param   mode One of the `SIRCM_*` constants, defining the mode to use.
 * @returns bool `true` if the color mode was changed successfully, `false`
 *               otherwise. Use ::sir_geterror to obtain information about any
 *               error that may have occurred.
 */
bool sir_setcolormode(sir_colormode mode);

/**
 * @brief Set new level registrations for `stdout`.
 *
 * By default, `stdout` is registered for the following levels:
 *
 * - debug   (SIRL_DEBUG)
 * - info    (SIRL_INFO)
 * - notice  (SIRL_NOTICE)
 * - warning (SIRL_WARN)
 *
 * To modify formatting options for `stdout`, use ::sir_stdoutopts.
 *
 * @see ::sir_stdoutopts
 *
 * @param   levels New bitmask of ::sir_level to register for. If you wish to use
 *                 the default levels, pass ::SIRL_DEFAULT.
 * @returns bool   `true` if successfully updated, `false` otherwise. Use
 *                 ::sir_geterror to obtain information about any error that may
 *                 have occurred.
 */
bool sir_stdoutlevels(sir_levels levels);

/**
 * @brief Set new formatting options for `stdout`.
 *
 * By default, `stdout` has the following formatting options:
 *
 * - ::SIRO_NOTIME
 * - ::SIRO_NOHOST
 * - ::SIRO_NOPID
 * - ::SIRO_NOTID
 *
 * To modify level registrations for `stdout`, use ::sir_stdoutlevels.
 *
 * @see ::sir_stdoutlevels
 *
 * @param   opts New bitmask of ::sir_option for `stdout`. If you wish to use the
 *               default values, pass ::SIRL_DEFAULT.
 * @returns bool `true` if successfully updated, `false` otherwise. Use
 *               ::sir_geterror to obtain information about any error that may
 *               have occurred.
 */
bool sir_stdoutopts(sir_options opts);

/**
 * @brief Set new level registrations for `stderr`.
 *
 * By default, `stderr` is registered for the following levels:
 *
 * - error     (SIRL_ERROR)
 * - critical  (SIRL_CRIT)
 * - alert     (SIRL_ALERT)
 * - emergency (SIRL_EMERG)
 *
 * To modify formatting options for `stderr`, use ::sir_stderropts.
 *
 * @see ::sir_stderropts
 *
 * @param   levels New bitmask of ::sir_level to register for. If you wish to use
 *                 the default levels, pass ::SIRL_DEFAULT.
 * @returns bool   `true` if successfully updated, `false` otherwise. Use
 *                 ::sir_geterror to obtain information about any error that may
 *                 have occurred.
 */
bool sir_stderrlevels(sir_levels levels);

/**
 * @brief Set new formatting options for `stderr`.
 *
 * By default, `stderr` has the following formatting options:
 *
 * - ::SIRO_NOTIME
 * - ::SIRO_NOHOST
 * - ::SIRO_NOPID
 * - ::SIRO_NOTID
 *
 * To modify level registrations for `stderr`, use ::sir_stderrlevels.
 *
 * @see ::sir_stderrlevels
 *
 * @param   opts New bitmask of ::sir_option for `stderr`. If you wish to use the
 *               default values, pass ::SIRL_DEFAULT.
 * @returns bool `true` if successfully updated, `false` otherwise. Use
 *               ::sir_geterror to obtain information about any error that may
 *               have occurred.
 */
bool sir_stderropts(sir_options opts);

/**
 * @brief Set new level registrations for the system logger destination.
 *
 * By default, the system logger is registered for the following levels:
 *
 * - notice    (SIRL_NOTICE)
 * - warning   (SIRL_WARNING)
 * - error     (SIRL_ERROR)
 * - critical  (SIRL_CRIT)
 * - emergency (SIRL_EMERG)
 *
 * To modify formatting options for the system logger, use ::sir_syslogopts.
 *
 * @see ::sir_syslogopts
 *
 * @param   levels New bitmask of ::sir_level to register for. If you wish to use
 *                 the default levels, pass ::SIRL_DEFAULT.
 * @returns bool   `true` if successfully updated, `false` otherwise. Use
 *                 ::sir_geterror to obtain information about any error that may
 *                 have occurred.
 */
bool sir_sysloglevels(sir_levels levels);

/**
 * @brief Set new formatting options for the system logger destination.
 *
 * By default, the system logger has the following formatting options:
 *
 * - ::SIRO_MSGONLY
 *
 * To modify level registrations for the system logger, use ::sir_sysloglevels.
 *
 * @see ::sir_sysloglevels
 *
 * @param   opts New bitmask of ::sir_option for the system logger. If you wish
 *               to use the default values, pass ::SIRO_DEFAULT.
 * @returns bool `true` if successfully updated, `false` otherwise. Use
 *               ::sir_geterror to obtain information about any error that may
 *               have occurred.
 */
bool sir_syslogopts(sir_options opts);

/**
 * @brief Set new system logger identity.
 *
 * In the context of the system logger (i.e., `syslog`/`os_log`/etc.), identity
 * refers to the `name` that appears in the log for the current process.
 *
 * Upon library initialization, the system logger identity is resolved as follows:
 *
 * 1. If the @ref sir_syslog_dest.identity "sirinit.d_syslog.identity" string is set,
 *    it will be used.
 * 2. If the ::sirinit.name string is set, it will be used.
 * 3. If the name of the current process is available, it will be used.
 * 4. If none of these are available, the string ::SIR_FALLBACK_SYSLOG_ID will
 *    be used.
 *
 * @remark If `SIR_NO_SYSTEM_LOGGERS` is defined when compiling libsir, this
 * function will immediately return false, and set the last error to
 * ::SIR_E_UNAVAIL.
 *
 * @param   identity The string to use as the system logger identity.
 * @returns bool     `true` if successfully updated, `false` otherwise. Use
 *                   ::sir_geterror to obtain information about any error that
 *                   may have occurred.
 */
bool sir_syslogid(const char* identity);

/**
 * @brief Set new system logger category.
 *
 * Some system logger facilities (e.g. `os_log`, the system logger on macOS), ask
 * for a category string when logging messages-to more easily group messages
 * together (which, as a byproduct, enhances searching).
 *
 * If you are not utilizing a system logger that requires a category string, you
 * do not need to call this function (or set @ref sir_syslog_dest.category
 * "sirinit.d_syslog.category").
 *
 * Upon library initialization, the system logger category is resolved as follows:
 *
 * 1. If the @ref sir_syslog_dest.category "sirinit.d_syslog.category" string is
 *    set, it will be used.
 * 2. The string ::SIR_FALLBACK_SYSLOG_CAT will be used.
 *
 * @remark If `SIR_NO_SYSTEM_LOGGERS` is defined when compiling libsir, this
 * function will immediately return false, and set the last error to
 * ::SIR_E_UNAVAIL.
 *
 * @param category The string to use as the system logger category.
 * @returns bool   `true` if successfully updated, `false` otherwise. Use
 *                 ::sir_geterror to obtain information about any error that
 *                 may have occurred.
 */
bool sir_syslogcat(const char* category);

/**
 * @brief Returns the current libsir version as a string.
 *
 * @note This may be helpful when using libsir as a shared library-it will always
 * return the value of the shared library, not the source version of libsir
 * that you built against.
 *
 * **Example:**
 *
 * ~~~
 * 2.2.6-dev
 * ~~~
 *
 * @returns const char* The current libsir version string.
 */
const char* sir_getversionstring(void);

/**
 * @brief Returns the current libsir version as a number.
 *
 * @note Can be formatted as a hexadecimal number with %08x.
 *
 * @returns uint32_t The current libsir version number.
 */
uint32_t sir_getversionhex(void);

/**
 * @brief Whether or not this is a pre-release version of libsir.
 *
 * @returns bool `true` if this is a pre-release version of libsir (i.e., not
 * a public release), `false` if this is a public release version.
 */
bool sir_isprerelease(void);

/**
 * @}
 * @}
 */

# if defined(__cplusplus)
}
# endif

#endif /* !_SIR_H_INCLUDED */
