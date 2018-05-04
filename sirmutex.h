/**
 * @file sirmutex.h
 * @brief Cross-platform mutex.
 */
#ifndef _SIR_MUTEX_H_INCLUDED
#define _SIR_MUTEX_H_INCLUDED

#include "sirtypes.h"

bool _sirmutex_create(sirmutex_t* mutex);
bool _sirmutex_trylock(sirmutex_t* mutex);
bool _sirmutex_lock(sirmutex_t* mutex);
bool _sirmutex_unlock(sirmutex_t* mutex);
bool _sirmutex_destroy(sirmutex_t* mutex);

#endif /* !_SIR_MUTEX_H_INCLUDED */
