/**
 * @file sirconfig.h
 * @brief Compile-time configuration
 *
 * The definitions herein may be modified for fine-grained control over the
 * appearance and content of log messages, default values, and various
 * thresholds (e.g. file and buffer sizes).
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
#ifndef _SIR_CONFIG_H_INCLUDED
#define _SIR_CONFIG_H_INCLUDED

/**
 * @defgroup config Configuration
 *
 * Definitions affecting appearance and content of log messages, default values,
 * and various thresholds. May be modified to suit a particular use case.
 *
 * @remark All format strings containing specifiers such as `%%d` are `printf`-style.
 * The man page or documentation for `printf` can be used as a guide.
 *
 * @attention Don't make changes until you've examined how these values are
 * utilized elsewhere in the source code! You _will_ break something.
 *
 * @addtogroup config
 * @{
 */

/**
 * The time stamp format string at the start of log messages–not including
 * milliseconds (as::SIR_MSECFORMAT), which is added separately.
 *
 * @remark Only applies if ::SIRO_NOTIME is not set.
 *
 * **Example**
 *   ~~~
 *   23:30:26
 *   ~~~
 */
#define SIR_TIMEFORMAT "%H:%M:%S"


/**
 * The format for milliseconds (1000ths of a second) in time stamps.
 *
 * @remark Only applies if ::SIRO_NOTIME *or* ::SIRO_NOMSEC are not set.
 * @remark ::SIRO_NOTIME implies ::SIRO_NOMSEC.
 *
 * **Example**
 *   ~~~
 *   .034
 *   ~~~
 */
#define SIR_MSECFORMAT ".%03ld"

/**
 * The string placed directly before the human-readable logging level.
 *
 * @remark Only applies if ::SIRO_NOLEVEL is not set.
 */
#define SIR_LEVELPREFIX "["

/**
 * The string placed directly after the human-readable logging level.
 *
 * @remark Only applies if ::SIRO_NOLEVEL is not set.
 */
#define SIR_LEVELSUFFIX "]"

/**
 * The string placed directly before process and thread IDs.
 *
 * @remark Only applies if ::SIRO_NONAME is not set.
 */
#define SIR_PIDPREFIX "("

/**
 * The character placed directly after process and thread IDs.
 *
 * @remark Only applies if ::SIRO_NONAME is not set.
 */
#define SIR_PIDSUFFIX ")"

/**
 * The format for the current process/thread ID.
 *
 * @remark Only applies if ::SIRO_NOPID or ::SIRO_NOTID are not set.
 */
#define SIR_PIDFORMAT "%d"

/**
 * The string to place between process and thread IDs.
 *
 * @remark Only applies if both ::SIRO_NOPID and ::SIRO_NOTID are not set.
 *
 * **Example**
 *   ~~~
 *   3435.1189
 *   ~~~
 */
#define SIR_PIDSEPARATOR "."

/** The string passed to fopen/fopen_s for log files. */
#define SIR_FOPENMODE "a"

/**
 * The size, in bytes, at which a log file will be rolled/archived.
 * @remark Default = 5 MiB.
 */
#define SIR_FROLLSIZE (1024 * 1024 * 5)

/**
 * The time format string used in file headers (see ::SIR_FHFORMAT).
 *
 * @remark Only applies if ::SIRO_NOHDR is not set.
 *
 * **Example**
 *   ~~~
 *   15:13:41 Fri 9 Jun 23 (-0600)
 *   ~~~
 */
#define SIR_FHTIMEFORMAT "%H:%M:%S %a %d %b %y (%z)"

/**
 * The format string written to a log file when logging begins or the file
 * is rolled/archived.
 *
 * @remark Only applies if ::SIRO_NOHDR is not set.
 *
 * - The first `%%s` format specifier is the message:
 *   - ::SIR_FHBEGIN
 *   - ::SIR_FHROLLED
 *
 * - The second `%%s` is the current date/time in the format specified by
 *   ::SIR_FHTIMEFORMAT.
 */
#define SIR_FHFORMAT "\n\n----- %s %s -----\n\n"

/**
 * The string included in ::SIR_FHFORMAT when a logging session begins.
 *
 * @remark Only applies if ::SIRO_NOHDR is not set.
 */
#define SIR_FHBEGIN "session begin @"

/**
 * The string included in ::SIR_FHFORMAT when a file is rolled/archived due to
 * becoming larger than ::SIR_FROLLSIZE bytes in size.
 *
 * @remark Only applies if ::SIRO_NOHDR is not set.
 *
 * The `%%s` format specifier is the path of the archived file.
 */
#define SIR_FHROLLED "archived as %s due to size @"

/**
 * The time format string for rolled/archived log files (see ::SIR_FNAMEFORMAT).
 *
 * **Example**
 *   ~~~
 *   23-06-09-122049
 *   ~~~
 */
#define SIR_FNAMETIMEFORMAT "%y-%m-%d-%H%M%S"

/**
 * The sequence number format string for rolled/archived log files (see
 * ::SIR_FNAMEFORMAT).
 *
 * **Example**
 *   ~~~
 *   -1
 *   ~~~
 */
#define SIR_FNAMESEQFORMAT "-%hu"

/**
 * The format string for rolled/archived log file names.
 *
 * - The first %%s format specifier is the original file name, up to but not
 *   including the last full stop (.), if any exist.
 *
 * - The second %%s is the time stamp as defined by ::SIR_FNAMETIMEFORMAT.
 *
 * - The third %%s is a sequence number, which may be used in the event that
 *   a log file with the same name already exists (i.e., 2 or more files are
 *   rolled/archived within a second). Its format is defined by ::SIR_FNAMESEQFORMAT.
 *
 * - The fourth %%s is the original file name including, and beyond the last
 *   full stop, if one was found.
 *
 * **Example**
 *   ~~~
 *   `oldname.log`  ->  `oldname-23-06-09-122049-1.log`
 *   ~~~
 */
#define SIR_FNAMEFORMAT "%s-%s%s%s"

/** The human-readable form of the ::SIRL_EMERG level. */
#define SIRL_S_EMERG  "emrg"

/** The human-readable form of the ::SIRL_ALERT level. */
#define SIRL_S_ALERT  "alrt"

/** The human-readable form of the ::SIRL_CRIT level. */
#define SIRL_S_CRIT   "crit"

/** The human-readable form of the ::SIRL_ERROR level. */
#define SIRL_S_ERROR  "erro"

/** The human-readable form of the ::SIRL_WARN level. */
#define SIRL_S_WARN   "warn"

/** The human-readable form of the ::SIRL_NOTICE level. */
#define SIRL_S_NOTICE "noti"

/** The human-readable form of the ::SIRL_INFO level. */
#define SIRL_S_INFO   "info"

/** The human-readable form of the ::SIRL_DEBUG level. */
#define SIRL_S_DEBUG  "debg"

/** The maximum number of log files that may be registered at one time. */
#define SIR_MAXFILES 16

/** The size, in characters, of the buffer used to hold file header format strings. */
#define SIR_MAXFHEADER 128

/**
 * The maximum number of characters allowable in one log message. This
 * does not include accompanying formatted output (see ::SIR_MAXOUTPUT).
 */
#define SIR_MAXMESSAGE 2048

/** The size, in characters, of the buffer used to hold time format strings. */
#define SIR_MAXTIME 64

/** The size, in characters, of the buffer used to hold millisecond strings. */
#define SIR_MAXMSEC 5

/** The size, in characters, of the buffer used to hold level format strings. */
#define SIR_MAXLEVEL 7

/**
 * The size, in characters, of the buffer used to hold process/appname
 * format strings.
 */
#define SIR_MAXNAME 32

/**
 * The size, in characters, of the buffer used to hold system logger identity
 * strings.
 */
#define SIR_MAX_SYSLOG_ID 128

/**
 * The size, in characters, of the buffer used to hold system logger category
 * strings.
 */
#define SIR_MAX_SYSLOG_CAT 64

/** The size, in characters, of the buffer used to hold process/thread IDs/names. */
#define SIR_MAXPID 16

/** The maximum number of whitespace and miscellaneous characters included in output. */
#define SIR_MAXMISC 7

/**
 * The size, in characters, of the buffer used to hold a sequence of styling
 * data in 16-color mode.
 */
#define SIR_MAXSTYLE_16_COLOR 16
#define SIR_MAXSTYLE SIR_MAXSTYLE_16_COLOR

/** The maximum size, in characters, of final formatted output. */
#define SIR_MAXOUTPUT \
    (SIR_MAXMESSAGE + (SIR_MAXSTYLE * 2) + SIR_MAXTIME + SIR_MAXLEVEL + \
        SIR_MAXNAME + (SIR_MAXPID   * 2) + SIR_MAXMISC + 1)

/** The maximum size, in characters, of an error message. */
#define SIR_MAXERROR 256

/**
 * The format string for error messages returned by ::sir_geterror.
 *
 * - The first `%%s` format specifier is the function name.
 * - The second `%%s` is the file name.
 * - The `%%lu` is the line number in the file.
 * - The third `%%s` is the error message.
 *
 * **Example**
 *   ~~~
 *   Error in findneedle (haystack.c:384): 'Too much hay'
 *   ~~~
 */
#define SIR_ERRORFORMAT "Error in %s (%s:%u): '%s'"

/** The string that represents any unknown. */
#define SIR_UNKNOWN "<unknown>"

/** stderr destination string. */
#define SIR_DESTNAME_STDERR     "stderr"

/** stdout destination string. */
#define SIR_DESTNAME_STDOUT     "stdout"

/** System logger destination string. */
#define SIR_DESTNAME_SYSLOG     "syslog"

/** Fallback system logger identity. */
#define SIR_FALLBACK_SYSLOG_ID  "libsir"

/** Fallback system logger category. */
#define SIR_FALLBACK_SYSLOG_CAT "general"

/**
 * The number of actual levels; ::SIRL_NONE, ::SIRL_ALL, and ::SIRL_DEFAULT
 * are pseudo levels and end up being mapped (or not) to the others.
 */
#define SIR_NUMLEVELS 8

/**
 * The number of actual distinct options; ::SIRO_NONE, ::SIRO_ALL, ::SIRO_DEFAULT,
 * and ::SIRO_MSGONLY are pseudo options that end up being mapped (or not) to the others.
 */
#define SIR_NUMOPTIONS 8

/**
 * The number of entries in the 4-bit (16-color) map (::sir_style_16color_map)
 * 3 foreground attributes + 17 foreground colors + 17 background colors (default)
 * counts as a color.
 */
#define SIR_NUM16_COLOR_MAPPINGS 37

/** @} */

#endif /* !_SIR_CONFIG_H_INCLUDED */
