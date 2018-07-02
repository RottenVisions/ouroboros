@echo off
set curpath=%~dp0
if defined OURO_ROOT (echo %OURO_ROOT%) else set OURO_ROOT=%curpath:~0,-28%
if defined OURO_RES_PATH (echo %OURO_RES_PATH%) else set OURO_RES_PATH=%OURO_ROOT%ouro/res/;%OURO_ROOT%assets/;%OURO_ROOT%assets/scripts/;%OURO_ROOT%assets/res/
if defined OURO_BIN_PATH (echo %OURO_BIN_PATH%) else set OURO_BIN_PATH=%OURO_ROOT%ouro/bin/server/

if defined uid (echo %uid%) else set uid=%random%%%32760+1

start guiconsole.exe