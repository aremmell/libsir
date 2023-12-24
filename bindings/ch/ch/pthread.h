/*
 * ch/pthread.h
 *
 * Version: 2.2.5
 *
 * -----------------------------------------------------------------------------
 *
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2018-2024 Jeffrey H. Johnson <trnsz@pobox.com>
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

#ifndef _CH_PTHREAD_H_
# define _CH_PTHREAD_H_

# ifndef ENOTSUP
#  define ENOTSUP EOPNOTSUPP
# endif /* ifndef ENOTSUP */

typedef unsigned long int pthread_t;

typedef struct _pthread_descr_struct *_pthread_descr;

struct _pthread_queue {
    _pthread_descr head;
    _pthread_descr tail;
};

typedef struct {
    int            m_spinlock;
    int            m_count;
    _pthread_descr m_owner;
    int            m_kind;
    struct         _pthread_queue m_waiting;
} pthread_mutex_t;

# define PTHREAD_MUTEX_INITIALIZER \
    { 0, 0, 0, PTHREAD_MUTEX_FAST_NP, { 0, 0 } }

# define PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP \
    { 0, 0, 0, PTHREAD_MUTEX_RECURSIVE_NP, { 0, 0 } }

typedef struct {
    int    c_spinlock;
    struct _pthread_queue c_waiting;
} pthread_cond_t;

# define PTHREAD_COND_INITIALIZER \
    { 0, { 0, 0 } }

enum {
    PTHREAD_CREATE_JOINABLE,
    PTHREAD_CREATE_DETACHED
};

enum {
    PTHREAD_INHERIT_SCHED,
    PTHREAD_EXPLICIT_SCHED
};

enum {
    PTHREAD_SCOPE_SYSTEM,
    PTHREAD_SCOPE_PROCESS
};

typedef struct {
    int    detachstate;
    int    schedpolicy;
    struct sched_param schedparam;
    int    inheritsched;
    int    scope;
} pthread_attr_t;

enum {
    PTHREAD_MUTEX_FAST_NP,
    PTHREAD_MUTEX_RECURSIVE_NP,
    PTHREAD_MUTEX_ERRORCHECK_NP
};

typedef struct {
    int mutexkind;
} pthread_mutexattr_t;

typedef struct {
    int dummy;
} pthread_condattr_t;

typedef unsigned int pthread_key_t;

typedef int pthread_once_t;

# define PTHREAD_ONCE_INIT 0

enum {
    PTHREAD_CANCEL_ENABLE,
    PTHREAD_CANCEL_DISABLE
};

enum {
    PTHREAD_CANCEL_DEFERRED,
    PTHREAD_CANCEL_ASYNCHRONOUS
};

# define PTHREAD_CANCELED ((void *)-1)

#endif /* _CH_PTHREAD_H_ */
