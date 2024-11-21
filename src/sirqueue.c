/*
 * sirqueue.c
 *
 * Version: 2.2.6
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

#include "sir/queue.h"
#include "sir/helpers.h"
#include "sir/errors.h"

sir_queue_node* _sir_queue_node_create(void* data) {
    sir_queue_node* retval = calloc(1, sizeof(sir_queue_node));
    if (!retval)
        (void)_sir_handleerr(errno);
    else
        retval->data = data;

    return retval;
}

bool _sir_queue_node_destroy(sir_queue_node** node, void** data) {
    bool valid = _sir_validptrptr(node) && _sir_validptr(*node);

    if (valid) {
        if (data)
            *data = (*node)->data;

        _sir_safefree(node);
    }

    return valid;
}

bool _sir_queue_create(sir_queue** q) {
    bool valid = _sir_validptrptr(q);

    if (valid) {
        *q = calloc(1, sizeof(sir_queue));
        if (!_sir_validptrnofail(*q))
            (void)_sir_handleerr(errno);
    }

    return valid && _sir_validptrnofail(*q);
}

bool _sir_queue_destroy(sir_queue** q) {
    bool valid = _sir_validptrptr(q) && _sir_validptr(*q);

    if (valid) {
        sir_queue_node* next = (*q)->head;
        while (next) {
            sir_queue_node* this_node = next;
            void* data                = NULL;
            next                      = this_node->next;

            if (_sir_queue_node_destroy(&this_node, &data))
                _sir_safefree(&data);
        }

        _sir_safefree(q);
    }

    return valid;
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

bool _sir_queue_isempty(const sir_queue* q) {
    return !q || !q->head;
}

bool _sir_queue_push(sir_queue* q, void* data) {
    bool retval = false;

    if (_sir_validptr(q)) {
        if (!q->head) {
            q->head = _sir_queue_node_create(data);
            retval = NULL != q->head;
        } else {
            sir_queue_node* next = q->head;
            while (next) {
                if (!next->next) {
                    next->next = _sir_queue_node_create(data);
                    if (next->next) {
                        retval = true;
                        break;
                    }
                }
                next = next->next;
            }
        }
    }

    return retval;
}

bool _sir_queue_pop(sir_queue* q, void** data) {
    bool retval = false;

    if (!_sir_queue_isempty(q) && _sir_validptrptr(data)) {
        sir_queue_node* old_head = q->head;
        q->head                  = old_head->next;

        retval = _sir_queue_node_destroy(&old_head, data);
    }

    return retval;
}
