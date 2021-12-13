@echo off

call tools\buildenv.bat

cmake -S%~dp0\.. -B%~dp0\..\build
pause

