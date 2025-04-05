/*
 * sirfilesystem.c
 *
 * Version: 2.2.6
 *
 * -----------------------------------------------------------------------------
 *
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2018-2024 Ryan M. Lederman <lederman@gmail.com>
 * Copyright (c) 2018-2024 Jeffrey H. Johnson <trnsz@pobox.com>
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
 *
 * -----------------------------------------------------------------------------
 */

//-V::522
#include "sir/filesystem.h"
#include "sir/internal.h"

#if defined(__WIN__)
# if defined(__EMBARCADEROC__) && defined(_WIN64)
#  pragma comment(lib, "shlwapi.a")
# else
#  pragma comment(lib, "shlwapi.lib")
# endif
#endif

bool _sir_pathgetstat(const char* restrict path, struct stat* restrict st, sir_rel_to rel_to) {
    if (!_sir_validstr(path) || !_sir_validptr(st))
        return false;

    (void)memset(st, 0, sizeof(struct stat));

    int stat_ret          = -1;
    bool relative         = false;
    const char* base_path = NULL;

    if (!_sir_getrelbasepath(path, &relative, &base_path, rel_to))
        return false;

    if (relative) {
#if !defined(__WIN__)
# if defined(__MACOS__) || defined(_AIX) || defined(__EMSCRIPTEN__)
#  if !defined(O_SEARCH)
        int open_flags = O_DIRECTORY;
#  else
        int open_flags = O_SEARCH;
#  endif
# elif defined(__linux__) || defined(__HURD__) || defined(__QNX__) || \
       defined(__managarm__)
#  if !defined(__SUNPRO_C) && !defined(__SUNPRO_CC) && defined(O_PATH)
        int open_flags = O_PATH | O_DIRECTORY;
#  else
        int open_flags = O_DIRECTORY;
#  endif
# elif defined(__FreeBSD__) || defined(__CYGWIN__) || defined(__serenity__)
        int open_flags = O_EXEC | O_DIRECTORY;
# elif defined(__SOLARIS__) || defined(__DragonFly__) || \
       defined(__NetBSD__) || defined(__HAIKU__) || defined(__OpenBSD__)
        int open_flags = O_DIRECTORY;
# else
#  error "unknown open_flags for your platform; please contact the developers"
# endif

# if !(defined(_AIX) || !defined(__PASE__))
        int fd = open(base_path, open_flags);
        if (-1 == fd) {
            _sir_safefree(&base_path);
            return _sir_handleerr(errno);
        }
        stat_ret = fstatat(fd, path, st, AT_SYMLINK_NOFOLLOW);
# else
        // HACKHACK: fstatat does not work properly for any fd other than AT_FDCWD.
        SIR_UNUSED(open_flags);
        int fd = AT_FDCWD;
        if (rel_to == SIR_PATH_REL_TO_CWD) {
            /* cppcheck-suppress invalidFunctionArg */
            stat_ret = fstatat(fd, path, st, AT_SYMLINK_NOFOLLOW);
        } else if (rel_to == SIR_PATH_REL_TO_APP) {
            char abs_path[SIR_MAXPATH] = {0};
            (void)snprintf(abs_path, SIR_MAXPATH, "%s/%s", base_path, path);
            stat_ret = stat(abs_path, st);
        }
# endif
        _sir_safeclose(&fd);
        _sir_safefree(&base_path);
    } else {
        stat_ret = stat(path, st);
    }
#else /* __WIN__ */
        char abs_path[SIR_MAXPATH] = {0};
        (void)snprintf(abs_path, SIR_MAXPATH, "%s\\%s", base_path, path);

# if defined(__EMBARCADEROC__) && (__clang_major__ < 15)
        /* Embarcadero <12 does not like paths that end in slashes, nor does it appreciate
         * paths like './' and '../'; this hack is needed for RAD Studio 11.3 and earlier. */
        char resolved_path[SIR_MAXPATH] = {0};
        if (!GetFullPathNameA(abs_path, SIR_MAXPATH, resolved_path, NULL)) {
            _sir_safefree(&base_path);
            return _sir_handlewin32err(GetLastError());
        }

        PathRemoveBackslashA(resolved_path);
        (void)_sir_strlcpy(abs_path, resolved_path, SIR_MAXPATH);
# endif

        stat_ret = stat(abs_path, st);
        _sir_safefree(&base_path);
    } else {
        stat_ret = stat(path, st);
    }
#endif
    if (-1 == stat_ret && (ENOENT == errno || ENOTDIR == errno))
        st->st_size = SIR_STAT_NONEXISTENT;

    return (-1 != stat_ret || ENOENT == errno || ENOTDIR == errno) ? true : _sir_handleerr(errno);
}

bool _sir_pathexists(const char* path, bool* exists, sir_rel_to rel_to) {
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

bool _sir_openfile(FILE* restrict* restrict f, const char* restrict path,
    const char* restrict mode, sir_rel_to rel_to) {
    if (!_sir_validptrptr(f) || !_sir_validstr(path) || !_sir_validstr(mode))
        return false;

    bool relative         = false;
    const char* base_path = NULL;

    if (!_sir_getrelbasepath(path, &relative, &base_path, rel_to))
        return false;

    if (relative) {
        char abs_path[SIR_MAXPATH] = {0};
        (void)snprintf(abs_path, SIR_MAXPATH, "%s/%s", base_path, path);

        int ret = _sir_fopen(f, abs_path, mode);
        _sir_safefree(&base_path);
        return 0 == ret;
    }

    return 0 == _sir_fopen(f, path, mode);
}

#if defined(_AIX)
static char cur_cwd[SIR_MAXPATH];
#endif
char* _sir_getcwd(void) {
#if !defined(__WIN__)
# if defined(__linux__) && (defined(__GLIBC__) && defined(_GNU_SOURCE))
    char* cur = get_current_dir_name();
    if (!_sir_validptrnofail(cur))
        (void)_sir_handleerr(errno);
    return cur;
# elif defined(_AIX)
    if (getcwd(cur_cwd, sizeof(cur_cwd)) == 0) {
        (void)_sir_handleerr(errno);
        return NULL;
    } else {
        return strndup(cur_cwd, SIR_MAXPATH);
    }
# else
    char* cur = getcwd(NULL, 0);
    if (NULL == cur)
        (void)_sir_handleerr(errno);
    return cur;
# endif
#else /* __WIN__ */
    DWORD size = GetCurrentDirectoryA(0UL, NULL);
    if (0UL == size) {
        _sir_handlewin32err(GetLastError());
        return NULL;
    }

    char* cur = calloc(size, sizeof(char));
    if (!cur) {
        (void)_sir_handleerr(errno);
        return NULL;
    }

    if (!GetCurrentDirectoryA(size, cur)) {
        _sir_handlewin32err(GetLastError());
        _sir_safefree(cur);
        return NULL;
    }

    return cur;
#endif
}

char* _sir_getappfilename(void) {
#if defined(__linux__) || defined(__NetBSD__) || defined(__SOLARIS__) || \
    defined(__DragonFly__) || defined(__CYGWIN__) || defined(__serenity__) || \
    defined(__HURD__) || defined(__EMSCRIPTEN__) || defined(__managarm__)
# define __READLINK_OS__
# if defined(__linux__) || defined(__CYGWIN__) || \
     defined(__serenity__) || defined(__HURD__) || defined(__EMSCRIPTEN__) || \
     defined(__managarm__)
#  define PROC_SELF "/proc/self/exe"
# elif defined(__NetBSD__)
#  define PROC_SELF "/proc/curproc/exe"
# elif defined(__DragonFly__)
#  define PROC_SELF "/proc/curproc/file"
# elif defined(__SOLARIS__)
#  define PROC_SELF "/proc/self/path/a.out"
# endif
    struct stat st;
    if (-1 == lstat(PROC_SELF, &st)) {
        (void)_sir_handleerr(errno);
        return NULL;
    }

    size_t size = (st.st_size > 0) ? st.st_size + 2 : SIR_MAXPATH;
#else
    size_t size = SIR_MAXPATH;
#endif

    char* buffer  = NULL;
    bool resolved = false;

    do {
        _sir_safefree(&buffer);
        buffer = (char*)calloc(size, sizeof(char));
        if (NULL == buffer) {
            resolved = _sir_handleerr(errno);
            break;
        }

#if !defined(__WIN__)
# if defined(__READLINK_OS__)
        ssize_t rdlink = _sir_readlink(PROC_SELF, buffer, size - 1);
        if (-1L != rdlink && rdlink < (ssize_t)size - 1) {
            resolved = true;
            break;
        } else if (-1L == rdlink) {
            resolved = _sir_handleerr(errno);
            break;
        } else if (rdlink == (ssize_t)size - 1L) {
            /* it is possible that truncation occurred. as a security
             * precaution, fail; someone may have tampered with the link. */
            _sir_selflog("warning: readlink reported truncation; not using result!");
            resolved = false;
            break;
        }
# elif defined(__QNX__)
        FILE *self = fopen("/proc/self/exefile", "r");
        if (!self) {
            resolved = _sir_handleerr(errno);
            break;
        }
        int ch;
        size_t length = 0;
        while ((ch = fgetc(self)) != EOF) {
            if (length > size) {
                size = length + 1;
                continue;
            }
            buffer[length++] = ch;
        }
        int ret = fclose(self);
        resolved = ret == 0 ? true : _sir_handleerr(errno);
        break;
# elif defined(_AIX)
        if (size <= SIR_MAXPATH) {
            size = size + SIR_MAXPATH + 1L;
            continue;
        }
        int ret = _sir_aixself(buffer, &size);
        resolved = ret == 0 ? true : _sir_handleerr(errno);
        break;
# elif defined(__OpenBSD__)
        size_t length = (size_t)_sir_openbsdself(NULL, 0);
        if (length < 1) {
            resolved = _sir_handleerr(errno);
            break;
        }
        if (length > size) {
            size = length + 1;
            continue;
        }
        (void)_sir_openbsdself(buffer, length);
        resolved = _sir_validptrnofail(buffer);
        break;
# elif defined(__BSD__)
        int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1 };
        int ret = sysctl(mib, 4, buffer, &size, NULL, 0);
        if (0 == ret) {
            resolved = true;
            break;
        } else {
            if (ENOMEM == errno && 0 == sysctl(mib, 4, NULL, &size, NULL, 0))
                continue; /* grow buffer. */

            resolved = _sir_handleerr(errno);
            break;
        }
# elif defined(__HAIKU__)
        status_t ret =
            find_path(B_APP_IMAGE_SYMBOL, B_FIND_PATH_IMAGE_PATH, NULL, buffer, SIR_MAXPATH);
        if (B_OK == ret) {
            resolved = true;
            break;
        } else if (B_BUFFER_OVERFLOW == ret) {
            /* grow buffer. */
            continue;
        } else {
            resolved = _sir_handleerr(errno);
            break;
        }
# elif defined(__MACOS__)
        int ret = _NSGetExecutablePath(buffer, (uint32_t*)&size);
        if (0 == ret) {
            resolved = true;
            break;
        } else if (-1 == ret) {
            /* grow buffer. */
            continue;
        } else {
            resolved = _sir_handleerr(errno);
            break;
        }
# else
#  error "no implementation for your platform; please contact the developers"
# endif
#else /* __WIN__ */
        DWORD ret = GetModuleFileNameA(NULL, buffer, (DWORD)size);
        if (0UL != ret && ret < (DWORD)size) {
            resolved = true;
            break;
        } else if (0UL == ret) {
            resolved = _sir_handlewin32err(GetLastError());
            break;
        } else if (ret == (DWORD)size || ERROR_INSUFFICIENT_BUFFER == GetLastError()) {
            /* Windows has no concept of letting you know how much larger
             * your buffer needed to be; it just truncates the string and
             * returns size. So, we'll guess. */
            size += SIR_PATH_BUFFER_GROW_BY;
            continue;
        }
#endif

    } while (true);

    if (!resolved)
        _sir_safefree(&buffer);

    return buffer;
}

char* _sir_getappbasename(void) {
    char* filename = _sir_getappfilename();
    if (!_sir_validstr(filename)) {
        _sir_safefree(&filename);
        return NULL;
    }

    const char* retval = _sir_getbasename(filename);
    char* bname = strndup(retval, strnlen(retval, SIR_MAXPATH));

    _sir_safefree(&filename);
    return bname;
}

char* _sir_getappdir(void) {
    char* filename = _sir_getappfilename();
    if (!_sir_validstr(filename)) {
        _sir_safefree(&filename);
        return NULL;
    }

    const char* retval = _sir_getdirname(filename);
    char* dname = strndup(retval, strnlen(retval, SIR_MAXPATH));

    _sir_safefree(&filename);
    return dname;
}

char* _sir_getbasename(char* restrict path) {
    if (!_sir_validstr(path))
        return ".";

#if !defined(__WIN__)
    return basename(path);
#else /* __WIN__ */
    return PathFindFileNameA(path);
#endif
}

char* _sir_getdirname(char* restrict path) {
    if (!_sir_validstr(path))
        return ".";

#if !defined(__WIN__)
    return dirname(path);
#else /* __WIN__ */
    (void)PathRemoveFileSpecA((LPSTR)path);
    return path;
#endif
}

bool _sir_ispathrelative(const char* restrict path, bool* restrict relative) {
    bool valid = _sir_validstr(path) && _sir_validptr(relative);

    if (valid) {
#if !defined(__WIN__)
        if (path[0] == '/' || (path[0] == '~' && path[1] == '/'))
            *relative = false;
        else
            *relative = true;
#else /* __WIN__ */
        *relative = (TRUE == PathIsRelativeA(path));
#endif
    }

    return valid;
}

bool _sir_getrelbasepath(const char* restrict path, bool* restrict relative,
    const char* restrict* restrict base_path, sir_rel_to rel_to) {
    if (!_sir_validstr(path) || !_sir_validptr(relative) ||
        !_sir_validptrptr(base_path) || !_sir_ispathrelative(path, relative))
        return false;

    if (*relative) {
        switch (rel_to) {
            case SIR_PATH_REL_TO_APP: return NULL != (*base_path = _sir_getappdir());
            case SIR_PATH_REL_TO_CWD: return NULL != (*base_path = _sir_getcwd());
            default: return _sir_seterror(_SIR_E_INVALID);
        }
    }

    return true;
}

#if defined(_AIX)
# if !defined(__PASE__)
#  define SIR_MAXSLPATH (SIR_MAXPATH + 16)
int _sir_aixself(char* buffer, size_t* size) {
    ssize_t res;
    char cwd[SIR_MAXPATH], cwdl[SIR_MAXPATH];
    char symlink[SIR_MAXSLPATH], temp_buffer[SIR_MAXPATH];
    char pp[64];
    struct psinfo ps;
    int fd;
    char** argv;
    char* tokptr;

    if ((buffer == NULL) || (size == NULL))
        return -1;

    (void)snprintf(pp, sizeof(pp), "/proc/%llu/psinfo", (unsigned long long)_sir_getpid());

    fd = open(pp, O_RDONLY);
    if (fd < 0)
        return -1;

    res = read(fd, &ps, sizeof(ps));
    close(fd);
    if (res < 0)
        return -1;

    if (ps.pr_argv == 0)
        return -1;

    argv = (char**)*((char***)(intptr_t)ps.pr_argv);

    if ((argv == NULL) || (argv[0] == NULL))
        return -1;

    if (argv[0][0] == '/') {
        (void)snprintf(symlink, SIR_MAXPATH, "%s", argv[0]);

        res = _sir_readlink(symlink, temp_buffer, SIR_MAXPATH);
        if (res < 0)
            (void)_sir_strncpy(buffer, SIR_MAXPATH, symlink, SIR_MAXPATH);
        else
            (void)snprintf(buffer, *size - 1, "%s/%s", (char*)dirname(symlink),
                temp_buffer);

        *size = strnlen(buffer, SIR_MAXPATH);
        return 0;
    } else if (argv[0][0] == '.') {
        char* relative = strchr(argv[0], '/');
        if (relative == NULL)
            return -1;

        (void)snprintf(cwd, SIR_MAXPATH, "/proc/%llu/cwd", (unsigned long long)_sir_getpid());
        res = _sir_readlink(cwd, cwdl, sizeof(cwdl) - 1);
        if (res < 0)
            return -1;

        (void)snprintf(symlink, SIR_MAXPATH, "%s%s", cwdl, relative + 1);

        res = _sir_readlink(symlink, temp_buffer, SIR_MAXPATH);
        if (res < 0)
            (void)_sir_strncpy(buffer, SIR_MAXPATH, symlink, SIR_MAXPATH);
        else
            (void)snprintf(buffer, *size - 1, "%s/%s", (char*)dirname(symlink), temp_buffer);

        *size = strnlen(buffer, SIR_MAXPATH);
        return 0;
    } else if (strchr(argv[0], '/') != NULL) {
        (void)snprintf(cwd, SIR_MAXPATH, "/proc/%llu/cwd", (unsigned long long)_sir_getpid());

        res = _sir_readlink(cwd, cwdl, sizeof(cwdl) - 1);
        if (res < 0)
            return -1;

        (void)snprintf(symlink, SIR_MAXPATH, "%s%s", cwdl, argv[0]);

        res = _sir_readlink(symlink, temp_buffer, SIR_MAXPATH);
        if (res < 0)
            (void)_sir_strncpy(buffer, SIR_MAXPATH, symlink, SIR_MAXPATH);
        else
            (void)snprintf(buffer, *size - 1, "%s/%s", (char*)dirname(symlink), temp_buffer);

        *size = strnlen(buffer, SIR_MAXPATH);
        return 0;
    } else {
        char clonedpath[16384];
        char* token = NULL;
        struct stat statstruct;

        char* path = getenv("PATH");
        if (sizeof(clonedpath) <= strnlen(path, SIR_MAXPATH))
            return -1;

        (void)_sir_strncpy(clonedpath, SIR_MAXPATH, path, SIR_MAXPATH);

        token = strtok_r(clonedpath, ":", &tokptr);

        (void)snprintf(cwd, SIR_MAXPATH, "/proc/%llu/cwd", (unsigned long long)_sir_getpid());

        res = _sir_readlink(cwd, cwdl, sizeof(cwdl) - 1);
        if (res < 0)
            return -1;

        while (token != NULL) {
            if (token[0] == '.') {
                char* relative = strchr(token, '/');
                if (relative != NULL) {
                    (void)snprintf(symlink, SIR_MAXSLPATH, "%s%s/%s", cwdl, relative + 1,
                        ps.pr_fname);
                } else {
                    (void)snprintf(symlink, SIR_MAXSLPATH, "%s%s", cwdl, ps.pr_fname);
                }

                if (stat(symlink, &statstruct) != -1) {
                    res = _sir_readlink(symlink, temp_buffer, SIR_MAXPATH);
                    if (res < 0)
                        (void)_sir_strncpy(buffer, SIR_MAXPATH, symlink, SIR_MAXPATH);
                    else
                        (void)snprintf(buffer, *size - 1, "%s/%s", (char*)dirname(symlink), temp_buffer);

                    *size = strnlen(buffer, SIR_MAXPATH);
                    return 0;
                }
            } else {
                (void)snprintf(symlink, SIR_MAXPATH, "%s/%s", token, ps.pr_fname);
                if (stat(symlink, &statstruct) != -1) {
                    res = _sir_readlink(symlink, temp_buffer, SIR_MAXPATH);
                    if (res < 0)
                        (void)_sir_strncpy(buffer, SIR_MAXPATH, symlink, SIR_MAXPATH);
                    else
                        (void)snprintf(buffer, *size - 1, "%s/%s", (char*)dirname(symlink), temp_buffer);

                    *size = strnlen(buffer, SIR_MAXPATH);
                    return 0;
                }
            }

            token = strtok_r(NULL, ":", &tokptr);
        }
        return -1;
    }
}
# elif defined(__PASE__)
int _sir_aixself(char* buffer, size_t* size) {
    struct procsinfo pinfo[16] = {0};
    int numproc = 0;
    int index = 0;
    pid_t pid = _sir_getpid();
    while ((numproc = getprocs(pinfo, sizeof(struct procsinfo), NULL, 0, &index, 16)) > 0) {
        for (int n = 0; n < numproc; n++) {
            if (pinfo[n].pi_state == SZOMB)
                continue;
            if (pinfo[n].pi_pid == pid) {
                char tmp[SIR_MAXPATH + 2] = {0};
                if (-1 == getargs(&pinfo[n], sizeof(struct procsinfo), tmp, sizeof(tmp))) {
                    (void)_sir_handleerr(errno);
                    break;
                }
                if (strchr(tmp, '/')) {
                    char tmp2[SIR_MAXPATH] = {0};
                    const char* rp = realpath(tmp, tmp2);
                    if (!rp) {
                        (void)_sir_handleerr(errno);
                        break;
                    }
                    (void)_sir_strncpy(buffer, *size, tmp2, strnlen(tmp2, *size - 1));
                } else {
                    (void)_sir_resolvepath(tmp, buffer, *size);
                }
                if (_sir_validstrnofail(buffer))
                    return 0;
                break;
            }
        }
    }
    return -1;
}
# endif
#endif

bool _sir_deletefile(const char* restrict path) {
    if (!_sir_validstr(path))
        return false;

#if !defined(__WIN__)
    return (0 == unlink(path)) ? true : _sir_handleerr(errno);
#else /* __WIN__ */
    return (FALSE != DeleteFileA(path)) ? true : _sir_handlewin32err(GetLastError());
#endif
}

#if defined(__OpenBSD__)
int _sir_openbsdself(char* buffer, int size) {
    char buffer1[4096];
    char buffer2[SIR_MAXPATH];
    char** argv    = (char**)buffer1;
    char* resolved = NULL;
    int length     = -1;

    while (true) {
        size_t mib_size = 0;
        int mib[4] = { CTL_KERN, KERN_PROC_ARGS, getpid(), KERN_PROC_ARGV };

        if (sysctl(mib, 4, NULL, &mib_size, NULL, 0) != 0)
            break;

        if (mib_size > sizeof(buffer1)) {
            argv = (char**)malloc(mib_size);
            if (!argv)
                break;
        }

        if (sysctl(mib, 4, argv, &mib_size, NULL, 0) != 0)
            break;

        if (strchr(argv[0], '/')) {
            resolved = realpath(argv[0], buffer2);
            if (!resolved)
                break;
        } else {
            if (-1 != _sir_resolvepath(argv[0], buffer2, sizeof(buffer2)))
                resolved = buffer2;
            if (!resolved)
                break;
        }

        length = (int)strnlen(resolved, SIR_MAXPATH);
        if (length <= size)
            (void)memcpy(buffer, resolved, (size_t)length);
        break;
    }
    if (argv != (char**)buffer1)
        _sir_safefree(&argv);

    return length;
}
#endif

#if defined(__OpenBSD__) || (defined(_AIX) && defined(__PASE__))
int _sir_resolvepath(const char* restrict path, char* restrict buffer, size_t size) {
    if (!_sir_validstr(path) || !_sir_validptr(buffer))
        return -1;

    const char* envvar = getenv("PATH");
    if (!_sir_validstr(envvar))
        return -1;

    size_t pathlen         = strnlen(path, SIR_MAXPATH);
    const char* cursor     = envvar;
    char tmp[SIR_MAXPATH]  = {0};
    char tmp2[SIR_MAXPATH] = {0};
    int length             = -1;

    while (true) {
        const char* separator = strchr(cursor, ':');
        const char* end = separator ? separator : cursor + strnlen(cursor, SIR_MAXPATH);
        if (end - cursor > 0) {
            if (*(end - 1) == '/')
                --end;
            if (((end - cursor) + 1UL + pathlen + 1UL) <= sizeof(tmp)) {
                (void)memcpy(tmp, cursor, end - cursor);
                tmp[end - cursor] = '/';
                (void)memcpy(tmp + (end - cursor) + 1, path, pathlen + 1);
                if (realpath(tmp, tmp2))
                    break;
            }
        }
        if (!separator)
            break;
        cursor = ++separator;
    }

    if (_sir_validstrnofail(tmp2)) {
        length = (int)strnlen(tmp2, SIR_MAXPATH);
        (void)_sir_strncpy(buffer, size, tmp2, (size_t)length);
    }

    return length;
}
#endif
