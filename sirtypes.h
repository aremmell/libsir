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
# define _SIR_TYPES_H_INCLUDED

# include "sirplatform.h"
# include "sirconfig.h"
# include "siransimacros.h"

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

/** Color mode selection. */
typedef enum {
    SIRCM_16      = 0, /**< 4-bit 16-color mode. */
    SIRCM_256     = 1, /**< 8-bit 256-color mode. */
    SIRCM_RGB     = 2, /**< 24-bit RGB-color mode. */
    SIRCM_INVALID = -1 /**< Represents the invalid color mode. */
} sir_colormode;

/** Attributes and colors for stdio output. */
typedef enum {
    /* attributes. */
    SIRTA_NORMAL  = 0, /**< Normal text. */
    SIRTA_BOLD    = 1, /**< Bold text. */
    SIRTA_DIM     = 2, /**< Dimmed text. */
    SIRTA_EMPH    = 3, /**< Italicized/emphasized text. */
    SIRTA_ULINE   = 4, /**< Underlined text. */
} sir_textattr;

typedef enum {
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
    SIRTC_WHITE    = 15, /**< White foreground. */
    /* 8-bit (256-color). */
    SIRTC_16       = 16,
    SIRTC_17       = 17,
    SIRTC_18       = 18,
    SIRTC_19       = 19,
    SIRTC_20       = 20,
    SIRTC_21       = 21,
    SIRTC_22       = 22,
    SIRTC_23       = 23,
    SIRTC_24       = 24,
    SIRTC_25       = 25,
    SIRTC_26       = 26,
    SIRTC_27       = 27,
    SIRTC_28       = 28,
    SIRTC_29       = 29,
    SIRTC_30       = 30,
    SIRTC_31       = 31,
    SIRTC_32       = 32,
    SIRTC_33       = 33,
    SIRTC_34       = 34,
    SIRTC_35       = 35,
    SIRTC_36       = 36,
    SIRTC_37       = 37,
    SIRTC_38       = 38,
    SIRTC_39       = 39,
    SIRTC_40       = 40,
    SIRTC_41       = 41,
    SIRTC_42       = 42,
    SIRTC_43       = 43,
    SIRTC_44       = 44,
    SIRTC_45       = 45,
    SIRTC_46       = 46,
    SIRTC_47       = 47,
    SIRTC_48       = 48,
    SIRTC_49       = 49,
    SIRTC_50       = 50,
    SIRTC_51       = 51,
    SIRTC_52       = 52,
    SIRTC_53       = 53,
    SIRTC_54       = 54,
    SIRTC_55       = 55,
    SIRTC_56       = 56,
    SIRTC_57       = 57,
    SIRTC_58       = 58,
    SIRTC_59       = 59,
    SIRTC_60       = 60,
    SIRTC_61       = 61,
    SIRTC_62       = 62,
    SIRTC_63       = 63,
    SIRTC_64       = 64,
    SIRTC_65       = 65,
    SIRTC_66       = 66,
    SIRTC_67       = 67,
    SIRTC_68       = 68,
    SIRTC_69       = 69,
    SIRTC_70       = 70,
    SIRTC_71       = 71,
    SIRTC_72       = 72,
    SIRTC_73       = 73,
    SIRTC_74       = 74,
    SIRTC_75       = 75,
    SIRTC_76       = 76,
    SIRTC_77       = 77,
    SIRTC_78       = 78,
    SIRTC_79       = 79,
    SIRTC_80       = 80,
    SIRTC_81       = 81,
    SIRTC_82       = 82,
    SIRTC_83       = 83,
    SIRTC_84       = 84,
    SIRTC_85       = 85,
    SIRTC_86       = 86,
    SIRTC_87       = 87,
    SIRTC_88       = 88,
    SIRTC_89       = 89,
    SIRTC_90       = 90,
    SIRTC_91       = 91,
    SIRTC_92       = 92,
    SIRTC_93       = 93,
    SIRTC_94       = 94,
    SIRTC_95       = 95,
    SIRTC_96       = 96,
    SIRTC_97       = 97,
    SIRTC_98       = 98,
    SIRTC_99       = 99,
    SIRTC_100      = 100,
    SIRTC_101      = 101,
    SIRTC_102      = 102,
    SIRTC_103      = 103,
    SIRTC_104      = 104,
    SIRTC_105      = 105,
    SIRTC_106      = 106,
    SIRTC_107      = 107,
    SIRTC_108      = 108,
    SIRTC_109      = 109,
    SIRTC_110      = 110,
    SIRTC_111      = 111,
    SIRTC_112      = 112,
    SIRTC_113      = 113,
    SIRTC_114      = 114,
    SIRTC_115      = 115,
    SIRTC_116      = 116,
    SIRTC_117      = 117,
    SIRTC_118      = 118,
    SIRTC_119      = 119,
    SIRTC_120      = 120,
    SIRTC_121      = 121,
    SIRTC_122      = 122,
    SIRTC_123      = 123,
    SIRTC_124      = 124,
    SIRTC_125      = 125,
    SIRTC_126      = 126,
    SIRTC_127      = 127,
    SIRTC_128      = 128,
    SIRTC_129      = 129,
    SIRTC_130      = 130,
    SIRTC_131      = 131,
    SIRTC_132      = 132,
    SIRTC_133      = 133,
    SIRTC_134      = 134,
    SIRTC_135      = 135,
    SIRTC_136      = 136,
    SIRTC_137      = 137,
    SIRTC_138      = 138,
    SIRTC_139      = 139,
    SIRTC_140      = 140,
    SIRTC_141      = 141,
    SIRTC_142      = 142,
    SIRTC_143      = 143,
    SIRTC_144      = 144,
    SIRTC_145      = 145,
    SIRTC_146      = 146,
    SIRTC_147      = 147,
    SIRTC_148      = 148,
    SIRTC_149      = 149,
    SIRTC_150      = 150,
    SIRTC_151      = 151,
    SIRTC_152      = 152,
    SIRTC_153      = 153,
    SIRTC_154      = 154,
    SIRTC_155      = 155,
    SIRTC_156      = 156,
    SIRTC_157      = 157,
    SIRTC_158      = 158,
    SIRTC_159      = 159,
    SIRTC_160      = 160,
    SIRTC_161      = 161,
    SIRTC_162      = 162,
    SIRTC_163      = 163,
    SIRTC_164      = 164,
    SIRTC_165      = 165,
    SIRTC_166      = 166,
    SIRTC_167      = 167,
    SIRTC_168      = 168,
    SIRTC_169      = 169,
    SIRTC_170      = 170,
    SIRTC_171      = 171,
    SIRTC_172      = 172,
    SIRTC_173      = 173,
    SIRTC_174      = 174,
    SIRTC_175      = 175,
    SIRTC_176      = 176,
    SIRTC_177      = 177,
    SIRTC_178      = 178,
    SIRTC_179      = 179,
    SIRTC_180      = 180,
    SIRTC_181      = 181,
    SIRTC_182      = 182,
    SIRTC_183      = 183,
    SIRTC_184      = 184,
    SIRTC_185      = 185,
    SIRTC_186      = 186,
    SIRTC_187      = 187,
    SIRTC_188      = 188,
    SIRTC_189      = 189,
    SIRTC_190      = 190,
    SIRTC_191      = 191,
    SIRTC_192      = 192,
    SIRTC_193      = 193,
    SIRTC_194      = 194,
    SIRTC_195      = 195,
    SIRTC_196      = 196,
    SIRTC_197      = 197,
    SIRTC_198      = 198,
    SIRTC_199      = 199,
    SIRTC_200      = 200,
    SIRTC_201      = 201,
    SIRTC_202      = 202,
    SIRTC_203      = 203,
    SIRTC_204      = 204,
    SIRTC_205      = 205,
    SIRTC_206      = 206,
    SIRTC_207      = 207,
    SIRTC_208      = 208,
    SIRTC_209      = 209,
    SIRTC_210      = 210,
    SIRTC_211      = 211,
    SIRTC_212      = 212,
    SIRTC_213      = 213,
    SIRTC_214      = 214,
    SIRTC_215      = 215,
    SIRTC_216      = 216,
    SIRTC_217      = 217,
    SIRTC_218      = 218,
    SIRTC_219      = 219,
    SIRTC_220      = 220,
    SIRTC_221      = 221,
    SIRTC_222      = 222,
    SIRTC_223      = 223,
    SIRTC_224      = 224,
    SIRTC_225      = 225,
    SIRTC_226      = 226,
    SIRTC_227      = 227,
    SIRTC_228      = 228,
    SIRTC_229      = 229,
    SIRTC_230      = 230,
    SIRTC_231      = 231,
    SIRTC_232      = 232, /**< Greyscale begins here, through 255. */
    SIRTC_233      = 233,
    SIRTC_234      = 234,
    SIRTC_235      = 235,
    SIRTC_236      = 236,
    SIRTC_237      = 237,
    SIRTC_238      = 238,
    SIRTC_239      = 239,
    SIRTC_240      = 240,
    SIRTC_241      = 241,
    SIRTC_242      = 242,
    SIRTC_243      = 243,
    SIRTC_244      = 244,
    SIRTC_245      = 245,
    SIRTC_246      = 246,
    SIRTC_247      = 247,
    SIRTC_248      = 248,
    SIRTC_249      = 249,
    SIRTC_250      = 250,
    SIRTC_251      = 251,
    SIRTC_252      = 252,
    SIRTC_253      = 253,
    SIRTC_254      = 254,
    SIRTC_255      = 255,
    SIRTC_DEFAULT  = 256, /**< Represents the default color. */
    SIRTC_INVALID  = -1   /**< Represents the invalid color. */
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
    sir_textattr attrs; /**< Text attributes. */
    sir_textcolor fg;   /**< Foreground color. */
    sir_textcolor bg;   /**< Background color. */
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
     * factility in use.
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

    /** The color mode to use for stdio. Defaults to 4-bit (16-color) mode. */
    sir_colormode color_mode;

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

/** ::sir_level <-> human-readable string form. */
typedef struct {
    const sir_level level; /**< The level for which the string applies. */
    const char* fmt;       /**< The formatted string representation. */
} sir_level_str_pair;

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
