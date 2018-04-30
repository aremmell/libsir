#ifndef _SIR_DEFAULTS_H_INCLUDED
#define _SIR_DEFAULTS_H_INCLUDED

#include "sirtypes.h"

#pragma message "default init"
static const sirinit sir_defaults;

static const sir_level_style_map sir_default_styles[] = {
    {SIRL_DEBUG, SIRS_FG_BLACK | SIRS_BRIGHT},
    {SIRL_INFO, SIRS_FG_WHITE},
    {SIRL_NOTICE, SIRS_FG_CYAN},
    {SIRL_WARN, SIRS_FG_YELLOW | SIRS_BRIGHT},
    {SIRL_ERROR, SIRS_FG_RED},
    {SIRL_CRIT, SIRS_FG_RED | SIRS_BRIGHT},
    {SIRL_ALERT, SIRS_FG_RED | SIRS_BRIGHT},
    {SIRL_EMERG, SIRS_FG_RED | SIRS_BRIGHT},
    {0, 0}
};

#endif /* !_SIR_DEFAULTS_H_INCLUDED */
