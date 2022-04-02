@echo off

echo Pulling latest commits for submodules
git submodule foreach git pull origin HEAD --force

