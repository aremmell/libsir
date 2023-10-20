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

#if !defined(_WRS_FUNC_NORETURN_)
# define _WRS_FUNC_NORETURN __attribute__((__noreturn__))
#endif

typedef int64_t        INT64;
typedef uint64_t       UINT64;

typedef _Vx_ARGINT     ARGINT;
typedef _Vx_BOOL       BOOL;
typedef _Vx_FUNCPTR    FUNCPTR;
typedef _Vx_INT16      INT16;
typedef _Vx_INT32      INT32;
typedef _Vx_INT8       INT8;
typedef _Vx_MSG_Q_ID   MSG_Q_ID;
typedef _Vx_OBJ_HANDLE OBJ_HANDLE;
typedef _Vx_PART_ID    PART_ID;
typedef _Vx_REG_SET    REG_SET;
typedef _Vx_SEM_ID     SEM_ID;
typedef _Vx_STATUS     STATUS;
typedef _Vx_UCHAR      UCHAR;
typedef _Vx_UINT16     UINT16;
typedef _Vx_UINT32     UINT32;
typedef _Vx_UINT8      UINT8;
typedef _Vx_UINT       UINT;
typedef _Vx_ULONG      ULONG;
typedef _Vx_USHORT     USHORT;
typedef _Vx_VOID       VOID;

typedef OBJ_HANDLE     CONDVAR_ID;
typedef OBJ_HANDLE     MSG_Q_ID_KERNEL;
typedef OBJ_HANDLE     RTP_ID;
typedef OBJ_HANDLE     SD_ID;
typedef OBJ_HANDLE     SEM_ID_KERNEL;
typedef OBJ_HANDLE     TASK_ID;

# include <vxWorks.h>
# include <types/vxWind.h>
# include <vxCpuLib.h>
# include <taskLib.h>

/* # undef SIR_NO_THREAD_NAMES */
/* # define SIR_NO_THREAD_NAMES */

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
