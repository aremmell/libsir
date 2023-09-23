#!/usr/bin/env python3
# libsir Python module setup script
# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Copyright (c) 2018-current Ryan M. Lederman

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
    version="2.2.4",
    author="libsir contributors",
    description="""The Standard Incident Reporter Library""",
    ext_modules=[sir_module],
    py_modules=["sir"],
)
