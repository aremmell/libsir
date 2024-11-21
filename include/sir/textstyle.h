/*
 * textstyle.h
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

#ifndef _SIR_TEXTSTYLE_H_INCLUDED
# define _SIR_TEXTSTYLE_H_INCLUDED

# include "sir/types.h"

extern sir_text_style_data sir_text_style_section;

/**  Returns the final string form of the current ::sir_textstyle for a ::sir_level. */
const char* _sir_gettextstyle(sir_level level);

/** Sets the ::sir_textstyle for a ::sir_level. */
bool _sir_settextstyle(sir_level level, const sir_textstyle* style);

/** Retrieves the default ::sir_textstyle for a ::sir_level. */
const sir_textstyle* _sir_getdefstyle(sir_level level);

/** Resets all per-level ::sir_textstyle to defaults. */
bool _sir_resettextstyles(void);

/** Creates the ANSI escape sequence that produces the associated text style. */
bool _sir_formatstyle(sir_colormode mode, const sir_textstyle* style,
    char buf[SIR_MAXSTYLE]);

/** Validates a ::sir_textstyle based on color mode. */
bool _sir_validtextstyle(sir_colormode mode, const sir_textstyle* style);

/** Sets the current color mode. */
bool _sir_setcolormode(sir_colormode mode);

#endif /* !_SIR_TEXTSTYLE_H_INCLUDED */
