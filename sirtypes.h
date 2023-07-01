/*
 * sirtypes.h
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2023
 * Version:   2.2.0
 * License:   The MIT License (MIT)
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
#ifndef _SIR_TYPES_H_INCLUDED
#define _SIR_TYPES_H_INCLUDED

#include "sirplatform.h"
#include "sirconfig.h"
#include "siransimacros.h"

/**
 * @addtogroup public
 * @{
 *
 * @defgroup publictypes Types
 * @{
 */

/** Log file identifier type. */
typedef const int* sirfileid;

/** Defines the available levels (severity/priority) of logging output. */
typedef enum {
    SIRL_NONE    = 0x0000, /**< No output. */
    SIRL_EMERG   = 0x0001, /**< Nuclear war, Armageddon, etc. */
    SIRL_ALERT   = 0x0002, /**< Action required ASAP. */
    SIRL_CRIT    = 0x0004, /**< Critical errors. */
    SIRL_ERROR   = 0x0008, /**< Errors. */
    SIRL_WARN    = 0x0010, /**< Warnings that could likely be ignored. */
    SIRL_NOTICE  = 0x0020, /**< Normal but significant. */
    SIRL_INFO    = 0x0040, /**< Informational messages. */
    SIRL_DEBUG   = 0x0080, /**< Debugging/diagnostic output. */
    SIRL_ALL     = 0x00ff, /**< Include all logging levels. */
    SIRL_DEFAULT = 0x0100  /**< Default levels for this type of destination. */
} sir_level;

/** ::sir_level bitmask type. */
typedef uint16_t sir_levels;

/** Formatting options for a destination. */
typedef enum {
    SIRO_ALL     = 0x00000000, /**< Include all formatting and functionality. */
    SIRO_NOTIME  = 0x00000100, /**< Exclude time stamps (implies ::SIRO_NOMSEC). */
    SIRO_NOMSEC  = 0x00000200, /**< Exclude millisecond-resolution in time stamps. */
    SIRO_NOHOST  = 0x00000400, /**< Exclude local hostname. */
    SIRO_NOLEVEL = 0x00000800, /**< Exclude human-readable logging level. */
    SIRO_NONAME  = 0x00001000, /**< Exclude process/app name. */
    SIRO_NOPID   = 0x00002000, /**< Exclude process ID. */
    SIRO_NOTID   = 0x00004000, /**< Exclude thread ID/name. */
    SIRO_NOHDR   = 0x00010000, /**< Don't write header messages to log files. */
    SIRO_MSGONLY = 0x00007f00, /**< Sets all other options except ::SIRO_NOHDR. */
    SIRO_DEFAULT = 0x00100000  /**< Default options for this type of destination. */
} sir_option;

/** ::sir_option bitmask type. */
typedef uint32_t sir_options;

/** Styles for 16-color stdio output. */
typedef enum {
    SIRS_NONE        = 0x00000000, /**< Used internally; has no effect. */
    SIRS_BRIGHT      = 0x00000000, /**< If set, the foreground color is 'intensified'. */
    SIRS_DIM         = 0x00000000, /**< If set, the foreground color is 'dimmed'. */
    ////////////////// 0x0000fff0
    SIRS_FG_BLACK    = 0x000089a0, /**< Black foreground. */
    SIRS_FG_RED      = 0x000059b0, /**< Red foreground. */
    SIRS_FG_GREEN    = 0x00006670, /**< Green foreground. */
    SIRS_FG_YELLOW   = 0x00001310, /**< Yellow foreground. */
    SIRS_FG_BLUE     = 0x00004970, /**< Blue foreground. */
    SIRS_FG_MAGENTA  = 0x00003430, /**< Magenta foreground. */
    SIRS_FG_CYAN     = 0x00006090, /**< Cyan foreground. */
    SIRS_FG_LGRAY    = 0x00006f00, /**< Light gray foreground. */
    SIRS_FG_DEFAULT  = 0x00007210, /**< Use the default foreground color. */
    SIRS_FG_DGRAY    = 0x00007880, /**< Dark gray foreground. */
    SIRS_FG_LRED     = 0x00008220, /**< Light red foreground. */
    SIRS_FG_LGREEN   = 0x00009300, /**< Light green foreground. */
    SIRS_FG_LYELLOW  = 0x00003160, /**< Light yellow foreground. */
    SIRS_FG_LBLUE    = 0x00007100, /**< Light blue foreground. */
    SIRS_FG_LMAGENTA = 0x00005220, /**< Light magenta foreground. */
    SIRS_FG_LCYAN    = 0x00005540, /**< Light cyan foreground. */
    SIRS_FG_WHITE    = 0x00001480, /**< White foreground. */
    ////////////////// 0x0fff0000
    SIRS_BG_BLACK    = 0x089a0000, /**< Black background. */
    SIRS_BG_RED      = 0x059b0000, /**< Red background. */
    SIRS_BG_GREEN    = 0x06670000, /**< Green background. */
    SIRS_BG_YELLOW   = 0x01310000, /**< Yellow background. */
    SIRS_BG_BLUE     = 0x04970000, /**< Blue background. */
    SIRS_BG_MAGENTA  = 0x03430000, /**< Magenta background. */
    SIRS_BG_CYAN     = 0x06090000, /**< Cyan background. */
    SIRS_BG_LGRAY    = 0x06f00000, /**< Light gray background. */
    SIRS_BG_DEFAULT  = 0x07210000, /**< Use the default background color. */
    SIRS_BG_DGRAY    = 0x07880000, /**< Dark gray background. */
    SIRS_BG_LRED     = 0x08220000, /**< Light red background. */
    SIRS_BG_LGREEN   = 0x09300000, /**< Light green background. */
    SIRS_BG_LYELLOW  = 0x03160000, /**< Light yellow background. */
    SIRS_BG_LBLUE    = 0x07100000, /**< Light blue background. */
    SIRS_BG_LMAGENTA = 0x05220000, /**< Light magenta background. */
    SIRS_BG_LCYAN    = 0x05540000, /**< Light cyan background. */
    SIRS_BG_WHITE    = 0x01480000, /**< White background. */
    SIRS_INVALID     = 0x0000ffff  /**< Represents the invalid text style. */
} sir_textstyle;

/**
 * @struct sir_stdio_dest
 * @brief Configuration for stdio destinations (stdout and stderr).
 *
 * @see ::sir_level
 * @see ::sir_option
 * @see ::sir_syslog_dest
 */
typedef struct {
    /** ::sir_level bitmask defining output levels to register for. */
    sir_levels levels;

    /** ::sir_option bitmask defining the formatting of output. */
    sir_options opts;
} sir_stdio_dest;

/**
 * @struct sir_syslog_dest
 * @brief Configuration for the system logger destination.
 *
 * @see ::sir_level
 * @see ::sir_option
 * @see ::sir_stdio_dest
 */
typedef struct {
    /** ::sir_level bitmask defining levels to register for. */
    sir_levels levels;

    /**
     * ::sir_option bitmask defining the formatting of output.
     *
     * @remark Unlike the stdio and log file destinations, not all options are
     * supported. This is due to the fact that system logging facilities typically
     * already include the information represented by ::sir_option on their own.
     *
     * Furthermore, the supported options vary based on the system logging
     * factility in use.
     *
     * @note If your system supports syslog, and libsir is compiled with the intent
     * to use it (::SIR_SYSLOG_ENABLED is defined), then at least ::SIRO_NOPID is
     * supported.
     */
    sir_options opts;

    /** Reserved for internal use; do not modify. */
    struct {
        /** State bitmask. */
        uint32_t mask;

        /** System logger handle/identifier. */
        void* logger;
    } _state;

    /**
     * The identity string to pass to the system logger.
     *
     * If not set, and the name in the ::sirinit struct
     * is set, that will be used instead.
     *
     * Failing that, an attempt will be made to use the file name
     * of the calling process. If that is unsuccessful as well,
     * the string ::SIR_FALLBACK_SYSLOG_ID will be used.
     *
     * @note Can be modified at runtime by calling ::sir_syslogid.
     */
    char identity[SIR_MAX_SYSLOG_ID];

    /**
     * The category string to pass to the system logger.
     *
     * Some system loggers (e.g. `os_log` on macOS) require a
     * category string to group and filter log messages.
     *
     * If not set, the string ::SIR_FALLBACK_SYSLOG_CAT will be used instead.
     *
     * @note Can be modified at runtime by calling ::sir_syslogcat.
     */
    char category[SIR_MAX_SYSLOG_CAT];
} sir_syslog_dest;

/**
 * @struct sirinit
 * @brief libsir initialization and configuration data.
 *
 * @note Pass a pointer to an instance of this structure to ::sir_init
 * to begin using libsir.
 *
 * @see ::sir_stdio_dest
 * @see ::sir_syslog_dest
 */
typedef struct {
    /** stdout configuration. */
    sir_stdio_dest d_stdout;

    /** stderr configuration. */
    sir_stdio_dest d_stderr;

    /** System logger configuration. */
    sir_syslog_dest d_syslog;

    /**
     * If set, defines the name that will appear in messages sent to stdio and
     * log file destinations.
     *
     * Set ::SIRO_NONAME in a destination's options bitmask to exclude it from
     * log messages.
     */
    char name[SIR_MAXNAME];
} sirinit;

/**
 * @}
 * @}
 * */

/**
 * @internal
 * @{
 */

/** Text style attribute mask. */
#define _SIRS_ATTR_MASK 0x0000000f

/** Text style foreground color mask. */
#define _SIRS_FG_MASK 0x0000fff0

/** Text style background color mask. */
#define _SIRS_BG_MASK 0x0fff0000

/** True if foreground and background colors are the same. */
#define _SIRS_SAME_COLOR(fg, bg) ((((bg) >> 12) & _SIRS_FG_MASK) == (fg))

/** Magic number used to determine if libsir has been initialized. */
#define _SIR_MAGIC 0x60906090

#if defined(__WIN__)
typedef void (*invalparamfn)(const wchar_t*, const wchar_t*, const wchar_t*,
    unsigned int, uintptr_t);
#endif

/** Internally-used global config container. */
typedef struct {
    sirinit si;
    struct {
        char hostname[SIR_MAXHOST];
        pid_t pid;
    } state;
} sirconfig;

/** Internally-used error type. */
typedef struct {
    uint32_t code;
    const char* const message;
} sirerror;

/** Log file data. */
typedef struct {
    char* path;
    sir_levels levels;
    sir_options opts;
    FILE* f;
    int id;
} sirfile;

/** Log file cache. */
typedef struct {
    sirfile* files[SIR_MAXFILES];
    size_t count;
} sirfcache;

/** Formatted output container. */
typedef struct {
    char style[SIR_MAXSTYLE];
    char timestamp[SIR_MAXTIME];
    char msec[SIR_MAXMSEC];
    const char* hostname;
    const char* level;
    const char* name;
    char pid[SIR_MAXPID];
    char tid[SIR_MAXPID];
    char message[SIR_MAXMESSAGE];
    char output[SIR_MAXOUTPUT];
    size_t output_len;
} sirbuf;

/** ::sir_level <-> ::sir_textstyle mapping. */
typedef struct {
    const sir_level level;  /**< The level for which the style applies. */
    sir_textstyle style;    /**< The ::sir_textstyle representation. */
    char str[SIR_MAXSTYLE]; /**< The formatted string representation. */
} sir_level_style_tuple;

/** ::sir_level <-> human-readable string form. */
typedef struct {
    const sir_level level; /**< The level for which the string applies. */
    const char* fmt;       /**< The formatted string representation. */
} sir_level_str_pair;

/** Public (::sir_textstyle) <-> values used to generate styled stdio output. */
typedef struct {
    const sir_textstyle from; /**< The public text style flag(s). */
    const uint16_t to;        /**< The internal value(s). */
} sir_style_16color_pair;

/** Mutex <-> protected section mapping. */
typedef enum {
    SIRMI_CONFIG = 0, /**< The ::sirconfig section. */
    SIRMI_FILECACHE,  /**< The ::sirfcache section. */
    SIRMI_TEXTSTYLE,  /**< The ::sir_level_style_tuple section. */
} sir_mutex_id;

/** Error type. */
typedef struct {
    uint32_t lasterror;
    int os_error;
    char os_errmsg[SIR_MAXERROR];

    struct {
        const char* func;
        const char* file;
        uint32_t line;
    } loc;
} sir_thread_err;

/** Bitmask defining which values are to be updated in the global config. */
typedef enum {
    SIRU_LEVELS     = 0x00000001,
    SIRU_OPTIONS    = 0x00000002,
    SIRU_SYSLOG_ID  = 0x00000004,
    SIRU_SYSLOG_CAT = 0x00000008,
    SIRU_ALL        = 0x0000000f
} sir_config_data_field;

/** Encapsulates dynamic updating of current configuration. */
typedef struct {
    uint32_t fields;
    sir_levels* levels;
    sir_options* opts;
    const char* sl_identity;
    const char* sl_category;
} sir_update_config_data;

/** Bitmask defining the state of a system logger facility. */
typedef enum {
    SIRSL_IS_OPEN  = 0x00000001,
    SIRSL_LEVELS   = 0x00000002,
    SIRSL_OPTIONS  = 0x00000004,
    SIRSL_CATEGORY = 0x00000008,
    SIRSL_IDENTITY = 0x00000010,
    SIRSL_UPDATED  = 0x00000020,
    SIRSL_IS_INIT  = 0x00000040
} sir_syslog_state;

/** @} */

#endif /* !_SIR_TYPES_H_INCLUDED */
