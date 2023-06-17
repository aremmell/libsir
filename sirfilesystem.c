/*
 * sirfilesystem.c
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
#include "sirfilesystem.h"
#include "sirinternal.h"

#if defined(__WIN__)
# pragma comment(lib, "Shlwapi.lib")
#endif

bool _sir_pathgetstat(const char* restrict path, struct stat* restrict st, sir_rel_to rel_to) {
    if (!_sir_validstr(path) || !_sir_validptr(st))
        return false;

    memset(st, 0, sizeof(struct stat));

    int stat_ret  = -1;
    bool relative = false;
    if (!_sir_ispathrelative(path, &relative))
        return false;
    
    if (relative) {
        char* base_path = NULL;
        switch(rel_to) {
            case SIR_PATH_REL_TO_APP: base_path = _sir_getappdir(); break;
            case SIR_PATH_REL_TO_CWD: base_path = _sir_getcwd(); break;
            default: _sir_seterror(_SIR_E_INVALID); return false;
        }

        if (!base_path) {
            _sir_handleerr(errno);
            return false;
        }

#if !defined(__WIN__)
# if defined(__MACOS__)
        int open_flags = O_SEARCH;
# elif defined(__linux__)
        int open_flags = O_PATH | O_DIRECTORY;
# elif defined(__BSD__)
        int open_flags = O_EXEC | O_DIRECTORY;
# endif

        int fd = open(base_path, open_flags);
        if (-1 == fd) {
            _sir_handleerr(errno);
            _sir_safefree(base_path);
            return false;
        }

        stat_ret = fstatat(fd, path, st, AT_SYMLINK_NOFOLLOW);
        _sir_safeclose(&fd);
        _sir_safefree(base_path);
    } else {
        stat_ret = stat(path, st);
    }
#else // __WIN__
        char abs_path[SYSTEST_MAXPATH] = {0};
        snprintf(abs_path, SYSTEST_MAXPATH, "%s\\%s", base_path, path);

        stat_ret = stat(abs_path, st);
        systest_safefree(base_path);
    } else {
        stat_ret = stat(path, st);
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

bool _sir_pathexists(const char* restrict path, bool* restrict exists, sir_rel_to rel_to) {
    if (!_sir_validstr(path) || !_sir_validptr(exists))
        return false;

    *exists = false;

    struct stat st = {0};
    bool stat_ret  = _sir_pathgetstat(path, &st, rel_to);
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
        ssize_t read = readlink("/proc/self/exe", buffer, size - 1);
        if (-1 != read && read < (ssize_t)size - 1) {
            resolved = true;
            break;
        } else {
            if (-1 == read) {
                _sir_handleerr(errno);
                resolved = false;
                break;
            } else if (read == (ssize_t)size - 1) {
                /* It is possible that truncation occurred. Grow the
                   buffer and try again. */
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
        if (0 == ret) {
            resolved = true;
            break;
        } else {
            if (ENOMEM == errno && 0 == sysctl(mib, 4, NULL, &size, NULL, 0)) {
                grow = true;
                continue;
            }
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
    }

    return buffer;
}

char* _sir_getappbasename(void) {
    char* filename = _sir_getappfilename();
    if (!_sir_validstr(filename))
        return NULL;
#pragma message("TODO: screw basename/dirname(); hand-roll them OR enable CRT debugging and see if they're leaking heap memory")
    char* retval = _sir_getbasename(filename);
    char* bname  = strdup(retval);

    _sir_safefree(filename);
    return bname;
}

char* _sir_getappdir(void) {
    char* filename = _sir_getappfilename();
    if (!_sir_validstr(filename))
        return NULL;

    char* retval = _sir_getdirname(filename);
    char* dirname = strdup(retval);

    _sir_safefree(filename);
    return dirname;
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

char* _sir_stattostring(const struct stat* restrict st) {
    if (!_sir_validptr(st))
        return false;

    char* buffer = (char*)calloc(SIR_STAT_BUFFER_SIZE, sizeof(char));
    if (!buffer) {
        _sir_handleerr(errno);
        return NULL;
    }

#if defined(__WIN__)
# define S_IFIFO _S_IFIFO
# define S_IRUSR 0x0100
# define S_IWUSR 0x0080
# define S_IXUSR 0x0040
# define S_IRGRP 0x0010
# define S_IWGRP 0x0400
# define S_IXGRP 0x0004
# define S_IROTH 0x0020
# define S_IWOTH 0x0800
# define S_IXOTH 0x0008
#endif

    char* type = "";
    switch (st->st_mode & S_IFMT) {
        case S_IFBLK:  type = "block device"; break;
        case S_IFCHR:  type = "character special"; break;        
        case S_IFDIR:  type = "directory"; break;        
        case S_IFIFO:  type = "pipe (fifo)"; break;
        case S_IFLNK:  type = "symlink"; break;
        case S_IFREG:  type = "regular"; break;
        case S_IFSOCK: type = "socket"; break;
        default:       type = SIR_UNKNOWN; break;
    }

    char mode[32] = { 0 };
    snprintf(mode, sizeof(mode), "%c%c%c%c%c%c%c%c%c%c (%03o)",
        (_sir_bittest(st->st_mode & 0xF000, S_IFDIR) ? 'd' : '-'),
        (_sir_bittest(st->st_mode & 0x0FFF, S_IRUSR) ? 'r' : '-'),
        (_sir_bittest(st->st_mode & 0x0FFF, S_IWUSR) ? 'w' : '-'),
        (_sir_bittest(st->st_mode & 0x0FFF, S_IXUSR) ? 'x' : '-'),
        (_sir_bittest(st->st_mode & 0x0FFF, S_IRGRP) ? 'r' : '-'),
        (_sir_bittest(st->st_mode & 0x0FFF, S_IWGRP) ? 'w' : '-'),
        (_sir_bittest(st->st_mode & 0x0FFF, S_IXGRP) ? 'x' : '-'),
        (_sir_bittest(st->st_mode & 0x0FFF, S_IROTH) ? 'r' : '-'),
        (_sir_bittest(st->st_mode & 0x0FFF, S_IWOTH) ? 'w' : '-'),
        (_sir_bittest(st->st_mode & 0x0FFF, S_IXOTH) ? 'x' : '-'),
        ((st->st_mode & 0x0FFF) & ((S_IRUSR | S_IWUSR | S_IXUSR) |
            (S_IRGRP | S_IWGRP | S_IXGRP) |
            (S_IROTH | S_IWOTH | S_IXOTH))));

    snprintf(buffer, SIR_STAT_BUFFER_SIZE, "{ type: %s, size: %ld, "
        "mode: %s }", type, (long)st->st_size, mode);

    return buffer;
}
