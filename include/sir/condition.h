#ifndef _SIR_CONDITION_H_INCLUDED
# define _SIR_CONDITION_H_INCLUDED

# include "sir/types.h"

/** Creates/initializes a new condition variable. */
bool _sir_condcreate(sir_condition* cond);

/** Signals a condition variable. */
bool _sir_condsignal(sir_condition* cond);

/** Broadcast signals a condition variable. */
bool _sir_condbroadcast(sir_condition* cond);

/** Destroys a condition variable. */
bool _sir_conddestroy(sir_condition* cond);

/** Waits indefinitely for a condition variable to become signaled. */
bool _sir_condwait(sir_condition* cond, sir_mutex* mutex);

/** Waits a given amount of time for a condition variable to become signaled. */
bool _sir_condwait_timeout(sir_condition* cond, sir_mutex* mutex, sir_wait* how_long);

#endif /* !_SIR_CONDITION_H_INCLUDED */
