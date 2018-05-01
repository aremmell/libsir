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

static bool _sir_write_std(sir_style_map style, const sirchar_t* message, FILE* stream);

bool _sir_stderr_write(const sirchar_t* message) {
    sir_style_map style;    
    return _sir_write_std(style, message, stderr);
}

bool _sir_stdout_write(const sirchar_t* message) {
    sir_style_map style;    
    return _sir_write_std(style, message, stdout);
}

static bool _sir_write_std(sir_style_map style, const sirchar_t* message, FILE* stream) {

    assert(validstr(message));
    assert(stream);



    int write = validstr(message) ? fputs(message, stream) : -1;
    assert(write >= 0);

    return write >= 0;
}

#else

static bool _sir_write_stdwin32(sir_style_map style, const sirchar_t* message, HANDLE console);

bool _sir_stderr_write(const sirchar_t* message) {
    sir_style_map style;
    return _sir_write_stdwin32(style, message, GetStdHandle(STD_ERROR_HANDLE));
}

bool _sir_stdout_write(const sirchar_t* message) {   
    sir_style_map style;    
    return _sir_write_stdwin32(style, message, GetStdHandle(STD_OUTPUT_HANDLE));
}

static bool _sir_write_stdwin32(sir_style_map style,const sirchar_t* message, HANDLE console) {
#pragma message "apply style on win32"
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

#endif  /* !_WIN32 */

//https://docs.microsoft.com/en-us/windows/console/getconsolescreenbufferinfo
//https://docs.microsoft.com/en-us/windows/console/setconsoletextattribute

// 65001
//https://docs.microsoft.com/en-us/windows/console/setconsoleoutputcp

/*const sir_style_map _sir_getdefstyle(sir_level level) {

    static const sir_style_map invalid = {_SIRS_INVALID};
    assert(validlevels(level));

    for (size_t n = 0; n < _COUNTOF(sir_default_styles); n++) {
        if (sir_default_styles[n].level == level)
            return sir_default_styles[n];
    }

    return invalid;
}

const uint16_t _sir_getprivstyle(uint32_t from) {

    for (size_t n = 0; n < _COUNTOF(sir_priv_map); n++) {
        if (sir_priv_map[n].from == from) {
            return sir_priv_map[n].to;
        }
    }

    return 0;
}

void _sir_splitstyle(sir_textstyle style, uint16_t* attr, uint16_t* fg, uint16_t* bg) {
    
    uint16_t tmpattr = (style & _SIRS_ATTR_MASK);
    uint16_t tmpfg = (style & _SIRS_FG_MASK);
    uint16_t tmpbg = (style & _SIRS_BG_MASK);
   
    assert(validstylecat(tmpattr));
    assert(validstylecat(tmpfg));
    assert(validstylecat(tmpbg));

    *attr = tmpattr;
    *fg = tmpfg;
    *bg = tmpbg;
}

bool _sir_getfinalstyle(sir_style_map style, sir_textstyle_final* out) {
    
/*     uint16_t attr = (style.attr & _SIRS_ATTR_MASK);
    validstylecat(attr);

    if (SIRS_NONE != attr) {
        if (SIRS_DEFAULT == attr) {
        }
    }

    uint16_t privattr = _sir_getprivstyle(attr);
    uint16_t privfg = _sir_getprivstyle(fg);
    uint16_t privbg = _sir_getprivstyle(bg); 

    _sir_selflog("%s: attr = 0x%hx, fg = 0x%hx, bg = 0x%hx, pattr: 0x%hx, pfg: 0x%hx, pbg: 0x%hx\n", __func__,
        attr, fg, bg, privattr, privfg, privbg);

    return _sir_privtofinal(privattr, privfg, privbg, out);
}

bool _sir_privtofinal(uint16_t attr, uint16_t fg, uint16_t bg, sir_textstyle_final* out) {
 #ifndef _WIN32

    assert(out);

    if (out) {
        bool bright = flagtest(attr, SIRS_BRIGHT);
        sirchar_t fgfmt[4] = {0};
        sirchar_t bgfmt[4] = {0};

        if (fg != 0)
            snprintf(fgfmt, 4, ";%02hd", fg);

        if (bg != 0)
            snprintf(bgfmt, 4, ";%02hd", bg);

        /* '\33[nn;nn;nm' 
        snprintf(*out, _SIR_MAXSTYLE, "\033[%02d%s%sm", bright ? 1 : 0,
            fgfmt, bgfmt);
    }

    return validstr(*out);
#else
#pragma message "TODO: color finalizing on win32"
#endif
}*/

/* \endcond PRIVATE */
