/**
 * @file sirtypes.h
 * @brief Library types.
 */
#ifndef _SIR_TYPES_H_INCLUDED
#define _SIR_TYPES_H_INCLUDED

#include "sirplatform.h"
#include "sirconfig.h"

/**
 * @addtogroup public
 * @{
 */

/** The error code type. */
typedef uint32_t sirerrcode_t;

/** The value used to represent an invalid file identifier. */
static const int SIR_INVALID = -1;

/** Defines the available levels \a (severity/priority) of logging output. */
typedef enum {
    SIRL_EMERG   = 0x1,  /**< Nuclear war, Armageddon, etc. */
    SIRL_ALERT   = 0x2,  /**< Action required ASAP. */
    SIRL_CRIT    = 0x4,  /**< Critical errors. */
    SIRL_ERROR   = 0x8,  /**< Errors. */
    SIRL_WARN    = 0x10, /**< Warnings that could likely be ignored. */
    SIRL_NOTICE  = 0x20, /**< Normal but significant. */
    SIRL_INFO    = 0x40, /**< Informational messages. */
    SIRL_DEBUG   = 0x80, /**< Debugging/diagnostic output. */
    SIRL_ALL     = 0xff, /**< Includes all logging levels. */
    SIRL_DEFAULT = 0x100 /**< Use the default levels for this type of destination. */
} sir_level;

/**
 * Used to differentiate between a single ::sir_level and one or more
 * bitwise OR'd together.
 */
typedef uint16_t sir_levels;

/** Formatting options for a destination. */
typedef enum {

    /** Don't include time stamps in output. */
    SIRO_NOTIME = 0x200,

    /** Don't include the human-readable logging level in output. */
    SIRO_NOLEVEL = 0x400,

    /** Don't include the process/app name in output. */
    SIRO_NONAME = 0x800,

    /**
     * Don't include milliseconds in time stamps. If \a not set, time stamps
     * in output to this destination will be appended with the current millisecond
     * in addition to the hour, minute, and second. If ::SIRO_NOTIME is set, this has no effect.
     */
    SIRO_NOMSEC = 0x1000,

    /** Don't include the process ID in output. */
    SIRO_NOPID = 0x2000,

    /**
     * Don't include the thread ID in output. If ::SIRO_NOPID is set,
     * this is ignored, and no thread ID is ever included.
     */
    SIRO_NOTID = 0x4000,

    /**
     * Don't write header messages when logging begins, or the file is rolled.
     * Only applicable to log files.
     */
    SIRO_NOHDR = 0x10000,

    /**
     * Includes all other options; effectively disables all output formatting except
     * the original formatted message (does not include ::SIRO_NOHDR; set that flag
     * in addition to remove header messages).
     */
    SIRO_MSGONLY = 0xeff00,

    /** Use the default for this type of destination. See
     * sirdefaults.h for specifics.
     */
    SIRO_DEFAULT = 0x100000,    
} sir_option;

/**
 * Used to differentiate between a single ::sir_option and one or more
 * bitwise OR'd together.
 */
typedef uint32_t sir_options;

/** Available styles \a (colors, brightness) for console output. */
typedef enum {
    SIRS_NONE        = 0,       /**< Used internally; has no effect. */
    SIRS_BRIGHT      = 0x1,     /**< If set, the foreground color is 'intensified'. */
    SIRS_DIM         = 0x2,     /**< If set, the foreground color is 'dimmed'. */
    SIRS_FG_BLACK    = 0x10,    /**< Black foreground. */
    SIRS_FG_RED      = 0x20,    /**< Red foreground. */
    SIRS_FG_GREEN    = 0x30,    /**< Green foreground. */
    SIRS_FG_YELLOW   = 0x40,    /**< Yellow foreground. */
    SIRS_FG_BLUE     = 0x50,    /**< Blue foreground. */
    SIRS_FG_MAGENTA  = 0x60,    /**< Magenta foreground. */
    SIRS_FG_CYAN     = 0x70,    /**< Cyan foreground. */
    SIRS_FG_WHITE    = 0x80,    /**< White foreground. */
    SIRS_FG_LGRAY    = 0x90,    /**< Light gray foreground. */
    SIRS_FG_DGRAY    = 0xa0,    /**< Dark gray foreground. */
    SIRS_FG_LRED     = 0xb0,    /**< Light red foreground. */
    SIRS_FG_LGREEN   = 0xc0,    /**< Light green foreground. */
    SIRS_FG_LYELLOW  = 0xd0,    /**< Light yellow foreground. */
    SIRS_FG_LBLUE    = 0xe0,    /**< Light blue foreground. */
    SIRS_FG_LMAGENTA = 0xf0,    /**< Light magenta foreground. */
    SIRS_FG_LCYAN    = 0xf10,   /**< Light cyan foreground. */
    SIRS_FG_DEFAULT  = 0xf20,   /**< Use the default foreground color. */
    SIRS_BG_BLACK    = 0x1000,  /**< Black background. */
    SIRS_BG_RED      = 0x2000,  /**< Red background. */
    SIRS_BG_GREEN    = 0x3000,  /**< Green background. */
    SIRS_BG_YELLOW   = 0x4000,  /**< Yellow background. */
    SIRS_BG_BLUE     = 0x5000,  /**< Blue background. */
    SIRS_BG_MAGENTA  = 0x6000,  /**< Magenta background. */
    SIRS_BG_CYAN     = 0x7000,  /**< Cyan background. */
    SIRS_BG_WHITE    = 0x8000,  /**< White background. */
    SIRS_BG_LGRAY    = 0x9000,  /**< Light gray background. */
    SIRS_BG_DGRAY    = 0xa000,  /**< Dark gray background. */
    SIRS_BG_LRED     = 0xb000,  /**< Light red background. */
    SIRS_BG_LGREEN   = 0xc000,  /**< Light green background. */
    SIRS_BG_LYELLOW  = 0xd000,  /**< Light yellow background. */
    SIRS_BG_LBLUE    = 0xe000,  /**< Light blue background. */
    SIRS_BG_LMAGENTA = 0xf000,  /**< Light magenta background. */
    SIRS_BG_LCYAN    = 0xf1000, /**< Light cyan background. */
    SIRS_BG_DEFAULT  = 0xf2000, /**< Use the default background color. */
    SIRS_INVALID     = 0xf3000  /**< Represents the invalid text style. */
} sir_textstyle;

/** The underlying type used for characters in output. */
typedef char sirchar_t;

/**
 * @struct sir_stdio_dest
 * @brief Configuration for \a stdio destinations \a (stdout and stderr).
 */
typedef struct {
    sir_levels  levels;
    sir_options opts;
} sir_stdio_dest;

/**
 * @struct sir_syslog_dest
 * @brief Configuration for the \a syslog destination.
 */
typedef struct {
    sir_levels levels;
    bool       includePID;
} sir_syslog_dest;

/**
 * @struct sirinit
 *
 * @brief Initialization data for the library.
 *
 * Allocate an instance of this struct and pass it to ::sir_init
 * in order to begin using the library.
 */
typedef struct {
    sir_stdio_dest d_stdout; /**< \a stdout configuration. */
    sir_stdio_dest d_stderr; /**< \a stderr configuration. */

#ifndef SIR_NO_SYSLOG
    sir_syslog_dest d_syslog; /**< \a syslog configuration. */
#endif

    /**
     * If set, defines the name that will appear in formatted output.
     * Set ::SIRO_NONAME for a destination to supppress it.
     */
    sirchar_t processName[SIR_MAXNAME];
} sirinit;

/** Library error type. */
typedef struct {
    sirerrcode_t code;
    const sirchar_t * const message;
} sirerror;

/** @} */

/**
 * @addtogroup intern
 * @{
 */

/** Text style attribute mask. */
#define _SIRS_ATTR_MASK 0xf

/** Text style foreground color mask. */
#define _SIRS_FG_MASK 0xff0

/** Text style background color mask. */
#define _SIRS_BG_MASK 0xff000

/** Magic number used to determine if the library has been initialized. */
#define _SIR_MAGIC 0x60906090

/** The maximum size of an error message generated
 * by the OS. */
#define SIR_MAXERROR 256

/** The error message used when an error is not resolved. */
#define SIR_UNKERROR "<unknown>"

/** Log file data. */
typedef struct {
    sirchar_t*  path;
    sir_levels  levels;
    sir_options opts;
    FILE*       f;
    int         id;
} sirfile;

/** Log file cache. */
typedef struct {
    sirfile* files[SIR_MAXFILES];
    size_t   count;
} sirfcache;

/** Formatted output sent to destinations. */
typedef struct {
    sirchar_t* style;
    sirchar_t* timestamp;
    sirchar_t* msec;
    sirchar_t* level;
    sirchar_t* name;
    sirchar_t* pid;
    sirchar_t* tid;
    sirchar_t* message;
    sirchar_t* output;
} siroutput;

/** Indexes into ::sirbuf buffers. */
typedef enum {
    _SIRBUF_STYLE = 0,
    _SIRBUF_TIME,
    _SIRBUF_MSEC,
    _SIRBUF_LEVEL,
    _SIRBUF_NAME,
    _SIRBUF_PID,
    _SIRBUF_TID,
    _SIRBUF_MSG,
    _SIRBUF_OUTPUT,
    _SIRBUF_MAX
} sirbuf_idx;

/** Buffers for output formatting. */
typedef struct {
    sirchar_t style[SIR_MAXSTYLE];
    sirchar_t timestamp[SIR_MAXTIME];
    sirchar_t msec[SIR_MAXMSEC];
    sirchar_t level[SIR_MAXLEVEL];
    sirchar_t name[SIR_MAXNAME];
    sirchar_t pid[SIR_MAXPID];
    sirchar_t tid[SIR_MAXPID];
    sirchar_t message[SIR_MAXMESSAGE];
    sirchar_t output[SIR_MAXOUTPUT];
} sirbuf;

/** ::sir_level <> ::sir_textstyle mapping. */
typedef struct {
    sir_level level;
    uint32_t  style;
} sir_style_map;

/** Public (::sir_textstyle) <> platform text style mapping. */
typedef struct {
    uint32_t from; /**< The public text style flag(s). */
    uint16_t to;   /**< The internal value. */
} sir_style_priv_map;

/** Mutex <> protected section mapping. */
typedef enum {
    _SIRM_INIT = 0,  /**< The ::sirinit section. */
    _SIRM_FILECACHE, /**< The ::sirfcache section. */
    _SIRM_TEXTSTYLE, /**< The ::sir_style_map section. */
} sir_mutex_id;

/** @} */

#endif /* !_SIR_TYPES_H_INCLUDED */
