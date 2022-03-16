@echo off

setlocal

set INSTALLER=vulkan-sdk.exe

echo Downloading Vulkan
curl -fGL https://sdk.lunarg.com/sdk/download/latest/windows/vulkan-sdk.exe -o %INSTALLER%

echo Installing Vulkan
vulkan-sdk --accept-licenses --default-answer --confirm-command install

if not "%ERRORLEVEL%" == "0" (
	echo Failed to install Vulkan SDK, error %ERRORLEVEL%. Make sure you have the permissions required, and if you already have an install and want a fresh one to uninstall it first
) else (
	echo Vulkan SDK installed
)

echo Deleting installer
del /s/q/f %INSTALLER%

endlocal

