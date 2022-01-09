@echo off

:: I kinda wanted to call this script Razzle, but that's an honour only Microsoft is worthy of

:: Try to set up a Visual C++ environment (copied from a script on my development drive)
set "vs2019_install=C:\Program Files\Microsoft Visual Studio\2022\Preview"
if not exist "%vs2019_install%" (
	set "vs2019_install=C:\Program Files\Microsoft Visual Studio\2022\Community"
)
if not exist "%vs2019_install%" (
	set "vs2019_install=C:\Program Files\Microsoft Visual Studio\2019\Enterprise" :: Github Actions gives enterprise installs
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

set PATH=%CD%\tools;%CD%\out;%CD%\deps\bin;%PATH%
