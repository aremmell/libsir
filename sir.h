/**
 * @file sir.h
 * @brief Public interface to libsir
 *
 * The functions and types defined here comprise the entire set intended for
 * use by an implementor of the library-unless modification is desired.
 * 
 * @author    Ryan M. Lederman \<lederman@gmail.com\>
 * @date      2018-2023
 * @version   2.2.0
 * @copyright The MIT License (MIT)
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
#ifndef _SIR_H_INCLUDED
#define _SIR_H_INCLUDED

#include "sirplatform.h"
#include "sirtypes.h"

#if defined(__cplusplus)
extern "C" {
#endif /* !__cplusplus */

/**
 * @defgroup public Public Interface
 *
 * Functions and types that comprise the public interface to libsir.
 *
 * @addtogroup public
 * @{
 */

bool sir_makeinit(sirinit* si);
bool sir_init(sirinit* si);
bool sir_cleanup(void);

uint16_t sir_geterror(sirchar_t message[SIR_MAXERROR]);

bool sir_debug(const sirchar_t* format, ...);
bool sir_info(const sirchar_t* format, ...);
bool sir_notice(const sirchar_t* format, ...);
bool sir_warn(const sirchar_t* format, ...);
bool sir_error(const sirchar_t* format, ...);
bool sir_crit(const sirchar_t* format, ...);
bool sir_alert(const sirchar_t* format, ...);
bool sir_emerg(const sirchar_t* format, ...);

sirfileid_t sir_addfile(const sirchar_t* path, sir_levels levels, sir_options opts);
bool sir_remfile(sirfileid_t id);

bool sir_filelevels(sirfileid_t id, sir_levels levels);
bool sir_fileopts(sirfileid_t id, sir_options opts);

bool sir_settextstyle(sir_level level, sir_textstyle style);
bool sir_resettextstyles(void);

bool sir_stdoutlevels(sir_levels levels);
bool sir_stdoutopts(sir_options opts);

bool sir_stderrlevels(sir_levels levels);
bool sir_stderropts(sir_options opts);

bool sir_sysloglevels(sir_levels levels);
bool sir_syslogopts(sir_options opts);
bool sir_syslogid(const char* identity);
bool sir_syslogcat(const char* category);

/** @} */

#if defined(__cplusplus)
}
#endif /* !__cplusplus */

#endif /* !_SIR_H_INCLUDED */
