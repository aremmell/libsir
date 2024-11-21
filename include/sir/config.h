/**
 * @file config.h
 *
 * @brief Compile-time configuration
 *
 * The definitions herein may be modified for fine-grained control over the
 * appearance and content of log messages, default values, and various
 * thresholds (e.g. file and buffer sizes).
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

#ifndef _SIR_CONFIG_H_INCLUDED
# define _SIR_CONFIG_H_INCLUDED

/**
 * The time stamp format string at the start of log messages-not including
 * milliseconds (as::SIR_MSECFORMAT), which is added separately.
 *
 * @remark Only applies if ::SIRO_NOTIME is not set.
 *
 * **Example**
 *   ~~~
 *   23:30:26
 *   ~~~
 */
# if !defined(SIR_TIMEFORMAT)
#  define SIR_TIMEFORMAT "%H:%M:%S"
# endif

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
# if !defined(SIR_MSECFORMAT)
#  define SIR_MSECFORMAT ".%03ld"
# endif

/**
 * The carriage return (CR) character to use in the end of line
 * sequence when SIR_USE_EOL_CRLF is defined.
 */
# if !defined(SIR_EOL_CR)
#  define SIR_EOL_CR "\r"
# endif

/**
 * The line feed (LF) character to use in the end of line sequence.
 */
# if !defined(SIR_EOL_LF)
#  define SIR_EOL_LF "\n"
# endif

/**
 * The end of line sequence. If SIR_USE_EOL_CRLF is defined, the
 * sequence will be SIR_EOL_CR + SIR_EOL_LF; otherwise just SIR_EOL_LF.
 */
# if !defined(SIR_USE_EOL_CRLF)
#  define SIR_EOL SIR_EOL_LF
# else
#  define SIR_EOL SIR_EOL_CR SIR_EOL_LF
# endif

/**
 * The string placed directly before the human-readable logging level.
 *
 * @remark Only applies if ::SIRO_NOLEVEL is not set.
 */
# if !defined(SIR_LEVELPREFIX)
#  define SIR_LEVELPREFIX "["
# endif

/**
 * The string placed directly after the human-readable logging level.
 *
 * @remark Only applies if ::SIRO_NOLEVEL is not set.
 */
# if !defined(SIR_LEVELSUFFIX)
#  define SIR_LEVELSUFFIX "]"
# endif

/**
 * The string placed directly before process and thread IDs.
 *
 * @remark Only applies if ::SIRO_NONAME is not set.
 */
# if !defined(SIR_PIDPREFIX)
#  define SIR_PIDPREFIX "("
# endif

/**
 * The character placed directly after process and thread IDs.
 *
 * @remark Only applies if ::SIRO_NONAME is not set.
 */
# if !defined(SIR_PIDSUFFIX)
#  define SIR_PIDSUFFIX ")"
# endif

/**
 * The format for the current process ID.
 *
 * @remark Only applies if ::SIRO_NOPID is not set.
 */
# if !defined(SIR_PIDFORMAT)
#  define SIR_PIDFORMAT "%d"
# endif

/**
 * The format for the current thread ID.
 *
 * @remark Only applies if ::SIRO_NOTID is not set.
 */
# if !defined(SIR_TIDFORMAT)
#  if defined(__USE_HEX_TIDS__)
#   define SIR_TIDFORMAT "%x"
#  else
#   define SIR_TIDFORMAT "%d"
#  endif
# endif

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
# if !defined(SIR_PIDSEPARATOR)
#  define SIR_PIDSEPARATOR "."
# endif

/** The string passed to fopen/fopen_s for log files. */
# if !defined(SIR_FOPENMODE)
#  define SIR_FOPENMODE "a"
# endif

/**
 * The size, in bytes, at which a log file will be rolled/archived.
 * @remark Default = 5 MiB.
 */
# if !defined(SIR_FROLLSIZE)
#  define SIR_FROLLSIZE (1024 * 1024 * 5)
# endif

/**
 * The time format string used in file headers (see ::SIR_FHFORMAT).
 *
 * @remark Only applies if ::SIRO_NOHDR is not set.
 *
 * **Example**
 *   ~~~
 *   15:13:41 Fri 9 Jun 2023 (-0600)
 *   ~~~
 */
# if !defined(SIR_FHTIMEFORMAT)
#  define SIR_FHTIMEFORMAT "%H:%M:%S %a %d %b %Y (%z)"
# endif

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
# if !defined(SIR_FHFORMAT)
#  define SIR_FHFORMAT SIR_EOL SIR_EOL "----- %s %s -----" SIR_EOL SIR_EOL
# endif

/**
 * The string included in ::SIR_FHFORMAT when a logging session begins.
 *
 * @remark Only applies if ::SIRO_NOHDR is not set.
 */
# if !defined(SIR_FHBEGIN)
#  define SIR_FHBEGIN "session begin @"
# endif

/**
 * The string included in ::SIR_FHFORMAT when a file is rolled/archived due to
 * becoming larger than ::SIR_FROLLSIZE bytes in size.
 *
 * @remark Only applies if ::SIRO_NOHDR is not set.
 *
 * The `%%s` format specifier is the path of the archived file.
 */
# if !defined(SIR_FHROLLED)
#  define SIR_FHROLLED "archived as %s due to size @"
# endif

/**
 * The time format string for rolled/archived log files (see ::SIR_FNAMEFORMAT).
 *
 * **Example**
 *   ~~~
 *   2023-06-09-122049
 *   ~~~
 */
# if !defined(SIR_FNAMETIMEFORMAT)
#  define SIR_FNAMETIMEFORMAT "%Y-%m-%d-%H%M%S"
# endif

/**
 * The sequence number format string for rolled/archived log files (see
 * ::SIR_FNAMEFORMAT).
 *
 * **Example**
 *   ~~~
 *   -1
 *   ~~~
 */
# if !defined(SIR_FNAMESEQFORMAT)
#  define SIR_FNAMESEQFORMAT "-%hu"
# endif

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
# if !defined(SIR_FNAMEFORMAT)
#  define SIR_FNAMEFORMAT "%s-%s%s%s"
# endif

/** The human-readable form of the ::SIRL_EMERG level. */
# if !defined(SIRL_S_EMERG)
#  define SIRL_S_EMERG  "emrg"
# endif

/** The human-readable form of the ::SIRL_ALERT level. */
# if !defined(SIRL_S_ALERT)
#  define SIRL_S_ALERT  "alrt"
# endif

/** The human-readable form of the ::SIRL_CRIT level. */
# if !defined(SIRL_S_CRIT)
#  define SIRL_S_CRIT   "crit"
# endif

/** The human-readable form of the ::SIRL_ERROR level. */
# if !defined(SIRL_S_ERROR)
#  define SIRL_S_ERROR  "erro"
# endif

/** The human-readable form of the ::SIRL_WARN level. */
# if !defined(SIRL_S_WARN)
#  define SIRL_S_WARN   "warn"
# endif

/** The human-readable form of the ::SIRL_NOTICE level. */
# if !defined(SIRL_S_NOTICE)
#  define SIRL_S_NOTICE "noti"
# endif

/** The human-readable form of the ::SIRL_INFO level. */
# if !defined(SIRL_S_INFO)
#  define SIRL_S_INFO   "info"
# endif

/** The human-readable form of the ::SIRL_DEBUG level. */
# if !defined(SIRL_S_DEBUG)
#  define SIRL_S_DEBUG  "debg"
# endif

/** The maximum number of log files that may be registered at one time. */
# if !defined(SIR_MAXFILES)
#  define SIR_MAXFILES 16
# endif

/** The maximum number of plugin modules that may be loaded at one time. */
# if !defined(SIR_MAXPLUGINS)
#  define SIR_MAXPLUGINS 16
# endif

/** The size, in characters, of the buffer used to hold file header format strings. */
# if !defined(SIR_MAXFHEADER)
#  define SIR_MAXFHEADER 128
# endif

/**
 * The maximum number of characters allowable in one log message. This
 * does not include accompanying formatted output (see ::SIR_MAXOUTPUT).
 */
# if !defined(SIR_MAXMESSAGE)
#  define SIR_MAXMESSAGE 4096
# endif

/** The size, in characters, of the buffer used to hold time format strings. */
# if !defined(SIR_MAXTIME)
#  define SIR_MAXTIME 64
# endif

/** The size, in characters, of the buffer used to hold millisecond strings. */
# if !defined(SIR_MAXMSEC)
#  define SIR_MAXMSEC 5
# endif

/** The size, in characters, of the buffer used to hold level format strings. */
# if !defined(SIR_MAXLEVEL)
#  define SIR_MAXLEVEL 7
# endif

/**
 * The size, in characters, of the buffer used to hold process/appname
 * format strings.
 */
# if !defined(SIR_MAXNAME)
#  define SIR_MAXNAME 32
# endif

/**
 * The size, in characters, of the buffer used to hold system logger identity
 * strings.
 */
# if !defined(SIR_MAX_SYSLOG_ID)
#  define SIR_MAX_SYSLOG_ID 128
# endif

/**
 * The size, in characters, of the buffer used to hold system logger category
 * strings.
 */
# if !defined(SIR_MAX_SYSLOG_CAT)
#  define SIR_MAX_SYSLOG_CAT 64
# endif

/** The maximum number of whitespace and miscellaneous characters included in output. */
# if !defined(SIR_MAXMISC)
#  define SIR_MAXMISC 7
# endif

/**
 * The size, in characters, of the buffer used to hold a sequence of styling
 * data in any color mode (the largest possible sequence, which is:
 * `\x1b[a;fb;m;rrr;ggg;bbb;fb;m;rrr;ggg;bbbm`) plus a null terminator.
 */
# if !defined(SIR_NO_TEXT_STYLING)
#  if !defined(SIR_MAXSTYLE)
#   if !defined(SIR_USE_EOL_CRLF)
#    define SIR_MAXSTYLE 43
#   else
#    define SIR_MAXSTYLE 44
#   endif
#  endif
# else
#  if !defined(SIR_MAXSTYLE)
#   if !defined(SIR_USE_EOL_CRLF)
#    define SIR_MAXSTYLE 1
#   else
#    define SIR_MAXSTYLE 2
#   endif
#  endif
# endif

/** The maximum size, in characters, of final formatted output. */
# define SIR_MAXOUTPUT \
    (SIR_MAXMESSAGE + (SIR_MAXSTYLE * 2) + SIR_MAXTIME + SIR_MAXLEVEL + \
        SIR_MAXNAME + (SIR_MAXPID   * 2) + SIR_MAXMISC + 2 + 1)

/** The maximum size, in characters, of an error message. */
# if !defined(SIR_MAXERROR)
#  define SIR_MAXERROR 256
# endif

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
# define SIR_ERRORFORMAT "Error in %s (%s:%u): '%s'"

/** The string that represents any unknown. */
# if !defined(SIR_UNKNOWN)
#  define SIR_UNKNOWN "<unknown>"
# endif

/** stderr destination string. */
# if !defined(SIR_DESTNAME_STDERR)
#  define SIR_DESTNAME_STDERR     "stderr"
# endif

/** stdout destination string. */
# if !defined(SIR_DESTNAME_STDOUT)
#  define SIR_DESTNAME_STDOUT     "stdout"
# endif

/** System logger destination string. */
# if !defined(SIR_DESTNAME_SYSLOG)
#  define SIR_DESTNAME_SYSLOG     "syslog"
# endif

/** Fallback system logger identity. */
# if !defined(SIR_FALLBACK_SYSLOG_ID)
#  define SIR_FALLBACK_SYSLOG_ID  "libsir"
# endif

/** Fallback system logger category. */
# if !defined(SIR_FALLBACK_SYSLOG_CAT)
#  define SIR_FALLBACK_SYSLOG_CAT "general"
# endif

/**
 * The number of actual levels; ::SIRL_NONE, ::SIRL_ALL, and ::SIRL_DEFAULT
 * are pseudo levels and end up being mapped (or not) to the others.
 */
# define SIR_NUMLEVELS 8

/**
 * The number of actual options; ::SIRO_ALL, ::SIRO_DEFAULT, and ::SIRO_MSGONLY
 * are pseudo options that end up being mapped (or not) to the others.
 */
# define SIR_NUMOPTIONS 8

/**
 * The number of seconds to let elapse before checking if the hostname needs
 * refreshing. The default is an eager 1 minute. Better safe than wrong?
 */
# if !defined(SIR_HNAME_CHK_INTERVAL)
#  define SIR_HNAME_CHK_INTERVAL 60
# endif

/**
 * The number of milliseconds to let elapse before re-formatting the current
 * thread identifier and/or name.
 */
# if !defined(SIR_THRD_CHK_INTERVAL)
#  define SIR_THRD_CHK_INTERVAL 333.0
# endif

/**
 * The number of writes to a file to let occur before checking its current size to
 * determine if it needs to be rolled.
 */
# if !defined(SIR_FILE_CHK_SIZE_WRITES)
#  define SIR_FILE_CHK_SIZE_WRITES 10
# endif

# if defined(SIR_OS_LOG_ENABLED)
/**
 * The special format specifier to send to os_log. By default, the log will only
 * show "<private>" in place of the original message. By using "%{public}s", the
 * message contents will be visible in the log.
 */
#  if !defined(SIR_OS_LOG_FORMAT)
#   define SIR_OS_LOG_FORMAT "%{public}s"
#  endif
# endif

/**
 * The number of consecutive duplicate messages that will cause libsir to
 * squelch further identical messages, and instead log the message
 * ::SIR_SQUELCH_MSG_FORMAT.
 */
# if !defined(SIR_SQUELCH_THRESHOLD)
#  define SIR_SQUELCH_THRESHOLD 5
# endif

/**
 * If duplicate messages continue to be logged after the threshold is met, the
 * threshold will be multiplied by this number, resulting in longer intervals
 * between ::SIR_SQUELCH_MSG_FORMAT messages.
 */
# if !defined(SIR_SQUELCH_BACKOFF_FACTOR)
#  define SIR_SQUELCH_BACKOFF_FACTOR 2
# endif

/**
 * The message to be logged when ::SIR_SQUELCH_THRESHOLD (or a multiple thereof)
 * consecutive duplicate messages are logged.
 */
# if !defined(SIR_SQUELCH_MSG_FORMAT)
#  define SIR_SQUELCH_MSG_FORMAT "previous message repeated %zu times"
# endif

#endif /* !_SIR_CONFIG_H_INCLUDED */
