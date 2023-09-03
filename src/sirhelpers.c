/*
 * sirhelpers.c
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2023
 * Version:   2.2.3
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
        (void)_sir_handleerr(errno);

    *fd = -1;
}

void _sir_safefclose(FILE* restrict* restrict f) {
    if (!f || !*f)
        return;

    if (0 != fclose(*f))
        (void)_sir_handleerr(errno);

    *f = NULL;
}

bool _sir_validfd(int fd) {
    /** stdin, stdout, stderr use up 0, 1, 2 */
    if (2 >= fd)
        return _sir_handleerr(EBADF);

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
    return (-1 != ret || EBADF != errno) ? true : _sir_handleerr(errno);
}

/** Validates a sir_update_config_data structure. */
bool _sir_validupdatedata(sir_update_config_data* data) {
    if (!_sir_validptr(data))
        return false;

    bool valid = true;
    if ((data->fields & SIRU_ALL) == 0U || (data->fields & ~SIRU_ALL) != 0U)
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
        (void)_sir_seterror(_SIR_E_INVALID);
        SIR_ASSERT(!valid);
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
         ((levels & ~SIRL_ALL) == 0U)))
         return true;

    _sir_selflog("invalid levels: %04"PRIx16, levels);
    return _sir_seterror(_SIR_E_LEVELS);
}

bool _sir_validlevel(sir_level level) {
    if (SIRL_INFO   == level || SIRL_DEBUG == level ||
        SIRL_NOTICE == level || SIRL_WARN  == level ||
        SIRL_ERROR  == level || SIRL_CRIT  == level ||
        SIRL_ALERT  == level || SIRL_EMERG == level)
        return true;

    _sir_selflog("invalid level: %04"PRIx16, level);
    return _sir_seterror(_SIR_E_LEVELS);
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
         ((opts & ~(SIRO_MSGONLY | SIRO_NOHDR)) == 0U)))
         return true;

    _sir_selflog("invalid options: %08"PRIx32, opts);
    return _sir_seterror(_SIR_E_OPTIONS);
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
            return _sir_seterror(_SIR_E_TEXTATTR);
        }
    }
}

bool _sir_validtextcolor(sir_colormode mode, sir_textcolor color) {
    bool valid = false;
    switch (mode) {
        case SIRCM_16:
            /* in 16-color mode:
             * compare to 30..37, 39, 40..47, 49, 90..97, 100..107. */
            valid = SIRTC_DEFAULT == color ||
                    (color >= 30U && color <= 37U) || color == 39U ||
                    (color >= 40U && color <= 47U) || color == 49U ||
                    (color >= 90U && color <= 97U) || (color >= 100U && color <= 107U);
            break;
        case SIRCM_256:
            /* in 256-color mode: compare to 0..255. sir_textcolor is unsigned,
             * so only need to ensure it's <= 255. */
            valid = SIRTC_DEFAULT == color || color <= 255U;
            break;
        case SIRCM_RGB: {
            /* in RGB-color mode: mask and compare to 0x00ffffff. */
            valid = SIRTC_DEFAULT == color || ((color & 0xff000000U) == 0U);
            break;
        }
        case SIRCM_INVALID: // GCOVR_EXCL_START
        default:
            valid = false;
            break;
    } // GCOVR_EXCL_STOP

    if (!valid) {
        _sir_selflog("invalid text color for mode %d %08"PRIx32" (%"PRIu32")",
            mode, color, color);
        (void)_sir_seterror(_SIR_E_TEXTCOLOR);
    }

    return valid;
}

bool _sir_validcolormode(sir_colormode mode) {
    switch (mode) {
        case SIRCM_16:
        case SIRCM_256:
        case SIRCM_RGB:
            return true;
        case SIRCM_INVALID:
        default: {
            _sir_selflog("invalid color mode: %d", mode);
            return _sir_seterror(_SIR_E_COLORMODE);
        }
    }
}

int _sir_strncpy(char* restrict dest, size_t destsz, const char* restrict src,
    size_t count) {
    if (_sir_validptr(dest) && _sir_validstr(src)) {
# if defined(__HAVE_STDC_SECURE_OR_EXT1__)
        int ret = strncpy_s(dest, destsz, src, count);
        if (0 != ret) {
            (void)_sir_handleerr(ret);
            return -1;
        }
        return 0;
# else
        SIR_UNUSED(count);
        size_t cpy = strlcpy(dest, src, destsz);
        SIR_ASSERT_UNUSED(cpy < destsz, cpy);
        return 0;
# endif
    }

    return -1;
}

int _sir_strncat(char* restrict dest, size_t destsz, const char* restrict src,
    size_t count) {
    if (_sir_validptr(dest) && _sir_validstr(src)) {
# if defined(__HAVE_STDC_SECURE_OR_EXT1__)
        int ret = strncat_s(dest, destsz, src, count);
        if (0 != ret) {
            (void)_sir_handleerr(ret);
            return -1;
        }
        return 0;
# else
        SIR_UNUSED(count);
        size_t cat = strlcat(dest, src, destsz);
        SIR_ASSERT_UNUSED(cat < destsz, cat);
        return 0;
# endif
    }

    return -1;
}

int _sir_fopen(FILE* restrict* restrict streamptr, const char* restrict filename,
    const char* restrict mode) {
    if (_sir_validptrptr(streamptr) && _sir_validstr(filename) && _sir_validstr(mode)) {
#if defined(__HAVE_STDC_SECURE_OR_EXT1__)
        int ret = fopen_s(streamptr, filename, mode);
        if (0 != ret) {
            (void)_sir_handleerr(ret);
            return -1;
        }
        return 0;
#else
        *streamptr = fopen(filename, mode);
        if (!*streamptr) {
            (void)_sir_handleerr(errno);
            return -1;
        }
        return 0;
#endif
    }

    return -1;
}

bool _sir_getchar(char* input) {
    if (!_sir_validptr(input))
        return false;

#if defined(__WIN__)
# if defined(__EMBARCADEROC__)
     *input = (char)getch();
# else
     *input = (char)_getch();
# endif
     return true;
#else /* !__WIN__ */
    struct termios cur = {0}, new = {0};
    if (0 != tcgetattr(STDIN_FILENO, &cur))
        return _sir_handleerr(errno);

    memcpy(&new, &cur, sizeof(struct termios));
    new.c_lflag &= ~(ICANON | ECHO);

    if (0 != tcsetattr(STDIN_FILENO, TCSANOW, &new))
        return _sir_handleerr(errno);

    *input = (char)getchar();

    return 0 == tcsetattr(STDIN_FILENO, TCSANOW, &cur) ? true
        : _sir_handleerr(errno);
#endif
}
