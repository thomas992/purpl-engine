@echo off

powershell -command ". %~dp0chocoscripts\Get-EnvironmentVariable.ps1; . %~dp0chocoscripts\Get-EnvironmentVariableNames.ps1; . %~dp0chocoscripts\Update-SessionEnvironment.ps1; . %~dp0chocoscripts\Write-FunctionCallLogMessage.ps1; refreshenv"

