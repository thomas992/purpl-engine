@echo off

for /f "usebackq" %%f in (`cmd /c "dir /s/b"`) do findstr /i "%1" %%f && echo. & echo %%f & echo.

