/*
 * plugin_dummy.c
 *
 * Version: 2.2.5
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

#include "plugin_dummy.h"
#include "sir/ansimacros.h"
#include "sir/helpers.h"
#include <stdio.h>

#if !defined(PLUGIN_NAME)
# define PLUGIN_NAME "plugin_dummy"
#endif

/*
 * Controlling misbehavior via preprocessor macros:
 *
 * When the following are defined upon compilation, this plugin will exhibit
 * behavior that will cause libsir to reject it during the loading/validation
 * process. Only one at a time need be defined, since one failed check will result
 * in the loader immediately rejecting and unloading the module.
 *
 * - PLUGINDUMMY_BADBEHAVIOR1: return false from 'sir_plugin_query'
 * - PLUGINDUMMY_BADBEHAVIOR2: set info::iface_ver != SIR_PLUGIN_VCURRENT
 * - PLUGINDUMMY_BADBEHAVIOR3: set invalid values in sir_plugininfo
 * - PLUGINDUMMY_BADBEHAVIOR4: missing an export
 * - PLUGINDUMMY_BADBEHAVIOR5: return false from 'sir_plugin_init'
 * - PLUGINDUMMY_BADBEHAVIOR6: return false from 'sir_plugin_write' and
 *   'sir_plugin_cleanup'
 */

#if defined(__WIN__)
BOOL APIENTRY DllMain(HMODULE module, DWORD ul_reason_for_call, LPVOID reserved) {
    SIR_UNUSED(module);
    SIR_UNUSED(ul_reason_for_call);
    SIR_UNUSED(reserved);
    return TRUE;
}
#endif

static const uint8_t maj_ver   = 1U;
static const uint8_t min_ver   = 0U;
static const uint8_t bld_ver   = 0U;
static const sir_levels levels = SIRL_DEBUG | SIRL_INFO;
static const sir_options opts  = SIRO_NOHOST | SIRO_NOTID;
static const char* author      = "libsir contributors";
static const char* desc        = "Logs messages and function calls to stdout.";
static const uint64_t caps     = 0ULL;

PLUGIN_EXPORT bool sir_plugin_query(sir_plugininfo* info) {
#if defined(PLUGINDUMMY_BADBEHAVIOR2)
    info->iface_ver = 255U;
#else
    info->iface_ver = SIR_PLUGIN_VCURRENT;
#endif
    info->maj_ver   = maj_ver;
    info->min_ver   = min_ver;
    info->bld_ver   = bld_ver;
#if defined(PLUGINDUMMY_BADBEHAVIOR3)
    info->levels    = 0xfe23;
    info->opts      = 0x1234abcd;
    info->author    = NULL;
    SIR_UNUSED(author);
    info->desc      = "";
    SIR_UNUSED(desc);
#else
    info->levels    = levels;
    info->opts      = opts;
    info->author    = author;
    info->desc      = desc;
#endif
    info->caps      = caps;

    (void)printf("\t" SIR_DGRAY("" PLUGIN_NAME " ('%s')") SIR_EOL, __func__);

#if defined(PLUGINDUMMY_BADBEHAVIOR1)
    return false;
#else
    return true;
#endif
}

#if !defined(PLUGINDUMMY_BADBEHAVIOR4)
PLUGIN_EXPORT bool sir_plugin_init(void) {
    (void)printf("\t" SIR_DGRAY("" PLUGIN_NAME " ('%s')") SIR_EOL, __func__);
# if defined(PLUGINDUMMY_BADBEHAVIOR5)
    return false;
# else
    return true;
# endif
}
#endif

PLUGIN_EXPORT bool sir_plugin_write(sir_level level, const char* message) {
#if defined(PLUGINDUMMY_BADBEHAVIOR6)
    SIR_UNUSED(level);
    SIR_UNUSED(message);
    return false;
#else
    (void)printf("\t" SIR_DGRAY("" PLUGIN_NAME " (%s): level: %04"PRIx16", message: %s"),
                 __func__, level, message);
    return true;
#endif
}

PLUGIN_EXPORT bool sir_plugin_cleanup(void) { //-V524
    (void)printf("\t" SIR_DGRAY("" PLUGIN_NAME " ('%s')") SIR_EOL, __func__);
#if defined(PLUGINDUMMY_BADBEHAVIOR6)
    return false;
#else
    return true;
#endif
}
