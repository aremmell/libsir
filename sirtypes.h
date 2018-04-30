/*!
 * \file sirtypes.h
 * \brief Definitions of types used by the SIR library.
 * \author Ryan Matthew Lederman <lederman@gmail.com>
 */
#ifndef _SIR_TYPES_H_INCLUDED
#define _SIR_TYPES_H_INCLUDED

#include "sirplatform.h"
#include "sirconfig.h"

/*! Logging levels
 *
 * Each enum value corresponds to a function that sends output
 * to all destinations for that level (::sirdebug and pals).
 *
 * Since the values are also flags (i.e., powers of two), they can be combined
 * to filter different levels of importance but not strictly linearly.
 */
typedef enum {
    SIRL_EMERG  = 0x1,  /*!< Nuclear war, Armageddon, etc. */
    SIRL_ALERT  = 0x2,  /*!< Action required ASAP. */
    SIRL_CRIT   = 0x4,  /*!< Critical errors. */
    SIRL_ERROR  = 0x8,  /*!< Errors. */
    SIRL_WARN   = 0x10, /*!< Warnings that could likely be ignored. */
    SIRL_NOTICE = 0x20, /*!< Normal but significant. */
    SIRL_INFO   = 0x40, /*!< Informational messages. */
    SIRL_DEBUG  = 0x80, /*!< Verbose debugging output. */
    SIRL_ALL    = 0xff  /*!< Includes all logging levels. */
} sir_level;

/*! One or more ::sir_level, bitwise OR'd. */
typedef uint16_t sir_levels;

/*! Output destination options
 *
 * Set any desired flags on the corresponding member in the ::sirinit structure
 * to fine-tune the output formatting for each destination.
 *
 * \attention For \a syslog (if enabled), the time stamp, name, and level are always
 * stripped from the output, since \a syslog already supplies this information. These
 * options only apply to \a stderr, \a stdout, and log files.
 */
typedef enum {
    SIRO_DEFAULT = 0,     /*!< Use the default for this type of destination. */
    SIRO_NOLEVEL = 0x100, /*!< Do not include the human-readable logging level in output. */
    SIRO_NONAME  = 0x200, /*!< Do not include the process/app name in output. */

    /*! Do not include time stamps in output. */
    SIRO_NOTIME = 0x400,

    /*! Do not include milliseconds in time stamps. If \a not set, time stamps
     * in output to this destination will be appended with the current millisecond
     * in addition to the hour, minute, and second. (see ::SIR_MSECFORMAT)
     * If ::SIRO_NOTIME is set, this has no effect.
     *
     * \attention Only available if \a SIR_MSEC_TIMER is defined, which varies by platform.
     */
    SIRO_NOMSEC = 0x800,

    /*! Don't write header messages when logging begins, or the file is rolled.
     * Only applicable to log files. Setting on any other destination
     * will have no effect. */
    SIRO_NOHDR = 0x1000,

    /*! Includes all other options; effectively disables all output formatting except
     * the original formatted message (does not include ::SIRO_NOHDR; set that flag
     * in addition to remove header messages).
     */
    SIRO_MSGONLY = 0xef00
} sir_option;

/*! One or more ::sir_option, bitwise OR'd. */
typedef uint16_t sir_options;

/*! Available styles (i.e., colors, brightness, etc.) for console output. */
typedef enum {
    SIRS_NONE = 0,
    SIRS_BOLD = 0x1,
    SIRS_BRIGHT = 0x2,
    SIRS_FG_RED = 0x4,
    SIRS_FG_GREEN = 0x8,
    SIRS_FG_BLUE = 0x10,
    SIRS_FG_YELLOW = 0x20,
    SIRS_FG_MAGENTA = 0x40,
    SIRS_FG_CYAN = 0x80,
    SIRS_FG_WHITE = 0x100,
    SIRS_FG_BLACK = 0x200,
    SIRS_BG_RED = 0x1000,
    SIRS_BG_GREEN = 0x200,
    SIRS_BG_BLUE = 0x400,
    SIRS_BG_YELLOW = 0x800,
    SIRS_BG_MAGENTA = 0x1000,
    SIRS_BG_CYAN = 0x2000,
    SIRS_BG_WHITE = 0x4000,
    SIRS_BG_BLACK = 0x8000 
} sir_textstyle;

/*! The underlying type to use for characters in output. */
typedef char sirchar_t;

/*! \struct sirinit
 *
 * \brief Initialization data for the library.
 *
 * Allocate an instance of this struct and pass it to ::sir_init
 * in order to begin using the library.
 *
 * Don't forget to call ::sir_cleanup when you're done.
 */
typedef struct {
    /*! Logging levels (::sir_level) to route to \a stdout. */
    sir_levels stdOutLevels;

    /*! Output options (::sir_option) for \a stdout. Set to ::SIRO_DEFAULT (0) to use
     * the default options for this type of destination (i.e., stdio, file, syslog).
     */
    sir_options stdOutOptions;

    /*! Logging levels (::sir_level) to route to \a stderr. */
    sir_levels stdErrLevels;

    /*! Output options (::sir_option) for \a stderr. Set to ::SIRO_DEFAULT (0) to use
     * the default options for this type of destination (i.e., stdio, file, syslog)
     */
    sir_options stdErrOptions;

    /*! Logging levels (::sir_level) to route to \a syslog.
     *
     * \attention Only available if \a SIR_NO_SYSLOG is \a not defined in the preprocessor.
     * If \a _WIN32 is defined, \a SIR_NO_SYSLOG is automatically defined.
     */
    sir_levels sysLogLevels;

    /*! Whether or not to include the current process identifier in messages sent
     * to \a syslog (default: false).
     *
     * \attention Only available if \a SIR_NO_SYSLOG is \a not defined in the preprocessor.
     *  If \a _WIN32 is defined, \a SIR_NO_SYSLOG is automatically defined.
     */
    bool sysLogIncludePID;

    /*! A custom name to include in output (default: \a none). If ::SIRO_NONAME
     * is set in the options for a destination, no name is included in output.
     * Set to \a NULL for the default.
     */
    const sirchar_t* processName;

    /*! A custom time format passed to \a strftime to use in output
     * (default: ::SIR_TIMEFORMAT). If ::SIRO_NOTIME is set in the options
     * for a destination, no time is included in output. Set to \a NULL for the default.
     */
    const sirchar_t* timeFmt;
} sirinit;

/*! \cond PRIVATE */

#define _SIR_BRIGHT_MASK 0x7
#define _SIR_MAGIC 0x60906090

#define _SIRBUF_TIME 0
#define _SIRBUF_MSEC 1
#define _SIRBUF_LEVEL 2
#define _SIRBUF_NAME 3
#define _SIRBUF_MSG 4
#define _SIRBUF_OUTPUT 5
#define _SIRBUF_MAX 5

typedef struct {
    sirchar_t*  path;
    sir_levels  levels;
    sir_options opts;
    FILE*       f;
    int         id;
} sirfile;

typedef struct {
    sirfile* files[SIR_MAXFILES];
    size_t   count;
} sirfcache;

typedef struct {
    sirchar_t* timestamp;
    sirchar_t* msec;
    sirchar_t* level;
    sirchar_t* name;
    sirchar_t* message;
    sirchar_t* output;
} siroutput;

typedef struct {
    sirchar_t timestamp[SIR_MAXTIME];
    sirchar_t msec[SIR_MAXMSEC];
    sirchar_t level[SIR_MAXLEVEL];
    sirchar_t name[SIR_MAXNAME];
    sirchar_t message[SIR_MAXMESSAGE];
    sirchar_t output[SIR_MAXOUTPUT];
} sirbuf;

typedef struct {
    sir_level level;
    uint32_t styles;
} sir_level_style_map;

#ifndef _WIN32
typedef const sirchar_t* sir_textstyle_final;

typedef enum {
    _SIRS_RESET = 0,
    _SIRS_BRIGHT = 1,
    _SIRS_FG_BLACK = 30,
    _SIRS_FG_RED = 31,
    _SIRS_FG_GREEN = 32,
    _SIRS_FG_YELLOW = 33,
    _SIRS_FG_BLUE = 34,
    _SIRS_FG_MAGENTA = 35,
    _SIRS_FG_CYAN = 36,
    _SIRS_FG_WHITE = 37,
    _SIRS_BG_BLACK = _SIRS_FG_BLACK + 10,
    _SIRS_BG_RED = _SIRS_FG_RED + 10,
    _SIRS_BG_GREEN = _SIRS_FG_GREEN + 10,
    _SIRS_BG_YELLOW = _SIRS_FG_YELLOW + 10,
    _SIRS_BG_BLUE = _SIRS_FG_BLUE + 10,
    _SIRS_BG_MAGENTA = _SIRS_FG_MAGENTA + 10,
    _SIRS_BG_CYAN = _SIRS_FG_CYAN + 10,
    _SIRS_BG_WHITE = _SIRS_FG_WHITE + 10
} sir_textstyle_priv;
#else

typedef const WORD sir_textstyle_final;

typedef enum {
    _SIRS_RESET = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
    _SIRS_BRIGHT = FOREGROUND_INTENSITY,
    _SIRS_FG_BLACK = 0,
    _SIRS_FG_RED = FOREGROUND_RED,
    _SIRS_FG_GREEN = FOREGROUND_GREEN,
    _SIRS_FG_YELLOW = FOREGROUND_RED | FOREGROUND_GREEN,
    _SIRS_FG_BLUE = FOREGROUND_BLUE,
    _SIRS_FG_MAGENTA = FOREGROUND_RED | FOREGROUND_BLUE,
    _SIRS_FG_CYAN = FOREGROUND_GREEN | FOREGROUND_BLUE,
    _SIRS_FG_WHITE = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
    _SIRS_BG_BLACK = 0,
    _SIRS_BG_RED = BACKGROUND_RED,
    _SIRS_BG_GREEN = BACKGROUND_GREEN,
    _SIRS_BG_YELLOW = BACKGROUND_RED | BACKGROUND_GREEN,
    _SIRS_BG_BLUE = BACKGROUND_BLUE,
    _SIRS_BG_MAGENTA = BACKGROUND_RED | BACKGROUND_BLUE,
    _SIRS_BG_CYAN = BACKGROUND_GREEN | BACKGROUND_BLUE,
    _SIRS_BG_WHITE = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE
} sir_textstyle_priv;
#endif

/*! \endcond */

#endif /* !_SIR_TYPES_H_INCLUDED */
