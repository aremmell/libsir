/*!
 * \file sirmutex.c
 * 
 * Implementation of mutex platform abstractions for the
 * Standard Incident Reporter (SIR) library.
 *
 * \author Ryan Matthew Lederman <lederman@gmail.com>
 * \version 1.1.0
 * \date 2003-2018
 * \copyright MIT License
 */
#include "sirmutex.h"
#include "sirplatform.h"

/*! \cond PRIVATE */

bool _sirmutex_trylock(sirmutex_t* mutex) {
    return false;
}

bool _sirmutex_lock(sirmutex_t* mutex) {
    return false;
}

void _sirmutex_unlock(sirmutex_t* mutex) {
}

void _sirmutex_destroy(sirmutex_t* mutex) {
}

/*! \endcond */
