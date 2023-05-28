/**
 * @file sirtypes.h
 * @brief Public and internal types.
 *
 * This file and accompanying source code originated from <https://github.com/aremmell/libsir>.
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
#ifndef _SIR_TYPES_H_INCLUDED
#define _SIR_TYPES_H_INCLUDED

#include "sirplatform.h"
#include "sirconfig.h"

/**
 * @addtogroup public
 * @{
 */

/** The file identifier type. */
typedef const int* sirfileid_t;

/** The error code type. */
typedef uint32_t sirerror_t;

/** Defines the available levels \a (severity/priority) of logging output. */
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
    SIRL_ALL     = 0x00ff, /**< Includes all logging levels. */
    SIRL_DEFAULT = 0x0100  /**< Use the default levels for this type of destination. */
} sir_level;

/**
 * Used to differentiate between a single ::sir_level and one or more
 * of them bitwise OR'd together.
 */
typedef uint16_t sir_levels;

/** Formatting options for a destination. */
typedef enum {
    SIRO_ALL     = 0x00000000, /**< Effectively = ::SIRO_MSGONLY. */
    SIRO_NOTIME  = 0x00000200, /**< Exclude time stamps (implies ::SIRO_NOMSEC). */
    SIRO_NOLEVEL = 0x00000400, /**< Exclude human-readable logging level. */
    SIRO_NONAME  = 0x00000800, /**< Exclude process/app name. */
    SIRO_NOMSEC  = 0x00001000, /**< Exclude millisecond-resolution in time stamps. */
    SIRO_NOPID   = 0x00002000, /**< Exclude process ID (does not imply ::SIRO_NOTID). */
    SIRO_NOTID   = 0x00004000, /**< Exclude thread ID/name. */
    SIRO_NOHDR   = 0x00010000, /**< Don't write header messages to log files. */
    SIRO_MSGONLY = 0x000eff00, /**< Sets all other options except ::SIRO_NOHDR. */
    SIRO_DEFAULT = 0x00100000  /**< Use the default options for this type of destination. */
} sir_option;

/**
 * Used to differentiate between a single ::sir_option and one or more
 * bitwise OR'd together.
 */
typedef uint32_t sir_options;

/** Styles for 16-color console output. */
typedef enum {
    SIRS_NONE        = 0x00000000, /**< Used internally; has no effect. */
    SIRS_BRIGHT      = 0x00000001, /**< If set, the foreground color is 'intensified'. */
    SIRS_DIM         = 0x00000002, /**< If set, the foreground color is 'dimmed'. */
    SIRS_FG_BLACK    = 0x00000010, /**< Black foreground. */
    SIRS_FG_RED      = 0x00000020, /**< Red foreground. */
    SIRS_FG_GREEN    = 0x00000030, /**< Green foreground. */
    SIRS_FG_YELLOW   = 0x00000040, /**< Yellow foreground. */
    SIRS_FG_BLUE     = 0x00000050, /**< Blue foreground. */
    SIRS_FG_MAGENTA  = 0x00000060, /**< Magenta foreground. */
    SIRS_FG_CYAN     = 0x00000070, /**< Cyan foreground. */
    SIRS_FG_DEFAULT  = 0x00000080, /**< Use the default foreground color. */
    SIRS_FG_LGRAY    = 0x00000090, /**< Light gray foreground. */
    SIRS_FG_DGRAY    = 0x000000a0, /**< Dark gray foreground. */
    SIRS_FG_LRED     = 0x000000b0, /**< Light red foreground. */
    SIRS_FG_LGREEN   = 0x000000c0, /**< Light green foreground. */
    SIRS_FG_LYELLOW  = 0x000000d0, /**< Light yellow foreground. */
    SIRS_FG_LBLUE    = 0x000000e0, /**< Light blue foreground. */
    SIRS_FG_LMAGENTA = 0x000000f0, /**< Light magenta foreground. */
    SIRS_FG_LCYAN    = 0x00000f10, /**< Light cyan foreground. */
    SIRS_FG_WHITE    = 0x00000f20, /**< White foreground. */
    SIRS_BG_BLACK    = 0x00001000, /**< Black background. */
    SIRS_BG_RED      = 0x00002000, /**< Red background. */
    SIRS_BG_GREEN    = 0x00003000, /**< Green background. */
    SIRS_BG_YELLOW   = 0x00004000, /**< Yellow background. */
    SIRS_BG_BLUE     = 0x00005000, /**< Blue background. */
    SIRS_BG_MAGENTA  = 0x00006000, /**< Magenta background. */
    SIRS_BG_CYAN     = 0x00007000, /**< Cyan background. */
    SIRS_BG_DEFAULT  = 0x00008000, /**< Use the default background color. */
    SIRS_BG_LGRAY    = 0x00009000, /**< Light gray background. */
    SIRS_BG_DGRAY    = 0x0000a000, /**< Dark gray background. */
    SIRS_BG_LRED     = 0x0000b000, /**< Light red background. */
    SIRS_BG_LGREEN   = 0x0000c000, /**< Light green background. */
    SIRS_BG_LYELLOW  = 0x0000d000, /**< Light yellow background. */
    SIRS_BG_LBLUE    = 0x0000e000, /**< Light blue background. */
    SIRS_BG_LMAGENTA = 0x0000f000, /**< Light magenta background. */
    SIRS_BG_LCYAN    = 0x000f1000, /**< Light cyan background. */
    SIRS_BG_WHITE    = 0x000f2000, /**< White background. */
    SIRS_INVALID     = 0x000f3000  /**< Represents the invalid text style. */
} sir_textstyle;

/** The underlying type used for characters in output. */
typedef char sirchar_t;

/**
 * @struct sir_stdio_dest
 * @brief Configuration for stdio destinations (stdout and stderr).
 */
typedef struct {
    sir_levels  levels;
    sir_options opts;
} sir_stdio_dest;

/**
 * @struct sir_syslog_dest
 * @brief Configuration for the syslog destination.
 */
typedef struct {
    sir_levels levels;
    bool include_pid;
} sir_syslog_dest;

/**
 * @struct sirinit
 *
 * @brief Initialization data for libsir.
 *
 * Allocate an instance of this struct and pass it to ::sir_init
 * in order to begin using libsir.
 */
typedef struct {
    sir_stdio_dest d_stdout;  /**< stdout configuration. */
    sir_stdio_dest d_stderr;  /**< stderr configuration. */
#if !defined(SIR_NOSYSLOG)    
    sir_syslog_dest d_syslog; /**< syslog configuration (if available). */
#endif

    /** If set, defines the name that will appear in formatted output.
     * Set ::SIRO_NONAME for a destination to supppress it. */
    sirchar_t processName[SIR_MAXNAME];
} sirinit;

/** Library error type. */
typedef struct {
    sirerror_t code;
    const sirchar_t * const message;
} sirerror;

/** @} */

/**
 * @addtogroup intern
 * @{
 */

/** Text style attribute mask. */
#define _SIRS_ATTR_MASK 0x0000000f

/** Text style foreground color mask. */
#define _SIRS_FG_MASK 0x00000ff0

/** Text style background color mask. */
#define _SIRS_BG_MASK 0x000ff000

/** True if foreground and background colors are the same. */
#define _SIRS_SAME_COLOR(fg, bg) (((bg >> 8) & _SIRS_FG_MASK) == fg)

/** Magic number used to determine if libsir has been initialized. */
#define _SIR_MAGIC 0x60906090

/** Log file data. */
typedef struct {
    sirchar_t*  path;
    sir_levels  levels;
    sir_options opts;
    FILE* f;
    int id;
} sirfile;

/** Log file cache. */
typedef struct {
    sirfile* files[SIR_MAXFILES];
    size_t count;
} sirfcache;

/** Formatted output sent to destinations. */
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
    size_t output_len;
} sirbuf;

/** ::sir_level <-> default ::sir_textstyle mapping. */
typedef struct {
    const sir_level level; /**< The level for which the style applies. */
    uint32_t style;        /**< The default value. */
} sir_level_style_pair;

/** ::sir_level <-> string representation mapping (@ref sirconfig.h) */
typedef struct {
    const sir_level level;
    const char* str;
} sir_level_str_pair;

/** Public (::sir_textstyle) <-> values used to generate styled terminal output. */
typedef struct {
    const uint32_t from; /**< The public text style flag(s). */
    const uint16_t to;   /**< The internal value. */
} sir_style_16color_pair;

/** Mutex <-> protected section mapping. */
typedef enum {
    _SIRM_INIT = 0,  /**< The ::sirinit section. */
    _SIRM_FILECACHE, /**< The ::sirfcache section. */
    _SIRM_TEXTSTYLE, /**< The ::sir_level_style_pair section. */
} sir_mutex_id;

/** Error type. */
typedef struct {
    sirerror_t lasterror;
    int os_error;
    sirchar_t os_errmsg[SIR_MAXERROR];

    struct {
        const sirchar_t* func;
        const sirchar_t* file;
        uint32_t line;
    } loc;
} sir_thread_err;

/** Encapsulates dynamic updating of current configuration. */
typedef struct {
    sir_levels* levels;
    sir_options* opts;
} sir_update_config_data;

/** @} */

#endif /* !_SIR_TYPES_H_INCLUDED */
