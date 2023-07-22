/*
 * sirmutex.c
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2023
 * Version:   2.2.1
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
#include "sir/mutex.h"
#include "sir/internal.h"
#include "sir/platform.h"

#if !defined(__WIN__) /* pthread mutex implementation */
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

bool _sir_condcreate(sir_condition* cond) {
    if (_sir_validptr(cond)) {
        int op = pthread_cond_init(cond, NULL);
        return 0 == op ? true : _sir_handleerr(op);
    }

    return false;
}

bool _sir_condsignal(sir_condition* cond) {
    if (_sir_validptr(cond)) {
        int op = pthread_cond_signal(cond);
        return 0 == op ? true : _sir_handleerr(op);
    }

    return false;
}

bool _sir_condbroadcast(sir_condition* cond) {
    if (_sir_validptr(cond)) {
        int op = pthread_cond_broadcast(cond);
        return 0 == op ? true : _sir_handleerr(op);
    }

    return false;
}

bool _sir_conddestroy(sir_condition* cond) {
    if (_sir_validptr(cond)) {
        int op = pthread_cond_destroy(cond);
        return 0 == op ? true : _sir_handleerr(op);
    }

    return false;
}

bool _sir_condwait(sir_condition* cond, sir_mutex* mutex) {
    if (_sir_validptr(cond) && _sir_validptr(mutex)) {
        int op = pthread_cond_wait(cond, mutex);
        return 0 == op ? true : _sir_handleerr(op);
    }

    return false;
}

bool _sir_condwait_timeout(sir_condition* cond, sir_mutex* mutex, sir_wait* howlong) {
    if (_sir_validptr(cond) && _sir_validptr(mutex) && _sir_validptr(howlong)) {
        int op = pthread_cond_timedwait(cond, mutex, howlong);
        return 0 == op ? true : _sir_handleerr(op);
    }

    return false;
}
#else /* __WIN__ */
static
bool _sir_waitwin32(HANDLE handle, DWORD msec) {
    if (_sir_validptr(handle)) {
        DWORD wait = WaitForSingleObject(handle, msec);
        switch (wait) {
            case WAIT_ABANDONED:
            case WAIT_FAILED:
                _sir_selflog("warning: WaitForSingleObject returned %08x; danger ahead", wait);
                return WAIT_FAILED != wait;
                break;
            case WAIT_TIMEOUT:
            case WAIT_OBJECT_0: return true;
            default: return false; // GCOVR_EXCL_LINE
        }
    }

    return false;
}

bool _sir_mutexcreate(sir_mutex* mutex) {
    if (_sir_validptr(mutex)) {
        sir_mutex tmp = CreateMutex(NULL, FALSE, NULL);
        if (!tmp)
            return _sir_handlewin32err(GetLastError());

        *mutex = tmp;
        return true;
    }

    return false;
}

bool _sir_mutexlock(sir_mutex* mutex) {
    return _sir_validptr(mutex) ? _sir_waitwin32(*mutex, INFINITE) : false;
}

bool _sir_mutextrylock(sir_mutex* mutex) {
    return _sir_validptr(mutex) ? _sir_waitwin32(*mutex, 0) : false;
}

bool _sir_mutexunlock(sir_mutex* mutex) {
    if (_sir_validptr(mutex))
        return (FALSE != ReleaseMutex(*mutex)) ? true : _sir_handlewin32err(GetLastError());
    return false;
}

bool _sir_mutexdestroy(sir_mutex* mutex) {
    if (_sir_validptr(mutex))
        return (FALSE != CloseHandle(*mutex)) ? true : _sir_handlewin32err(GetLastError());
    return false;
}

bool _sir_condcreate(sir_condition* cond) {
    if (_sir_validptr(cond)) {
        sir_mutex tmp = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (!tmp)
            return _sir_handlewin32err(GetLastError());

        *cond = tmp;
        return true;
    }

    return false;
}

bool _sir_condsignal(sir_condition* cond) {
    if (_sir_validptr(cond))
        return (FALSE != SetEvent(*cond)) ? true : _sir_handlewin32err(GetLastError());
    return false;
}

bool _sir_condbroadcast(sir_condition* cond) {
    return _sir_condsignal(cond);
}

bool _sir_conddestroy(sir_condition* cond) {
    if (_sir_validptr(cond)) {
        return (FALSE != CloseHandle(*cond)) ? true : _sir_handlewin32err(GetLastError());
    return false;
}

bool _sir_condwait(sir_condition* cond, sir_mutex* mutex) {
    _SIR_UNUSED(mutex);

    if (_sir_validptr(cond)) {
        DWORD wait = WaitForSingleObject(*cond, INFINITE);
        return WAIT_OBJECT_0 == wait ? true : _sir_handlewin32err(GetLastError());
    }

    return false;
}

bool _sir_condwait_timeout(sir_condition* cond, sir_mutex* mutex, sir_wait* howlong) {
    _SIR_UNUSED(mutex);

    if (_sir_validptr(cond) && _sir_validptr(howlong))
        return _sir_waitwin32(*cond, *howlong);

    return false;
}
#endif /* !__WIN__ */
