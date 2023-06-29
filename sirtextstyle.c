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

const char* _sir_gettextstyle(sir_level level) {
    sir_level_style_tuple* map = _sir_locksection(SIRMI_TEXTSTYLE);
    if (!map) {
        _sir_seterror(_SIR_E_INTERNAL);
        return false;
    }

    const char* found        = SIR_UNKNOWN;
    static const size_t low  = 0;
    static const size_t high = SIR_NUMLEVELS - 1;

    _SIR_DECLARE_BIN_SEARCH(low, high);
    _SIR_BEGIN_BIN_SEARCH();

    if (map[_mid].level == level) {
        found = map[_mid].str;
        break;
    }

    _SIR_ITERATE_BIN_SEARCH((map[_mid].level < level ? 1 : -1));
    _SIR_END_BIN_SEARCH();

    _sir_unlocksection(SIRMI_TEXTSTYLE);
    return found;
}

bool _sir_settextstyle(sir_level level, sir_textstyle style) {
    _sir_seterror(_SIR_E_NOERROR);

    if (!_sir_sanity() || !_sir_validlevel(level) || !_sir_validstyle(style, NULL, NULL, NULL))
        return false;

    sir_level_style_tuple* map = _sir_locksection(SIRMI_TEXTSTYLE);
    if (!map) {
        _sir_seterror(_SIR_E_INTERNAL);
        return false;
    }

    bool updated              = false;
    static const size_t low   = 0;
    static const size_t high  = SIR_NUMLEVELS - 1;

    _SIR_DECLARE_BIN_SEARCH(low, high);
    _SIR_BEGIN_BIN_SEARCH();

    if (map[_mid].level == level) {
        map[_mid].style = style;
        updated         = _sir_formatstyle(style, map[_mid].str, SIR_MAXSTYLE);
        break;
    }

    _SIR_ITERATE_BIN_SEARCH((map[_mid].level < level ? 1 : -1));
    _SIR_END_BIN_SEARCH();

    _sir_unlocksection(SIRMI_TEXTSTYLE);

    assert(updated);
    return updated;


    return false;
}

sir_textstyle _sir_getdefstyle(sir_level level) {
    switch (level) {
        case SIRL_EMERG:  return sir_lvl_emerg_def_style;
        case SIRL_ALERT:  return sir_lvl_alert_def_style;
        case SIRL_CRIT:   return sir_lvl_crit_def_style;
        case SIRL_ERROR:  return sir_lvl_error_def_style;
        case SIRL_WARN:   return sir_lvl_warn_def_style;
        case SIRL_NOTICE: return sir_lvl_notice_def_style;
        case SIRL_INFO:   return sir_lvl_info_def_style;
        case SIRL_DEBUG:  return sir_lvl_debug_def_style;
        default: /* this should never happen. */
            assert(!"invalid sir_level");
            return SIRS_INVALID;
    }
}

bool _sir_resettextstyles(void) {
    sir_level_style_tuple* map = _sir_locksection(SIRMI_TEXTSTYLE);
    if (!map) {
        _sir_seterror(_SIR_E_INTERNAL);
        return false;
    }

    bool all_ok = true;
    for (size_t n = 0; n < SIR_NUMLEVELS; n++) {
        map[n].style = _sir_getdefstyle(map[n].level);
        all_ok &= _sir_formatstyle(map[n].style, map[n].str, SIR_MAXSTYLE);
    }

    _sir_unlocksection(SIRMI_TEXTSTYLE);
    return all_ok;
}

uint16_t _sir_getprivstyle(sir_textstyle style) {
    static const size_t idx_attr_start = 0;
    static const size_t idx_attr_end   = 2;

    static const size_t idx_fg_start = 3;
    static const size_t idx_fg_end   = 19;

    static const size_t idx_bg_start = 20;
    static const size_t idx_bg_end   = _sir_countof(sir_style_16color_map) - 1;

    size_t low  = 0;
    size_t high = 0;

    if (style <= SIRS_DIM) {
        /* looking up an attribute */
        low  = idx_attr_start;
        high = idx_attr_end;
    } else if (style <= SIRS_FG_WHITE) {
        /* looking up a foreground color */
        low  = idx_fg_start;
        high = idx_fg_end;
    } else {
        /* looking up a background color */
        low  = idx_bg_start;
        high = idx_bg_end;
    }

    _SIR_DECLARE_BIN_SEARCH(low, high);
    _SIR_BEGIN_BIN_SEARCH();

    if (sir_style_16color_map[_mid].from == style)
        return sir_style_16color_map[_mid].to;

    _SIR_ITERATE_BIN_SEARCH((sir_style_16color_map[_mid].from < style ? 1 : -1));
    _SIR_END_BIN_SEARCH();

    return _sir_getprivstyle(SIRS_FG_DEFAULT);
}

bool _sir_formatstyle(sir_textstyle style, char* buf, size_t size) {
    if (_sir_validptr(buf)) {
        uint32_t attr = 0;
        uint32_t fg   = 0;
        uint32_t bg   = 0;

        if (_sir_validstyle(style, &attr, &fg, &bg)) {
            uint16_t privattr = _sir_getprivstyle(attr);
            uint16_t privfg   = _sir_getprivstyle(fg);
            uint16_t privbg   = _sir_getprivstyle(bg);

            char fgfmt[7] = {0};
            char bgfmt[7] = {0};

            if (privfg != 0)
                snprintf(fgfmt, 7, ";%hu", privfg);

            if (privbg != 0)
                snprintf(bgfmt, 7, ";%hu", privbg);

            /* '\x1b[n;nnn;nnnm' */
            snprintf(buf, size, "%s%hu%s%sm", SIR_ESC, privattr, fgfmt, bgfmt);

            return _sir_validstr(buf);
        }
    }

    return false;
}

bool _sir_validstyle(sir_textstyle style, uint32_t* pattr, uint32_t* pfg, uint32_t* pbg) {
    uint32_t attr = (style & _SIRS_ATTR_MASK);
    uint32_t fore = (style & _SIRS_FG_MASK);
    uint32_t back = (style & _SIRS_BG_MASK);

    bool attrvalid = attr <= SIRS_DIM;
    bool fgvalid   = fore <= SIRS_FG_WHITE;
    bool bgvalid   = back <= SIRS_BG_WHITE && !_SIRS_SAME_COLOR(fore, back);

#pragma message( \
        "TODO: See if we can't rearrange the values and bitmasks for these values so things like 'SIRS_FG_RED | SIRS_FG_DEFAULT' can be invalidated (right now that == SIRS_FG_DGRAY")

    if (_sir_validptrnofail(pattr))
        *pattr = attrvalid ? attr : 0;

    if (_sir_validptrnofail(pfg))
        *pfg = fgvalid ? fore : 0;

    if (_sir_validptrnofail(pbg))
        *pbg = bgvalid ? back : 0;

    if (attrvalid && fgvalid && bgvalid)
        return true;

    _sir_seterror(_SIR_E_TEXTSTYLE);
    assert("!invalid text style");

    return false;
}
