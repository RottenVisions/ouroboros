@echo off
set curpath=%~dp0

cd ..
set OURO_ROOT=%cd%
set OURO_RES_PATH=%OURO_ROOT%/ouro/res/;%curpath%/;%curpath%/scripts/;%curpath%/res/
set OURO_BIN_PATH=%OURO_ROOT%/ouro/bin/server/

cd %OURO_ROOT%/ouro/tools/server/guiconsole/
start guiconsole.exe
