#include "sirtextstyle.h"
#include "sirinternal.h"
#include "sirdefaults.h"

bool _sir_validstyle(sir_textstyle style, uint16_t* pattr, uint16_t *pfg, uint16_t* pbg) {

    uint16_t attr = (style & _SIRS_ATTR_MASK);
    uint16_t fg = (style & _SIRS_FG_MASK);
    uint16_t bg = (style & _SIRS_BG_MASK);

    bool attrvalid = attr <= SIRS_BRIGHT;
    bool fgvalid = fg <= SIRS_FG_WHITE;
    bool bgvalid = bg <= SIRS_BG_WHITE;

    assert(attrvalid);
    assert(fgvalid);
    assert(bgvalid);

    if (pattr && pfg && pbg) {
        *pattr = attrvalid ? attr : 0;
        *pfg = fgvalid ? fg : 0;
        *pbg = bgvalid ? bg : 0;
    }

    return attrvalid && fgvalid && bgvalid;
}

bool _sir_setdefstyle(sir_level level, sir_textstyle style) {

    assert(validlevel(level));

    if (validlevel(level)) {
        bool validstyle = _sir_validstyle(style, NULL, NULL, NULL);
        assert(validstyle);

        if (validstyle) {
            for (size_t n = 0; n < _COUNTOF(sir_default_styles); n++) {
                if (sir_default_styles[n].level == level) {
                    sir_default_styles[n].style = style;
                    return true;
                }
            }
        }
    }

    return false;
}

sir_textstyle _sir_getdefstyle(sir_level level) {

    assert(validlevel(level));

    for (size_t n = 0; n < _COUNTOF(sir_default_styles); n++) {
        if (sir_default_styles[n].level == level)
            return sir_default_styles[n].style;
    }

    return SIRS_INVALID;
}

uint16_t _sir_getprivstyle(uint16_t cat) {

    for (size_t n = 0; n < _COUNTOF(sir_priv_map); n++) {
        if (sir_priv_map[n].from == cat) {
            return sir_priv_map[n].to;
        }
    }

    return _sir_getprivstyle(SIRS_NONE);    
}

bool _sir_formatstyle(sir_textstyle style, sirchar_t* buf, size_t size) {
    
    assert(buf);

    if (buf) {
        
        uint16_t attr;;
        uint16_t fg;
        uint16_t bg;

        if (_sir_validstyle(style, &attr, &fg, &bg)) {

            uint16_t privattr = _sir_getprivstyle(attr);
            uint16_t privfg = _sir_getprivstyle(fg);
            uint16_t privbg = _sir_getprivstyle(bg);

 #ifndef _WIN32
            sirchar_t fgfmt[5] = {0};
            sirchar_t bgfmt[5] = {0};

            if (privfg != 0)
                snprintf(fgfmt, 5, ";%03hu", privfg);

            if (privbg != 0)
                snprintf(bgfmt, 5, ";%03hu", privbg);

            /* '\033[n;nnn;nnnm' */
            snprintf(buf, size, "\033[%1hu%s%sm", privattr, fgfmt, bgfmt);

            return validstr(buf);
#else
            uint16_t final = privattr | privfg | privbg;
            memcpy(buf, &final, sizeof(uint16_t));
            return true;
#endif
        }    
    }

    return false;    
}