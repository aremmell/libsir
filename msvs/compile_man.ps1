# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Copyright (c) 2018-current Ryan M. Lederman

$dir = split-path $MyInvocation.MyCommand.Definition -Parent
try {
    . ("$dir\util.ps1")
} catch {
    write-host "failed to load required PowerShell scripts!"
}

$machine  = "x64"
$filename = "sir_wineventlog"
$install_ps1 = "$dir\install_man.ps1"

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

# prompt the user, letting them know the next step requires elevation
while ($True) {
    $answer = read-host "the logging provider must now be installed, which requires" `
        "elevation. proceed [y/n]? "

    if ($answer -ieq "n") {
        echo-blue "run $install_ps1 as administrator to complete installation"
        exit 0
    }

    if ($answer -ieq "y") {
        break
    }

    echo-red "the only valid responses are 'y' or 'n'"
}



echo-green "done. all commands completed successfully."
