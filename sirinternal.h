/*!
 * \file sirinternal.h
 *
 * Core internal definitions for the SIR library.
 *
 * \author Ryan Matthew Lederman <lederman@gmail.com>
 */
#ifndef _SIR_INTERNAL_H_INCLUDED
#define _SIR_INTERNAL_H_INCLUDED

#include "sirplatform.h"
#include "sirtypes.h"

/*! \cond PRIVATE */

extern sirinit  sir_s;
extern sirfiles sir_fc;
extern sirbuf   sir_b;

bool _sir_lv(sir_level level, const sirchar_t* format, va_list args);
void _sir_l(const sirchar_t* format, ...);

bool             _sir_dispatch(sir_level level, siroutput* output);
const sirchar_t* _sir_format(sir_options, siroutput* output);
bool             _sir_stderr_write(const sirchar_t* message);
bool             _sir_stdout_write(const sirchar_t* message);
#ifndef SIR_NO_SYSLOG
int _sir_syslog_maplevel(sir_level level);
#endif

void       _sirbuf_reset(sirbuf* buf);
sirchar_t* _sirbuf_get(sirbuf* buf, size_t idx);

bool _sir_options_sanity(const sirinit* si);

const sirchar_t* _sir_levelstr(sir_level level);
bool             _sir_destwantslevel(sir_levels destLevels, sir_level level);

bool  _sir_getlocaltime(time_t* tbuf, long* nsecbuf);
bool  _sir_formattime(time_t now, sirchar_t* buffer, const sirchar_t* format);

pid_t _sir_getpid();
pid_t _sir_gettid();

#ifdef _WIN32
void _sir_invalidparam(const wchar_t* expression, const wchar_t* function, const wchar_t* file,
    unsigned int line, uintptr_t pReserved);
#endif

/*! \endcond */

#endif /* !_SIR_INTERNAL_H_INCLUDED */
