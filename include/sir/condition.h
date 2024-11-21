/**
 * @file condition.h
 *
 * @version 2.2.6
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

#ifndef _SIR_CONDITION_H_INCLUDED
# define _SIR_CONDITION_H_INCLUDED

# include "sir/types.h"

/**
 * Creates/initializes a new condition variable.
 *
 * @param cond Pointer to a sir_condition to initialize
 * @returns bool `true` if successful, `false` otherwise.
 */
bool _sir_condcreate(sir_condition* cond);

# if 0
/**
 * Signals a condition variable.
 *
 * @param cond Pointer to a sir_condition to signal.
 * @returns bool `true` if successful, `false` otherwise.
 */
bool _sir_condsignal(sir_condition* cond);
# endif

/**
 * Broadcast signals a condition variable.
 *
 * @param cond Pointer to a sir_condition to broadcast on.
 * @returns bool `true` if successful, `false` otherwise.
 */
bool _sir_condbroadcast(sir_condition* cond);

/**
 * Destroys a condition variable.
 *
 * @param cond Pointer to a sir_condition to destroy.
 * @returns bool `true` if successful, `false` otherwise.
 */
bool _sir_conddestroy(sir_condition* cond);

# if 0
/**
 * Waits indefinitely for a condition variable to become signaled.
 *
 * @param cond Pointer to a sir_condition to wait on.
 * @returns bool `true` if successful, `false` otherwise.
 */
bool _sir_condwait(sir_condition* cond, sir_mutex* mutex);
# endif

/**
 * Waits a given amount of time for a condition variable to become signaled.
 *
 * @param cond Pointer to a sir_condition to wait on.
 * @param mutex Associated mutex object.
 * @param howlong How long to wait before timing out.
 * @returns bool `true` if successful, `false` if an error occurred or the operation
 * timed out..
 */
bool _sir_condwait_timeout(sir_condition* cond, sir_mutex* mutex, const sir_wait* howlong);

#endif /* !_SIR_CONDITION_H_INCLUDED */
