/*!
 * \file sirdefaults.h
 */
#ifndef _SIR_DEFAULTS_H_INCLUDED
#define _SIR_DEFAULTS_H_INCLUDED

#include "sirtypes.h"

static sir_style_map sir_default_styles[] = {
    {SIRL_DEBUG, SIRS_BRIGHT | SIRS_FG_BLACK},
    {SIRL_INFO, SIRS_FG_WHITE},
    {SIRL_NOTICE, SIRS_FG_CYAN},
    {SIRL_WARN, SIRS_FG_YELLOW},
    {SIRL_ERROR, SIRS_FG_RED},
    {SIRL_CRIT, SIRS_BRIGHT | SIRS_FG_RED},
    {SIRL_ALERT, SIRS_BRIGHT | SIRS_FG_YELLOW | SIRS_BG_GREEN},
    {SIRL_EMERG, SIRS_BRIGHT | SIRS_FG_RED | SIRS_BG_WHITE}
};

static const size_t sir_num_default_style
    = sizeof(sir_default_styles) / sizeof(sir_style_map);

#endif /* !_SIR_DEFAULTS_H_INCLUDED */
