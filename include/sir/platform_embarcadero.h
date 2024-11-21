/*
 * platform_embarcadero.h
 *
 * Version: 2.2.6
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

#ifndef _SIR_PLATFORM_EMBARCADERO_H_INCLUDED
# define _SIR_PLATFORM_EMBARCADERO_H_INCLUDED

# if (defined(__TURBOC__) || defined(__BORLANDC__) || \
     defined(__BCPLUSPLUS__) || defined(__CODEGEARC__))
#  if !defined(__EMBARCADEROC__)
#   define __EMBARCADEROC__
#  endif
#  if !defined(SIR_MSVCRT_MINGW)
#   define SIR_MSVCRT_MINGW
#  endif
#  undef __HAVE_STDC_SECURE_OR_EXT1__
#  if defined(__UINTPTR_FMTx__)
#   define SIR_UIPTRx __UINTPTR_FMTx__
#  elif defined(PRIxPTR)
#   define SIR_UIPTRx PRIxPTR
#  endif
#  if defined(SIR_UIPTRx)
#   undef PRIxPTR
#   define PRIxPTR SIR_UIPTRx
#  endif
#  if defined(__UINTPTR_FMTX__)
#   define SIR_UIPTRX __UINTPTR_FMTX__
#  elif defined(PRIXPTR)
#   define SIR_UIPTRx PRIXPTR
#  endif
#  if defined(SIR_UIPTRX)
#   undef PRIXPTR
#   define PRIXPTR SIR_UIPTRX
#  endif
#  if defined(__clang_major__)
#   if defined(SIR_PTHREAD_GETNAME_NP) && (__clang_major__ >= 15)
#    undef SIR_PTHREAD_GETNAME_NP
#   endif
#  else
#   define __clang_major__ 0
#  endif
# endif

#endif /* !_SIR_PLATFORM_EMBARCADERO_H_INCLUDED */
