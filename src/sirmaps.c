/*
 * sirmaps.c
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

#include "sir/maps.h"
#include "sir/defaults.h"

/**
 * @brief Mapping of ::sir_level <-> ::sir_textstyle & ANSI escape codes.
 *
 * ::sir_settextstyle and ::sir_resettextstyles modify the style
 * values in this array at runtime; only the SIRL_* values are constant.
 */
#if !defined(SIR_NO_TEXT_STYLING)
sir_level_style_tuple sir_level_to_style_map[SIR_NUMLEVELS] = {
    {SIRL_EMERG,  {0}, {0}},
    {SIRL_ALERT,  {0}, {0}},
    {SIRL_CRIT,   {0}, {0}},
    {SIRL_ERROR,  {0}, {0}},
    {SIRL_WARN,   {0}, {0}},
    {SIRL_NOTICE, {0}, {0}},
    {SIRL_INFO,   {0}, {0}},
    {SIRL_DEBUG,  {0}, {0}}
};
#endif

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
