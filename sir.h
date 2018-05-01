/*!
 * \file sir.h
 * \brief Definition of the public interface to the SIR library.
 * \author Ryan Matthew Lederman <lederman@gmail.com>
 */
#ifndef _SIR_H_INCLUDED
#define _SIR_H_INCLUDED

#include "sirplatform.h"
#include "sirtypes.h"

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
 * \return boolean success indicating whether or not all destinations
 * meant to receive this level were successfully processed.
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
 * \return boolean success indicating whether or not all destinations
 * meant to receive this level were successfully processed.
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
 * \return boolean success indicating whether or not all destinations
 * meant to receive this level were successfully processed.
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
 * \return boolean success indicating whether or not all destinations
 * meant to receive this level were successfully processed.
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
 * \return boolean success indicating whether or not all destinations
 * meant to receive this level were successfully processed.
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
 * \return boolean success indicating whether or not all destinations
 * meant to receive this level were successfully processed.
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
 * \return boolean success indicating whether or not all destinations
 * meant to receive this level were successfully processed.
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
 * \return boolean success indicating whether or not all destinations
 * meant to receive this level were successfully processed.
 */
bool siremerg(const sirchar_t* format, ...);

/*! \fn int sir_addfile(const sirchar_t* path, sir_levels levels, sir_options opts)
 *
 * \brief Adds a log file output destination.
 *
 * Adds a log file to be appended to whenever a message
 * matching any of the ::sir_level flags set in \a levels is emitted.
 *
 * \param[in] path The path to the log file. It will be created if it doesn't
 * exist.
 *
 * \param[in] levels One or more ::sir_level logging levels for which
 * output should be append to the file.
 *
 * \param[in] opts Zero or more ::sir_option flags to control output formatting.
 *
 * \return A unique identifier for the file, or ::SIR_INVALID (-1) if an error occurs.
 */
int sir_addfile(const sirchar_t* path, sir_levels levels, sir_options opts);

/*! \fn bool sir_remfile(int id)
 *
 * \brief Removes a previously added log file.
 *
 * \param[in] id The identifier returned from ::sir_addfile.
 *
 * \return boolean success
 */
bool sir_remfile(int id);

bool sir_settextstyle(sir_level level, sir_textstyle style);

#ifdef __cplusplus
}
#endif /* !__cplusplus */

#endif /* !_SIR_H_INCLUDED */
