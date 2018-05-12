/**
 * @file sirdefaults.h
 * @brief Compile-time default levels, options, and text styilng.
 *
 * This file and accompanying source code originated from <https://github.com/ryanlederman/sir>.
 * If you obtained it elsewhere, all bets are off.
 *
 * @author Ryan M. Lederman <lederman@gmail.com>
 * @copyright
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Ryan M. Lederman
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
 * @addtogroup custom
 * @{
 */

/** Default levels for \a stdout. */
static const sir_levels sir_stdout_def_lvls
    = SIRL_DEBUG | SIRL_INFO | SIRL_NOTICE | SIRL_WARN;

/** Default options for \a stdout. */
static const sir_options sir_stdout_def_opts
    = SIRO_NOTIME | SIRO_NOPID | SIRO_NOTID;

/** Default levels for \a stderr. */
static const sir_levels sir_stderr_def_lvls
    = SIRL_ERROR | SIRL_CRIT | SIRL_EMERG;

/** Default options for \a stderr. */
static const sir_options sir_stderr_def_opts 
    = SIRO_NOTIME | SIRO_NOPID | SIRO_NOTID;

/** Default levels for \a syslog (if enabled). */
static const sir_levels sir_syslog_def_lvls
    = SIRL_WARN | SIRL_CRIT | SIRL_ALERT | SIRL_EMERG;

/** Default levels for \a log \a files. */
static const sir_levels sir_file_def_lvls
    = SIRL_ALL;

/** Default options for \a log \a files. */
static const sir_options sir_file_def_opts
    = 0; /* (all output) */

/** Default mapping of ::sir_level to ::sir_textstyle. */
static const sir_style_map sir_default_styles[SIR_NUMLEVELS] = {
    {SIRL_DEBUG, SIRS_FG_DGRAY},
    {SIRL_INFO, SIRS_FG_WHITE},
    {SIRL_NOTICE, SIRS_FG_CYAN},
    {SIRL_WARN, SIRS_FG_YELLOW},
    {SIRL_ERROR, SIRS_FG_RED},
    {SIRL_CRIT, SIRS_BRIGHT | SIRS_FG_RED},
    {SIRL_ALERT, SIRS_BRIGHT | SIRS_FG_BLACK | SIRS_BG_LYELLOW},
    {SIRL_EMERG, SIRS_BRIGHT | SIRS_FG_LYELLOW | SIRS_BG_RED},
};

/** @} */

#endif /* !_SIR_DEFAULTS_H_INCLUDED */
