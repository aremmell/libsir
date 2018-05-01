/*!
 * \file sirconsole.h
 * \brief Internal implementation of console color management in the SIR library.
 * \author Ryan Matthew Lederman <lederman@gmail.com>
 */
#include "sirconsole.h"
#include "sirinternal.h"
#include "sirdefaults.h"

/* \cond PRIVATE */

#ifndef _WIN32

static bool _sir_write_std(const sirchar_t* message, FILE* stream);

bool _sir_stderr_write(const sirchar_t* message) {
    return _sir_write_std(message, stderr);
}

bool _sir_stdout_write(const sirchar_t* message) {
    return _sir_write_std(message, stdout);
}

static bool _sir_write_std(const sirchar_t* message, FILE* stream) {

    assert(validstr(message));
    assert(stream);

    int write = validstr(message) ? fputs(message, stream) : -1;
    assert(write >= 0);

    return write >= 0;
}

#else

static bool _sir_write_stdwin32(uint16_t style, const sirchar_t* message, HANDLE console);

bool _sir_stderr_write(uint16_t style, const sirchar_t* message) {
    return _sir_write_stdwin32(style, message, GetStdHandle(STD_ERROR_HANDLE));
}

bool _sir_stdout_write(uint16_t style, const sirchar_t* message) {   
    return _sir_write_stdwin32(style, message, GetStdHandle(STD_OUTPUT_HANDLE));
}

static bool _sir_write_stdwin32(uint16_t style, const sirchar_t* message, HANDLE console) {

    assert(validstr(message));
    assert(INVALID_HANDLE_VALUE != console);

    if (INVALID_HANDLE_VALUE == console) {
        _sir_handleerr(GetLastError());
        return false;
    }

    CONSOLE_SCREEN_BUFFER_INFO csbfi = {0};

    if (!GetConsoleScreenBufferInfo(console, &csbfi)) {
        _sir_handleerr(GetLastError());
        return false;
    }

    if (!SetConsoleTextAttribute(console, style)) {
        _sir_handleerr(GetLastError());
        return false;
    }        

    size_t chars = strnlen(message, SIR_MAXOUTPUT);
    DWORD written = 0;

    do {
        DWORD pass = 0;

        if (!WriteConsole(console, message, chars, &pass, NULL)) {
            _sir_handleerr(GetLastError());
            break;
        }

        written += pass;
    } while (written < chars);

    SetConsoleTextAttribute(console, csbfi.wAttributes);

    return written == chars;    
}

#endif  /* !_WIN32 */

sir_textstyle _sir_getdefstyle(sir_level level) {

    assert(validlevels(level));

    for (size_t n = 0; n < _COUNTOF(sir_default_styles); n++) {
        if (sir_default_styles[n].level == level)
            return sir_default_styles[n].style;
    }

    return SIRS_INVALID;
}

uint16_t _sir_getprivstyle(uint16_t cat) {

    for (size_t n = 0; n < _COUNTOF(sir_priv_map); n++) {
        if (sir_priv_map[n].from == cat) {
            return sir_priv_map[n].to;
        }
    }

    return _sir_getprivstyle(SIRS_NONE);    
}

bool _sir_formatstyle(sir_textstyle style, sirchar_t* buf, size_t size) {
    
    assert(buf);

    if (buf) {
        
        uint16_t attr = (style & _SIRS_ATTR_MASK);
        uint16_t fg = (style & _SIRS_FG_MASK);
        uint16_t bg = (style & _SIRS_BG_MASK);
    
        bool attrvalid = attr <= SIRS_BRIGHT;
        bool fgvalid = fg <= SIRS_FG_WHITE;
        bool bgvalid = bg <= SIRS_BG_WHITE;

        assert(attrvalid);
        assert(fgvalid);
        assert(bgvalid);

        if (attrvalid && fgvalid && bgvalid) {

            uint16_t privattr = _sir_getprivstyle(attr);
            uint16_t privfg = _sir_getprivstyle(fg);
            uint16_t privbg = _sir_getprivstyle(bg);

 #ifndef _WIN32
            sirchar_t fgfmt[4] = {0};
            sirchar_t bgfmt[4] = {0};

            if (privfg != 0)
                snprintf(fgfmt, 4, ";%2hd", privfg);

            if (privbg != 0)
                snprintf(bgfmt, 4, ";%2hd", privbg);

            /* '\e[nn;nn;nm' */
            snprintf(buf, size, "\033[%1d%s%sm", privattr, fgfmt, bgfmt);

            return validstr(buf);
#else
            uint16_t final = privattr | privfg | privbg;
            memcpy(buf, &final, sizeof(uint16_t));
            memset(buf + sizeof(uint16_t), 0, SIR_MAXSTYLE - sizeof(uint16_t));
            return true;
#endif
        }    
    }

    return false;    
}

/* \endcond PRIVATE */
