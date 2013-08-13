/*
 *  sir.c : ANSI C Implementation of Standard Incident Reporter (SIR)
 *
 *  Copyright 2003-2004 - All Rights Reserved
 *
 *  Author:
 *    Ryan Matthew Lederman
 *    ryan@ript.net
 *
 *  Date:
 *    January 02, 2005
 *
 *  Version: 1.0.4
 *
 *  License:
 *    This software is provided to the end user "as-is", with no warranty
 *    implied or otherwise.  The author is for all intents and purposes
 *    the owner of this source code.  The author grants the end user the
 *    privilege to use this software in any application, commercial or
 *    otherwise, with the following restrictions:
 *      1.) If the end user wishes to modify this source code, he/she must
 *       not distribute the source code as the original source code, and
 *       must clearly document the changes made to the source code in any
 *       distribution of said source code.
 *      2.) This license agreement must always be displayed in any version
 *       of the source code.
 *    The author will not be held liable for ANY damage, loss of data,
 *    loss of income, or any other form of loss that results directly or 
 *    indirectly from the use of this software.
 *
 */
#include "sir.h"

/* 
  Global state variables
 */
SIRSTRUCT        sir_s;
SIRFILECACHE     sir_fc;
SIRCALLBACKCACHE sir_cc;

/*
 * FUNCTION: Sir_Init()
 * 
 * SYNOPSIS: Initializes the Sir system for
 * use in an application.  This must be the
 * first function called.
 * 
 * ARGUMENTS: Pointer to a SIRSTRUCT structure
 * that contains the options being requested
 * from the system.
 * 
 * RETURN VALUE: Returns 0 if successful, or -1
 * if an error occurs.
 * 
*/
int Sir_Init(PSIRSTRUCT pss) {
	
  int r = -1;

  if (NULL != pss) {

    if (0UL == pss->opts) { pss->opts = SIRO_DEFAULT; }

    if (0 != _IsValidSirStruct(pss)) {

      Sir_Reset();
      memcpy(&sir_s, pss, sizeof(SIRSTRUCT));
      r = 0;

    }

  }

	return r;
}

/*
 * FUNCTION: Sir_Reset()
 * 
 * SYNOPSIS: Frees resources allocated by
 * Sir, and resets the internal state variables.
 * 
 * ARGUMENTS: None
 * 
 * RETURN VALUE: None
 * 
*/
void Sir_Reset(void) {

	_SirFileCache_Destroy(&sir_fc);
	_SirCallbackCache_Destroy(&sir_cc);

	memset(&sir_s,  0, sizeof(SIRSTRUCT));
	memset(&sir_fc, 0, sizeof(SIRFILECACHE));
	memset(&sir_cc, 0, sizeof(SIRCALLBACKCACHE));

}

/*
 * FUNCTION: Sir_GetOpts()
 * 
 * SYNOPSIS: Retrieves the bitmask of options
 * in use by the Sir system.
 * 
 * ARGUMENTS: None
 * 
 * RETURN VALUE: Returns an unsigned long bitmask
 * 
*/
u_long Sir_GetOpts(void) { return sir_s.opts; }

/*
 * FUNCTION: Sir()
 * 
 * SYNOPSIS: Sir() is the heart of the Sir system.
 * Use this function to send the output to all associated
 * output destinations.  Any destinations with one or more
 * of the bits in <type> will recieve the output.
 * 
 * ARGUMENTS: Unsigned long bitmask of output types
 * to route the output to, and the output (a'la printf())
 * 
 * RETURN VALUE: Returns 0 if successful, or -1
 * if an error occurs.
 * 
*/
int Sir(u_long type, TCHAR *out, ...) {

	TCHAR output[SIR_MAXPRINT]  = {0};
	TCHAR *timestr              = NULL;
	va_list valist              = {0};
	PSIRBUF pbuf                = NULL;
	time_t  now                 = 0;
  int r                       = -1;

	pbuf = _Sirbuf_Create();

  if (NULL != pbuf) {

    if (sir_s.opts & SIRO_TIMESTAMP) {

      time(&now);
      timestr = sctime(&now);
      timestr[sstrlen(timestr)-1] = _T('\0');

      _Sirbuf_Append(pbuf, timestr);
      _Sirbuf_Append(pbuf, _T(" : "));

    }

    va_start(valist, out);
    svsnprintf(output, SIR_MAXPRINT, out, valist);
    va_end(valist);

    if (0 == _Sirbuf_Append(pbuf, output)) {

      if (sir_s.opts & SIRO_LF) {

        _Sirbuf_Append(pbuf, _T("\n"));

      } else if (sir_s.opts & SIRO_CRLF) {

        _Sirbuf_Append(pbuf, _T("\r\n"));

      }

      r = _Sir_SendToDestinations(pbuf->ptr, type);

    }

    _Sirbuf_Destroy(pbuf);
  }

	return r; 
}

/*
 * FUNCTION: Sir_AddFile()
 * 
 * SYNOPSIS: Adds a disk file as an output destination
 * maintained by the Sir system.  The file will recieve
 * output whenever Sir() is called with any of the flags
 * you specify in <mask>.
 * 
 * ARGUMENTS: Pointer to a string that contains
 * the path of the file, and an unsigned long bitmask
 * of the types you wish to associate with this file.
 * (SIRT_*)
 * 
 * RETURN VALUE: Returns 0 if successful, or -1
 * if an error occurs.
 * 
*/
int Sir_AddFile(TCHAR *path, u_long mask) {

	PSIRFILE pf = NULL;
  int r       = -1;

  if (strptr(path) && (0UL != mask)) {

    pf = _SirFile_Create(path, mask);

    if (NULL != pf) {

      r = _SirFileCache_AddFile(&sir_fc, pf);

    }

  }

  return r;
}

/*
 * FUNCTION: Sir_AddCallback()
 * 
 * SYNOPSIS: Adds a callback function as an output destination
 * maintained by the Sir system.  The function will be called
 * whenever Sir() is called with any of the flags you specify 
 * in <mask> (SIRT_*).
 * 
 * ARGUMENTS: Pointer to a SIRCALLBACK-type function (sir.h),
 * unsigned long bitmask of types to associate with the function,
 * and an unsigned long that will be maintained by the Sir system
 * and sent to the function whenever it is called.
 * 
 * RETURN VALUE: Returns 0 if successful, or -1
 * if an error occurs.
 * 
*/
int Sir_AddCallback(SIRCALLBACK pfn, u_long mask, u_long data) {
	
	PSIRCALLBACKOBJ pco = NULL;
  int r               = -1;

  if ((NULL != pfn) && (0UL != mask)) {

    pco = _SirCallbackObj_Create(pfn, mask, data);

    if (NULL != pco) {

      r = _SirCallbackCache_AddCallback(&sir_cc, pco);

    }

  }

	return r;
}

/*
 * FUNCTION: Sir_RemFile()
 * 
 * SYNOPSIS: Removes an existing disk file
 * from the Sir system's managed queue.
 * 
 * ARGUMENTS: Pointer to a string that contains
 * the path of the file.
 * 
 * RETURN VALUE: Returns 0 if successful, or -1
 * if an error occurs.
 * 
*/
int Sir_RemFile(TCHAR *path) {

	return _SirFileCache_RemFile(&sir_fc, path);
}

/*
 * FUNCTION: Sir_RemCallback()
 * 
 * SYNOPSIS: Removes an existing callback function
 * from the Sir system's managed queue.
 * 
 * ARGUMENTS: Pointer to the function to remove.
 * 
 * RETURN VALUE: Returns 0 if successful, or -1
 * if an error occurs.
 * 
*/
int Sir_RemCallback(SIRCALLBACK pfn) {

	return _SirCallbackCache_RemCallback(&sir_cc, pfn);
}

/*
 * FUNCTION: Sir_Cleanup()
 * 
 * SYNOPSIS: Frees resources allocated by
 * Sir, and resets the internal state variables.
 * Functionally equivalent to Sir_Reset().
 * 
 * ARGUMENTS: None
 * 
 * RETURN VALUE: None
 * 
*/
void Sir_Cleanup(void) { Sir_Reset(); }

/* 
  Internal Sir functions;
  These are undocumented because
  you should not need to access
  them externally.
 */

int _Sir_SendToDestinations(TCHAR *output, u_long mask) {

  int r = -1;

  if (strptr(output) && (0UL != mask)) {

#ifdef _DEBUG
    if (mask & SIRT_DEBUG)   {
    
      if (sir_s.opts & SIRO_CRLF && sir_s.opts & SIRO_NODBGCRLF) {

        output[sstrlen(output) - 2] = _T('\n');
        output[sstrlen(output) - 1] = _T('\0');

        _Sir_Dispatch(output, SIRT_DEBUG);

      }
    
    }
#endif // !_DEBUG
	  if (mask & SIRT_ERROR)   { _Sir_Dispatch(output, SIRT_ERROR); }
	  if (mask & SIRT_WARNING) { _Sir_Dispatch(output, SIRT_WARNING); }
	  if (mask & SIRT_FATAL)   { _Sir_Dispatch(output, SIRT_FATAL); }
	  if (mask & SIRT_LOG)     { _Sir_Dispatch(output, SIRT_LOG); }
	  if (mask & SIRT_SCREEN)  { _Sir_Dispatch(output, SIRT_SCREEN); }
	  if (sir_s.opts & SIRO_FILES)     { _SirFileCache_Dispatch(&sir_fc, output, mask); }
	  if (sir_s.opts & SIRO_CALLBACKS) { _SirCallbackCache_Dispatch(&sir_cc, output, mask); }

    r = 0;

  }

	return r;
}

void _Sir_Dispatch(TCHAR *output, u_long type) {

#ifdef _DEBUG
#ifdef Sir_Debug
	if (sir_s.f_debug & type) { Sir_Debug(output); }
#endif /* _!Sir_Debug */
#endif // !_DEBUG

	if (sir_s.f_stderr & type) { sfprintf(stderr, output); }

	if (sir_s.f_stdout & type) { sfprintf(stdout, output); }

}

int _Sir_File(TCHAR *path, TCHAR *output) {

  int r = -1;

  if (strptr(path) && strptr(output)) {

    FILE *f = NULL;

    f = sfopen(path, _T("a'"));

    if (NULL != f) {

      if (0 == fwrite(output, sizeof(TCHAR), sstrlen(output), f)) {

        if ((0 != feof(f)) && (0 == ferror(f))) {

          r = 0;

        }

      } else {

        r = 0;

      }

      fclose(f);

    }

  }

	return r; 
}

PSIRBUF _Sirbuf_Create(void) {

	PSIRBUF pbuf = (PSIRBUF)calloc(1U, sizeof(SIRBUF));

  if (NULL != pbuf) {

		pbuf->ptr = (TCHAR *)malloc(0U);

    if (NULL!= pbuf->ptr) {

      pbuf->size = 0U;
      return pbuf;

    }

  }

  return (PSIRBUF)(NULL);
}

int _Sirbuf_Append(PSIRBUF pbuf, TCHAR *str) {

  size_t copy = 0U;
  int r       = -1;

  if ((NULL != pbuf) && (NULL != pbuf->ptr)) {

    if (strptr(str)) {

      copy = (sstrlen(str) * sizeof(TCHAR));

      if (0U < copy) {

        pbuf->ptr = (TCHAR *)realloc(pbuf->ptr, pbuf->size + copy + sizeof(TCHAR));

        if (NULL != pbuf->ptr) {

          memset(((char *)pbuf->ptr + pbuf->size), 0, copy + sizeof(TCHAR));
          memcpy(((char *)pbuf->ptr + pbuf->size), str, copy);

          pbuf->size += copy;

          r = 0L;
        }       

      }

    }

  }

	return r;
}

int _Sirbuf_Destroy(PSIRBUF pbuf) {

	if ((NULL != pbuf) && (NULL != pbuf->ptr)) {

		free(pbuf->ptr);
		free(pbuf);

		return 0;
	}

	return -1;
}

PSIRFILE _SirFile_Create(TCHAR *path, u_long mask) {
	
	PSIRFILE p = NULL;

  if (strptr(path) && (0UL != mask)) {

    p = (PSIRFILE)calloc(1U, sizeof(SIRFILE));

    if (NULL!= p) {

      p->path = (TCHAR *)calloc(sstrlen(path) + 1U, sizeof(TCHAR));

      if (NULL != p->path) {

        memcpy(p->path, path, sstrlen(path) * sizeof(TCHAR));
        p->bitmask = mask;

        return p;

      }

    }

  }

	return (PSIRFILE)(NULL);
}

void _SirFile_Destroy(PSIRFILE pf) {

  if (NULL != pf) {

    if (strptr(pf->path)) { free(pf->path); }

    free(pf);

  }

}

int _SirFileCache_AddFile(PSIRFILECACHE pfc, PSIRFILE pf) {

  int r = -1;

  if ((NULL != pfc) && (NULL != pf)) {

    if (0U < pfc->count) {

      pfc->files[pfc->count] = pf;
      pfc->count++;

    } else {

      pfc->files[0] = pf;
      pfc->count    = 1U;

    }

    r = 0;

  }

	return r;
}

int _SirFileCache_RemFile(PSIRFILECACHE pfc, TCHAR *path) {

	size_t n  = 0U;
	size_t i  = 0U;
  int r     = -1;

  if ((NULL != pfc) && strptr(path) && (0U < pfc->count)) {

	  for (n = 0; n < pfc->count; n++) {

      if ((NULL != pfc->files[n]) && (0 != *pfc->files[n]->path)) {

		    if (0L == sstrcmp(pfc->files[n]->path, path)) {

			    _SirFile_Destroy(pfc->files[n]);

			    for (i = n; i < pfc->count - 1U; i++) {
				    pfc->files[i] = pfc->files[i + 1];
			    }

			    pfc->count--;
			    r = 0;
          break;

		    }

      }

	  }

  }
	
	return r;
}

int _SirFileCache_Destroy(PSIRFILECACHE pfc) {
	
  int r = -1;

  if ((NULL != pfc) && (0U < pfc->count)) {
    
    size_t n  = 0U;

	  for (; n < pfc->count; n++ ) {

		  _SirFile_Destroy(pfc->files[n]);
		  pfc->files[n] = NULL;
		  pfc->count--;

	  }

    r = 0;
  }

	return r;
}

int _SirFileCache_Dispatch(PSIRFILECACHE pfc, TCHAR *output, u_long mask) {
	
  int r = 0;

  if ((NULL != pfc) && strptr(output) && (0UL != mask)) {

    size_t n = 0U;

	  for (; (n < pfc->count) && (0 == r); n++ ) {

      if ((NULL != pfc->files[n]) && strptr(pfc->files[n]->path)) {

		    if (pfc->files[n]->bitmask & mask) {

			    if (0 == _Sir_File(pfc->files[n]->path, output)) {

				    r = 0;

          } else {

            r = -1;

          }

		    }

      }

	  }

  }

	return r;
}

PSIRCALLBACKOBJ _SirCallbackObj_Create(SIRCALLBACK pfn, u_long mask, u_long data) {

	PSIRCALLBACKOBJ pco = NULL;

  if ((NULL != pfn) && (0UL != mask)) {

    pco = (PSIRCALLBACKOBJ)calloc(1U, sizeof(SIRCALLBACKOBJ));

    if (NULL != pco) {

	    pco->pfn  = pfn;
	    pco->mask = mask;
	    pco->data = data;

      return pco;

    }

  }

	return (PSIRCALLBACKOBJ)(NULL);
}

void _SirCallbackObj_Destroy(PSIRCALLBACKOBJ pco) {
	
  if (NULL != pco) { free(pco); }

}

int _SirCallbackCache_AddCallback(PSIRCALLBACKCACHE pcc, PSIRCALLBACKOBJ pco) {
	
  int r = -1;

  if ((NULL != pcc) && (NULL != pco)) {

	  if (0 < pcc->count) {

		  pcc->c[pcc->count] = pco;
		  pcc->count++;

	  } else {

		  pcc->c[0]  = pco;
		  pcc->count = 1U;

	  }

    r = 0;

  }

	return r;
}

int _SirCallbackCache_RemCallback(PSIRCALLBACKCACHE pcc, SIRCALLBACK pfn) {
	
  int r = -1;

  if ((NULL != pcc) && (NULL != pfn) && (0UL < pcc->count)) {

    size_t n = 0U;
	  size_t i = 0U;

	  for (; n < pcc->count; n++ ) {

      if (NULL != pcc->c[n]) {

		    if (pcc->c[n]->pfn == pfn) {

			    _SirCallbackObj_Destroy(pcc->c[n]);

			    for (i = n; i < pcc->count; i++) {

				    pcc->c[i] = pcc->c[i + 1];

			    }

			    pcc->count--;
			    r = 0;
          break;

		    }

      }

	  }

  }

	return r;
}

int _SirCallbackCache_Destroy(PSIRCALLBACKCACHE pcc) {
	
  int r = -1;

  if ((NULL != pcc) && (0UL < pcc->count)) {

    size_t n = 0U;

	  for (; n < pcc->count; n++) {

		  _SirCallbackObj_Destroy(pcc->c[n]);
		  pcc->c[n] = NULL;
		  pcc->count--;

	  }

    r = 0;

  }

	return r;
}

int _SirCallbackCache_Dispatch(PSIRCALLBACKCACHE pcc, TCHAR *output, u_long mask) {

  int r = -1;
	
  if ((NULL != pcc) && strptr(output) && (0UL != mask)) {
    
    size_t n = 0U;

	  for (; n < pcc->count; n++ ) {

		  if (pcc->c[n]->mask & mask) {

			  if ((0L != pcc->c[n]->pfn)) {

				  pcc->c[n]->pfn(output, pcc->c[n]->data);

			  }

		  }

	  }

    r = 0;

  }

	return r;
}

int _IsValidSirStruct(PSIRSTRUCT pss) {

  int r = 1;

  if (0UL == pss->f_debug) {

    if (0UL == pss->f_stderr) {

      if (0UL == pss->f_stdout) {

        if (0UL == (pss->opts & SIRO_FILES)) {

          if (0UL == (pss->opts & SIRO_CALLBACKS)) {

            r = 0;

          }

        }

      }

    }

  }

  return r;
}
