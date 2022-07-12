@echo off

makepkg genmap /f %~dp0..\build\chunks.xml /d %~dp0..\build\game
makepkg pack /f %~dp0..\build\chunks.xml /d %~dp0..\build\game /pd %~dp0..\build

