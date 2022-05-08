@echo off

for %%x in (%*) do (
	echo %%x
	type NUL >> %%x
)

