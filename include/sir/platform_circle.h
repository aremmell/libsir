/*
 * platform_circle.h
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
#ifndef _SIR_PLATFORM_CIRCLE_H_INCLUDED
# define _SIR_PLATFORM_CIRCLE_H_INCLUDED

# if !defined(NEED_TA_CAST)
#  define NEED_TA_CAST 1
# endif
# if !defined(NEED_PT_CAST)
#  define NEED_PT_CAST 1
# endif

/*
 * Avoid stdatomic.h when using GNU libstdc++
 * https://github.com/seanbaxter/circle/issues/183
 */

# if !defined(_LIBCPP_ABI_VERSION)
#  undef __HAVE_ATOMIC_H__
# else
#  if __has_include(<stdatomic.h>)
#   include <stdatomic.h>
#  else
#   undef __HAVE_ATOMIC_H__
#  endif
# endif

/*
 * Ensure consistent use of built-in variadic functions
 * https://github.com/seanbaxter/circle/issues/182
 */

# if defined(__has_builtin)
#  define HAS_BUILTIN __has_builtin
# else
#  define HAS_BUILTIN 0
# endif
# if HAS_BUILTIN(__builtin_va_arg)
#  if defined(va_arg)
#   undef va_arg
#  endif
#  define va_arg __builtin_va_arg
# endif
# if HAS_BUILTIN(__builtin_va_copy)
#  if defined(va_copy)
#   undef va_copy
#  endif
#  define va_copy __builtin_va_copy
# endif
# if HAS_BUILTIN(__builtin_va_end)
#  if defined(va_end)
#   undef va_end
#  endif
#  define va_end __builtin_va_end
# endif
# if HAS_BUILTIN(__builtin_va_list)
#  if defined(va_list)
#   undef va_list
#  endif
#  define va_list __builtin_va_list
# endif
# if HAS_BUILTIN(__builtin_va_start)
#  if defined(va_start)
#   undef va_start
#  endif
#  define va_start __builtin_va_start
# endif

#endif /* !_SIR_PLATFORM_CIRCLE_H_INCLUDED */
