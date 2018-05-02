/*!
 * \file sirinternal.h
 * \brief Core internal definitions for the SIR library.
 * \author Ryan Matthew Lederman <lederman@gmail.com>
 */
#ifndef _SIR_INTERNAL_H_INCLUDED
#define _SIR_INTERNAL_H_INCLUDED

#include "sirplatform.h"
#include "sirtypes.h"
#include "sirmacros.h"

/*! \cond PRIVATE */

#define SIR_MAXERROR 256
#define SIR_UNKERROR "<unknown>"

extern sirinit  sir_s;
extern sirfcache sir_fc;
extern sirbuf   sir_b;
volatile extern uint32_t sir_magic;

bool _sir_sanity();
bool _sir_logv(sir_level level, const sirchar_t* format, va_list args);

bool             _sir_dispatch(sir_level level, siroutput* output);
const sirchar_t* _sir_format(bool styling, sir_options opts, siroutput* output);

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

#ifdef SIR_SELFLOG
#define _sir_handleerr(err) \
    _sir_handleerr_impl(err, __func__, __FILE__, __LINE__);

void _sir_handleerr_impl(sirerror_t err, const sirchar_t* func,
    const sirchar_t* file, uint32_t line);    
void _sir_selflog(const sirchar_t* format, ...);
#else
#define _sir_selflog(format, ...) ((void)(0))
#define _sir_handleerr(err) ((void)(0))
#endif

/*! \endcond */

#endif /* !_SIR_INTERNAL_H_INCLUDED */
