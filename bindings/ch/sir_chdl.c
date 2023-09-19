#include <ch.h>
#include <stdbool.h>
#include <sir/types.h>
#include "chsir.h"

EXPORTCH bool
sir_makeinit_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    sirinit *si;
    bool retval;

    Ch_VaStart(interp, ap, varg);
    si     = Ch_VaArg(interp, ap, sirinit *);
    retval = sir_makeinit(si);
    Ch_VaEnd(interp, ap);
    return retval;
}

EXPORTCH bool
sir_init_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    sirinit *si;
    bool retval;

    Ch_VaStart(interp, ap, varg);
    si     = Ch_VaArg(interp, ap, sirinit *);
    retval = sir_init(si);
    Ch_VaEnd(interp, ap);
    return retval;
}

EXPORTCH bool
sir_cleanup_chdl(void) {
    return sir_cleanup();
}

EXPORTCH bool
sir_isprerelease_chdl(void) {
    return sir_isprerelease();
}

EXPORTCH bool
sir_isinitialized_chdl(void) {
    return sir_isinitialized();
}

EXPORTCH uint16_t
sir_geterror_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    char m[SIR_MAXERROR];
    char *message = m;
    uint16_t retval;

    Ch_VaStart(interp, ap, varg);
    message = Ch_VaArg(interp, ap, char *);
    retval  = sir_geterror(message);
    Ch_VaEnd(interp, ap);
    return retval;
}

EXPORTCH const char *
sir_getversionstring_chdl(void) {
    return sir_getversionstring();
}

EXPORTCH uint32_t
sir_getversionhex_chdl(void) {
    return sir_getversionhex();
}

EXPORTCH bool
sir_debug_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    char ch_message[SIR_MAXMESSAGE];
    const char *message = ch_message;
    bool retval;

    Ch_VaStart(interp, ap, varg);
    message = Ch_VaArg(interp, ap, const char *);
    retval  = sir_debug(message);
    Ch_VaEnd(interp, ap);
    return retval;
}

EXPORTCH bool
sir_info_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    char ch_message[SIR_MAXMESSAGE];
    const char *message = ch_message;
    bool retval;

    Ch_VaStart(interp, ap, varg);
    message = Ch_VaArg(interp, ap, const char *);
    retval  = sir_info(message);
    Ch_VaEnd(interp, ap);
    return retval;
}

EXPORTCH bool
sir_warn_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    char ch_message[SIR_MAXMESSAGE];
    const char *message = ch_message;
    bool retval;

    Ch_VaStart(interp, ap, varg);
    message = Ch_VaArg(interp, ap, const char *);
    retval  = sir_warn(message);
    Ch_VaEnd(interp, ap);
    return retval;
}

EXPORTCH bool
sir_alert_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    char ch_message[SIR_MAXMESSAGE];
    const char *message = ch_message;
    bool retval;

    Ch_VaStart(interp, ap, varg);
    message = Ch_VaArg(interp, ap, const char *);
    retval  = sir_alert(message);
    Ch_VaEnd(interp, ap);
    return retval;
}

EXPORTCH bool
sir_notice_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    char ch_message[SIR_MAXMESSAGE];
    const char *message = ch_message;
    bool retval;

    Ch_VaStart(interp, ap, varg);
    message = Ch_VaArg(interp, ap, const char *);
    retval  = sir_notice(message);
    Ch_VaEnd(interp, ap);
    return retval;
}

EXPORTCH bool
sir_error_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    char ch_message[SIR_MAXMESSAGE];
    const char *message = ch_message;
    bool retval;

    Ch_VaStart(interp, ap, varg);
    message = Ch_VaArg(interp, ap, const char *);
    retval  = sir_error(message);
    Ch_VaEnd(interp, ap);
    return retval;
}

EXPORTCH bool
sir_crit_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    char ch_message[SIR_MAXMESSAGE];
    const char *message = ch_message;
    bool retval;

    Ch_VaStart(interp, ap, varg);
    message = Ch_VaArg(interp, ap, const char *);
    retval  = sir_crit(message);
    Ch_VaEnd(interp, ap);
    return retval;
}

EXPORTCH bool
sir_emerg_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    char ch_message[SIR_MAXMESSAGE];
    const char *message = ch_message;
    bool retval;

    Ch_VaStart(interp, ap, varg);
    message = Ch_VaArg(interp, ap, const char *);
    retval  = sir_emerg(message);
    Ch_VaEnd(interp, ap);
    return retval;
}
