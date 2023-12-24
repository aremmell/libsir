/*
 * sirthreadpool.c
 *
 * Version: 2.2.5
 *
 * -----------------------------------------------------------------------------
 *
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2018-2024 Ryan M. Lederman <lederman@gmail.com>
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

#include "sir/condition.h"
#include "sir/threadpool.h"
#include "sir/internal.h"
#include "sir/queue.h"
#include "sir/mutex.h"

#if !defined(__WIN__)
static void* thread_pool_proc(void* arg);
#else
static unsigned __stdcall thread_pool_proc(void* arg);
#endif

bool _sir_threadpool_create(sir_threadpool** pool, size_t num_threads) {
    if (!pool || !num_threads || num_threads > SIR_THREADPOOL_MAX_THREADS)
        return _sir_seterror(_SIR_E_INVALID);

    *pool = calloc(1, sizeof(sir_threadpool));
    if (!*pool)
        return _sir_handleerr(errno);

    (*pool)->threads = calloc(num_threads, sizeof(sir_thread));
    if (!(*pool)->threads) {
        int err = errno;
        _sir_safefree(pool);
        return _sir_handleerr(err);
    }

    (*pool)->num_threads = num_threads;

    if (!_sir_queue_create(&(*pool)->jobs) || !_sir_condcreate(&(*pool)->cond) ||
        !_sir_mutexcreate(&(*pool)->mutex)) {
        bool destroy = _sir_threadpool_destroy(pool);
        SIR_ASSERT_UNUSED(destroy, destroy);
        return false;
    }

#if !defined(__WIN__)
    pthread_attr_t attr = {0};
    int op = pthread_attr_init(&attr);
    if (0 != op) {
        bool destroy = _sir_threadpool_destroy(pool);
        SIR_ASSERT_UNUSED(destroy, destroy);
        return _sir_handleerr(op);
    }
#endif

    int thrd_err     = 0;
    bool thrd_create = true;
    for (size_t n = 0; n < num_threads; n++) {
#if !defined(__WIN__)
        op = pthread_create(&(*pool)->threads[n], &attr, &thread_pool_proc, *pool);
        if (0 != op) {
            (*pool)->threads[n] = 0;
            thrd_err    = op;
            thrd_create = false;
            break;
        }
#else /* __WIN__ */
        (*pool)->threads[n] = (HANDLE)_beginthreadex(NULL, 0, &thread_pool_proc,
            *pool, 0, NULL);
        if (!(*pool)->threads[n]) {
            thrd_err    = errno;
            thrd_create = false;
            break;
        }
#endif
    }

#if !defined(__WIN__)
    op = pthread_attr_destroy(&attr);
    SIR_ASSERT_UNUSED(0 == op, op);
#endif

    if (!thrd_create) {
        bool destroy = _sir_threadpool_destroy(pool);
        SIR_ASSERT_UNUSED(destroy, destroy);
        return _sir_handleerr(thrd_err);
    }

    return !!*pool;
}

bool _sir_threadpool_add_job(sir_threadpool* pool, sir_threadpool_job* job) {
    bool retval = false;

    if (pool && pool->jobs && job && job->fn && job->data) {
        bool locked = _sir_mutexlock(&pool->mutex);
        SIR_ASSERT(locked);

        if (locked) {
            if (_sir_queue_push(pool->jobs, job)) {
                retval = _sir_condbroadcast(&pool->cond);
                _sir_selflog("added job; new size: %zu", _sir_queue_size(pool->jobs));
            }

            bool unlocked = _sir_mutexunlock(&pool->mutex);
            SIR_ASSERT_UNUSED(unlocked, unlocked);
        }
    }

    return retval;
}

bool _sir_threadpool_destroy(sir_threadpool** pool) {
    if (!pool || !*pool)
        return _sir_seterror(_SIR_E_INVALID);

    bool locked = _sir_mutexlock(&(*pool)->mutex);
    SIR_ASSERT(locked);

    if (locked) {
        _sir_selflog("broadcasting signal to condition var...");
        (*pool)->cancel = true;

        bool bcast = _sir_condbroadcast(&(*pool)->cond);
        SIR_ASSERT_UNUSED(bcast, bcast);

        bool unlock = _sir_mutexunlock(&(*pool)->mutex);
        SIR_ASSERT_UNUSED(unlock, unlock);
    }

    bool destroy = true;
    for (size_t n = 0; n < (*pool)->num_threads; n++) {
        SIR_ASSERT(0 != (*pool)->threads[n]);
        if (0 == (*pool)->threads[n])
            continue;
        _sir_selflog("joining thread %zu of %zu...", n + 1, (*pool)->num_threads);
#if !defined(__WIN__)
        int join = pthread_join((*pool)->threads[n], NULL);
        SIR_ASSERT(0 == join);
        _sir_eqland(destroy, 0 == join);
#else /* __WIN__ */
        DWORD join = WaitForSingleObject((*pool)->threads[n], INFINITE);
        SIR_ASSERT(WAIT_OBJECT_0 == join);
        _sir_eqland(destroy, WAIT_OBJECT_0 == join);
#endif
    }

    _sir_eqland(destroy, _sir_queue_destroy(&(*pool)->jobs));
    SIR_ASSERT(destroy);

    _sir_eqland(destroy, _sir_conddestroy(&(*pool)->cond));
    SIR_ASSERT(destroy);

    _sir_eqland(destroy, _sir_mutexdestroy(&(*pool)->mutex));
    SIR_ASSERT(destroy);

    _sir_safefree(&(*pool)->threads);
    _sir_safefree(pool);

    return destroy;
}

#if !defined(__WIN__)
static void* thread_pool_proc(void* arg)
#else
static unsigned __stdcall thread_pool_proc(void* arg)
#endif
{
    sir_threadpool* pool = (sir_threadpool*)arg;
    while (true) {
        bool locked = _sir_mutexlock(&pool->mutex);
        SIR_ASSERT_UNUSED(locked, locked);

        while (_sir_queue_isempty(pool->jobs) && !pool->cancel) {
#if !defined(__WIN__)
            /* seconds; absolute fixed time. */
            sir_wait wait = {time(NULL) + 2, 0};
#else
            /* msec; relative from now. */
            sir_wait wait = 2000;
#endif
            (void)_sir_condwait_timeout(&pool->cond, &pool->mutex, &wait);
        }

        if (!pool->cancel) {
            sir_threadpool_job* job = NULL;
            bool job_popped         = _sir_queue_pop(pool->jobs, (void**)&job);

            bool unlocked = _sir_mutexunlock(&pool->mutex);
            SIR_ASSERT_UNUSED(unlocked, unlocked);

            if (job_popped) {
                _sir_selflog("picked up job (fn: %"PRIxPTR", data: %p)",
                    (uintptr_t)job->fn, job->data);
                job->fn(job->data);
                _sir_safefree(&job);
            }
        } else {
            _sir_selflog("cancel flag is set; exiting");
            bool unlocked = _sir_mutexunlock(&pool->mutex);
            SIR_ASSERT_UNUSED(unlocked, unlocked);
            break;
        }
    }

#if !defined(__WIN__)
    return NULL;
#else /* __WIN__ */
    return 0U;
#endif
}
