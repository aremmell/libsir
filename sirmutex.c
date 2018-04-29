/*!
 * \file sirmutex.c
 * \brief Internal implementation of the cross-platform mutex used by the SIR library.
 * \author Ryan Matthew Lederman <lederman@gmail.com>
 */
#include "sirmutex.h"
#include "sirplatform.h"
#include "sirinternal.h"

/*! \cond PRIVATE */

#ifndef _WIN32 /* pthread mutex implementation */

bool _sirmutex_create(sirmutex_t* mutex) {

    assert(mutex);

    if (mutex) {
        pthread_mutexattr_t attr;

        int op = pthread_mutexattr_init(&attr);
        _sir_handleerr(op);
        
        if (0 == op) {
            op = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
            _sir_handleerr(op);

            if (0 == op) {
                op = pthread_mutex_init(mutex, &attr);
                _sir_handleerr(op);

                return 0 != op;
            }
        }
    }

    return false;
}

bool _sirmutex_trylock(sirmutex_t* mutex) {

    assert(mutex);

    if (mutex) {
        int op = pthread_mutex_trylock(mutex);
        _sir_handleerr(op);
        return 0 != op;
    }

    return false;
}

bool _sirmutex_lock(sirmutex_t* mutex) {

    assert(mutex);

    if (mutex) {
        int op = pthread_mutex_lock(mutex);
        _sir_handleerr(op);
        return 0 != op;
    }

    return false;
}

bool _sirmutex_unlock(sirmutex_t* mutex) {

    assert(mutex);

    if (mutex) {
        int op = pthread_mutex_unlock(mutex);
        _sir_handleerr(op);
        return 0 != op;
    }

    return false;    
}

bool _sirmutex_destroy(sirmutex_t* mutex) {
    assert(mutex);

    if (mutex) {
        int op = pthread_mutex_destroy(mutex);
        _sir_handleerr(op);
        return 0 != op;
    }

    return false;       
}

#else /* Win32 mutex implementation */

bool _sirmutex_create(sirmutex_t* mutex) {
    return false;
}

bool _sirmutex_trylock(sirmutex_t* mutex) {
    return false;
}

bool _sirmutex_lock(sirmutex_t* mutex) {
    return false;
}

bool _sirmutex_unlock(sirmutex_t* mutex) {
    return false;
}

bool _sirmutex_destroy(sirmutex_t* mutex) {
    return false;
}
#endif /* !_WIN32 */

/*! \endcond */
