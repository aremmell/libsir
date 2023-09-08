/*
 * vim: filetype=c:tabstop=4:tw=100:expandtab
 * SPDX-License-Identifier: MIT
 * scspell-id: 783576da-f630-11ec-962c-80ee73e9b8e7
 *
 * ---------------------------------------------------------------------------
 *
 * Copyright (c) 2015 Jeff Roberts
 * Copyright (c) 2017 Sean Barrett
 * Copyright (c) 2020 Peter Miller
 * Copyright (c) 2020-2022 Jeffrey H. Johnson <trnsz@pobox.com>
 * Copyright (c) 2021-2023 The DPS8M Development Team
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 *  * The above copyright notice and this permission notice shall be
 *    included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHOR OR COPYRIGHT HOLDER BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * ---------------------------------------------------------------------------
 */
#ifdef SIR_DPSPRINTF

# include <ctype.h>
# include <math.h>
# include <stdarg.h>
# include <stddef.h>
# include <stdio.h>
# include <stdlib.h>
# include <stdint.h>

# ifndef _DPSPRINTF_H
#  define _DPSPRINTF_H 1
#  define USING_DPSPRINTF 1

#  undef vsprintf
#  undef vsnprintf
#  undef sprintf
#  undef snprintf
#  undef vfprintf
#  undef vprintf
#  undef fprintf
#  undef printf

#  define vsprintf  dps_vsprintf  //-V1059
#  define vsnprintf dps_vsnprintf //-V1059
#  define sprintf   dps_sprintf   //-V1059
#  define snprintf  dps_snprintf  //-V1059
#  define vfprintf  dps_vfprintf  //-V1059
#  define vprintf   dps_vprintf   //-V1059
#  define fprintf   dps_fprintf   //-V1059
#  define printf    dps_printf    //-V1059

#  ifndef DPS_SPRINTF_IMPLEMENTATION
#   define DPS_SPRINTF_IMPLEMENTATION 1
#  endif

#  ifndef DPS_SPRINTF_DEFAULT
#   define DPS_SPRINTF_DEFAULT 1
#  endif

typedef char *DPS_S_SPRINTFCB (const char *buf, void *user, int len);

#  ifdef DPS_SPRINTF_LD
int32_t dps__real_to_str (char const **start, uint32_t *len, char *out,
                          int32_t *decimal_pos, long double value,
                          uint32_t frac_digits);
#  else
int32_t dps__real_to_str (char const **start, uint32_t *len, char *out,
                          int32_t *decimal_pos, double value,
                          uint32_t frac_digits);
#  endif

int vsprintf
     (char *buf, char const *fmt, va_list va);
int vsnprintf
     (char *buf, int count, char const *fmt, va_list va);
int sprintf
     (char *buf, char const *fmt, ...);
int snprintf
     (char *buf, int count, char const *fmt, ...);
int vsprintfcb
     (DPS_S_SPRINTFCB *callback,
      void *user, char *buf, char const *fmt, va_list va);
int vfprintf
     (FILE *stream, const char *format, va_list va);
int vprintf
     (const char *format, va_list va);
int fprintf
     (FILE *stream, const char *format, ...);
int printf
     (const char *format, ...);
void set_separators
       (char comma, char period);

# endif

#endif
