#!/bin/env bash

echo "Exporting environment variables..."
export VCTargetsPath="/c/Program Files/Microsoft Visual Studio/2022/Community/Msbuild/Microsoft/VC/v170/"
echo "Running MSBuild..."
/c/Program\ Files/Microsoft\ Visual\ Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe ./msvs/libsir.sln -t:Rebuild -p:Config=Release -p:Platform=x64 || {
    echo "Command failed with exit code $?!"
}

#echo "Looking for Microsoft.Cpp.Default.props files..."
#find /c/ -name Microsoft.Cpp.Default.props -type f -print 2>/dev/null
