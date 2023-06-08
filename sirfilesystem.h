/**
 * @file sirfilesystem.h
 * @brief Internal filesystem-related functionality.
 *
 * This file and accompanying source code originated from <https://github.com/aremmell/libsir>.
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
#ifndef _SIR_FILESYSTEM_H_INCLUDED
#define _SIR_FILESYSTEM_H_INCLUDED

#include "sirplatform.h"

/**
 * @addtogroup intern
 * @{
 */

/** Defines how many characters to grow a buffer by which was deemed too small
 * by a system call (with no information regarding the necessary size). */
#define SIR_PATH_BUFFER_GROW_BY 32

/**
 * Determines if a file or directory exists in the filesystem.
 * Returns false if an error occurs or an argument is invalid. Places the result
 * in \p exists.
*/
bool _sir_pathexists(const char* restrict path, bool* restrict exists);

/**
 * Returns the current working directory for the calling process. If an error
 * occurs, returns NULL. Pointers returned must be deallocated with free().
 */
char* _sir_getcwd(void);

/**
 * Returns the absolute path of the binary executable file for the calling
 * process. If an error occurs, returns NULL. Pointers returned must be
 * deallocated with free().
 */
char* _sir_getappfilename(void);

/**
 * Returns the absolute path of the directory containing the binary file
 * of the calling process (not necessarily the current working directory).
 */
char* _sir_getappdir(void);

/**
 * Returns only last component of a path.
 * May return ".", "/", or \p path if no determination can be made.
 */
char* _sir_getbasename(char* restrict path);

/**
 * Returns all but the last component of a path.
 * May return "." "/", or \p path if no determination can be made.
*/
char* _sir_getdirname(char* restrict path);

/**
 * Determines if a given path is relative (or absolute).
 * Returns false if an error occurs or an argument is invalid. Places the result
 * in \p relative.
 */
bool _sir_ispathrelative(const char* restrict path, bool* restrict relative);

/** @} */

#endif // !_SIR_FILESYSTEM_H_INCLUDED
