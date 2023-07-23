/*
 * sirqueue.c
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
#include "sir/queue.h"
#include "sir/helpers.h"
#include "sir/errors.h"

sir_queue_node* _sir_queue_node_create(void* data) {
    sir_queue_node* retval = calloc(1, sizeof(sir_queue_node));
    if (!retval)
        _sir_handleerr(errno);
    else
        retval->data = data;

    return retval;
}

bool _sir_queue_node_destroy(sir_queue_node** node, void** data) {
    if (!node || !*node)
        return false;

    if (data)
        *data = (*node)->data;

    _sir_safefree(node);

    return true;
}

bool _sir_queue_create(sir_queue** q) {
    if (!q)
        return false;

    *q = calloc(1, sizeof(sir_queue));
    if (!*q)
        _sir_handleerr(errno);

    return !!*q;
}

bool _sir_queue_destroy(sir_queue** q) {
    if (!q || !*q)
        return false;

    sir_queue_node* next = (*q)->head;
    while (next) {
        sir_queue_node* this_node = next;
        next                      = this_node->next;
        (void)_sir_queue_node_destroy(&this_node, NULL);
    }

    _sir_safefree(q);

    return true;
}

size_t _sir_queue_size(sir_queue* q) {
    if (_sir_queue_isempty(q))
        return 0;

    sir_queue_node* next = q->head->next;
    size_t idx           = 1;
    while (next) {
        idx++;
        next = next->next;
    }

    return idx;
}

bool _sir_queue_isempty(sir_queue* q) {
    return !q || !q->head;
}

bool _sir_queue_push(sir_queue* q, void* data) {
    if (!q)
        return false;

    if (!q->head) {
        q->head = _sir_queue_node_create(data);
        return true;
    }

    sir_queue_node* next = q->head;
    while (next) {
        if (!next->next) {
            next->next = _sir_queue_node_create(data);
            if (next->next)
                return true;
        }
        next = next->next;
    }

    return false;
}

bool _sir_queue_pop(sir_queue* q, void** data) {
    if (_sir_queue_isempty(q) || !data)
        return false;

    sir_queue_node* old_head = q->head;
    q->head                  = old_head->next;

    return _sir_queue_node_destroy(&old_head, data);
}
