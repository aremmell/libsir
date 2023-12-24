/*
 * sir.i
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

/* module name. */
%module sir

/* this is passed verbatim to the C wrapper. */
%{
#include "sir.h"
%}

/* strip prefix sir* from symbols. */
%ignore sirinit;
%rename(InitData) sirinit;
%ignore sir_level;
%rename(Level) sir_level;
%ignore sir_levels;
%rename(Levels) sir_levels;
%ignore sir_option;
%rename(Option) sir_option;
%ignore sir_options;
%rename(Options) sir_options;
%ignore sirfileid;
%rename(FileID) sirfileid;
%ignore sirpluginid;
%rename(PluginID) sirpluginid;
%ignore sir_textattr;
%rename(TextAttr) sir_textattr;
%ignore sir_textcolor;
%rename(TextColor) sir_textcolor;
%ignore sir_colormode;
%rename(ColorMode) sir_colormode;
%ignore sir_errorinfo;
%rename(ErrorInfo) sir_errorinfo;

/* constants/enums. */
%ignore SIRL_NONE;
%rename(LVL_NONE) SIRL_NONE;

%ignore SIRL_EMERG;
%rename(LVL_EMERG) SIRL_EMERG;

%ignore SIRL_ALERT;
%rename(LVL_ALERT) SIRL_ALERT;

%ignore SIRL_CRIT;
%rename(LVL_CRIT) SIRL_CRIT;

%ignore SIRL_ERROR;
%rename(LVL_ERROR) SIRL_ERROR;

%ignore SIRL_WARN;
%rename(LVL_WARN) SIRL_WARN;

%ignore SIRL_NOTICE;
%rename(LVL_NOTICE) SIRL_NOTICE;

%ignore SIRL_INFO;
%rename(LVL_INFO) SIRL_INFO;

%ignore SIRL_DEBUG;
%rename(LVL_DEBUG) SIRL_DEBUG;

%ignore SIRL_ALL;
%rename(LVL_ALL) SIRL_ALL;

%ignore SIRL_DEFAULT;
%rename(LVL_DEFAULT) SIRL_DEFAULT;

%ignore SIRO_ALL;
%rename(OPT_ALL) SIRO_ALL;

%ignore SIRO_NOTIME;
%rename(OPT_NOTIME) SIRO_NOTIME;

%ignore SIRO_NOMSEC;
%rename(OPT_NOMSEC) SIRO_NOMSEC;

%ignore SIRO_NOHOST;
%rename(OPT_NOHOST) SIRO_NOHOST;

%ignore SIRO_NOLEVEL;
%rename(OPT_NOLEVEL) SIRO_NOLEVEL;

%ignore SIRO_NONAME;
%rename(OPT_NONAME) SIRO_NONAME;

%ignore SIRO_NOPID;
%rename(OPT_NOPID) SIRO_NOPID;

%ignore SIRO_NOTID;
%rename(OPT_NOTID) SIRO_NOTID;

%ignore SIRO_NOHDR;
%rename(OPT_NOHDR) SIRO_NOHDR;

%ignore SIRO_MSGONLY;
%rename(OPT_MSGONLY) SIRO_MSGONLY;

%ignore SIRO_DEFAULT;
%rename(OPT_DEFAULT) SIRO_DEFAULT;

%ignore SIRCM_16;
%rename(CLRMODE_16) SIRCM_16;

%ignore SIRCM_256;
%rename(CLRMODE_256) SIRCM_256;

%ignore SIRCM_RGB;
%rename(CLRMODE_RGB) SIRCM_RGB;

%ignore SIRCM_INVALID;
%rename(CLRMODE_INVALID) SIRCM_INVALID;

%ignore SIRTA_NORMAL;
%rename(TEXT_NORMAL) SIRTA_NORMAL;

%ignore SIRTA_BOLD;
%rename(TEXT_BOLD) SIRTA_BOLD;

%ignore SIRTA_DIM;
%rename(TEXT_DIM) SIRTA_DIM;

%ignore SIRTA_EMPH;
%rename(TEXT_EMPH) SIRTA_EMPH;

%ignore SIRTA_ULINE;
%rename(TEXT_ULINE) SIRTA_ULINE;

%ignore SIRTC_BLACK;
%rename(TXTCLR_BLACK) SIRTC_BLACK;

%ignore SIRTC_RED;
%rename(COLOR_RED) SIRTC_RED;

%ignore SIRTC_GREEN;
%rename(COLOR_GREEN) SIRTC_GREEN;

%ignore SIRTC_YELLOW;
%rename(COLOR_YELLOW) SIRTC_YELLOW;

%ignore SIRTC_BLUE;
%rename(COLOR_BLUE) SIRTC_BLUE;

%ignore SIRTC_MAGENTA;
%rename(COLOR_MAGENTA) SIRTC_MAGENTA;

%ignore SIRTC_CYAN;
%rename(COLOR_CYAN) SIRTC_CYAN;

%ignore SIRTC_LGRAY;
%rename(COLOR_LGRAY) SIRTC_LGRAY;

%ignore SIRTC_DGRAY;
%rename(COLOR_DGRAY) SIRTC_DGRAY;

%ignore SIRTC_BRED;
%rename(COLOR_BRED) SIRTC_BRED;

%ignore SIRTC_BGREEN;
%rename(COLOR_BGREEN) SIRTC_BGREEN;

%ignore SIRTC_BYELLOW;
%rename(COLOR_BYELLOW) SIRTC_BYELLOW;

%ignore SIRTC_BBLUE;
%rename(COLOR_BBLUE) SIRTC_BBLUE;

%ignore SIRTC_BMAGENTA;
%rename(COLOR_BMAGENTA) SIRTC_BMAGENTA;

%ignore SIRTC_BCYAN;
%rename(COLOR_BCYAN) SIRTC_BCYAN;

%ignore SIRTC_WHITE;
%rename(COLOR_WHITE) SIRTC_WHITE;

%ignore SIRTC_DEFAULT;
%rename(COLOR_DEFAULT) SIRTC_DEFAULT;

/* functions. */
%ignore sir_makeinit;
%rename(makeinit) sir_makeinit;
%ignore sir_init;
%rename(init) sir_init;
%ignore sir_cleanup;
%rename(cleanup) sir_cleanup;
%ignore sir_geterror;
%rename(geterror) sir_geterror;
%ignore sir_geterrorinfo;
%rename (geterrorinfo) sir_geterrorinfo;
%ignore sir_debug;
%rename(debug) sir_debug;
%ignore sir_info;
%rename(info) sir_info;
%ignore sir_notice;
%rename(notice) sir_notice;
%ignore sir_warn;
%rename(warn) sir_warn;
%ignore sir_error;
%rename(error) sir_error;
%ignore sir_crit;
%rename(crit) sir_crit;
%ignore sir_alert;
%rename(alert) sir_alert;
%ignore sir_emerg;
%rename(emerg) sir_emerg;
%ignore sir_addfile;
%rename(addfile) sir_addfile;
%ignore sir_remfile;
%rename(remfile) sir_remfile;
%ignore sir_loadplugin;
%rename(loadplugin) sir_loadplugin;
%ignore sir_unloadplugin;
%rename(unloadplugin) sir_unloadplugin;
%ignore sir_filelevels;
%rename(setfilelevels) sir_filelevels;
%ignore sir_fileopts;
%rename(setfileoptions) sir_fileopts;
%ignore sir_settextstyle;
%rename(settextstyle) sir_settextstyle;
%ignore sir_resettextstyles;
%rename(resettextstyles) sir_resettextstyles;
%ignore sir_makergb;
%rename(makergb) sir_makergb;
%ignore sir_setcolormode;
%rename(setcolormode) sir_setcolormode;
%ignore sir_stdoutlevels;
%rename(setstdoutlevels) sir_stdoutlevels;
%ignore sir_stdoutopts;
%rename(setstdoutoptions) sir_stdoutopts;
%ignore sir_stderrlevels;
%rename(setstderrlevels) sir_stderrlevels;
%ignore sir_stderropts;
%rename(setstderroptions) sir_stderropts;
%ignore sir_sysloglevels;
%rename(setsysloglevels) sir_sysloglevels;
%ignore sir_syslogopts;
%rename(setsyslogoptions) sir_syslogopts;
%ignore sir_syslogid;
%rename(setsyslogid) sir_syslogid;
%ignore sir_syslogcat;
%rename(setsyslogcategory) sir_syslogcat;
%ignore sir_getversionstring;
%rename(getversionstring) sir_getversionstring;
%ignore sir_getversionhex;
%rename(getversionhex) sir_getversionhex;
%ignore sir_isprerelease;
%rename(isprerelease) sir_isprerelease;

/* need this for uint32_t and friends. */
%import <stdint.i>

/* module exports (public functions + types) */
%include "../../include/sir/types.h"
%include "../../include/sir.h"
