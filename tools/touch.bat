@echo off

for %%i in (%*) do if not (%%i == %1) type NUL >> %%i

