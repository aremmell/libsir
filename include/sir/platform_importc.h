/*
 * platform-importc.h
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
#ifndef _SIR_PLATFORM_IMPORTC_H_INCLUDED
# define _SIR_PLATFORM_IMPORTC_H_INCLUDED

# undef __HAVE_ATOMIC_H__
# undef __SIZEOF_INT128__
# define _BITS_FLOATN_H 1
# undef __builtin_bswap16
# define __builtin_bswap16
# undef __builtin_bswap32
# define __builtin_bswap32
# undef __builtin_bswap64
# define __builtin_bswap64
# undef __builtin___snprintf_chk
# define __builtin___snprintf_chk
# undef __builtin___vsnprintf_chk
# define __builtin___vsnprintf_chk
# undef __builtin___strlcpy_chk
# define __builtin___strlcpy_chk
# undef __builtin___strlcat_chk
# define __builtin___strlcat_chk
# undef __builtin_object_size
# define __builtin_object_size
# undef __extension__
# define __extension__
# undef __asm__
# define __asm__ asm
# if !defined(__restrict)
#  define __restrict restrict
# endif
# if !defined(__inline)
#  define __inline
# endif

#endif /* !_SIR_PLATFORM_IMPORTC_H_INCLUDED */
