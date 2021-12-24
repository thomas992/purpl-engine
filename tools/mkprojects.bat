@echo off

call tools\buildenv.bat

gn gen %~dp0\..\out
pause

