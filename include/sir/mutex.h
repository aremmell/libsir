/*
 * mutex.h
 *
 * Version: 2.2.5
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

#ifndef _SIR_MUTEX_H_INCLUDED
# define _SIR_MUTEX_H_INCLUDED

# include "sir/types.h"

/** Creates/initializes a new mutex. */
bool _sir_mutexcreate(sir_mutex* mutex);

/** Attempts to lock a mutex and waits indefinitely. */
bool _sir_mutexlock(sir_mutex* mutex);

/** Determines if a mutex is locked without waiting. */
bool _sir_mutextrylock(sir_mutex* mutex);

/** Unlocks a previously locked mutex. */
bool _sir_mutexunlock(sir_mutex* mutex);

/** Destroys a mutex. */
bool _sir_mutexdestroy(sir_mutex* mutex);

#endif /* !_SIR_MUTEX_H_INCLUDED */
