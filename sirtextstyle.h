/**
 * @file sirtextstyle.h
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
#ifndef _SIR_TEXTSTYLE_H_INCLUDED
#define _SIR_TEXTSTYLE_H_INCLUDED

#include "sirtypes.h"

/**
 * @addtogroup intern
 * @{
 */

/** Overrides for level <> text style mappings (::sir_settextstyle). */
static sir_style_map sir_override_styles[SIR_NUMLEVELS] = {
    {SIRL_DEBUG, SIRS_INVALID},
    {SIRL_INFO, SIRS_INVALID},
    {SIRL_NOTICE, SIRS_INVALID},
    {SIRL_WARN, SIRS_INVALID},
    {SIRL_ERROR, SIRS_INVALID},
    {SIRL_CRIT, SIRS_INVALID},
    {SIRL_ALERT, SIRS_INVALID},
    {SIRL_EMERG, SIRS_INVALID},
};

/** mapping of ::sir_textstyle <> platform values. */
static const sir_style_priv_map sir_priv_map[] = {

#ifndef _WIN32
    {SIRS_NONE, 0},
    {SIRS_BRIGHT, 1},
    {SIRS_DIM, 2},
    /* foreground */
    {SIRS_FG_BLACK, 30},
    {SIRS_FG_RED, 31},
    {SIRS_FG_GREEN, 32},
    {SIRS_FG_YELLOW, 33},
    {SIRS_FG_BLUE, 34},
    {SIRS_FG_MAGENTA, 35},
    {SIRS_FG_CYAN, 36},
    {SIRS_FG_LGRAY, 37},
    {SIRS_FG_DGRAY, 90},
    {SIRS_FG_LRED, 91},
    {SIRS_FG_LGREEN, 92},
    {SIRS_FG_LYELLOW, 93},
    {SIRS_FG_LBLUE, 94},
    {SIRS_FG_LMAGENTA, 95},
    {SIRS_FG_LCYAN, 96},
    {SIRS_FG_WHITE, 97},
    {SIRS_FG_DEFAULT, 39},
    /* background */
    {SIRS_BG_BLACK, 40},
    {SIRS_BG_RED, 41},
    {SIRS_BG_GREEN, 42},
    {SIRS_BG_YELLOW, 43},
    {SIRS_BG_BLUE, 44},
    {SIRS_BG_MAGENTA, 45},
    {SIRS_BG_CYAN, 46},
    {SIRS_BG_LGRAY, 47},
    {SIRS_BG_DGRAY, 100},
    {SIRS_BG_LRED, 101},
    {SIRS_BG_LGREEN, 102},
    {SIRS_BG_LYELLOW, 103},
    {SIRS_BG_LBLUE, 104},
    {SIRS_BG_LMAGENTA, 105},
    {SIRS_BG_LCYAN, 106},
    {SIRS_BG_WHITE, 107},
    {SIRS_BG_DEFAULT, 49},
#else
    {SIRS_NONE, 0},
    {SIRS_BRIGHT, FOREGROUND_INTENSITY},
    {SIRS_DIM, 0},
    {SIRS_FG_BLACK, 0},
    {SIRS_FG_RED, FOREGROUND_RED},
    {SIRS_FG_GREEN, FOREGROUND_GREEN},
    {SIRS_FG_YELLOW, FOREGROUND_RED | FOREGROUND_GREEN},
    {SIRS_FG_BLUE, FOREGROUND_BLUE},
    {SIRS_FG_MAGENTA, FOREGROUND_RED | FOREGROUND_BLUE},
    {SIRS_FG_CYAN, FOREGROUND_GREEN | FOREGROUND_BLUE},
    {SIRS_FG_LGRAY, FOREGROUND_INTENSITY},
    {SIRS_FG_DGRAY, FOREGROUND_INTENSITY},
    {SIRS_FG_LRED, FOREGROUND_RED | FOREGROUND_INTENSITY},
    {SIRS_FG_LGREEN, FOREGROUND_GREEN | FOREGROUND_INTENSITY},
    {SIRS_FG_LYELLOW, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY},
    {SIRS_FG_LBLUE, FOREGROUND_BLUE | FOREGROUND_INTENSITY},
    {SIRS_FG_LMAGENTA, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY},
    {SIRS_FG_LCYAN, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY},
    {SIRS_FG_WHITE, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE},
    {SIRS_FG_DEFAULT, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE},

    {SIRS_BG_BLACK, 0},
    {SIRS_BG_RED, BACKGROUND_RED},
    {SIRS_BG_GREEN, BACKGROUND_GREEN},
    {SIRS_BG_YELLOW, BACKGROUND_RED | BACKGROUND_GREEN},
    {SIRS_BG_BLUE, BACKGROUND_BLUE},
    {SIRS_BG_MAGENTA, BACKGROUND_RED | BACKGROUND_BLUE},
    {SIRS_BG_CYAN, BACKGROUND_GREEN | BACKGROUND_BLUE},
    {SIRS_BG_LGRAY, BACKGROUND_INTENSITY},
    {SIRS_BG_DGRAY, BACKGROUND_INTENSITY},
    {SIRS_BG_LRED, BACKGROUND_RED | BACKGROUND_INTENSITY},
    {SIRS_BG_LGREEN, BACKGROUND_GREEN | BACKGROUND_INTENSITY},
    {SIRS_BG_LYELLOW, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY},
    {SIRS_BG_LBLUE, BACKGROUND_BLUE | BACKGROUND_INTENSITY},
    {SIRS_BG_LMAGENTA, BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY},
    {SIRS_BG_LCYAN, BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY},
    {SIRS_BG_WHITE, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE},
    {SIRS_BG_DEFAULT, 0},
#endif
};

/** Validates a ::sir_textstyle and splits it into its component parts. */
bool _sir_validstyle(sir_textstyle style, uint32_t* pattr, uint32_t* pfg, uint32_t* pbg);

/**
 * Retrieves the override ::sir_textstyle for a ::sir_level  if one is set.
 * Otherwise, returns the default text style for that level.
 */
sir_textstyle _sir_gettextstyle(sir_level level);

/** Retrieves the default ::sir_textstyle for a ::sir_level. */
sir_textstyle _sir_getdefstyle(const sir_style_map* map, sir_level level);

/** Sets the ::sir_textstyle for a ::sir_level. */
bool _sir_settextstyle(sir_level level, sir_textstyle style);

/** Resets all override ::sir_textstyle. */
bool _sir_resettextstyles(void);

/** Retrieves the platform value for a component part of a ::sir_textstyle. */
uint16_t _sir_getprivstyle(uint32_t cat);

/** Combines component parts of a platform text style value into its final form. */
bool _sir_formatstyle(sir_textstyle style, sirchar_t* buf, size_t size);

/** @} */

#endif /* !_SIR_TEXTSTYLE_H_INCLUDED */
