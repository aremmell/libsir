/**
 * @file template.h
 * @brief Defines the interface that libsir plugins must implement.
 *
 * libsir plugins are shared library objects, which must export a specific set
 * of functions. Those functions are defined here; if a version bump occurs,
 * preprocessor macros will be used to indicate which version a function was
 * added in.
 *
 * @author    Ryan M. Lederman \<lederman@gmail.com\>
 * @date      2018-2023
 * @version   2.2.0
 * @copyright The MIT License (MIT)
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
#ifndef _SIR_PLUGINS_TEMPLATE_H_INCLUDED
# define _SIR_PLUGINS_TEMPLATE_H_INCLUDED

# include <sirtypes.h>

# define SIR_PLUGIN_V1 1
# define SIR_PLUGIN_VCURRENT SIR_PLUGIN_V1

/* The libsir-to-plugin query data structure. */
typedef struct {
    uint8_t iface_ver; /**< Plugin interface version. */
    uint8_t maj_ver;   /**< Major version number. */
    uint8_t min_ver;   /**< Minor version number. */
    uint8_t bld_ver;   /**< Build/patch version number. */
    sir_levels levels; /**< Level registration bitmask. */
    sir_options opts;  /**< Formatting options bitmask. */
    char* author;      /**< Plugin author information. */
    char* description; /**< Plugin description. */
    uint64_t caps;     /**< Future-proof plugin capabilities bitmask. */
} sir_plugin_info;

/**
 * @brief Called by libsir after the plugin library object is loaded, but before
 * any other functions are probed or called.
 *
 * The plugin must fill out the information in the `info` structure in a way
 * satisfactory to libsir, or it will immediately be unloaded.
 *
 * - `iface_ver` should be set to `SIR_PLUGIN_CURRENT_VER`; if the interface
 * version does not match libsir's expectation, the plugin will be unloaded.
 * - `char*` members must be malloc'd by the plugin, and will be freed by libsir.
 *
 * @param   info Pointer to a ::sir_plugin_info structure to be initialized by
 * the plugin.
 * @returns bool `true` if the `info` structure was successfully initialized,
 * `false` otherwise.
 */
bool sir_plugin_query(sir_plugin_info* info);

// loaded, passed the checks on data from `sir_plugin_loaded`. Plugin should
// initialize its internal state.
bool sir_plugin_init(void);

// handler for log message writes. must complete *very* quickly, or if unable to
// guarantee quick completion (i.e. network operation involved), must be async.
bool sir_plugin_write(sir_level level, const char* message);

// the plugin is about to be unloaded, and should stop any ongoing operations
// and clean up allocated resources.
bool sir_plugin_shutdown(void);

#endif /* !_SIR_PLUGINS_TEMPLATE_H_INCLUDED */
