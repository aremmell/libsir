/**
 * @file sirtextstyle.c
 * @brief stdio text styling.
 *
 * This file and accompanying source code originated from <https://github.com/aremmell/libsir>.
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
        assert(attrvalid && fgvalid && bgvalid);
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

            size_t low  = 0;
            size_t high = SIR_NUMLEVELS - 1;
            size_t mid  = (low + high) / 2;

            do {
                if (map[mid].level == level && map[mid].style != SIRS_INVALID) {
                    override = true;
                    found = map[mid].style;
                    break;
                }

                if (low == high)
                    break;

                if (level > map[mid].level)
                    low = mid + 1;
                else
                    high = mid - 1;

                mid = (low + high) / 2;
            } while (true);

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

            size_t low = 0;
            size_t high = SIR_NUMLEVELS - 1;
            size_t mid = (low + high) / 2;

            do {
                if (map[mid].level == level) {
                    found = map[mid].style;
                    break;
                }

                if (low == high)
                    break;

                if (level > map[mid].level)
                    low = mid + 1;
                else
                    high = mid - 1;

                mid = (low + high) / 2;
            } while (true);

            return found;
        }
    }

    return SIRS_INVALID;
}

bool _sir_settextstyle(sir_level level, sir_textstyle style)
{
    _sir_seterror(_SIR_E_NOERROR);

    if (_sir_sanity() && _sir_validlevel(level) && _sir_validstyle(style, NULL, NULL, NULL)) {
        sir_style_map* map = _sir_locksection(_SIRM_TEXTSTYLE);
        assert(map);

        if (map) {
            size_t low = 0;
            size_t high = SIR_NUMLEVELS - 1;
            size_t mid = (low + high) / 2;

            bool updated = false;
            do {
                if (map[mid].level == level) {
                    map[mid].style = style;
                    updated = true;
                    break;
                }

                if (low == high)
                    break;

                if (level > map[mid].level)
                    low = mid + 1;
                else
                    high = mid - 1;

                mid = (low + high) / 2;
            } while (true);

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

uint16_t _sir_getprivstyle(uint32_t style) {

    static const size_t idx_attr_start = 0;
    static const size_t idx_attr_end = 2;

    static const size_t idx_fg_start = 3;
    static const size_t idx_fg_end = 19;

    static const size_t idx_bg_start = 20;
    static const size_t idx_bg_end = _sir_countof(sir_priv_map) - 2;

    size_t low = 0;
    size_t high = 0;

    if (style <= SIRS_DIM) {
        low = idx_attr_start;
        high = idx_attr_end;
    } else if (style <= SIRS_FG_DEFAULT) {
        low = idx_fg_start;
        high = idx_fg_end;
    } else {
        low = idx_bg_start;
        high = idx_bg_end;
    }

    size_t mid = (low + high) / 2;

    do
    {
        if (sir_priv_map[mid].from == style)
            return sir_priv_map[mid].to;

        if (low == high)
            break;

        if (style > sir_priv_map[mid].from)
            low = mid + 1;
        else
            high = mid - 1;

        mid = (low + high) / 2;

    } while (true);

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

#if !defined(_WIN32)
            sirchar_t fgfmt[5] = {0};
            sirchar_t bgfmt[5] = {0};

            if (privfg != 0)
                snprintf(fgfmt, 5, ";%03hu", privfg);

            if (privbg != 0)
                snprintf(bgfmt, 5, ";%03hu", privbg);

            /* '\033[n;nnn;nnnm' */
            snprintf(buf, size, "\033[%hu%s%sm", privattr, fgfmt, bgfmt);

            return _sir_validstr(buf);
#else
            assert(size == sizeof(uint16_t));
            if (size < sizeof(uint16_t))
                return false;

            uint16_t final = privattr | privfg | privbg;
            memcpy(buf, &final, sizeof(uint16_t));
            return true;
#endif
        }
    }

    return false;
}

/** @} */
