#Requires -RunAsAdministrator

################################################################################
#
# Version: 2.2.6
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

param(
    [Parameter(Mandatory=$true)][string]$provider
)

$dir = split-path $MyInvocation.MyCommand.Definition -Parent
try {
    . ("$dir\util.ps1")
} catch {
    echo-red "failed to load required PowerShell scripts!"
    exit 1
}

$machine  = "x64"
$filename = "sir_wineventlog"

# generates .h and .rc files
echo-blue "generating $filename.h and $filename.rc..."
mc -vc ".\$filename.man"
exit-on-failure "failed!"

# converts the .rc into a .res
echo-blue "converting $filename.rc -> $filename.res..."
rc -nologo ".\$filename.rc"
exit-on-failure "failed!"

# links the resource-only DLL
$dll_file = "${filename}_${machine}.dll"

echo-blue "linking $filename.res into resource-only DLL $dll_file..."
link -nologo -machine:$machine -dll -noentry -release -out:$dll_file .\$filename.res
exit-on-failure "failed!"

# have to add read permission explicitly or event viewer can't access
echo-blue "setting permissions..."
icacls ".\$dll_file" /grant:r Everyone:RX
exit-on-failure "failed!"

# make a copy of the input manifest and rename it
echo-blue "making a copy of $filename.man at $provider.man..."
copy-item -Force .\$filename.man ".\$provider.man"
exit-on-failure "failed to copy .\$filename.man to .\$provider.man!"

# replace the provider name with the one from the command line
echo-blue "replacing provider name with '$provider'..."
(get-content ".\$provider.man") -replace 'provider name="([\w-.]+)"',
    "provider name=`"${provider}`"" | set-content ".\$provider.man"
exit-on-failure "failed to replace provider name with '$provider' in .\$provider.man!"

# copy the DLL to the system directory
$target_dir = "${env:SystemRoot}\System32"
echo-blue "copying .\$dll_file to $target_dir..."
copy-item -Force ".\$dll_file" "${target_dir}\$dll_file"
exit-on-failure "failed to copy .\$dll_file to $target_dir! is event viewer running?"

# register the manifest, replacing the file names
echo-blue "registering $provider.man..."
wevtutil im /rf:${target_dir}\$dll_file /mf:${target_dir}\$dll_file `
    /pf:${target_dir}\$dll_file ".\$provider.man"
exit-on-failure "failed to register logging provider!"

echo-green ("successfully registered logging provider as '$provider'. to unregister, run" +
    " 'wevtutil um $provider.man' `ndone.")
