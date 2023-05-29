/**
 * @file sirfilesystem.c
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
#include "sirfilesystem.h"
#include "sirinternal.h"

#if defined(_WIN32)
# pragma comment(lib, "Shlwapi.lib")
#endif

/**
 * @addtogroup intern
 * @{
 */

bool _sir_fileexists(const char* restrict path) {

    if (!_sir_validstr(path))
        return false;

#pragma message("TODO: add handling of relative filenames (to the directory the binary resides in, not the cwd")       

#if !defined(_WIN32)
    struct stat st = {0};
    if (0 != stat(path, &st)) {
        if (ENOENT != errno)
#pragma message("TODO: use getappdir and fstatat() here")        
            _sir_handleerr(errno);
        return false;
    }
    return true;
#else    
    return = (TRUE == PathFileExistsA(path));
#endif
}

bool _sir_getcwd(char* restrict dir, size_t size) {
    if (!_sir_validptr(dir))
        return false;

    if (size < SIR_MAXPATH) {
        _sir_seterror(_SIR_E_INVALID);
        return false;
    }

    bool retval = true;

#if !defined(_WIN32)
# if defined(__linux__) && defined(_GNU_SOURCE)
    char* cur = get_current_dir_name();
    strncpy(dir, cur, strnlen(cur, SYSTEST_MAXPATH));
# else
    if (NULL == getcwd(dir, size)) {
        _sir_handleerr(errno);
        retval = false;
    }
# endif
#else // _WIN32
    if (NULL == _getcwd(dir, (int)size)) {
        _sir_handleerr(errno);
        retval = false;
    }
#endif
    return retval;
}

bool _sir_getappfilename(char* restrict buffer, size_t size) {

    if (!_sir_validptr(buffer))
        return false;

    if (size < SIR_MAXPATH) {
        _sir_seterror(_SIR_E_INVALID);
        return false;
    }

#pragma message("TODO: we're going to have to change these to return int, in cases like readlink needs more buffer space, we can't just return false.")

    bool retval = true;

#if !defined(_WIN32)
# if defined(__linux__)
#  if defined(__HAVE_UNISTD_READLINK__)
    ssize_t read = readlink("/proc/self/exe", buffer, size);
    if (-1 == read || read > (ssize_t)size) {
        bool too_small = read > (ssize_t)size;
        // TODO: if too_small == true: buffer is too small; change size to a pointer so we can store
        // the necesssary size there.        
        _sir_handleerr(errno);
        retval = false;
    }
#  else
#   error "unable to resolve readlink(); see man readlink and its feature test macro requirements."
#  endif
# elif defined(__FreeBSD__)
    int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1 };
    if (0 != sysctl(mib, 4, buffer, &size, NULL, 0)) {
        _sir_handleerr(errno);
        retval = false;
    }
# elif defined(__APPLE__)
    uint32_t size32 = (uint32_t)size;
    if (0 != _NSGetExecutablePath(buffer, &size32)) {
        // TODO: buffer is too small; need size32 bytes
        _sir_handleerr(errno);
        retval = false;
    }
# else
#  error "no implementation for your platform; please contact the author."
# endif
#else // _WIN32
    DWORD ret = GetModuleFileNameA(NULL, buffer, (DWORD)size);
    DWORD le  = GetLastError();
    if (0 == ret || ERROR_INSUFFICIENT_BUFFER == le) {
        bool too_small = ERROR_INSUFFICIENT_BUFFER == le;
        // TODO: if too_small == true: buffer is too small; ret contains the chars stored in buffer.
        _sir_handlewin32err(GetLastError());
        retval = false;
    }
#endif
    return retval;
}

bool _sir_getappdir(char* restrict buffer, size_t size) {
    char tmp[SIR_MAXPATH] = {0};
    if (_sir_getappfilename(tmp, SIR_MAXPATH)) {
        strncpy(buffer, _sir_getdirname(tmp), strnlen(tmp, size));
        return true;
    }
    return false;
}

char* _sir_getbasename(char* restrict path) {
    if (!_sir_validstr(path))
        return ".";

#if !defined(_WIN32)
    return basename(path);
#else
# pragma message("TODO: need to verify the behavior of these functions on windows as compared to *nix")
    // this isn't going to work; basename() and dirname() will strip
    // path components off the end, too. need to verify whether this
    // function only returns a file name part if it contains a full stop,
    // or perhaps it queries the path to see if it's a file or directory.
    // https://www.man7.org/linux/man-pages/man3/basename.3.html
    return PathFindFileNameA(path);
#endif
}

char* _sir_getdirname(char* restrict path) {
    if (!_sir_validstr(path))
        return ".";

#if !defined(_WIN32)
    return dirname(path);
#else
    // TODO: same problem as above.
    if (!PathRemoveFileSpecA((LPSTR)path))
        _sir_handlewin32err(GetLastError());
    return path;
#endif
}

/** @} */
