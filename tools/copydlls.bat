@echo off
setlocal enabledelayedexpansion

if "%1" == "" (
	echo Missing architecture, must be x64 or ARM64
	exit /b
)

set DLLS=flecs libzstd SDL2

set ROOTDIR=%~dp0..
set DLLSRCDIR=!ROOTDIR!\deps\lib\%1
set DLLDESTDIR=!ROOTDIR!\data\bin

if not exist %DLLSRCDIR% (
	echo Missing DLLs
	exit /b
)
if not exist !DLLDESTDIR! mkdir !DLLDESTDIR!
for %%x in (!DLLS!) do (
	set dll_src=!DLLSRCDIR!\%%x.dll
	set dll_dest=!DLLDESTDIR!\%%x.dll
	echo Copying "!dll_src!" to "!dll_dest!"
	copy /b "!dll_src!" "!dll_dest!"
)

