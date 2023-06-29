/*
 * sirmaps.c
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
#include "sirmaps.h"
#include "sirdefaults.h"

/**
 * @brief Mapping of ::sir_level <-> ::sir_textstyle & ANSI escape codes.
 *
 * ::sir_settextstyle and ::sir_resettextstyles modify the style
 * values in this array at runtime; only the SIRL_* values are constant.
 */
sir_level_style_tuple sir_level_to_style_map[SIR_NUMLEVELS] = {
    {SIRL_EMERG,  SIRS_INVALID, {0}},
    {SIRL_ALERT,  SIRS_INVALID, {0}},
    {SIRL_CRIT,   SIRS_INVALID, {0}},
    {SIRL_ERROR,  SIRS_INVALID, {0}},
    {SIRL_WARN,   SIRS_INVALID, {0}},
    {SIRL_NOTICE, SIRS_INVALID, {0}},
    {SIRL_INFO,   SIRS_INVALID, {0}},
    {SIRL_DEBUG,  SIRS_INVALID, {0}}
};

/**
 * @brief Mapping of ::sir_level <-> human-readable string forms.
 *
 * ::_sir_formattedlevelstr retrieves values from this array.
 */
sir_level_str_pair sir_level_to_str_map[SIR_NUMLEVELS] = {
    {SIRL_EMERG,  SIR_LEVELPREFIX SIRL_S_EMERG  SIR_LEVELSUFFIX},
    {SIRL_ALERT,  SIR_LEVELPREFIX SIRL_S_ALERT  SIR_LEVELSUFFIX},
    {SIRL_CRIT,   SIR_LEVELPREFIX SIRL_S_CRIT   SIR_LEVELSUFFIX},
    {SIRL_ERROR,  SIR_LEVELPREFIX SIRL_S_ERROR  SIR_LEVELSUFFIX},
    {SIRL_WARN,   SIR_LEVELPREFIX SIRL_S_WARN   SIR_LEVELSUFFIX},
    {SIRL_NOTICE, SIR_LEVELPREFIX SIRL_S_NOTICE SIR_LEVELSUFFIX},
    {SIRL_INFO,   SIR_LEVELPREFIX SIRL_S_INFO   SIR_LEVELSUFFIX},
    {SIRL_DEBUG,  SIR_LEVELPREFIX SIRL_S_DEBUG  SIR_LEVELSUFFIX}
};

/**
 * @brief Mapping of ::sir_textstyle <-> values used to generate
 * styled terminal output for 4-bit (16-color) mode.
 *
 * @attention Entries *must* remain in numeric ascending order
 * (by SIRS_*); binary search is used to look up entries based
 * on those values.
 */
const sir_style_16color_pair sir_style_16color_map[SIR_NUM16_COLOR_MAPPINGS] = {
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
