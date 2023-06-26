/**
 * @file sirdefaults.h
 * @brief Default levels, options, and text styilng.
 *
 * The values herein represent the defaults for output destinations if defaults
 * are requested upon initialization.
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
#ifndef _SIR_DEFAULTS_H_INCLUDED
#define _SIR_DEFAULTS_H_INCLUDED

#include "sirtypes.h"

/**
 * @defgroup default Defaults
 *
 * Default ::sir_option bitmasks, ::sir_level registrations, and
 * ::sir_textstyle bitmasks.
 *
 * @see ::SIRO_DEFAULT
 * @see ::SIRL_DEFAULT
 *
 * @addtogroup default
 * @{
 */

/**
 * Default levels for stdout.
 *
 * The stdout destination is registered for these levels if
 * ::SIRL_DEFAULT is set on the ::sir_stdio_dest when
 * ::sir_init is called.
 *
 * @note Can be modified at runtime by calling ::sir_stdoutlevels.
 */
static const sir_levels sir_stdout_def_lvls
    = SIRL_DEBUG | SIRL_INFO | SIRL_NOTICE | SIRL_WARN;

/**
 * Default options for stdout.
 *
 * These options are applied to the stdout destination if
 * ::SIRO_DEFAULT is set on the ::sir_stdio_dest when
 * ::sir_init is called.
 *
 * @note Can be modified at runtime by calling ::sir_stdoutopts.
 */
static const sir_options sir_stdout_def_opts
    = SIRO_NOTIME | SIRO_NOHOST | SIRO_NOPID | SIRO_NOTID;

/**
 * Default levels for stderr.
 *
 * The stderr destination is registered for these levels if
 * ::SIRL_DEFAULT is set on the ::sir_stdio_dest when
 * ::sir_init is called.
 *
 * @note Can be modified at runtime by calling ::sir_stderrlevels.
 */
static const sir_levels sir_stderr_def_lvls
    = SIRL_ERROR | SIRL_CRIT | SIRL_ALERT | SIRL_EMERG;

/**
 * Default options for stderr.
 *
 * These options are applied to the stderr destination if
 * ::SIRO_DEFAULT is set on the ::sir_stdio_dest when
 * ::sir_init is called.
 *
 * @note Can be modified at runtime by calling ::sir_stderropts.
 */
static const sir_options sir_stderr_def_opts
    = SIRO_NOTIME | SIRO_NOHOST | SIRO_NOPID | SIRO_NOTID;

/**
 * Default levels for the system logger.
 *
 * The system logger destination is registered for these levels
 * if ::SIRL_DEFAULT is set on the ::sir_syslog_dest when
 * ::sir_init is called.
 *
 * @note Can be modified at runtime by calling ::sir_sysloglevels.
 */
static const sir_levels sir_syslog_def_lvls
    = SIRL_NOTICE | SIRL_WARN | SIRL_ERROR | SIRL_CRIT |
      SIRL_ALERT | SIRL_EMERG;

/**
 * Default options for the system logger.
 *
 * Applied to the system logger destination if ::SIRO_DEFAULT
 * is set on the ::sir_syslog_dest when ::sir_init is called.
 *
 * @note Can be modified at runtime by calling ::sir_syslogopts.
 *
 * @note ::SIRO_MSGONLY disables all output formatting, leaving
 * just the message text itself.
 */
static const sir_options sir_syslog_def_opts
    = SIRO_MSGONLY;

/**
 * Default levels for log files.
 *
 * Log file destinations are registered for these levels if
 * ::SIRL_DEFAULT is passed to ::sir_addfile.
 *
 * @note Can be modified at runtime by calling ::sir_filelevels.
 *
 * @note ::SIRL_ALL includes every logging level from debug to emergency.
 */
static const sir_levels sir_file_def_lvls
    = SIRL_ALL;

/**
 * Default options for log files.
 *
 * Applied to log file destinations if ::SIRO_DEFAULT is
 * passed to ::sir_addfile.
 *
 * @note Can be modified at runtime by calling ::sir_fileopts.
 */
static const sir_options sir_file_def_opts
    = SIRO_ALL | SIRO_NOHOST;

/**
 * Default ::sir_textstyle for ::SIRL_EMERG.
 *
 * Applied to stdio destinations upon library initialization.
 *
 * @note Can be modified at runtime by calling ::sir_settextstyle.
 */
static const sir_textstyle sir_lvl_emerg_def_style
    = SIRS_BRIGHT | SIRS_FG_LYELLOW | SIRS_BG_RED;

/**
 * Default ::sir_textstyle for ::SIRL_ALERT.
 *
 * Applied to stdio destinations upon library initialization.
 *
 * @note Can be modified at runtime by calling ::sir_settextstyle.
 */
static const sir_textstyle sir_lvl_alert_def_style
    = SIRS_BRIGHT | SIRS_FG_BLACK | SIRS_BG_LYELLOW;

/**
 * Default ::sir_textstyle for ::SIRL_CRIT.
 *
 * Applied to stdio destinations upon library initialization.
 *
 * @note Can be modified at runtime by calling ::sir_settextstyle.
 */
static const sir_textstyle sir_lvl_crit_def_style
    = SIRS_BRIGHT | SIRS_FG_RED;

/**
 * Default ::sir_textstyle for ::SIRL_ERROR.
 *
 * Applied to stdio destinations upon library initialization.
 *
 * @note Can be modified at runtime by calling ::sir_settextstyle.
 */
static const sir_textstyle sir_lvl_error_def_style
    = SIRS_FG_RED;

/**
 * Default ::sir_textstyle for ::SIRL_WARN.
 *
 * Applied to stdio destinations upon library initialization.
 *
 * @note Can be modified at runtime by calling ::sir_settextstyle.
 */
static const sir_textstyle sir_lvl_warn_def_style
    = SIRS_FG_YELLOW;                

/**
 * Default ::sir_textstyle for ::SIRL_NOTICE.
 *
 * Applied to stdio destinations upon library initialization.
 *
 * @note Can be modified at runtime by calling ::sir_settextstyle.
 */
static const sir_textstyle sir_lvl_notice_def_style
    = SIRS_FG_CYAN;

/**
 * Default ::sir_textstyle for ::SIRL_INFO.
 *
 * Applied to stdio destinations upon library initialization.
 *
 * @note Can be modified at runtime by calling ::sir_settextstyle.
 */
static const sir_textstyle sir_lvl_info_def_style
    = SIRS_FG_WHITE;

/**
 * Default ::sir_textstyle for ::SIRL_DEBUG.
 *
 * Applied to stdio destinations upon library initialization.
 *
 * @note Can be modified at runtime by calling ::sir_settextstyle.
 */
static const sir_textstyle sir_lvl_debug_def_style
    = SIRS_FG_DGRAY;

/** @} */

#endif /* !_SIR_DEFAULTS_H_INCLUDED */
