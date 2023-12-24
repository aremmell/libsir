/*
 * platform_orangec.h
 *
 * Version: 2.2.5
 *
 * -----------------------------------------------------------------------------
 *
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2018-2024 Jeffrey H. Johnson <trnsz@pobox.com>
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

#ifndef _SIR_PLATFORM_ORANGEC_H_INCLUDED
# define _SIR_PLATFORM_ORANGEC_H_INCLUDED

# if defined(__ORANGEC_MAJOR__) && defined(__ORANGEC_MINOR__) && defined(__ORANGEC_PATCHLEVEL__)
#  if __ORANGEC_MAJOR__ <= 6 && __ORANGEC_MINOR__ <= 71 && __ORANGEC_PATCHLEVEL__ <= 9
#   if !defined(ORANGEC_VERSION_WARNING)
#    warning OrangeC versions before 6.71.10 are unsupported.
#    define ORANGEC_VERSION_WARNING
#   endif
#  endif
# endif

# include <sysinfoapi.h>

# if !defined(SIR_MSVCRT_MINGW)
#  define SIR_MSVCRT_MINGW
# endif

#endif /* !_SIR_PLATFORM_ORANGEC_H_INCLUDED */
