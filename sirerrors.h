/**
 * @file sirerrors.h
 * @brief Library error management.
 */
#ifndef _SIR_ERRORS_H_INCLUDED
#define _SIR_ERRORS_H_INCLUDED

#include "sirhelpers.h"
#include "sirtypes.h"

#define SIR_E_NOERROR   0x00000000  /**< The operation completed successfully */
#define SIR_E_UNKNOWN   0x8fff0000  /**< Represents the unknown error case. */
#define SIR_E_NOTREADY  _sir_mkerror(1)  /**< SIR has not been initialized */
#define SIR_E_ALREADY   _sir_mkerror(2)  /**< SIR is already initialized */
#define SIR_E_DUPFILE   _sir_mkerror(3)  /**< File already managed by SIR */
#define SIR_E_NOFILE    _sir_mkerror(4)  /**< File not managed by SIR */
#define SIR_E_FCFULL    _sir_mkerror(5)  /**< Maximum number of files already managed */
#define SIR_E_OPTIONS   _sir_mkerror(6)  /**< Option flags are invalid */
#define SIR_E_LEVELS    _sir_mkerror(7)  /**< Level flags are invalid */
#define SIR_E_TEXTSTYLE _sir_mkerror(8)  /**< Text style is invalid */
#define SIR_E_STRING    _sir_mkerror(9)  /**< Invalid string argument */
#define SIR_E_NODEST    _sir_mkerror(10) /**< No destinations registered for level */

static const sirerror sir_errors[] = {
    {SIR_E_NOERROR, "The operation completed successfully"},
    {SIR_E_NOTREADY, "SIR has not been initialized"},
    {SIR_E_ALREADY, "SIR is already initialized"},
    {SIR_E_DUPFILE, "File already managed by SIR"},
    {SIR_E_NOFILE, "File not managed by SIR"},
    {SIR_E_FCFULL, "Maximum number of files already managed"},
    {SIR_E_OPTIONS, "Option flags are invalid"},
    {SIR_E_LEVELS, "Level flags are invalid"},
    {SIR_E_TEXTSTYLE, "Text style is invalid"},
    {SIR_E_STRING, "Invalid string argument"},
    {SIR_E_NODEST, "No destinations registered for level"}
};

void _sir_seterror(sirerror_t err);
sirerror_t _sir_geterror(sirchar_t message[SIR_MAXERROR]);

#endif /* !_SIR_ERRORS_H_INCLUDED */
