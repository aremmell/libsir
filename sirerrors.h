/**
 * @file sirerrors.h
 * @brief Library error management.
 */
#ifndef _SIR_ERRORS_H_INCLUDED
#define _SIR_ERRORS_H_INCLUDED

#include "sirhelpers.h"
#include "sirtypes.h"

#define SIR_E_NOERROR   0           /**< The operation completed successfully */
#define SIR_E_NOTREADY  makeerr(1)  /**< SIR has not been initialized */
#define SIR_E_ALREADY   makeerr(2)  /**< SIR is already initialized */
#define SIR_E_DUPFILE   makeerr(3)  /**< File already managed by SIR */
#define SIR_E_NOFILE    makeerr(4)  /**< File not managed by SIR */
#define SIR_E_OPTIONS   makeerr(5)  /**< Option flags are invalid */
#define SIR_E_LEVELS    makeerr(6)  /**< Level flags are invalid */
#define SIR_E_TEXTSTYLE makeerr(7)  /**< Text style is invalid */
#define SIR_E_STRING    makeerr(8)  /**< Invalid string argument */
#define SIR_E_NODEST    makeerr(9)  /**< No destinations registered for level */

static const sirerror sir_errors[] = {
    {SIR_E_NOERROR, "The operation completed successfully"},
    {SIR_E_NOTREADY, "SIR has not been initialized"},
    {SIR_E_ALREADY, "SIR is already initialized"},
    {SIR_E_DUPFILE, "File already managed by SIR"},
    {SIR_E_NOFILE, "File not managed by SIR"},
    {SIR_E_OPTIONS, "Option flags are invalid"},
    {SIR_E_LEVELS, "Level flags are invalid"},
    {SIR_E_TEXTSTYLE, "Text style is invalid"},
    {SIR_E_STRING, "Invalid string argument"},
    {SIR_E_NODEST, "No destinations registered for level"}
};

void _sir_seterror(const sirerrcode_t err);
sirerrcode_t _sir_geterror(sirchar_t message[SIR_MAXERROR]);

#endif /* !_SIR_ERRORS_H_INCLUDED */
