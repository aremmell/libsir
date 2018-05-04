/**
 * @file sirconfig.h
 * 
 * @brief Compile-time configuration.
 * 
 * Edit the values in this file to modify the behavior of the library.
 * 
 * @attention Don't edit this file unless you know what you're doing and you have
 * read the corresponding code that utilizes these values!
 * 
 * @defgroup custom Customization
 * 
 * Compile-time configuration that can be modified to alter the behavior
 * of the library.
 */
#ifndef _SIR_CONFIG_H_INCLUDED
#define _SIR_CONFIG_H_INCLUDED

/**
 * @addtogroup custom
 * @{
 */

/** The time stamp format string at the beginning of log messages. */
#define SIR_TIMEFORMAT "%H:%M:%S"

/** The format for the current millisecond in time stamps. */
#define SIR_MSECFORMAT ".%03ld"

/**
 * The time format string in file headers (see ::SIR_FHFORMAT).
 * @remark sample: `15:13:41 Sat 28 Apr 18 (-0600)`
 */
#define SIR_FHTIMEFORMAT "%H:%M:%S %a %d %b %y (%z)"

/**
 * The format string written to a log file when logging begins or the file
 * is rolled. The \a second %s format identifier is the current date/time
 * (see ::SIR_FHTIMEFORMAT).
 */
#define SIR_FHFORMAT "\n\n----- %s (%s) -----\n\n"

/**
 * The string included in ::SIR_FHFORMAT when a logging session begins.
 */
#define SIR_FHBEGIN "session begin"

/**
 * The string included in ::SIR_FHFORMAT when a file is rolled due to size.
 */
#define SIR_FHROLLED "file rolled due to size"

/** The string representation of the ::SIRL_EMERG level in output. */
#define SIRL_S_EMERG "EMER"

/** The string representation of the ::SIRL_ALERT level in output. */
#define SIRL_S_ALERT "ALRT"

/** The string representation of the ::SIRL_CRIT level in output. */
#define SIRL_S_CRIT "CRIT"

/** The string representation of the ::SIRL_ERROR level in output. */
#define SIRL_S_ERROR "ERR"

/** The string representation of the ::SIRL_WARN level in output. */
#define SIRL_S_WARN "WARN"

/** The string representation of the ::SIRL_NOTICE level in output. */
#define SIRL_S_NOTICE "NOTF"

/** The string representation of the ::SIRL_INFO level in output. */
#define SIRL_S_INFO "INFO"

/** The string representation of the ::SIRL_DEBUG level in output. */
#define SIRL_S_DEBUG "DBG"

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
#define SIR_MAXLEVEL 6

/** The size, in characters, of the buffer used to hold process/appname
 * format strings. */
#define SIR_MAXNAME 32

/** The maximum size, in characters, of final formatted output. */
#define SIR_MAXOUTPUT \
    SIR_MAXMESSAGE + (SIR_MAXSTYLE * 2) \
  + SIR_MAXTIME + SIR_MAXLEVEL + SIR_MAXNAME + 1

/** The string passed to fopen/fopen_s for log files. */
#define SIR_FOPENMODE "a"

/** The size, in bytes, at which a log file will be rolled. */
#define SIR_FROLLSIZE 1024L * 1024L * 5L

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
