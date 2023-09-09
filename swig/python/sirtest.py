#!/usr/bin/env python

import sir

if __name__ == "__main__":
    s = sir.InitData()
    if not sir.MakeInit(s):
        print("Error: MakeInit failed")
        exit(1)

    s.name = "Python!"
    s.d_stdout.levels = sir.SIRL_ALL
    s.d_stdout.opts   = sir.SIRO_NOHOST
    s.d_syslog.levels = sir.SIRL_NONE

    if not sir.Init(s):
        print("Error: Init failed")
        exit(1)

    sir.Debug('This is a test of the %s system. Testing %d' % ('libsir', 123))
    sir.Debug('Adding log file %s' % ('sir-python.log'))

    f = sir.AddFile('sir-python.log', sir.SIRL_ALL, sir.SIRO_ALL)
    if f == 0:
        print("Error: AddFile failed")
        exit(1)

    sir.Info('Testing info level')
    sir.Notice('Testing notice level')
    sir.Warn('Testing warning level')
    sir.Error('Testing error level')
    sir.Crit('Testing critical level')
    sir.Alert('Testing alert level')
    sir.Emerg('Testing emergency level')
    sir.Info('Cleaning up...')

    sir.RemFile(f)
    sir.Cleanup()
