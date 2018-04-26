/*!
 * \file sir.h
 * \brief Definitions for the Standard Incident Reporter (SIR) library.
 *
 * \author Ryan Matthew Lederman <lederman@gmail.com>
 * \version 1.1.0
 * \date 2003-2018
 * \copyright MIT License
 */

#ifndef _SIR_H_INCLUDED
#define _SIR_H_INCLUDED

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#ifndef _WIN32
/*! The sequence of characters used to terminate output messages. */
#define SIR_LINEENDING "\n"
#else
/*! The sequence of characters used to terminate output messages. */
#define SIR_LINEENDING "\r\n"
#endif

/*! The maximum number of log files that may be registered. */
#define SIR_MAXFILES 16

/*! The maximum number of characters that may be included in one message. */
#define SIR_MAXMESSAGE 2048

/*! The initial size, in characters, of the buffer used to store messages. */
#define SIR_MINPRINT 512

/*! The default time format string passed to localtime(). */
#define SIR_TIMEFORMAT "%T"

/*! The size, in characters, of the buffer used to hold time format strings. */
#define SIR_MAXTIME 32

/*! The size, in characters, of the buffer used to hold level format strings. */
#define SIR_MAXLEVEL 16

/*! The size, in characters, of the buffer used to hold
 * process/appname format strings.
 */
#define SIR_MAXNAME 16

/*! The string representation of the ::SIRL_EMERG level in output. */
#define SIRL_S_EMERG "EMER"

/*! The string representation of the ::SIRL_ALERT level in output. */
#define SIRL_S_ALERT "ALRT"

/*! The string representation of the ::SIRL_CRIT level in output. */
#define SIRL_S_CRIT "CRIT"

/*! The string representation of the ::SIRL_ERROR level in output. */
#define SIRL_S_ERROR "ERR"

/*! The string representation of the ::SIRL_WARN level in output. */
#define SIRL_S_WARN "WARN"

/*! The string representation of the ::SIRL_NOTICE level in output. */
#define SIRL_S_NOTICE "NOTF"

/*! The string representation of the ::SIRL_INFO level in output. */
#define SIRL_S_INFO "INFO"

/*! The string representation of the ::SIRL_DEBUG level in output. */
#define SIRL_S_DEBUG "DBG"

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
 */
typedef enum {
    SIRO_NOLEVEL = 0x100, /*!< Do not include the human-readable logging level in output. */
    SIRO_NONAME = 0x200,  /*!< Do not include the process/app name in output. */
    SIRO_NOTIME = 0x400,  /*!< Do not include time stamps in output. */
} sir_option;

/*! One or more ::sir_option, bitwise OR'd. */
typedef uint16_t sir_options;

/*! The underlying type to use for characters in output. */
typedef char sirchar_t;

/*! \struct sirinit
 *
 * \brief Initialization userData for the library.
 *
 * Allocate an instance of this struct and pass it to ::sir_init
 * in order to begin using the library.
 *
 * Don't forget to call ::sir_cleanup when you're done.
 */
typedef struct {
    /*! Logging levels (::sir_level) to route to stdout. */
    sir_levels stdOutLevels;

    /*! Output options (::sir_option) for stdout. */
    sir_options stdOutOptions;

    /*! Logging levels (::sir_level) to route to stderr. */
    sir_levels stdErrLevels;

    /*! Output options (::sir_option) for stderr. */
    sir_options stdErrOptions;

#ifndef _WIN32
    /*! Logging levels (::sir_level) to route to syslog. Only available if
     * _WIN32 is not defined. */
    sir_levels sysLogLevels;

    /*! Output options (::sir_option) for syslog. Only available if _WIN32 is
     * not defined. */
    sir_options sysLogOptions;
#endif

    /*! A custom name to include in output (default: process name). If
     * ::SIRO_NONAME is set in the options for the destination, no name is
     * included in output. Set to NULL for the default.
     */
    const sirchar_t* appName;

    /*! A custom time format to use in output (default: ::SIR_TIMEFORMAT). If
     * ::SIRO_NOTIME is set in the options for the destination, no time is
     * included in output. Set to NULL for the default.
     */
    const sirchar_t* timeFmt;

    /*! Where SIR should send its own internal logging (default: stderr) to
     * assist in diagnosing any potential issues that may arise.  Set to NULL for the default.
     *
     * Note that if
     * set, you need to use setlinebuf() in order to avoid any delays in output.
     *
     * Additionally, if set, SIR will \a not close the stream upon cleanup.
     */
    FILE* selfOutput;
} sirinit;

#ifdef __cplusplus
extern "C" {
#endif /* !__cplusplus */

/*! \fn int sir_init(const sirinit *si)
 *
 * \brief Initializes the library.
 *
 * You must call this function before any others in the library
 * or you're gonna have a bad time.
 *
 * \param[in] si Pointer a to sirinit struct containing initialization options.
 *
 * \return boolean success
 */
bool sir_init(const sirinit* si);

/*! \fn int sirdebug(const sirchar_t *format, ...)
 *
 * \brief Log a formatted debug-level message.
 *
 * All output destinations whose flags have ::SIRL_DEBUG set
 * will receive the formatted message.
 *
 * \param[in] format A printf-style format string.
 * \param[in] ... A list of arguments whose type match the format
 *  specifier at the same index in the format string.
 *
 * \return boolean success
 */
bool sirdebug(const sirchar_t* format, ...);

/*! \fn int sirinfo(const sirchar_t *format, ...)
 *
 * \brief Log a formatted informational message.
 *
 * All output destinations whose flags have ::SIRL_INFO set
 * will receive the formatted message.
 *
 * \param[in] format A printf-style format string.
 * \param[in] ... A list of arguments whose type match the format
 *  specifier at the same index in the format string.
 *
 * \return boolean success
 */
bool sirinfo(const sirchar_t* format, ...);

/*! \fn int sirnotice(const sirchar_t *format, ...)
 *
 * \brief Log a formatted notice message.
 *
 * All output destinations whose flags have ::SIRL_NOTICE set
 * will receive the formatted message.
 *
 * \param[in] format A printf-style format string.
 * \param[in] ... A list of arguments whose type match the format
 *  specifier at the same index in the format string.
 *
 * \return boolean success
 */
bool sirnotice(const sirchar_t* format, ...);

/*! \fn int sirwarn(const sirchar_t *format, ...)
 *
 * \brief Log a formatted warning message.
 *
 * All output destinations whose flags have ::SIRL_WARN set
 * will receive the formatted message.
 *
 * \param[in] format A printf-style format string.
 * \param[in] ... A list of arguments whose type match the format
 *  specifier at the same index in the format string.
 *
 * \return boolean success
 */
bool sirwarn(const sirchar_t* format, ...);

/*! \fn int sirerror(const sirchar_t *format, ...)
 *
 * \brief Log a formatted error message.
 *
 * All output destinations whose flags have ::SIRL_ERROR set
 * will receive the formatted message.
 *
 * \param[in] format A printf-style format string.
 * \param[in] ... A list of arguments whose type match the format
 *  specifier at the same index in the format string.
 *
 * \return boolean success
 */
bool sirerror(const sirchar_t* format, ...);

/*! \fn int sircrit(const sirchar_t *format, ...)
 *
 * \brief Log a formatted critical message.
 *
 * All output destinations whose flags have ::SIRL_CRIT set
 * will receive the formatted message.
 *
 * \param[in] format A printf-style format string.
 * \param[in] ... A list of arguments whose type match the format
 *  specifier at the same index in the format string.
 *
 * \return boolean success
 */
bool sircrit(const sirchar_t* format, ...);

/*! \fn int siralert(const sirchar_t *format, ...)
 *
 * \brief Log a formatted alert message.
 *
 * All output destinations whose flags have ::SIRL_ALERT set
 * will receive the formatted message.
 *
 * \param[in] format A printf-style format string.
 * \param[in] ... A list of arguments whose type match the format
 *  specifier at the same index in the format string.
 *
 * \return boolean success
 */
bool siralert(const sirchar_t* format, ...);

/*! \fn int siremerg(const sirchar_t *format, ...)
 *
 * \brief Log a formatted emergency message.
 *
 * All output destinations whose flags have ::SIRL_EMERG set
 * will receive the formatted message.
 *
 * \param[in] format A printf-style format string.
 * \param[in] ... A list of arguments whose type match the format
 *  specifier at the same index in the format string.
 *
 * \return boolean success
 */
bool siremerg(const sirchar_t* format, ...);

/*! \fn void sir_cleanup()
 *
 * \brief Shuts down the library.
 *
 * Call to free all allocated memory and reset internal data structures
 * to their default state.
 *
 * \return none
 */
void sir_cleanup();

/*! \fn int sir_addfile(const sirchar_t* path, sir_levels levels, sir_options
 * opts)
 *
 * \brief Adds a log file output destination.
 *
 * Adds a log file to be appended to whenever a message
 * matching any of the ::sir_level flags set in \a levels is emitted.
 *
 * \param[in] path The path to the log file. It will be created if it doesn't
 * exist. \param[in] levels One or more ::sir_level logging levels for which
 * output should be append to the file. \param[in] opts Zero or more
 * ::sir_option flags to control output formatting.
 *
 * \return A unique identifier for the file, or zero if an error occurs.
 */
int sir_addfile(const sirchar_t* path, sir_levels levels, sir_options opts);

/*! \fn bool sir_remfile(int id)
 *
 * \brief Removes a previously added log file.
 *
 * \param[in] id The identifier returned from ::sir_addfile earlier.
 *
 * \return boolean success
 */
bool sir_remfile(int id);

#ifdef __cplusplus
}
#endif /* !__cplusplus */

/*! \cond PRIVATE */

typedef struct {
    sirchar_t*  path;
    sir_levels  levels;
    sir_options opts;
    FILE*       f;
    int         fd;
    int         id;
} sirfile;

typedef struct {
    sirfile* files[SIR_MAXFILES];
    size_t   count;
} sirfiles;

typedef struct {
    sirchar_t* ptr;
    size_t     size;
    size_t     used;
} sirbuf;

typedef struct {
    sirchar_t timestamp[SIR_MAXTIME];
    sirchar_t level[SIR_MAXLEVEL];
    sirchar_t name[SIR_MAXNAME];
    sirchar_t message[SIR_MAXMESSAGE];
} siroutput;

#define _SIR_L_START(format) \
    bool    r = false;       \
    va_list args;            \
    va_start(args, format);

#define _SIR_L_END(args) va_end(args);

#define validstr(n) (NULL != n && *n != '\0')

#define flagtest(flags, test) ((flags & test) == test)

#define safefree(p) \
    if (p) {        \
        free(p);    \
        p = NULL;   \
    }

#define safefclose(f) \
    if (f) {          \
        fclose(f);    \
        f = NULL;     \
    }

bool _sir_lv(sir_level level, const sirchar_t* format, va_list args);
bool _sir_l(const sirchar_t* format, ...);

bool _sir_dispatch(sir_level level, const siroutput* output);

bool _sirbuf_append(sirbuf* buf, const sirchar_t* str);
bool _sirbuf_reset(sirbuf* buf, bool realloc);

sirfile* _sirfile_create(int id, const sirchar_t* path, sir_levels levels, sir_options opts);
bool     _sirfile_write(sirfile* sf, const sirchar_t* output);
void     _sirfile_destroy(sirfile* sf);
bool     _sirfile_validate(sirfile* sf);

FILE* _sir_fopen(const sirchar_t* path);
void  _sir_fclose(FILE** f);

bool _sir_files_add(sirfiles* sfc, const sirchar_t* path, sir_levels levels, sir_options opts);
bool _sir_files_rem(sirfiles* sfc, int id);
bool _sir_files_destroy(sirfiles* sfc);
bool _sir_files_dispatch(sirfiles* sfc, sir_level level, const siroutput* output);

bool _sir_options_sanity(const sirinit* si);

const sirchar_t* _sir_levelstr(sir_level level);
bool             _sir_destwantslevel(sir_levels destLevels, sir_level level);

/*! \endcond */

#endif /* !_SIR_H_INCLUDED */
