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
#define _SIR_TEXTSTYLE_H_INCLUDED

#include "sirtypes.h"

/**  Returns the final string form of the current ::sir_textstyle for a ::sir_level. */
const char* _sir_gettextstyle(sir_level level);

/** Sets the ::sir_textstyle for a ::sir_level. */
bool _sir_settextstyle(sir_level level, sir_textstyle style);

/** Retrieves the default ::sir_textstyle for a ::sir_level. */
sir_textstyle _sir_getdefstyle(sir_level level);

/** Resets all per-level ::sir_textstyle to default. */
bool _sir_resettextstyles(void);

/** Retrieves the opaque numeric value for a component part of a ::sir_textstyle. */
uint16_t _sir_getprivstyle(sir_textstyle style);

/** Combines component parts of a platform text style value into its final form. */
bool _sir_formatstyle(sir_textstyle style, char* buf, size_t size);

/** Validates a ::sir_textstyle and splits it into its component parts. */
bool _sir_validstyle(sir_textstyle style, uint32_t* pattr, uint32_t* pfg, uint32_t* pbg);

#endif /* !_SIR_TEXTSTYLE_H_INCLUDED */
