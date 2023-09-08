#!/usr/bin/env python

import sir

if __name__ == "__main__":
    s = sir.sirinit()
    if not sir.sir_makeinit(s):
        print("Error: sir_makeinit failed")
        exit(1)

    s.name = "Python!"
    s.d_stdout.levels = sir.SIRL_ALL
    s.d_stdout.opts   = sir.SIRO_NOHOST
    s.d_syslog.levels = sir.SIRL_NONE

    if not sir.sir_init(s):
        print("Error: sir_init failed")
        exit(1)

    sir.sir_debug('This is a test of the %s system. Testing %d' % ('libsir', 123))
    sir.sir_debug('Adding log file %s' % ('sir-python.log'))

    f = sir.sir_addfile('sir-python.log', sir.SIRL_ALL, sir.SIRO_ALL)

    sir.sir_info('Testing info level')
    sir.sir_notice('Testing notice level')
    sir.sir_warn('Testing warning level')
    sir.sir_error('Testing error level')
    sir.sir_crit('Testing critical level')
    sir.sir_alert('Testing alert level')
    sir.sir_emerg('Testing emergency level')
    sir.sir_info('Cleaning up...')

    sir.sir_remfile(f)
    sir.sir_cleanup()
