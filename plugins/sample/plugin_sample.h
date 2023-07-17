/*
 * plugin_sample.h
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
#ifndef _SIR_PLUGIN_SAMPLE_H_INCLUDED
# define _SIR_PLUGIN_SAMPLE_H_INCLUDED

# include "sir/platform.h"
# include "sir/types.h"

# if defined(__WIN__)
BOOL APIENTRY DllMain(HMODULE module, DWORD ul_reason_for_call, LPVOID reserved);
#  define PLUGIN_EXPORT __declspec(dllexport)
# else
#  define PLUGIN_EXPORT
# endif

/**
 * @defgroup plugins Plugins
 *
 * Functions exported by libsir plugins.
 *
 * @addtogroup plugins
 * @{
 */

/**
 * @brief Called by libsir after the plugin library object is loaded, but before
 * any other functions are probed or called.
 *
 * The plugin must fill out the information in the `info` structure in a way
 * satisfactory to libsir, or it will immediately be unloaded:
 *
 * - all members of the ::sir_plugininfo structure are required to be set by the
 *   plugin.
 * - `iface_ver` must be set to `SIR_PLUGIN_VCURRENT`; if the interface version
 *   does not match libsir's expectation, the plugin will be unloaded.
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

/** @}@} */

#endif /* !_SIR_PLUGIN_SAMPLE_H_INCLUDED */
