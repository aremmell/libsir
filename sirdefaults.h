#ifndef _SIR_DEFAULTS_H_INCLUDED
#define _SIR_DEFAULTS_H_INCLUDED

#include "sirtypes.h"

#pragma message "default init"
static const sirinit sir_defaults;

static const sir_style_map sir_default_styles[] = {
    {SIRL_DEBUG, SIRS_BRIGHT | SIRS_FG_BLACK},
    {SIRL_INFO, SIRS_FG_WHITE},
    {SIRL_NOTICE, SIRS_FG_CYAN},
    {SIRL_WARN, SIRS_BRIGHT | SIRS_FG_YELLOW},
    {SIRL_ERROR, SIRS_FG_RED},
    {SIRL_CRIT, SIRS_BRIGHT | SIRS_FG_RED},
    {SIRL_ALERT, SIRS_BRIGHT | SIRS_FG_RED},
    {SIRL_EMERG, SIRS_BRIGHT | SIRS_FG_RED}
};

#endif /* !_SIR_DEFAULTS_H_INCLUDED */
