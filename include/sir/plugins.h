/*
 * plugins.h
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

#ifndef _SIR_PLUGINS_H_INCLUDED
# define _SIR_PLUGINS_H_INCLUDED

# include "sir/types.h"

typedef bool (*sir_plugin_pred)(const void*, const sir_plugin*);

sirpluginid _sir_plugin_load(const char* path);
sirpluginid _sir_plugin_probe(sir_plugin* plugin);
sir_pluginexport _sir_plugin_getexport(sir_pluginhandle handle, const char* name);
bool _sir_plugin_unload(sir_plugin* plugin);

sirpluginid _sir_plugin_add(sir_plugin* plugin);
bool _sir_plugin_rem(sirpluginid id);
void _sir_plugin_destroy(sir_plugin** plugin);

bool _sir_plugin_cache_pred_id(const void* match, const sir_plugin* iter);

sirpluginid _sir_plugin_cache_add(sir_plugincache* spc, sir_plugin* plugin);
sir_plugin* _sir_plugin_cache_find_id(const sir_plugincache* spc, sirpluginid id);
sir_plugin* _sir_plugin_cache_find(const sir_plugincache* spc, const void* match, sir_plugin_pred pred);
bool _sir_plugin_cache_rem(sir_plugincache* spc, sirpluginid id);
bool _sir_plugin_cache_destroy(sir_plugincache* spc);
bool _sir_plugin_cache_dispatch(const sir_plugincache* spc, sir_level level, sirbuf* buf,
    size_t* dispatched, size_t* wanted);

#endif /* !_SIR_PLUGINS_H_INCLUDED */
