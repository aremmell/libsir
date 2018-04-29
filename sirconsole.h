/*!
 * \file sirconsole.h
 * \brief Internal definitions for console color management in the SIR library.
 * \author Ryan Matthew Lederman <lederman@gmail.com>
 */
#ifndef _SIR_CONSOLE_H_INCLUDED
#define _SIR_CONSOLE_H_INCLUDED

#include "sirplatform.h"
#include "sirtypes.h"

/*
fg
30 Black
31 Red
32 Green
33 Yellow
34 Blue
35 Magenta
36 Cyan
37 White

bg
40 Black
41 Red
42 Green
43 Yellow
44 Blue
45 Magenta
46 Cyan
47 White
*/
/*
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

static const WORD bgMask( BACKGROUND_BLUE      | 
 18                                BACKGROUND_GREEN     | 
 19                                BACKGROUND_RED       | 
 20                                BACKGROUND_INTENSITY   );
 21      static const WORD fgMask( FOREGROUND_BLUE      | 
 22                                FOREGROUND_GREEN     | 
 23                                FOREGROUND_RED       | 
 24                                FOREGROUND_INTENSITY   );
 25      
 26      static const WORD fgBlack    ( 0 ); 
 27      static const WORD fgLoRed    ( FOREGROUND_RED   ); 
 28      static const WORD fgLoGreen  ( FOREGROUND_GREEN ); 
 29      static const WORD fgLoBlue   ( FOREGROUND_BLUE  ); 
 30      static const WORD fgLoCyan   ( fgLoGreen   | fgLoBlue ); 
 31      static const WORD fgLoMagenta( fgLoRed     | fgLoBlue ); 
 32      static const WORD fgLoYellow ( fgLoRed     | fgLoGreen ); 
 33      static const WORD fgLoWhite  ( fgLoRed     | fgLoGreen | fgLoBlue ); 
 34      static const WORD fgGray     ( fgBlack     | FOREGROUND_INTENSITY ); 
 35      static const WORD fgHiWhite  ( fgLoWhite   | FOREGROUND_INTENSITY ); 
 36      static const WORD fgHiBlue   ( fgLoBlue    | FOREGROUND_INTENSITY ); 
 37      static const WORD fgHiGreen  ( fgLoGreen   | FOREGROUND_INTENSITY ); 
 38      static const WORD fgHiRed    ( fgLoRed     | FOREGROUND_INTENSITY ); 
 39      static const WORD fgHiCyan   ( fgLoCyan    | FOREGROUND_INTENSITY ); 
 40      static const WORD fgHiMagenta( fgLoMagenta | FOREGROUND_INTENSITY ); 
 41      static const WORD fgHiYellow ( fgLoYellow  | FOREGROUND_INTENSITY );
 42      static const WORD bgBlack    ( 0 ); 
 43      static const WORD bgLoRed    ( BACKGROUND_RED   ); 
 44      static const WORD bgLoGreen  ( BACKGROUND_GREEN ); 
 45      static const WORD bgLoBlue   ( BACKGROUND_BLUE  ); 
 46      static const WORD bgLoCyan   ( bgLoGreen   | bgLoBlue ); 
 47      static const WORD bgLoMagenta( bgLoRed     | bgLoBlue ); 
 48      static const WORD bgLoYellow ( bgLoRed     | bgLoGreen ); 
 49      static const WORD bgLoWhite  ( bgLoRed     | bgLoGreen | bgLoBlue ); 
 50      static const WORD bgGray     ( bgBlack     | BACKGROUND_INTENSITY ); 
 51      static const WORD bgHiWhite  ( bgLoWhite   | BACKGROUND_INTENSITY ); 
 52      static const WORD bgHiBlue   ( bgLoBlue    | BACKGROUND_INTENSITY ); 
 53      static const WORD bgHiGreen  ( bgLoGreen   | BACKGROUND_INTENSITY ); 
 54      static const WORD bgHiRed    ( bgLoRed     | BACKGROUND_INTENSITY ); 
 55      static const WORD bgHiCyan   ( bgLoCyan    | BACKGROUND_INTENSITY ); 
 56      static const WORD bgHiMagenta( bgLoMagenta | BACKGROUND_INTENSITY ); 
 57      static const WORD bgHiYellow ( bgLoYellow  | BACKGROUND_INTENSITY );
 */
typedef enum {
    SIRC_RED = 1,
    SIRC_GREEN,
    SIRC_BLUE,
    SIRC_YELLOW,
    SIRC_MAGENTA,
    SIRC_CYAN,
    SIRC_WHITE,
    SIRC_BLACK,
    SIRC_GRAY
} sircolor;

typedef enum {
    SIRA_BRIGHT = 1,
    SIRA_BOLD,
    SIRA_DIM
} sirattr;

#endif /* !_SIR_CONSOLE_H_INCLUDED */
