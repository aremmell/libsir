#include "sir/condition.h"
#include "sir/internal.h"
#include "sir/platform.h"

#if !defined(__WIN__) /* pthread implementation */
bool _sir_condcreate(sir_condition* cond) {
    if (_sir_validptr(cond)) {
        int op = pthread_cond_init(cond, NULL);
        return 0 == op ? true : _sir_handleerr(op);
    }

    return false;
}

# if 0
bool _sir_condsignal(sir_condition* cond) {
    if (_sir_validptr(cond)) {
        int op = pthread_cond_signal(cond);
        return 0 == op ? true : _sir_handleerr(op);
    }

    return false;
}
# endif

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

# if 0
bool _sir_condwait(sir_condition* cond, sir_mutex* mutex) {
    if (_sir_validptr(cond) && _sir_validptr(mutex)) {
        int op = pthread_cond_wait(cond, mutex);
        return 0 == op ? true : _sir_handleerr(op);
    }

    return false;
}
# endif

bool _sir_condwait_timeout(sir_condition* cond, sir_mutex* mutex, sir_wait* howlong) {
    if (_sir_validptr(cond) && _sir_validptr(mutex) && _sir_validptr(howlong)) {
        int op = pthread_cond_timedwait(cond, mutex, howlong);
        return 0 == op ? true : ETIMEDOUT == op ? false : _sir_handleerr(op);
    }

    return false;
}
#else /* __WIN__ */
bool _sir_condcreate(sir_condition* cond) {
    if (_sir_validptr(cond)) {
        InitializeConditionVariable(cond);
        return true;
    }

    return false;
}

# if 0
bool _sir_condsignal(sir_condition* cond) {
    if (_sir_validptr(cond)) {
        WakeConditionVariable(cond);
        return true;
    }

    return false;
}
# endif

bool _sir_condbroadcast(sir_condition* cond) {
    if (_sir_validptr(cond)) {
        WakeAllConditionVariable(cond);
        return true;
    }

    return false;
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
    if (_sir_validptr(cond) && _sir_validptr(mutex) && _sir_validptr(how_long))
        return (FALSE != SleepConditionVariableCS(cond, mutex, *how_long)) ? true
            : _sir_handlewin32err(GetLastError());
    return false;
}
#endif
