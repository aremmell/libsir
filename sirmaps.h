/**
 * @file sirmaps.h
 * @brief Static arrays used to map values of varying types.
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
#ifndef _SIR_MAPS_H_INCLUDED
#define _SIR_MAPS_H_INCLUDED

#include "sirtypes.h"
#include "sirhelpers.h"

/**
 * @addtogroup intern
 * @{
 */

/**
 * @brief Overrides for ::sir_level <-> ::sir_textstyle mappings.
 *
 * ::sir_settextstyle sets (overrides) the style values in this array
 * at runtime; only the SIRL_* values are constant.
 * 
 * ::sir_default_style_map in @ref sirdefaults.h contains the constant,
 * default styles for each level.
 * 
 * @attention Entries *must* remain in numeric ascending order (by SIRL_*);
 * binary search is used to look up entries based on those values.
 */
static sir_level_style_pair sir_override_style_map[] = {
    {SIRL_EMERG,  SIRS_INVALID},
    {SIRL_ALERT,  SIRS_INVALID},
    {SIRL_CRIT,   SIRS_INVALID},
    {SIRL_ERROR,  SIRS_INVALID},
    {SIRL_WARN,   SIRS_INVALID},
    {SIRL_NOTICE, SIRS_INVALID},
    {SIRL_INFO,   SIRS_INVALID},
    {SIRL_DEBUG,  SIRS_INVALID}
};

/**
 * @brief Mapping of ::sir_level <-> string representation (@ref sirconfig.h)
 * 
 * ::_sir_levelstr obtains string representations from this array
 * for output formatting.
 * 
 * @attention Entries *must* remain in numeric ascending order (by SIRL_*);
 * binary search is used to look up entries based on those values.
 */
static const sir_level_str_pair sir_level_str_map[] = {
    {SIRL_EMERG,  SIRL_S_EMERG},
    {SIRL_ALERT,  SIRL_S_ALERT},
    {SIRL_CRIT,   SIRL_S_CRIT},
    {SIRL_ERROR,  SIRL_S_ERROR},
    {SIRL_WARN,   SIRL_S_WARN},
    {SIRL_NOTICE, SIRL_S_NOTICE},
    {SIRL_INFO,   SIRL_S_INFO},
    {SIRL_DEBUG,  SIRL_S_DEBUG}
};

/** 
 * @brief Mapping of ::sir_textstyle <-> values used to generate
 * styled terminal output for 4-bit (16 color) mode.
 * 
 * @attention Entries *must* remain in numeric ascending order
 * (by SIRS_*); binary search is used to look up entries based
 * on those values.
 */
static const sir_style_16color_pair sir_style_16color_map[] = {
    /* intensity */
    {SIRS_NONE,          0},
    {SIRS_BRIGHT,        1},
    {SIRS_DIM,           2},
    /* foreground color */
    {SIRS_FG_BLACK,     30},
    {SIRS_FG_RED,       31},
    {SIRS_FG_GREEN,     32},
    {SIRS_FG_YELLOW,    33},
    {SIRS_FG_BLUE,      34},
    {SIRS_FG_MAGENTA,   35},
    {SIRS_FG_CYAN,      36},
    {SIRS_FG_LGRAY,     37},
    {SIRS_FG_DEFAULT,   39},
    {SIRS_FG_DGRAY,     90},
    {SIRS_FG_LRED,      91},
    {SIRS_FG_LGREEN,    92},
    {SIRS_FG_LYELLOW,   93},
    {SIRS_FG_LBLUE,     94},
    {SIRS_FG_LMAGENTA,  95},
    {SIRS_FG_LCYAN,     96},
    {SIRS_FG_WHITE,     97},
    /* background color */
    {SIRS_BG_BLACK,     40},
    {SIRS_BG_RED,       41},
    {SIRS_BG_GREEN,     42},
    {SIRS_BG_YELLOW,    43},
    {SIRS_BG_BLUE,      44},
    {SIRS_BG_MAGENTA,   45},
    {SIRS_BG_CYAN,      46},
    {SIRS_BG_LGRAY,     47},
    {SIRS_BG_DEFAULT,   49},
    {SIRS_BG_DGRAY,    100},
    {SIRS_BG_LRED,     101},
    {SIRS_BG_LGREEN,   102},
    {SIRS_BG_LYELLOW,  103},
    {SIRS_BG_LBLUE,    104},
    {SIRS_BG_LMAGENTA, 105},
    {SIRS_BG_LCYAN,    106},
    {SIRS_BG_WHITE,    107}
};

/** @} */

#endif // !_SIR_MAPS_H_INCLUDED
