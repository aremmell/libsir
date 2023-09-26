# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Copyright (c) 2018-current Ryan M. Lederman

function Echo-Color($color, $msg)
{
    $esc=$([char]27)
    Write-Host "${esc}[${color}m${msg}${esc}[0m"
}

function Echo-Blue($msg)
{
    Echo-Color -Color 94 -Msg $msg
}

function Echo-Red($msg)
{
    Echo-Color -Color 31 -Msg $msg
}

function Echo-Green($msg)
{
     Echo-Color -Color 32 -Msg $msg
}