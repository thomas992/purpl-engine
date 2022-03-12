@echo off

echo Installing dependencies

call %~dp0buildenv
git submodule update --init
pip install markupsafe==2.0.1
pip install jinja2
call %~dp0installvulkan

echo You should now be able to build the engine

