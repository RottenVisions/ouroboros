@echo off
set curpath=%~dp0

cd ..
set OURO_ROOT=%cd%
set OURO_RES_PATH=%OURO_ROOT%/ouro/res/;%curpath%/;%curpath%/scripts/;%curpath%/res/
set OURO_BIN_PATH=%OURO_ROOT%/ouro/bin/server/

if defined uid (echo UID = %uid%)

echo OURO_ROOT = %OURO_ROOT%
echo OURO_RES_PATH = %OURO_RES_PATH%
echo OURO_BIN_PATH = %OURO_BIN_PATH%

cd %curpath%
start %OURO_BIN_PATH%/obcmd.exe --clientsdk=unity --outpath=%curpath%/plugins/Ouroboros
