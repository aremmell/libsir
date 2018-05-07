/**
 * @file sirerrors.c
 * @brief Library error management.
 */

#include "sirerrors.h"

static thread_local sirerror_t sir_lasterror = SIR_E_NOERROR;

void _sir_seterror(const sirerror_t err) {

    assert(_sir_validerror(err));

    if (_sir_validerror(err)) {
        sir_lasterror = err;

#pragma message "refactor internal error handling to go through sir_selflog"
/*         if (SIR_E_NOERROR != sir_lasterror) {
            sirchar_t message[SIR_MAXERROR] = {0};
            _sir_geterror(message);
            _sir_selflog("%s: library error: (0x%04hx, '%s')\n", __func__,
                _sir_geterrcode(sir_lasterror), _sir_validstr(message) ? message : SIR_UNKERROR);
        } */
    }
}

sirerror_t _sir_geterror(sirchar_t message[SIR_MAXERROR]) {

    for (size_t n = 0; n < _sir_countof(sir_errors); n++) {
        if (sir_errors[n].e == sir_lasterror) {
            strncpy(message, sir_errors[n].msg, SIR_MAXERROR);
            return sir_errors[n].e;
        }
    }

    assert(false);
    return SIR_E_UNKNOWN;
}
