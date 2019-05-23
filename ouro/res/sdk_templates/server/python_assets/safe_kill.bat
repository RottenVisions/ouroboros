@echo off
set curpath=%~dp0

cd ..
set OURO_ROOT=%cd%
set OURO_RES_PATH=%OURO_ROOT%/kbe/res/;%curpath%/;%curpath%/scripts/;%curpath%/res/
set OURO_BIN_PATH=%OURO_ROOT%/kbe/bin/server/

echo OURO_ROOT = %OURO_ROOT%
echo OURO_RES_PATH = %OURO_RES_PATH%
echo OURO_BIN_PATH = %OURO_BIN_PATH%

cd %curpath%

"%OURO_BIN_PATH%/obcmd.exe" --getuid > nul
if not defined uid set uid=%errorlevel%
echo UID = %uid%

if defined OURO_ROOT (python "%OURO_ROOT%/kbe\tools\server\pycluster\cluster_controller.py" stop %uid%) else (python "..\kbe\tools\server\pycluster\cluster_controller.py" stop %uid%)
