/*
 * sirplugins.c
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2023
 * Version:   2.2.0
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
#include "sirplugins.h"
#include "sirinternal.h"

sirpluginid _sir_plugin_load(const char* path) {
    if (!_sir_validstr(path))
        return NULL;

#if !defined (__WIN__)
    sir_pluginhandle handle = dlopen(path, RTLD_NOW | RTLD_LOCAL);
    if (!handle) {
        const char* err = dlerror();
        _sir_selflog("error: dlopen('%s') failed (%s)", path, _SIR_PRNSTR(err));
        _sir_handleerr(errno);
        return NULL;
    }
#else /* __WIN__ */
    UINT old_error_mode     = SetErrorMode(SEM_FAILCRITICALERRORS);
    sir_pluginhandle handle = LoadLibraryA(path);
    SetErrorMode(old_error_mode);
    if (!handle) {
        DWORD err = GetLastError();
        _sir_selflog("error: LoadLibraryA(%s) failed (%lu)", path, err);
        _sir_handlewin32err(err);
        return NULL;
    }
#endif

    sirplugin* plugin = (sirplugin*)calloc(1, sizeof(sirplugin));
    if (!plugin) {
        _sir_handleerr(errno);
        return NULL;
    }

    plugin->path = strdup(path);
    if (!plugin->path) {
        _sir_handleerr(errno);
        return NULL;
    }

    plugin->handle = handle;
    plugin->loaded = true;

    _sir_selflog("loaded plugin file (path: '%s', addr: %p); probing...",
        plugin->path, plugin->handle);

    return _sir_plugin_probe(plugin);
}

sirpluginid _sir_plugin_probe(sirplugin* plugin) {
    if (!_sir_validptr(plugin) || !_sir_validptr(plugin->handle) ||
        !_sir_validstr(plugin->path))
        return NULL;

#if SIR_PLUGIN_VCURRENT == SIR_PLUGIN_V1
    plugin->iface.query   =
        _sir_plugin_getexport(plugin->handle, "sir_plugin_query");
    plugin->iface.init    =
        _sir_plugin_getexport(plugin->handle, "sir_plugin_init");
    plugin->iface.write   =
        _sir_plugin_getexport(plugin->handle, "sir_plugin_write");
    plugin->iface.cleanup =
        _sir_plugin_getexport(plugin->handle, "sir_plugin_cleanup");

    if (!plugin->iface.query || !plugin->iface.init ||
        !plugin->iface.write || !plugin->iface.cleanup) {
        _sir_selflog("error: some exports not resolved in plugin file (path:"
                     " '%s', addr: %p)!", plugin->path, plugin->handle);
        _sir_selflog("exports (query: %p, init: %p, write: %p, cleanup; %p)",
                     plugin->iface.query, plugin->iface.init,
                     plugin->iface.write, plugin->iface.cleanup);
        _sir_plugin_destroy(&plugin);
        return NULL;
    }

    /* query the plugin for information. */
    if (!((sir_plugin_queryfn)plugin->iface.query)(&plugin->info)) {
        _sir_selflog("error: plugin returned false from query fn!");
        _sir_plugin_destroy(&plugin);
        return NULL;
    }

    /* verify version. */
    if (plugin->info.iface_ver != SIR_PLUGIN_VCURRENT) {
        _sir_selflog("error: plugin (path: '%s', addr: %p) has version %"PRIu8
                     ", libsir has %"PRIu8, plugin->path, plugin->handle,
                     plugin->info.iface_ver, (uint8_t)SIR_PLUGIN_VCURRENT);
        _sir_plugin_destroy(&plugin);
        return NULL;
    }

    /* plugin is valid; print its information out and add to cache. */
    plugin->valid = true;

    _sir_selflog("successfully loaded plugin (path: '%s', addr: %p); properties:"
                 "\n{\n\tmaj_ver = %"PRIu8"\n\tmin_ver = %"PRIu8"\n\tbld_ver = "
                 "%"PRIu8"\n\tlevels = %04"PRIx16"\n\topts = %08"PRIx32"\n\t"
                 "author = '%s'\n\tdesc = '%s'\n\tcaps = %016"PRIx64"\n}",
                 plugin->path, plugin->handle, plugin->info.maj_ver,
                 plugin->info.min_ver, plugin->info.bld_ver, plugin->info.levels,
                 plugin->info.opts, _SIR_PRNSTR(plugin->info.author),
                 _SIR_PRNSTR(plugin->info.description), plugin->info.caps);

    return _sir_plugin_add(plugin);
#else
# error "no implementation for this plugin version"
#endif
}

sir_pluginexport _sir_plugin_getexport(sir_pluginhandle handle, const char* name) {
    if (!_sir_validptr(handle))
        return NULL;

#if !defined(__WIN__)
    sir_pluginexport addr = dlsym(handle, name);
    if (!addr) {
        const char* err = dlerror();
        _sir_selflog("error: dlsym(%p, '%s') failed (%s)", handle, name,
            _SIR_PRNSTR(err));
        _sir_handleerr(errno);
        return NULL;
    }
#else /* __WIN__ */
    sir_pluginexport addr = GetProcAddressA(handle, name);
    if (!addr) {
        DWORD err = GetLastError();
        _sir_selflog("error: GetProcAddressA(%p, '%s') failed (%lu)", handle,
            name, err);
        _sir_handlewin32err(err);
        return NULL;
    }
#endif

    _sir_selflog("resolved plugin export (name: '%s', addr: %p)", name, addr);
    return addr;
}

sirpluginid _sir_plugin_add(sirplugin* plugin) {
    return NULL;
}

sirplugin* _sir_plugin_find(const void* match, sir_plugin_pred pred) {
    return NULL;
}

bool _sir_plugin_rem(sirpluginid id) {
    return false;
}

void _sir_plugin_unload(sirplugin* plugin) {
    if (!_sir_validptrnofail(plugin))
        return;

    /* if the cleanup export was resolved, call it. */
    if (plugin->iface.cleanup)
        ((sir_plugin_cleanupfn)plugin->iface.cleanup)();

#if !defined(__WIN__)
    int ret = dlclose(plugin->handle);
    if (0 != ret) {
        const char* err = dlerror();
        _sir_selflog("error: dlclose(%p) failed (%s)", plugin->handle,
            _SIR_PRNSTR(err));
        _sir_handleerr(errno);
        return;
    }
#else /* __WIN__ */
    if (!FreeLibrary(plugin->handle)) {
        DWORD err = GetLastError();
        _sir_selflog("error: FreeLibrary(%p) failed (%lu)", plugin->handle, err);
        _sir_handlewin32err(err);
        return;
    }
#endif

    plugin->handle = NULL;
    _sir_selflog("unloaded plugin (path: '%s')", plugin->path);
}

void _sir_plugin_destroy(sirplugin** plugin) {
    if (!_sir_validptrnofail(*plugin))
        return;

    _sir_plugin_unload(*plugin);
    _sir_safefree(&(*plugin)->path);
    _sir_safefree(plugin);
}
