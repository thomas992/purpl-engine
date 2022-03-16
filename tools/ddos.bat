@echo off

:: Swaps out DOS line endings for Unix ones (hence the name, "ddos" sounds like de-DOS and a file from a Hollywood movie :) )
for /f "usebackq" %%f in (`cmd /c dir /s/b ^| findstr /v ".git build deps"`) do %~dp0dos2unix %%f
