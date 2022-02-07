@echo off

for /f %%i in ("%1") do set dllpath=%%~dpni

if "%2" == "" (
	set defname=%dllpath%.def
) else (
	set defname=%2
)

echo Writing to %defname%

for %%A in ("%dllpath%") do (set dllbasename=%%~nxA)

echo LIBRARY %dllbasename%> %defname%
echo.>> %defname%
echo EXPORTS>> %defname%

setlocal enabledelayedexpansion
for /f "tokens=1-4" %%1 in ('dumpbin /exports %1') do (
	set /a ordinal=%%1 2>nul
	set /a hint=0x%%2 2>nul
	set /a rva=0x%%3 2>nul
	if !ordinal! equ %%1 if !hint! equ 0x%%2 if !rva! equ 0x%%3 echo 	%%4 @%%1>> %defname%
	echo Processed export %%4 with ordinal %%1 at RVA %%3
)
