/*
 * version.h
 *
 * Version: 2.2.6
 *
 * -----------------------------------------------------------------------------
 *
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2018-2024 Ryan M. Lederman <lederman@gmail.com>
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

#ifndef _SIR_VERSION_H_INCLUDED
# define _SIR_VERSION_H_INCLUDED

# include <stdint.h>

/** The current libsir major version component. */
# define SIR_VERSION_MAJOR 2

/** The current libsir minor version component. */
# define SIR_VERSION_MINOR 2

/** The current libsir patch version component. */
# define SIR_VERSION_PATCH 6

/** 1 if this is a release version of libsir, 0 otherwise. */
# define SIR_VERSION_IS_RELEASE 0

/** The current libsir version suffix. */
# define SIR_VERSION_SUFFIX "-dev"

/** The current libsir version as a number. */
# define SIR_VERSION_HEX ((SIR_VERSION_MAJOR << 16) | \
                          (SIR_VERSION_MINOR <<  8) | \
                          (SIR_VERSION_PATCH))

# define _SIR_VER2STR(x) #x
# define _SIR_MK_VER_STR(maj, min, patch) \
    _SIR_VER2STR(maj) "." _SIR_VER2STR(min) "." _SIR_VER2STR(patch) SIR_VERSION_SUFFIX

#endif /* !_SIR_VERSION_H_INCLUDED */
