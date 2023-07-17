/*
 * sirhelpers.c
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
#include "sir/helpers.h"
#include "sir/errors.h"

void __sir_safefree(void** pp) {
    if (!pp || !*pp)
        return;

    free(*pp);
    *pp = NULL;
}

void _sir_safeclose(int* restrict fd) {
    if (!fd || 0 > *fd)
        return;

    if (-1 == close(*fd))
        _sir_handleerr(errno);

    *fd = -1;
}

void _sir_safefclose(FILE* restrict* restrict f) {
    if (!f || !*f)
        return;

    if (0 != fclose(*f))
        _sir_handleerr(errno);

    *f = NULL;
}

bool _sir_validfd(int fd) {
    /** stdin, stdout, stderr use up 0, 1, 2 */
    if (2 >= fd) {
        _sir_handleerr(EBADF);
        return false;
    }
#if !defined(__WIN__)
    int ret = fcntl(fd, F_GETFL);
#else /* __WIN__ */
# if !defined(SIR_MSVCRT_MINGW)
    invalparamfn old = _set_thread_local_invalid_parameter_handler(_sir_invalidparameter);
# endif
    struct _stat st;
    int ret = _fstat(fd, &st);
# if !defined(SIR_MSVCRT_MINGW)
    _set_thread_local_invalid_parameter_handler(old);
# endif
#endif
    bool valid = -1 != ret || EBADF != errno;
    if (-1 == ret)
        _sir_handleerr(errno);
    return valid;
}

/** Validates a sir_update_config_data structure. */
bool _sir_validupdatedata(sir_update_config_data* data) {
    if (!_sir_validptr(data))
        return false;

    bool valid = true;
    if ((data->fields & SIRU_ALL) == 0 || (data->fields & ~SIRU_ALL) != 0)
        valid = false;

    if (valid && _sir_bittest(data->fields, SIRU_LEVELS))
        valid &= (_sir_validptrnofail(data->levels) &&
            _sir_validlevels(*data->levels));

    if (valid && _sir_bittest(data->fields, SIRU_OPTIONS))
        valid &= (_sir_validptrnofail(data->opts) &&
            _sir_validopts(*data->opts));

    if (valid && _sir_bittest(data->fields, SIRU_SYSLOG_ID))
        valid &= _sir_validstrnofail(data->sl_identity);

    if (valid && _sir_bittest(data->fields, SIRU_SYSLOG_CAT))
        valid &= _sir_validstrnofail(data->sl_category);

    if (!valid) {
        _sir_seterror(_SIR_E_INVALID);
        SIR_ASSERT("!invalid sir_update_config_data");
    }

    return valid;
}

bool _sir_validlevels(sir_levels levels) {
    if ((SIRL_ALL == levels || SIRL_NONE == levels) ||
        ((_sir_bittest(levels, SIRL_INFO)           ||
         _sir_bittest(levels, SIRL_DEBUG)           ||
         _sir_bittest(levels, SIRL_NOTICE)          ||
         _sir_bittest(levels, SIRL_WARN)            ||
         _sir_bittest(levels, SIRL_ERROR)           ||
         _sir_bittest(levels, SIRL_CRIT)            ||
         _sir_bittest(levels, SIRL_ALERT)           ||
         _sir_bittest(levels, SIRL_EMERG))          &&
         ((levels & ~SIRL_ALL) == 0)))
         return true;

    _sir_selflog("invalid levels: %04"PRIx16, levels);
    _sir_seterror(_SIR_E_LEVELS);

    return false;
}

bool _sir_validlevel(sir_level level) {
    if (SIRL_INFO   == level || SIRL_DEBUG == level ||
        SIRL_NOTICE == level || SIRL_WARN  == level ||
        SIRL_ERROR  == level || SIRL_CRIT  == level ||
        SIRL_ALERT  == level || SIRL_EMERG == level)
        return true;

    _sir_selflog("invalid level: %04"PRIx32, level);
    _sir_seterror(_SIR_E_LEVELS);
    return false;
}

bool _sir_validopts(sir_options opts) {
    if ((SIRO_ALL == opts || SIRO_MSGONLY == opts) ||
        ((_sir_bittest(opts, SIRO_NOTIME)          ||
         _sir_bittest(opts, SIRO_NOHOST)           ||
         _sir_bittest(opts, SIRO_NOLEVEL)          ||
         _sir_bittest(opts, SIRO_NONAME)           ||
         _sir_bittest(opts, SIRO_NOMSEC)           ||
         _sir_bittest(opts, SIRO_NOPID)            ||
         _sir_bittest(opts, SIRO_NOTID)            ||
         _sir_bittest(opts, SIRO_NOHDR))           &&
         ((opts & ~(SIRO_MSGONLY | SIRO_NOHDR)) == 0)))
         return true;

    _sir_selflog("invalid options: %08"PRIx32, opts);
    _sir_seterror(_SIR_E_OPTIONS);

    return false;
}

bool _sir_validtextattr(sir_textattr attr) {
    switch(attr) {
        case SIRTA_NORMAL:
        case SIRTA_BOLD:
        case SIRTA_DIM:
        case SIRTA_EMPH:
        case SIRTA_ULINE:
            return true;
        default: {
            _sir_selflog("invalid text attr: %d", attr);
            _sir_seterror(_SIR_E_TEXTATTR);
            return false;
        }
    }
}

bool _sir_validtextcolor(sir_colormode mode, sir_textcolor color) {
    if (!_sir_validcolormode(mode))
        return false;

    bool valid = false;
    switch (mode) {
        case SIRCM_16:
            /* in 16-color mode:
             * compare to 30..37, 39, 40..47, 49, 90..97, 100..107. */
            valid = SIRTC_DEFAULT == color ||
                    (color >= 30 && color <= 37) || color == 39 ||
                    (color >= 40 && color <= 47) || color == 49 ||
                    (color >= 90 && color <= 97) || (color >= 100 && color <= 107);
            break;
        case SIRCM_256:
            /* in 256-color mode: compare to 0..255. sir_textcolor is unsigned,
             * so only need to ensure it's <= 255. */
            valid = SIRTC_DEFAULT == color || color <= 255;
            break;
        case SIRCM_RGB: {
            /* in RGB-color mode: mask and compare to 0x00ffffff. */
            valid = SIRTC_DEFAULT == color || ((color & 0xff000000) == 0);
            break;
        }
        case SIRCM_INVALID:
        default:
            valid = false;
            break;
    }

    if (!valid) {
        _sir_selflog("invalid text color for mode %d %08"PRIx32" (%"PRId32")",
            mode, color, color);
        _sir_seterror(_SIR_E_TEXTCOLOR);
    }

    return valid;
}

bool _sir_validcolormode(sir_colormode mode) {
    switch (mode) {
        case SIRCM_16:
        case SIRCM_256:
        case SIRCM_RGB:
            return true;
        default: {
            _sir_selflog("invalid color mode: %d", mode);
            _sir_seterror(_SIR_E_COLORMODE);
            return false;
        }
    }
}

bool __sir_validstr(const char* restrict str, bool fail) {
    bool valid = str && (*str != '\0');
    if (!valid && fail) {
        _sir_seterror(_SIR_E_STRING);
        SIR_ASSERT(!"invalid string");
    }
    return valid;
}

bool __sir_validptr(const void* restrict p, bool fail) {
    bool valid = NULL != p;
    if (!valid && fail) {
        _sir_seterror(_SIR_E_NULLPTR);
        SIR_ASSERT(!"NULL pointer");
    }
    return valid;
}

bool __sir_validptrptr(const void* restrict* pp, bool fail) {
    bool valid = NULL != pp;
    if (!valid && fail) {
        _sir_seterror(_SIR_E_NULLPTR);
        SIR_ASSERT("!NULL pointer");
    }
    return valid;
}

int _sir_strncpy(char* restrict dest, size_t destsz, const char* restrict src, size_t count) {
    if (_sir_validptr(dest) && _sir_validstr(src)) {
#if defined(__HAVE_STDC_SECURE_OR_EXT1__)
        int ret = strncpy_s(dest, destsz, src, count);
        if (0 != ret) {
            _sir_handleerr(ret);
            return -1;
        }
        return 0;
#else
        _SIR_UNUSED(count);
        size_t cpy = strlcpy(dest, src, destsz);
        SIR_ASSERT(cpy < destsz);
        _SIR_UNUSED(cpy);
        return 0;
#endif
    }

    return -1;
}

int _sir_strncat(char* restrict dest, size_t destsz, const char* restrict src, size_t count) {
    if (_sir_validptr(dest) && _sir_validstr(src)) {
#if defined(__HAVE_STDC_SECURE_OR_EXT1__)
        int ret = strncat_s(dest, destsz, src, count);
        if (0 != ret) {
            _sir_handleerr(ret);
            return -1;
        }
        return 0;
#else
        _SIR_UNUSED(count);
        size_t cat = strlcat(dest, src, destsz);
        SIR_ASSERT(cat < destsz);
        _SIR_UNUSED(cat);
        return 0;
#endif
    }

    return -1;
}

int _sir_fopen(FILE* restrict* restrict streamptr, const char* restrict filename,
    const char* restrict mode) {
    if (_sir_validptrptr(streamptr) && _sir_validstr(filename) && _sir_validstr(mode)) {
#if defined(__HAVE_STDC_SECURE_OR_EXT1__)
        int ret = fopen_s(streamptr, filename, mode);
        if (0 != ret) {
            _sir_handleerr(ret);
            return -1;
        }
        return 0;
#else
        *streamptr = fopen(filename, mode);
        if (!*streamptr) {
            _sir_handleerr(errno);
            return -1;
        }
        return 0;
#endif
    }

    return -1;
}

struct tm* _sir_localtime(const time_t* restrict timer, struct tm* restrict buf) {
    if (_sir_validptr(timer) && _sir_validptr(buf)) {
#if defined(__HAVE_STDC_SECURE_OR_EXT1__)
# if defined(__WIN__)
        errno_t ret = (errno_t)localtime_s(buf, timer);
        if (0 != ret) {
            _sir_handleerr(ret);
            return NULL;
        }

        return buf;
# else // __WIN__
        struct tm* ret = localtime_s(timer, buf);
        if (!ret)
            _sir_handleerr(errno);

        return ret;
# endif
#else
        _SIR_UNUSED(buf);
        struct tm* ret = localtime(timer);
        if (!ret)
            _sir_handleerr(errno);
        return ret;
#endif
    }

    return NULL;
}

int _sir_getchar(void) {
#if defined(__WIN__)
    return _getch();
#else // !__WIN__
    struct termios cur = {0};
    struct termios new = {0};

    int get = tcgetattr(STDIN_FILENO, &cur);
    if (0 != get) {
        _sir_handleerr(errno);
        return -1;
    }

    memcpy(&new, &cur, sizeof(struct termios));
    new.c_lflag &= ~(ICANON | ECHO);

    int set = tcsetattr(STDIN_FILENO, TCSANOW, &new);
    if (0 != set) {
        _sir_handleerr(errno);
        return -1;
    }

    int ch = getchar();

    set = tcsetattr(STDIN_FILENO, TCSANOW, &cur);
    if (0 != set) {
        _sir_handleerr(errno);
        return -1;
    }

    return ch;
#endif
}
