@echo off

call %~dp0\buildenv.bat

gn gen %~dp0\..\out --export-compile-commands --ide=vs
pause

