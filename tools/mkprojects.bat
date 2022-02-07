@echo off

call %~dp0buildenv.bat

gn gen %~dp0..\out\win --export-compile-commands --ide=vs2022
pause
