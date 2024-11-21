/*
 * sirtextstyle.c
 *
 * Version: 2.2.6
 *
 * -----------------------------------------------------------------------------
 *
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2018-2024 Ryan M. Lederman <lederman@gmail.com>
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
 *
 * -----------------------------------------------------------------------------
 */

#include "sir/textstyle.h"
#include "sir/internal.h"
#include "sir/defaults.h"

#if !defined(SIR_NO_TEXT_STYLING)
static sir_colormode sir_color_mode = SIRCM_16;

/**
 * Wrapper around the level-to-style map and the color mode. This is the data
 * structure protected by the SIRMI_TEXTSTYLE mutex. */
sir_text_style_data sir_text_style_section = {
    &sir_level_to_style_map[0],
    &sir_color_mode
};

const char* _sir_gettextstyle(sir_level level) {
    static const size_t low  = 0;
    static const size_t high = SIR_NUMLEVELS - 1;

    _SIR_LOCK_SECTION(sir_text_style_data, data, SIRMI_TEXTSTYLE, NULL);
    const char* retval = SIR_UNKNOWN;

    _SIR_DECLARE_BIN_SEARCH(low, high);
    _SIR_BEGIN_BIN_SEARCH()

    if (data->map[_mid].level == level) {
        retval = data->map[_mid].str;
        break;
    }

    _SIR_ITERATE_BIN_SEARCH((data->map[_mid].level < level ? 1 : -1));
    _SIR_END_BIN_SEARCH();
    _SIR_UNLOCK_SECTION(SIRMI_TEXTSTYLE);

    return retval;
}

bool _sir_settextstyle(sir_level level, const sir_textstyle* style) {
    (void)_sir_seterror(_SIR_E_NOERROR);

    if (!_sir_sanity() || !_sir_validlevel(level))
        return false;

    _SIR_LOCK_SECTION(sir_text_style_data, data, SIRMI_TEXTSTYLE, false);
    bool updated              = false;
    static const size_t low   = 0;
    static const size_t high  = SIR_NUMLEVELS - 1;

    _SIR_DECLARE_BIN_SEARCH(low, high);
    _SIR_BEGIN_BIN_SEARCH()

    if (data->map[_mid].level == level) {
        (void)memcpy(&data->map[_mid].style, style, sizeof(sir_textstyle));
        updated = _sir_formatstyle(*data->color_mode, style, data->map[_mid].str);
        break;
    }

    _SIR_ITERATE_BIN_SEARCH((data->map[_mid].level < level ? 1 : -1));
    _SIR_END_BIN_SEARCH();
    _SIR_UNLOCK_SECTION(SIRMI_TEXTSTYLE);

    SIR_ASSERT(updated);
    return updated;
}

const sir_textstyle* _sir_getdefstyle(sir_level level) {
    switch (level) {
        case SIRL_EMERG:  return &sir_lvl_emerg_def_style;
        case SIRL_ALERT:  return &sir_lvl_alert_def_style;
        case SIRL_CRIT:   return &sir_lvl_crit_def_style;
        case SIRL_ERROR:  return &sir_lvl_error_def_style;
        case SIRL_WARN:   return &sir_lvl_warn_def_style;
        case SIRL_NOTICE: return &sir_lvl_notice_def_style;
        case SIRL_INFO:   return &sir_lvl_info_def_style;
        case SIRL_DEBUG:  return &sir_lvl_debug_def_style;
        // GCOVR_EXCL_START
        default: /* this should never happen */
            SIR_ASSERT(level);
            return &sir_lvl_info_def_style;
        // GCOVR_EXCL_STOP
    }
}

bool _sir_resettextstyles(void) {
    (void)_sir_seterror(_SIR_E_NOERROR);

    if (!_sir_sanity())
        return false;

    _SIR_LOCK_SECTION(sir_text_style_data, data, SIRMI_TEXTSTYLE, false);
    bool all_ok = true;
    for (size_t n = 0; n < SIR_NUMLEVELS; n++) {
        (void)memcpy(&data->map[n].style, _sir_getdefstyle(data->map[n].level),
             sizeof(sir_textstyle));
        _sir_eqland(all_ok, _sir_formatstyle(*data->color_mode, &data->map[n].style,
            data->map[n].str));
    }

    _SIR_UNLOCK_SECTION(SIRMI_TEXTSTYLE);
    return all_ok;
}

bool _sir_formatstyle(sir_colormode mode, const sir_textstyle* style,
    char buf[SIR_MAXSTYLE]) {
    if (!_sir_validtextstyle(mode, style))
        return false;

    _sir_resetstr(buf);

    switch (mode) {
        case SIRCM_16:
            /* \x1b[attr;fg;bgm */
            return 0 < snprintf(buf, SIR_MAXSTYLE, "%s%"PRIu8";%"PRIu8";%"PRIu8"m",
                SIR_ESC, (uint8_t)style->attr, (uint8_t)_sir_mkansifgcolor(style->fg),
                (uint8_t)_sir_mkansibgcolor(style->bg));
        case SIRCM_256: {
            /* \x1b[attr;38;5;fg;48;5;bgm */
            return 0 < snprintf(buf, SIR_MAXSTYLE,
                "%s%"PRIu8";%"PRIu8";5;%"PRIu8";%"PRIu8";5;%"PRIu8"m", SIR_ESC,
                (uint8_t)style->attr, (uint8_t)_sir_getansifgcmd(style->fg),
                (uint8_t)style->fg, (uint8_t)_sir_getansibgcmd(style->bg),
                (uint8_t)style->bg);
        }
        case SIRCM_RGB: {
            /* \x1b[attr;38;2;rrr;ggg;bbb;48;2;rrr;ggg;bbbm */
            return 0 < snprintf(buf, SIR_MAXSTYLE,
                "%s%"PRIu8";%"PRIu8";2;%"PRIu8";%"PRIu8";%"PRIu8";%"PRIu8
                ";2;%"PRIu8";%"PRIu8";%"PRIu8"m", SIR_ESC, (uint8_t)style->attr,
                (uint8_t)_sir_getansifgcmd(style->fg), _sir_getredfromcolor(style->fg),
                _sir_getgreenfromcolor(style->fg), _sir_getbluefromcolor(style->fg),
                (uint8_t)_sir_getansibgcmd(style->bg), _sir_getredfromcolor(style->bg),
                _sir_getgreenfromcolor(style->bg), _sir_getbluefromcolor(style->bg));
        }
        case SIRCM_INVALID: // GCOVR_EXCL_START
        default:
            return false;
    } // GCOVR_EXCL_STOP
}

bool _sir_validtextstyle(sir_colormode mode, const sir_textstyle* style) {
    if (!_sir_validptr(style) || !_sir_validcolormode(mode))
        return false;

    sir_textcolor fg = SIRCM_16 == mode ? _sir_mkansifgcolor(style->fg) : style->fg;
    sir_textcolor bg = SIRCM_16 == mode ? _sir_mkansibgcolor(style->bg) : style->bg;

    if (!_sir_validtextattr(style->attr) || !_sir_validtextcolor(mode, fg) ||
        !_sir_validtextcolor(mode, bg))
        return false;

    if (SIRTC_DEFAULT != style->fg && SIRTC_DEFAULT != style->bg &&
        style->fg == style->bg) {
        _sir_selflog("error: fg color %08"PRIx32" and bg color %08"PRIx32
                     " are identical; text would be invisible", style->fg,
                     style->bg);
        SIR_ASSERT(SIRTC_DEFAULT != style->fg && SIRTC_DEFAULT != style->bg &&
                style->fg == style->bg);
        return _sir_seterror(_SIR_E_TEXTSTYLE);
    }

    return true;
}

bool _sir_setcolormode(sir_colormode mode) {
    if (!_sir_validcolormode(mode))
        return false;

    _SIR_LOCK_SECTION(sir_text_style_data, data, SIRMI_TEXTSTYLE, false);
    if (*data->color_mode != mode) {
        sir_colormode old = *data->color_mode;
        *data->color_mode = mode;
        _sir_selflog("color mode changed from %"PRId32" to %"PRId32, old, mode);

        /* when the color mode changes, it's necessary to regenerate the text styles
         * we're holding. for example in the case of downgrading color modes, the
         * styles in memory could be incompatible with the new mode. */
        if (!_sir_resettextstyles()) {
            _sir_selflog("error: failed to reset text styles!");
            _SIR_UNLOCK_SECTION(SIRMI_TEXTSTYLE);
            return false;
        }
    } else {
        _sir_selflog("skipped superfluous update of color mode: %"PRId32, mode);
    }
    _SIR_UNLOCK_SECTION(SIRMI_TEXTSTYLE);

    return true;
}
#else /* SIR_NO_TEXT_STYLING */
bool _sir_settextstyle(sir_level level, const sir_textstyle* style) {
    SIR_UNUSED(level);
    SIR_UNUSED(style);
    return false;
}

const char* _sir_gettextstyle(sir_level level) { // GCOVR_EXCL_START
    SIR_UNUSED(level);
    SIR_ASSERT(false);
    return NULL;
}

const sir_textstyle* _sir_getdefstyle(sir_level level) {
    SIR_UNUSED(level);
    SIR_ASSERT(false);
    return NULL;
}

bool _sir_resettextstyles(void) {
    return false;
}

bool _sir_formatstyle(sir_colormode mode, const sir_textstyle* style,
    char buf[SIR_MAXSTYLE]) {
    SIR_UNUSED(mode);
    SIR_UNUSED(style);
    SIR_UNUSED(buf);
    return false;
}

bool _sir_validtextstyle(sir_colormode mode, const sir_textstyle* style) {
    SIR_UNUSED(mode);
    SIR_UNUSED(style);
    return false;
}

bool _sir_setcolormode(sir_colormode mode) {
    SIR_UNUSED(mode);
    return false;
} // GCOVR_EXCL_STOP
#endif
