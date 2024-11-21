/**
 * @file sir.c
 *
 * @brief Public interface to libsir
 *
 * The functions and types defined here comprise the entire set intended for
 * use by an implementer of the library-unless modification is desired.
 *
 * @version 2.2.6
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

#include "sir.h"
#include "sir/internal.h"
#include "sir/filecache.h"
#include "sir/plugins.h"
#include "sir/textstyle.h"
#include "sir/defaults.h"

bool sir_makeinit(sirinit* si) {
    return _sir_makeinit(si);
}

bool sir_init(sirinit* si) {
    return _sir_init(si);
}

bool sir_cleanup(void) {
    return _sir_cleanup();
}

bool sir_isinitialized(void) {
    return _sir_isinitialized();
}

uint16_t sir_geterror(char message[SIR_MAXERROR]) {
    return _sir_geterrcode(_sir_geterror(message));
}

void sir_geterrorinfo(sir_errorinfo* err) {
    _sir_geterrorinfo(err);
}

PRINTF_FORMAT_ATTR(1, 2)
bool sir_debug(PRINTF_FORMAT const char* format, ...) {
    _SIR_L_START(format);
    ret = _sir_logv(SIRL_DEBUG, format, args);
    _SIR_L_END();
    return ret;
}

PRINTF_FORMAT_ATTR(1, 2)
bool sir_info(PRINTF_FORMAT const char* format, ...) {
    _SIR_L_START(format);
    ret = _sir_logv(SIRL_INFO, format, args);
    _SIR_L_END();
    return ret;
}

PRINTF_FORMAT_ATTR(1, 2)
bool sir_notice(PRINTF_FORMAT const char* format, ...) {
    _SIR_L_START(format);
    ret = _sir_logv(SIRL_NOTICE, format, args);
    _SIR_L_END();
    return ret;
}

PRINTF_FORMAT_ATTR(1, 2)
bool sir_warn(PRINTF_FORMAT const char* format, ...) {
    _SIR_L_START(format);
    ret = _sir_logv(SIRL_WARN, format, args);
    _SIR_L_END();
    return ret;
}

PRINTF_FORMAT_ATTR(1, 2)
bool sir_error(PRINTF_FORMAT const char* format, ...) {
    _SIR_L_START(format);
    ret = _sir_logv(SIRL_ERROR, format, args);
    _SIR_L_END();
    return ret;
}

PRINTF_FORMAT_ATTR(1, 2)
bool sir_crit(PRINTF_FORMAT const char* format, ...) {
    _SIR_L_START(format);
    ret = _sir_logv(SIRL_CRIT, format, args);
    _SIR_L_END();
    return ret;
}

PRINTF_FORMAT_ATTR(1, 2)
bool sir_alert(PRINTF_FORMAT const char* format, ...) {
    _SIR_L_START(format);
    ret = _sir_logv(SIRL_ALERT, format, args);
    _SIR_L_END();
    return ret;
}

PRINTF_FORMAT_ATTR(1, 2)
bool sir_emerg(PRINTF_FORMAT const char* format, ...) {
    _SIR_L_START(format);
    ret = _sir_logv(SIRL_EMERG, format, args);
    _SIR_L_END();
    return ret;
}

sirfileid sir_addfile(const char* path, sir_levels levels, sir_options opts) {
    return _sir_addfile(path, levels, opts);
}

bool sir_remfile(sirfileid id) {
    return _sir_remfile(id);
}

sirpluginid sir_loadplugin(const char* path) {
#if !defined(SIR_NO_PLUGINS)
    return _sir_plugin_load(path);
#else
    SIR_UNUSED(path);
    return _sir_seterror(_SIR_E_UNAVAIL);
#endif
}

bool sir_unloadplugin(sirpluginid id) {
#if !defined(SIR_NO_PLUGINS)
    return _sir_plugin_rem(id);
#else
    SIR_UNUSED(id);
    return _sir_seterror(_SIR_E_UNAVAIL);
#endif
}

bool sir_filelevels(sirfileid id, sir_levels levels) {
    _sir_defaultlevels(&levels, sir_file_def_lvls);
    sir_update_config_data data = {SIRU_LEVELS, &levels, NULL, NULL, NULL};
    return _sir_updatefile(id, &data);
}

bool sir_fileopts(sirfileid id, sir_options opts) {
    _sir_defaultopts(&opts, sir_file_def_opts);
    sir_update_config_data data = {SIRU_OPTIONS, NULL, &opts, NULL, NULL};
    return _sir_updatefile(id, &data);
}

bool sir_settextstyle(sir_level level, sir_textattr attr, sir_textcolor fg,
    sir_textcolor bg) {
    sir_textstyle style = {
        attr,
        fg,
        bg
    };

    return _sir_settextstyle(level, &style);
}

bool sir_resettextstyles(void) {
    return _sir_resettextstyles();
}

sir_textcolor sir_makergb(sir_textcolor r, sir_textcolor g, sir_textcolor b) {
    return _sir_makergb(r, g, b);
}

bool sir_setcolormode(sir_colormode mode) {
    return _sir_setcolormode(mode);
}

bool sir_stdoutlevels(sir_levels levels) {
    _sir_defaultlevels(&levels, sir_stdout_def_lvls);
    sir_update_config_data data = {SIRU_LEVELS, &levels, NULL, NULL, NULL};
    return _sir_writeinit(&data, _sir_stdoutlevels);
}

bool sir_stdoutopts(sir_options opts) {
    _sir_defaultopts(&opts, sir_stdout_def_opts);
    sir_update_config_data data = {SIRU_OPTIONS, NULL, &opts, NULL, NULL};
    return _sir_writeinit(&data, _sir_stdoutopts);
}

bool sir_stderrlevels(sir_levels levels) {
    _sir_defaultlevels(&levels, sir_stderr_def_lvls);
    sir_update_config_data data = {SIRU_LEVELS, &levels, NULL, NULL, NULL};
    return _sir_writeinit(&data, _sir_stderrlevels);
}

bool sir_stderropts(sir_options opts) {
    _sir_defaultopts(&opts, sir_stderr_def_opts);
    sir_update_config_data data = {SIRU_OPTIONS, NULL, &opts, NULL, NULL};
    return _sir_writeinit(&data, _sir_stderropts);
}

bool sir_sysloglevels(sir_levels levels) {
#if !defined(SIR_NO_SYSTEM_LOGGERS)
    _sir_defaultlevels(&levels, sir_syslog_def_lvls);
    sir_update_config_data data = {SIRU_LEVELS, &levels, NULL, NULL, NULL};
    return _sir_writeinit(&data, _sir_sysloglevels);
#else
    SIR_UNUSED(levels);
    return _sir_seterror(_SIR_E_UNAVAIL);
#endif
}

bool sir_syslogopts(sir_options opts) {
#if !defined(SIR_NO_SYSTEM_LOGGERS)
    _sir_defaultopts(&opts, sir_syslog_def_opts);
    sir_update_config_data data = {SIRU_OPTIONS, NULL, &opts, NULL, NULL};
    return _sir_writeinit(&data, _sir_syslogopts);
#else
    SIR_UNUSED(opts);
    return _sir_seterror(_SIR_E_UNAVAIL);
#endif
}

bool sir_syslogid(const char* identity) {
#if !defined(SIR_NO_SYSTEM_LOGGERS)
    sir_update_config_data data = {SIRU_SYSLOG_ID, NULL, NULL, identity, NULL};
    return _sir_writeinit(&data, _sir_syslogid);
#else
    SIR_UNUSED(identity);
    return _sir_seterror(_SIR_E_UNAVAIL);
#endif
}

bool sir_syslogcat(const char* category) {
#if !defined(SIR_NO_SYSTEM_LOGGERS)
    sir_update_config_data data = {SIRU_SYSLOG_CAT, NULL, NULL, NULL, category};
    return _sir_writeinit(&data, _sir_syslogcat);
#else
    SIR_UNUSED(category);
    return _sir_seterror(_SIR_E_UNAVAIL);
#endif
}

const char* sir_getversionstring(void) {
    return _SIR_MK_VER_STR(SIR_VERSION_MAJOR, SIR_VERSION_MINOR, SIR_VERSION_PATCH);
}

uint32_t sir_getversionhex(void) {
    return SIR_VERSION_HEX;
}

bool sir_isprerelease(void) {
    return (!SIR_VERSION_IS_RELEASE);
}
