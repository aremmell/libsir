#include "sir/condition.h"
#include "sir/internal.h"
#include "sir/platform.h"

#if !defined(__WIN__) /* pthread implementation */
bool _sir_condcreate(sir_condition* cond) {
    bool valid = _sir_validptr(cond);
    int op     = EINVAL;

    if (valid)
        op = pthread_cond_init(cond, NULL);

    return valid ? (0 == op ? true : _sir_handleerr(op)) : false;
}

#if 0
bool _sir_condsignal(sir_condition* cond) {
    bool valid = _sir_validptr(cond);
    int op     = -1;

    if (valid)
        op = pthread_cond_signal(cond);

    return valid ? (0 == op ? true : _sir_handleerr(op)) : false;
}
#endif

bool _sir_condbroadcast(sir_condition* cond) {
    bool valid = _sir_validptr(cond);
    int op     = EINVAL;

    if (valid)
        op = pthread_cond_broadcast(cond);

    return valid ? (0 == op ? true : _sir_handleerr(op)) : false;
}

bool _sir_conddestroy(sir_condition* cond) {
    bool valid = _sir_validptr(cond);
    int op     = EINVAL;

    if (valid)
        op = pthread_cond_destroy(cond);

    return valid ? (0 == op ? true : _sir_handleerr(op)) : false;
}

#if 0
bool _sir_condwait(sir_condition* cond, sir_mutex* mutex) {
    bool valid = _sir_validptr(cond) && _sir_validptr(mutex);
    int op     = EINVAL;

    if (valid)
        op = pthread_cond_wait(cond, mutex);

    return valid ? (0 == op ? true : _sir_handleerr(op)) : false;
}
#endif

bool _sir_condwait_timeout(sir_condition* cond, sir_mutex* mutex, sir_wait* how_long) {
    bool valid = _sir_validptr(cond) && _sir_validptr(mutex) && _sir_validptr(how_long);
    int op     = EINVAL;

    if (valid)
        op = pthread_cond_timedwait(cond, mutex, how_long);

    return (valid ? ((0 == op ? true : ETIMEDOUT == op) ? false : _sir_handleerr(op)) : false);
}
#else /* __WIN__ */
bool _sir_condcreate(sir_condition* cond) {
    bool valid = _sir_validptr(cond);

    if (valid)
        InitializeConditionVariable(cond);

    return valid;
}

#if 0
bool _sir_condsignal(sir_condition* cond) {
    bool valid = _sir_validptr(cond);

    if (valid)
        WakeConditionVariable(cond);

    return valid;
}
#endif

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

#if 0
bool _sir_condwait(sir_condition* cond, sir_mutex* mutex) {
    DWORD how_long = INFINITE;
    return _sir_condwait_timeout(cond, mutex, &how_long);
}
#endif

bool _sir_condwait_timeout(sir_condition* cond, sir_mutex* mutex, sir_wait* how_long) {
    bool valid = _sir_validptr(cond) && _sir_validptr(mutex) && _sir_validptr(how_long);
    return (valid ? ((FALSE != SleepConditionVariableCS(cond, mutex, *how_long)) ? true
        : _sir_handlewin32err(GetLastError())) : false);
}
#endif
