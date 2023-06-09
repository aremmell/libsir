/**
 * @file sirdefaults.h
 * @brief Compile-time default levels, options, and text styilng.
 *
 * @author    Ryan M. Lederman \<lederman@gmail.com\>
 * @date      2018-2023
 * @version   @doxyconfig PROJECT_NUMBER
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
 * @addtogroup config
 * @{
 */

/** Default levels for stdout. */
static const sir_levels sir_stdout_def_lvls
    = SIRL_DEBUG | SIRL_INFO | SIRL_NOTICE | SIRL_WARN;

/** Default options for stdout. */
static const sir_options sir_stdout_def_opts
    = SIRO_NOTIME | SIRO_NOPID | SIRO_NOTID;

/** Default levels for stderr. */
static const sir_levels sir_stderr_def_lvls
    = SIRL_ERROR | SIRL_CRIT | SIRL_ALERT | SIRL_EMERG;

/** Default options for stderr. */
static const sir_options sir_stderr_def_opts
    = SIRO_NOTIME | SIRO_NOPID | SIRO_NOTID;

/** Default levels for the system logger. */
static const sir_levels sir_syslog_def_lvls
    = SIRL_NOTICE | SIRL_WARN | SIRL_ERROR | SIRL_CRIT |
      SIRL_ALERT | SIRL_EMERG;

/** Default options for the system logger. */
static const sir_options sir_syslog_def_opts
    = SIRO_ALL;

/** Default levels for log files. */
static const sir_levels sir_file_def_lvls
    = SIRL_ALL;

/** Default options for log files. */
static const sir_options sir_file_def_opts
    = SIRO_ALL;

/** 
 * @brief Default mapping of ::sir_level to ::sir_textstyle.
 *
 * @attention Entries *must* remain in numeric ascending order (by SIRL_*);
 * binary search is used to look up entries based on those values.
 */
static const sir_level_style_pair sir_default_style_map[] = {
    {SIRL_EMERG,  SIRS_BRIGHT | SIRS_FG_LYELLOW | SIRS_BG_RED},
    {SIRL_ALERT,  SIRS_BRIGHT | SIRS_FG_BLACK | SIRS_BG_LYELLOW},
    {SIRL_CRIT,   SIRS_BRIGHT | SIRS_FG_RED},
    {SIRL_ERROR,  SIRS_FG_RED},
    {SIRL_WARN,   SIRS_FG_YELLOW},
    {SIRL_NOTICE, SIRS_FG_CYAN},
    {SIRL_INFO,   SIRS_FG_WHITE},
    {SIRL_DEBUG,  SIRS_FG_DGRAY}
};

/** @} */

#endif /* !_SIR_DEFAULTS_H_INCLUDED */
