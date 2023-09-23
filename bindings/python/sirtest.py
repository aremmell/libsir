#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Copyright (c) 2018-current Ryan M. Lederman

import sir

if __name__ == "__main__":
    s = sir.InitData()
    if not sir.makeinit(s):
        print("Error: makeinit failed")
        exit(1)

    s.name = "Python!"
    s.d_stdout.opts   = sir.OPT_NOHOST
    s.d_stderr.opts   = sir.OPT_NOHOST
    s.d_syslog.levels = sir.LVL_NONE

    if not sir.init(s):
        print("Error: init failed")
        exit(1)

    sir.debug("This is a test of the %s system. Testing %d" % ("libsir", 123))
    sir.debug("Adding log file %s" % ("sir-python.log"))

    f = sir.addfile("sir-python.log", sir.LVL_ALL, sir.OPT_ALL)
    if f == 0:
        print("Error: addfile failed")
        exit(1)

    sir.info("Testing info level")
    sir.notice("Testing notice level")
    sir.warn("Testing warning level")
    sir.error("Testing error level")
    sir.crit("Testing critical level")
    sir.alert("Testing alert level")
    sir.emerg("Testing emergency level")

    sir.info("Cleaning up...")

    sir.remfile(f)
    sir.cleanup()
