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

#if defined(__WIN__)
# pragma comment(lib, "Shlwapi.lib")
#endif

bool _sir_pathgetstat(const char* restrict path, struct stat* restrict st) {
    if (!_sir_validstr(path) || !_sir_validptr(st))
        return false;

    memset(st, 0, sizeof(struct stat));

    bool relative = false;
    if (!_sir_ispathrelative(path, &relative))
        return false;

    int stat_ret = -1;

#if !defined(__WIN__)
    if (relative) {
#if defined(__MACOS__)
        int open_flags = O_SEARCH;
#elif defined(__linux__)
        int open_flags = O_PATH | O_DIRECTORY;
#elif defined(__BSD__)
        int open_flags = O_EXEC | O_DIRECTORY;
#endif
        int fd = open(".", open_flags);
        if (-1 == fd) {
            _sir_handleerr(errno);
            return false;
        }
        stat_ret = fstatat(fd, path, st, AT_SYMLINK_NOFOLLOW);
        _sir_safeclose(&fd);
    } else {
        stat_ret = stat(path, st);
    }

#else //__WIN__
    struct _stat _st = {0};
    if (relative) {
        char abs_path[SIR_MAXPATH] = {0};
        if (NULL == _fullpath(abs_path, path, SIR_MAXPATH)) {
            _sir_handleerr(errno);
            return false;
        }
        stat_ret = _stat(abs_path, &_st);
    } else {
        stat_ret = _stat(path, &_st);
    }
#endif

    if (-1 == stat_ret) {
        if (ENOENT == errno) {
            st->st_size = SIR_STAT_NONEXISTENT;
            return true;
        } else {
            _sir_handleerr(errno);
            return false;
        }
    }

    return true;
}

bool _sir_pathexists(const char* restrict path, bool* restrict exists) {
    if (!_sir_validstr(path) || !_sir_validptr(exists))
        return false;

    *exists = false;

    struct stat st = {0};
    bool stat_ret  = _sir_pathgetstat(path, &st);
    if (!stat_ret)
        return false;

    *exists = (st.st_size != SIR_STAT_NONEXISTENT);
    return true;
}

char* _sir_getcwd(void) {
#if !defined(__WIN__)
# if defined(__linux__) && defined(_GNU_SOURCE)
    char* cur = get_current_dir_name();
    if (NULL == cur)
        _sir_handleerr(errno);
    return cur;
# else
    char* cur = getcwd(NULL, 0);
    if (NULL == cur)
        _sir_handleerr(errno);
    return cur;
# endif
#else // __WIN__
    char* cur = _getcwd(NULL, 0);
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
        if (grow) {
            _sir_selflog("reallocating %zu bytes for buffer and trying again...", size);
            _sir_safefree(buffer);

            buffer = (char*)calloc(size, sizeof(char));
            if (NULL == buffer) {
                _sir_handleerr(errno);
                resolved = false;
                break;
            }

            grow = false;
        }

#if !defined(__WIN__)
# if defined(__linux__)
#  if defined(__HAVE_UNISTD_READLINK__)
        ssize_t read = readlink("/proc/self/exe", buffer, size);
        _sir_selflog("readlink() returned: %ld (size = %zu)", read, size);
        if (-1 != read && read < (ssize_t)size) {
            resolved = true;
            break;
        } else {
            if (-1 == read) {
                _sir_handleerr(errno);
                resolved = false;
                break;
            } else if (read >= (ssize_t)size) {
                /* readlink, like Windows' impl, doesn't have a concept
                * of letting you know how much larger your buffer needs
                * to be; it just truncates the string and returns how
                * many chars it wrote there. */
                size += SIR_PATH_BUFFER_GROW_BY;
                grow = true;
                continue;
            }
        }
#  else
#   error "unable to resolve readlink(); see man readlink and its feature test macro requirements."
#  endif
# elif defined(__BSD__)
        int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1 };
        int ret = sysctl(mib, 4, buffer, &size, NULL, 0);
        _sir_selflog("sysctl() returned: %d (size = %zu)", ret, size);
        if (0 == ret) {
            resolved = true;
            break;
        } else {
            _sir_handleerr(errno);
            resolved = false;
            break;
        }
# elif defined(__MACOS__)
        int ret = _NSGetExecutablePath(buffer, (uint32_t*)&size);
        if (0 == ret) {
            resolved = true;
            break;
        } else if (-1 == ret) {
            grow = true;
            continue;
        } else {
            _sir_handleerr(errno);
            resolved = false;
            break;
        }
# else
#  error "no implementation for your platform; please contact the author."
# endif
#else // __WIN__
        DWORD ret = GetModuleFileNameA(NULL, buffer, (DWORD)size);
        _sir_selflog("GetModuleFileNameA() returned: %lu (size = %zu)", ret, size);
        if (0 != ret && ret < (DWORD)size) {
            resolved = true;
            break;
        } else {
            if (0 == ret) {
                _sir_handlewin32err(GetLastError());
                resolved = false;
                break;
            } else if (ret == (DWORD)size || ERROR_INSUFFICIENT_BUFFER == GetLastError()) {
                /* Windows has no concept of letting you know how much larger
                * your buffer needed to be; it just truncates the string and
                * returns size. So, we'll guess. */
                size += SIR_PATH_BUFFER_GROW_BY;
                grow = true;
                continue;
            }
        }
#endif

    } while (true);

    if (!resolved) {
        _sir_safefree(buffer);
        _sir_selflog("failed to resolve filename!");
    } else {
        _sir_selflog("successfully resolved: '%s'", buffer);
    }

    return buffer;
}

char* _sir_getappdir(void) {
    char* filename = _sir_getappfilename();
    if (!_sir_validstr(filename))
        return NULL;

    char* dirname = strdup(filename);
    if (!_sir_validstr(dirname)) {
        _sir_safefree(filename);
        return NULL;
    }

    _sir_safefree(filename);
    return _sir_getdirname(dirname);
}

char* _sir_getbasename(char* restrict path) {
    if (!_sir_validstr(path))
        return ".";

#if !defined(__WIN__)
    return basename(path);
#else // __WIN__
    return PathFindFileNameA(path);
#endif
}

char* _sir_getdirname(char* restrict path) {
    if (!_sir_validstr(path))
        return ".";

#if !defined(__WIN__)
    return dirname(path);
#else // __WIN__
    BOOL unused = PathRemoveFileSpecA((LPSTR)path);
    _SIR_UNUSED(unused);
    return path;
#endif
}

bool _sir_ispathrelative(const char* restrict path, bool* restrict relative) {
    if (!_sir_validstr(path) || !_sir_validptr(relative))
        return false;

#if !defined(__WIN__)
    if (path[0] == '/' || (path[0] == '~' && path[1] == '/'))
        *relative = false;
    else
        *relative = true;
    return true;
#else // __WIN__
    *relative = (TRUE == PathIsRelativeA(path));
    return true;
#endif    
}

/*char* _sir_stattostring(const struct stat* restrict st) {
    if (!_sir_validptr(st))
        return false;

    char* buffer = (char*)calloc(SIR_STAT_BUFFER_SIZE, sizeof(char));
    if (!buffer) {
        _sir_handleerr(errno);
        return NULL;
    }

    char* type = "";
    switch (st->st_mode & S_IFMT) {
        case S_IFIFO:  type = "named pipe (fifo)"; break;
        case S_IFCHR:  type = "character special"; break;
        case S_IFDIR:  type = "directory"; break;
        case S_IFBLK:  type = "block special"; break;
        case S_IFREG:  type = "regular"; break;
        case S_IFLNK:  type = "symlink"; break;
        case S_IFSOCK: type = "socket"; break;
        default: type = SIR_UNKNOWN; break;
    }

    char mode[17] = {0};
    snprintf(mode, sizeof(mode), "%c%c%c%c%c%c%c%c%c (%03o)",
        (_sir_bittest(st->st_mode, S_IRUSR) ? 'r' : '-'),
        (_sir_bittest(st->st_mode, S_IWUSR) ? 'w' : '-'),
        (_sir_bittest(st->st_mode, S_IXUSR) ? 'x' : '-'),
        (_sir_bittest(st->st_mode, S_IRGRP) ? 'r' : '-'),
        (_sir_bittest(st->st_mode, S_IWGRP) ? 'w' : '-'),
        (_sir_bittest(st->st_mode, S_IXGRP) ? 'x' : '-'),
        (_sir_bittest(st->st_mode, S_IROTH) ? 'r' : '-'),
        (_sir_bittest(st->st_mode, S_IWOTH) ? 'w' : '-'),
        (_sir_bittest(st->st_mode, S_IXOTH) ? 'x' : '-'),
        (st->st_mode & ((S_IRUSR | S_IWUSR | S_IXUSR) |
                        (S_IRGRP | S_IWGRP | S_IXGRP) |
                        (S_IROTH | S_IWOTH | S_IXOTH))));

    snprintf(buffer, SIR_STAT_BUFFER_SIZE, "{ type: %s, size: %ld, uid: %u, "
        "gid: %u, mode: %s }", type, (long)st->st_size, st->st_uid, st->st_gid, mode);

    return buffer;
}*/
