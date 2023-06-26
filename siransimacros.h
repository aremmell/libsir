/**
 * @file siransimacros.h
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
 * @version   2.2.0
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
 *  - 38:                        Set foreground color (with pattern to follow: 256 or 24-bit RGB
 * color)
 *  - 39:                        Set default foreground color
 *  - 40–47, 100–107 ("bright"): Set background color (number = color ID)
 *  - 48:                        Set background color (with pattern to follow: 256 or 24-bit RGB
 * color)
 *  - 49:                        Set default background color
 *    === to test
 * =====================================================================================
 *  - 3:                         Emphasis (off: 23?) √
 *  - 4:                         Underline (off: 24) √
 *  - 5:                         Blinking (off: 25) √ (have to enable in settings)
 *  - 7:                         Inverse (off: 27) √
 *  - 9:                         Strikethrough (off: 29?) √
 *  - 51:                        Framed (off: 54)    x
 *  - 52:                        Encircled (off: 54) x
 *  - 53:                        Overlined (off: 55) x
 *  -  1:                        Bold (off: 21) √
 *
 * The 24-bit RGB spectrum is laid out as follows:
 *
 * 0x00-0x07:  standard colors (same as the 4-bit colors)
 * 0x08-0x0F:  high intensity colors
 * 0x10-0xE7:  6 × 6 × 6 cube (216 colors): 16 + 36 × r + 6 × g + b (0 ≤ r, g, b ≤ 5)
 * 0xE8-0xFF:  grayscale from black to white in 24 steps
 */

#define SIR_ESC "\x1b[" /**< Begins an ANSI escape sequence. */
#define SIR_ESC_M "m"   /**< Marks the end of a sequence. */

#define SIR_ESC_SEQ(codes, s) SIR_ESC codes SIR_ESC_M s
#define SIR_ESC_SEQE(codes)   SIR_ESC_SEQ(codes, "")

/** Resets all previously applied colors and effects/attributes. */
#define SIR_ESC_RST SIR_ESC_SEQE("0")

/** A few fun characters. */
#define SIR_R_ARROW "\xe2\x86\x92"
#define SIR_L_ARROW "\xe2\x86\x90"
#define SIR_BULLET  "\xe2\x80\xa2"

/**
 * Creates a sequence of colored characters 's' with foreground color 'fg',
 * background color 'bg', and attributes 'attr' (0=normal, 1=bold, 2=dim).
 * Ends by resetting to the default fg/bg color, and normal attr.
 */
#define COLOR(attr, fg, bg, s) \
    SIR_ESC_SEQ(#attr ";" #fg ";" #bg, s) SIR_ESC_SEQE("0;39;49")

/** Strike-through. */
#define STRIKE(s) SIR_ESC_SEQ("9", s) SIR_ESC_SEQE("29")

/** Inverted fg/bg. */
#define INVERT(s) SIR_ESC_SEQ("7", s) SIR_ESC_SEQE("27")

/** Underliend. */
#define ULINE(s) SIR_ESC_SEQ("4", s) SIR_ESC_SEQE("24")

/** Emphasis/italic. */
#define EMPH(s) SIR_ESC_SEQ("3", s) SIR_ESC_SEQE("23")

/** Bold. */
#define BOLD(s) SIR_ESC_SEQ("1", s) SIR_ESC_SEQE("21")

#define BLACK(s)     COLOR(0, 30, 49, s)
#define BLACKB(s)    COLOR(1, 30, 49, s)

#define RED(s)       COLOR(0, 31, 49, s)
#define REDB(s)      COLOR(1, 31, 49, s)

#define LRED(s)      COLOR(0, 91, 49, s)
#define LREDB(s)     COLOR(1, 91, 49, s)

#define GREEN(s)     COLOR(0, 32, 49, s)
#define GREENB(s)    COLOR(1, 32, 49, s)

#define LGREEN(s)    COLOR(0, 92, 49, s)
#define LGREENB(s)   COLOR(1, 92, 49, s)

#define YELLOW(s)    COLOR(0, 33, 49, s)
#define YELLOWB(s)   COLOR(1, 33, 49, s)

#define LYELLOW(s)   COLOR(0, 93, 49, s)
#define LYELLOWB(s)  COLOR(1, 93, 49, s)

#define BLUE(s)      COLOR(0, 34, 49, s)
#define BLUEB(s)     COLOR(1, 34, 49, s)

#define LBLUE(s)     COLOR(0, 94, 49, s)
#define LBLUEB(s)    COLOR(1, 94, 49, s)

#define MAGENTA(s)   COLOR(0, 35, 49, s)
#define MAGENTAB(s)  COLOR(1, 35, 49, s)

#define LMAGENTA(s)  COLOR(0, 95, 49, s)
#define LMAGENTAB(s) COLOR(1, 95, 49, s)

#define CYAN(s)      COLOR(0, 36, 49, s)
#define CYANB(s)     COLOR(1, 36, 49, s)

#define LCYAN(s)     COLOR(0, 96, 49, s)
#define LCYANB(s)    COLOR(1, 96, 49, s)

#define LGRAY(s)     COLOR(0, 37, 49, s)
#define LGRAYB(s)    COLOR(1, 37, 49, s)

#define DGRAY(s)     COLOR(0, 90, 49, s)
#define DGRAYB(s)    COLOR(1, 90, 49, s)

#define WHITE(s)     COLOR(0, 97, 49, s)
#define WHITEB(s)    COLOR(1, 97, 49, s)
