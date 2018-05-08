/**
 * @file sirerrors.h
 * @brief Library error management.
 */
#ifndef _SIR_ERRORS_H_INCLUDED
#define _SIR_ERRORS_H_INCLUDED

#include "sirhelpers.h"
#include "sirtypes.h"

/** 
 * @defgroup errors Error handling
 * 
 * Functions, types, and codes used for error handling.
 * 
 * @addtogroup errors 
 * @{
 */

/** Error codes. */
enum {
    SIR_E_NOERROR   = 0,    /**< The operation completed successfully */
    SIR_E_NOTREADY  = 1,    /**< SIR has not been initialized */
    SIR_E_ALREADY   = 2,    /**< SIR is already initialized */
    SIR_E_DUPFILE   = 3,    /**< File already managed by SIR */
    SIR_E_NOFILE    = 4,    /**< File not managed by SIR */
    SIR_E_FCFULL    = 5,    /**< Maximum number of files already managed */
    SIR_E_OPTIONS   = 6,    /**< Option flags are invalid */
    SIR_E_LEVELS    = 7,    /**< Level flags are invalid */
    SIR_E_TEXTSTYLE = 8,    /**< Text style is invalid */
    SIR_E_STRING    = 9,    /**< Invalid string argument */
    SIR_E_NODEST    = 10,   /**< No destinations registered for level */
    SIR_E_PLATFORM  = 11,   /**< Platform error code %d: %s */
    SIR_E_UNKNOWN   = 4095, /**< Error is not known */    
};

/** @} */

#define _SIR_E_NOERROR   SIR_E_NOERROR
#define _SIR_E_UNKNOWN   _sir_mkerror(SIR_E_UNKNOWN)
#define _SIR_E_NOTREADY  _sir_mkerror(SIR_E_NOTREADY)
#define _SIR_E_ALREADY   _sir_mkerror(SIR_E_ALREADY)
#define _SIR_E_DUPFILE   _sir_mkerror(SIR_E_DUPFILE)
#define _SIR_E_NOFILE    _sir_mkerror(SIR_E_NOFILE)
#define _SIR_E_FCFULL    _sir_mkerror(SIR_E_FCFULL)
#define _SIR_E_OPTIONS   _sir_mkerror(SIR_E_OPTIONS)
#define _SIR_E_LEVELS    _sir_mkerror(SIR_E_LEVELS)
#define _SIR_E_TEXTSTYLE _sir_mkerror(SIR_E_TEXTSTYLE)
#define _SIR_E_STRING    _sir_mkerror(SIR_E_STRING)
#define _SIR_E_NODEST    _sir_mkerror(SIR_E_NODEST)
#define _SIR_E_PLATFORM  _sir_mkerror(SIR_E_PLATFORM)

static const struct {
    sirerror_t       e;
    const sirchar_t* msg;
} sir_errors[] = {
    { _SIR_E_NOERROR, "The operation completed successfully" },
    { _SIR_E_NOTREADY, "SIR has not been initialized" },
    { _SIR_E_ALREADY, "SIR is already initialized" },
    { _SIR_E_DUPFILE, "File already managed by SIR" },
    { _SIR_E_NOFILE, "File not managed by SIR" },
    { _SIR_E_FCFULL, "Maximum number of files already managed" },
    { _SIR_E_OPTIONS, "Option flags are invalid" },
    { _SIR_E_LEVELS, "Level flags are invalid" },
    { _SIR_E_TEXTSTYLE, "Text style is invalid" },
    { _SIR_E_STRING, "Invalid string argument" },
    { _SIR_E_NODEST, "No destinations registered for level" },
    { _SIR_E_PLATFORM, "Platform error code %d: %s" },
};

void __sir_seterror(sirerror_t err, const sirchar_t* func, const sirchar_t* file, uint32_t line);
#define _sir_seterror(err) __sir_seterror(err, __func__, __FILE__, __LINE__)

void __sir_setoserror(int, const sirchar_t*, const sirchar_t*, const sirchar_t*, uint32_t);
#define _sir_setoserror(err, msg) __sir_setoserror(err, msg, __func__, __FILE__, __LINE__)

/** Handle a POSIX error. */
void __sir_handleerr(int code, const sirchar_t* func, const sirchar_t* file, uint32_t line);
#define _sir_handleerr(code) __sir_handleerr(code, __func__, __FILE__, __LINE__)

#ifdef _WIN32
/**
 * Some Win32 API error codes overlap POSIX error codes, so they need to be handled separately.
 * Mapping them sounds great, but in practice valuable information about what went wrong is totally
 * lost in translation.
 */
void __sir_handlewin32err(DWORD code, const sirchar_t* func, const sirchar_t* file, uint32_t line);
#define _sir_handlewin32err(code) __sir_handlewin32err(code, __func__, __FILE__, __LINE__)
#endif

/** Returns information about the last error that occurred. */
sirerror_t _sir_geterror(sirchar_t message[SIR_MAXERROR]);

#ifdef SIR_SELFLOG
/** Log an internal message to \a stderr. */
void _sir_selflog(const sirchar_t* format, ...);
#else
#define _sir_selflog(format, ...) ((void)(0))
#endif

#endif /* !_SIR_ERRORS_H_INCLUDED */
