/*
 * platform_cxx.h
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2023
 * Version:   2.2.3
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
#ifndef _SIR_PLATFORM_CXX_H_INCLUDED
# define _SIR_PLATFORM_CXX_H_INCLUDED

# if defined(__cplusplus)
#  if !defined(CPLUSPLUS_MODE)
#   define CPLUSPLUS_MODE 1
#  endif
#  if !defined(NEED_TA_CAST)
#   define NEED_TA_CAST 1
#  endif
#  if !defined(NEED_PT_CAST)
#   define NEED_PT_CAST 1
#  endif
#  if defined(_MSC_VER)
#   undef NEED_PT_CAST
#  endif
#  if defined(restrict)
#   undef restrict
#  endif
#  define restrict __restrict
#  if defined(__BEGIN_DECLS)
#   undef __BEGIN_DECLS
#  endif
#  define __BEGIN_DECLS extern "C" {
#  if defined(__END_DECLS)
#   undef __END_DECLS
#  endif
#  define __END_DECLS }
# else
#  define __BEGIN_DECLS
#  define __END_DECLS
# endif

#endif /* !_SIR_PLATFORM_CXX_H_INCLUDED */
