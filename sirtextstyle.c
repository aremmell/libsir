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
        return NULL;
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

bool _sir_settextstyle(sir_level level, const sir_textstyle* style) {
    _sir_seterror(_SIR_E_NOERROR);

    if (!_sir_sanity() || !_sir_validlevel(level))
        return false;

    sir_colormode mode = _sir_getcolormode();
    if (!_sir_validtextstyle(mode, style))
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
        memcpy(&map[_mid].style, style, sizeof(sir_textstyle));
        updated         = _sir_formatstyle(mode, style, map[_mid].str);
        break;
    }

    _SIR_ITERATE_BIN_SEARCH((map[_mid].level < level ? 1 : -1));
    _SIR_END_BIN_SEARCH();

    _sir_unlocksection(SIRMI_TEXTSTYLE);

    SIR_ASSERT(updated);
    return updated;
}

const sir_textstyle const* _sir_getdefstyle(sir_level level) {
    switch (level) {
        case SIRL_EMERG:  return &sir_lvl_emerg_def_style;
        case SIRL_ALERT:  return &sir_lvl_alert_def_style;
        case SIRL_CRIT:   return &sir_lvl_crit_def_style;
        case SIRL_ERROR:  return &sir_lvl_error_def_style;
        case SIRL_WARN:   return &sir_lvl_warn_def_style;
        case SIRL_NOTICE: return &sir_lvl_notice_def_style;
        case SIRL_INFO:   return &sir_lvl_info_def_style;
        case SIRL_DEBUG:  return &sir_lvl_debug_def_style;
        default: /* this should never happen. */
            SIR_ASSERT(!"invalid sir_level");
            return NULL;
    }
}

bool _sir_resettextstyles(void) {
    _sir_seterror(_SIR_E_NOERROR);

    if (!_sir_sanity())
        return false;

    sir_colormode mode = _sir_getcolormode();
    if (!_sir_validcolormode(mode))
        return false;

    sir_level_style_tuple* map = _sir_locksection(SIRMI_TEXTSTYLE);
    if (!map) {
        _sir_seterror(_SIR_E_INTERNAL);
        return false;
    }

    bool all_ok = true;
    for (size_t n = 0; n < SIR_NUMLEVELS; n++) {
        memcpy(&map[n].style, _sir_getdefstyle(map[n].level), sizeof(sir_textstyle));
        all_ok &= _sir_formatstyle(mode, &map[n].style, map[n].str);
    }

    _sir_unlocksection(SIRMI_TEXTSTYLE);
    return all_ok;
}

bool _sir_formatstyle(sir_colormode mode, const sir_textstyle* style,
    char buf[SIR_MAXSTYLE]) {
    if (!_sir_validtextstyle(mode, style))
        return false;

    _sir_resetstr(buf);

    bool formatted = false;
    switch (mode) {
        case SIRCM_16:
            /* \x1b[attr;fg;bgm */
            return 0 < snprintf(buf, SIR_MAXSTYLE, "%s%d;%d;%dm", SIR_ESC,
                style->attrs, style->fg, style->bg);
        break;
        case SIRCM_256:
            /* \x1b[attr;38;5;fg;48;5;bgm */
        break;
        case SIRCM_RGB:
            /* \x1b[attr;38;2;rrr;ggg;bbb;48;2;rrr;ggg;bbbm */
        break;
    }

    return formatted;
}

bool _sir_validtextstyle(sir_colormode mode, const sir_textstyle* style) {
    if (!_validptr(style) || !_sir_validcolormode(mode))
        return false;

    bool valid = false;
    if (!_sir_validtextattr(style->attrs) || !_sir_validtextcolor(mode, style))
        return false;

    if (style->fg == style->bg) {
        _sir_selflog("error: fg color %08" PRIx32 " and bg color %08" PRIx32
                     " are identical; text would be invisible", style->fg,
                     style->bg);
        // TODO: more validation
        _sir_seterror(_SIR_E_TEXTSTYLE);
        SIR_ASSERT("!invalid text style");
        return false;
    }

    return true;
}

sir_colormode _sir_getcolormode(void) {
    sirconfig* _cfg = _sir_locksection(SIRMI_CONFIG);
    if (!_cfg) {
        _sir_seterror(_SIR_E_INTERNAL);
        return SIRCM_INVALID;
    }

    sir_colormode mode = _cfg->si.color_mode;
    _sir_unlocksection(SIRMI_CONFIG);
    return mode;
}
