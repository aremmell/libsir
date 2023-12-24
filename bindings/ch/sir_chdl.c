/*
 * sir_chdl.c
 *
 * Version: 2.2.5
 *
 * -----------------------------------------------------------------------------
 *
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2018-2024 Jeffrey H. Johnson <trnsz@pobox.com>
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

//-V::1049
#include <ch.h>
#include <stdbool.h>
#include "chsir.h"
#include "sir/internal.h"

EXPORTCH bool
sir_makeinit_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    sirinit*   si;
    bool       retval;

    Ch_VaStart(interp, ap, varg);

    si     = Ch_VaArg(interp, ap, sirinit *);
    retval = sir_makeinit(si);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_init_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    sirinit*   si;
    bool       retval;

    Ch_VaStart(interp, ap, varg);

    si     = Ch_VaArg(interp, ap, sirinit *);
    retval = sir_init(si);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_cleanup_chdl(void) {

    return sir_cleanup();
}

EXPORTCH bool
sir_isprerelease_chdl(void) {

    return sir_isprerelease();
}

EXPORTCH bool
sir_isinitialized_chdl(void) {

    return sir_isinitialized();
}

EXPORTCH uint16_t
sir_geterror_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    char*      message;
    uint16_t   retval;

    Ch_VaStart(interp, ap, varg);

    message = Ch_VaArg(interp, ap, char *);
    retval  = sir_geterror(message);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH void
sir_geterrorinfo_chdl(void *varg) {
    ChInterp_t     interp;
    ChVaList_t     ap;
    sir_errorinfo* err;

    Ch_VaStart(interp, ap, varg);

    err = Ch_VaArg(interp, ap, sir_errorinfo*);
    sir_geterrorinfo(err);

    Ch_VaEnd(interp, ap);
}

EXPORTCH const char*
sir_getversionstring_chdl(void) {

    return sir_getversionstring();
}

EXPORTCH uint32_t
sir_getversionhex_chdl(void) {

    return sir_getversionhex();
}

EXPORTCH bool
sir_debug_chdl(void *varg) {
    ChInterp_t  interp;
    ChVaList_t  ap;
    const char* message;
    bool        retval;

    Ch_VaStart(interp, ap, varg);

    message = Ch_VaArg(interp, ap, const char *);
    retval  = sir_debug("%s", message);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_info_chdl(void *varg) {
    ChInterp_t  interp;
    ChVaList_t  ap;
    const char* message;
    bool        retval;

    Ch_VaStart(interp, ap, varg);

    message = Ch_VaArg(interp, ap, const char *);
    retval  = sir_info("%s", message);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_warn_chdl(void *varg) {
    ChInterp_t  interp;
    ChVaList_t  ap;
    const char* message;
    bool        retval;

    Ch_VaStart(interp, ap, varg);

    message = Ch_VaArg(interp, ap, const char *);
    retval  = sir_warn("%s", message);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_alert_chdl(void *varg) {
    ChInterp_t  interp;
    ChVaList_t  ap;
    const char* message;
    bool        retval;

    Ch_VaStart(interp, ap, varg);

    message = Ch_VaArg(interp, ap, const char *);
    retval  = sir_alert("%s", message);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_notice_chdl(void *varg) {
    ChInterp_t  interp;
    ChVaList_t  ap;
    const char* message;
    bool        retval;

    Ch_VaStart(interp, ap, varg);

    message = Ch_VaArg(interp, ap, const char *);
    retval  = sir_notice("%s", message);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_error_chdl(void *varg) {
    ChInterp_t  interp;
    ChVaList_t  ap;
    const char* message;
    bool        retval;

    Ch_VaStart(interp, ap, varg);

    message = Ch_VaArg(interp, ap, const char *);
    retval  = sir_error("%s", message);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_crit_chdl(void *varg) {
    ChInterp_t  interp;
    ChVaList_t  ap;
    const char* message;
    bool        retval;

    Ch_VaStart(interp, ap, varg);

    message = Ch_VaArg(interp, ap, const char *);
    retval  = sir_crit("%s", message);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_emerg_chdl(void *varg) {
    ChInterp_t  interp;
    ChVaList_t  ap;
    const char* message;
    bool        retval;

    Ch_VaStart(interp, ap, varg);

    message = Ch_VaArg(interp, ap, const char *);
    retval  = sir_emerg("%s", message);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
_sir_setthreadname_chdl(void *varg) {
    ChInterp_t  interp;
    ChVaList_t  ap;
    const char* name;
    bool        retval;

    Ch_VaStart(interp, ap, varg);

    name   = Ch_VaArg(interp, ap, const char *);
    retval = _sir_setthreadname(name);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH long
__sir_nprocs_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    bool       test_mode;
    long       retval;

    Ch_VaStart(interp, ap, varg);

    test_mode = Ch_VaArg(interp, ap, bool);
    retval    = __sir_nprocs(test_mode);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH sirfileid
sir_addfile_chdl(void *varg) {
    ChInterp_t  interp;
    ChVaList_t  ap;
    const char* path;
    sir_levels  levels;
    sir_options opts;
    sirfileid   retval;

    Ch_VaStart(interp, ap, varg);

    path   = Ch_VaArg(interp, ap, const char *);
    levels = Ch_VaArg(interp, ap, sir_levels);
    opts   = Ch_VaArg(interp, ap, sir_options);
    retval = sir_addfile(path, levels, opts);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_remfile_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    sirfileid  id;
    bool       retval;

    Ch_VaStart(interp, ap, varg);

    id     = Ch_VaArg(interp, ap, sirfileid);
    retval = sir_remfile(id);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH sirpluginid
sir_loadplugin_chdl(void *varg) {
    ChInterp_t  interp;
    ChVaList_t  ap;
    const char* path;
    sirpluginid retval;

    Ch_VaStart(interp, ap, varg);

    path   = Ch_VaArg(interp, ap, const char*);
    retval = sir_loadplugin(path);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_unloadplugin_chdl(void *varg) {
    ChInterp_t  interp;
    ChVaList_t  ap;
    sirpluginid id;
    bool        retval;

    Ch_VaStart(interp, ap, varg);

    id     = Ch_VaArg(interp, ap, sirpluginid);
    retval = sir_unloadplugin(id);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_resettextstyles_chdl(void) {

    return sir_resettextstyles();
}

EXPORTCH bool
sir_setcolormode_chdl(void *varg) {
    ChInterp_t    interp;
    ChVaList_t    ap;
    sir_colormode mode;
    bool          retval;

    Ch_VaStart(interp, ap, varg);

    mode   = Ch_VaArg(interp, ap, sir_colormode);
    retval = sir_setcolormode(mode);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_stdoutlevels_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    sir_levels levels;
    bool       retval;

    Ch_VaStart(interp, ap, varg);

    levels = Ch_VaArg(interp, ap, sir_levels);
    retval = sir_stdoutlevels(levels);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_stdoutopts_chdl(void *varg) {
    ChInterp_t  interp;
    ChVaList_t  ap;
    sir_options opts;
    bool        retval;

    Ch_VaStart(interp, ap, varg);

    opts   = Ch_VaArg(interp, ap, sir_options);
    retval = sir_stdoutopts(opts);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_stderrlevels_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    sir_levels levels;
    bool       retval;

    Ch_VaStart(interp, ap, varg);

    levels = Ch_VaArg(interp, ap, sir_levels);
    retval = sir_stderrlevels(levels);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_stderropts_chdl(void *varg) {
    ChInterp_t  interp;
    ChVaList_t  ap;
    sir_options opts;
    bool        retval;

    Ch_VaStart(interp, ap, varg);

    opts   = Ch_VaArg(interp, ap, sir_options);
    retval = sir_stderropts(opts);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_sysloglevels_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    sir_levels levels;
    bool       retval;

    Ch_VaStart(interp, ap, varg);

    levels = Ch_VaArg(interp, ap, sir_levels);
    retval = sir_sysloglevels(levels);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_syslogopts_chdl(void *varg) {
    ChInterp_t  interp;
    ChVaList_t  ap;
    sir_options opts;
    bool        retval;

    Ch_VaStart(interp, ap, varg);

    opts   = Ch_VaArg(interp, ap, sir_options);
    retval = sir_syslogopts(opts);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_syslogid_chdl(void *varg) {
    ChInterp_t  interp;
    ChVaList_t  ap;
    const char* identity;
    bool        retval;

    Ch_VaStart(interp, ap, varg);

    identity = Ch_VaArg(interp, ap, const char*);
    retval   = sir_syslogid(identity);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_syslogcat_chdl(void *varg) {
    ChInterp_t  interp;
    ChVaList_t  ap;
    const char* category;
    bool        retval;

    Ch_VaStart(interp, ap, varg);

    category = Ch_VaArg(interp, ap, const char*);
    retval   = sir_syslogcat(category);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH sir_textcolor
sir_makergb_chdl(void *varg) {
    ChInterp_t    interp;
    ChVaList_t    ap;
    sir_textcolor r;
    sir_textcolor g;
    sir_textcolor b;
    sir_textcolor retval;

    Ch_VaStart(interp, ap, varg);

    r      = Ch_VaArg(interp, ap, sir_textcolor);
    g      = Ch_VaArg(interp, ap, sir_textcolor);
    b      = Ch_VaArg(interp, ap, sir_textcolor);
    retval = sir_makergb(r, g, b);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_settextstyle_chdl(void *varg) {
    ChInterp_t    interp;
    ChVaList_t    ap;
    sir_level     level;
    sir_textattr  attr;
    sir_textcolor fg;
    sir_textcolor bg;
    bool          retval;

    Ch_VaStart(interp, ap, varg);

    level  = Ch_VaArg(interp, ap, sir_level);
    attr   = Ch_VaArg(interp, ap, sir_textattr);
    fg     = Ch_VaArg(interp, ap, sir_textcolor);
    bg     = Ch_VaArg(interp, ap, sir_textcolor);
    retval = sir_settextstyle(level, attr, fg, bg);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_filelevels_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    sirfileid  id;
    sir_levels levels;
    bool       retval;

    Ch_VaStart(interp, ap, varg);

    id     = Ch_VaArg(interp, ap, sirfileid);
    levels = Ch_VaArg(interp, ap, sir_levels);
    retval = sir_filelevels(id, levels);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_fileopts_chdl(void *varg) {
    ChInterp_t  interp;
    ChVaList_t  ap;
    sirfileid   id;
    sir_options opts;
    bool        retval;

    Ch_VaStart(interp, ap, varg);

    id     = Ch_VaArg(interp, ap, sirfileid);
    opts   = Ch_VaArg(interp, ap, sir_options);
    retval = sir_fileopts(id, opts);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH char*
_sir_strremove_chdl(void *varg) {
    ChInterp_t  interp;
    ChVaList_t  ap;
    char*       str;
    const char* sub;
    char*       retval;

    Ch_VaStart(interp, ap, varg);

    str    = Ch_VaArg(interp, ap, char*);
    sub    = Ch_VaArg(interp, ap, const char*);
    retval = _sir_strremove(str, sub);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH char*
_sir_strsqueeze_chdl(void *varg) {
    ChInterp_t  interp;
    ChVaList_t  ap;
    char*       str;
    char*       retval;

    Ch_VaStart(interp, ap, varg);

    str    = Ch_VaArg(interp, ap, char*);
    retval = _sir_strsqueeze(str);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH char*
_sir_strredact_chdl(void *varg) {
    ChInterp_t  interp;
    ChVaList_t  ap;
    char*       str;
    const char* sub;
    char        c;
    char*       retval;

    Ch_VaStart(interp, ap, varg);

    str    = Ch_VaArg(interp, ap, char*);
    sub    = Ch_VaArg(interp, ap, const char*);
    c      = Ch_VaArg(interp, ap, char);
    retval = _sir_strredact(str, sub, c);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH char*
_sir_strreplace_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    char*      str;
    char       c;
    char       n;
    char*      retval;

    Ch_VaStart(interp, ap, varg);

    str    = Ch_VaArg(interp, ap, char*);
    c      = Ch_VaArg(interp, ap, char);
    n      = Ch_VaArg(interp, ap, char);
    retval = _sir_strreplace(str, c, n);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH size_t
_sir_strcreplace_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    char*      str;
    char       c;
    char       n;
    int32_t    max;
    size_t     retval;

    Ch_VaStart(interp, ap, varg);

    str    = Ch_VaArg(interp, ap, char*);
    c      = Ch_VaArg(interp, ap, char);
    n      = Ch_VaArg(interp, ap, char);
    max    = Ch_VaArg(interp, ap, int32_t);
    retval = _sir_strcreplace(str, c, n, max);

    Ch_VaEnd(interp, ap);

    return retval;
}
