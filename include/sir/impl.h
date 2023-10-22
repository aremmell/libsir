/*
 * impl.h
 *
 * -----------------------------------------------------------------------------
 *
 * strlcat from: $OpenBSD: strlcat.c,v 1.19 2019/01/25 00:19:25 millert Exp $
 * strlcpy from: $OpenBSD: strlcpy.c,v 1.16 2019/01/25 00:19:25 millert Exp $
 * strnlen from: $OpenBSD: strnlen.c,v 1.9  2019/01/25 00:19:25 millert Exp $
 * strndup from: $OpenBSD: strndup.c,v 1.3  2019/01/25 00:19:25 millert Exp $
 *
 * -----------------------------------------------------------------------------
 *
 * SPDX-License-Identifier: ISC
 *
 * Copyright (c) 1998, 2010, 2015 Todd C. Miller <millert@openbsd.org>
 * Copyright (c) 2018-2023 Ryan M. Lederman <lederman@gmail.com>
 * Copyright (c) 2018-2023 Jeffrey H. Johnson <trnsz@pobox.com>
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
 * OpenBSD strlcat()
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
 * OpenBSD strlcpy
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
 * OpenBSD strnlen
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
 * OpenBSD strndup
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
