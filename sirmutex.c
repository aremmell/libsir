/*!
 * \file sirmutex.c
 *
 * Internal implementation of platform-dependent mutex used by the SIR library.
 *
 * \author Ryan Matthew Lederman <lederman@gmail.com>
 */
#include "sirmutex.h"
#include "sirplatform.h"

/*! \cond PRIVATE */

bool _sirmutex_trylock(sirmutex_t* mutex) {
    return false;
}

bool _sirmutex_lock(sirmutex_t* mutex) {
    return false;
}

void _sirmutex_unlock(sirmutex_t* mutex) {
}

void _sirmutex_destroy(sirmutex_t* mutex) {
}

/*! \endcond */
