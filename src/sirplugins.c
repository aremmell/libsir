/*
 * sirplugins.c
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2023
 * Version:   2.2.1
 *
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
#include "sir/plugins.h"
#include "sir/internal.h"

#if !defined(SIR_NO_PLUGINS)
sirpluginid _sir_plugin_load(const char* path) {
    if (!_sir_validstr(path))
        return 0;

# if !defined (__WIN__)
    sir_pluginhandle handle = dlopen(path, RTLD_NOW | RTLD_LOCAL);
    if (!handle) {
        const char* err = dlerror();
        _sir_selflog("error: dlopen('%s') failed (%s)", path, _SIR_PRNSTR(err));
        (void)_sir_handleerr(errno);
        return 0;
    }
# else /* __WIN__ */
    UINT old_error_mode     = SetErrorMode(SEM_FAILCRITICALERRORS);
    sir_pluginhandle handle = LoadLibraryA(path);
    SetErrorMode(old_error_mode);
    if (!handle) {
        DWORD err = GetLastError();
        _sir_selflog("error: LoadLibraryA(%s) failed (%lu)", path, err);
        (void)_sir_handlewin32err(err);
        return 0;
    }
# endif

    sir_plugin* plugin = (sir_plugin*)calloc(1, sizeof(sir_plugin));
    if (!plugin) {
        (void)_sir_handleerr(errno);
        return 0;
    }

    plugin->handle = handle;
    plugin->loaded = true;
    plugin->path   = strdup(path);

    if (!plugin->path) {
        _sir_plugin_destroy(&plugin);
        return _sir_handleerr(errno);
    }

    _sir_selflog("loaded plugin (path: '%s', addr: %p); probing...",
        plugin->path, plugin->handle);

    return _sir_plugin_probe(plugin);
}

sirpluginid _sir_plugin_probe(sir_plugin* plugin) {
    if (!_sir_validptr(plugin) || !_sir_validptr(plugin->handle) ||
        !_sir_validstr(plugin->path)) {
        _sir_plugin_destroy(&plugin);
        return 0;
    }

# if SIR_PLUGIN_VCURRENT == SIR_PLUGIN_V1
    /* if/when new versions of plugin interfaces are introduced, we will need to
     * modify/extend the following code:
     *
     * - remove the enclosing #if
     * - get the v1 exports (all versions will have v1 exports), resolve them, and
     * call sir_plugin_query.
     * - switch on version returned to resolve additional exports. this will
     * necessitate additional versioned interface strctures as members of the
     * sir_plugin struct, e.g. ifacev1, ifacev2). */
    plugin->iface.query   = (sir_plugin_queryfn)
        _sir_plugin_getexport(plugin->handle, SIR_PLUGIN_EXPORT_QUERY);
    plugin->iface.init    = (sir_plugin_initfn)
        _sir_plugin_getexport(plugin->handle, SIR_PLUGIN_EXPORT_INIT);
    plugin->iface.write   = (sir_plugin_writefn)
        _sir_plugin_getexport(plugin->handle, SIR_PLUGIN_EXPORT_WRITE);
    plugin->iface.cleanup = (sir_plugin_cleanupfn)
        _sir_plugin_getexport(plugin->handle, SIR_PLUGIN_EXPORT_CLEANUP);

    if (!plugin->iface.query || !plugin->iface.init ||
        !plugin->iface.write || !plugin->iface.cleanup) {
        _sir_selflog("error: export(s) not resolved for plugin (path:"
                     " '%s', addr: %p)!", plugin->path, plugin->handle);
        _sir_selflog("exports (query: %"PRIxPTR", init: %"PRIxPTR", write:"
                     " %"PRIxPTR", cleanup; %"PRIxPTR")",
                     (uintptr_t)plugin->iface.query, (uintptr_t)plugin->iface.init,
                     (uintptr_t)plugin->iface.write, (uintptr_t)plugin->iface.cleanup);
        _sir_seterror(_SIR_E_PLUGINBAD);
        _sir_plugin_destroy(&plugin);
        return 0;
    }
# else
#  error "plugin version not implemented"
# endif

    /* query the plugin for information. */
    if (!plugin->iface.query(&plugin->info)) {
        _sir_selflog("error: plugin (path: '%s', addr: %p) returned false from"
                     " query fn!", plugin->path, plugin->handle);
        _sir_seterror(_SIR_E_PLUGINERR);
        _sir_plugin_destroy(&plugin);
        return 0;
    }

    /* verify version. */
    if (!plugin->info.iface_ver || plugin->info.iface_ver > SIR_PLUGIN_VCURRENT) {
        _sir_selflog("error: plugin (path: '%s', addr: %p) has version"
                     " %"PRIu8"; libsir has %d", plugin->path, plugin->handle,
                     plugin->info.iface_ver, SIR_PLUGIN_VCURRENT);
        _sir_seterror(_SIR_E_PLUGINVER);
        _sir_plugin_destroy(&plugin);
        return 0;
    }

    /* verify level registration bitmask. */
    if (!_sir_validlevels(plugin->info.levels)) {
        _sir_selflog("error: plugin (path: '%s', addr: %p) has invalid levels"
                     " %04"PRIx16, plugin->path, plugin->handle, plugin->info.levels);
        _sir_seterror(_SIR_E_PLUGINDAT);
        _sir_plugin_destroy(&plugin);
        return 0;
    }

    /* verify formatting options bitmask. */
    if (!_sir_validopts(plugin->info.opts)) {
        _sir_selflog("error: plugin (path: '%s', addr: %p) has invalid opts"
                     " %08"PRIx32, plugin->path, plugin->handle, plugin->info.opts);
        _sir_seterror(_SIR_E_PLUGINDAT);
        _sir_plugin_destroy(&plugin);
        return 0;
    }

    /* verify strings */
    if (!_sir_validstrnofail(plugin->info.author) ||
        !_sir_validstrnofail(plugin->info.desc)) {
        _sir_selflog("error: plugin (path: '%s', addr: %p) has invalid author"
                     " or description", plugin->path, plugin->handle);
        _sir_seterror(_SIR_E_PLUGINDAT);
        _sir_plugin_destroy(&plugin);
        return 0;
    }

    /* plugin is valid; tell it to initialize, assign it an id,
     * print its information, and add to cache. */
    if (!plugin->iface.init()) {
        _sir_selflog("error: plugin (path: '%s', addr: %p) failed to initialize!",
            plugin->path, plugin->handle);
        _sir_seterror(_SIR_E_PLUGINERR);
        _sir_plugin_destroy(&plugin);
        return 0;
    }

    plugin->id    = FNV32_1a((const uint8_t*)&plugin->iface, sizeof(sir_pluginiface));
    plugin->valid = true;

    _sir_selflog("successfully validated plugin (path: '%s', id: %08"PRIx32")."
                 " properties:\n{\n\tversion = %"PRIu8".%"PRIu8".%"PRIu8"\n\t"
                 "levels = %04"PRIx16"\n\topts = %08"PRIx32"\n\tauthor = '%s'"
                 "\n\tdesc = '%s'\n\tcaps = %016"PRIx64"\n}", plugin->path,
                 plugin->id, plugin->info.maj_ver, plugin->info.min_ver,
                 plugin->info.bld_ver, plugin->info.levels, plugin->info.opts,
                 _SIR_PRNSTR(plugin->info.author), _SIR_PRNSTR(plugin->info.desc),
                 plugin->info.caps);

    sirpluginid retval = _sir_plugin_add(plugin);
    if (0 == retval) {
        _sir_selflog("error: failed to add plugin (path: '%s', addr: %p) to"
                     " cache; unloading", plugin->path, plugin->handle);
        _sir_plugin_destroy(&plugin);
    }

    return retval;
}

uintptr_t _sir_plugin_getexport(sir_pluginhandle handle, const char* name) {
    if (!_sir_validptr(handle) || !_sir_validstr(name))
        return 0;

# if !defined(__WIN__)
    sir_pluginexport addr = dlsym(handle, name);
    if (!addr) {
        const char* err = dlerror();
        _sir_selflog("error: dlsym(%p, '%s') failed (%s)", handle, name,
            _SIR_PRNSTR(err));
        (void)_sir_handleerr(errno);
        return 0;
    }
# else /* __WIN__ */
    sir_pluginexport addr = GetProcAddress(handle, name);
    if (!addr) {
        DWORD err = GetLastError();
        _sir_selflog("error: GetProcAddress(%p, '%s') failed (%lu)", handle,
            name, err);
        (void)_sir_handlewin32err(err);
        return 0;
    }
# endif

    _sir_selflog("successfully resolved plugin export (name: '%s', addr: %p)",
        name, addr);
    return (uintptr_t)addr;
}

void _sir_plugin_unload(sir_plugin* plugin) {
    if (!_sir_validptrnofail(plugin))
        return;

    /* if the plugin cleanup export was resolved, call it. */
    if (plugin->iface.cleanup && !plugin->iface.cleanup())
        _sir_selflog("error: plugin (path: '%s', addr: %p) reports unsuccessful"
                     " cleanup!", plugin->path, plugin->handle);

# if !defined(__WIN__)
    int ret = dlclose(plugin->handle);
    if (0 != ret) {
        const char* err = dlerror();
        _sir_selflog("error: dlclose(%p) failed (%s)", plugin->handle, //-V774
            _SIR_PRNSTR(err));
        (void)_sir_handleerr(errno);
        return;
    }
# else /* __WIN__ */
    if (!FreeLibrary(plugin->handle)) {
        DWORD err = GetLastError();
        _sir_selflog("error: FreeLibrary(%p) failed (%lu)", plugin->handle, err);
        (void)_sir_handlewin32err(err);
        return;
    }
# endif

    plugin->handle = NULL;
    plugin->loaded = false;
    _sir_selflog("unloaded plugin (path: '%s', id: %08"PRIx32")", plugin->path,
        plugin->id);
}

sirpluginid _sir_plugin_add(sir_plugin* plugin) {
    _sir_seterror(_SIR_E_NOERROR);

    if (!_sir_sanity() || !_sir_validptr(plugin))
        return 0;

    _SIR_LOCK_SECTION(sir_plugincache, spc, SIRMI_PLUGINCACHE, 0);
    sirpluginid retval = _sir_plugin_cache_add(spc, plugin);
    _SIR_UNLOCK_SECTION(SIRMI_PLUGINCACHE);

    return retval;
}

bool _sir_plugin_rem(sirpluginid id) {
    _sir_seterror(_SIR_E_NOERROR);

    if (!_sir_sanity())
        return false;

    _SIR_LOCK_SECTION(sir_plugincache, spc, SIRMI_PLUGINCACHE, false);
    bool retval = _sir_plugin_cache_rem(spc, id);
    _SIR_UNLOCK_SECTION(SIRMI_PLUGINCACHE);

    return retval;
}

void _sir_plugin_destroy(sir_plugin** plugin) {
    if (!_sir_validptrptr(plugin) || !_sir_validptr(*plugin))
        return;

    _sir_plugin_unload(*plugin);
    _sir_safefree(&(*plugin)->path);
    _sir_safefree(plugin);
}

bool _sir_plugin_cache_pred_id(const void* match, sir_plugin* iter) {
    return iter->id == *((sirpluginid*)match);
}

sirpluginid _sir_plugin_cache_add(sir_plugincache* spc, sir_plugin* plugin) {
    if (!_sir_validptr(spc) || !_sir_validptr(plugin))
        return 0;

    if (spc->count >= SIR_MAXPLUGINS) {
        _sir_seterror(_SIR_E_NOROOM);
        return 0;
    }

    sir_plugin* existing = _sir_plugin_cache_find_id(spc, plugin->id);
    if (NULL != existing) {
        _sir_selflog("error: already have plugin (path: '%s', id %08"PRIx32")",
            existing->path, plugin->id);
        _sir_seterror(_SIR_E_DUPITEM);
        return 0;
    }

    _sir_selflog("adding plugin (path: %s, id: %08"PRIx32"); count = %zu",
    plugin->path, plugin->id, spc->count + 1);
    spc->plugins[spc->count++] = plugin;
    return plugin->id;
}

sir_plugin* _sir_plugin_cache_find_id(sir_plugincache* spc, sirpluginid id) {
    return _sir_plugin_cache_find(spc, &id, &_sir_plugin_cache_pred_id);
}

sir_plugin* _sir_plugin_cache_find(sir_plugincache* spc, const void* match,
    sir_plugin_pred pred) {
    if (!_sir_validptr(spc) || !_sir_validptr(match) || !_sir_validfnptr(pred))
        return NULL;

    for (size_t n = 0; n < spc->count; n++) {
        if (pred(match, spc->plugins[n]))
            return spc->plugins[n];
    }

    return NULL;
}

bool _sir_plugin_cache_rem(sir_plugincache* spc, sirpluginid id) {
    if (!_sir_validptr(spc))
        return false;

    for (size_t n = 0; n < spc->count; n++) {
        if (spc->plugins[n]->id == id) {
            _sir_selflog("removing plugin (path: '%s', id: %"PRIx32"); count = %zu",
                spc->plugins[n]->path, spc->plugins[n]->id, spc->count - 1);

            _sir_plugin_destroy(&spc->plugins[n]);

            for (size_t i = n; i < spc->count - 1; i++) {
                spc->plugins[i] = spc->plugins[i + 1];
                spc->plugins[i + 1] = NULL;
            }

            spc->count--;
            return true;
        }
    }

    _sir_seterror(_SIR_E_NOITEM);
    return false;
}

bool _sir_plugin_cache_destroy(sir_plugincache* spc) {
    if (!_sir_validptr(spc))
        return false;

    for (size_t n = 0; n < spc->count; n++) {
        _sir_plugin_destroy(&spc->plugins[n]);
        spc->plugins[n] = NULL;
        spc->count--;
    }

    SIR_ASSERT(spc->count == 0);
    memset(spc, 0, sizeof(sir_plugincache));
    return true;
}

bool _sir_plugin_cache_dispatch(sir_plugincache* spc, sir_level level, sirbuf* buf,
    size_t* dispatched, size_t* wanted) {
    if (!_sir_validptr(spc) || !_sir_validlevel(level) || !_sir_validptr(buf) ||
        !_sir_validptr(dispatched) || !_sir_validptr(wanted))
        return false;

    const char* write    = NULL;
    sir_options lastopts = 0;

    *dispatched = 0;
    *wanted     = 0;

    for (size_t n = 0; n < spc->count; n++) {
        if (!_sir_bittest(spc->plugins[n]->info.levels, level)) {
            _sir_selflog("level %04"PRIx32" not set in level mask (%04"PRIx16
                         ") for plugin (path: '%s', id: %08"PRIx32"); skipping",
                         level, spc->plugins[n]->info.levels, spc->plugins[n]->path,
                         spc->plugins[n]->id);
            continue;
        }

        (*wanted)++;

        if (!write || spc->plugins[n]->info.opts != lastopts) {
            write = _sir_format(false, spc->plugins[n]->info.opts, buf);
            SIR_ASSERT(write);
            lastopts = spc->plugins[n]->info.opts;
        }

        if (write && spc->plugins[n]->iface.write(level, write)) {
            (*dispatched)++;
        } else {
            _sir_selflog("error: write to plugin (path: '%s', id: %08"PRIx32")"
                         " failed!", spc->plugins[n]->path, spc->plugins[n]->id);
        }
    }

    return (*dispatched == *wanted);
}
#else /* SIR_NO_PLUGINS */
sirpluginid _sir_plugin_load(const char* path) {
    _SIR_UNUSED(path);
    return 0;
}

sirpluginid _sir_plugin_probe(sir_plugin* plugin) {
    _SIR_UNUSED(plugin);
    return 0;
}

uintptr_t _sir_plugin_getexport(sir_pluginhandle handle, const char* name) {
    _SIR_UNUSED(handle);
    _SIR_UNUSED(name);
    return 0;
}

void _sir_plugin_unload(sir_plugin* plugin) {
    _SIR_UNUSED(plugin);
}

sirpluginid _sir_plugin_add(sir_plugin* plugin) {
    _SIR_UNUSED(plugin);
    return 0;
}

bool _sir_plugin_rem(sirpluginid id) {
    _SIR_UNUSED(id);
    return false;
}

void _sir_plugin_destroy(sir_plugin** plugin) {
    _SIR_UNUSED(plugin);
}

bool _sir_plugin_cache_pred_id(const void* match, sir_plugin* iter) {
    _SIR_UNUSED(match);
    _SIR_UNUSED(iter);
    return false;
}

sirpluginid _sir_plugin_cache_add(sir_plugincache* spc, sir_plugin* plugin) {
    _SIR_UNUSED(spc);
    _SIR_UNUSED(plugin);
    return 0;
}

sir_plugin* _sir_plugin_cache_find_id(sir_plugincache* spc, sirpluginid id) {
    _SIR_UNUSED(spc);
    _SIR_UNUSED(id);
    return NULL;
}

sir_plugin* _sir_plugin_cache_find(sir_plugincache* spc, const void* match,
    sir_plugin_pred pred) {
    _SIR_UNUSED(spc);
    _SIR_UNUSED(match);
    _SIR_UNUSED(pred);
    return NULL;
}

bool _sir_plugin_cache_rem(sir_plugincache* spc, sirpluginid id) {
    _SIR_UNUSED(spc);
    _SIR_UNUSED(id);
    return false;
}

bool _sir_plugin_cache_destroy(sir_plugincache* spc) {
    _SIR_UNUSED(spc);
    return false;
}

bool _sir_plugin_cache_dispatch(sir_plugincache* spc, sir_level level, sirbuf* buf,
    size_t* dispatched, size_t* wanted) {
    _SIR_UNUSED(spc);
    _SIR_UNUSED(level);
    _SIR_UNUSED(buf);
    _SIR_UNUSED(dispatched);
    _SIR_UNUSED(wanted);
    return false;
}
#endif
