/*
 * sirfilesystem.h
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2023
 * Version:   2.2.0
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
#ifndef _SIR_FILESYSTEM_H_INCLUDED
#define _SIR_FILESYSTEM_H_INCLUDED

#include "sirplatform.h"

/** Defines how many characters to grow a buffer by which was deemed too small
 * by a system call (with no information regarding the necessary size). */
#define SIR_PATH_BUFFER_GROW_BY 32

/** Buffer size, in characters, for struct stat -> string. */
#define SIR_STAT_BUFFER_SIZE 128

/** Special flag to indicate to the caller that the file in question
 * does not exist (_sir_pathgetstat). */
#define SIR_STAT_NONEXISTENT ((off_t)0xffffff02)

/** Flags used to specify which directory to use as the base reference for
 * testing relative paths. */
#define SIR_PATH_REL_TO_CWD ((uint16_t)(0x0001))
#define SIR_PATH_REL_TO_APP ((uint16_t)(0x0002))

bool _sir_pathgetstat(const char* restrict path, struct stat* restrict st, uint16_t rel_to);
bool _sir_pathexists(const char* restrict path, bool* restrict exists, uint16_t rel_to);

char* _sir_getcwd(void);

char* _sir_getappfilename(void);
char* _sir_getappbasename(void);

char* _sir_getappdir(void);

char* _sir_getbasename(char* restrict path);

char* _sir_getdirname(char* restrict path);

bool _sir_ispathrelative(const char* restrict path, bool* restrict relative);

char* _sir_stattostring(const struct stat* restrict st);

#endif // !_SIR_FILESYSTEM_H_INCLUDED
