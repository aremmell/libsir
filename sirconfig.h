 /**
  * @file sirconfig.h
  * @brief Compile-time constants.
  *
  * This file and accompanying source code originated from <https://github.com/ryanlederman/sir>.
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
#ifndef _SIR_CONFIG_H_INCLUDED
#define _SIR_CONFIG_H_INCLUDED

/**
 * @defgroup custom Customization
 *
 * Compile-time configuration that can be modified to alter the behavior
 * of libsir.
 *
 * @attention Don't make changes to anything until you've studied how
 * these values are used elsewhere in the source code.
 *
 * @addtogroup custom
 * @{
 */

/** The time stamp format string at the beginning of log messages. */
#define SIR_TIMEFORMAT "%H:%M:%S"

/** The format for the current millisecond in time stamps. */
#define SIR_MSECFORMAT ".%03ld"

/** The format for the human-readable logging level. */
#define SIR_LEVELFORMAT "[%s]"

/** The format for the current process/thread ID,
 * if no name is available/desired. */
#define SIR_PIDFORMAT "%d"

/** The character to place between process and thread IDs. */
#define SIR_PIDSEPARATOR "."

/** The string passed to fopen/fopen_s for log files. */
#define SIR_FOPENMODE "a"

/** The size, in bytes, at which a log file will be rolled/archived. */
#define SIR_FROLLSIZE (1024L * 1024L * 5L)

/**
 * The time format string in file headers (see ::SIR_FHFORMAT).
 * @remark sample: `15:13:41 Sat 28 Apr 18 (-0600)`
 */
#define SIR_FHTIMEFORMAT "%H:%M:%S %a %d %b %y (%z)"

/**
 * The format string written to a log file when logging begins or the file
 * is rolled/archived.
 *
 * - The \a first %%s format specifier is the message (e.g., ::SIR_FHBEGIN, ::SIR_FHROLLED)
 *
 * - The \a second %%s is the current date/time (see ::SIR_FHTIMEFORMAT).
 */
#define SIR_FHFORMAT "\n\n----- %s %s -----\n\n"

/**
 * The string included in ::SIR_FHFORMAT when a logging session begins.
 */
#define SIR_FHBEGIN "session begin @"

/**
 * The string included in ::SIR_FHFORMAT when a file is rolled/archived
 * due to size.
 *
 * The %%s format specifier is the path of the archived file.
 */
#define SIR_FHROLLED "archived as %s due to size @"

/**
 * The time format string for rolled/archived log files (see ::SIR_FNAMEFORMAT).
 * @remark sample: `18-05-05-122049`
 */
#define SIR_FNAMETIMEFORMAT "%y-%m-%d-%H%M%S"

/**
 * The format string for rolled/archived log file names.
 *
 * - The \a first %%s format specifier is the name part (the name up to the last '.') of the original
 * file name.
 *
 * - The \a second %%s is the time stamp as defined by SIR_ROLLTIMEFORMAT.
 *
 * - The \a third %%s is the extension part (the name after and including the last '.')
 * of the original file name if one is present.
 *
 * @remark sample: `oldname-18-05-05-122049.log`
 */
#define SIR_FNAMEFORMAT "%s-%s%s"

/** The human-readable form of the ::SIRL_EMERG level. */
#define SIRL_S_EMERG "emrg"

/** The human-readable form of the ::SIRL_ALERT level. */
#define SIRL_S_ALERT "alrt"

/** The human-readable form of the ::SIRL_CRIT level. */
#define SIRL_S_CRIT "crit"

/** The human-readable form of the ::SIRL_ERROR level. */
#define SIRL_S_ERROR "erro"

/** The human-readable form of the ::SIRL_WARN level. */
#define SIRL_S_WARN "warn"

/** The human-readable form of the ::SIRL_NOTICE level. */
#define SIRL_S_NOTICE "noti"

/** The human-readable form of the ::SIRL_INFO level. */
#define SIRL_S_INFO "info"

/** The human-readable form of the ::SIRL_DEBUG level. */
#define SIRL_S_DEBUG "debg"

/** The maximum number of log files that may be registered. */
#define SIR_MAXFILES 16

/**
 * The maximum number of characters that may be included in one message,
 * not including other parts of the output, like the timestamp and level.
 */
#define SIR_MAXMESSAGE 2048

/** The size, in characters, of the buffer used to hold time format strings. */
#define SIR_MAXTIME 64

/** The size, in characters, of the buffer used to hold millisecond strings. */
#define SIR_MAXMSEC 5

/** The size, in characters, of the buffer used to hold level format strings. */
#define SIR_MAXLEVEL 7

/** The size, in characters, of the buffer used to hold process/appname
 * format strings. */
#define SIR_MAXNAME 32

/** The size, in characters, of the buffer used to hold process/thread IDs/names. */
#define SIR_MAXPID 16

/** The maximum number of whitespace and misc. characters included in output. */
#define SIR_MAXMISC 7

/** The maximum size, in characters, of final formatted output. */
#define SIR_MAXOUTPUT \
    SIR_MAXMESSAGE + (SIR_MAXSTYLE * 2) + SIR_MAXTIME + SIR_MAXLEVEL + SIR_MAXNAME + (SIR_MAXPID * 2) + \
        SIR_MAXMISC + 1

/** The maximum size, in characters, of an error message. */
#define SIR_MAXERROR 256

/**
 * The format string for error messages returned by ::_sir_geterror.
 *
 * - The \a first %%s format specifier is the function name.
 *
 * - The \a second %%s is the file name.
 *
 * - The %%lu is the line number in the file.
 *
 * - The \a third %%s is the error message.
 *
 * @remark sample: `Error in findneedle (haystack.c:384): 'Too much hay'`
 */
#define SIR_ERRORFORMAT "Error in %s (%s:%u): '%s'"

/** The string that represents any unknown. */
#define SIR_UNKNOWN "<unknown>"

/** The value that represents an invalid file identifier. */
#define SIR_INVALID (int)-1

/** Used for level <> text style mapping. Update if adding or removing
 * levels or bad things will happen.
 */
#define SIR_NUMLEVELS 9

#ifndef _WIN32
/** The size, in characters, of the buffer used to hold text styling data. */
#define SIR_MAXSTYLE 16
/** The string used to reset any styling applied to text in stdio output. */
#define SIR_ENDSTYLE "\033[0m"
/** The default clock used to obtain the current millisecond from \a clock_gettime. */
#define SIR_MSECCLOCK CLOCK_MONOTONIC
#else
#define SIR_MAXSTYLE sizeof(uint16_t)
#undef SIR_ENDSTYLE
#undef SIR_MSECCLOCK
#endif

/** @} */

#endif /* !_SIR_CONFIG_H_INCLUDED */
