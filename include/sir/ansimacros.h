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
 *   printf(RED("Something terrible happened!\n"));
 *   ~~~
 *
 * @version 2.2.5
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
# undef COLOR
# if !defined(SIR_NO_TEXT_STYLING)
#  define COLOR(attr, fg, bg, s) \
     SIR_ESC_SEQ(#attr ";" #fg ";" #bg, s) SIR_ESC_SEQE("0;39;49")
# else
#  define COLOR(attr, fg, bg, s) s
# endif

# undef STRIKE
# if !defined(SIR_NO_TEXT_STYLING)
#  define STRIKE(s) SIR_ESC_SEQ("9", s) SIR_ESC_SEQE("29") /**< Strike-through. */
# else
#  define STRIKE(s) s
# endif

# undef INVERT
# if !defined(SIR_NO_TEXT_STYLING)
#  define INVERT(s) SIR_ESC_SEQ("7", s) SIR_ESC_SEQE("27") /**< Inverted fg/bg. */
# else
#  define INVERT(s) s
# endif

# undef ULINE
# if !defined(SIR_NO_TEXT_STYLING)
#  define ULINE(s) SIR_ESC_SEQ("4", s) SIR_ESC_SEQE("24") /**< Underlined. */
# else
#  define ULINE(s) s
# endif

# undef EMPH
# if !defined(SIR_NO_TEXT_STYLING)
#  define EMPH(s) SIR_ESC_SEQ("3", s) SIR_ESC_SEQE("23") /**< Emphasis/italic. */
# else
#  define EMPH(s) s
# endif

# undef BOLD
# if !defined(SIR_NO_TEXT_STYLING)
#  define BOLD(s) SIR_ESC_SEQ("1", s) SIR_ESC_SEQE("22") /**< Bold. */
# else
#  define BOLD(s) s
# endif

# undef BLINK
# if !defined(SIR_NO_TEXT_STYLING)
#  define BLINK(s) SIR_ESC_SEQ("5", s) SIR_ESC_SEQE("25") /**< Blinking text. */
# else
#  define BLINK(s) s
# endif

# undef BLACK
# if !defined(SIR_NO_TEXT_STYLING)
#  define BLACK(s) COLOR(0, 30, 49, s) /**< Black foreground text. */
# else
#  define BLACK(s) s
# endif

# undef BLACKB
# if !defined(SIR_NO_TEXT_STYLING)
#  define BLACKB(s) COLOR(1, 30, 49, s) /**< Bold black foreground text. */
# else
#  define BLACKB(s) s
# endif

# undef RED
# if !defined(SIR_NO_TEXT_STYLING)
#  define RED(s) COLOR(0, 31, 49, s) /**< Red foreground text. */
# else
#  define RED(s) s
# endif

# undef REDB
# if !defined(SIR_NO_TEXT_STYLING)
#  define REDB(s) COLOR(1, 31, 49, s) /**< Bold red foreground text. */
# else
#  define REDB(s) s
# endif

# undef BRED
# if !defined(SIR_NO_TEXT_STYLING)
#  define BRED(s) COLOR(0, 91, 49, s) /**< Bright red foreground text. */
# else
#  define BRED(s) s
# endif

# undef BREDB
# if !defined(SIR_NO_TEXT_STYLING)
#  define BREDB(s) COLOR(1, 91, 49, s) /**< Bold bright red foreground text. */
# else
#  define BREDB(s) s
# endif

# undef GREEN
# if !defined(SIR_NO_TEXT_STYLING)
#  define GREEN(s) COLOR(0, 32, 49, s) /**< Green foreground text. */
# else
#  define GREEN(s) s
# endif

# undef GREENB
# if !defined(SIR_NO_TEXT_STYLING)
#  define GREENB(s) COLOR(1, 32, 49, s) /**< Bold green foreground text. */
# else
#  define GREENB(s) s
# endif

# undef BGREEN
# if !defined(SIR_NO_TEXT_STYLING)
#  define BGREEN(s) COLOR(0, 92, 49, s) /**< Bright green foreground text. */
# else
#  define BGREEN(s) s
# endif

# undef BGREENB
# if !defined(SIR_NO_TEXT_STYLING)
#  define BGREENB(s) COLOR(1, 92, 49, s) /**< Bold bright green foreground text. */
# else
#  define BGREENB(s) s
# endif

# undef YELLOW
# if !defined(SIR_NO_TEXT_STYLING)
#  define YELLOW(s) COLOR(0, 33, 49, s) /**< Yellow foreground text. */
# else
#  define YELLOW(s) s
# endif

# undef YELLOWB
# if !defined(SIR_NO_TEXT_STYLING)
#  define YELLOWB(s) COLOR(1, 33, 49, s) /**< Bold yellow foreground text. */
# else
#  define YELLOWB(s) s
# endif

# undef BYELLOW
# if !defined(SIR_NO_TEXT_STYLING)
#  define BYELLOW(s) COLOR(0, 93, 49, s) /**< Bright yellow foreground text. */
# else
#  define BYELLOW(s) s
# endif

# undef BYELLOWB
# if !defined(SIR_NO_TEXT_STYLING)
#  define BYELLOWB(s) COLOR(1, 93, 49, s) /**< Bold bright yellow foreground text. */
# else
#  define BYELLOWB(s) s
# endif

# undef BLUE
# if !defined(SIR_NO_TEXT_STYLING)
#  define BLUE(s) COLOR(0, 34, 49, s) /**< Blue foreground text. */
# else
#  define BLUE(s) s
# endif

# undef BLUEB
# if !defined(SIR_NO_TEXT_STYLING)
#  define BLUEB(s) COLOR(1, 34, 49, s) /**< Bold blue foreground text. */
# else
#  define BLUEB(s) s
# endif

# undef BBLUE
# if !defined(SIR_NO_TEXT_STYLING)
#  define BBLUE(s) COLOR(0, 94, 49, s) /**< Bright blue foreground text. */
# else
#  define BBLUE(s) s
# endif

# undef BBLUEB
# if !defined(SIR_NO_TEXT_STYLING)
#  define BBLUEB(s) COLOR(1, 94, 49, s) /**< Bold bright blue foreground text. */
# else
#  define BBLUEB(s) s
# endif

# undef MAGENTA
# if !defined(SIR_NO_TEXT_STYLING)
#  define MAGENTA(s) COLOR(0, 35, 49, s) /**< Magenta foreground text. */
# else
#  define MAGENTA(s) s
# endif

# undef MAGENTAB
# if !defined(SIR_NO_TEXT_STYLING)
#  define MAGENTAB(s) COLOR(1, 35, 49, s) /**< Bold magenta foreground text. */
# else
#  define MAGENTAB(s) s
# endif

# undef BMAGENTA
# if !defined(SIR_NO_TEXT_STYLING)
#  define BMAGENTA(s) COLOR(0, 95, 49, s) /**< Bright magenta foreground text. */
# else
#  define BMAGENTA(s) s
# endif

# undef BMAGENTAB
# if !defined(SIR_NO_TEXT_STYLING)
#  define BMAGENTAB(s) COLOR(1, 95, 49, s) /**< Bold bright magenta foreground text. */
# else
#  define BMAGENTAB(s) s
# endif

# undef CYAN
# if !defined(SIR_NO_TEXT_STYLING)
#  define CYAN(s) COLOR(0, 36, 49, s) /**< Cyan foreground text. */
# else
#  define CYAN(s) s
# endif

# undef CYANB
# if !defined(SIR_NO_TEXT_STYLING)
#  define CYANB(s) COLOR(1, 36, 49, s) /**< Bold cyan foreground text. */
# else
#  define CYANB(s) s
# endif

# undef BCYAN
# if !defined(SIR_NO_TEXT_STYLING)
#  define BCYAN(s) COLOR(0, 96, 49, s) /**< Bright cyan foreground text. */
# else
#  define BCYAN(s) s
# endif

# undef BCYANB
# if !defined(SIR_NO_TEXT_STYLING)
#  define BCYANB(s) COLOR(1, 96, 49, s) /**< Bold bright cyan foreground text. */
# else
#  define BCYANB(s) s
# endif

# undef BGRAY
# if !defined(SIR_NO_TEXT_STYLING)
#  define BGRAY(s) COLOR(0, 37, 49, s) /**< Bright gray foreground text. */
# else
#  define BGRAY(s) s
# endif

# undef BGRAYB
# if !defined(SIR_NO_TEXT_STYLING)
#  define BGRAYB(s) COLOR(1, 37, 49, s) /**< Bold bright gray foreground text. */
# else
#  define BGRAYB(s) s
# endif

# undef DGRAY
# if !defined(SIR_NO_TEXT_STYLING)
#  define DGRAY(s) COLOR(0, 90, 49, s) /**< Dark gray foreground text. */
# else
#  define DGRAY(s) s
# endif

# undef DGRAYB
# if !defined(SIR_NO_TEXT_STYLING)
#  define DGRAYB(s) COLOR(1, 90, 49, s) /**< Bold dark gray foreground text. */
# else
#  define DGRAYB(s) s
# endif

# undef WHITE
# if !defined(SIR_NO_TEXT_STYLING)
#  define WHITE(s) COLOR(0, 97, 49, s) /**< White foreground text. */
# else
#  define WHITE(s) s
# endif

# undef WHITEB
# if !defined(SIR_NO_TEXT_STYLING)
#  define WHITEB(s) COLOR(1, 97, 49, s) /**< Bold white foreground text. */
# else
#  define WHITEB(s) s
# endif

#endif /* ! _SIR_ANSI_MACROS_H_INCLUDED */
