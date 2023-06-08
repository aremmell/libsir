/**
 * @file sirtextstyle.h
 * @brief stdio text styling.
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
#ifndef _SIR_TEXTSTYLE_H_INCLUDED
#define _SIR_TEXTSTYLE_H_INCLUDED

#include "sirtypes.h"

/** Escape sequences:
 * 
 *  16 colors:      \033[<dim=0..2>;<fg=30..37, 90..97>;<bg=40..47, 100..107>m
 *  256 colors:     \033[<fg=38|bg=48>;5;<0..255>m
 *  24-bit RGB:     \033[<fg=38|bg=48>;2;<R=0..255>;<G=0..255>;<B=0..255>m
 *  Clear all:      \033[0m
 * 
 *  Note: evidently, you can chain these sequences to set both fore and back simultaneously:
 *    \033[38;5;128;48;5;206m
 *     
 *  - 30–37, 90–97 ("bright"):   Set foreground color (number = color ID)
 *  - 38:                        Set foreground color (with pattern to follow: 256 or 24-bit RGB color)
 *  - 39:                        Set default foreground color
 *  - 40–47, 100–107 ("bright"): Set background color (number = color ID)
 *  - 48:                        Set background color (with pattern to follow: 256 or 24-bit RGB color)
 *  - 49:                        Set default background color
 *    === to test =====================================================================================
 *  - 4:                         Underline (off: 24)
 *  - 5:                         Blinking (off: 25)
 *  - 51:                        Framed (off: 54)
 *  - 52:                        Encircled (off: 54)
 *  - 53:                        Overlined (off: 55)
 *  -  1:                        Bold (off: 21)
 * 
 * The 24-bit RGB spectrum is laid out as follows:
 * 
 * 0x00-0x07:  standard colors (same as the 4-bit colors)
 * 0x08-0x0F:  high intensity colors
 * 0x10-0xE7:  6 × 6 × 6 cube (216 colors): 16 + 36 × r + 6 × g + b (0 ≤ r, g, b ≤ 5)
 * 0xE8-0xFF:  grayscale from black to white in 24 steps
 */

// TODO: reminder that these are partially already defined in sirconfig.h
#define SIR_ESC_START "\x1b["
#define SIR_ESC_END   "m"

/** A couple fun characters that can be sent to stdio. */
#define SIR_R_ARROW "\xe2\x86\x92"
#define SIR_L_ARROW "\xe2\x86\x90"
#define SIR_BULLET  "\xe2\x80\xa2"

/** The maximum size of a color escape code sequence in bytes. */
// TODO

/** Validates a ::sir_textstyle and splits it into its component parts. */
bool _sir_validstyle(sir_textstyle style, uint32_t* pattr, uint32_t* pfg,
    uint32_t* pbg);

/**
 * Retrieves the override ::sir_textstyle for a ::sir_level if one is set.
 * Otherwise, returns the default text style for that level.
 */
sir_textstyle _sir_gettextstyle(sir_level level);

/** Retrieves the default ::sir_textstyle for a ::sir_level. */
sir_textstyle _sir_getdefstyle(sir_level level);

/** Sets the ::sir_textstyle for a ::sir_level. */
bool _sir_settextstyle(sir_level level, sir_textstyle style);

/** Resets all per-level ::sir_textstyle to default. */
bool _sir_resettextstyles(void);

/** Retrieves the opaque numeric value for a component part of a ::sir_textstyle. */
uint16_t _sir_getprivstyle(uint32_t style);

/** Combines component parts of a platform text style value into its final form. */
bool _sir_formatstyle(sir_textstyle style, sirchar_t* buf, size_t size);

#endif /* !_SIR_TEXTSTYLE_H_INCLUDED */
