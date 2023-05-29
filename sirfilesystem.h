/**
 * @file sirfilesystem.h
 * @brief ◊
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

/**
 * @brief Determines if a file or directory exists in the filesystem.
 * 
 * @param path null-terminated string containing the path to test.
 * 
 * @return true TODO
 * @return false TODO
 */
bool _sir_fileexists(const char* restrict path);

/**
 * @brief Obtains the current working directory for the calling process.
 * 
 * @note The working directory is not necessarily the same as the directory
 *       that the binary resides in. Use @ref _sir_getappdir to obtain that path.
 * 
 * @param buffer Pointer to a string of characters not less than @ref SIR_MAXPATH in length.
 * @param size TODO
 * @return true TODO
 * @return false TODO
 */
bool _sir_getcwd(char* restrict buffer, size_t size);
#pragma messsage("TODO: Doxygen the rest of these")
bool _sir_getappfilename(char* restrict buffer, size_t size);
bool _sir_getappdir(char* restrict buffer, size_t size);
char* _sir_getbasename(char* restrict path);
char* _sir_getdirname(char* restrict path); 

/** @} */

#endif // !_SIR_FILESYSTEM_H_INCLUDED
