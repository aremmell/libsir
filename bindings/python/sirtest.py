#!/usr/bin/env python3

################################################################################
#
# Version: 2.2.5
#
################################################################################
#
# SPDX-License-Identifier: MIT
#
# Copyright (c) 2018-2024 Ryan M. Lederman <lederman@gmail.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
################################################################################

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
