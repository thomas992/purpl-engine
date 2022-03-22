@echo off

for /f "usebackq" %%f in (`cmd /c "dir /s/b | findstr /v .git"`) do findstr /i "%1" %%f && echo. & echo %%f & echo.

