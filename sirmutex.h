/**
 * @file sirmutex.h
 * @brief Cross-platform mutex.
 */
#ifndef _SIR_MUTEX_H_INCLUDED
#define _SIR_MUTEX_H_INCLUDED

#include "sirtypes.h"

/**
 * @addtogroup intern
 * @{
 */

/** Creates/initializes a new mutex. */
bool _sirmutex_create(sirmutex_t* mutex);

/** Determines if a mutex is locked without waiting. */
bool _sirmutex_trylock(sirmutex_t* mutex);

/** Attempts to lock a mutex and waits indefinitely. */
bool _sirmutex_lock(sirmutex_t* mutex);

/** Unlocks a previously locked mutex. */
bool _sirmutex_unlock(sirmutex_t* mutex);

/** Destroys a mutex. */
bool _sirmutex_destroy(sirmutex_t* mutex);

/** @} */

#endif /* !_SIR_MUTEX_H_INCLUDED */
