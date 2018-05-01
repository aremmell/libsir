/*!
 * \file sirconsole.h
 * \brief Internal definitions for console color management in the SIR library.
 * \author Ryan Matthew Lederman <lederman@gmail.com>
 */
#ifndef _SIR_CONSOLE_H_INCLUDED
#define _SIR_CONSOLE_H_INCLUDED

#include "sirplatform.h"
#include "sirtypes.h"

/*! \cond PRIVATE */

static const sir_style_priv_map sir_priv_map[] = {

#ifndef _WIN32
    {SIRS_NONE, 0},
    {SIRS_BRIGHT, 1},
    {SIRS_FG_BLACK, 30},    
    {SIRS_FG_RED, 31},
    {SIRS_FG_GREEN, 32},
    {SIRS_FG_YELLOW, 33},
    {SIRS_FG_BLUE, 34},
    {SIRS_FG_MAGENTA, 35},
    {SIRS_FG_CYAN, 36},
    {SIRS_FG_WHITE, 37},
    {SIRS_BG_BLACK, 40},    
    {SIRS_BG_RED, 41},
    {SIRS_BG_GREEN, 42},
    {SIRS_BG_YELLOW, 43},    
    {SIRS_BG_BLUE, 44},
    {SIRS_BG_MAGENTA, 45},
    {SIRS_BG_CYAN, 46},
    {SIRS_BG_WHITE, 47}    
#else
    {SIRS_NONE, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE},
    {SIRS_BRIGHT, FOREGROUND_INTENSITY},
    {SIRS_FG_BLACK, 0},    
    {SIRS_FG_RED, FOREGROUND_RED,
    {SIRS_FG_GREEN, FOREGROUND_GREEN},
    {SIRS_FG_YELLOW, FOREGROUND_RED | FOREGROUND_GREEN},
    {SIRS_FG_BLUE, FOREGROUND_BLUE},
    {SIRS_FG_MAGENTA, FOREGROUND_RED | FOREGROUND_BLUE},
    {SIRS_FG_CYAN, FOREGROUND_GREEN | FOREGROUND_BLUE},
    {SIRS_FG_WHITE, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE},
    {SIRS_BG_BLACK, 0},
    {SIRS_BG_RED, BACKGROUND_RED},
    {SIRS_BG_GREEN, BACKGROUND_GREEN},
    {SIRS_BG_YELLOW, BACKGROUND_RED | BACKGROUND_GREEN},    
    {SIRS_BG_BLUE, BACKGROUND_BLUE},
    {SIRS_BG_MAGENTA, BACKGROUND_RED | BACKGROUND_BLUE},
    {SIRS_BG_CYAN, BACKGROUND_GREEN | BACKGROUND_BLUE},
    {SIRS_BG_WHITE, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE}    
#endif
};

bool             _sir_stderr_write(const sirchar_t* message);
bool             _sir_stdout_write(const sirchar_t* message);

/* const sir_style_map _sir_getdefstyle(sir_level level);
const uint16_t _sir_getprivstyle(uint32_t from);

void _sir_splitstyle(sir_textstyle style, uint16_t* attr, uint16_t* fg, uint16_t* bg);
bool _sir_getfinalstyle(sir_style_map style, sir_textstyle_final* out);
bool _sir_privtofinal(uint16_t attr, uint16_t fg, uint16_t bg, sir_textstyle_final* out); */

/*! \endcond PRIVATE */

#endif /* !_SIR_CONSOLE_H_INCLUDED */
