/**
 * @file sircondition.c
 *
 * @author    Ryan M. Lederman \<lederman@gmail.com\>
 * @date      2018-2023
 * @version   2.2.2
 * @copyright The MIT License (MIT)
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
#include "sir/condition.h"
#include "sir/internal.h"
#include "sir/platform.h"

#if !defined(__WIN__) /* pthread implementation */
bool _sir_condcreate(sir_condition* cond) {
    bool valid = _sir_validptr(cond);

    if (valid) {
        int op = pthread_cond_init(cond, NULL);
        valid = 0 == op ? true : _sir_handleerr(op);
    }

    return valid;
}

# if 0
bool _sir_condsignal(sir_condition* cond) {
    bool valid = _sir_validptr(cond);

    if (valid) {
        int op = pthread_cond_signal(cond);
        valid = 0 == op ? true : _sir_handleerr(op);
    }

    return valid;
}
# endif

bool _sir_condbroadcast(sir_condition* cond) {
    bool valid = _sir_validptr(cond);

    if (valid) {
        int op = pthread_cond_broadcast(cond);
        valid = 0 == op ? true : _sir_handleerr(op);
    }

    return valid;
}

bool _sir_conddestroy(sir_condition* cond) {
    bool valid = _sir_validptr(cond);

    if (valid) {
        int op = pthread_cond_destroy(cond);
        valid = 0 == op ? true : _sir_handleerr(op);
    }

    return valid;
}

# if 0
bool _sir_condwait(sir_condition* cond, sir_mutex* mutex) {
    bool valid = _sir_validptr(cond) && _sir_validptr(mutex);

    if (valid) {
        int op = pthread_cond_wait(cond, mutex);
        valid = 0 == op ? true : _sir_handleerr(op);
    }

    return valid;
}
# endif

bool _sir_condwait_timeout(sir_condition* cond, sir_mutex* mutex, sir_wait* howlong) {
    bool valid = _sir_validptr(cond) && _sir_validptr(mutex) && _sir_validptr(howlong);

    if (valid) {
        int op = pthread_cond_timedwait(cond, mutex, howlong);
        valid = 0 == op ? true : ETIMEDOUT == op ? false : _sir_handleerr(op);
    }

    return valid;
}
#else /* __WIN__ */
bool _sir_condcreate(sir_condition* cond) {
    bool valid = _sir_validptr(cond);

    if (valid)
        InitializeConditionVariable(cond);

    return valid;
}

# if 0
bool _sir_condsignal(sir_condition* cond) {
    bool valid = _sir_validptr(cond);

    if (valid)
        WakeConditionVariable(cond);

    return valid;
}
# endif

bool _sir_condbroadcast(sir_condition* cond) {
    bool valid = _sir_validptr(cond);

    if (valid)
        WakeAllConditionVariable(cond);

    return valid;
}

bool _sir_conddestroy(sir_condition* cond) {
    SIR_UNUSED(cond);
    return true;
}

# if 0
bool _sir_condwait(sir_condition* cond, sir_mutex* mutex) {
    DWORD how_long = INFINITE;
    return _sir_condwait_timeout(cond, mutex, &how_long);
}
# endif

bool _sir_condwait_timeout(sir_condition* cond, sir_mutex* mutex, sir_wait* how_long) {
    bool valid = _sir_validptr(cond) && _sir_validptr(mutex) && _sir_validptr(how_long);

    if (valid)
        valid = (FALSE != SleepConditionVariableCS(cond, mutex, *how_long))
            ? true : _sir_handlewin32err(GetLastError());

    return valid;
}
#endif
