/*
 * sirmutex.c
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2023
 * Version:   2.2.0
 * License:   The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "sirmutex.h"
#include "sirinternal.h"
#include "sirplatform.h"

#if !defined(__WIN__) /* pthread mutex implementation */

bool _sirmutex_create(sir_mutex* mutex) {
    if (_sir_validptr(mutex)) {
        pthread_mutexattr_t attr;

        int op = pthread_mutexattr_init(&attr);
        if (0 != op)
            _sir_handleerr(op);

        if (0 == op) {
            op = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
            if (0 != op)
                _sir_handleerr(op);

            if (0 == op) {
                op = pthread_mutex_init(mutex, &attr);
                if (0 != op)
                    _sir_handleerr(op);
                return 0 == op;
            }
        }
    }

    return false;
}

bool _sir_mutexrylock(sir_mutex* mutex) {
    if (_sir_validptr(mutex)) {
        int op = pthread_mutex_trylock(mutex);
        if (0 != op)
            _sir_handleerr(op);
        return 0 == op;
    }

    return false;
}

bool _sirmutex_lock(sir_mutex* mutex) {
    if (_sir_validptr(mutex)) {
        int op = pthread_mutex_lock(mutex);
        if (0 != op)
            _sir_handleerr(op);
        return 0 == op;
    }

    return false;
}

bool _sirmutex_unlock(sir_mutex* mutex) {
    if (_sir_validptr(mutex)) {
        int op = pthread_mutex_unlock(mutex);
        if (0 != op)
            _sir_handleerr(op);
        return 0 == op;
    }

    return false;
}

bool _sirmutex_destroy(sir_mutex* mutex) {
    if (_sir_validptr(mutex)) {
        int op = pthread_mutex_destroy(mutex);
        if (0 != op)
            _sir_handleerr(op);
        return 0 == op;
    }

    return false;
}

#else /* __WIN__ */

static bool _sirmutex_waitwin32(sir_mutex mutex, DWORD msec);

bool _sirmutex_create(sir_mutex* mutex) {
    if (_sir_validptr(mutex)) {
        sir_mutex tmp = CreateMutex(NULL, FALSE, NULL);

        if (!tmp) {
            _sir_handlewin32err(GetLastError());
            return false;
        }

        *mutex = tmp;
        return true;
    }

    return false;
}

bool _sir_mutexrylock(sir_mutex* mutex) {
    return _sirmutex_waitwin32(*mutex, 0);
}

bool _sirmutex_lock(sir_mutex* mutex) {
    return _sirmutex_waitwin32(*mutex, INFINITE);
}

bool _sirmutex_unlock(sir_mutex* mutex) {
    if (_sir_validptr(mutex)) {
        BOOL release = ReleaseMutex(*mutex);

        if (!release)
            _sir_handlewin32err(GetLastError());

        return FALSE != release;
    }

    return false;
}

bool _sirmutex_destroy(sir_mutex* mutex) {
    if (_sir_validptr(mutex)) {
        BOOL close = CloseHandle(*mutex);

        if (!close)
            _sir_handlewin32err(GetLastError());

        return FALSE != close;
    }

    return false;
}

static bool _sirmutex_waitwin32(sir_mutex mutex, DWORD msec) {
    if (_sir_validptr(mutex)) {
        DWORD wait = WaitForSingleObject(mutex, msec);

        switch (wait) {
            case WAIT_ABANDONED:
            case WAIT_FAILED:
                _sir_selflog("warning: WaitForSingleObject returned %08x; danger ahead", wait);
                return WAIT_FAILED != wait;
                break;
            case WAIT_TIMEOUT:
            case WAIT_OBJECT_0: return true;
        }
    }

    return false;
}

#endif // !__WIN__
