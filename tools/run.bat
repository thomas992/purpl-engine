@echo off

pushd %~dp0
echo Running from %CD%

set PATH=%PATH%;%~dp0bin
%~dp0demo.exe
popd

pause
