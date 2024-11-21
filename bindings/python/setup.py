#!/usr/bin/env python3

# libsir Python module setup script
################################################################################
#
# Version: 2.2.6
#
################################################################################
#
# SPDX-License-Identifier: MIT
#
# Copyright (c) 2018-2024 Ryan M. Lederman <lederman@gmail.com>
# Copyright (c) 2018-2024 Jeffrey H. Johnson <trnsz@pobox.com>
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

from setuptools.command.build_ext import build_ext
from distutils.core import setup, Extension

class BuildExt(build_ext):
    def build_extensions(self):
        self.compiler.compiler_so = list( filter( lambda x: x != "-fasynchronous-unwind-tables", self.compiler.compiler_so))
        self.compiler.compiler_so = list( filter( lambda x: x != "-fcf-protection", self.compiler.compiler_so))
        self.compiler.compiler_so = list( filter( lambda x: x != "-fexceptions", self.compiler.compiler_so))
        self.compiler.compiler_so = list( filter( lambda x: x != "-fno-exceptions", self.compiler.compiler_so))
        self.compiler.compiler_so = list( filter( lambda x: x != "-fno-strict-overflow", self.compiler.compiler_so))
        self.compiler.compiler_so = list( filter( lambda x: x != "-fstack-clash-protection", self.compiler.compiler_so))
        self.compiler.compiler_so = list( filter( lambda x: x != "-fstack-protector-all", self.compiler.compiler_so))
        self.compiler.compiler_so = list( filter( lambda x: x != "-fstack-protector-none", self.compiler.compiler_so))
        self.compiler.compiler_so = list( filter( lambda x: x != "-fstack-protector-strong", self.compiler.compiler_so))
        self.compiler.compiler_so = list( filter( lambda x: x != "-grecord-gcc-switches", self.compiler.compiler_so))
        self.compiler.compiler_so = list( filter( lambda x: x != "-mtune=generic", self.compiler.compiler_so))
        self.compiler.compiler_so = list( filter( lambda x: x != "-pipe", self.compiler.compiler_so))
        self.compiler.compiler_so = list( filter( lambda x: x != "-Wall", self.compiler.compiler_so))
        self.compiler.compiler_so = list( filter( lambda x: x != "-Werror=format-security", self.compiler.compiler_so))
        self.compiler.compiler_so = list( filter( lambda x: x != "-Werror", self.compiler.compiler_so))
        self.compiler.compiler_so = list( filter( lambda x: x != "-Wformat-security", self.compiler.compiler_so))
        self.compiler.compiler_so = list( filter( lambda x: x != "-Wformat", self.compiler.compiler_so))
        self.compiler.compiler_so = list( filter( lambda x: x != "-Wno-unused-result", self.compiler.compiler_so))
        self.compiler.compiler_so = list( filter( lambda x: x != "-Wsign-compare", self.compiler.compiler_so))
        self.compiler.compiler_so = list( filter( lambda x: x != "-Wstrict-prototypes", self.compiler.compiler_so))
        super().build_extensions()

sir_module = Extension(
    "_sir",
    sources=["sir_wrap.c"],
    include_dirs=["../../include"],
    extra_link_args=["-L../../build/lib", "-lsir_s"],
)

setup(
    cmdclass={"build_ext": BuildExt},
    name="sir",
    version="2.2.6",
    author="libsir contributors",
    description="""The Standard Incident Reporter Library""",
    ext_modules=[sir_module],
    py_modules=["sir"],
)
