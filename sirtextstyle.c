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

    SIR_ASSERT(updated);
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
            SIR_ASSERT(!"invalid sir_level");
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
    static const size_t idx_bg_end   = SIR_NUM16_COLOR_MAPPINGS - 1;

    size_t low  = 0;
    size_t high = 0;

    if ((style & _SIRS_ATTR_MASK) != 0) {
        /* looking up an attribute */
        low  = idx_attr_start;
        high = idx_attr_end;
    } else if ((style & _SIRS_FG_MASK) != 0) {
        /* looking up a foreground color */
        low  = idx_fg_start;
        high = idx_fg_end;
    } else if ((style & _SIRS_BG_MASK) != 0) {
        /* looking up a background color */
        low  = idx_bg_start;
        high = idx_bg_end;
    }

    /* skip if not assigned due to invalid input. */
    if (high > low) {
        for (size_t n = low; n <= high; n++) {
        if (sir_style_16color_map[n].from == style)
            return sir_style_16color_map[n].to;
        }
    }

    SIR_ASSERT("!invalid text style");
    return SIRS_INVALID;
}

bool _sir_formatstyle(sir_textstyle style, char* buf, size_t size) {
    if (_sir_validptr(buf)) {
        uint32_t attr = 0;
        uint32_t fg   = 0;
        uint32_t bg   = 0;

        if (_sir_validstyle(style, &attr, &fg, &bg)) {
            uint16_t privattr = 0 != attr ? _sir_getprivstyle(attr) : 0;
            uint16_t privfg   = 0 != fg   ? _sir_getprivstyle(fg)   : 0;
            uint16_t privbg   = 0 != bg   ? _sir_getprivstyle(bg)   : 0;

            char fgfmt[7] = {0};
            char bgfmt[7] = {0};

            if (0 != privfg && SIRS_INVALID != privfg)
                snprintf(fgfmt, 7, ";%hu", privfg);

            if (0 != privbg && SIRS_INVALID != privbg)
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
    bool fgvalid   = false;
    bool bgvalid   = false;
    bool fgbgsame  = (fore != SIRS_FG_DEFAULT && back != SIRS_BG_DEFAULT) &&
                      _SIRS_SAME_COLOR(fore, back);

    if (fgbgsame)
        _sir_selflog("error: fg color %08" PRIx32 " and bg color %08" PRIx32
                     " are identical; text would be invisible",
                     fore, back);

    if (!fgbgsame) {
        switch (fore) {
            case 0:
            case SIRS_FG_BLACK:
            case SIRS_FG_RED:
            case SIRS_FG_GREEN:
            case SIRS_FG_YELLOW:
            case SIRS_FG_BLUE:
            case SIRS_FG_MAGENTA:
            case SIRS_FG_CYAN:
            case SIRS_FG_LGRAY:
            case SIRS_FG_DEFAULT:
            case SIRS_FG_DGRAY:
            case SIRS_FG_LRED:
            case SIRS_FG_LGREEN:
            case SIRS_FG_LYELLOW:
            case SIRS_FG_LBLUE:
            case SIRS_FG_LMAGENTA:
            case SIRS_FG_LCYAN:
            case SIRS_FG_WHITE:
                fgvalid = true;
            break;
            default:
                _sir_selflog("error: %08" PRIx32 " is not a valid fg color!", fore);
            break;
        }
    }

    if (!fgbgsame) {
        switch (back) {
            case 0:
            case SIRS_BG_BLACK:
            case SIRS_BG_RED:
            case SIRS_BG_GREEN:
            case SIRS_BG_YELLOW:
            case SIRS_BG_BLUE:
            case SIRS_BG_MAGENTA:
            case SIRS_BG_CYAN:
            case SIRS_BG_LGRAY:
            case SIRS_BG_DEFAULT:
            case SIRS_BG_DGRAY:
            case SIRS_BG_LRED:
            case SIRS_BG_LGREEN:
            case SIRS_BG_LYELLOW:
            case SIRS_BG_LBLUE:
            case SIRS_BG_LMAGENTA:
            case SIRS_BG_LCYAN:
            case SIRS_BG_WHITE:
                bgvalid = true;
            break;
            default:
                _sir_selflog("error: %08" PRIx32 " is not a valid bg color!", back);
            break;
        }
    }

    if (_sir_validptrnofail(pattr))
        *pattr = attrvalid ? attr : 0;

    if (_sir_validptrnofail(pfg))
        *pfg = fgvalid ? fore : 0;

    if (_sir_validptrnofail(pbg))
        *pbg = bgvalid ? back : 0;

    if (attrvalid && fgvalid && bgvalid)
        return true;

    _sir_seterror(_SIR_E_TEXTSTYLE);
    SIR_ASSERT("!invalid text style");

    return false;
}
