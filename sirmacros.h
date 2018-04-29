/*!
 * \file sirmacros.h
 *
 * Utility macros for the Standard Incident Reporter (SIR) library.
 *
 * \author Ryan Matthew Lederman <lederman@gmail.com>
 * \version 1.1.0
 * \date 2003-2018
 * \copyright MIT License
 */
#ifndef _SIR_MACROS_H_INCLUDED
#define _SIR_MACROS_H_INCLUDED

#define _SIR_L_START(format) \
    bool    r = false;       \
    va_list args;            \
    va_start(args, format);

#define _SIR_L_END(args) va_end(args);

#endif /* !_SIR_MACROS_H_INCLUDED */
