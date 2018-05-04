/**
 * @file sirinternal.h
 * @brief Internal definitions.
 */
#ifndef _SIR_INTERNAL_H_INCLUDED
#define _SIR_INTERNAL_H_INCLUDED

#include "sirmacros.h"
#include "sirtypes.h"

/** @cond PRIVATE */

bool _sir_sanity();
bool _sir_options_sanity(const sirinit* si);

bool  _sir_init(const sirinit* si);
void* _sir_locksection(sir_mutex_id mid);
bool  _sir_unlocksection(sir_mutex_id mid);
bool  _sir_mapmutexid(sir_mutex_id mid, sirmutex_t** m, void** section);
bool  _sir_cleanup();

#ifndef _WIN32
void _sir_initmutex_si_once();
void _sir_initmutex_fc_once();
void _sir_initmutex_ts_once();
#else
BOOL CALLBACK _sir_initmutex_si_once(PINIT_ONCE ponce, PVOID param, PVOID* ctx);
BOOL CALLBACK _sir_initmutex_fc_once(PINIT_ONCE ponce, PVOID param, PVOID* ctx);
BOOL CALLBACK _sir_initmutex_ts_once(PINIT_ONCE ponce, PVOID param, PVOID* ctx);
#endif

void _sir_initmutex(sirmutex_t* mutex);
void _sir_once(sironce_t* once, sir_once_fn func);

bool _sir_logv(sir_level level, const sirchar_t* format, va_list args);

bool             _sir_dispatch(sirinit* si, sir_level level, siroutput* output);
const sirchar_t* _sir_format(bool styling, sir_options opts, siroutput* output);

#ifndef SIR_NO_SYSLOG
int _sir_syslog_maplevel(sir_level level);
#endif

sirchar_t* _sirbuf_get(sirbuf* buf, size_t idx);

const sirchar_t* _sir_levelstr(sir_level level);
bool             _sir_destwantslevel(sir_levels destLevels, sir_level level);

bool _sir_getlocaltime(time_t* tbuf, long* nsecbuf);
bool _sir_formattime(time_t now, sirchar_t* buffer, const sirchar_t* format);

pid_t _sir_getpid();
pid_t _sir_gettid();

#ifdef SIR_SELFLOG
#define _sir_handleerr(err) _sir_handleerr_impl(err, __func__, __FILE__, __LINE__);

void _sir_handleerr_impl(sirerror_t err, const sirchar_t* func, const sirchar_t* file, uint32_t line);
void _sir_selflog(const sirchar_t* format, ...);
#else
#define _sir_selflog(format, ...) ((void)(0))
#define _sir_handleerr(err) ((void)(0))
#endif

/** @endcond private */

#endif /* !_SIR_INTERNAL_H_INCLUDED */
