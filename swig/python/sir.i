/*
 * sir.i
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2023
 * Version:   2.2.4
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
%ignore sir_makeinit;
%rename(MakeInit) sir_makeinit;
%ignore sir_init;
%rename(Init) sir_init;
%ignore sir_cleanup;
%rename(Cleanup) sir_cleanup;
%ignore sir_geterror;
%rename(GetError) sir_geterror;
%ignore sir_debug;
%rename(Debug) sir_debug;
%ignore sir_info;
%rename(Info) sir_info;
%ignore sir_notice;
%rename(Notice) sir_notice;
%ignore sir_warn;
%rename(Warn) sir_warn;
%ignore sir_error;
%rename(Error) sir_error;
%ignore sir_crit;
%rename(Crit) sir_crit;
%ignore sir_alert;
%rename(Alert) sir_alert;
%ignore sir_emerg;
%rename(Emerg) sir_emerg;
%ignore sir_addfile;
%rename(AddFile) sir_addfile;
%ignore sir_remfile;
%rename(RemFile) sir_remfile;
%ignore sir_loadplugin;
%rename(LoadPlugin) sir_loadplugin;
%ignore sir_unloadplugin;
%rename(UnloadPlugin) sir_unloadplugin;
%ignore sir_filelevels;
%rename(SetFileLevels) sir_filelevels;
%ignore sir_fileopts;
%rename(SetFileOptions) sir_fileopts;
%ignore sir_settextstyle;
%rename(SetTextStyle) sir_settextstyle;
%ignore sir_resettextstyles;
%rename(ResetTextStyles) sir_resettextstyles;
%ignore sir_makergb;
%rename(MakeRGB) sir_makergb;
%ignore sir_setcolormode;
%rename(SetColorMode) sir_setcolormode;
%ignore sir_stdoutlevels;
%rename(SetStdOutLevels) sir_stdoutlevels;
%ignore sir_stdoutopts;
%rename(SetStdOutOptions) sir_stdoutopts;
%ignore sir_stderrlevels;
%rename(SetStdErrLevels) sir_stderrlevels;
%ignore sir_stderropts;
%rename(SetStdErrOptions) sir_stderropts;
%ignore sir_sysloglevels;
%rename(SetSysLogLevels) sir_sysloglevels;
%ignore sir_syslogopts;
%rename(SetSysLogOptions) sir_syslogopts;
%ignore sir_syslogid;
%rename(SetSysLogID) sir_syslogid;
%ignore sir_syslogcat;
%rename(SetSysLogCategory) sir_syslogcat;
%ignore sir_getversionstring;
%rename(GetVersionString) sir_getversionstring;
%ignore sir_getversionhex;
%rename(GetVersionHex) sir_getversionhex;
%ignore sir_isprerelease;
%rename(IsPreRelease) sir_isprerelease;

/* need this for uint32_t and friends. */
%import <stdint.i>

/* module exports (public functions + types) */
%include "../../include/sir/types.h"
%include "../../include/sir.h"
