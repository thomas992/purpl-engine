@echo off

call %~dp0buildenv.bat

devenv %~dp0..\out\win\all.sln
