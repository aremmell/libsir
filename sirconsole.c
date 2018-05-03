/*!
 * \file sirconsole.c
 */
#include "sirconsole.h"
#include "sirinternal.h"
#include "sirtextstyle.h"

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

static CRITICAL_SECTION stdout_cs;
static sironce_t        stdout_once = SIR_ONCE_INIT;

static CRITICAL_SECTION stderr_cs;
static sironce_t        stderr_once = SIR_ONCE_INIT;

static bool _sir_write_stdwin32(uint16_t style, const sirchar_t* message, HANDLE console);
static BOOL CALLBACK _sir_initcs(PINIT_ONCE ponce, PVOID param, PVOID* ctx);

bool _sir_stderr_write(uint16_t style, const sirchar_t* message) {
    BOOL initcs = InitOnceExecuteOnce(&stderr_once, _sir_initcs, &stderr_cs, NULL);
    assert(FALSE != initcs);

    EnterCriticalSection(&stderr_cs);
    bool r = _sir_write_stdwin32(style, message, GetStdHandle(STD_ERROR_HANDLE));
    LeaveCriticalSection(&stderr_cs);
    return r;
}

bool _sir_stdout_write(uint16_t style, const sirchar_t* message) {
    BOOL initcs = InitOnceExecuteOnce(&stdout_once, _sir_initcs, &stdout_cs, NULL);
    assert(FALSE != initcs);

    EnterCriticalSection(&stdout_cs);
    bool r = _sir_write_stdwin32(style, message, GetStdHandle(STD_OUTPUT_HANDLE));
    LeaveCriticalSection(&stdout_cs);
    return r;
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

    size_t chars   = strnlen(message, SIR_MAXOUTPUT) - 1;
    DWORD  written = 0;

    do {
        DWORD pass = 0;

        if (!WriteConsole(console, message + written, chars - written, &pass, NULL)) {
            _sir_handleerr(GetLastError());
            break;
        }

        written += pass;
    } while (written < chars);

    /* Have to set the attributes back before writing the newline
     * or you'll get background color artifacting before the next output.
     */
    SetConsoleTextAttribute(console, csbfi.wAttributes);
    DWORD newline = 0;
    WriteConsole(console, "\n", 1, &newline, NULL);

    return written == chars;
}

static BOOL CALLBACK _sir_initcs(PINIT_ONCE ponce, PVOID param, PVOID* ctx) {
    InitializeCriticalSection((LPCRITICAL_SECTION)param);
    return TRUE;
}

#endif /* !_WIN32 */

/* \endcond PRIVATE */
