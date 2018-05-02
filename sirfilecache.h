/*!
 * \file sirfilecache.h
 * \brief Internal definitions for log file management in the SIR library.
 * \author Ryan Matthew Lederman <lederman@gmail.com>
 */
#ifndef _SIR_FILECACHE_H_INCLUDED
#define _SIR_FILECACHE_H_INCLUDED

#include "sirplatform.h"
#include "sirtypes.h"

/*! \cond PRIVATE */

typedef bool (*sir_fcache_pred)(const void* match, sirfile* iter);

int _sir_addfile(const sirchar_t* path, sir_levels levels, sir_options opts);
bool _sir_remfile(int id);

sirfile* _sirfile_create(const sirchar_t* path, sir_levels levels, sir_options opts);
bool     _sir_write(sirfile* sf, const sirchar_t* output);
bool     _sirfile_writeheader(sirfile* sf, const sirchar_t* msg);
bool _sirfile_needsroll(sirfile* sf);
bool _sirfile_roll(sirfile* sf);
void     _sirfile_destroy(sirfile* sf);
bool     _sirfile_validate(sirfile* sf);

int  _sir_fcache_add(sirfcache* sfc, const sirchar_t* path, sir_levels levels, sir_options opts);
bool _sir_fcache_rem(sirfcache* sfc, int id);

bool _sir_fcache_pred_path(const void* match, sirfile* iter);
sirfile* _sir_fcache_find(sirfcache* sfc, const void* match, sir_fcache_pred pred);

bool _sir_fcache_destroy(sirfcache* sfc);
bool _sir_fcache_dispatch(sirfcache* sfc, sir_level level, siroutput* output);

FILE* _sir_fopen(const sirchar_t* path);
void  _sir_fclose(FILE** f);
void  _sir_fflush(FILE* f);
bool _sir_fflush_all();


/*! \endcond */

#endif /* !_SIR_FILECACHE_H_INCLUDED */
