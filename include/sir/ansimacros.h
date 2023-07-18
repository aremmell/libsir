/**
 * @file ansimacros.h
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
 * @author    Ryan M. Lederman \<lederman@gmail.com\>
 * @date      2018-2023
 * @version   2.2.1
 * @copyright The MIT License (MIT)
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
#ifndef _SIR_ANSI_MACROS_H_INCLUDED
# define _SIR_ANSI_MACROS_H_INCLUDED

# define SIR_ESC "\x1b[" /**< Begins an ANSI escape sequence. */
# define SIR_ESC_M "m"   /**< Marks the end of a sequence. */

# define SIR_ESC_SEQ(codes, s) SIR_ESC codes SIR_ESC_M s
# define SIR_ESC_SEQE(codes)   SIR_ESC_SEQ(codes, "")

/** Resets all previously applied colors and effects/attributes. */
# define SIR_ESC_RST SIR_ESC_SEQE("0")

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
# define COLOR(attr, fg, bg, s) \
    SIR_ESC_SEQ(#attr ";" #fg ";" #bg, s) SIR_ESC_SEQE("0;39;49")

# define STRIKE(s) SIR_ESC_SEQ("9", s) SIR_ESC_SEQE("29") /**< Strike-through. */
# define INVERT(s) SIR_ESC_SEQ("7", s) SIR_ESC_SEQE("27") /**< Inverted fg/bg. */
# define ULINE(s)  SIR_ESC_SEQ("4", s) SIR_ESC_SEQE("24") /**< Underlined. */
# define EMPH(s)   SIR_ESC_SEQ("3", s) SIR_ESC_SEQE("23") /**< Emphasis/italic. */
# define BOLD(s)   SIR_ESC_SEQ("1", s) SIR_ESC_SEQE("22") /**< Bold. */
# define BLINK(s)  SIR_ESC_SEQ("5", s) SIR_ESC_SEQE("25") /**< Blinking text. */

# define BLACK(s)     COLOR(0, 30, 49, s) /**< Black foreground text. */
# define BLACKB(s)    COLOR(1, 30, 49, s) /**< Bold black foreground text. */

# define RED(s)       COLOR(0, 31, 49, s) /**< Red foreground text. */
# define REDB(s)      COLOR(1, 31, 49, s) /**< Bold red foreground text. */

# define BRED(s)      COLOR(0, 91, 49, s) /**< Bright red foreground text. */
# define BREDB(s)     COLOR(1, 91, 49, s) /**< Bold bright red foreground text. */

# define GREEN(s)     COLOR(0, 32, 49, s) /**< Green foreground text. */
# define GREENB(s)    COLOR(1, 32, 49, s) /**< Bold green foreground text. */

# define BGREEN(s)    COLOR(0, 92, 49, s) /**< Bright green foreground text. */
# define BGREENB(s)   COLOR(1, 92, 49, s) /**< Bold bright green foreground text. */

# define YELLOW(s)    COLOR(0, 33, 49, s) /**< Yellow foreground text. */
# define YELLOWB(s)   COLOR(1, 33, 49, s) /**< Bold yellow foreground text. */

# define BYELLOW(s)   COLOR(0, 93, 49, s) /**< Bright yellow foreground text. */
# define BYELLOWB(s)  COLOR(1, 93, 49, s) /**< Bold bright yellow foreground text. */

# define BLUE(s)      COLOR(0, 34, 49, s) /**< Blue foreground text. */
# define BLUEB(s)     COLOR(1, 34, 49, s) /**< Bold blue foreground text. */

# define BBLUE(s)     COLOR(0, 94, 49, s) /**< Bright blue foreground text. */
# define BBLUEB(s)    COLOR(1, 94, 49, s) /**< Bold bright blue foreground text. */

# define MAGENTA(s)   COLOR(0, 35, 49, s) /**< Magenta foreground text. */
# define MAGENTAB(s)  COLOR(1, 35, 49, s) /**< Bold magenta foreground text. */

# define BMAGENTA(s)  COLOR(0, 95, 49, s) /**< Bright magenta foreground text. */
# define BMAGENTAB(s) COLOR(1, 95, 49, s) /**< Bold bright magenta foreground text. */

# define CYAN(s)      COLOR(0, 36, 49, s) /**< Cyan foreground text. */
# define CYANB(s)     COLOR(1, 36, 49, s) /**< Bold cyan foreground text. */

# define BCYAN(s)     COLOR(0, 96, 49, s) /**< Bright cyan foreground text. */
# define BCYANB(s)    COLOR(1, 96, 49, s) /**< Bold bright cyan foreground text. */

# define BGRAY(s)     COLOR(0, 37, 49, s) /**< Bright gray foreground text. */
# define BGRAYB(s)    COLOR(1, 37, 49, s) /**< Bold bright gray foreground text. */

# define DGRAY(s)     COLOR(0, 90, 49, s) /**< Dark gray foreground text. */
# define DGRAYB(s)    COLOR(1, 90, 49, s) /**< Bold dark gray foreground text. */

# define WHITE(s)     COLOR(0, 97, 49, s) /**< White foreground text. */
# define WHITEB(s)    COLOR(1, 97, 49, s) /**< Bold white foreground text. */

#endif /* ! _SIR_ANSI_MACROS_H_INCLUDED */
