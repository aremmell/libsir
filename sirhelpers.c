#include "sirhelpers.h"
#include "sirerrors.h"

bool _sir_validfid(int id) {
    bool valid = id >= 0;
    if (!valid) {
        _sir_seterror(SIR_E_NOFILE);
        assert(valid);
    }
    return valid;
}

bool _sir_validlevels(sir_levels levels) {
    bool valid = levels <= SIRL_ALL;
    if (!valid) {
        _sir_seterror(SIR_E_LEVELS);
        assert(valid);
    }
    return valid;
}

bool _sir_validlevel(sir_level level) {
    bool valid = 0 != level && !(level & (level - 1));
    if (!valid) {
        _sir_seterror(SIR_E_LEVELS);
        assert(valid);
    }
    return valid;
}

bool _sir_validopts(sir_options opts) {
    bool valid = (opts & SIRL_ALL) == 0 && opts <= 0xfff00;
    if (!valid) {
        _sir_seterror(SIR_E_OPTIONS);
        assert(valid);
    }
    return valid;
}

bool __sir_validstr(const sirchar_t* str, bool fail) {
    bool valid = str && (*str != (sirchar_t)'\0');
    if (!valid && fail) {
        _sir_seterror(SIR_E_STRING);
        assert(valid);
    }
    return valid;
}
