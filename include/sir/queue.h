/*
 * queue.h
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

#ifndef _SIR_QUEUE_H_INCLUDED
# define _SIR_QUEUE_H_INCLUDED

# include "sir/types.h"

/** Creates a sir_queue_node and sets its data property. */
sir_queue_node* _sir_queue_node_create(void* data);

/** Deletes a previously created sir_queue_node and optionally returns its
 * data. */
bool _sir_queue_node_destroy(sir_queue_node** node, void** data);

/** Creates an empty sir_queue. */
bool _sir_queue_create(sir_queue** q);

/** Destroys a sir_queue (empty or otherwise). */
bool _sir_queue_destroy(sir_queue** q);

/** Returns the number of nodes in a queue. */
size_t _sir_queue_size(sir_queue* q);

/** `true` if the queue contains zero nodes, `false` otherwise. */
bool _sir_queue_isempty(const sir_queue* q);

/** Pushes a new node onto the back of a queue. */
bool _sir_queue_push(sir_queue* q, void* data);

/** Pops a node off the front of a queue, if one is available. */
bool _sir_queue_pop(sir_queue* q, void** data);

#endif /* !_SIR_QUEUE_H_INCLUDED */
