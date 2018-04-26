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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#ifndef _WIN32

#ifndef SIR_NO_SYSLOG
#include <syslog.h>
#endif

/*! \typedef */
typedef char sirchar_t;

#ifndef _T
#define _T(x) x
#endif
#define sctime ctime
#define sstrlen strlen
#define svsnprintf vsnprintf
#define sfprintf fprintf
#define sfopen fopen
#define sstrcmp strcmp

#else

#include <Windows.h>
#include <tchar.h>

#define sctime _tctime
#define sstrlen _tcslen
#define svsnprintf _vsntprintf
#define sfprintf _ftprintf
#define sfopen _tfopen
#define sstrcmp _tcscmp

typdef TCHAR sirchar_t;

#endif

#define strptr(n) (((n != NULL) && ((*n) != 0)) ? 1 : 0)
#define smaskset(flags, test) ((flags & test) == test)
#define safefree(p)  \
    if (NULL != p) { \
        free(p);     \
        p = NULL;    \
    }

#define SIR_MAXFILES 256
#define SIR_MAXCALLBACKS 256
#define SIR_MAXPRINT 2000
#define SIR_MINPRINT 256

#define SIR_TIMEFORMAT "%T"
#define SIR_TIMELEN 32

#define SIRO_LF 0x01U
#define SIRO_FILES 0x02U
#define SIRO_CALLBACKS 0x04U
#define SIRO_CRLF 0x08U
#define SIRO_NODBGCRLF 0x10U
#define SIRO_NOLEVEL 0x20U
#define SIRO_NONAME 0x40U
#define SIRO_NOTIMESTAMP 0x80U
#define SIRO_DEFAULT (SIRO_TIMESTAMP | SIRO_LF)

#define SIRL_S_EMERG "EMER"
#define SIRL_S_ALERT "ALRT"
#define SIRL_S_CRIT "CRIT"
#define SIRL_S_ERROR "ERR"
#define SIRL_S_WARNING "WARN"
#define SIRL_S_NOTICE "NOTF"
#define SIRL_S_INFO "INFO"
#define SIRL_S_DEBUG "DBG"

/*! Logging levels
 *
 * Each enum value corresponds to a function that sends output
 * to all destinations for that level (::sirdebug and pals).
 * 
 * Since the values are also flags (i.e.: powers of two),
 * they can be combined to filter different levels of importance but not strictly linearly. Any
 * combination of levels can be used to configure an output destination.
 */
enum {
    SIRL_EMERG   = 0x1,  /*!< Nuclear war, Armageddon, etc.  */
    SIRL_ALERT   = 0x2,  /*!< Action required ASAP. */
    SIRL_CRIT    = 0x4,  /*!< Critical errors. */
    SIRL_ERROR   = 0x8,  /*!< Errors. */
    SIRL_WARNING = 0x10, /*!< Warnings that could likely be ignored. */
    SIRL_NOTICE  = 0x20, /*!< Normal but significant. */
    SIRL_INFO    = 0x40, /*!< Informational messages. */
    SIRL_DEBUG   = 0x80, /*!< Verbose debugging output. */
    SIRL_ALL     = 0xFF  /*!< Includes all logging levels. */
};

/*! Function signature for callback output destinations.
 *
 * \param[in] message The formatted message.
 * \param[in] userData User-defined data to pass along to the callback.
 */
typedef void (*sircallbackfn)(const sirchar_t* message, uint64_t userData);

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
    uint32_t         opts;
    uint32_t         f_stdout;
    uint32_t         f_stderr;
    uint32_t         f_debug;
    uint32_t         f_syslog;
    const sirchar_t* fmtOverride;
    const sirchar_t* appName;
} sirinit;

typedef struct {
    sirchar_t* path;
    uint32_t   bitmask;
} sirfile;

typedef struct {
    sirfile* files[SIR_MAXFILES];
    size_t   count;
} sirfiles;

typedef struct {
    sircallbackfn cb;
    uint32_t      mask;
    uint64_t      data;
} sircallback;

typedef struct {
    sircallback* c[SIR_MAXCALLBACKS];
    size_t       count;
} sircallbacks;

typedef struct {
    sirchar_t* ptr;
    size_t     size;
} sirbuf;

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
 * \return 0 if successful, or non-zero otherwise.
 */
int sir_init(const sirinit* si);

/*! \fn int sirdebug(const sirchar_t *format, ...)
 *
 * \brief Log a formatted debug-level message.
 *
 * All output destinations whose flags have ::SIRL_DEBUG set
 * will receive the formatted message.
 *
 * \param[in] format A printf-style format string.
 * \param[in] (varargs) A list of arguments whose type match the format
 *  specifier at the same index in the format string.
 * \return 0 if successful, or non-zero otherwise.
 */
int sirdebug(const sirchar_t* format, ...);

/*! \fn int sirinfo(const sirchar_t *format, ...)
 *
 * \brief Log a formatted informational message.
 *
 * All output destinations whose flags have ::SIRL_INFO set
 * will receive the formatted message.
 *
 * \param[in] format A printf-style format string.
 * \param[in] (varargs) A list of arguments whose type match the format
 *  specifier at the same index in the format string.
 * \return 0 if successful, or non-zero otherwise.
 */
int sirinfo(const sirchar_t* format, ...);

/*! \fn int sirnotice(const sirchar_t *format, ...)
 *
 * \brief Log a formatted notice message.
 *
 * All output destinations whose flags have ::SIRL_NOTICE set
 * will receive the formatted message.
 *
 * \param[in] format A printf-style format string.
 * \param[in] (varargs) A list of arguments whose type match the format
 *  specifier at the same index in the format string.
 * \return 0 if successful, or non-zero otherwise.
 */
int sirnotice(const sirchar_t* format, ...);

/*! \fn int sirwarn(const sirchar_t *format, ...)
 *
 * \brief Log a formatted warning message.
 *
 * All output destinations whose flags have ::SIRL_WARNING set
 * will receive the formatted message.
 *
 * \param[in] format A printf-style format string.
 * \param[in] (varargs) A list of arguments whose type match the format
 *  specifier at the same index in the format string.
 * \return 0 if successful, or non-zero otherwise.
 */
int sirwarn(const sirchar_t* format, ...);

/*! \fn int sirerror(const sirchar_t *format, ...)
 *
 * \brief Log a formatted error message.
 *
 * All output destinations whose flags have ::SIRL_ERROR set
 * will receive the formatted message.
 *
 * \param[in] format A printf-style format string.
 * \param[in] (varargs) A list of arguments whose type match the format
 *  specifier at the same index in the format string.
 * \return 0 if successful, or non-zero otherwise.
 */
int sirerror(const sirchar_t* format, ...);

/*! \fn int sircrit(const sirchar_t *format, ...)
 *
 * \brief Log a formatted critical message.
 *
 * All output destinations whose flags have ::SIRL_CRIT set
 * will receive the formatted message.
 *
 * \param[in] format A printf-style format string.
 * \param[in] (varargs) A list of arguments whose type match the format
 *  specifier at the same index in the format string.
 * \return 0 if successful, or non-zero otherwise.
 */
int sircrit(const sirchar_t* format, ...);

/*! \fn int siralert(const sirchar_t *format, ...)
 *
 * \brief Log a formatted alert message.
 *
 * All output destinations whose flags have ::SIRL_ALERT set
 * will receive the formatted message.
 *
 * \param[in] format A printf-style format string.
 * \param[in] (varargs) A list of arguments whose type match the format
 *  specifier at the same index in the format string.
 * \return 0 if successful, or non-zero otherwise.
 */
int siralert(const sirchar_t* format, ...);

/*! \fn int siremerg(const sirchar_t *format, ...)
 *
 * \brief Log a formatted emergency message.
 *
 * All output destinations whose flags have ::SIRL_EMERG set
 * will receive the formatted message.
 *
 * \param[in] format A printf-style format string.
 * \param[in] (varargs) A list of arguments whose type match the format
 *  specifier at the same index in the format string.
 * \return 0 if successful, or non-zero otherwise.
 */
int siremerg(const sirchar_t* format, ...);

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

/*
 * FUNCTION: sir_addfile()
 *
 * SYNOPSIS: Adds a disk file as an output destination
 * maintained by the Sir system.  The file will recieve
 * output whenever Sir() is called with any of the flags
 * you specify in <mask>.
 *
 * ARGUMENTS: Pointer to a string that contains
 * the path of the file, and an unsigned long bitmask
 * of the types you wish to associate with this file.
 * (SIRT_*)
 *
 * RETURN VALUE: Returns 0 if successful, or -1
 * if an error occurs.
 *
 */

int sir_addfile(const sirchar_t* path, uint32_t mask);

/*
 * FUNCTION: sir_remfile()
 *
 * SYNOPSIS: Removes an existing disk file
 * from the Sir system's managed queue.
 *
 * ARGUMENTS: Pointer to a string that contains
 * the path of the file.
 *
 * RETURN VALUE: Returns 0 if successful, or -1
 * if an error occurs.
 *
 */
int sir_remfile(const sirchar_t* path);

/*
 * FUNCTION: sir_addcallback()
 *
 * SYNOPSIS: Adds a callback function as an output destination
 * maintained by the Sir system.  The function will be called
 * whenever Sir() is called with any of the flags you specify
 * in <mask> (SIRT_*).
 *
 * ARGUMENTS: Pointer to a sircallbackfn-type function (sir.h),
 * unsigned long bitmask of types to associate with the function,
 * and an unsigned long that will be maintained by the Sir system
 * and sent to the function whenever it is called.
 *
 * RETURN VALUE: Returns 0 if successful, or -1
 * if an error occurs.
 *
 */
int sir_addcallback(sircallbackfn cb, uint32_t mask, uint64_t data);

/*! \fn int sir_remcallback(sircallbackfn cb)
 *
 * \brief Remove an existing \ref sircallbackfn from the list of functions that can receive log messages.
 *
 * \return 0 if successful, or non-zero otherwise.
 */
int sir_remcallback(sircallbackfn cb);

#ifdef __cplusplus
}
#endif /* !__cplusplus */

/*! \cond PRIVATE */

#define _SIR_L_START()  \
    va_list args = {0}; \
    int     r    = 1;

#define _SIR_L_END(args) va_end(args);

int _sir_lv(uint32_t type, const sirchar_t* format, va_list args);

int  _sir_dispatchfile(const sirchar_t* path, const sirchar_t* output);
int  _sir_dispatchall(const sirchar_t* output, uint32_t mask);
void _sir_dispatch(const sirchar_t* output, uint32_t type);

sirbuf* _sirbuf_create(void);
int     _sirbuf_append(sirbuf* buf, const sirchar_t* str);
void    _sirbuf_destroy(sirbuf* buf);

sirfile* _sirfile_create(const sirchar_t* path, uint32_t mask);
void     _sirfile_destroy(sirfile* sf);

int _sir_files_add(sirfiles* sfc, sirfile* pf);
int _sir_files_rem(sirfiles* sfc, const sirchar_t* path);
int _sir_files_destroy(sirfiles* sfc);
int _sir_files_dispatch(sirfiles* sfc, const sirchar_t* output, uint32_t mask);

sircallback* _sircallback_create(sircallbackfn cb, uint32_t mask, uint64_t userData);
void         _sircallback_destroy(sircallback* pco);

int _sir_callbacks_add(sircallbacks* scc, sircallback* scb);
int _sir_callbacks_rem(sircallbacks* scc, sircallbackfn cb);
int _sir_callbacks_destroy(sircallbacks* scc);
int _sir_callbacks_dispatch(sircallbacks* scc, const sirchar_t* output, uint32_t mask);

/*! \endcond */

#endif /* !_SIR_H_INCLUDED */
