/**
 * @file sirtextstyle.c
 * @brief stdio text styling.
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
#include "sirtextstyle.h"
#include "sirinternal.h"
#include "sirdefaults.h"

/**
 * @addtogroup intern
 * @{
 */

bool _sir_validstyle(sir_textstyle style, uint32_t* pattr, uint32_t* pfg, uint32_t* pbg) {

    uint32_t attr = (style & _SIRS_ATTR_MASK);
    uint32_t fg   = (style & _SIRS_FG_MASK);
    uint32_t bg   = (style & _SIRS_BG_MASK);

    bool attrvalid = attr <= SIRS_DIM;
    bool fgvalid   = fg <= SIRS_FG_DEFAULT;
    bool bgvalid   = bg <= SIRS_BG_DEFAULT;

    if (pattr && pfg && pbg) {
        *pattr = attrvalid ? attr : 0;
        *pfg   = fgvalid ? fg : 0;
        *pbg   = bgvalid ? bg : 0;
    }

    if (!attrvalid || !fgvalid || !bgvalid) {
        _sir_seterror(_SIR_E_TEXTSTYLE);

        assert(attrvalid);
        assert(fgvalid);
        assert(bgvalid);
        return false;
    }

    return true;
}

sir_textstyle _sir_gettextstyle(sir_level level) {

    if (_sir_validlevel(level)) {
        sir_style_map* map = _sir_locksection(_SIRM_TEXTSTYLE);
        assert(map);

        if (map) {
            sir_textstyle found = SIRS_INVALID;
            bool override = false;

            for (size_t n = 0; n < SIR_NUMLEVELS; n++) {
                if (map[n].level == level && map[n].style != SIRS_INVALID) {
                    override = true;
                    found = map[n].style;
                    break;
                }
            }

            if (!override)
                found = _sir_getdefstyle(sir_default_styles, level);

            _sir_unlocksection(_SIRM_TEXTSTYLE);
            return found;
        }
    }

    return SIRS_INVALID;
}

sir_textstyle _sir_getdefstyle(const sir_style_map* map, sir_level level) {

    if (_sir_validlevel(level)) {
        if (map) {
            sir_textstyle found = SIRS_INVALID;
            for (size_t n = 0; n < SIR_NUMLEVELS; n++) {
                if (map[n].level == level) {
                    found = map[n].style;
                    break;
                }
            }

            return found;
        }
    }

    return SIRS_INVALID;
}

bool _sir_settextstyle(sir_level level, sir_textstyle style) {

    _sir_seterror(_SIR_E_NOERROR);

    if (_sir_sanity() && _sir_validlevel(level) && _sir_validstyle(style, NULL, NULL, NULL)) {
        sir_style_map* map = _sir_locksection(_SIRM_TEXTSTYLE);
        assert(map);

        if (map) {
            bool updated = false;
            for (size_t n = 0; n < SIR_NUMLEVELS; n++) {
                if (map[n].level == level) {
                    map[n].style = style;
                    updated      = true;
                    break;
                }
            }

            return _sir_unlocksection(_SIRM_TEXTSTYLE) && updated;
        }
    }

    return false;
}

bool _sir_resettextstyles(void) {
    sir_style_map* map = _sir_locksection(_SIRM_TEXTSTYLE);
    assert(map);

    if (map) {
        for (size_t n = 0; n < SIR_NUMLEVELS; n++)
            map[n].style = SIRS_INVALID;

        _sir_unlocksection(_SIRM_TEXTSTYLE);
        return true;
    }

    return false;
}

uint16_t _sir_getprivstyle(uint32_t cat) {

    for (size_t n = 0; n < _sir_countof(sir_priv_map); n++) {
        if (sir_priv_map[n].from == cat) {
            return sir_priv_map[n].to;
        }
    }

    return _sir_getprivstyle(SIRS_NONE);
}

bool _sir_formatstyle(sir_textstyle style, sirchar_t* buf, size_t size) {

    if (_sir_validptr(buf)) {

        uint32_t attr;
        uint32_t fg;
        uint32_t bg;

        if (_sir_validstyle(style, &attr, &fg, &bg)) {

            uint16_t privattr = _sir_getprivstyle(attr);
            uint16_t privfg   = _sir_getprivstyle(fg);
            uint16_t privbg   = _sir_getprivstyle(bg);

#ifndef _WIN32
            sirchar_t fgfmt[5] = {0};
            sirchar_t bgfmt[5] = {0};

            if (privfg != 0)
                snprintf(fgfmt, 5, ";%03hhu", privfg);

            if (privbg != 0)
                snprintf(bgfmt, 5, ";%03hhu", privbg);

            /* '\033[n;nnn;nnnm' */
            snprintf(buf, size, "\033[%.1hu%s%sm", privattr, fgfmt, bgfmt);

            return _sir_validstr(buf);
#else
            uint16_t final = privattr | privfg | privbg;
            memcpy(buf, &final, sizeof(uint16_t));
            return true;
#endif
        }
    }

    return false;
}

/** @} */
