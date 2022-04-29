@echo off

:: I kinda wanted to call this script Razzle, but that's an honour only Microsoft is worthy of

:: Try to set up a Visual C++ environment (copied from a script on my development drive)
for /F "tokens=* USEBACKQ" %%F in (`"%~dp0vswhere.exe" -all -prerelease -latest -products * -property installationPath`) do (
	set vs2019_install=%%F
)

echo Found Visual Studio at %vs2019_install%

for /F "tokens=* USEBACKQ" %%F in (`type "%vs2019_install%\VC\Auxiliary\Build\Microsoft.VCToolsVersion.default.txt"`) do (
	set msvc_defaultver=%%F
)
echo Default MSVC version is %msvc_defaultver%
set "msvc_path=%vs2019_install%\VC\Tools\MSVC\%msvc_defaultver%"
c:
pushd "%vs2019_install%\VC\Auxiliary\Build"
call vcvarsall.bat %PURPL_BUILDENV_ARCH% %*
popd
%~d0

:: Set the architecture
for %%G in (%PURPL_BUILDENV_ARCH% %1) do (
	set PURPL_BUILDENV_ARCH=%%G
)

set "PATH=%~dp0;%~dp0..\build\install;%PATH%"
set PURPL_DATA_DIR=%~dp0..

echo Build environment for architecture '%PURPL_BUILDENV_ARCH%' initialized
