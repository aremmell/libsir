/*
 * filecache.h
 *
 * Version: 2.2.5
 *
 * -----------------------------------------------------------------------------
 *
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2018-2024 Ryan M. Lederman <lederman@gmail.com>
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

#ifndef _SIR_FILECACHE_H_INCLUDED
# define _SIR_FILECACHE_H_INCLUDED

# include "sir/types.h"

typedef bool (*sir_fcache_pred)(const void* match, const sirfile* iter);

sirfileid _sir_addfile(const char* path, sir_levels levels, sir_options opts);
bool _sir_updatefile(sirfileid id, const sir_update_config_data* data);
bool _sir_remfile(sirfileid id);

sirfile* _sirfile_create(const char* path, sir_levels levels, sir_options opts);
bool _sirfile_open(sirfile* sf);
void _sirfile_close(sirfile* sf);
bool _sirfile_write(sirfile* sf, const char* output);
bool _sirfile_writeheader(sirfile* sf, const char* msg);
bool _sirfile_needsroll(sirfile* sf);
bool _sirfile_roll(sirfile* sf, char** newpath);
void _sirfile_rollifneeded(sirfile* sf);
bool _sirfile_archive(sirfile* sf, const char* newpath);
bool _sirfile_splitpath(const sirfile* sf, char** name, char** ext);
void _sirfile_destroy(sirfile** sf);
bool _sirfile_validate(const sirfile* sf);
bool _sirfile_update(sirfile* sf, const sir_update_config_data* data);

sirfileid _sir_fcache_add(sirfcache* sfc, const char* path, sir_levels levels,
    sir_options opts);
bool _sir_fcache_update(const sirfcache* sfc, sirfileid id, const sir_update_config_data* data);
bool _sir_fcache_rem(sirfcache* sfc, sirfileid id);
void _sir_fcache_shift(sirfcache* sfc, size_t idx);

bool _sir_fcache_pred_path(const void* match, const sirfile* iter);
bool _sir_fcache_pred_id(const void* match, const sirfile* iter);
sirfile* _sir_fcache_find(const sirfcache* sfc, const void* match, sir_fcache_pred pred);

bool _sir_fcache_destroy(sirfcache* sfc);
bool _sir_fcache_dispatch(const sirfcache* sfc, sir_level level, sirbuf* buf,
    size_t* dispatched, size_t* wanted);

void _sir_fflush(FILE* f);

#endif /* !_SIR_FILECACHE_H_INCLUDED */
