@echo off

:: Swaps out DOS line endings for Unix ones (hence the name, "ddos" sounds like de-DOS)
for /f "usebackq" %%f in (`dir /s/b`) do dos2unix %%f
