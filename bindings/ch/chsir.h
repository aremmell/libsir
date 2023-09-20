/*
 * chsir.h
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2023
 * Version:   2.2.4
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
#ifndef _CHSIR_H_INCLUDED
# define _CHSIR_H_INCLUDED

# if defined(_CH_) || defined(__CH__)

#  include <dlfcn.h>

extern void* _Chsir_handle = dlopen("libsir.dl", RTLD_LAZY);

#  pragma exec _Chsir_handle ? 0 : \
    fprintf(_stderr, "Error: dlopen(): %s\n", dlerror()), \
    fprintf(_stderr, "       cannot get _Chsir_handle in %s:%d\n", \
            __FILE__, __LINE__);

void
_dlclose_sir(void) {
    dlclose(_Chsir_handle);
}

atexit(_dlclose_sir);

# endif

# include "../../include/sir.h"

#endif /* _CHSIR_H_INCLUDED */
