/*
 * mcmb.c
 *
 * Version: 2120.5.07-dps (libcmb 3.5.6)
 *
 * -----------------------------------------------------------------------------
 *
 * scspell-id: ec490dbd-f630-11ec-a71d-80ee73e9b8e7
 *
 * ---------------------------------------------------------------------------
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2002-2019 Devin Teske <dteske@FreeBSD.org>
 * Copyright (c) 2020-2024 Jeffrey H. Johnson <trnsz@pobox.com>
 * Copyright (c) 2021-2024 The DPS8M Development Team
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * ---------------------------------------------------------------------------
 */

//-V::550,575,629,701,1048,1059

/*
 * mcmb - (miniature) combinatorics utility
 *
 * This code was derived from the libcmb combinatorics
 * library and the cmb combinatorics utility written by
 * Devin Teske <dteske@FreeBSD.org> and is distributed
 * under the terms of a two-clause BSD license.
 */

#if !defined(_GNU_SOURCE)
# define _GNU_SOURCE
#endif

#if !defined(__EXTENSIONS__)
# define __EXTENSIONS__
#endif

#include <sys/param.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#if defined(__APPLE__)
# include <xlocale.h>
#endif
#include <locale.h>

#if defined(__MACH__) && defined(__APPLE__) && \
  ( defined(__PPC__) || defined(_ARCH_PPC) )
# include <mach/clock.h>
# include <mach/mach.h>
# if defined(MACOSXPPC)
#  undef MACOSXPPC
# endif /* if defined(MACOSXPPC) */
# define MACOSXPPC 1
#endif /* if defined(__MACH__) && defined(__APPLE__) &&
           ( defined(__PPC__) || defined(_ARCH_PPC) ) */
#if !defined(TRUE)
# define TRUE 1
#endif /* if !defined(TRUE) */

#if !defined(FALSE)
# define FALSE 0
#endif /* if !defined(FALSE) */

#undef FREE
#define FREE(p) do  \
  {                 \
    free((p));      \
    (p) = NULL;     \
  } while(0)

/*
 * Version information
 */

#define CMB_VERSION         0
#define CMB_H_VERSION_MAJOR 3
#define CMB_H_VERSION_MINOR 5
#define CMB_H_VERSION_PATCH 9

/*
 * Macros for cmb_config options bitmask
 */
struct cmb_config;             /* Forward declaration               */
#define CMB_OPT_EMPTY    0x08  /* Show empty set with no items      */
#define CMB_OPT_NUMBERS  0x10  /* Show combination sequence numbers */
#define CMB_OPT_RESERVED 0x20  /* Reserved for future use by cmb(3) */
#define CMB_OPT_OPTION1  0x40  /* Available (unused by cmb(3))      */
#define CMB_OPT_OPTION2  0x80  /* Available (unused by cmb(3))      */

/*
 * Macros for defining call-back functions/pointers
 */

#define CMB_ACTION(x)           \
  int x(                        \
    struct cmb_config *config,  \
    uint64_t seq,               \
    uint32_t nitems,            \
    char *items[])

/*
 * Anatomy of config option to pass as cmb*() config argument
 */

struct cmb_config
{
  uint8_t  options;    /* CMB_OPT_* bitmask. Default 0              */
  char    *delimiter;  /* Item separator (default is " ")           */
  char    *prefix;     /* Prefix text for each combination          */
  char    *suffix;     /* Suffix text for each combination          */
  uint32_t size_min;   /* Minimum number of elements in combination */
  uint32_t size_max;   /* Maximum number of elements in combination */

  uint64_t count;      /* Number of combinations                    */
  uint64_t start;      /* Starting combination                      */

  void *data;          /* Reserved for action callback              */

  /*
   * cmb(3) function callback; called for each combination (default is
   * cmb_print()). If the return from action() is non-zero, cmb() will
   * stop calculation. The cmb() return value is the first non-zero
   * result from action(), zero otherwise.
   */

  CMB_ACTION(( *action ));
};

static int cmb(struct cmb_config *_config, uint32_t _nitems, char *_items[]);

static uint64_t cmb_count(struct cmb_config *_config, uint32_t _nitems);

static int cmb_print(struct cmb_config *_config, uint64_t _seq,
                     uint32_t _nitems, char *_items[]);

static const char *libcmb_version(int _type);

/*
 * Inline functions
 */

static inline void
cmb_print_seq(uint64_t seq)
{
  (void)fprintf(stdout, "%" PRIu64 " ", seq);
}

/*
 * Transformations
 */

static int cmb_transform_precision;
struct cmb_xitem
{
  char *cp; /* original item */
  union cmb_xitem_type
  {
    long double ld; /* item as long double */
  } as;
};

#define CMB_TRANSFORM_EQ(eq, op, x, seqt, seqp)                               \
  int x(struct cmb_config *config, seqt seq, uint32_t nitems, char *items[])  \
  {                                                                           \
    uint8_t show_numbers = FALSE;                                             \
    uint32_t n;                                                               \
    long double ld;                                                           \
    long double total = 0;                                                    \
    const char *delimiter = " ";                                              \
    const char *prefix = NULL;                                                \
    const char *suffix = NULL;                                                \
    struct cmb_xitem *xitem = NULL;                                           \
                                                                              \
    if (config != NULL)                                                       \
      {                                                                       \
        if (config->delimiter != NULL)                                        \
        delimiter = config->delimiter;                                        \
        if (( config->options & CMB_OPT_NUMBERS ) != 0)                       \
        show_numbers = TRUE;                                                  \
        prefix = config->prefix;                                              \
        suffix = config->suffix;                                              \
      }                                                                       \
    if (show_numbers)                                                         \
      seqp(seq);                                                              \
    if (prefix != NULL && !opt_quiet)                                         \
      (void)fprintf(stdout, "%s", prefix);                                    \
    if (nitems > 0)                                                           \
      {                                                                       \
        (void)memcpy(&xitem, &items[0], sizeof ( struct cmb_xitem * ));       \
        total = xitem->as.ld;                                                 \
        if (!opt_quiet)                                                       \
          {                                                                   \
            (void)fprintf(stdout, "%s", xitem->cp);                           \
            if (nitems > 1)                                                   \
            (void)fprintf(stdout, "%s" #op "%s", delimiter, delimiter);       \
          }                                                                   \
      }                                                                       \
    for (n = 1; n < nitems; n++)                                              \
      {                                                                       \
        (void)memcpy(&xitem, &items[n], sizeof ( struct cmb_xitem * ));       \
        ld = xitem->as.ld;                                                    \
        total = eq;                                                           \
        if (!opt_quiet)                                                       \
          {                                                                   \
            (void)fprintf(stdout, "%s", xitem->cp);                           \
            if (n < nitems - 1)                                               \
            (void)fprintf(stdout, "%s" #op "%s", delimiter, delimiter);       \
          }                                                                   \
      }                                                                       \
    if (suffix != NULL && !opt_quiet)                                         \
      (void)fprintf(stdout, "%s", suffix);                                    \
    (void)fprintf(stdout,                                                     \
      "%s%.*Lf\n",                                                            \
      opt_quiet ? "" : " = ",                                                 \
      cmb_transform_precision,                                                \
      total);                                                                 \
    return 0;                                                                 \
  }

#define CMB_TRANSFORM_OP(op, x)                                               \
  CMB_TRANSFORM_EQ(total op ld, op, x, uint64_t, cmb_print_seq)

/*
 * Find transformations
 */

static char *cmb_transform_find_buf;

static int cmb_transform_find_buf_size;

static struct cmb_xitem *cmb_transform_find;

#define CMB_TRANSFORM_EQ_FIND(eq, op, x, seqt, seqp)                          \
  int x(struct cmb_config *config, seqt seq, uint32_t nitems, char *items[])  \
  {                                                                           \
    uint8_t show_numbers = FALSE;                                             \
    uint32_t n;                                                               \
    int len;                                                                  \
    long double ld;                                                           \
    long double total = 0;                                                    \
    const char *delimiter = " ";                                              \
    const char *prefix = NULL;                                                \
    const char *suffix = NULL;                                                \
    struct cmb_xitem *xitem = NULL;                                           \
                                                                              \
    for (n = 0; n < nitems; n++)                                              \
      {                                                                       \
        (void)memcpy(&xitem, &items[n], sizeof ( struct cmb_xitem * ));       \
        ld = xitem->as.ld;                                                    \
        total = eq;                                                           \
      }                                                                       \
    if (cmb_transform_precision == 0)                                         \
      {                                                                       \
        if (total != cmb_transform_find->as.ld)                               \
          {                                                                   \
            return 0;                                                         \
          }                                                                   \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        len = snprintf(NULL, 0, "%.*Lf", cmb_transform_precision, total) + 1; \
        if (len > cmb_transform_find_buf_size)                                \
          {                                                                   \
            cmb_transform_find_buf                                            \
              = realloc(cmb_transform_find_buf, (unsigned long)len);          \
            if (cmb_transform_find_buf == NULL)                               \
              {                                                               \
                (void)fprintf(stderr, "\rFATAL: Out of memory?! Aborting at"  \
                                      " %s[%s:%d]\r\n",                       \
                              __func__, __FILE__, __LINE__);                  \
                abort();                                                      \
                /*NOTREACHED*/ /* unreachable */                              \
              }                                                               \
            cmb_transform_find_buf_size = len;                                \
          }                                                                   \
        (void)sprintf(                                                        \
          cmb_transform_find_buf,                                             \
          "%.*Lf",                                                            \
          cmb_transform_precision,                                            \
          total);                                                             \
        if (strcmp(cmb_transform_find_buf, cmb_transform_find->cp) != 0)      \
        return 0;                                                             \
      }                                                                       \
    if (config != NULL)                                                       \
      {                                                                       \
        if (config->delimiter != NULL)                                        \
        delimiter = config->delimiter;                                        \
        if (( config->options & CMB_OPT_NUMBERS ) != 0)                       \
        show_numbers = TRUE;                                                  \
        prefix = config->prefix;                                              \
        suffix = config->suffix;                                              \
      }                                                                       \
    if (show_numbers)                                                         \
      seqp(seq);                                                              \
    if (prefix != NULL && !opt_quiet)                                         \
      (void)fprintf(stdout, "%s", prefix);                                    \
    if (nitems > 0)                                                           \
      {                                                                       \
        (void)memcpy(&xitem, &items[0], sizeof ( struct cmb_xitem * ));       \
        if (!opt_quiet)                                                       \
          {                                                                   \
            (void)fprintf(stdout, "%s", xitem->cp);                           \
            if (nitems > 1)                                                   \
            (void)fprintf(stdout, "%s" #op "%s", delimiter, delimiter);       \
          }                                                                   \
      }                                                                       \
    for (n = 1; n < nitems; n++)                                              \
      {                                                                       \
        (void)memcpy(&xitem, &items[n], sizeof ( struct cmb_xitem * ));       \
        if (!opt_quiet)                                                       \
          {                                                                   \
            (void)fprintf(stdout, "%s", xitem->cp);                           \
            if (n < nitems - 1)                                               \
            (void)fprintf(stdout, "%s" #op "%s", delimiter, delimiter);       \
          }                                                                   \
      }                                                                       \
    if (suffix != NULL && !opt_quiet)                                         \
      (void)fprintf(stdout, "%s", suffix);                                    \
    (void)fprintf(stdout,                                                     \
      "%s%.*Lf\n",                                                            \
      opt_quiet ? "" : " = ",                                                 \
      cmb_transform_precision,                                                \
      total);                                                                 \
    return 0;                                                                 \
  }

#define CMB_TRANSFORM_OP_FIND(op, x)                                          \
  CMB_TRANSFORM_EQ_FIND(total op ld, op, x, uint64_t, cmb_print_seq)

/*
 * Limits
 */

#if defined(BUFSIZE_MAX)
# undef BUFSIZE_MAX
#endif /* if defined(BUFSIZE_MAX) */
#define BUFSIZE_MAX ( 2 * 1024 * 1024 )

/*
 * Buffer size for read(2) input
 */

#if !defined(MAXPHYS)
# define MAXPHYS ( 128 * 1024 )
#endif /* if !defined(MAXPHYS) */

/*
 * Memory strategy threshold, in pages; if physmem
 * is larger than this, use a large buffer.
 */

#if !defined(PHYSPAGES_THRESHOLD)
# define PHYSPAGES_THRESHOLD ( 32 * 1024 )
#endif /* if !defined(PHYSPAGES_THRESHOLD) */

/*
 * Small (default) buffer size in bytes.
 * It's inefficient for this to be smaller than MAXPHYS.
 */

#define BUFSIZE_SMALL ( MAXPHYS )

/*
 * Math macros
 */

#if defined(MIN)
# undef MIN
#endif /* if defined(MIN) */
#define MIN(x, y) (( x ) < ( y ) ? ( x ) : ( y ))

#if defined(MAX)
# undef MAX
#endif /* if defined(MAX) */
#define MAX(x, y) (( x ) > ( y ) ? ( x ) : ( y ))

#if !defined(MAXPATHLEN)
# if defined(PATH_MAX) && PATH_MAX > 1024
#  define MAXPATHLEN PATH_MAX
# else
#  define MAXPATHLEN 1024
# endif /* if defined(PATH_MAX) && PATH_MAX > 1024 */
#endif /* if !defined(MAXPATHLEN) */

#if !defined(PATH_MAX)
# define PATH_MAX MAXPATHLEN
#endif /* if !defined(PATH_MAX) */

#if !defined(CMB_PARSE_FRAGSIZE)
# define CMB_PARSE_FRAGSIZE 512
#endif /* if !defined(CMB_PARSE_FRAGSIZE) */

static const char mcmbver[]         = "2120.5.07-dps";
static const char libversion[]      = "libcmb 3.5.6";

/*
 * Takes one of below described type constants. Returns string version.
 *
 * TYPE                 DESCRIPTION
 * CMB_VERSION          Short version text. For example, "x.y".
 *
 * For unknown type, the text "not available" is returned.
 */

static const char *
libcmb_version(int type)
{
  switch (type)
    {
    case CMB_VERSION:
      return libversion;

    default:
      return "not available";
    }
}

/*
 * Takes pointer to `struct cmb_config' options
 * and number of items. Returns total number of
 * combinations according to config options.
 */

static uint64_t
cmb_count(struct cmb_config *config, uint32_t nitems)
{
  uint8_t show_empty = FALSE;
  int8_t nextset = 1;
  uint32_t curset;
  uint32_t i = nitems;
  uint32_t k;
  uint32_t p;
  uint32_t setdone = nitems;
  uint32_t setinit = 1;
  uint64_t count = 0;
  long double z = 1;
  uint64_t ncombos;

  errno = 0;
  if (nitems == 0)
    {
      return 0;
    }

  /*
   * Process config options
   */

  if (config != NULL)
    {
      if (( config->options & CMB_OPT_EMPTY ) != 0)
        {
          show_empty = TRUE;
        }

      if (config->size_min != 0 || config->size_max != 0)
        {
          setinit = config->size_min;
          setdone = config->size_max;
        }
    }

  /*
   * Adjust values to be non-zero (mathematical constraint)
   */

  if (setinit == 0)
    {
      setinit = 1;
    }

  if (setdone == 0)
    {
      setdone = 1;
    }

  /*
   * Return zero if the request is out of range
   */

  if (setinit > nitems && setdone > nitems)
    {
      return 0;
    }

  /*
   * Enforce limits so we don't run over bounds
   */

  if (setinit > nitems)
    {
      setinit = nitems;
    }

  if (setdone > nitems)
    {
      setdone = nitems;
    }

  /*
   * Check for integer overflow
   */

  if (( setinit > setdone && setinit - setdone >= 64 )
      || ( setinit < setdone && setdone - setinit >= 64 ))
    {
      errno = ERANGE;
      return 0;
    }

  /*
   * If entire set is requested, return 2^N[-1]
   */

  if (( setinit == 1 && setdone == nitems )
      || ( setinit == nitems && setdone == 1 ))
    {
      if (show_empty)
        {
          if (nitems >= 64)
            {
              errno = ERANGE;
              return 0;
            }
          /* cppcheck-suppress shiftTooManyBits */
          return 1 << nitems;
        }
      return ULLONG_MAX >> ( 64 - nitems );
    }

  /*
   * Set the direction of flow (incrementing vs. decrementing)
   */

  if (setinit > setdone)
    {
      nextset = -1;
    }

  /*
   * Loop over each `set' in the configured direction until we are done
   */

  if (show_empty)
    {
      count++;
    }

  p = nextset > 0 ? setinit - 1 : setinit;
  for (k = 1; k <= p; k++)
    {
      z = ( z * i-- ) / k;
    }

  for (curset = setinit; nextset > 0 ? curset <= setdone : curset >= setdone;
       curset += (uint32_t)nextset)
    {

      /*
       * Calculate number of combinations (incrementing)
       */

      if (nextset > 0)
        {
          z = ( z * i-- ) / k++;
        }

      /*
       * Add number of combinations in this set to total
       */

      if (( ncombos = (uint64_t)z ) == 0)
        {
          errno = ERANGE;
          return 0;
        }

      if (ncombos > ULLONG_MAX - count)
        {
          errno = ERANGE;
          return 0;
        }

      count += ncombos;

      /*
       * Calculate number of combinations (decrementing)
       */

      if (nextset < 0)
        {
          z = ( z * --k ) / ++i;
        }
    }

  return count;
}

/*
 * Takes pointer to `struct cmb_config' options, number of
 * items, and array of `char *' items.  Calculates
 * combinations according to options and either prints
 * combinations to stdout (default) or runs `action' if
 * passed-in as function pointer member of `config' argument.
 */

static int
cmb(struct cmb_config *config, uint32_t nitems, char *items[])
{
  uint8_t docount      = FALSE;
  uint8_t doseek       = FALSE;
  uint8_t show_empty   = FALSE;
  uint8_t show_numbers = FALSE;
  int8_t nextset       = 1;
  int retval           = 0;
  uint32_t curset;
  uint32_t i = nitems;
  uint32_t k;
  uint32_t n;
  uint32_t p;
  uint32_t seed;
  uint32_t setdone     = nitems;
  uint32_t setinit     = 1;
  uint32_t setmax;
  uint32_t setnums_last;
  uint32_t setpos;
  uint32_t setpos_backend;
  uint64_t combo;
  uint64_t count       = 0;
  uint64_t ncombos;
  uint64_t seek        = 0;
  uint64_t seq         = 1;
  long double z        = 1;
  char **curitems;
  uint32_t *setnums;
  uint32_t *setnums_backend;

  CMB_ACTION(( *action )) = cmb_print;

  errno = 0;

  /*
   * Process config options
   */

  if (config != NULL)
    {
      if (config->action != NULL)
        {
          action = config->action;
        }

      if (config->count != 0)
        {
          docount = TRUE;
          count = config->count;
        }

      if (( config->options & CMB_OPT_EMPTY ) != 0)
        {
          show_empty = TRUE;
        }

      if (( config->options & CMB_OPT_NUMBERS ) != 0)
        {
          show_numbers = TRUE;
        }

      if (config->size_min != 0 || config->size_max != 0)
        {
          setinit = config->size_min;
          setdone = config->size_max;
        }

      if (config->start > 1)
        {
          doseek = TRUE;
          seek = config->start;
          if (show_numbers)
            {
              seq = seek;
            }
        }
    }

  if (!show_empty)
    {
      if (nitems == 0)
        {
          return 0;
        }
      else if (cmb_count(config, nitems) == 0)
        {
          return errno;
        }
    }

  /*
   * Adjust values to be non-zero (mathematical constraint)
   */

  if (setinit == 0)
    {
      setinit = 1;
    }

  if (setdone == 0)
    {
      setdone = 1;
    }

  /*
   * Enforce limits so we don't run over bounds
   */

  if (setinit > nitems)
    {
      setinit = nitems;
    }

  if (setdone > nitems)
    {
      setdone = nitems;
    }

  /*
   * Set the direction of flow (incrementing vs. decrementing)
   */

  if (setinit > setdone)
    {
      nextset = -1;
    }

  /*
   * Show the empty set consisting of a single combination of no-items
   */

  if (nextset > 0 && show_empty)
    {
      if (!doseek)
        {
          retval = action(config, seq++, 0, NULL);
          if (retval != 0)
            {
              return retval;
            }

          if (docount && --count == 0)
            {
              return retval;
            }
        }
      else
        {
          seek--;
          if (seek == 1)
            {
              doseek = FALSE;
            }
        }
    }

  if (nitems == 0)
    {
      return 0;
    }

  /*
   * Allocate memory
   */

  setmax = setdone > setinit ? setdone : setinit;
  if (( curitems = (char **)malloc(sizeof ( char * ) * setmax)) == NULL)
    {
      (void)fprintf(stderr, "\rFATAL: Out of memory?! Aborting at %s[%s:%d]\r\n",
                    __func__, __FILE__, __LINE__);
      abort();
      /*NOTREACHED*/ /* unreachable */
    }

  if (( setnums = (uint32_t *)malloc(sizeof ( uint32_t ) * setmax)) == NULL)
    {
      (void)fprintf(stderr, "\rFATAL: Out of memory?! Aborting at %s[%s:%d]\r\n",
                    __func__, __FILE__, __LINE__);
      abort();
      /*NOTREACHED*/ /* unreachable */
    }

  if (( setnums_backend =
          (uint32_t *)malloc(sizeof ( uint32_t ) * setmax)) == NULL)
    {
      (void)fprintf(stderr, "\rFATAL: Out of memory?! Aborting at %s[%s:%d]\r\n",
                    __func__, __FILE__, __LINE__);
      abort();
      /*NOTREACHED*/ /* unreachable */
    }

  /*
   * Loop over each `set' in the configured direction until we are done.
   */

  p = nextset > 0 ? setinit - 1 : setinit;
  for (k = 1; k <= p; k++)
    {
      z = ( z * i-- ) / k;
    }

  for (curset = setinit; nextset > 0 ? curset <= setdone : curset >= setdone;
       curset += (uint32_t)nextset)
    {

      /*
       * Calculate number of combinations (incrementing)
       */

      if (nextset > 0)
        {
          z = ( z * i-- ) / k++;
        }

      /*
       * Cast number of combinations in set to integer
       */

      if (( ncombos = (uint64_t)z ) == 0)
        {
          FREE (setnums_backend);
          FREE (setnums);
          FREE (curitems);
          return errno = ERANGE;
        }

      /*
       * Jump to next set if requested start is beyond this one
       */

      if (doseek)
        {
          if (seek > ncombos)
            {
              seek -= ncombos;
              if (nextset < 0)
                {
                  z = ( z * --k ) / ++i;
                }

              continue;
            }
          else if (seek == 1)
            {
              doseek = FALSE;
            }
        }

      /*
       * Fill array with the initial positional arguments
       */

      for (n = 0; n < curset; n++)
        {
          curitems[n] = items[n];
        }

      /*
       * Produce results with the first set of items
       */

      if (!doseek)
        {
          retval = action(config, seq++, curset, curitems);
          if (retval != 0)
            {
              break;
            }

          if (docount && --count == 0)
            {
              break;
            }
        }

      /*
       * Prefill two arrays used for matrix calculations.
       *
       * The first array (setnums) is a linear sequence starting at
       * one (1) and ending at N (where N is the same integer as the
       * current set we're operating on). For example, if we are
       * currently on a set-of-2, setnums is 1, 2.
       *
       * The second array (setnums_backend) is a linear sequence
       * starting at nitems-N and ending at nitems (again, N is the
       * same integer as the current set we are operating on; nitems
       * is the total number of items). For example, if we are
       * operating on a set-of-2, and nitems is 8, setnums_backend is
       * set to 7, 8.
       */

      for (n = 0; n < curset; n++)
        {
          if (setnums == NULL) //-V547
            {
              (void)fprintf(stderr, "\rFATAL: Out of memory?! Aborting at %s[%s:%d]\r\n",
                            __func__, __FILE__, __LINE__);
              abort();
              /*NOTREACHED*/ /* unreachable */
            }
          setnums[n] = n;
        }

      p = 0;
      for (n = curset; n > 0; n--)
        {
          if (setnums_backend == NULL) //-V547
            {
              (void)fprintf(stderr, "\rFATAL: Out of memory?! Aborting at %s[%s:%d]\r\n",
                            __func__, __FILE__, __LINE__);
              abort();
              /*NOTREACHED*/ /* unreachable */
            }
          setnums_backend[p++] = nitems - n;
        }

      /*
       * Process remaining self-similar combinations in the set.
       */

      for (combo = 1; combo < ncombos; combo++)
        {
          setnums_last = curset;

          /*
           * Using self-similarity (matrix) theorem, determine
           * (by comparing the [sliding] setnums to the stored
           * setnums_backend) the number of arguments that remain
           * available for shifting into a new setnums value
           * (for later mapping into curitems).
           *
           * In essence, determine when setnums has slid into
           * setnums_backend in which case we can mathematically
           * use the last item to find the next-new item.
           */

          for (n = curset; n > 0; n--)
            {
              setpos = setnums[n - 1];
              setpos_backend = setnums_backend[n - 1];

              /*
               * If setpos is equal to or greater than
               * setpos_backend then we keep iterating over
               * the current set's list of argument positions
               * until otherwise; each time incrementing the
               * amount of numbers we must produce from
               * formulae rather than stored position.
               */

              setnums_last = n - 1;
              if (setpos < setpos_backend)
                {
                  break;
                }
            }

          /*
           * The next few stanzas are dedicated to rebuilding
           * the setnums array for mapping positional items
           * [immediately following] into curitems.
           */

          /*
           * Get the generator number used to populate unknown
           * positions in the matrix (using self-similarity).
           */

          seed = setnums[setnums_last];

          /*
           * Use the generator number to populate any position
           * numbers that weren't carried over from previous
           * combination run -- using self-similarity theorem.
           */

          for (n = setnums_last; n <= curset; n++)
            {
              setnums[n] = seed + n - setnums_last + 1;
            }

          /*
           * Now map new setnums into values stored in items
           */

          for (n = 0; n < curset; n++)
            {
              curitems[n] = items[setnums[n]];
            }

          /*
           * Produce results with this set of items
           */

          if (doseek)
            {
              seek--;
              if (seek == 1)
                {
                  doseek = FALSE;
                }
            }

          if (!doseek || seek == 1)
            {
              doseek = FALSE;
              retval = action(config, seq++, curset, curitems);
              if (retval != 0)
                {
                  goto cmb_return;
                }

              if (docount && --count == 0)
                {
                  goto cmb_return;
                }
            }
        } /* for combo */

      /*
       * Calculate number of combinations (decrementing)
       */

      if (nextset < 0)
        {
          z = ( z * --k ) / ++i;
        }
    } /* for curset */

  /*
   * Show the empty set consisting of a single combination of no-items
   */

  if (nextset < 0 && show_empty)
    {
      if (( !doseek || seek == 1 ) && ( !docount || count > 0 ))
        {
          retval = action(config, seq++, 0, NULL);
        }
    }

cmb_return:
  FREE(curitems);
  FREE(setnums);
  FREE(setnums_backend);

  return retval;
}

CMB_ACTION(cmb_print)
{
  uint8_t show_numbers  = FALSE;
  uint32_t n;
  const char *delimiter = " ";
  const char *prefix    = NULL;
  const char *suffix    = NULL;

  /*
   * Process config options
   */

  if (config != NULL)
    {
      if (config->delimiter != NULL)
        {
          delimiter = config->delimiter;
        }

      if (( config->options & CMB_OPT_NUMBERS ) != 0)
        {
          show_numbers = TRUE;
        }

      prefix = config->prefix;
      suffix = config->suffix;
    }

  if (show_numbers)
    {
      (void)fprintf(stdout, "%" PRIu64 " ", seq);
    }

  if (prefix != NULL)
    {
      (void)fprintf(stdout, "%s", prefix);
    }

  for (n = 0; n < nitems; n++)
    {
      (void)fprintf(stdout, "%s", items[n]);
      if (n < nitems - 1)
        {
          (void)fprintf(stdout, "%s", delimiter);
        }
    }

  if (suffix != NULL)
    {
      (void)fprintf(stdout, "%s", suffix);
    }

  (void)fprintf(stdout, "\n");

  return 0;
}

#if !defined(UINT_MAX)
# define UINT_MAX 0xFFFFFFFF
#endif /* if !defined(UINT_MAX) */

static char version[] = "3.9.5";

/*
 * Environment
 */

static char *pgm; /* set to argv[0] by main() */

/*
 * Globals
 */

static uint8_t opt_quiet    = FALSE;
static const char digit[11] = "0123456789";

#if !defined(__attribute__)
# define __attribute__(xyz) /* Ignore */
#endif /* if !defined(__attribute__) */

#if !defined(_Noreturn)
# define _Noreturn __attribute__ (( noreturn ))
#endif /* if !defined(_Noreturn) */

/*
 * Function prototypes
 */

static void _Noreturn cmb_usage(void);

static uint64_t cmb_rand_range(uint64_t range);

static CMB_ACTION(cmb_add);
static CMB_ACTION(cmb_div);
static CMB_ACTION(cmb_mul);
static CMB_ACTION(cmb_sub);

static CMB_ACTION(cmb_add_find);
static CMB_ACTION(cmb_div_find);
static CMB_ACTION(cmb_mul_find);
static CMB_ACTION(cmb_sub_find);

static size_t numlen(const char *s);

static size_t rangelen(const char *s, size_t nlen, size_t slen);

static size_t unumlen(const char *s);

static size_t urangelen(const char *s, size_t nlen, size_t slen);

static uint8_t parse_range(const char *s, uint32_t *min, uint32_t *max);

static uint8_t parse_unum(const char *s, uint32_t *n);

static uint8_t parse_urange(const char *s, uint32_t *min, uint32_t *max);

static uint32_t range_char(uint32_t start, uint32_t stop, uint32_t idx,
                           char *dst[]);

static uint32_t range_float(uint32_t start, uint32_t stop, uint32_t idx,
                            char *dst[]);

/*
 * Inline functions
 */

static inline uint32_t
hash32s(const void *buf, size_t len, uint32_t h)
{
  const unsigned char *p = buf;

  for (size_t i = 0; i < len; i++)
    h = h * 31 + p[i];

  h ^= h >> 17;
  h *= UINT32_C(0xed5ad4bb);
  h ^= h >> 11;
  h *= UINT32_C(0xac4c1b51);
  h ^= h >> 15;
  h *= UINT32_C(0x31848bab);
  h ^= h >> 14;

  return h;
}

static inline uint8_t
p2(uint64_t x)
{
  return x == ( x & -x );
}
static inline uint64_t
urand64(void)
{
  return ( (uint64_t)lrand48() << 42 ) + ( (uint64_t)lrand48() << 21 )
         + (uint64_t)lrand48();
}

/*
 * Transformations (-X op)
 */

struct cmb_xfdef
{
  const char *opname;
  CMB_ACTION(( *action ));
  CMB_ACTION(( *action_find ));
};

static struct cmb_xfdef cmb_xforms[] = {
  { "multiply", cmb_mul, cmb_mul_find },
  { "divide",   cmb_div, cmb_div_find },
  { "add",      cmb_add, cmb_add_find },
  { "subtract", cmb_sub, cmb_sub_find },
  { NULL,       NULL,    NULL         },
};

#if ( defined(__VERSION__) && defined(__GNUC__) ) || \
  ( defined(__VERSION__) && defined(__clang_version__) )
# if !defined(HAVE_BUILD)
#  define HAVE_BUILD
# endif /* if !defined(HAVE_BUILD) */
#endif /* if  ( defined(__VERSION__) && defined(__GNUC__) ) ||
           ( defined(__VERSION__) && defined(__clang_version__) */

#define XSTR_EMAXLEN 32767

static const char
*xstrerror_l(int errnum)
{
  int saved = errno;
  const char *ret = NULL;
  static /* __thread */ char buf[XSTR_EMAXLEN];

#if defined(__APPLE__) || defined(_AIX) || \
      defined(__MINGW32__) || defined(__MINGW64__) || \
        defined(CROSS_MINGW32) || defined(CROSS_MINGW64)
# if defined(__MINGW32__) || defined(__MINGW64__) || \
        defined(CROSS_MINGW32) || defined(CROSS_MINGW64)
  if (strerror_s(buf, sizeof(buf), errnum) == 0) ret = buf; /*LINTOK: xstrerror_l*/
# else
  if (strerror_r(errnum, buf, sizeof(buf)) == 0) ret = buf; /*LINTOK: xstrerror_l*/
# endif
#else
# if defined(__NetBSD__)
  locale_t loc = LC_GLOBAL_LOCALE;
# else
  locale_t loc = uselocale((locale_t)0);
# endif
  locale_t copy = loc;
  if (copy == LC_GLOBAL_LOCALE)
    copy = duplocale(copy);

  if (copy != (locale_t)0)
    {
      ret = strerror_l(errnum, copy); /*LINTOK: xstrerror_l*/
      if (loc == LC_GLOBAL_LOCALE)
        {
          freelocale(copy);
        }
    }
#endif

  if (!ret)
    {
      (void)snprintf(buf, sizeof(buf), "Unknown error %d", errnum);
      ret = buf;
    }

  errno = saved;
  return ret;
}

static locale_t locale;

static int
init_locale(void)
{
  locale = newlocale(LC_ALL_MASK, "", (locale_t)0);
  if (locale == (locale_t)0)
      return 0;
  return 1;
}

int
main(int argc, char *argv[])
{
  (void)setlocale(LC_ALL, "");
  (void)init_locale();
  uint8_t free_find         = FALSE;
  uint8_t opt_empty         = FALSE;
  uint8_t opt_find          = FALSE;
  uint8_t opt_precision     = FALSE;
  uint8_t opt_randi         = FALSE;
  uint8_t opt_range         = FALSE;
  uint8_t opt_total         = FALSE;
  uint8_t opt_version       = FALSE;
#if defined(HAVE_BUILD)
  uint8_t opt_build         = FALSE;
#endif /* if defined(HAVE_BUILD) */
  const char *cp            = NULL;
  char *cmdver              = version;
  char *endptr              = NULL;
  char **items              = NULL;
  char **items_tmp          = NULL;
  const char *libver        = libcmb_version(CMB_VERSION);
  char *opt_transform       = NULL;
  int ch                    = 0;
  int len                   = 0;
  int retval                = EXIT_SUCCESS;
  uint32_t i                = 0;
  uint32_t n                = 0;
  uint32_t nitems           = 0;
  uint32_t rstart           = 0;
  uint32_t rstop            = 0;
  size_t config_size        = sizeof ( struct cmb_config ) + 1;
  size_t optlen             = 0;
  struct cmb_config *config = NULL;
  struct cmb_xitem *xitem   = NULL;
  uint64_t count            = 0;
  uint64_t nstart           = 0; /* negative start */
  uint64_t ritems           = 0;
  uint64_t ull              = 0;
  unsigned long ul          = 0;
  struct timeval tv;

  pgm = argv[0]; /* store a copy of invocation name */

  /*
   * Allocate config structure
   */

  if (( config = malloc(config_size)) == NULL)
    {
      (void)fprintf(stderr, "\rFATAL: Out of memory?! Aborting at %s[%s:%d]\r\n",
                    __func__, __FILE__, __LINE__);
      abort();
      /*NOTREACHED*/ /* unreachable */
    }

#if !defined(bzero)
# define bzero(b,len) ((void)memset((b), '\0', (len)), (void) 0)
#endif /* if !defined(bzero) */

  bzero(config, sizeof ( struct cmb_config ));

  /*
   * Process command-line options
   */

#define OPTSTRING "c:d:eF:hi:k:NP:p:qrs:tVvX:"
  while (( ch = getopt(argc, argv, OPTSTRING)) != -1)
    {
      switch (ch)
        {
        case 'c': /* count */
          if (( optlen = strlen(optarg)) == 0 || unumlen(optarg) != optlen)
            {
              (void)fprintf(stderr, "Error: -c: %s `%s'\r\n",
                xstrerror_l(EINVAL), optarg);
              _Exit(EXIT_FAILURE);
              /*NOTREACHED*/ /* unreachable */
            }

          errno = 0;
          config->count = strtoull(optarg, (char **)NULL, 10);
          if (errno != 0)
            {
              (void)fprintf(stderr, "Error: -c: %s `%s'\r\n",
                xstrerror_l(errno), optarg);
              _Exit(EXIT_FAILURE);
              /*NOTREACHED*/ /* unreachable */
            }

          break;

        case 'd': /* delimiter */
          config->delimiter = optarg;
          break;

        case 'e': /* empty */
          opt_empty = TRUE;
          config->options ^= CMB_OPT_EMPTY;
          break;

        case 'F': /* find */
          opt_find = TRUE;
          if (cmb_transform_find != NULL)
            {
              FREE(cmb_transform_find);
            }

          if (( cmb_transform_find =
            malloc(sizeof ( struct cmb_xitem ))) == NULL)
            {
              (void)fprintf(stderr, "\rFATAL: Out of memory?! Aborting at %s[%s:%d]\r\n",
                            __func__, __FILE__, __LINE__);
              abort();
              /*NOTREACHED*/ /* unreachable */
            }

          cmb_transform_find->cp = optarg;
          endptr = NULL;
          errno = 0;
          cmb_transform_find->as.ld = strtold(optarg, &endptr);
          if (endptr == NULL || *endptr != '\0')
            {
              if (errno == 0)
                {
                  errno = EINVAL;
                }

              (void)fprintf(stderr, "Error: -F: %s `%s'\r\n",
                xstrerror_l(errno), optarg);
              _Exit(EXIT_FAILURE);
              /*NOTREACHED*/ /* unreachable */
            }

          break;

        case 'h': /* help */
          FREE(config);
          cmb_usage();
          /*NOTREACHED*/ /* unreachable */
          break;

        case 'i': /* start */
          if (( optlen = strlen(optarg)) > 0
              && strncmp("random", optarg, optlen) == 0)
            {
              opt_randi = TRUE;
            }
          else if (optlen == 0 || numlen(optarg) != optlen)
            {
              (void)fprintf(stderr, "Error: -i: %s `%s'\r\n",
                xstrerror_l(EINVAL), optarg);
              _Exit(EXIT_FAILURE);
              /*NOTREACHED*/ /* unreachable */
            }

          if (!opt_randi)
            {
              errno = 0;
              if (*optarg == '-')
                {
                  nstart = strtoull(&optarg[1], (char **)NULL, 10);
                }
              else
                {
                  config->start = strtoull(optarg, (char **)NULL, 10);
                }

              if (errno != 0)
                {
                  (void)fprintf(stderr, "Error: -i: %s `%s'\r\n",
                    xstrerror_l(errno), optarg);
                  _Exit(EXIT_FAILURE);
                  /*NOTREACHED*/ /* unreachable */
                }
            }

          break;

        case 'k': /* size */
          if (!parse_range(optarg, &( config->size_min ),
               &( config->size_max )))
            {
              (void)fprintf(stderr, "Error: -k: %s `%s'\r\n",
                xstrerror_l(errno), optarg);
              _Exit(EXIT_FAILURE);
              /*NOTREACHED*/ /* unreachable */
            }

          break;

        case 'N': /* numbers */
          config->options ^= CMB_OPT_NUMBERS;
          break;

        case 'P': /* precision */
          if (!parse_unum(optarg, (uint32_t *)&cmb_transform_precision))
            {
              (void)fprintf(stderr, "Error: -n: %s `%s'\r\n",
                xstrerror_l(errno), optarg);
              _Exit(EXIT_FAILURE);
              /*NOTREACHED*/ /* unreachable */
            }

          opt_precision = TRUE;
          break;

        case 'p': /* prefix */
          config->prefix = optarg;
          break;

        case 'q': /* quiet */
          opt_quiet = 1;
          break;

        case 'r': /* range */
          opt_range = TRUE;
          break;

        case 's': /* suffix */
          config->suffix = optarg;
          break;

        case 't': /* total */
          opt_total = TRUE;
          break;

#if defined(HAVE_BUILD)
        case 'V': /* build */
          opt_build = TRUE;
          break;
#endif /* if defined(HAVE_BUILD) */

        case 'v': /* version */
          opt_version = TRUE;
          break;

        case 'X': /* transform */
          opt_transform = optarg;
          break;

        default: /* unhandled argument (based on switch) */
          cmb_usage();
          /*NOTREACHED*/ /* unreachable */
        }
    }
  argc -= optind;
  argv += optind;

  /*
   * Process `-V' and '-v' command-line options
   */

  if (opt_version)
    {
      (void)fprintf(stdout, "mcmb: (miniature) combinatorics utility"
        " %s (cmb %s + %s)\r\n", mcmbver, cmdver, libver);
#if defined(HAVE_BUILD)
      if (!opt_build)
        {
#endif /* if defined(HAVE_BUILD) */
          FREE(config);
          exit(EXIT_SUCCESS);
          /*NOTREACHED*/ /* unreachable */
#if defined(HAVE_BUILD)
        }
#endif /* if defined(HAVE_BUILD) */
    }

#if defined(HAVE_BUILD)
  if (opt_build)
    {
# if defined(__VERSION__) && defined(__GNUC__)
#  if !defined (__clang_version__) || defined(__INTEL_COMPILER)
      char xcmp[2];
      /* cppcheck-suppress invalidPrintfArgType_s */
      (void)sprintf(xcmp, "%.1s", __VERSION__ );
      if (!isdigit((int)xcmp[0]))
        {
          /* cppcheck-suppress invalidPrintfArgType_s */
          (void)fprintf(stdout, "Compiler: %s\r\n", __VERSION__ );
        }
      else
        {
          /* cppcheck-suppress invalidPrintfArgType_s */
          (void)fprintf(stdout, "Compiler: GCC %s\r\n", __VERSION__ );
        }
#  else
      /* cppcheck-suppress invalidPrintfArgType_s */
      (void)fprintf(stdout, "Compiler: Clang %s\r\n", __clang_version__ );
#  endif /* if !defined (__clang_version__) || defined(__INTEL_COMPILER) */
# else
      /* cppcheck-suppress invalidPrintfArgType_s */
      (void)fprintf(stdout, "Compiler: %s\r\n", __VERSION__ );
# endif /* if defined(__VERSION__) && defined(__GNUC__) */
      FREE(config);
      exit(EXIT_SUCCESS);
      /*NOTREACHED*/ /* unreachable */
    }
#endif /* if defined(HAVE_BUILD) */

  /*
   * At least one non-option argument is required unless `-e' is given
   */

  if (argc == 0 && !opt_empty)
    {
      cmb_usage();
      /*NOTREACHED*/ /* unreachable */
    }

  /*
   * `-X op' required if given `-F num'
   */

  if (opt_find && opt_transform == NULL)
    {
      (void)fprintf(stderr, "Error: `-X op' required when using `-F num'\r\n");
      _Exit(EXIT_FAILURE);
      /*NOTREACHED*/ /* unreachable */
    }

  /*
   * `-X op' required if given `-P num'
   */

  if (opt_precision && opt_transform == NULL)
    {
      (void)fprintf(stderr, "Error: `-X op' required when using `-P num'\r\n");
      _Exit(EXIT_FAILURE);
      /*NOTREACHED*/ /* unreachable */
    }

  /*
   * Calculate number of items
   */

  if (nitems == 0 || nitems > (uint32_t)argc) //-V560
    {
      nitems = (uint32_t)argc;
    }

  if (opt_range)
    {
      for (n = 0; n < nitems; n++)
        {
          if (!parse_urange(argv[n], &rstart, &rstop))
            {
              (void)fprintf(stderr, "Error: -r: %s `%s'\r\n",
                xstrerror_l(errno), argv[n]);
              _Exit(EXIT_FAILURE);
              /*NOTREACHED*/ /* unreachable */
            }

          if (unumlen(argv[n]) == strlen(argv[n]))
            {
              rstop = rstart;
              rstart = 1;
            }

          if (rstart < rstop)
            {
              ull = rstop - rstart + 1;
            }
          else
            {
              ull = rstart - rstop + 1;
            }

          if (ritems + ull > UINT_MAX)
            {
              (void)fprintf(stderr, "Error: -r: Too many items\r\n");
              _Exit(EXIT_FAILURE);
              /*NOTREACHED*/ /* unreachable */
            }

          ritems += ull;
        }
    }

  /*
   * Print total for num items and exit if given `-t -r'
   */

  if (opt_total && opt_range)
    {
      count = cmb_count(config, (uint32_t)ritems);

      if (errno)
        {
          (void)fprintf(stderr, "Error: %s (%d)\r\n", xstrerror_l(errno), errno);
          /*NOTREACHED*/ /* unreachable */
        }

      (void)fprintf(stdout, "%" PRIu64 "%s", count, "\n");
      FREE(config);
      exit(EXIT_SUCCESS);
      /*NOTREACHED*/ /* unreachable */
    }

  /*
   * Read arguments ...
   */

  if (opt_range)
    {

      /*
       * ... as a series of ranges if given `-r'
       */

      if (( items = calloc(ritems, sizeof ( char * ))) == NULL)
        {
          (void)fprintf(stderr, "\rFATAL: Out of memory?! Aborting at %s[%s:%d]\r\n",
                        __func__, __FILE__, __LINE__);
          abort();
          /*NOTREACHED*/ /* unreachable */
        }

      i = 0;
      for (n = 0; n < nitems; n++)
        {
          (void)parse_urange(argv[n], &rstart, &rstop);
          if (unumlen(argv[n]) == strlen(argv[n]))
            {
              rstop = rstart;
              rstart = 1;
            }

          if (opt_transform != NULL)
            {
              i = range_float(rstart, rstop, i, items);
            }
          else
            {
              i = range_char(rstart, rstop, i, items);
            }
        }

      nitems = (uint32_t)ritems;
    }
  else
    {

      /*
       * ... as a series of strings or numbers if given `-X op'
       */

      items = argv;
    }

  /*
   * Transforms (-X op).
   */

  if (opt_transform != NULL)
    {
      if (( optlen = strlen(opt_transform)) == 0)
        {
          (void)fprintf(stderr, "Error: -X %s\r\n", xstrerror_l(EINVAL));
          _Exit(EXIT_FAILURE);
          /*NOTREACHED*/ /* unreachable */
        }

      ch = -1; //-V1048
      while (( cp = cmb_xforms[++ch].opname ) != NULL)
        {
          if (strncmp(cp, opt_transform, optlen) != 0)
            {
              continue;
            }

          if (opt_find)
            {
              config->action = cmb_xforms[ch].action_find;
            }
          else
            {
              config->action = cmb_xforms[ch].action;
            }

          break;
        }
      if (config->action == NULL)
        {
          (void)fprintf(stderr, "Error: -X: %s `%s'\r\n",
            xstrerror_l(EINVAL), opt_transform);
          _Exit(EXIT_FAILURE);
          /*NOTREACHED*/ /* unreachable */
        }

      /*
       * Convert items into array of struct pointers
       */

      if (!opt_range)
        {
          ul = sizeof ( struct cmb_xitem * );
          if (( items_tmp = calloc(nitems, ul)) == NULL)
            {
              (void)fprintf(stderr, "\rFATAL: Out of memory?! Aborting at %s[%s:%d]\r\n",
                            __func__, __FILE__, __LINE__);
              abort();
              /*NOTREACHED*/ /* unreachable */
            }

          for (n = 0; n < nitems; n++)
            {
              if (( xitem = malloc(sizeof ( struct cmb_xitem ))) == NULL)
                {
                  (void)fprintf(stderr, "\rFATAL: Out of memory?! Aborting at %s[%s:%d]\r\n",
                                __func__, __FILE__, __LINE__);
                  abort();
                  /*NOTREACHED*/ /* unreachable */
                }

              xitem->cp = items[n];
              endptr = NULL;
              errno = 0;
              xitem->as.ld = strtold(items[n], &endptr);
              if (endptr == NULL || *endptr != '\0')
                {
                  if (errno == 0)
                    {
                      errno = EINVAL;
                    }

                  (void)fprintf(stderr, "Error: -X: %s `%s'\r\n",
                    xstrerror_l(errno), items[n]);
                  _Exit(EXIT_FAILURE);
                  /*NOTREACHED*/ /* unreachable */
                }

              items_tmp[n] = (char *)xitem;
              if (opt_precision)
                {
                  continue;
                }

              if (( cp = strchr(items[n], '.')) != NULL)
                {
                  len = (int)strlen(items[(long)n]);
                  len -= (cp - items[(long)n] + 1);
                  if (len > cmb_transform_precision)
                    {
                      cmb_transform_precision = len;
                    }
                }
            }

          items = items_tmp;
        }
    }

  /*
   * Adjust precision based on `-F num'
   */

  if (opt_find)
    {
      if (( cp = strchr(cmb_transform_find->cp, '.')) != NULL)
        {
          len = (int)strlen(cmb_transform_find->cp);
          len -= cp - (cmb_transform_find->cp + 1);
          if (len > cmb_transform_precision)
            {
              cmb_transform_precision = len;
            }
          else
            {
              len = snprintf(
                NULL,
                0,
                "%.*Lf",
                cmb_transform_precision,
                cmb_transform_find->as.ld)
                    + 1;
              cmb_transform_find->cp = malloc((unsigned long)len);
              if (cmb_transform_find->cp == NULL)
                {
                  (void)fprintf(stderr, "\rFATAL: Out of memory?! Aborting at %s[%s:%d]\r\n",
                                __func__, __FILE__, __LINE__);
                  abort();
                  /*NOTREACHED*/ /* unreachable */
                }

              free_find = TRUE;
              (void)sprintf(
                cmb_transform_find->cp,
                "%.*Lf",
                cmb_transform_precision,
                cmb_transform_find->as.ld);
            }
        }
      else if (cmb_transform_precision > 0)
        {
          len = snprintf(
            NULL,
            0,
            "%.*Lf",
            cmb_transform_precision,
            cmb_transform_find->as.ld);
          cmb_transform_find->cp = malloc((unsigned long)len);
          if (cmb_transform_find->cp == NULL)
            {
              (void)fprintf(stderr, "\rFATAL: Out of memory?! Aborting at %s[%s:%d]\r\n",
                            __func__, __FILE__, __LINE__);
              abort();
              /*NOTREACHED*/ /* unreachable */
            }

          free_find = TRUE;
          (void)sprintf(
            cmb_transform_find->cp,
            "%.*Lf",
            cmb_transform_precision,
            cmb_transform_find->as.ld);
        }
    }

  /*
   * Calculate combinations
   */

  if (opt_total)
    {
      count = cmb_count(config, nitems);

      if (errno)
        {
          (void)fprintf(stderr, "\rFATAL: %s (%d)\r\n", xstrerror_l(errno), errno);
          _Exit(EXIT_FAILURE);
          /*NOTREACHED*/ /* unreachable */
        }

      (void)fprintf(stdout, "%" PRIu64 "%s", count, "\n");
    }
  else
    {
      if (opt_randi)
        {
          count = cmb_count(config, nitems);
          if (errno)
            {
              (void)fprintf(stderr, "\rFATAL: %s (%d)\r\n", xstrerror_l(errno), errno);
              _Exit(EXIT_FAILURE);
              /*NOTREACHED*/ /* unreachable */
            }

          if (gettimeofday(&tv, NULL) == 0)
            {
              uint32_t h = 0;  /* initial hash value */
#if __STDC_VERSION__ < 201112L
              /* LINTED E_OLD_STYLE_FUNC_DECL */
              void *(*mallocptr)() = malloc;
              h = hash32s(&mallocptr, sizeof(mallocptr), h);
#endif /* if __STDC_VERSION__ < 201112L */
              void *small = malloc(1);
              h = hash32s(&small, sizeof(small), h);
              FREE(small);
              void *big = malloc(1UL << 20);
              h = hash32s(&big, sizeof(big), h);
              FREE(big);
              void *ptr = &ptr;
              h = hash32s(&ptr, sizeof(ptr), h);
              time_t t = time(0);
              h = hash32s(&t, sizeof(t), h);
#if !defined(_AIX)
              for (int i = 0; i < 1000; i++)
                {
                  unsigned long counter = 0;
                  clock_t start = clock();
                  while (clock() == start)
                    {
                      counter++;
                    }
                  h = hash32s(&start, sizeof(start), h);
                  h = hash32s(&counter, sizeof(counter), h);
                }
#endif
              int mypid = (int)getpid();
              h = hash32s(&mypid, sizeof(mypid), h);
              char rnd[4];
              FILE *f = fopen("/dev/urandom", "rb");
              if (f)
                {
                  if (fread(rnd, sizeof(rnd), 1, f))
                    {
                      h = hash32s(rnd, sizeof(rnd), h);
                    }
                }
              srand48((long)(h));
              config->start = cmb_rand_range(count) + 1;
            }
          else
            {
              (void)fprintf(stderr, "\rFATAL: %s (%d)\r\n", xstrerror_l(errno), errno);
              _Exit(EXIT_FAILURE);
              /*NOTREACHED*/ /* unreachable */
            }
        }
      else if (nstart != 0)
        {
          count = cmb_count(config, nitems);
          if (errno)
            {
              (void)fprintf(stderr, "\rFATAL: %s (%d)\r\n", xstrerror_l(errno), errno);
              _Exit(EXIT_FAILURE);
              /*NOTREACHED*/ /* unreachable */
            }

          if (count > nstart)
            {
              config->start = count - nstart + 1;
            }
          else
            {
              config->start = 0;
            }
        }

      retval = cmb(config, nitems, items);
      if (errno)
        {
          (void)fprintf(stderr, "\rFATAL: %s (%d)\r\n", xstrerror_l(errno), errno);
          _Exit(EXIT_FAILURE);
          /*NOTREACHED*/ /* unreachable */
        }
    }

  /*
   * Clean up
   */

  if (opt_range)
    {
      for (n = 0; n < nitems; n++)
        {
          FREE(items[n]);
        }

      FREE(items);
    }
  else if (opt_transform)
    {
      for (n = 0; n < nitems; n++)
        {
          (void)memcpy(&xitem, &items[n], sizeof ( char * ));
          if (opt_range) //-V547
            {
              FREE(xitem->cp);
            }

          FREE(xitem);
        }

      FREE(items);
    }

  if (opt_find)
    {
      if (free_find)
        {
          FREE(cmb_transform_find->cp);
        }

      FREE(cmb_transform_find);
      if (cmb_transform_find_buf != NULL)
        {
          FREE(cmb_transform_find_buf);
        }
    }

  FREE(config);

  return retval;
}

/*
 * Print short usage statement to stderr and exit with error status.
 */

#define OPTFMT    "  %-10s  %s\r\n"
#define OPTFMT_1U "  %-10s  %s%u%s\r\n"

static void
cmb_usage(void)
{
  (void)fprintf(stderr,
    "Usage: %s { [SWITCHES] } { <ITEMS> ... }\n\n", pgm);
  (void)fprintf(stderr,
    " Switches:\r\n");
  (void)fprintf(stderr, OPTFMT,
    "-c num", "Produce num combinations (defaults to '0' for all)");
  (void)fprintf(stderr, OPTFMT,
    "-d text", "Item delimiter (default is ' ')");
  (void)fprintf(stderr, OPTFMT,
    "-e", "Include the empty set with no items");
  (void)fprintf(stderr, OPTFMT,
    "-F num", "Find '-X op' results matching num");
  (void)fprintf(stderr, OPTFMT,
    "-h", "Print this help text and exit");
  (void)fprintf(stderr, OPTFMT,
    "-i num", "Skip the first num minus one combinations");
  (void)fprintf(stderr, OPTFMT,
    "-k size", "Number or range ('min..max' or 'min-max') of items");
  (void)fprintf(stderr, OPTFMT,
    "-N", "Show combination sequence numbers");
  (void)fprintf(stderr, OPTFMT,
    "-P num", "Set floating point precision when given '-X op'");
  (void)fprintf(stderr, OPTFMT,
    "-p text", "Prefix text for each line of output");
  (void)fprintf(stderr, OPTFMT,
    "-q", "Quiet mode; do not print item(s) from set when given '-X op'");
  (void)fprintf(stderr, OPTFMT_1U,
    "-r", "Treat arguments as ranges (of up-to ", UINT_MAX, " items)");
  (void)fprintf(stderr, OPTFMT,
    "-s text", "Suffix text for each line of output");
  (void)fprintf(stderr, OPTFMT,
    "-t", "Print total number of combinations and exit");
#if defined(HAVE_BUILD)
  (void)fprintf(stderr, OPTFMT,
    "-V", "Print build information and exit");
#endif /* if defined(HAVE_BUILD) */
  (void)fprintf(stderr, OPTFMT,
    "-v", "Print version information and exit");
  (void)fprintf(stderr, OPTFMT,
    "-X op", "Perform math on item(s) where 'op' is add, sub, div, or mul");

  _Exit(EXIT_FAILURE);
}

/*
 * Return pseudo-random 64-bit unsigned integer in range 0 <= return <= range.
 */

static uint64_t
cmb_rand_range(uint64_t range)
{
  static const uint64_t M = ( uint64_t ) ~0;

  if (range == 0)
    {
      (void)fprintf(stderr, "\rFATAL: Range cannot be zero!\r\n");
      _Exit(EXIT_FAILURE);
      /*NOTREACHED*/ /* unreachable */
    }

  uint64_t exclusion = p2(range) ? 0 : M % range + 1;
  uint64_t res = 0;

  while ( ( res = urand64() ) < exclusion )
    {
      (void)0; /* NULL */
    }
  return res % range;
}

static size_t
numlen(const char *s)
{
  if (s == NULL || *s == '\0')
    {
      return 0;
    }
  else if (*s == '-')
    {
      return strspn(&s[1], digit) + 1;
    }
  return strspn(s, digit);
}

static size_t
rangelen(const char *s, size_t nlen, size_t slen)
{
  size_t rlen;
  const char *cp = s;

  if (nlen == 0)
    {
      return 0;
    }
  else if (nlen == slen)
    {
      return nlen;
    }

  cp += nlen;
  if (*cp == '-')
    {
      cp += 1;
      if (*cp == '\0')
        {
          return 0;
        }

      return nlen + strspn(cp, digit) + 1;
    }
  else if (strncmp(cp, "..", 2) == 0)
    {
      cp += 2;
      if (( rlen = numlen(cp)) == 0)
        {
          return 0;
        }

      return nlen + rlen + 2;
    }
  return 0;
}

static uint8_t
parse_range(const char *s, uint32_t *min, uint32_t *max)
{
  const char *cp;
  size_t nlen;
  size_t optlen;
  uint64_t ull;

  errno = 0;

  if (s == NULL
      || (( nlen = numlen(s)) != ( optlen = strlen(s))
          && rangelen(s, nlen, optlen) != optlen ))
    {
      errno = EINVAL;
      return FALSE;
    }

  if (*s == '-')
    {
      ull = strtoull(&s[1], (char **)NULL, 10);
      *min = UINT_MAX - (uint32_t)ull;
    }
  else
    {
      ull = strtoull(s, (char **)NULL, 10);
      *min = (uint32_t)ull;
    }

  if (errno != 0)
    {
      return FALSE;
    }
  else if (ull > UINT_MAX)
    {
      errno = ERANGE;
      return FALSE;
    }

  cp = &s[nlen];
  if (*cp == '\0')
    {
      *max = *s == '-' ? (uint32_t)ull : *min;
    }
  else if (( strncmp(cp, "..", 2)) == 0)
    {
      cp += 2;
      if (*cp == '-')
        {
          ull = strtoull(&cp[1], (char **)NULL, 10);
          *max = UINT_MAX - (uint32_t)ull;
        }
      else
        {
          ull = strtoull(cp, (char **)NULL, 10);
          *max = (uint32_t)ull;
        }

      if (errno != 0)
        {
          return FALSE;
        }
      else if (ull > UINT_MAX)
        {
          errno = ERANGE;
          return FALSE;
        }
    }
  else if (*cp == '-')
    {
      ull = strtoull(&cp[1], (char **)NULL, 10);
      if (errno != 0)
        {
          return FALSE;
        }
      else if (ull > UINT_MAX)
        {
          errno = ERANGE;
          return FALSE;
        }

      *max = (uint32_t)ull;
    }
  else
    {
      errno = EINVAL;
      return FALSE;
    }

  return TRUE;
}

static size_t
unumlen(const char *s)
{
  if (s == NULL || *s == '\0')
    {
      return 0;
    }
  return strspn(s, digit);
}

static size_t
urangelen(const char *s, size_t nlen, size_t slen)
{
  size_t rlen;
  const char *cp = s;

  if (nlen == 0)
    {
      return 0;
    }
  else if (nlen == slen)
    {
      return nlen;
    }

  cp += nlen;
  if (*cp == '-')
    {
      cp += 1;
      if (*cp == '\0')
        {
          return 0;
        }

      return nlen + strspn(cp, digit) + 1;
    }
  else if (strncmp(cp, "..", 2) == 0)
    {
      cp += 2;
      if (( rlen = unumlen(cp)) == 0)
        {
          return 0;
        }

      return nlen + rlen + 2;
    }
  return 0;
}

static uint8_t
parse_unum(const char *s, uint32_t *n)
{
  uint64_t ull;

  errno = 0;

  if (s == NULL || unumlen(s) != strlen(s))
    {
      errno = EINVAL;
      return FALSE;
    }

  ull = strtoull(optarg, (char **)NULL, 10);
  if (errno != 0)
    {
      return FALSE;
    }
  else if (ull > UINT_MAX)
    {
      errno = ERANGE;
      return FALSE;
    }

  *n = (uint32_t)ull;

  return TRUE;
}

static uint8_t
parse_urange(const char *s, uint32_t *min, uint32_t *max)
{
  const char *cp;
  size_t nlen;
  size_t optlen;
  uint64_t ull;

  errno = 0;

  if (s == NULL
      || (( nlen = unumlen(s)) != ( optlen = strlen(s))
          && urangelen(s, nlen, optlen) != optlen )
      || *s == '-')
    {
      errno = EINVAL;
      return FALSE;
    }

  ull = strtoull(s, (char **)NULL, 10);
  *min = *max = (uint32_t)ull;
  if (errno != 0)
    {
      return FALSE;
    }
  else if (ull > UINT_MAX)
    {
      errno = ERANGE;
      return FALSE;
    }

  cp = &s[nlen];
  if (*cp == '\0')
    {
      *max = *min; //-V1048
    }
  else if (( strncmp(cp, "..", 2)) == 0)
    {
      cp += 2;
      ull = strtoull(cp, (char **)NULL, 10);
      *max = (uint32_t)ull;
      if (errno != 0)
        {
          return FALSE;
        }
      else if (ull > UINT_MAX)
        {
          errno = ERANGE;
          return FALSE;
        }
    }
  else if (*cp == '-')
    {
      ull = strtoull(&cp[1], (char **)NULL, 10);
      if (errno != 0)
        {
          return FALSE;
        }
      else if (ull > UINT_MAX)
        {
          errno = ERANGE;
          return FALSE;
        }

      *max = (uint32_t)ull;
    }
  else
    {
      errno = EINVAL;
      return FALSE;
    }

  return TRUE;
}

static uint32_t
range_char(uint32_t start, uint32_t stop, uint32_t idx, char *dst[])
{
  int len;
  uint32_t num;

  if (start <= stop)
    {
      for (num = start; num <= stop; num++)
        {
          len = snprintf(NULL, 0, "%u", num) + 1;
          if (( dst[idx] = malloc((unsigned long)len)) == NULL)
            {
              (void)fprintf(stderr, "\rFATAL: Out of memory?! Aborting at %s[%s:%d]\r\n",
                            __func__, __FILE__, __LINE__);
              abort();
              /*NOTREACHED*/ /* unreachable */
            }

          (void)sprintf(dst[idx], "%u", num);
          idx++;
        }
    }
  else
    {
      for (num = start; num >= stop; num--)
        {
          len = snprintf(NULL, 0, "%u", num) + 1;
          if (( dst[idx] = (char *)malloc((unsigned long)len)) == NULL)
            {
              (void)fprintf(stderr, "\rFATAL: Out of memory?! Aborting at %s[%s:%d]\r\n",
                            __func__, __FILE__, __LINE__);
              abort();
              /*NOTREACHED*/ /* unreachable */
            }

          (void)sprintf(dst[idx], "%u", num);
          idx++;
        }
    }

  return idx;
}

static uint32_t
range_float(uint32_t start, uint32_t stop, uint32_t idx, char *dst[])
{
  int len;
  uint32_t num;
  size_t size;
  struct cmb_xitem *xitem;

  size = sizeof ( struct cmb_xitem );
  if (start <= stop)
    {
      for (num = start; num <= stop; num++)
        {
          if (( xitem = malloc(size)) == NULL)
            {
              (void)fprintf(stderr, "\rFATAL: Out of memory?! Aborting at %s[%s:%d]\r\n",
                            __func__, __FILE__, __LINE__);
              abort();
              /*NOTREACHED*/ /* unreachable */
            }

          len = snprintf(NULL, 0, "%u", num) + 1;
          if (( xitem->cp = malloc((unsigned long)len)) == NULL)
            {
              (void)fprintf(stderr, "\rFATAL: Out of memory?! Aborting at %s[%s:%d]\r\n",
                            __func__, __FILE__, __LINE__);
              abort();
              /*NOTREACHED*/ /* unreachable */
            }

          (void)sprintf(xitem->cp, "%u", num);
          xitem->as.ld = (long double)num;
          dst[idx] = (char *)xitem;
          idx++;
        }
    }
  else
    {
      for (num = start; num >= stop; num--)
        {
          if (( xitem = malloc(size)) == NULL)
            {
              (void)fprintf(stderr, "\rFATAL: Out of memory?! Aborting at %s[%s:%d]\r\n",
                            __func__, __FILE__, __LINE__);
              abort();
              /*NOTREACHED*/ /* unreachable */
            }

          len = snprintf(NULL, 0, "%u", num) + 1;
          if (( xitem->cp = malloc((unsigned long)len)) == NULL)
            {
              (void)fprintf(stderr, "\rFATAL: Out of memory?! Aborting at %s[%s:%d]\r\n",
                            __func__, __FILE__, __LINE__);
              abort();
              /*NOTREACHED*/ /* unreachable */
            }

          (void)sprintf(xitem->cp, "%u", num);
          xitem->as.ld = (long double)num;
          dst[idx] = (char *)xitem;
          idx++;
        }
    }

  return idx;
}

/*
 * Transformation functions
 */

static CMB_TRANSFORM_OP(*, cmb_mul)
static CMB_TRANSFORM_OP(/, cmb_div)
static CMB_TRANSFORM_OP(+, cmb_add)
static CMB_TRANSFORM_OP(-, cmb_sub)

/*
 * Find transformation functions
 */

static CMB_TRANSFORM_OP_FIND(*, cmb_mul_find)
static CMB_TRANSFORM_OP_FIND(/, cmb_div_find)
static CMB_TRANSFORM_OP_FIND(+, cmb_add_find)
static CMB_TRANSFORM_OP_FIND(-, cmb_sub_find)

// vim: filetype=c:tabstop=4:ai:expandtab
