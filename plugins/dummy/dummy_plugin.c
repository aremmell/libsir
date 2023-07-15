/*
 * dummy_plugin.c
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
#include "dummy_plugin.h"
#include <siransimacros.h>
#include <sirhelpers.h>
#include <stdio.h>

/*
 * Controlling misbehavior via preprocessor macros:
 *
 * When the following are defined upon compilation, this plugin will exhibit
 * behavior that will cause libsir to reject it during the loading/validation
 * process. Only one at a time need be defined, since one failed check will result
 * in the loader immediately rejecting and unloading the module.
 *
 * - DUMMYPLUGIN_BADBEHAVIOR1: return false from 'sir_plugin_query'
 * - DUMMYPLUGIN_BADBEHAVIOR2: set info::iface_ver != SIR_PLUGIN_VCURRENT
 * - DUMMYPLUGIN_BADBEHAVIOR3: set info::levels and/or info::opts to invalid values
 * - DUMMYPLUGIN_BADBEHAVIOR4: missing an export
 */

#if defined(__WIN__)
BOOL APIENTRY DllMain(HMODULE module, DWORD ul_reason_for_call, LPVOID reserved)
{
    _SIR_UNUSED(module);
    _SIR_UNUSED(reserved);
#if defined(SIR_DEBUG)
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
#else
    _SIR_UNUSED(ul_reason_for_call);
#endif

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
#if defined(DUMMYPLUGIN_BADBEHAVIOR2)
    info->iface_ver = 255;
#else
    info->iface_ver = SIR_PLUGIN_VCURRENT;
#endif
    info->maj_ver   = maj_ver;
    info->min_ver   = min_ver;
    info->bld_ver   = bld_ver;
#if defined(DUMMYPLUGIN_BADBEHAVIOR3)
    info->levels    = 0xfe23;
    info->opts      = 0x1234abcd;
#else
    info->levels    = levels;
    info->opts      = opts;
#endif
    info->author    = author;
    info->desc      = desc;
    info->caps      = caps;

    printf("\t" DGRAY("dummy_plugin ('%s')") "\n", __func__);

#if defined(DUMMYPLUGIN_BADBEHAVIOR1)
    return false;
#else
    return true;
#endif
}

#if !defined(DUMMYPLUGIN_BADBEHAVIOR4)
PLUGIN_EXPORT bool sir_plugin_init(void) {
    printf("\t" DGRAY("dummy_plugin ('%s')") "\n", __func__);
    return true;
}
#endif

PLUGIN_EXPORT bool sir_plugin_write(sir_level level, const char* message) {
    printf("\t" DGRAY("dummy_plugin (%s): level: %04"PRIx32", message: '%s'") "\n",
        __func__, level, message);
    return true;
}

PLUGIN_EXPORT bool sir_plugin_cleanup(void) {
    printf("\t" DGRAY("dummy_plugin ('%s')") "\n", __func__);
    return true;
}
