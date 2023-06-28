/**
 * @file sir.c
 * @brief Public interface to libsir
 *
 * The functions and types defined here comprise the entire set intended for
 * use by an implementor of the library-unless modification is desired.
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
#include "sir.h"
#include "sirinternal.h"
#include "sirfilecache.h"
#include "sirtextstyle.h"
#include "sirdefaults.h"

bool sir_makeinit(sirinit* si) {
    return _sir_makeinit(si);
}

bool sir_init(sirinit* si) {
    return _sir_init(si);
}

bool sir_cleanup(void) {
    return _sir_cleanup();
}

uint16_t sir_geterror(char message[SIR_MAXERROR]) {
    return _sir_geterrcode(_sir_geterror(message));
}

bool sir_debug(const char* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_DEBUG, format, args);
    _SIR_L_END(args);
    return r;
}

bool sir_info(const char* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_INFO, format, args);
    _SIR_L_END(args);
    return r;
}

bool sir_notice(const char* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_NOTICE, format, args);
    _SIR_L_END(args);
    return r;
}

bool sir_warn(const char* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_WARN, format, args);
    _SIR_L_END(args);
    return r;
}

bool sir_error(const char* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_ERROR, format, args);
    _SIR_L_END(args);
    return r;
}

bool sir_crit(const char* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_CRIT, format, args);
    _SIR_L_END(args);
    return r;
}

bool sir_alert(const char* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_ALERT, format, args);
    _SIR_L_END(args);
    return r;
}

bool sir_emerg(const char* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_EMERG, format, args);
    _SIR_L_END(args);
    return r;
}

sirfileid_t sir_addfile(const char* path, sir_levels levels, sir_options opts) {
    return _sir_addfile(path, levels, opts);
}

bool sir_remfile(sirfileid_t id) {
    return _sir_remfile(id);
}

bool sir_filelevels(sirfileid_t id, sir_levels levels) {
    _sir_defaultlevels(&levels, sir_file_def_lvls);
    sir_update_config_data data = {SIRU_LEVELS, &levels, NULL, NULL, NULL};
    return _sir_updatefile(id, &data);
}

bool sir_fileopts(sirfileid_t id, sir_options opts) {
    _sir_defaultopts(&opts, sir_file_def_opts);
    sir_update_config_data data = {SIRU_OPTIONS, NULL, &opts, NULL, NULL};
    return _sir_updatefile(id, &data);
}

bool sir_settextstyle(sir_level level, sir_textstyle style) {
    return _sir_settextstyle(level, style);
}

bool sir_resettextstyles(void) {
    return _sir_resettextstyles();
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
    _SIR_UNUSED(levels);
    _sir_seterror(_SIR_E_UNAVAIL);
    return false;
#endif
}

bool sir_syslogopts(sir_options opts) {
#if !defined(SIR_NO_SYSTEM_LOGGERS)
    _sir_defaultopts(&opts, sir_syslog_def_opts);
    sir_update_config_data data = {SIRU_OPTIONS, NULL, &opts, NULL, NULL};
    return _sir_writeinit(&data, _sir_syslogopts);
#else
    _SIR_UNUSED(opts);
    _sir_seterror(_SIR_E_UNAVAIL);
    return false;
#endif
}

bool sir_syslogid(const char* identity) {
#if !defined(SIR_NO_SYSTEM_LOGGERS)
    sir_update_config_data data = {SIRU_SYSLOG_ID, NULL, NULL, identity, NULL};
    return _sir_writeinit(&data, _sir_syslogid);
#else
    _SIR_UNUSED(identity);
    _sir_seterror(_SIR_E_UNAVAIL);
    return false;
#endif
}

bool sir_syslogcat(const char* category) {
#if !defined(SIR_NO_SYSTEM_LOGGERS)
    sir_update_config_data data = {SIRU_SYSLOG_CAT, NULL, NULL, NULL, category};
    return _sir_writeinit(&data, _sir_syslogcat);
#else
    _SIR_UNUSED(category);
    _sir_seterror(_SIR_E_UNAVAIL);
    return false;
#endif
}
