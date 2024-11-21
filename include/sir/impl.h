/*
 * impl.h
 *
 * Version: 2.2.6
 *
 * -----------------------------------------------------------------------------
 *
 * SPDX-License-Identifier: ISC and BSD-3-Clause
 *
 * -----------------------------------------------------------------------------
 *
 * Copyright (c) 1998, 2010, 2015 Todd C. Miller <millert@openbsd.org>
 * Copyright (c) 2018-2024 Jeffrey H. Johnson <trnsz@pobox.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * -----------------------------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * OpenBSD strlcat() -- 1.19 2019/01/25 00:19:25 millert
 * -----------------------------------------------------------------------------
 */

#if defined(SIR_IMPL_STRLCAT) && !defined(SIR_IMPL_STRLCAT_DEF)
# undef strlcat

/*
 * Appends src to string dst of size dsize (unlike strncat, dsize is the
 * full size of dst, not space left).  At most dsize-1 characters
 * will be copied.  Always NUL terminates (unless dsize <= strlen(dst)).
 * Returns strlen(src) + MIN(dsize, strlen(initial dst)).
 * If retval >= dsize, truncation occurred.
 */

static inline size_t
_sir_strlcat(char *dst, const char *src, size_t dsize)
{
  const char *odst  = dst;
  const char *osrc  = src;
  size_t      n     = dsize;
  size_t      dlen;

  /*
   * Find the end of dst and adjust bytes
   * left, but don't go past the end.
   */

  while (n-- != 0 && *dst != '\0')
    dst++;

  dlen  =   dst - odst;
  n     = dsize - dlen;

  if (n-- == 0)
    return dlen + strlen(src);

  while (*src != '\0') {
    if (n != 0) {
      *dst++ = *src;
      n--;
    }
    src++;
  }

  *dst = '\0';

  return dlen + (src - osrc); /* count does not include NUL */
}

# define strlcat _sir_strlcat
# define SIR_IMPL_STRLCAT_DEF 1
#endif /* SIR_IMPL_STRLCAT */

/*
 * -----------------------------------------------------------------------------
 * OpenBSD strlcpy -- 1.16 2019/01/25 00:19:25 millert
 *-----------------------------------------------------------------------------
 */

#if defined(SIR_IMPL_STRLCPY) && !defined(SIR_IMPL_STRLCPY_DEF)
# undef strlcpy

/*
 * Copy string src to buffer dst of size dsize.  At most dsize-1
 * chars will be copied.  Always NUL terminates (unless dsize == 0).
 * Returns strlen(src); if retval >= dsize, truncation occurred.
 */

static inline size_t
_sir_strlcpy(char *dst, const char *src, size_t dsize)
{
  const char *osrc   = src;
  size_t      nleft  = dsize;

  /*
   * Copy as many bytes as will fit.
   */

  if (nleft != 0) {
    while (--nleft != 0) {
      if ((*dst++ = *src++) == '\0')
        break;
    }
  }

  /*
   * Not enough room in dst, add
   * NUL and traverse rest of src.
   */

  if (nleft == 0) {
    if (dsize != 0)
      *dst = '\0'; /* NUL-terminate dst */
    while (*src++);
  }

  return src - osrc - 1; /* count does not include NUL */
}

# define strlcpy _sir_strlcpy
# define SIR_IMPL_STRLCPY_DEF 1
#endif /* SIR_IMPL_STRLCPY */

/*
 * -----------------------------------------------------------------------------
 * OpenBSD strnlen -- 1.9 2019/01/25 00:19:25 millert
 * -----------------------------------------------------------------------------
 */

#if (defined(SIR_IMPL_STRNLEN) || defined(SIR_IMPL_STRNDUP)) && \
     !defined(SIR_IMPL_STRNLEN_DEF)
# undef strnlen

static inline size_t
_sir_strnlen(const char *str, size_t maxlen)
{
  const char *cp;

  for (cp = str; maxlen != 0 && *cp != '\0'; cp++, maxlen--);

  return (size_t)(cp - str);
}

# define strnlen _sir_strnlen
# define SIR_IMPL_STRNLEN_DEF 1
#endif /* SIR_IMPL_STRNLEN */

/*
 * -----------------------------------------------------------------------------
 * OpenBSD strndup -- 1.3 2019/01/25 00:19:25 millert
 * -----------------------------------------------------------------------------
 */

#if defined(SIR_IMPL_STRNDUP) && !defined(SIR_IMPL_STRNDUP_DEF)
# undef strndup

static inline char*
_sir_strndup(const char *str, size_t maxlen)
{
  char   *copy;
  size_t len;

  len  = _sir_strnlen(str, maxlen);

  copy = (char*)malloc(len + 1);
  if (copy != NULL) {
    (void)memcpy(copy, str, len);
    copy[len] = '\0';
  }

  return copy;
}

# define strndup _sir_strndup
# define SIR_IMPL_STRNDUP_DEF 1
#endif /* SIR_IMPL_STRNDUP */

/*
 * -----------------------------------------------------------------------------
 * OpenBSD strcasestr -- 1.4 2015/08/31 02:53:57 guenther
 * -----------------------------------------------------------------------------
 */

/*
 * Copyright (c) 1990, 1993 The Regents of the University of California.
 *
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by Chris Torek.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#if defined(SIR_IMPL_STRCASESTR) && !defined(SIR_IMPL_STRCASESTR_DEF)
# undef strcasestr

static inline char*
_sir_strcasestr(const char *s, const char *find)
{
  char c, sc;
  size_t len;

  if ((c = *find++) != 0) {
    c = (char)tolower((unsigned char)c);
    len = strlen(find);
    do {
      do {
        if ((sc = *s++) == 0)
          return NULL;
      } while ((char)tolower((unsigned char)sc) != c);
    } while (strncasecmp(s, find, len) != 0);
    s--;
  }

  return (char *)s;
}

# define strcasestr _sir_strcasestr
# define SIR_IMPL_STRCASESTR_DEF 1
#endif /* SIR_IMPL_STRCASESTR */
