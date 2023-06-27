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
        switch (rel_to) {
            case SIR_PATH_REL_TO_APP: base_path = _sir_getappdir(); break;
            case SIR_PATH_REL_TO_CWD: base_path = _sir_getcwd(); break;
            default: _sir_seterror(_SIR_E_INVALID); return false;
        }

        if (!base_path) {
            _sir_handleerr(errno);
            return false;
        }

#if !defined(__WIN__)
# if defined(__MACOS__) || defined(_AIX)
        int open_flags = O_SEARCH;
# elif defined(__linux__)
        int open_flags = O_PATH | O_DIRECTORY;
# elif defined(__FreeBSD__)
        int open_flags = O_EXEC | O_DIRECTORY;
# elif defined(__NetBSD__) || defined(__DragonFly__) || defined(__SOLARIS__)
        int open_flags = O_DIRECTORY;
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
#else /* __WIN__ */
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

#if defined(_AIX)
char cur[PATH_MAX];
#endif
char* _sir_getcwd(void) {
#if !defined(__WIN__)
# if defined(__linux__) && defined(_GNU_SOURCE)
    char* cur = get_current_dir_name();
    if (NULL == cur)
        _sir_handleerr(errno);
    return cur;
# elif defined(_AIX)
    if (getcwd(cur, sizeof(cur)) == 0) {
        _sir_handleerr(errno);
        return NULL;
    } else {
        return cur;
    }
# else
    char* cur = getcwd(NULL, 0);
    if (NULL == cur)
        _sir_handleerr(errno);
# endif
    return cur;
#else /* __WIN__ */
    char* cur = _getcwd(NULL, 0);
    if (NULL == cur)
        _sir_handleerr(errno);
    return cur;
#endif
}

#if defined(_AIX)
# include <sys/procfs.h>

int
_sir_aix_exepath(char *buffer, size_t *size)
{
    ssize_t        res;
    char           cwd[PATH_MAX*2+1], cwdl[PATH_MAX+1];
    char           symlink[PATH_MAX*2+17], temp_buffer[PATH_MAX*2+1];
    char           pp[64];
    struct psinfo  ps;
    int            fd;
    char **        argv;

    if (( buffer == NULL ) || ( size == NULL ))
      {
        return -EINVAL;
      }

    snprintf(pp, sizeof ( pp ), "/proc/%lu/psinfo", (unsigned long)getpid());

    fd = open(pp, O_RDONLY);
    if (fd < 0)
      {
        return fd;
      }

    res = read(fd, &ps, sizeof ( ps ));
    close(fd);
    if (res < 0)
      {
        return res;
      }

    if (ps.pr_argv == 0)
      {
        return -EINVAL;
      }

    argv = (char **)*((char ***)(intptr_t)ps.pr_argv );

    if (( argv == NULL ) || ( argv[0] == NULL ))
      {
        return -EINVAL;
      }

    if (argv[0][0] == '/')
      {
        snprintf(symlink, PATH_MAX * 2 - 1, "%s", argv[0]);

        res = readlink(symlink, temp_buffer, PATH_MAX * 2 - 1);

        if (res < 0)
          {
            assert(*size > strlen(symlink));
            strcpy(buffer, symlink);
          }
        else
          {
            assert(*size > ( strlen(symlink) + 1 + strlen(temp_buffer)));
            snprintf(buffer, *size - 1, "%s/%s", (char *)dirname(symlink), temp_buffer);
          }

        *size = strlen(buffer);
        return 0;
      }
    else if (argv[0][0] == '.')
      {
        char *relative = strchr(argv[0], '/');
        if (relative == NULL)
          {
            return -EINVAL;
          }

        snprintf(cwd, PATH_MAX * 2 - 1, "/proc/%lu/cwd",
            (unsigned long)getpid());

        res = readlink(cwd, cwdl, sizeof ( cwdl ) - 1);
        if (res < 0)
          {
            return -errno;
          }

        snprintf(symlink, PATH_MAX * 2 + 1, "%s%s", cwdl, relative + 1);

        res = readlink(symlink, temp_buffer, PATH_MAX * 2 - 1);
        if (res < 0)
          {
            assert(*size > strlen(symlink));
            strcpy(buffer, symlink);
          }
        else
          {
            assert(*size > ( strlen(symlink) + 1 + strlen(temp_buffer)));
            snprintf(buffer, *size - 1, "%s/%s", (char *)dirname(symlink), temp_buffer);
          }

        *size = strlen(buffer);
        return 0;
      }
    else if (strchr(argv[0], '/') != NULL)
      {
        snprintf(cwd, PATH_MAX * 2 - 1, "/proc/%lu/cwd",
            (unsigned long)getpid());

        res = readlink(cwd, cwdl, sizeof ( cwdl ) - 1);
        if (res < 0)
          {
            return -errno;
          }

        snprintf(symlink, PATH_MAX * 2 + 1, "%s%s", cwdl, argv[0]);

        res = readlink(symlink, temp_buffer, PATH_MAX * 2 - 1);
        if (res < 0)
          {
            assert(*size > strlen(symlink));
            strcpy(buffer, symlink);
          }
        else
          {
            assert(*size > ( strlen(symlink) + 1 + strlen(temp_buffer)));
            snprintf(buffer, *size - 1, "%s/%s", (char *)dirname(symlink), temp_buffer);
          }

        *size = strlen(buffer);
        return 0;
      }
    else
      {
        char         clonedpath[16384];
        char *       token = NULL;
        struct stat  statstruct;

        char *       path = getenv("PATH");
        if (sizeof ( clonedpath ) <= strlen(path))
          {
            return -EINVAL;
          }

        strcpy(clonedpath, path);

        token = strtok(clonedpath, ":");

        snprintf(cwd, PATH_MAX * 2 - 1, "/proc/%lu/cwd",
            (unsigned long)getpid());
        res = readlink(cwd, cwdl, sizeof ( cwdl ) - 1);
        if (res < 0)
          {
            return -errno;
          }

        while (token != NULL)
          {
            if (token[0] == '.')
              {
                char *relative = strchr(token, '/');
                if (relative != NULL)
                  {
                    snprintf(symlink, PATH_MAX*2+17, "%s%s/%s",
                                cwdl, relative + 1, ps.pr_fname);
                  }
                else
                  {
                    snprintf(symlink, PATH_MAX*2+16, "%s%s",
                                cwdl, ps.pr_fname);
                  }

                if (stat(symlink, &statstruct) != -1)
                  {
                    res = readlink(symlink, temp_buffer, PATH_MAX * 2 - 1);
                    if (res < 0)
                      {
                        assert(*size > strlen(symlink));
                        strcpy(buffer, symlink);
                      }
                    else
                      {
                        assert(
                         *size > ( strlen(symlink) + 1 + strlen(temp_buffer)));
                        snprintf(buffer, *size - 1, "%s/%s",
                                (char *)dirname(symlink), temp_buffer);
                      }

                    *size = strlen(buffer);
                    return 0;
                  }
              }
            else
              {
                snprintf(symlink, PATH_MAX * 2 - 1, "%s/%s", token, ps.pr_fname);
                if (stat(symlink, &statstruct) != -1)
                  {
                    res = readlink(symlink, temp_buffer, PATH_MAX * 2 - 1);
                    if (res < 0)
                      {
                        assert(*size > strlen(symlink));
                        strcpy(buffer, symlink);
                      }
                    else
                      {
                        assert(
                         *size > ( strlen(symlink) + 1 + strlen(temp_buffer)));
                        snprintf(buffer, *size - 1, "%s/%s",
                                (char *)dirname(symlink), temp_buffer);
                      }

                    *size = strlen(buffer);
                    return 0;
                  }
              }

            token = strtok(NULL, ":");
          }
        return -EINVAL;
      }
}
#endif

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

#if defined(__linux__)
# define PROC_SELF "/proc/self/exe"
#elif defined(__DragonFly__)
# define PROC_SELF "/proc/curproc/file"
#elif defined(__NetBSD__)
# define PROC_SELF "/proc/curproc/exe"
#elif defined(__SOLARIS__)
# define PROC_SELF "/proc/self/path/a.out"
#endif

#if !defined(__WIN__)
# if defined(__linux__) || defined(__DragonFly__) || \
      defined(__NetBSD__) || defined(__SOLARIS__)
#  if defined(__HAVE_UNISTD_READLINK__)
        ssize_t read = readlink(PROC_SELF, buffer, size - 1);
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
# elif defined(_AIX)
        size_t  size;
        size  = sizeof ( &buffer ) / sizeof ( buffer[0] );
        int ret   = _sir_aix_exepath(buffer, &size);
        if (ret == 0) {
            resolved = true;
            break;
        } else if (ret == -1) {
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
#else /* __WIN__ */
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
