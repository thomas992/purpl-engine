@echo off

:: I kinda wanted to call this script Razzle, but that's an honour only Microsoft is worthy of

:: Try to set up a Visual C++ environment (copied from a script on my development drive)
for /F "tokens=* USEBACKQ" %%F in (`"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -property installationPath`) do (
	set vs2019_install=%%F
)

for /F "tokens=* USEBACKQ" %%F in (`type "%vs2019_install%\VC\Auxiliary\Build\Microsoft.VCToolsVersion.default.txt"`) do (
    set msvc_defaultver=%%F
)
echo Default MSVC version is %msvc_defaultver%
set msvc_path=%vs2019_install%\VC\Tools\MSVC\%msvc_defaultver%
c:
pushd "%vs2019_install%\VC\Auxiliary\Build"
call vcvarsall.bat x64
popd
%~d0

set PATH=%CD%\tools;%CD%\out;%CD%\deps\win-x64\bin;%PATH%

