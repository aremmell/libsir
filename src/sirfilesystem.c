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
#include "sir/filesystem.h"
#include "sir/internal.h"

#if defined(__WIN__)
# pragma comment(lib, "Shlwapi.lib")
#endif

bool _sir_pathgetstat(const char* restrict path, struct stat* restrict st, sir_rel_to rel_to) {
    if (!_sir_validstr(path) || !_sir_validptr(st))
        return false;

    memset(st, 0, sizeof(struct stat));

    int stat_ret          = -1;
    bool relative         = false;
    const char* base_path = NULL;

    if (!_sir_getrelbasepath(path, &relative, &base_path, rel_to))
        return false;

    if (relative) {
#if !defined(__WIN__)
# if defined(__MACOS__) || defined(_AIX)
        int open_flags = O_SEARCH;
# elif defined(__linux__)
#  if !defined(__SUNPRO_C) && !defined(__SUNPRO_CC)
        int open_flags = O_PATH | O_DIRECTORY;
#  else
        int open_flags = O_DIRECTORY;
#  endif
# elif defined(__FreeBSD__) || defined(__CYGWIN__) || defined(__serenity__)
        int open_flags = O_EXEC | O_DIRECTORY;
# elif defined(__SOLARIS__) || defined(__DragonFly__) || \
       defined(__NetBSD__) || defined(__HAIKU__) || defined(__OpenBSD__)
        int open_flags = O_DIRECTORY;
# elif defined(__VXWORKS__)
# pragma message("VxWorks: Implement open_flags for your filesystem.")
        int open_flags = O_RDONLY;
# else
#  error "unknown open_flags for your platform; please contact the author."
# endif

        int fd = open(base_path, open_flags);
#if !defined(__VXWORKS__)
        if (-1 == fd) {
            _sir_handleerr(errno);
            _sir_safefree(&base_path);
            return false;
        }
#endif

#if defined(__VXWORKS__)
# define fstatat(fd, name, st, sym) stat((name), (st))
#endif
        stat_ret = fstatat(fd, path, st, AT_SYMLINK_NOFOLLOW);
        _sir_safeclose(&fd);
        _sir_safefree(&base_path);
    } else {
        stat_ret = stat(path, st);
    }
#else /* __WIN__ */
        char abs_path[SIR_MAXPATH] = {0};
        snprintf(abs_path, SIR_MAXPATH, "%s\\%s", base_path, path);

        stat_ret = stat(abs_path, st);
        _sir_safefree(&base_path);
    } else {
        stat_ret = stat(path, st);
    }
#endif
#if !defined(__VXWORKS__)
    if (-1 == stat_ret) {
        if (ENOENT == errno) {
            st->st_size = SIR_STAT_NONEXISTENT;
            return true;
        } else {
            _sir_handleerr(errno);
            return false;
        }
    }
#else
# pragma message("VxWorks: Implement for your filesystem.")
#endif

    return true;
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
        snprintf(abs_path, SIR_MAXPATH, "%s/%s", base_path, path);

        int ret = _sir_fopen(f, abs_path, mode);
        _sir_safefree(&base_path);
        return 0 == ret;
    }

    return 0 == _sir_fopen(f, path, mode);
}

#if defined(_AIX) || defined(__VXWORKS__)
static char cur_cwd[SIR_MAXPATH];
#endif
char* _sir_getcwd(void) {
#if !defined(__WIN__)
# if defined(__linux__) && (defined(__GLIBC__) && defined(_GNU_SOURCE))
    char* cur = get_current_dir_name();
    if (NULL == cur)
        _sir_handleerr(errno);
    return cur;
# elif defined(_AIX) || defined(__VXWORKS__)
    if (getcwd(cur_cwd, sizeof(cur_cwd)) == 0) {
        _sir_handleerr(errno);
        return NULL;
    } else {
        return strndup(cur_cwd, SIR_MAXPATH);
    }
# else
    char* cur = getcwd(NULL, 0);
    if (NULL == cur)
        _sir_handleerr(errno);
    return cur;
# endif
#else /* __WIN__ */
    char* cur = _getcwd(NULL, 0);
    if (NULL == cur)
        _sir_handleerr(errno);
    return cur;
#endif
}

char* _sir_getappfilename(void) {
#if defined(__linux__) || defined(__NetBSD__) || defined(__SOLARIS__) || \
    defined(__DragonFly__) || defined(__CYGWIN__) || defined(__serenity__)
# define __READLINK_OS__
# if defined(__linux__) || defined(__CYGWIN__) || defined(__serenity__)
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
        _sir_handleerr(errno);
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
            _sir_handleerr(errno);
            resolved = false;
            break;
        }

#if !defined(__WIN__)
# if defined(__READLINK_OS__)
        /* Flawfinder: ignore */
        ssize_t read = readlink(PROC_SELF, buffer, size - 1);
        if (-1 != read && read < (ssize_t)size - 1) {
            resolved = true;
            break;
        } else if (-1 == read) {
            _sir_handleerr(errno);
            resolved = false;
            break;
        } else if (read == (ssize_t)size - 1) {
            /*
             * It is possible that truncation occurred. As a security
             * precaution, fail; someone may have tampered with the link.
             */
            _sir_selflog("warning: readlink reported truncation; not using result!");
            resolved = false;
            break;
        }
# elif defined(_AIX)
        if (size <= SIR_MAXPATH) {
            size = size + SIR_MAXPATH + 1;
            continue;
        }
        int ret = _sir_aixself(buffer, &size);
        if (ret == 0) {
            resolved = true;
            break;
        } else {
            _sir_handleerr(errno);
            resolved = false;
            break;
        }
# elif defined(__OpenBSD__)
        size_t length;
        int dirname_length;
        length = _sir_openbsdself(NULL, 0, &dirname_length);
        if (length < 1) {
            _sir_handleerr(errno);
            resolved = false;
            break;
        }
        if (length > size) {
            size = length + 1;
            continue;
        }
        (void)_sir_openbsdself(buffer, length, &dirname_length);
        if (!buffer) {
            resolved = false;
            break;
        }
        resolved = true;
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

            _sir_handleerr(errno);
            resolved = false;
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
            /* grow buffer. */
            continue;
        } else {
            _sir_handleerr(errno);
            resolved = false;
            break;
        }
# elif defined(__VXWORKS__)
# pragma message("VxWorks: Implement _sir_getappfilename for your filesystem.")
        resolved = false;
        break;
# else
#  error "no implementation for your platform; please contact the author."
# endif
#else /* __WIN__ */
        DWORD ret = GetModuleFileNameA(NULL, buffer, (DWORD)size);
        if (0 != ret && ret < (DWORD)size) {
            resolved = true;
            break;
        } else if (0 == ret) {
            _sir_handlewin32err(GetLastError());
            resolved = false;
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

    if (!resolved) {
        _sir_safefree(&buffer);
        _sir_selflog("error: failed to resolve filename!");
        return NULL;
    }

    return buffer;
}

char* _sir_getappbasename(void) {
    char* filename = _sir_getappfilename();
    if (_sir_validptr(filename) && !_sir_validstr(filename)) {
        _sir_safefree(&filename);
        return NULL;
    }

    char* retval = _sir_getbasename(filename);
    char* bname  = strdup(retval);

    _sir_safefree(&filename);
    return bname;
}

char* _sir_getappdir(void) {
    char* filename = _sir_getappfilename();
    if (_sir_validptr(filename) && !_sir_validstr(filename)) {
        _sir_safefree(&filename);
        return NULL;
    }

    char* retval  = _sir_getdirname(filename);
    char* dirname = strdup(retval);

    _sir_safefree(&filename);
    return dirname;
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
#else /* __WIN__ */
    *relative = (TRUE == PathIsRelativeA(path));
    return true;
#endif
}

bool _sir_getrelbasepath(const char* restrict path, bool* restrict relative,
    const char* restrict* restrict base_path, sir_rel_to rel_to) {
    if (!_sir_validstr(path) || !_sir_validptr(relative) || !_sir_validptrptr(base_path))
        return false;

    if (!_sir_ispathrelative(path, relative))
        return false;

    if (*relative) {
        switch (rel_to) {
            case SIR_PATH_REL_TO_APP: *base_path = _sir_getappdir(); break;
            case SIR_PATH_REL_TO_CWD: *base_path = _sir_getcwd(); break;
            default: _sir_seterror(_SIR_E_INVALID); return false;
        }

        if (!*base_path)
            return false;
    }

    return true;
}

#if defined(_AIX)
# define SIR_MAXSLPATH (SIR_MAXPATH + 16)
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

    snprintf(pp, sizeof(pp), "/proc/%llu/psinfo", (unsigned long long)_sir_getpid());

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
        snprintf(symlink, SIR_MAXPATH, "%s", argv[0]);

        /* Flawfinder: ignore */
        res = readlink(symlink, temp_buffer, SIR_MAXPATH);
        if (res < 0)
            _sir_strncpy(buffer, SIR_MAXPATH, symlink, SIR_MAXPATH);
        else
            snprintf(buffer, *size - 1, "%s/%s", (char*)dirname(symlink), temp_buffer);

        *size = strlen(buffer);
        return 0;
    } else if (argv[0][0] == '.') {
        char* relative = strchr(argv[0], '/');
        if (relative == NULL)
            return -1;

        snprintf(cwd, SIR_MAXPATH, "/proc/%llu/cwd", (unsigned long long)_sir_getpid());

        /* Flawfinder: ignore */
        res = readlink(cwd, cwdl, sizeof(cwdl) - 1);
        if (res < 0)
            return -1;

        snprintf(symlink, SIR_MAXPATH, "%s%s", cwdl, relative + 1);

        /* Flawfinder: ignore */
        res = readlink(symlink, temp_buffer, SIR_MAXPATH);
        if (res < 0)
            _sir_strncpy(buffer, SIR_MAXPATH, symlink, SIR_MAXPATH);
        else
            snprintf(buffer, *size - 1, "%s/%s", (char*)dirname(symlink), temp_buffer);

        *size = strlen(buffer);
        return 0;
    } else if (strchr(argv[0], '/') != NULL) {
        snprintf(cwd, SIR_MAXPATH, "/proc/%llu/cwd", (unsigned long long)_sir_getpid());

        /* Flawfinder: ignore */
        res = readlink(cwd, cwdl, sizeof(cwdl) - 1);
        if (res < 0)
            return -1;

        snprintf(symlink, SIR_MAXPATH, "%s%s", cwdl, argv[0]);

        /* Flawfinder: ignore */
        res = readlink(symlink, temp_buffer, SIR_MAXPATH);
        if (res < 0)
            _sir_strncpy(buffer, SIR_MAXPATH, symlink, SIR_MAXPATH);
        else
            snprintf(buffer, *size - 1, "%s/%s", (char*)dirname(symlink), temp_buffer);

        *size = strlen(buffer);
        return 0;
    } else {
        char clonedpath[16384];
        char* token = NULL;
        struct stat statstruct;

        char* path = getenv("PATH");
        if (sizeof(clonedpath) <= strlen(path))
            return -1;

        _sir_strncpy(clonedpath, SIR_MAXPATH, path, SIR_MAXPATH);

        token = strtok_r(clonedpath, ":", &tokptr);

        snprintf(cwd, SIR_MAXPATH, "/proc/%llu/cwd", (unsigned long long)_sir_getpid());
        /* Flawfinder: ignore */
        res = readlink(cwd, cwdl, sizeof(cwdl) - 1);
        if (res < 0)
            return -1;

        while (token != NULL) {
            if (token[0] == '.') {
                char* relative = strchr(token, '/');
                if (relative != NULL) {
                    snprintf(symlink, SIR_MAXSLPATH, "%s%s/%s", cwdl, relative + 1,
                        ps.pr_fname);
                } else {
                    snprintf(symlink, SIR_MAXSLPATH, "%s%s", cwdl, ps.pr_fname);
                }

                if (stat(symlink, &statstruct) != -1) {
                    /* Flawfinder: ignore */
                    res = readlink(symlink, temp_buffer, SIR_MAXPATH);
                    if (res < 0)
                        _sir_strncpy(buffer, SIR_MAXPATH, symlink, SIR_MAXPATH);
                    else
                        snprintf(buffer, *size - 1, "%s/%s", (char*)dirname(symlink), temp_buffer);

                    *size = strlen(buffer);
                    return 0;
                }
            } else {
                snprintf(symlink, SIR_MAXPATH, "%s/%s", token, ps.pr_fname);
                if (stat(symlink, &statstruct) != -1) {
                    /* Flawfinder: ignore */
                    res = readlink(symlink, temp_buffer, SIR_MAXPATH);
                    if (res < 0)
                        _sir_strncpy(buffer, SIR_MAXPATH, symlink, SIR_MAXPATH);
                    else
                        snprintf(buffer, *size - 1, "%s/%s", (char*)dirname(symlink), temp_buffer);

                    *size = strlen(buffer);
                    return 0;
                }
            }

            token = strtok_r(NULL, ":", &tokptr);
        }
        return -1;
    }
}
#endif

bool _sir_deletefile(const char* restrict path) {
    if (!_sir_validstr(path))
        return false;

#if !defined(__WIN__)
    if (0 != unlink(path)) {
        _sir_selflog("failed to delete: '%s' (%d)", path, errno);
        return false;
    }
    return true;
#else /* __WIN__ */
    if (!DeleteFileA(path)) {
        _sir_selflog("failed to delete: '%s' (%lu)", path, GetLastError());
        return false;
    }
    return true;
#endif
}

#if defined(__OpenBSD__)
int _sir_openbsdself(char* out, int capacity, int* dirname_length) {
    char buffer1[4096];
    char buffer2[PATH_MAX];
    char buffer3[PATH_MAX];
    char** argv    = (char**)buffer1;
    char* resolved = NULL;
    int length     = -1;

    while (1) {
        int mib[4] = { CTL_KERN, KERN_PROC_ARGS, getpid(), KERN_PROC_ARGV };
        size_t size;

        if (sysctl(mib, 4, NULL, &size, NULL, 0) != 0)
            break;

        if (size > sizeof(buffer1)) {
            argv = (char**)malloc(size);
            if (!argv)
                break;
        }

        if (sysctl(mib, 4, argv, &size, NULL, 0) != 0)
            break;

        if (strchr(argv[0], '/')) {
            resolved = realpath(argv[0], buffer2);
            if (!resolved)
                break;
        } else {
            const char* PATH = getenv("PATH");
            if (!PATH)
                break;
            size_t argv0_length = strlen(argv[0]);
            const char* begin   = PATH;
            while (1) {
                const char* separator = strchr(begin, ':');
                const char* end = separator ? separator : begin + strlen(begin);
                if (end - begin > 0) {
                    if (*(end - 1) == '/')
                        --end;
                    if (((end - begin) + 1UL + argv0_length + 1UL) <= sizeof(buffer2)) {
                        memcpy(buffer2, begin, end - begin);
                        buffer2[end - begin] = '/';
                        memcpy(buffer2 + (end - begin) + 1, argv[0], argv0_length + 1);
                        resolved = realpath(buffer2, buffer3);
                        if (resolved)
                            break;
                    }
                }
                if (!separator)
                    break;
                begin = ++separator;
            }
            if (!resolved)
                break;
        }

        length = (int)strlen(resolved);
        if (length <= capacity) {
            memcpy(out, resolved, (unsigned long)length);
            if (dirname_length) {
                int i;
                for (i = length - 1; i >= 0; --i) {
                    if (out[i] == '/') {
                        *dirname_length = i;
                        break;
                    }
                }
            }
        }
        break;
    }
    if (argv != (char**)buffer1)
        free(argv);

    return length;
}
#endif
