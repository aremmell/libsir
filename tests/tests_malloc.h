/*
 * tests_malloc.h
 *
 * Version: 2.2.6
 *
 * ----------------------------------------------------------------------------
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

#ifndef _SIR_TESTS_MALLOC_H_INCLUDED
# define _SIR_TESTS_MALLOC_H_INCLUDED

# if defined(MTMALLOC)
#  include <mtmalloc.h>
#  if !defined(DEBUG)
mallocctl(MTDOUBLEFREE, 0);
#  else
mallocctl(MTDOUBLEFREE, 1);
mallocctl(MTINITBUFFER, 1);
mallocctl(MTDEBUGPATTERN, 1);
#  endif
# endif

# if !defined(DEBUG_MALLOC_FILL_BYTE)
#  define DEBUG_MALLOC_FILL_BYTE 0x2E
# endif

# if defined(DUMA)
#  if defined(DUMA_EXPLICIT_INIT)
duma_init();
#  endif
#  if defined(DUMA_MIN_ALIGNMENT)
#   if DUMA_MIN_ALIGNMENT > 0
DUMA_SET_ALIGNMENT(DUMA_MIN_ALIGNMENT);
#   endif
#  endif
DUMA_SET_FILL(DEBUG_MALLOC_FILL_BYTE);
# endif

# if defined(DEBUG)
#  if defined(__GLIBC__)
#   if !defined(_GNU_SOURCE)
#    define _GNU_SOURCE 1
#   endif
#   if !defined(DUMA)
#    include <malloc.h>
#   endif
#   if GLIBC_VERSION >= 20400 && defined(M_PERTURB)
mallopt(M_PERTURB, DEBUG_MALLOC_FILL_BYTE);
#   endif
#   if defined(M_CHECK_ACTION)
mallopt(M_CHECK_ACTION, 3);
#   endif
#  endif
# endif

# if defined(__WIN__) && defined(DEBUG) && defined(_DEBUG)
#  if defined(_CRTDBG_CHECK_ALWAYS_DF) && \
      defined(_CRTDBG_DELAY_FREE_MEM_DF) && \
      defined(_CRTDBG_LEAK_CHECK_DF)
_CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_DELAY_FREE_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#  endif
# endif

#endif /* !_SIR_TESTS_MALLOC_H_INCLUDED */
