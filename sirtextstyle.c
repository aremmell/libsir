/*
 * sirtextstyle.c
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2023
 * Version:   2.2.0
 * License:   The MIT License (MIT)
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

bool _sir_validstyle(sir_textstyle style, uint32_t* pattr, uint32_t* pfg, uint32_t* pbg) {

    uint32_t attr = (style & _SIRS_ATTR_MASK);
    uint32_t fore = (style & _SIRS_FG_MASK);
    uint32_t back = (style & _SIRS_BG_MASK);

    bool attrvalid = attr <= SIRS_DIM;
    bool fgvalid   = fore <= SIRS_FG_WHITE;
    bool bgvalid   = back <= SIRS_BG_WHITE && !_SIRS_SAME_COLOR(fore, back);

#pragma message("TODO: See if we can't rearrange the values and bitmasks for these values so things like 'SIRS_FG_RED | SIRS_FG_DEFAULT' can be invalidated (right now that == SIRS_FG_DGRAY")

    if (_sir_validptrnofail(pattr))
        *pattr = attrvalid ? attr : 0;

    if (_sir_validptrnofail(pfg))
        *pfg = fgvalid ? fore : 0;

    if (_sir_validptrnofail(pbg))
        *pbg  = bgvalid ? back : 0;

    if (attrvalid && fgvalid && bgvalid)
        return true;

    _sir_seterror(_SIR_E_TEXTSTYLE);
    assert("!invalid text style");

    return false;
}

sir_textstyle _sir_gettextstyle(sir_level level) {
    if (_sir_validlevel(level)) {
        sir_level_style_pair* map = _sir_locksection(_SIRM_TEXTSTYLE);
        assert(map);

        if (map) {
            sir_textstyle found = SIRS_INVALID;
            bool override = false;

            size_t low  = 0;
            size_t high = SIR_NUMLEVELS - 1;

            _SIR_DECLARE_BIN_SEARCH(low, high);
            _SIR_BEGIN_BIN_SEARCH();

            if (map[_mid].level == level) {
                if (map[_mid].style != SIRS_INVALID) {
                    override = true;
                    found = map[_mid].style;
                }
                break;
            }

            int comparison = map[_mid].level < level ? 1 : -1;

            _SIR_ITERATE_BIN_SEARCH(comparison);
            _SIR_END_BIN_SEARCH();

            if (!override)
                found = _sir_getdefstyle(level);

            _sir_unlocksection(_SIRM_TEXTSTYLE);
            return found;
        }
    }

    return SIRS_INVALID;
}

sir_textstyle _sir_getdefstyle(sir_level level) {
    if (_sir_validlevel(level)) {
        sir_textstyle found = SIRS_INVALID;

        size_t low = 0;
        size_t high = _sir_countof(sir_default_style_map) - 1;

        _SIR_DECLARE_BIN_SEARCH(low, high);
        _SIR_BEGIN_BIN_SEARCH();

        if (sir_default_style_map[_mid].level == level) {
            found = sir_default_style_map[_mid].style;
            break;
        }

        int comparison = sir_default_style_map[_mid].level < level ? 1 : -1;

        _SIR_ITERATE_BIN_SEARCH(comparison);
        _SIR_END_BIN_SEARCH();

        return found;
    }

    return SIRS_INVALID;
}

bool _sir_settextstyle(sir_level level, sir_textstyle style) {
    
    _sir_seterror(_SIR_E_NOERROR);

    if (_sir_sanity() && _sir_validlevel(level) && _sir_validstyle(style, NULL, NULL, NULL)) {
        sir_level_style_pair* map = _sir_locksection(_SIRM_TEXTSTYLE);
        assert(map);

        if (map) {
            size_t low   = 0;
            size_t high  = SIR_NUMLEVELS - 1;
            bool updated = false;

            _SIR_DECLARE_BIN_SEARCH(low, high);
            _SIR_BEGIN_BIN_SEARCH();

            if (map[_mid].level == level) {
                map[_mid].style = style;
                updated = true;
                break;
            }

            int comparison = map[_mid].level < level ? 1 : -1;

            _SIR_ITERATE_BIN_SEARCH(comparison);
            _SIR_END_BIN_SEARCH();

            return _sir_unlocksection(_SIRM_TEXTSTYLE) && updated;
        }
    }

    return false;
}

bool _sir_resettextstyles(void) {
    sir_level_style_pair* map = _sir_locksection(_SIRM_TEXTSTYLE);
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
    static const size_t idx_bg_end = _sir_countof(sir_style_16color_map) - 1;

    size_t low = 0;
    size_t high = 0;

    if (style <= SIRS_DIM) {
        /* looking up an attribute */
        low = idx_attr_start;
        high = idx_attr_end;
    } else if (style <= SIRS_FG_WHITE) {
        /* looking up a foreground color */
        low = idx_fg_start;
        high = idx_fg_end;
    } else {
        /* looking up a background color */
        low = idx_bg_start;
        high = idx_bg_end;
    }

    _SIR_DECLARE_BIN_SEARCH(low, high);
    _SIR_BEGIN_BIN_SEARCH();

    if (sir_style_16color_map[_mid].from == style)
        return sir_style_16color_map[_mid].to;

    int comparison = sir_style_16color_map[_mid].from < style ? 1 : -1;

    _SIR_ITERATE_BIN_SEARCH(comparison);
    _SIR_END_BIN_SEARCH();

    return _sir_getprivstyle(SIRS_FG_DEFAULT);
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

            sirchar_t fgfmt[7] = {0};
            sirchar_t bgfmt[7] = {0};

            if (privfg != 0)
                snprintf(fgfmt, 7, ";%hu", privfg);

            if (privbg != 0)
                snprintf(bgfmt, 7, ";%hu", privbg);

            /* '\x1b[n;nnn;nnnm' */
            snprintf(buf, size, "%s%hu%s%sm", SIR_BEGINSTYLE, privattr, fgfmt, bgfmt);

            return _sir_validstr(buf);
        }
    }

    return false;
}
