/*!
 * \file sirtypes.h
 */
#ifndef _SIR_TYPES_H_INCLUDED
#define _SIR_TYPES_H_INCLUDED

#include "sirconfig.h"
#include "sirplatform.h"

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
    SIRO_NOTIME  = 0x100, /*!< Do not include time stamps in output. */
    SIRO_NOLEVEL = 0x200, /*!< Do not include the human-readable logging level in output. */
    SIRO_NONAME  = 0x400, /*!< Do not include the process/app name in output. */

    /*! Do not include milliseconds in time stamps. If \a not set, time stamps
     * in output to this destination will be appended with the current millisecond
     * in addition to the hour, minute, and second. (see ::SIR_MSECFORMAT)
     * If ::SIRO_NOTIME is set, this has no effect.
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
    SIRS_NONE        = 0,
    SIRS_BRIGHT      = 0x1,
    SIRS_FG_BLACK    = 0x10,
    SIRS_FG_RED      = 0x20,
    SIRS_FG_GREEN    = 0x30,
    SIRS_FG_YELLOW   = 0x40,
    SIRS_FG_BLUE     = 0x50,
    SIRS_FG_MAGENTA  = 0x60,
    SIRS_FG_CYAN     = 0x70,
    SIRS_FG_WHITE    = 0x80,
    SIRS_FG_LGRAY    = 0x90,
    SIRS_FG_DGRAY    = 0xa0,
    SIRS_FG_LRED     = 0xb0,
    SIRS_FG_LGREEN   = 0xc0,
    SIRS_FG_LYELLOW  = 0xd0,
    SIRS_FG_LBLUE    = 0xe0,
    SIRS_FG_LMAGENTA = 0xf0,
    SIRS_FG_LCYAN    = 0xf10,
    SIRS_FG_DEFAULT  = 0xf20,
    SIRS_BG_BLACK    = 0x1000,
    SIRS_BG_RED      = 0x2000,
    SIRS_BG_GREEN    = 0x3000,
    SIRS_BG_YELLOW   = 0x4000,
    SIRS_BG_BLUE     = 0x5000,
    SIRS_BG_MAGENTA  = 0x6000,
    SIRS_BG_CYAN     = 0x7000,
    SIRS_BG_WHITE    = 0x8000,
    SIRS_BG_LGRAY    = 0x9000,
    SIRS_BG_DGRAY    = 0xa000,
    SIRS_BG_LRED     = 0xb000,
    SIRS_BG_LGREEN   = 0xc000,
    SIRS_BG_LYELLOW  = 0xd000,
    SIRS_BG_LBLUE    = 0xe000,
    SIRS_BG_LMAGENTA = 0xf000,
    SIRS_BG_LCYAN    = 0xf1000,
    SIRS_BG_DEFAULT  = 0xf2000,
    SIRS_INVALID     = 0xf3000
} sir_textstyle;

/*! The underlying type to use for characters in output. */
typedef char sirchar_t;

/*! \struct sir_stdio_dest
 *
 * \brief Configuration for \a stdio-type destinations (\a stdout and \a stderr).
 */
typedef struct {
    /*! Logging levels (::sir_level) that will be routed to this
     * destination. Set to ::SIRL_ALL for all levels, or zero for none.
     */
    sir_levels levels;

    /*! Output options (::sir_option). Set to ::SIRO_DEFAULT for
     * the default.
     */
    sir_options opts;

} sir_stdio_dest;

/*! \struct sir_syslog_dest
 *
 * \brief Configuration for the \a syslog destination.
 */
typedef struct {
    /*! Logging levels (::sir_level) that will be routed to this
     * destination. Set to ::SIRL_ALL for all levels, or zero for none.
     */
    sir_levels levels;

    /*! Whether or not to include the current process identifier in messages sent
     * to \a syslog (default: false).
     *
     * \attention Only available if \a SIR_NO_SYSLOG is \a not defined in the preprocessor.
     *  If \a _WIN32 is defined, \a SIR_NO_SYSLOG is automatically defined.
     */
    bool includePID;
} sir_syslog_dest;

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

    /*! \brief Configuration for stdout */
    sir_stdio_dest d_stdout;

    /*! brief Configuration for stderr */
    sir_stdio_dest d_stderr;

    /*! \brief Configuration for syslog */
    sir_syslog_dest d_syslog;

    /*! A custom name to include in output (default: \a none). If ::SIRO_NONAME
     * is set in the options for a destination, no name is included in output.
     */
    sirchar_t processName[SIR_MAXNAME];

    /*! A custom time format passed to \a strftime to use in output
     * (default: ::SIR_TIMEFORMAT). If ::SIRO_NOTIME is set in the options
     * for a destination, no time is included in output.
     */
    sirchar_t timeFmt[SIR_MAXTIME];
} sirinit;

/*! \cond PRIVATE */

#define _SIRS_ATTR_MASK 0xf
#define _SIRS_FG_MASK 0xff0
#define _SIRS_BG_MASK 0xff000

#define _SIR_MAGIC 0x60906090

#define _SIRBUF_STYLE 0
#define _SIRBUF_TIME 1
#define _SIRBUF_MSEC 2
#define _SIRBUF_LEVEL 3
#define _SIRBUF_NAME 4
#define _SIRBUF_MSG 5
#define _SIRBUF_OUTPUT 6
#define _SIRBUF_MAX 6

#define SIR_MAXERROR 256
#define SIR_UNKERROR "<unknown>"

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
    sirchar_t* style;
    sirchar_t* timestamp;
    sirchar_t* msec;
    sirchar_t* level;
    sirchar_t* name;
    sirchar_t* message;
    sirchar_t* output;
} siroutput;

typedef struct {
    sirchar_t style[SIR_MAXSTYLE];
    sirchar_t timestamp[SIR_MAXTIME];
    sirchar_t msec[SIR_MAXMSEC];
    sirchar_t level[SIR_MAXLEVEL];
    sirchar_t name[SIR_MAXNAME];
    sirchar_t message[SIR_MAXMESSAGE];
    sirchar_t output[SIR_MAXOUTPUT];
} sirbuf;

typedef struct {
    sir_level level;
    uint32_t  style;
} sir_style_map;

typedef struct {
    uint32_t from;
    uint16_t to;
} sir_style_priv_map;

typedef enum { _SIRM_INIT = 0, _SIRM_FILECACHE, _SIRM_TEXTSTYLE } sir_mutex_id;

/*! \endcond */

#endif /* !_SIR_TYPES_H_INCLUDED */
