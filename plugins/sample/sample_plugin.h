/*
 * sample_plugin.h
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2023
 * Version:   2.2.0
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
#ifndef _SIR_SAMPLE_PLUGIN_H_INCLUDED
# define _SIR_SAMPLE_PLUGIN_H_INCLUDED

#include <sirplatform.h>
#include <sirtypes.h>

#if defined(__WIN__)
BOOL APIENTRY DllMain(HMODULE module, DWORD ul_reason_for_call, LPVOID reserved);
#define PLUGIN_EXPORT __declspec(dllexport)
#else
#define PLUGIN_EXPORT
#endif

PLUGIN_EXPORT bool sir_plugin_query(sir_plugininfo* info);
PLUGIN_EXPORT bool sir_plugin_init(void);
PLUGIN_EXPORT bool sir_plugin_write(sir_level level, const char* message);
PLUGIN_EXPORT bool sir_plugin_cleanup(void);

#endif // ! _SIR_SAMPLE_PLUGIN_H_INCLUDED
