/*
 * platform_importc.h
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

#ifndef _SIR_PLATFORM_IMPORTC_H_INCLUDED
# define _SIR_PLATFORM_IMPORTC_H_INCLUDED

# undef __HAVE_ATOMIC_H__
# undef __SIZEOF_INT128__
# undef BSWAP_REDEFINED
# define _BITS_FLOATN_H 1
# if defined(__APPLE__)
#  undef _DARWIN_C_SOURCE
#  define _DARWIN_C_SOURCE
#  include <libkern/OSByteOrder.h>
#  undef __builtin_bswap16
#  define __builtin_bswap16(x) _OSSwapInt16(x)
#  undef __builtin_bswap32
#  define __builtin_bswap32(x) _OSSwapInt32(x)
#  undef __builtin_bswap64
#  define __builtin_bswap64(x) _OSSwapInt64(x)
#  define BSWAP_REDEFINED 1
# elif defined(__linux__)
#  undef _GNU_SOURCE
#  define _GNU_SOURCE 1
#  if defined __has_include
#   if __has_include(<features.h>)
#    include <features.h>
#   endif
#  endif
#  if defined __has_include
#   if __has_include(<byteswap.h>)
#    include <byteswap.h>
#   endif
#  endif
#  if defined(__GLIBC__) && !defined(bswap_16) && !defined(bswap_32) && !defined(bswap_64)
#   undef __builtin_bswap16
#   define __builtin_bswap16(x) swap_16(x)
#   undef __builtin_bswap32
#   define __builtin_bswap32(x) swap_32(x)
#   undef __builtin_bswap64
#   define __builtin_bswap64(x) swap_64(x)
#   define BSWAP_REDEFINED 1
#  elif defined(bswap_16) && defined(bswap_32) && defined(bswap_64)
#   undef __builtin_bswap16
#   define __builtin_bswap16(x) bswap_16(x)
#   undef __builtin_bswap32
#   define __builtin_bswap32(x) bswap_32(x)
#   undef __builtin_bswap64
#   define __builtin_bswap64(x) bswap_64(x)
#   define BSWAP_REDEFINED 1
#  endif
# elif defined(__FreeBSD__)
#  undef _BSD_SOURCE
#  define _BSD_SOURCE
#  undef _DEFAULT_SOURCE
#  define _DEFAULT_SOURCE
#  include <sys/endian.h>
#  undef __builtin_bswap16
#  define __builtin_bswap16(x) bswap16(x)
#  undef __builtin_bswap32
#  define __builtin_bswap32(x) bswap32(x)
#  undef __builtin_bswap64
#  define __builtin_bswap64(x) bswap64(x)
#  define BSWAP_REDEFINED 1
# endif
# if !defined(BSWAP_REDEFINED)
#  undef __builtin_bswap16
#  define __builtin_bswap16
#  undef __builtin_bswap32
#  define __builtin_bswap32
#  undef __builtin_bswap64
#  define __builtin_bswap64
#  define BSWAP_REDEFINED 1
# endif
# undef __builtin___snprintf_chk
# define __builtin___snprintf_chk(s, c, flag, os, fmt, ...) snprintf(s, c, fmt, __VA_ARGS__)
# undef __builtin___sprintf_chk
# define __builtin___sprintf_chk(s, flag, os, fmt, ...) sprintf(s, fmt, __VA_ARGS__)
# undef __builtin___vsnprintf_chk
# define __builtin___vsnprintf_chk(s, c, flag, os, fmt, ...) vsnprintf(s, c, fmt, __VA_ARGS__)
# undef __builtin___strlcat_chk
# define __builtin___strlcat_chk(dest, src, x, n) strlcat(dest, src, x)
# undef __builtin___strlcpy_chk
# define __builtin___strlcpy_chk(dest, src, x, n) strlcpy(dest, src, x)
# undef __builtin_object_size
# define __builtin_object_size
# undef __extension__
# define __extension__
# undef __asm__
# if !defined(__FreeBSD__)
#  define __asm__ asm
# else
#  define __asm__(...)
# endif
# if !defined(__restrict)
#  define __restrict restrict
# endif
# if !defined(__inline)
#  define __inline
# endif

#endif /* !_SIR_PLATFORM_IMPORTC_H_INCLUDED */
