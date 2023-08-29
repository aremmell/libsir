/*
 * types.h
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2023
 * Version:   2.2.3
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
# define _SIR_TYPES_H_INCLUDED

# include "sir/platform.h"
# include "sir/config.h"
# include "sir/ansimacros.h"

/**
 * @addtogroup public
 * @{
 *
 * @defgroup publictypes Types
 * @{
 */

/** Log file identifier type. */
typedef uint32_t sirfileid;

/** Plugin module identifier type. */
typedef uint32_t sirpluginid;

/** Defines the available levels (severity/priority) of logging output. */
#define SIRL_NONE    0x0000u /**< No output. */
#define SIRL_EMERG   0x0001u /**< Nuclear war, Armageddon, etc. */
#define SIRL_ALERT   0x0002u /**< Action required ASAP. */
#define SIRL_CRIT    0x0004u /**< Critical errors. */
#define SIRL_ERROR   0x0008u /**< Errors. */
#define SIRL_WARN    0x0010u /**< Warnings that could likely be ignored. */
#define SIRL_NOTICE  0x0020u /**< Normal but significant. */
#define SIRL_INFO    0x0040u /**< Informational messages. */
#define SIRL_DEBUG   0x0080u /**< Debugging/diagnostic output. */
#define SIRL_ALL     0x00ffu /**< Include all logging levels. */
#define SIRL_DEFAULT 0x0100u /**< Default levels for this type of destination. */

/** The ::sir_level type. */
typedef uint16_t sir_level;

/** ::sir_level bitmask type. */
typedef uint32_t sir_levels;

/** Formatting options for a destination. */
#define SIRO_ALL     0x00000000u /**< Include all formatting and functionality. */
#define SIRO_NOTIME  0x00000100u /**< Exclude time stamps (implies ::SIRO_NOMSEC). */
#define SIRO_NOMSEC  0x00000200u /**< Exclude millisecond-resolution in time stamps. */
#define SIRO_NOHOST  0x00000400u /**< Exclude local hostname. */
#define SIRO_NOLEVEL 0x00000800u /**< Exclude human-readable logging level. */
#define SIRO_NONAME  0x00001000u /**< Exclude process/app name. */
#define SIRO_NOPID   0x00002000u /**< Exclude process ID. */
#define SIRO_NOTID   0x00004000u /**< Exclude thread ID/name. */
#define SIRO_NOHDR   0x00010000u /**< Don't write header messages to log files. */
#define SIRO_MSGONLY 0x00007f00u /**< Sets all other options except ::SIRO_NOHDR. */
#define SIRO_DEFAULT 0x00100000u /**< Default options for this type of destination. */

/** The ::sir_option type. */
typedef uint32_t sir_option;

/** ::sir_option bitmask type. */
typedef uint32_t sir_options;

/** Color mode selection. */
typedef enum {
    SIRCM_16      = 0, /**< 4-bit 16-color mode. */
    SIRCM_256     = 1, /**< 8-bit 256-color mode. */
    SIRCM_RGB     = 2, /**< 24-bit RGB-color mode. */
    SIRCM_INVALID = 3  /**< Represents the invalid color mode. */
} sir_colormode;

/** Attributes for stdio output. */
typedef enum {
    /* attributes. */
    SIRTA_NORMAL  = 0, /**< Normal text. */
    SIRTA_BOLD    = 1, /**< Bold text. */
    SIRTA_DIM     = 2, /**< Dimmed text. */
    SIRTA_EMPH    = 3, /**< Italicized/emphasized text. */
    SIRTA_ULINE   = 4, /**< Underlined text. */
} sir_textattr;

/** Colors for stdio output (16-color mode). */
enum {
    /* 4-bit (16-color). */
    SIRTC_BLACK    = 0,  /**< Black. */
    SIRTC_RED      = 1,  /**< Red. */
    SIRTC_GREEN    = 2,  /**< Green. */
    SIRTC_YELLOW   = 3,  /**< Yellow. */
    SIRTC_BLUE     = 4,  /**< Blue. */
    SIRTC_MAGENTA  = 5,  /**< Magenta. */
    SIRTC_CYAN     = 6,  /**< Cyan. */
    SIRTC_LGRAY    = 7,  /**< Light gray. */
    SIRTC_DGRAY    = 8,  /**< Dark gray. */
    SIRTC_BRED     = 9,  /**< Bright red. */
    SIRTC_BGREEN   = 10, /**< Bright green. */
    SIRTC_BYELLOW  = 11, /**< Bright yellow. */
    SIRTC_BBLUE    = 12, /**< Bright blue. */
    SIRTC_BMAGENTA = 13, /**< Bright magenta. */
    SIRTC_BCYAN    = 14, /**< Bright cyan. */
    SIRTC_WHITE    = 15, /**< White. */
    /* 8-bit (256-color) and 24-bit (RGB color) modes:
     * use the numeric representation (16..255) instead of an enum.
     * these colors do not have defined names like the above. */
    SIRTC_DEFAULT  = 256, /**< Represents the default color. */
    SIRTC_INVALID  = 257  /**< Represents the invalid color. */
};

/** stdio text color type. */
typedef uint32_t sir_textcolor;

/**
 * @struct sir_textstyle
 * @brief Container for all the information associated with the appearance of text
 * in the context of stdio.
 *
 * For 4-bit (16-color) and 8-bit (256-color) modes, fg and bg are simply the
 * associated SIRTC_* value. For 24-bit RGB color mode, fg and bg are packed as
 * follows: 0x00rrggbb.
 */
typedef struct {
    sir_textattr attr; /**< Text attributes. */
    sir_textcolor fg;  /**< Foreground color. */
    sir_textcolor bg;  /**< Background color. */
} sir_textstyle;

/**
 * @struct sir_stdio_dest
 * @brief Configuration for stdio destinations (stdout and stderr).
 *
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
 * @see ::sir_stdio_dest
 */
typedef struct {
    sir_levels levels; /**< ::sir_level bitmask defining levels to register for. */

    /**
     * ::sir_option bitmask defining the formatting of output.
     *
     * @remark Unlike the stdio and log file destinations, not all options are
     * supported. This is due to the fact that system logging facilities typically
     * already include the information represented by ::sir_option on their own.
     *
     * Furthermore, the supported options vary based on the system logging
     * facility in use.
     *
     * @note If your system supports syslog, and libsir is compiled with the intent
     * to use it (::SIR_SYSLOG_ENABLED is defined), then at least ::SIRO_NOPID is
     * supported.
     */
    sir_options opts;

    /** Reserved for internal use; do not modify. */
    struct {
        uint32_t mask; /**< State bitmask. */
        void* logger;  /**< System logger handle/identifier. */
    } _state;

    /**
     * The identity string to pass to the system logger.
     * @see ::sir_syslogid
     */
    char identity[SIR_MAX_SYSLOG_ID];

    /**
     * The category string to pass to the system logger.
     * @see ::sir_syslogcat
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
 * @see ::sir_makeinit
 * @see ::sir_stdio_dest
 * @see ::sir_syslog_dest
 */
typedef struct {
    sir_stdio_dest d_stdout;  /**< stdout configuration. */
    sir_stdio_dest d_stderr;  /**< stderr configuration. */
    sir_syslog_dest d_syslog; /**< System logger configuration. */

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
 */

/** Magic number used to determine if libsir has been initialized. */
# define _SIR_MAGIC 0x60906090

# if defined(__WIN__)
typedef void (*invalparamfn)(const wchar_t*, const wchar_t*, const wchar_t*,
    unsigned int, uintptr_t);
# endif

/** Internally-used global config container. */
typedef struct {
    sirinit si;
    struct {
        char hostname[SIR_MAXHOST];
        time_t last_hname_chk;
        char pidbuf[SIR_MAXPID];
        pid_t pid;

        /** Spam squelch state data. */
        struct {
            bool squelch;
            uint64_t hash;
            char prefix[2];
            size_t counter;
            size_t threshold;
        } last;
    } state;
} sirconfig;

/** Internally-used error type. */
typedef struct {
    uint32_t code;
    const char* const message;
} sirerror;

/** Internally-used log file data. */
typedef struct {
    const char* path;
    sir_levels levels;
    sir_options opts;
    FILE* f;
    sirfileid id;
} sirfile;

/** Log file cache. */
typedef struct {
    sirfile* files[SIR_MAXFILES];
    size_t count;
} sirfcache;

/** The libsir-to-plugin query data structure. */
typedef struct {
    uint8_t iface_ver;  /**< Plugin interface version. */
    uint8_t maj_ver;    /**< Major version number. */
    uint8_t min_ver;    /**< Minor version number. */
    uint8_t bld_ver;    /**< Build/patch version number. */
    sir_levels levels;  /**< Level registration bitmask. */
    sir_options opts;   /**< Formatting options bitmask. */
    const char* author; /**< Plugin author information. */
    const char* desc;   /**< Plugin description. */
    uint64_t caps;      /**< Plugin capabilities bitmask. */
} sir_plugininfo;

/** Plugin versioning. */
# define SIR_PLUGIN_V1 1
# define SIR_PLUGIN_VCURRENT SIR_PLUGIN_V1

/** Plugin export names for v1 */
# define SIR_PLUGIN_EXPORT_QUERY   "sir_plugin_query"
# define SIR_PLUGIN_EXPORT_INIT    "sir_plugin_init"
# define SIR_PLUGIN_EXPORT_WRITE   "sir_plugin_write"
# define SIR_PLUGIN_EXPORT_CLEANUP "sir_plugin_cleanup"

/** Plugin export typedefs for v1 */
typedef bool (*sir_plugin_queryfn)(sir_plugininfo*);
typedef bool (*sir_plugin_initfn)(void);
typedef bool (*sir_plugin_writefn)(sir_level, const char*);
typedef bool (*sir_plugin_cleanupfn)(void);

/** Plugin interface for v1. */
typedef struct {
    sir_plugin_queryfn query;     /**< Address of sir_plugin_query. */
    sir_plugin_initfn init;       /**< Address of sir_plugin_init. */
    sir_plugin_writefn write;     /**< Address of sir_plugin_write. */
    sir_plugin_cleanupfn cleanup; /**< Address of sir_plugin_cleanup. */
} sir_pluginifacev1;

typedef sir_pluginifacev1 sir_pluginiface;

/** Internally-used plugin module data. */
typedef struct {
    const char* path;
    sir_pluginhandle handle;
    sir_plugininfo info;
    bool loaded;
    bool valid;
    sir_pluginiface iface;
    sirpluginid id;
} sir_plugin;

/** Plugin module cache. */
typedef struct {
    sir_plugin* plugins[SIR_MAXPLUGINS];
    size_t count;
} sir_plugincache;

/** A node in a sir_queue. */
typedef struct _sir_queue_node {
    struct _sir_queue_node* next;
    void* data;
} sir_queue_node;

/** FIFO queue. */
typedef struct {
    sir_queue_node* head; /**< The first node in the linked list. */
} sir_queue;

/** Job used by a job queue. */
typedef struct {
    bool (*fn)(void*); /**< Callback to be executed as part of the job. */
    void* data;        /**< Data to pass to the callback. */
} sir_threadpool_job;

/** Thread pool/job queue data container. */
typedef struct {
    sir_thread* threads; /**< A list of thread handles. */
    size_t num_threads;  /**< The number of threads in the pool. */
    sir_queue* jobs;     /**< A queue of jobs to run (FIFO). */
    sir_condition cond;  /**< A condition which indicates that a job is ready. */
    sir_mutex mutex;     /**< A mutex to be paired with the condition variable. */
    bool cancel;         /**< Causes threads in the pool to exit when true. */
} sir_threadpool;

/** Formatted output container. */
typedef struct {
    char style[SIR_MAXSTYLE];
    char timestamp[SIR_MAXTIME];
    char msec[SIR_MAXMSEC];
    const char* hostname;
    const char* pid;
    const char* level;
    const char* name;
    char tid[SIR_MAXPID];
    char message[SIR_MAXMESSAGE];
    char output[SIR_MAXOUTPUT];
    size_t output_len;
} sirbuf;

/** ::sir_level <-> ::sir_textstyle mapping. */
typedef struct {
    const sir_level level;  /**< The level for which the style applies. */
    sir_textstyle style;    /**< The un-formatted representation. */
    char str[SIR_MAXSTYLE]; /**< The formatted string representation. */
} sir_level_style_tuple;

/** Container for  text style related data that is mutex protected. */
typedef struct {
    sir_level_style_tuple* map;
    sir_colormode* color_mode;
} sir_text_style_data;

/** ::sir_level <-> human-readable string form. */
typedef struct {
    const sir_level level; /**< The level for which the string applies. */
    const char* fmt;       /**< The formatted string representation. */
} sir_level_str_pair;

/** Mutex <-> protected section mapping. */
typedef enum {
    SIRMI_CONFIG = 0,  /**< The ::sirconfig section. */
    SIRMI_FILECACHE,   /**< The ::sirfcache section. */
    SIRMI_PLUGINCACHE, /**< The ::sir_plugincache section. */
    SIRMI_TEXTSTYLE,   /**< The ::sir_level_style_tuple section. */
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
    SIRU_LEVELS     = 0x00000001, /**< Update level registrations. */
    SIRU_OPTIONS    = 0x00000002, /**< Update formatting options. */
    SIRU_SYSLOG_ID  = 0x00000004, /**< Update system logger identity. */
    SIRU_SYSLOG_CAT = 0x00000008, /**< Update system logger category. */
    SIRU_ALL        = 0x0000000f  /**< Update all available fields. */
} sir_config_data_field;

/** Encapsulates dynamic updating of current configuration. */
typedef struct {
    uint32_t fields;         /**< ::sir_config_data_field bitmask. */
    sir_levels* levels;      /**< Level registrations. */
    sir_options* opts;       /**< Formatting options. */
    const char* sl_identity; /**< System logger identity. */
    const char* sl_category; /**< System logger category. */
} sir_update_config_data;

/** Bitmask defining the state of a system logger facility. */
typedef enum {
    SIRSL_IS_OPEN  = 0x00000001, /**< Log is open. */
    SIRSL_LEVELS   = 0x00000002, /**< Level registrations. */
    SIRSL_OPTIONS  = 0x00000004, /**< Formatting options. */
    SIRSL_CATEGORY = 0x00000008, /**< Category. */
    SIRSL_IDENTITY = 0x00000010, /**< Identity. */
    SIRSL_UPDATED  = 0x00000020, /**< Config has been updated. */
    SIRSL_IS_INIT  = 0x00000040  /**< Subsystem is initialized. */
} sir_syslog_state;

#endif /* !_SIR_TYPES_H_INCLUDED */
