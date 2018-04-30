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

    int write = validstr(message) ? -1 : fputs(message, stream);
    assert(write >= 0);

    return write >= 0;    
}

const sir_textstyle_priv _sir_getsysstyle(sir_textstyle style) {
    return 0;
}

#else

static bool _sir_write_stdwin32(const sirchar_t* message, HANDLE console);

bool _sir_stderr_write(const sirchar_t* message) {
    return _sir_write_stdwin32(message, GetStdHandle(STD_ERROR_HANDLE));
}

bool _sir_stdout_write(const sirchar_t* message) {
    return _sir_write_stdwin32(message, GetStdHandle(STD_OUTPUT_HANDLE));
}

static bool _sir_write_stdwin32(const sirchar_t* message, HANDLE console) {

    assert(validstr(message));
    assert(INVALID_HANDLE_VALUE != console);

    if (INVALID_HANDLE_VALUE == console) {
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

    return written == chars;    
}

const sir_textstyle_priv _sir_getsysstyle(sir_textstyle style) {
    return 0;
}

#endif  /* !_WIN32 */

//https://docs.microsoft.com/en-us/windows/console/getconsolescreenbufferinfo
//https://docs.microsoft.com/en-us/windows/console/setconsoletextattribute

// 65001
//https://docs.microsoft.com/en-us/windows/console/setconsoleoutputcp

const sir_textstyle _sir_getdefstyle(sir_level level) {

    assert(validlevels(level));

    for (size_t n = 0; n < _COUNTOF(sir_default_styles); n++) {
        if (sir_default_styles[n].level == level)
            return sir_default_styles[n].styles;
    }

    return SIRS_NONE;
}

/* \endcond PRIVATE */
