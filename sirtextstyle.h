/*
 * sirtextstyle.h
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
#ifndef _SIR_TEXTSTYLE_H_INCLUDED
# define _SIR_TEXTSTYLE_H_INCLUDED

# include "sirtypes.h"

/**  Returns the final string form of the current ::sir_textstyle for a ::sir_level. */
const char* _sir_gettextstyle(sir_level level);

/** Creates a ::sir_textstyle based on color mode. */
bool _sir_maketextstyle(sir_colormode mode, sir_textattr attrs, sir_textcolor fg,
    sir_textcolor bg, sir_textstyle* out);

/** Sets the ::sir_textstyle for a ::sir_level. */
bool _sir_settextstyle(sir_colormode mode, sir_level level,
    const sir_textstyle* style);

/** Retrieves the default ::sir_textstyle for a ::sir_level. */
const sir_textstyle const* _sir_getdefstyle(sir_level level);

/** Resets all per-level ::sir_textstyle to defaults. */
bool _sir_resettextstyles(sir_colormode mode);

/** Creates the ANSI escape sequence that produces the associated text style. */
bool _sir_formatstyle(sir_colormode mode, const sir_textstyle* style,
    char buf[SIR_MAXSTYLE]);

/** Validates a ::sir_textstyle based on color mode. */
bool _sir_validstyle(sir_colormode mode, const sir_textstyle* style);

#endif /* !_SIR_TEXTSTYLE_H_INCLUDED */
