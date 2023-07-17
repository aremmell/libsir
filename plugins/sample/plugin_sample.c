/*
 * plugin_sample.c
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
#include "plugin_sample.h"
#include <stdio.h>
#include "sir/ansimacros.h"
#include "sir/helpers.h"

#if defined(__WIN__)
BOOL APIENTRY DllMain(HMODULE module, DWORD ul_reason_for_call, LPVOID reserved)
{
    _SIR_UNUSED(module);
    _SIR_UNUSED(reserved);
# if defined(SIR_DEBUG)
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            OutputDebugStringA("Got DLL_PROCESS_ATTACH\n");
            break;
        case DLL_THREAD_ATTACH:
            OutputDebugStringA("Got DLL_THREAD_ATTACH\n");
            break;
        case DLL_THREAD_DETACH:
            OutputDebugStringA("Got DLL_THREAD_DETACH\n");
            break;
        case DLL_PROCESS_DETACH:
            OutputDebugStringA("Got DLL_PROCESS_DETACH\n");
            break;
    }
# else
    _SIR_UNUSED(ul_reason_for_call);
# endif

    return TRUE;
}
#endif

const uint8_t maj_ver   = 1;
const uint8_t min_ver   = 0;
const uint8_t bld_ver   = 0;
const sir_levels levels = SIRL_DEBUG | SIRL_INFO;
const sir_options opts  = SIRO_NOHOST | SIRO_NOTID;
const char* author      = "libsir contributors";
const char* desc        = "Logs messages and function calls to stdout.";
const uint64_t caps     = 0;

PLUGIN_EXPORT bool sir_plugin_query(sir_plugininfo* info) {
    info->iface_ver = SIR_PLUGIN_VCURRENT;
    info->maj_ver   = maj_ver;
    info->min_ver   = min_ver;
    info->bld_ver   = bld_ver;
    info->levels    = levels;
    info->opts      = opts;
    info->author    = author;
    info->desc      = desc;
    info->caps      = caps;

    printf("\t" DGRAY("sample_plugin ('%s')") "\n", __func__);
    return true;
}

PLUGIN_EXPORT bool sir_plugin_init(void) {
    printf("\t" DGRAY("sample_plugin ('%s')") "\n", __func__);
    return true;
}

PLUGIN_EXPORT bool sir_plugin_write(sir_level level, const char* message) {
    printf("\t" DGRAY("sample_plugin (%s): level: %04"PRIx32", message: %s") "\n",
        __func__, level, message);
    return true;
}

PLUGIN_EXPORT bool sir_plugin_cleanup(void) { //-V524
    printf("\t" DGRAY("sample_plugin ('%s')") "\n", __func__);
    return true;
}
