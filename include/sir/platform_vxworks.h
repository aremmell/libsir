/*
 * platform_vxworks.h
 *
 * Author:    Jeffrey H. Johnson <trnsz@pobox.com>
 * Copyright: Copyright (c) 2018-2023
 * Version:   2.2.4
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
#ifndef _SIR_PLATFORM_VXWORKS_H_INCLUDED
# define _SIR_PLATFORM_VXWORKS_H_INCLUDED

# include <vxWorks.h>
# include <vxCpuLib.h>
# include <vxWorksCommon.h>

# undef SIR_NO_THREAD_NAMES
# define SIR_NO_THREAD_NAMES

# if !defined(CPUSET_ISZERO)
#  define CPUSET_ISZERO(cpuset) ((cpuset) == 0)
# endif

# if !defined(CPUSET_ISSET)
#  define CPUSET_ISSET(cpuset, n) ((cpuset) & (1U << (n)))
# endif

# if !defined(CPUSET_CLR)
#  define CPUSET_CLR(cpuset, n) ((cpuset) &= ~(1U << (n)))
# endif

#endif /* !_SIR_PLATFORM_VXWORKS_H_INCLUDED */
