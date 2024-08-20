/*
 * tests_malloc_bsd.h
 *
 * Version: 2.2.5
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

#ifndef _SIR_TESTS_MALLOC_BSD_H_INCLUDED
# define _SIR_TESTS_MALLOC_BSD_H_INCLUDED

# if defined(__FreeBSD__) && defined(DEBUG)
#  if !(__FreeBSD_version < 1000011)
const char *malloc_conf = "abort:true,confirm_conf:true,junk:true";
#  else
const char *malloc_conf = "JR";
#  endif
# endif

# if defined(__NetBSD__) && defined(DEBUG)
const char *malloc_conf = "abort:true,junk:true";
# endif

# if defined(__OpenBSD__)
#  if defined(DEBUG)
char *malloc_options = "CFGJRU";
#  else
char *malloc_options = "j";
#  endif
# endif

#endif /* !_SIR_TESTS_MALLOC_BSD_H_INCLUDED */
