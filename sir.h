/*
 *  sir.h : ANSI C Implementation of Standard Incident Reporter (SIR)
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
#ifndef _SIR_H_INCLUDED
#define _SIR_H_INCLUDED

/* 
  System includes
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>

/*
  Special includes and defines
  for Win32 platforms
 */
#ifdef _WIN32

#include <Windows.h>
#include <tchar.h>

#define sctime      _tctime
#define sstrlen     _tcslen
#define svsnprintf  _vsntprintf
#define sfprintf    _ftprintf
#define sfopen      _tfopen
#define sstrcmp     _tcscmp

#else

/*
  Other platforms
 */
typedef char TCHAR, *PTCHAR;

#ifndef _T
#define _T(x)       x
#endif
#define sctime      ctime
#define sstrlen     strlen
#define svsnprintf  vsnprintf
#define sfprintf    fprintf
#define sfopen      fopen
#define sstrcmp     strcmp

#endif /* !_WIN32 */

#define strptr(n)(((n != NULL) && ((*n) != 0)) ? 1 : 0)

/*
  Hard-coded values
 */
#define SIR_MAXFILES      256
#define SIR_MAXCALLBACKS  256
#define SIR_MAXPRINT      2100

/* 
  Option constants
 */
#define SIRO_TIMESTAMP    0x00000002UL
#define SIRO_LF           0x00000004UL
#define SIRO_FILES        0x00000008UL
#define SIRO_CALLBACKS    0x00000020UL
#define SIRO_CRLF         0x00000040UL
#define SIRO_NODBGCRLF    0x00000080UL
#define SIRO_DEFAULT      (SIRO_TIMESTAMP | SIRO_CRLF | SIRO_NODBGCRLF)

/* 
  Output type constants
 */
#define SIRT_DEBUG   0x00000002UL
#define SIRT_ERROR   0x00000004UL
#define SIRT_WARNING 0x00000008UL
#define SIRT_FATAL   0x00000020UL
#define SIRT_LOG     0x00000040UL
#define SIRT_SCREEN  0x00000080UL

/*
  Make the sir function work with any case
 */
#define sir Sir
#define SIR Sir

/*
  Debug function pointer;
  TODO: Define Sir_Debug to your
  debug output function.
 */
#ifdef _WIN32
#define Sir_Debug OutputDebugString
#else
/* #define Sir_Debug <your_func> */
#endif /* !_WIN32 */

/* 
  Sir callback function prototype
 */
typedef void (*SIRCALLBACK)(TCHAR *out, u_long data);

/* 
  SIRSTRUCT Structure;
  Used to initialize the
  Sir system.
 */
typedef struct {

	u_long opts;      /* Option bitmask (SIRO_*) */
                    /* Type bitmasks (SIRT_*) */
  u_long f_stdout;  /* Output type bitmask for stdout */
  u_long f_stderr;  /* Output type bitmask for stderr */
  u_long f_debug;   /* Output type bitmask for debug */

} SIRSTRUCT, *PSIRSTRUCT;


/* 
  SIRFILE Structure;
  Retains information about
  a file destination for output.
 */
typedef struct {

	TCHAR *path;
	u_long bitmask;

} SIRFILE, *PSIRFILE;

/*
  SIRFILECACHE Structure;
  Used to maintain a list of
  file output destinations.
 */
typedef struct {

	PSIRFILE files[SIR_MAXFILES];
	size_t count;

} SIRFILECACHE, *PSIRFILECACHE;

/*
  SIRCALLBACKOBJ Structure;
  Retains information about
  a callback function output
  destination.
 */
typedef struct {

	SIRCALLBACK pfn;
	u_long mask;
	u_long data;

} SIRCALLBACKOBJ, *PSIRCALLBACKOBJ;

/*
  SIRCALLBACKCACHE Structure;
  Used to maintain a list of
  callback function output
  destinations.
 */
typedef struct {

	PSIRCALLBACKOBJ c[SIR_MAXCALLBACKS];
	size_t count;

} SIRCALLBACKCACHE, *PSIRCALLBACKCACHE;

/*
  SIRBUF Structure;
  Used as a string
  buffer.
 */
typedef struct {

	TCHAR *ptr;
	size_t size;

} SIRBUF, *PSIRBUF;


/*
  Exported Sir functions
 */


#ifdef __cplusplus
extern "C" {
#endif /* !__cplusplus */

/*
  Initializes Sir.
 */
int Sir_Init(PSIRSTRUCT pss);

/*
  Frees resources allocated
  by Sir.
 */
void Sir_Cleanup(void);

/*
  Resets the state of the
  Sir system.
 */
void Sir_Reset(void);

/*
  The routing funciton. Sends
  output to all applicable destinations.
 */
int Sir(u_long type, TCHAR *out, ...);

/*
  Adds a file output destination
  to Sir.
 */
int Sir_AddFile(TCHAR *path, u_long mask);

/*
  Removes a file output destination.
 */
int Sir_RemFile(TCHAR *path);

/*
  Adds a function pointer output destination
  to Sir.
 */
int Sir_AddCallback(SIRCALLBACK pfn, u_long mask, u_long data);

/*
  Removes a function pointer output destination.
 */
int Sir_RemCallback(SIRCALLBACK pfn);

/*
  Retrieves the option bitmask
  in use by the Sir system.
 */
u_long Sir_GetOpts(void);

#ifdef __cplusplus
}
#endif /* !__cplusplus */


/* 
  Internal functions
 */


/*
  Dispatchers
 */
int _Sir_File(TCHAR *path, TCHAR *output);
int _Sir_SendToDestinations(TCHAR *output, u_long mask);
void _Sir_Dispatch(TCHAR *output, u_long type);

/*
  Sirbuf
 */
PSIRBUF _Sirbuf_Create(void);
int _Sirbuf_Append(PSIRBUF pbuf, TCHAR *str);
int _Sirbuf_Destroy(PSIRBUF pbuf);

/*
  SirFile
 */
PSIRFILE _SirFile_Create(TCHAR *path, u_long mask);
void _SirFile_Destroy(PSIRFILE pf);
int _SirFileCache_AddFile(PSIRFILECACHE pfc, PSIRFILE pf);
int _SirFileCache_RemFile(PSIRFILECACHE pfc, TCHAR *path);
int _SirFileCache_Destroy(PSIRFILECACHE pfc);
int _SirFileCache_Dispatch(PSIRFILECACHE pfc, TCHAR *output, 
                           u_long mask);

/*
  SirCallbackObj
 */
PSIRCALLBACKOBJ _SirCallbackObj_Create(SIRCALLBACK pfn, 
                                       u_long mask, u_long data);
void _SirCallbackObj_Destroy(PSIRCALLBACKOBJ pco);
int _SirCallbackCache_AddCallback(PSIRCALLBACKCACHE pcc, 
                                  PSIRCALLBACKOBJ pco);
int _SirCallbackCache_RemCallback(PSIRCALLBACKCACHE pcc, 
                                  SIRCALLBACK pfn);
int _SirCallbackCache_Destroy(PSIRCALLBACKCACHE pcc);
int _SirCallbackCache_Dispatch(PSIRCALLBACKCACHE pcc, 
                               TCHAR *output, u_long mask);

int _IsValidSirStruct(PSIRSTRUCT pss);

#endif /* !_SIR_H_INCLUDED */
