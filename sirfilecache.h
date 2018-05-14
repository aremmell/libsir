/**
 * @file sirfilecache.h
 * @brief Internal log file management.
 *
 * This file and accompanying source code originated from <https://github.com/ryanlederman/sir>.
 * If you obtained it elsewhere, all bets are off.
 *
 * @author Ryan M. Lederman <lederman@gmail.com>
 * @copyright
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Ryan M. Lederman
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
#ifndef _SIR_FILECACHE_H_INCLUDED
#define _SIR_FILECACHE_H_INCLUDED

#include "sirtypes.h"

/**
 * @addtogroup intern
 * @{
 */

typedef bool (*sir_fcache_pred)(const void* match, sirfile* iter);
typedef void (*sir_fcache_update)(sirfile* si, sir_update_data* data);

sirfileid_t _sir_addfile(const sirchar_t* path, sir_levels levels, sir_options opts);
bool _sir_updatefile(sirfileid_t id, sir_update_data* data);
bool _sir_remfile(sirfileid_t id);

sirfile* _sirfile_create(const sirchar_t* path, sir_levels levels, sir_options opts);
bool     _sirfile_open(sirfile* sf);
void     _sirfile_close(sirfile* sf);
bool     _sir_write(sirfile* sf, const sirchar_t* output);
bool     _sirfile_writeheader(sirfile* sf, const sirchar_t* msg);
bool     _sirfile_needsroll(sirfile* sf);
bool     _sirfile_roll(sirfile* sf, sirchar_t** newpath);
bool     _sirfile_archive(sirfile* sf, const sirchar_t* newpath);
bool     _sirfile_splitpath(sirfile* sf, sirchar_t** name, sirchar_t** ext);
void     _sirfile_destroy(sirfile* sf);
bool     _sirfile_validate(sirfile* sf);
void     _sirfile_update(sirfile* sf, sir_update_data* data);

sirfileid_t _sir_fcache_add(sirfcache* sfc, const sirchar_t* path, sir_levels levels, sir_options opts);
bool _sir_fcache_update(sirfcache* sfc, sirfileid_t id, sir_update_data* data);
bool _sir_fcache_rem(sirfcache* sfc, sirfileid_t id);

bool     _sir_fcache_pred_path(const void* match, sirfile* iter);
bool     _sir_fcache_pred_id(const void* match, sirfile* iter);
sirfile* _sir_fcache_find(sirfcache* sfc, const void* match, sir_fcache_pred pred);

bool _sir_fcache_destroy(sirfcache* sfc);
bool _sir_fcache_dispatch(sirfcache* sfc, sir_level level, siroutput* output,
    size_t* dispatched, size_t* wanted);

FILE* _sir_fopen(const sirchar_t* path);
void  _sir_fclose(FILE** f);
void  _sir_fflush(FILE* f);
bool  _sir_fflush_all(void);

/** @} */

#endif /* !_SIR_FILECACHE_H_INCLUDED */
