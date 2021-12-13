@echo off

@rem Try to set up a Visual C++ environment (copied from a script on my development drive)
if "%COMPAT%"=="1" (
	set "vs2019_install=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community"
) else (
	set "vs2019_install=C:\Program Files\Microsoft Visual Studio\2022\Preview"
)

for /F "tokens=* USEBACKQ" %%F in (`type "%vs2019_install%\VC\Auxiliary\Build\Microsoft.VCToolsVersion.default.txt"`) do (
    set msvc_defaultver=%%F
)
set msvc_path=%vs2019_install%\VC\Tools\MSVC\%msvc_defaultver%
c:
pushd "%vs2019_install%\VC\Auxiliary\Build"
call vcvarsall.bat x64
popd
%~d0

set PATH=%CD%\tools;%CD%\tools\vcpkg;%PATH%

set VCPKG_DEFAULT_TRIPLET=x64-windows

