/**
 * @file sirhelpers.c
 * @brief Internal macros and inline functions.
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
#include "sirhelpers.h"
#include "sirerrors.h"

/**
 * @addtogroup intern
 * @{
 */

bool _sir_validfid(int id) {
    bool valid = id >= 0;
    if (!valid) {
        _sir_seterror(_SIR_E_NOFILE);
        assert(valid);
    }
    return valid;
}

bool _sir_validlevels(sir_levels levels) {
    bool valid = levels <= SIRL_ALL;
    if (!valid) {
        _sir_seterror(_SIR_E_LEVELS);
        assert(valid);
    }
    return valid;
}

bool _sir_validlevel(sir_level level) {
    bool valid = 0 != level && !(level & (level - 1));
    if (!valid) {
        _sir_seterror(_SIR_E_LEVELS);
        assert(valid);
    }
    return valid;
}

bool _sir_validopts(sir_options opts) {
    bool valid = (opts & SIRL_ALL) == 0 && opts <= 0xfff00;
    if (!valid) {
        _sir_seterror(_SIR_E_OPTIONS);
        assert(valid);
    }
    return valid;
}

bool __sir_validstr(const sirchar_t* str, bool fail) {
    bool valid = str && (*str != (sirchar_t)'\0');
    if (!valid && fail) {
        _sir_seterror(_SIR_E_STRING);
        assert(valid);
    }
    return valid;
}

int _sir_strncpy(sirchar_t* restrict dest, size_t destsz, const sirchar_t* restrict src, size_t count) {
    if (_sir_validptr(dest) && _sir_validstr(src)) {
#if defined(__HAVE_STDC_SECURE_OR_EXT1__)
        int ret = strncpy_s(dest, destsz, src, count);
        if (0 != ret) {
            _sir_handleerr(ret);
            return -1;
        }

        return 0;
#else
        _SIR_UNUSED(destsz);
        strncpy(dest, src, count);
        return 0;
#endif
    }

    return -1;
}

/**
  * Wrapper for strncat/strncat_s. Determines which one to use
  * based on preprocessor macros.
  */
int _sir_strncat(sirchar_t* restrict dest, size_t destsz, const sirchar_t* restrict src, size_t count) {
    if (_sir_validptr(dest) && _sir_validstr(src)) {
#if defined(__HAVE_STDC_SECURE_OR_EXT1__)
        int ret = strncat_s(dest, destsz, src, count);
        if (0 != ret) {
            _sir_handleerr(ret);
            return -1;
        }

        return 0;
#else
        _SIR_UNUSED(destsz);
        strncat(dest, src, count);
        return 0;
#endif
    }

    return -1;
}

/**
  * Wrapper for fopen/fopen_s. Determines which one to use
  * based on preprocessor macros.
  */
int _sir_fopen(FILE* restrict* restrict streamptr, const sirchar_t* restrict filename, const sirchar_t* restrict mode) {
    if (_sir_validptr(streamptr) && _sir_validstr(filename) && _sir_validstr(mode)) {
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
#if     defined(_WIN32)
        errno_t ret = localtime_s(buf, timer);
        if (0 != ret) {
            _sir_handleerr(ret);
            return NULL;
        }

        return buf;
#else
        struct tm* ret = localtime_s(timer, buf);
        if (!ret)
            _sir_handleerr(errno);

        return ret;
#endif
#else
        _SIR_UNUSED(buf);
        return localtime(timer);
#endif
    }

    return NULL;
}

int _sir_getchar(void) {
#if defined(_WIN32)
    return _getch();
#else
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
/** @} */
