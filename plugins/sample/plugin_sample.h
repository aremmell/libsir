/*
 * plugin_sample.h
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

#ifndef _SIR_PLUGIN_SAMPLE_H_INCLUDED
# define _SIR_PLUGIN_SAMPLE_H_INCLUDED

# include "sir/platform.h"
# include "sir/types.h"

/**
 * @defgroup plugins Plugins
 * @{
 */

# if defined(__WIN__)
/** Windows-only DLL entry point. */
BOOL APIENTRY DllMain(HMODULE module, DWORD ul_reason_for_call, LPVOID reserved);
#  define PLUGIN_EXPORT __declspec(dllexport) /**< Windows-only export keyword. */
# else
#  define PLUGIN_EXPORT
# endif

/**
 * @brief Called by libsir after the plugin library object is loaded, but before
 * any other functions are probed or called.
 *
 * The plugin must fill out the information in the `info` structure in a way
 * satisfactory to libsir, or it will immediately be unloaded:
 *
 * - all members of the ::sir_plugininfo structure are required to be set by the
 *   plugin.
 * - `iface_ver` must be set to `SIR_PLUGIN_VCURRENT`.
 * - `char*` members must be set to valid strings, and must point at static memory
 *   that will not go out of scope when the function exits. Do not allocate heap
 *   memory for these properties; they will be leaked.
 * - `levels` must be a valid ::sir_level bitmask.
 * - `opts` must be a valid ::sir_option bitmask.
 *
 * @note It is not possible to change these settings once chosen, so ensure that
 * they are correct.
 *
 * @see ::sir_level
 * @see ::sir_option
 * @see ::default
 *
 * @param   info Pointer to a ::sir_plugininfo structure to be initialized by
 *               the plugin.
 * @returns bool `true` if the `info` structure was successfully initialized,
 *               `false` otherwise. If `false`, the plugin will be unloaded.
 */
PLUGIN_EXPORT bool sir_plugin_query(sir_plugininfo* info);

/**
 * @brief Called by libsir after the plugin is queried and successfully validated.
 *
 * The plugin should only at the time of this call begin allocating resources and
 * initializing its internal state.
 *
 * @returns bool `true` if the plugin successfully initialized its internal
 *                state, `false` otherwise.
 */
PLUGIN_EXPORT bool sir_plugin_init(void);

/**
 * @brief Called by libsir when a message is dispatched on any level whose bit
 * was set in the `levels` bitmask of the ::sir_plugininfo structure when
 * ::sir_plugin_query was called.
 *
 * The `message` string will be pre-formatted according to the ::sir_option set
 * in the `opts` bitmask of the ::sir_plugininfo structure.
 *
 * @param   level   The ::sir_level of the message being dispatched.
 * @param   message A string containing the pre-formatted message being dispatched.
 * @returns bool    `true` if the message was successfully processed, `false`
 *                  otherwise.
 */
PLUGIN_EXPORT bool sir_plugin_write(sir_level level, const char* message);

/**
 * @brief Called by libsir when the plugin is about to be unloaded.
 *
 * The plugin should immediately begin releasing allocated resources and
 * resetting its internal state.
 *
 * @returns bool `true` if the plugin cleaned up successfully, `false` otherwise.
 */
PLUGIN_EXPORT bool sir_plugin_cleanup(void);

/**
 * @addtogroup plugins
 * @{
 *
 * ## Intro
 *
 * Using plugins, it is possible to extend libsir's reach when dispatching log
 * messages. A simple example of this would be a plugin that only registers for
 * emergency-level messages. Whenever it receives a message, it posts the message
 * to a REST API endpoint which results in a push notification being sent to a
 * mobile device.
 *
 * Use your imagination; essentially anything is possible. There are some caveats,
 * though: until the thread pool and job queue mechanisms implemented in
 * ([#121](https://github.com/aremmell/libsir/issues/121)) are utilized, plugins
 * must either only perform operations that are guaranteed to complete quickly,
 * or perform those operations asynchronously (*i.e., on another thread*).
 *
 * ## Versioning
 *
 * libsir's plugin interface will be versioned; the functions appearing on
 * this page comprise the plugin interface v1. If/when a new function export is
 * added (or one is modified), the version number will be bumped.
 *
 * When plugins are compiled, their interface version is hard-coded in. This means
 * that as libsir continues to evolve (and the version number increases), it can
 * still communicate with older plugins via backwards-compatible versioned
 * interfaces.
 *
 * ## Creating your own plugin
 *
 * The following steps should be taken in order to write your own fully-
 * functioning libsir plugin:
 *
 * - Make a copy of the `plugins/sample` directory (within `plugins`).
 * - Rename the new directory whatever you'd like. The name of the new directory
 * will determine the name of the plugin file that is produced.
 * - Study the implementation of the sample plugin. It is included below for your
 * convenience. Modify your copy to suit your specific needs.
 *
 * @include plugin_sample.c
 *
 * - `cd` back into the root directory of the repository and run `make clean
 * plugins`. If everything goes smoothly, your shiny new plugin should now be
 * located in `build/lib`. If you named your directory 'foo', you should see a
 * `plugin_foo.[so/dll]`.
 * - You can now move the plugin wherever you'd like, and use ::sir_loadplugin
 * to load it (after you've called ::sir_init, of course).
 *
 * If you encounter any problems, rebuild with `env SIR_SELFLOG=1 SIR_DEBUG=1
 * make clean plugins`. This will enable diagnostic output from libsir that
 * should aid in the diagnosis of any issues you encounter. If you're still stuck,
 * [reach out](https://github.com/aremmell/libsir/discussions/new?category=q-a)
 * and somebody will assist you within a reasonable amount of time.
 */

/**
 * @}
 * @}
 */

#endif /* !_SIR_PLUGIN_SAMPLE_H_INCLUDED */
