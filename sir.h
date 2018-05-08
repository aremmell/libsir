/**
 * @file sir.h
 * 
 * @brief Definitions for the public interface.
 * 
 * If you are a simple user of the library, the contents of this file as well as
 * the following should be sufficient for basic custom configuration:
 * 
 * - sirdefaults.h
 *   + Default text styling for each logging level and default formatting options for
 *   various types of destinations.
 * 
 * - sirtypes.h
 *   + Enums, structs, and other types used by SIR.
 * 
 * - sirconfig.h
 *   + Macros that control formatting, limitations and more.
 */
#ifndef _SIR_H_INCLUDED
#define _SIR_H_INCLUDED

#include "sirplatform.h"
#include "sirtypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* !__cplusplus */

/** 
 * @defgroup public Public Interface
 * 
 * Functions and types that comprise the public interface to the library.
 * 
 * @addtogroup public 
 * @{
 */

/**
 * @brief Initializes the library.
 * 
 * Any thread may initialize SIR, but any calls to other library functions
 * will fail until this call has completed.
 * 
 * @param si Initialization options.
 * 
 * @return boolean
 * @retval true SIR is initialized and ready to use.
 * @retval false Initialization failed.
 */
bool sir_init(sirinit* si);

/**
 * @brief Frees allocated resources and resets internal state.
 * 
 * It is not necessary to call this function from the same thread that
 * called ::sir_init. Any calls made after this to functions other than to
 * ::sir_init \a (in order to re-initialize) will fail.
 * 
 * @note Mutexes allocated during initialization are \a not destroyed during cleanup;
 * they are retained for potential further re-initialization.
 * 
 * @return boolean
 * @retval true SIR is cleaned up.
 * @retval false An error occurred.
 */
bool sir_cleanup();

/**
 * @brief Retrieves information about the last error that occurred within
 * the context of a library call.
 * 
 * @note Most C library and OS calls made by SIR are evaluated for failure.
 * If a failure of this type is encountered, this function returns ::_SIR_E_PLATFORM,
 * and \p message will contain a string identifying the underlying error code and
 * the message as reported by the platform.
 * 
 * @param message A buffer to which the human-readable error message is copied.
 * 
 * @return uint16_t
 * @retval SIR_E_NOERROR     0  The operation completed successfully
 * @retval SIR_E_NOTREADY    1  SIR has not been initialized
 * @retval SIR_E_ALREADY     2  SIR is already initialized 
 * @retval SIR_E_DUPFILE     3  File already managed by SIR
 * @retval SIR_E_NOFILE      4  File not managed by SIR
 * @retval SIR_E_FCFULL      5  Maximum number of files already managed
 * @retval SIR_E_OPTIONS     6  Option flags are invalid
 * @retval SIR_E_LEVELS      7  Level flags are invalid
 * @retval SIR_E_TEXTSTYLE   8  Text style is invalid
 * @retval SIR_E_STRING      9  Invalid string argument
 * @retval SIR_E_NODEST     10  No destinations registered for level
 * @retval SIR_E_PLATFORM   11  Platform error code %d: %s
 * @retval SIR_E_UNKNOWN  4095  Error is not known
 * 
 * @addtogroup errors
 */
uint16_t sir_geterror(sirchar_t message[SIR_MAXERROR]);

/**
 * @brief Log a formatted debug-level message.
 * 
 * This function maps internally to the logging level ::SIRL_DEBUG.
 * 
 * @param format A printf-style format string.
 * @param ... \a (variadic) Additional arguments whose types correspond to the format
 * specifier at the same index in \p format.
 * 
 * @return boolean
 * @retval true All destinations registered for this level were successfully processed.
 * @retval false One or more destinations were not successfully processed.
 */
bool sir_debug(const sirchar_t* format, ...);

/**
 * @brief Log a formatted informational message.
 * 
 * This function maps internally to the logging level ::SIRL_INFO.
 * 
 * @param format A printf-style format string.
 * @param ... \a (variadic) Additional arguments whose types correspond to the format
 * specifier at the same index in \p format.
 * 
 * @return boolean
 * @retval true All destinations registered for this level were successfully processed.
 * @retval false One or more destinations were not successfully processed.
 */
bool sir_info(const sirchar_t* format, ...);

/**
 * @brief Log a formatted notice message.
 * 
 * This function maps internally to the logging level ::SIRL_NOTICE.
 * 
 * @param format A printf-style format string.
 * @param ... \a (variadic) Additional arguments whose types correspond to the format
 * specifier at the same index in \p format.
 * 
 * @return boolean
 * @retval true All destinations registered for this level were successfully processed.
 * @retval false One or more destinations were not successfully processed.
 */
bool sir_notice(const sirchar_t* format, ...);

/**
 * @brief Log a formatted warning message.
 * 
 * This function maps internally to the logging level ::SIRL_WARN.
 * 
 * @param format A printf-style format string.
 * @param ... \a (variadic) Additional arguments whose types correspond to the format
 * specifier at the same index in \p format.
 * 
 * @return boolean
 * @retval true All destinations registered for this level were successfully processed.
 * @retval false One or more destinations were not successfully processed.
 */
bool sir_warn(const sirchar_t* format, ...);

/**
 * @brief Log a formatted error message.
 * 
 * This function maps internally to the logging level ::SIRL_ERROR.
 * 
 * @param format A printf-style format string.
 * @param ... \a (variadic) Additional arguments whose types correspond to the format
 * specifier at the same index in \p format.
 * 
 * @return boolean
 * @retval true All destinations registered for this level were successfully processed.
 * @retval false One or more destinations were not successfully processed.
 */
bool sir_error(const sirchar_t* format, ...);

/**
 * @brief Log a formatted critical error message.
 * 
 * This function maps internally to the logging level ::SIRL_CRIT.
 * 
 * @param format A printf-style format string.
 * @param ... \a (variadic) Additional arguments whose types correspond to the format
 * specifier at the same index in \p format.
 * 
 * @return boolean
 * @retval true All destinations registered for this level were successfully processed.
 * @retval false One or more destinations were not successfully processed.
 */
bool sir_crit(const sirchar_t* format, ...);

/**
 * @brief Log a formatted alert message.
 * 
 * This function maps internally to the logging level ::SIRL_ALERT.
 * 
 * @param format A printf-style format string.
 * @param ... \a (variadic) Additional arguments whose types correspond to the format
 * specifier at the same index in \p format.
 * 
 * @return boolean
 * @retval true All destinations registered for this level were successfully processed.
 * @retval false One or more destinations were not successfully processed.
 */
bool sir_alert(const sirchar_t* format, ...);

/**
 * @brief Log a formatted emergency message.
 * 
 * This function maps internally to the logging level ::SIRL_EMERG.
 * 
 * @param format A printf-style format string.
 * @param ... \a (variadic) Additional arguments whose types correspond to the format
 * specifier at the same index in \p format.
 * 
 * @return boolean
 * @retval true All destinations registered for this level were successfully processed.
 * @retval false One or more destinations were not successfully processed.
 */
bool sir_emerg(const sirchar_t* format, ...);

/**
 * @brief Add a log file to receive formatted output for one or more ::sir_level.
 * 
 * @param path The absolute or relative path to the file. If it does not exist, it will
 * be created.
 * @param levels One or more ::sir_level for which the file should be sent output. Use
 * ::SIRL_ALL to send all logging levels to the file.
 * @param opts Zero or more ::sir_option flags used to control output formatting.
 * 
 * @attention The sirfileid_t type is a pointer to the internally mapped
 * file descriptor for a file. If you modify the value at this address, the behavior
 * is undefined, and bad things will happen.
 * 
 * @return sirfileid_t
 * @retval The file descriptor for the file that was successfully added.
 * @retval NULL An error occurred while trying to add the file.
 */
sirfileid_t sir_addfile(const sirchar_t* path, sir_levels levels, sir_options opts);

/**
 * @brief Remove a previously added log file.
 * 
 * @param id The file descriptor returned from ::sir_addfile.
 * 
 * @return boolean
 * @retval true The file was removed and will no longer receive output.
 * @retval false An error occurred while trying to remove the file.
 */
bool sir_remfile(sirfileid_t id);

/**
 * @brief Sets the text style in \a stdio output for a ::sir_level of output.
 * 
 * @param level The level \a (only one is permitted per call) for which the style
 * will be set.
 * @param style Foreground color, background color, and brightness control flags
 * bitwise OR'd to create the text style for \p level. See ::sir_textstyle.
 * 
 * @return boolean
 * @retval true The style is valid and was applied.
 * @retval false An error occurred while trying to set the text style.
 */
bool sir_settextstyle(sir_level level, sir_textstyle style);

/** @} */

#ifdef __cplusplus
}
#endif /* !__cplusplus */

#endif /* !_SIR_H_INCLUDED */
