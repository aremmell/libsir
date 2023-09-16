#!/usr/bin/env python3
# libsir Python module setup script
# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Copyright (c) 2018-current Ryan M. Lederman

from distutils.core import setup, Extension
from setuptools.command.build_ext import build_ext

class BuildExt(build_ext):
    def build_extensions(self):
        if '-Wstrict-prototypes' in self.compiler.compiler_so:
            self.compiler.compiler_so.remove('-Wstrict-prototypes')
        if '-Wall' in self.compiler.compiler_so:
            self.compiler.compiler_so.remove('-Wall')
        if '-Werror' in self.compiler.compiler_so:
            self.compiler.compiler_so.remove('-Werror')
        if '-Wno-unused-result' in self.compiler.compiler_so:
            self.compiler.compiler_so.remove('-Wno-unused-result')
        if '-Wsign-compare' in self.compiler.compiler_so:
            self.compiler.compiler_so.remove('-Wsign-compare')
        super().build_extensions()

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

setup(cmdclass={'build_ext': BuildExt},
    name='sir',
    version='2.2.4',
    author="libsir contributors",
    description="""The Standard Incident Reporter Library""",
    ext_modules=[sir_module],
    py_modules=["sir"],
)
