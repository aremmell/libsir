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
#include <stdio.h>

static const char* author = "libsir contributors";
static const char* desc   = "Does nothing interesting. Logs messages to stdout.";

bool sir_plugin_query(sir_plugininfo* info) {
    info->iface_ver = SIR_PLUGIN_VCURRENT;
    info->maj_ver   = 1;
    info->min_ver   = 0;
    info->bld_ver   = 0;
    info->levels    = SIRL_DEBUG | SIRL_INFO;
    info->opts      = SIRO_NOHOST | SIRO_NOTID;
    info->author    = author;
    info->desc      = desc;
    info->caps      = 0;

    printf("dummy_plugin(%s)\n", __func__);
    return true;
}

bool sir_plugin_init(void) {
    printf("dummy_plugin(%s)\n", __func__);
    return true;
}

bool sir_plugin_write(sir_level level, const char* message) {
    printf("dummy_plugin(%s): %s\n", __func__, message);
    return true;
}

bool sir_plugin_cleanup(void) {
    printf("dummy_plugin(%s)\n", __func__);
    return true;
}
