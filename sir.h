/*!
 * \file sir.h
 */
#ifndef _SIR_H_INCLUDED
#define _SIR_H_INCLUDED

#include "sirplatform.h"
#include "sirtypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* !__cplusplus */

/*! \fn int sir_init(const sirinit *si)
 */
bool sir_init(const sirinit* si);

/*! \fn bool sir_cleanup()
 * \return boolean success
 */
bool sir_cleanup();

/*! \fn bool sir_debug(const sirchar_t *format, ...)
 */
bool sir_debug(const sirchar_t* format, ...);

/*! \fn bool sir_info(const sirchar_t *format, ...)
 */
bool sir_info(const sirchar_t* format, ...);

/*! \fn bool sir_notice(const sirchar_t *format, ...)
 */
bool sir_notice(const sirchar_t* format, ...);

/*! \fn bool sir_warn(const sirchar_t *format, ...)
 */
bool sir_warn(const sirchar_t* format, ...);

/*! \fn bool sir_error(const sirchar_t *format, ...)
 */
bool sir_error(const sirchar_t* format, ...);

/*! \fn bool sir_crit(const sirchar_t *format, ...)
 */
bool sir_crit(const sirchar_t* format, ...);

/*! \fn bool sir_alert(const sirchar_t *format, ...)
 */
bool sir_alert(const sirchar_t* format, ...);

/*! \fn bool sir_emerg(const sirchar_t *format, ...)
 */
bool sir_emerg(const sirchar_t* format, ...);

/*! \fn int sir_addfile(const sirchar_t* path, sir_levels levels, sir_options opts)
 */
int sir_addfile(const sirchar_t* path, sir_levels levels, sir_options opts);

/*! \fn bool sir_remfile(int id)
 */
bool sir_remfile(int id);

/*! \fn bool sir_settextstyle(sir_level level, sir_textstyle style)
 */
bool sir_settextstyle(sir_level level, sir_textstyle style);

#ifdef __cplusplus
}
#endif /* !__cplusplus */

#endif /* !_SIR_H_INCLUDED */
