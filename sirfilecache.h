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

sirfile* _sirfile_create(const sirchar_t* path, sir_levels levels, sir_options opts);
bool     _sirfile_write(sirfile* sf, const sirchar_t* output);
bool     _sirfile_writeheader(sirfile* sf);
void     _sirfile_destroy(sirfile* sf);
bool     _sirfile_validate(sirfile* sf);

FILE* _sir_fopen(const sirchar_t* path);
void  _sir_fclose(FILE** f);
void  _sir_fflush(FILE* f);
bool _sir_fflush_all();

int  _sir_files_add(sirfiles* sfc, const sirchar_t* path, sir_levels levels, sir_options opts);
bool _sir_files_rem(sirfiles* sfc, int id);
bool _sir_files_destroy(sirfiles* sfc);
bool _sir_files_dispatch(sirfiles* sfc, sir_level level, siroutput* output);

/*! \endcond */

#endif /* !_SIR_FILECACHE_H_INCLUDED */
