@echo off
set curpath=%~dp0

cd ..
set OURO_ROOT=%cd%
set OURO_RES_PATH=%OURO_ROOT%/kbe/res/;%curpath%/;%curpath%/scripts/;%curpath%/res/
set OURO_BIN_PATH=%OURO_ROOT%/kbe/bin/server/

cd %OURO_ROOT%/kbe/tools/server/guiconsole/
start guiconsole.exe
