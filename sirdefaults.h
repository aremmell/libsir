/**
 * @file sirdefaults.h
 * @brief Default options and text styles.
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
    = SIRL_ALL;

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
static sir_style_map sir_default_styles[] = {
    {SIRL_DEBUG, SIRS_FG_LGRAY},
    {SIRL_INFO, SIRS_FG_WHITE},
    {SIRL_NOTICE, SIRS_FG_CYAN},
    {SIRL_WARN, SIRS_FG_YELLOW},
    {SIRL_ERROR, SIRS_FG_RED},
    {SIRL_CRIT, SIRS_BRIGHT | SIRS_FG_RED},
    {SIRL_ALERT, SIRS_BRIGHT | SIRS_FG_BLACK | SIRS_BG_LYELLOW},
    {SIRL_EMERG, SIRS_BRIGHT | SIRS_FG_LYELLOW | SIRS_BG_RED},
};

/** The number of default text style mappings. */
static const size_t sir_num_default_style = sizeof(sir_default_styles) / sizeof(sir_style_map);

/** @} */

#endif /* !_SIR_DEFAULTS_H_INCLUDED */
