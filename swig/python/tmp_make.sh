#!/usr/bin/env sh
# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Copyright (c) 2018-current Ryan M. Lederman

command -v swig || {
    printf '\033[31mswig is not installed; exiting!\033[0m\n'
    exit 1
}

command -v python || {
    printf '\033[31mpython is not installed; exiting!\033[0m\n'
    exit 1
}

swig_input_files="sir.i"
swig_options="-python -threads -doxygen -outcurrentdir -v ${swig_input_files}"
printf '\033[1mExecuting swig %s...\033[0m\n' "${swig_options}"

swig ${swig_options} || {
    printf '\033[31mswig exited with non-zero code %d; exiting!\033[0m\n' $?
    exit $?
}

python_options="setup.py build_ext --inplace"
printf '\033[1mExecuting python %s...\033[0m\n' "${python_options}"

python ${python_options} || {
    printf '\033[31mpython exited with non-zero code %d; exiting!\033[0\n' $?
    exit $?
}

printf '\033[32mSuccessfully built libsir Python module!\033[0m\n'
