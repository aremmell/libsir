#!/usr/bin/env python3
# libsir Python module setup script
# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Copyright (c) 2018-current Ryan M. Lederman

from distutils.core import setup, Extension

sir_module = Extension(
    '_sir',
    sources=[
        'sir_wrap.c',
        '../../src/sir.c',
        '../../src/sircondition.c',
        '../../src/sirconsole.c',
        '../../src/sirfilecache.c',
        '../../src/sirfilesystem.c',
        '../../src/sirhelpers.c',
        '../../src/sirerrors.c',
        '../../src/sirinternal.c',
        '../../src/sirmaps.c',
        '../../src/sirmutex.c',
        '../../src/sirplugins.c',
        '../../src/sirqueue.c',
        '../../src/sirtextstyle.c',
        '../../src/sirthreadpool.c'],
        include_dirs=['../../include'
    ]
)

setup(name='sir',
    version='2.2.4',
    author="libsir contributors",
    description="""The Standard Incident Reporter Library""",
    ext_modules=[sir_module],
    py_modules=["sir"],
)
