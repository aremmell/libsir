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

#if defined(__WIN__)
BOOL APIENTRY DllMain(HMODULE module, DWORD ul_reason_for_call, LPVOID reserved)
{
    _SIR_UNUSED(module);
    _SIR_UNUSED(ul_reason_for_call);
    _SIR_UNUSED(reserved);
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

    return TRUE;
}
#endif

static const char* author = "libsir contributors";
static const char* desc   = "Does nothing interesting. Logs messages to stdout.";

/* Controlling misbehavior via preprocessor macros:
 *
 * When the following are defined upon compilation, this plugin will exhibit behavior
 * that will cause libsir to reject it during the loading/validation process. Only one
 * at a time need be defined, since one failed check will result in the loader immediately
 * unloading the module.
 *
 * - DUMMYPLUGIN_BADBEHAVIOR1: return false from 'sir_plugin_query'
 * - DUMMYPLUGIN_BADBEHAVIOR2: set info::iface_ver != SIR_PLUGIN_VCURRENT
 * - DUMMYPLUGIN_BADBEHAVIOR3: set info::levels and/or info::opts to invalid values
 * - DUMMYPLUGIN_BADBEHAVIOR4: missing an export
 */

PLUGIN_EXPORT bool sir_plugin_query(sir_plugininfo* info) {
    info->iface_ver = SIR_PLUGIN_VCURRENT;
    info->maj_ver   = 1;
    info->min_ver   = 0;
    info->bld_ver   = 0;
    info->levels    = SIRL_DEBUG | SIRL_INFO;
    info->opts      = SIRO_NOHOST | SIRO_NOTID;
    info->author    = author;
    info->desc      = desc;
    info->caps      = 0;

    printf("\t" DGRAY("dummy_plugin(%s)") "\n", __func__);
    return true;
}

PLUGIN_EXPORT bool sir_plugin_init(void) {
    printf("\t" DGRAY("dummy_plugin(%s)") "\n", __func__);
    return true;
}

PLUGIN_EXPORT bool sir_plugin_write(sir_level level, const char* message) {
    _SIR_UNUSED(level);
    printf("\t" DGRAY("dummy_plugin(%s): %s") "\n", __func__, message);
    return true;
}

PLUGIN_EXPORT bool sir_plugin_cleanup(void) {
    printf("\t" DGRAY("dummy_plugin(%s)") "\n", __func__);
    return true;
}
