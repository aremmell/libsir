function Echo-Color($color, $msg)
{
    $esc=$([char]27)
    Write-Output "${esc}[${color}m${msg}${esc}[0m"
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

# generates .h and .rc files
Echo-Blue -Msg "Generating .h and .rc..."
mc -uc sir_winlog_provider.man
if (!$?)
{
    Echo-Red -Msg "failed!"
    exit 1
}

# converts the .rc into a .res
Echo-Blue -Msg "Converting rc -> res..."
rc sir_winlog_provider.rc
if (!$?)
{
    Echo-Red -Msg "failed!"
    exit 1
}

# links the resource-only DLL
Echo-Blue -Msg "linking .res into resource-only DLL..."
link -machine:x64 -dll -noentry -release -out:sir_winlog_provider_x64.dll sir_winlog_provider.res
if (!$?)
{
    Echo-Red -Msg "failed!"
    exit 1
}

# elevates and executes the command that installs the provider
Echo-Blue -Msg "starting wevtutil in an elevated shell to install the provider..."
Start-Process -FilePath wevtutil.exe -Verb Runas -ArgumentList "im ${pwd}\sir_winlog_provider.man"
if (!$?)
{
    Echo-Red -Msg "failed!"
    exit 1
}

 Echo-Green -Msg "done. all commands completed successfully."
