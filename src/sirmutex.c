/*
 * sirmutex.c
 *
 * Version: 2.2.6
 *
 * -----------------------------------------------------------------------------
 *
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2018-2024 Ryan M. Lederman <lederman@gmail.com>
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
 *
 * -----------------------------------------------------------------------------
 */

//-V::522
#include "sir/mutex.h"
#include "sir/internal.h"
#include "sir/platform.h"

#if !defined(__WIN__) /* pthread implementation */
bool _sir_mutexcreate(sir_mutex* mutex) {
    if (_sir_validptr(mutex)) {
        pthread_mutexattr_t attr;
        int op = pthread_mutexattr_init(&attr);
        if (0 == op) {
            op = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
            if (0 == op) {
                op = pthread_mutex_init(mutex, &attr);
                if (0 == op)
                    return true;
            }
        }

        (void)_sir_handleerr(op);
    }

    return false;
}

bool _sir_mutexlock(sir_mutex* mutex) {
    if (_sir_validptr(mutex)) {
        int op = pthread_mutex_lock(mutex);
        return 0 == op ? true : _sir_handleerr(op);
    }

    return false;
}

bool _sir_mutextrylock(sir_mutex* mutex) {
    if (_sir_validptr(mutex)) {
        int op = pthread_mutex_trylock(mutex);
        return 0 == op ? true : _sir_handleerr(op);
    }

    return false;
}

bool _sir_mutexunlock(sir_mutex* mutex) {
    if (_sir_validptr(mutex)) {
        int op = pthread_mutex_unlock(mutex);
        return 0 == op ? true : _sir_handleerr(op);
    }

    return false;
}

bool _sir_mutexdestroy(sir_mutex* mutex) {
    if (_sir_validptr(mutex)) {
        int op = pthread_mutex_destroy(mutex);
        return 0 == op ? true : _sir_handleerr(op);
    }

    return false;
}
#else /* __WIN__ */
bool _sir_mutexcreate(sir_mutex* mutex) {
    if (_sir_validptr(mutex)) {
        InitializeCriticalSection(mutex);
        return true;
    }

    return false;
}

bool _sir_mutexlock(sir_mutex* mutex) {
    if (_sir_validptr(mutex)) {
        EnterCriticalSection(mutex);
        return true;
    }

    return false;
}

bool _sir_mutextrylock(sir_mutex* mutex) {
    if (_sir_validptr(mutex))
        return FALSE != TryEnterCriticalSection(mutex);

    return false;
}

bool _sir_mutexunlock(sir_mutex* mutex) {
    if (_sir_validptr(mutex)) {
        LeaveCriticalSection(mutex);
        return true;
    }

    return false;
}

bool _sir_mutexdestroy(sir_mutex* mutex) {
    if (_sir_validptr(mutex)) {
        DeleteCriticalSection(mutex);
        return true;
    }

    return false;
}
#endif /* !__WIN__ */
