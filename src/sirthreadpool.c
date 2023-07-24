/*
 * sirthreadpool.c
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
#include "sir/condition.h"
#include "sir/threadpool.h"
#include "sir/internal.h"
#include "sir/queue.h"
#include "sir/mutex.h"

#if !defined(__WIN__)
static void* thread_pool_proc(void* arg);
#else
static unsigned thread_pool_proc(void* arg);
#endif

bool _sir_threadpool_create(sir_threadpool** pool, size_t num_threads) {
    if (!pool || !num_threads)
        return false;

    *pool = calloc(1, sizeof(sir_threadpool));
    if (!*pool) {
        _sir_handleerr(errno);
        return false;
    }

    (*pool)->threads = calloc(num_threads, sizeof(sir_thread));
    if (!(*pool)->threads) {
        _sir_handleerr(errno);
        _sir_threadpool_destroy(pool);
        return false;
    }

    (*pool)->num_threads = num_threads;

    if (!_sir_queue_create(&(*pool)->jobs) || !_sir_condcreate(&(*pool)->cond) ||
        !_sir_mutexcreate(&(*pool)->mutex)) {
            _sir_threadpool_destroy(pool);
            return false;
    }

    for (size_t n = 0; n < num_threads; n++) {
#if !defined(__WIN__)
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        int op = pthread_create(&(*pool)->threads[n], &attr, &thread_pool_proc, *pool);
        if (0 != op) {
            (*pool)->threads[n] = 0;
            _sir_handleerr(op);
            _sir_threadpool_destroy(pool);
            return false;
        }

#else /* __WIN__ */
        (*pool)->threads[n] = (HANDLE)_beginthreadex(NULL, 0, &thread_pool_proc,
            *pool, 0, NULL);
        if (!(*pool)->threads[n]) {
            _sir_handleerr(errno);
            _sir_threadpool_destroy(pool);
            return false;
        }
#endif
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
        return false;

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

    for (size_t n = 0; n < (*pool)->num_threads; n++) {
        if ((*pool)->threads[n]) {
            _sir_selflog("joining thread %zu of %zu...", n + 1, (*pool)->num_threads);
#if !defined(__WIN__)
            int join = pthread_join((*pool)->threads[n], NULL);
            SIR_ASSERT_UNUSED(0 == join, join);
#else /* __WIN__ */
            DWORD join = WaitForSingleObject((*pool)->threads[n], INFINITE);
            SIR_ASSERT_UNUSED(WAIT_OBJECT_0 == join, join);
#endif
        }
    }

    bool destroy = _sir_queue_destroy(&(*pool)->jobs);
    SIR_ASSERT_UNUSED(destroy, destroy);

    destroy = _sir_conddestroy(&(*pool)->cond);
    SIR_ASSERT_UNUSED(destroy, destroy);

    destroy = _sir_mutexdestroy(&(*pool)->mutex);
    SIR_ASSERT_UNUSED(destroy, destroy);

    _sir_safefree(&(*pool)->threads);
    _sir_safefree(pool);

    return true;
}

#if !defined(__WIN__)
static void* thread_pool_proc(void* arg)
#else
static unsigned thread_pool_proc(void* arg)
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
                _sir_safefree(&job->data);
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
    return 0;
#endif
}
