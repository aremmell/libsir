#include "sirmutex.h"

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
