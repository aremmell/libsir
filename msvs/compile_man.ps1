#Requires -RunAsAdministrator

# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Copyright (c) 2018-current Ryan M. Lederman

param(
    [Parameter(Mandatory=$true)][string]$provider
)

$dir = split-path $MyInvocation.MyCommand.Definition -Parent
try {
    . ("$dir\util.ps1")
} catch {
    echo-red "failed to load required PowerShell scripts!"
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
(Get-Content ".\$provider.man") -replace 'provider name="([\w-.]+)"',
    "provider name=`"${provider}`"" | Set-Content ".\$provider.man"
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

echo-green ("sucessfully registered logging provider as '$provider'. to unregister, run" +
    " 'wevtutil um $provider.man' `ndone.")
