#ifndef _SIR_MUTEX_H_INCLUDED
#define _SIR_MUTEX_H_INCLUDED

#include "sirplatform.h"

sirmutex_t _sirmutex_create();
bool _sirmutex_trylock(sirmutex_t* mutex);
bool _sirmutex_lock(sirmutex_t* mutex);
void _sirmutex_unlock(sirmutex_t* mutex);
void _sirmutex_destroy(sirmutex_t* mutex);

#endif /* !_SIR_MUTEX_H_INCLUDED */
