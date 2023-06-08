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

bool _sir_fileexists(const char* restrict path, bool* restrict exists) {
    if (!_sir_validstr(path) || !_sir_validptr(exists))
        return false;

#if !defined(_WIN32)
    struct stat st = {0};
    if (0 != stat(path, &st)) {
        if (ENOENT != errno) {
            *exists = false;
            _sir_handleerr(errno);
            return false;
        }
    }
    *exists = true;
#else    
    *exists = (TRUE == PathFileExistsA(path));
#endif

    return true;
}

char* _sir_getcwd(void) {
#if !defined(_WIN32)
# if defined(__linux__) && defined(_GNU_SOURCE)
    char* cur = get_current_dir_name();
    if (NULL == cur)
        _sir_handleerr(errno);
    return cur;
# else
    char* cur = getcwd(NULL, SIR_MAXPATH);
    if (NULL == cur)
        _sir_handleerr(errno);
    return cur;
# endif
#else // _WIN32
    char* cur = _getcwd(NULL, SIR_MAXPATH);
    if (NULL == cur)
        _sir_handleerr(errno);
    return cur;
#endif
}

char* _sir_getappfilename(void) {
    char* buffer = (char*)calloc(SIR_MAXPATH, sizeof(char));
    if (NULL == buffer) {
        _sir_handleerr(errno);
        return NULL;
    }

    bool resolved = false;
    size_t size   = SIR_MAXPATH;
    bool grow     = false;

    do {
#if !defined(_WIN32)
# if defined(__linux__)
#  if defined(__HAVE_UNISTD_READLINK__)
        ssize_t read = readlink("/proc/self/exe", buffer, size);
_sir_selflog("readlink() returned: %ld (size = %zu)", read, size);
        if (-1 != read && read < (ssize_t)size) {
            resolved = true;
            break;
        } else {
            /* readlink, like Windows' impl, doesn't have a concept
             * of letting you know how much larger your buffer needs
             * to be; it just truncates the string and returns how
             * many chars it wrote there. */
            _sir_handleerr(errno);
            resolved = false;
            break;
        }
#  else
#   error "unable to resolve readlink(); see man readlink and its feature test macro requirements."
#  endif
# elif defined(__FreeBSD__)
        int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1 };
        int ret = sysctl(mib, 4, buffer, &size, NULL, 0);
        if (0 == ret) {
            resolved = true;
            break;
        } else {
            _sir_handleerr(errno);
            resolved = false;
            break;
        }
# elif defined(__APPLE__)
        int ret = _NSGetExecutablePath(buffer, (uint32_t*)&size);
        if (0 == ret) {
            resolved = true;
            break;
        } else if (-1 == ret) {
            grow = true;
            continue;
        } else  {
            _sir_handleerr(errno);
            resolved = false;
            break;
        }
# else
#  error "no implementation for your platform; please contact the author."
# endif
#else // _WIN32
        DWORD ret = GetModuleFileNameA(NULL, buffer, (DWORD)size);
_sir_selflog("GetModuleFileNameA() returned: %lu (size = %zu)", ret, size);        
        if (0 != ret && ret < (DWORD)size) {
            resolved = true;
            break;
        } else if (0 == ret || ERROR_INSUFFICIENT_BUFFER == GetLastError()) {
            /* Windows has no concept of letting you know how much larger
             * your buffer needed to be; it just truncates the string and
             * returns size. */
            _sir_handlewin32err(ERROR_INSUFFICIENT_BUFFER);
            resolved = false;
            break;
        }
#endif
        if (grow) {
_sir_selflog("reallocating %zu bytes for buffer and trying again...", size);            
            _sir_safefree(buffer);
            buffer = (char*)realloc(NULL, size);
            if (NULL == buffer) {
                _sir_handleerr(errno);
                resolved = false;
                break;
            }

            memset(buffer, 0, size);
            grow = false;
        }

    } while(true);

    if (!resolved)
        _sir_safefree(buffer);

_sir_selflog("successfully resolved: '%s'", buffer);
    return buffer;
}

char* _sir_getappdir(void) {
    char* filename = _sir_getappfilename();
    if (NULL == filename)
        return NULL;

    return _sir_getdirname(filename);
}

char* _sir_getbasename(char* restrict path) {
    if (!_sir_validstr(path))
        return ".";

#if !defined(_WIN32)
    return basename(path);
#else
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

#pragma message("TODO: come back and revisit dirname_r")
#if !defined(_WIN32)
/*# if defined(__APPLE__)
    //char dir[SIR_MAXPATH] = {0};
    char* retval = dirname_r(path, NULL);
    return retval;
# else*/
    return dirname(path);
//# endif
#else
    // TODO: same problem as above.
    if (!PathRemoveFileSpecA((LPSTR)path))
        _sir_handlewin32err(GetLastError());
    return path;
#endif
}

bool _sir_ispathrelative(const char* restrict path, bool* restrict relative) {
    if (!_sir_validstr(path) || _sir_validptr(relative))
        return false;

#if !defined(_WIN32)
    *relative = path[0] != '/';
#else
    *relative = TRUE == PathIsRelativeA(path);
#endif

    return true;
}

/** @} */
