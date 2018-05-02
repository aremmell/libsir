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

                return 0 == op;
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
        return 0 == op;
    }

    return false;
}

bool _sirmutex_lock(sirmutex_t* mutex) {

    assert(mutex);

    if (mutex) {
        int op = pthread_mutex_lock(mutex);
        _sir_handleerr(op);
        return 0 == op;
    }

    return false;
}

bool _sirmutex_unlock(sirmutex_t* mutex) {

    assert(mutex);

    if (mutex) {
        int op = pthread_mutex_unlock(mutex);
        _sir_handleerr(op);
        return 0 == op;
    }

    return false;    
}

bool _sirmutex_destroy(sirmutex_t* mutex) {
    assert(mutex);

    if (mutex) {
        int op = pthread_mutex_destroy(mutex);
        _sir_handleerr(op);
        return 0 == op;
    }

    return false;       
}

#else /* Win32 mutex implementation */

static bool _sirmutex_waitwin32(sirmutex_t mutex, DWORD msec);

bool _sirmutex_create(sirmutex_t* mutex) {

    assert(mutex);

    if (mutex) {
        sirmutex_t tmp = CreateMutex(NULL, FALSE, NULL);

        if (!tmp) {
            _sir_handleerr(GetLastError());
            return false;
        }

        *mutex = tmp;
        return true;
    }

    return false;
}

bool _sirmutex_trylock(sirmutex_t* mutex) {
    return _sirmutex_waitwin32(*mutex, 0);
}

bool _sirmutex_lock(sirmutex_t* mutex) {
    return _sirmutex_waitwin32(*mutex, INFINITE);
}

bool _sirmutex_unlock(sirmutex_t* mutex) {

    assert(mutex);

    if (mutex) {
        BOOL release = ReleaseMutex(*mutex);

        if (!release) 
            _sir_handleerr(GetLastError());

        return FALSE != release;
    }

    return false;
}

bool _sirmutex_destroy(sirmutex_t* mutex) {

    assert(mutex);

    if (mutex) {
        BOOL close = CloseHandle(*mutex);

        if (!close) 
            _sir_handleerr(GetLastError());

        return FALSE != close;
    }

    return false;
}

static bool _sirmutex_waitwin32(sirmutex_t mutex, DWORD msec) {

    assert(mutex);

    if (mutex) {
        DWORD wait = WaitForSingleObject(mutex, msec);

        switch(wait) {
            case WAIT_ABANDONED:
            case WAIT_FAILED:
                _sir_selflog("%s: warning: WaitForSingleObject returned 0x%08lx; danger ahead\n",
                    __func__, wait);
                return WAIT_FAILED != wait;
            break;
            case WAIT_TIMEOUT:
            case WAIT_OBJECT_0:
                return true;
        }
    }

    return false;
}

#endif /* !_WIN32 */

/*! \endcond */
