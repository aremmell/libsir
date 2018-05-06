/**
 * @file sirerrors.c
 * @brief Library error management.
 */

#include "sirerrors.h"

static thread_local sirerrcode_t sir_lasterror = SIR_E_NOERROR;

void _sir_seterror(const sirerrcode_t err) {

    assert(validerr(err));

    if (validerr(err))
        sir_lasterror = err;
}

sirerrcode_t _sir_geterror(sirchar_t message[SIR_MAXERROR]) {

    assert(message);

    if (message) {
        resetstr(message);

        for (size_t n = 0; n < _COUNTOF(sir_errors); n++) {
            if (sir_errors[n].code == sir_lasterror) {
                strncpy(message, sir_errors[n].message, SIR_MAXERROR);
                return sir_errors[n].code;
            }
        }
    }

    assert(false);
    return SIR_E_NOERROR;
}
