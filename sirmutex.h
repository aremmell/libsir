/*
 * sirmutex.h
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
#ifndef _SIR_MUTEX_H_INCLUDED
#define _SIR_MUTEX_H_INCLUDED

#include "sirtypes.h"

/** Creates/initializes a new mutex. */
bool _sirmutex_create(sir_mutex* mutex);

/** Determines if a mutex is locked without waiting. */
bool _sir_mutexrylock(sir_mutex* mutex);

/** Attempts to lock a mutex and waits indefinitely. */
bool _sirmutex_lock(sir_mutex* mutex);

/** Unlocks a previously locked mutex. */
bool _sirmutex_unlock(sir_mutex* mutex);

/** Destroys a mutex. */
bool _sirmutex_destroy(sir_mutex* mutex);

#endif /* !_SIR_MUTEX_H_INCLUDED */
