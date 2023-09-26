#Requires -RunAsAdministrator

# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Copyright (c) 2018-current Ryan M. Lederman

$dir = split-path $MyInvocation.MyCommand.Definition -Parent
try {
    . ("$dir\util.ps1")
} catch {
    write-host "failed to load required PowerShell scripts!"
    exit 1
}

$machine  = "x64"
$filename = "sir_wineventlog"

# generates .h and .rc files
echo-blue "generating $filename.h and $filename.rc..."
mc -vc ".\$filename.man"
if (!$?) {
    echo-red "failed!"
    exit 1
}

# converts the .rc into a .res
echo-blue "converting $filename.rc -> $filename.res..."
rc -nologo ".\$filename.rc"
if (!$?) {
    echo-red "failed!"
    exit 1
}

# links the resource-only DLL
echo-blue "linking $filename.res into resource-only DLL ${filename}_${machine}.dll..."
link -nologo -machine:$machine -dll -noentry -release -out:${filename}_${machine}.dll ".\$filename.res"
if (!$?) {
    echo-red "failed!"
    exit 1
}

# have to add read permission explicitly or event viewer can't access
echo-blue "setting permissions..."
icacls ".\${filename}_${machine}.dll" /grant:r Everyone:RX
if (!$?) {
    echo-red "failed!"
    exit 1
}

# get the provider name from the user
$provider = ""

while ($provider -eq "") {
    $provider = read-host "enter the name that should appear in the event viewer"
}

# make a copy of the input manifest and rename it
echo-blue "making a copy of $filename.man at $provider.man..."
copy-item -Force .\$filename.man ".\$provider.man"
if (!$?) {
    echo-red "failed to copy .\$filename.man to .\$provider.man!"
    exit 1
}

# replace the provider name with the one from the command line
echo-blue "replacing provider name with '$provider'..."
(Get-Content ".\$provider.man") -replace 'provider name="([\w-.]+)"',
    "provider name=`"${provider}`"" | Set-Content ".\$provider.man"
if (!$?) {
    echo-red "failed to replace provider name with '$provider' in .\$provider.man!"
    exit 1
}

# copy the DLL to the system directory
$target_dir = "${env:SystemRoot}\System32"

echo-blue "copying .\${filename}_${machine}.dll to $target_dir..."
copy-item -Force ".\${filename}_${machine}.dll" "${target_dir}\${filename}_${machine}.dll"
if (!$?) {
    echo-red "failed to copy .\${filename}_${machine}.dll to $target_dir!"
    exit 1
}

# register the manifest, replacing the file names
echo-blue "registering $provider.man..."
wevtutil im /rf:${target_dir}\${filename}_${machine}.dll `
    /mf:${target_dir}\${filename}_${machine}.dll `
    /pf:${target_dir}\${filename}_${machine}.dll ".\$provider.man"
if (!$?) {
    echo-red "failed to register logging provider!"
    exit 1
}

echo-green ("sucessfully registered logging provider. to unregister, run" +
    " 'wevtutil um $provider.man' `ndone.")
