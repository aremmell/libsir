/*
 * plugin_dummy.h
 *
 * Version: 2.2.6
 *
 * -----------------------------------------------------------------------------
 *
 * SPDX-License-Identifier: MIT
 *
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

#ifndef _SIR_PLUGIN_DUMMY_H_INCLUDED
# define _SIR_PLUGIN_DUMMY_H_INCLUDED

# include "sir/platform.h"
# include "sir/types.h"

# if defined(__WIN__)
BOOL APIENTRY DllMain(HMODULE module, DWORD ul_reason_for_call, LPVOID reserved);
#  define PLUGIN_EXPORT __declspec(dllexport)
# else
#  define PLUGIN_EXPORT
# endif

PLUGIN_EXPORT bool sir_plugin_query(sir_plugininfo* info);
PLUGIN_EXPORT bool sir_plugin_init(void);
PLUGIN_EXPORT bool sir_plugin_write(sir_level level, const char* message);
PLUGIN_EXPORT bool sir_plugin_cleanup(void);

#endif /* !_SIR_PLUGIN_DUMMY_H_INCLUDED */
