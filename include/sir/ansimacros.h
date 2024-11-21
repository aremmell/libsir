/**
 * @file ansimacros.h
 *
 * @brief ANSI escape sequence macros
 *
 * A collection of macros that can be used with libsir's logging functions as well
 * as [f]printf, puts, etc. to manipulate terminal output.
 *
 * **Example**
 *   ~~~
 *   printf(RED("Something terrible happened!" SIR_EOL));
 *   ~~~
 *
 * @version 2.2.6
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

#ifndef _SIR_ANSI_MACROS_H_INCLUDED
# define _SIR_ANSI_MACROS_H_INCLUDED

# if !defined(SIR_NO_TEXT_STYLING)
#  define SIR_ESC   "\x1b[" /**< Begins an ANSI escape sequence. */
#  define SIR_ESC_M "m"     /**< Marks the end of a sequence. */
# else
#  define SIR_ESC   ""
#  define SIR_ESC_M ""
# endif

# if !defined(SIR_NO_TEXT_STYLING)
#  define SIR_ESC_SEQ(codes, s) SIR_ESC codes SIR_ESC_M s
# else
#  define SIR_ESC_SEQ(codes, s) s
# endif

# define SIR_ESC_SEQE(codes)   SIR_ESC_SEQ(codes, "")

/** Resets all previously applied colors and effects/attributes. */
# if !defined(SIR_NO_TEXT_STYLING)
#  define SIR_ESC_RST SIR_ESC_SEQE("0")
# else
#  define SIR_ESC_RST ""
# endif

/** A few fun characters. */
# if !defined(__WIN__)
#  define SIR_R_ARROW "\xe2\x86\x92"
#  define SIR_L_ARROW "\xe2\x86\x90"
#  define SIR_BULLET  "\xe2\x80\xa2"
# else /* __WIN__ */
#  define SIR_R_ARROW "->"
#  define SIR_L_ARROW "<-"
#  define SIR_BULLET  "-"
# endif

/**
 * Creates a sequence of colored characters 's' with foreground color 'fg',
 * background color 'bg', and attributes 'attr' (`0=normal, 1=bold, 2=dim,
 * 3=italic, 4=underlined, 5=blinking, 7=inverted, 9=strikethrough`).
 * Ends by resetting to the default fg/bg color, and normal attr.
 */
# if !defined(SIR_NO_TEXT_STYLING)
#  define SIR_COLOR(attr, fg, bg, s) \
                           SIR_ESC_SEQ(#attr ";" #fg ";" #bg, s) SIR_ESC_SEQE("0;39;49")

#  define SIR_STRIKE(s)    SIR_ESC_SEQ("9", s) SIR_ESC_SEQE("29") /**< Strike-through. */
#  define SIR_INVERT(s)    SIR_ESC_SEQ("7", s) SIR_ESC_SEQE("27") /**< Inverted fg/bg. */
#  define SIR_ULINE(s)     SIR_ESC_SEQ("4", s) SIR_ESC_SEQE("24") /**< Underlined. */
#  define SIR_EMPH(s)      SIR_ESC_SEQ("3", s) SIR_ESC_SEQE("23") /**< Emphasis/italic. */
#  define SIR_BOLD(s)      SIR_ESC_SEQ("1", s) SIR_ESC_SEQE("22") /**< Bold. */
#  define SIR_BLINK(s)     SIR_ESC_SEQ("5", s) SIR_ESC_SEQE("25") /**< Blinking text. */

#  define SIR_BLACK(s)     SIR_COLOR(0, 30, 49, s) /**< Black foreground text. */
#  define SIR_BLACKB(s)    SIR_COLOR(1, 30, 49, s) /**< Bold black foreground text. */

#  define SIR_RED(s)       SIR_COLOR(0, 31, 49, s) /**< Red foreground text. */
#  define SIR_REDB(s)      SIR_COLOR(1, 31, 49, s) /**< Bold red foreground text. */
#  define SIR_BRED(s)      SIR_COLOR(0, 91, 49, s) /**< Bright red foreground text. */
#  define SIR_BREDB(s)     SIR_COLOR(1, 91, 49, s) /**< Bold bright red foreground text. */

#  define SIR_GREEN(s)     SIR_COLOR(0, 32, 49, s) /**< Green foreground text. */
#  define SIR_GREENB(s)    SIR_COLOR(1, 32, 49, s) /**< Bold green foreground text. */
#  define SIR_BGREEN(s)    SIR_COLOR(0, 92, 49, s) /**< Bright green foreground text. */
#  define SIR_BGREENB(s)   SIR_COLOR(1, 92, 49, s) /**< Bold bright green foreground text. */

#  define SIR_YELLOW(s)    SIR_COLOR(0, 33, 49, s) /**< Yellow foreground text. */
#  define SIR_YELLOWB(s)   SIR_COLOR(1, 33, 49, s) /**< Bold yellow foreground text. */
#  define SIR_BYELLOW(s)   SIR_COLOR(0, 93, 49, s) /**< Bright yellow foreground text. */
#  define SIR_BYELLOWB(s)  SIR_COLOR(1, 93, 49, s) /**< Bold bright yellow foreground text. */

#  define SIR_BLUE(s)      SIR_COLOR(0, 34, 49, s) /**< Blue foreground text. */
#  define SIR_BLUEB(s)     SIR_COLOR(1, 34, 49, s) /**< Bold blue foreground text. */
#  define SIR_BBLUE(s)     SIR_COLOR(0, 94, 49, s) /**< Bright blue foreground text. */
#  define SIR_BBLUEB(s)    SIR_COLOR(1, 94, 49, s) /**< Bold bright blue foreground text. */

#  define SIR_MAGENTA(s)   SIR_COLOR(0, 35, 49, s) /**< Magenta foreground text. */
#  define SIR_MAGENTAB(s)  SIR_COLOR(1, 35, 49, s) /**< Bold magenta foreground text. */
#  define SIR_BMAGENTA(s)  SIR_COLOR(0, 95, 49, s) /**< Bright magenta foreground text. */
#  define SIR_BMAGENTAB(s) SIR_COLOR(1, 95, 49, s) /**< Bold bright magenta foreground text. */

#  define SIR_CYAN(s)      SIR_COLOR(0, 36, 49, s) /**< Cyan foreground text. */
#  define SIR_CYANB(s)     SIR_COLOR(1, 36, 49, s) /**< Bold cyan foreground text. */
#  define SIR_BCYAN(s)     SIR_COLOR(0, 96, 49, s) /**< Bright cyan foreground text. */
#  define SIR_BCYANB(s)    SIR_COLOR(1, 96, 49, s) /**< Bold bright cyan foreground text. */

#  define SIR_BGRAY(s)     SIR_COLOR(0, 37, 49, s) /**< Bright gray foreground text. */
#  define SIR_BGRAYB(s)    SIR_COLOR(1, 37, 49, s) /**< Bold bright gray foreground text. */
#  define SIR_DGRAY(s)     SIR_COLOR(0, 90, 49, s) /**< Dark gray foreground text. */
#  define SIR_DGRAYB(s)    SIR_COLOR(1, 90, 49, s) /**< Bold dark gray foreground text. */

#  define SIR_WHITE(s)     SIR_COLOR(0, 97, 49, s) /**< White foreground text. */
#  define SIR_WHITEB(s)    SIR_COLOR(1, 97, 49, s) /**< Bold white foreground text. */
# else /* SIR_NO_TEXT_STYLING */
#  define SIR_COLOR(attr, fg, bg, s) s

#  define SIR_STRIKE(s)    s
#  define SIR_INVERT(s)    s
#  define SIR_ULINE(s)     s
#  define SIR_EMPH(s)      s
#  define SIR_BOLD(s)      s
#  define SIR_BLINK(s)     s

#  define SIR_BLACK(s)     s
#  define SIR_BLACKB(s)    s

#  define SIR_RED(s)       s
#  define SIR_REDB(s)      s
#  define SIR_BRED(s)      s
#  define SIR_BREDB(s)     s

#  define SIR_GREEN(s)     s
#  define SIR_GREENB(s)    s
#  define SIR_BGREEN(s)    s
#  define SIR_BGREENB(s)   s

#  define SIR_YELLOW(s)    s
#  define SIR_YELLOWB(s)   s
#  define SIR_BYELLOW(s)   s
#  define SIR_BYELLOWB(s)  s

#  define SIR_BLUE(s)      s
#  define SIR_BLUEB(s)     s
#  define SIR_BBLUE(s)     s
#  define SIR_BBLUEB(s)    s

#  define SIR_MAGENTA(s)   s
#  define SIR_MAGENTAB(s)  s
#  define SIR_BMAGENTA(s)  s
#  define SIR_BMAGENTAB(s) s

#  define SIR_CYAN(s)      s
#  define SIR_CYANB(s)     s
#  define SIR_BCYAN(s)     s
#  define SIR_BCYANB(s)    s

#  define SIR_BGRAY(s)     s
#  define SIR_BGRAYB(s)    s
#  define SIR_DGRAY(s)     s
#  define SIR_DGRAYB(s)    s

#  define SIR_WHITE(s)     s
#  define SIR_WHITEB(s)    s
# endif
#endif /* ! _SIR_ANSI_MACROS_H_INCLUDED */
