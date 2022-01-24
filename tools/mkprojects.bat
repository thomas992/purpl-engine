@echo off

call %~dp0buildenv.bat

gn gen %~dp0..\out --export-compile-commands --ide=vs2022
pause
