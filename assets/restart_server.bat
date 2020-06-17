taskkill /f /t /im machine.exe > nul 2> nul
taskkill /f /t /im logger.exe > nul 2> nul
taskkill /f /t /im dbmgr.exe > nul 2> nul
taskkill /f /t /im baseappmgr.exe > nul 2> nul
taskkill /f /t /im cellappmgr.exe > nul 2> nul
taskkill /f /t /im baseapp.exe > nul 2> nul
taskkill /f /t /im cellapp.exe > nul 2> nul
taskkill /f /t /im loginapp.exe > nul 2> nul
taskkill /f /t /im interfaces.exe > nul 2> nul
taskkill /f /t /im bots.exe > nul 2> nul

timeout /t 1 /nobreak > nul

@echo off
set curpath=%~dp0

cd ..
set OURO_ROOT=%cd%
set OURO_RES_PATH=%OURO_ROOT%/ouro/res/;%curpath%/;%curpath%/scripts/;%curpath%/res/
set OURO_BIN_PATH=%OURO_ROOT%/ouro/bin/server/

if defined uid (echo UID = %uid%)

cd %curpath%
call "kill_server.bat"

echo OURO_ROOT = %OURO_ROOT%
echo OURO_RES_PATH = %OURO_RES_PATH%
echo OURO_BIN_PATH = %OURO_BIN_PATH%

start %OURO_BIN_PATH%/machine.exe --cid=1000 --gus=1
start %OURO_BIN_PATH%/logger.exe --cid=2000 --gus=2
start %OURO_BIN_PATH%/interfaces.exe --cid=3000 --gus=3
start %OURO_BIN_PATH%/dbmgr.exe --cid=4000 --gus=4
start %OURO_BIN_PATH%/baseappmgr.exe --cid=5000 --gus=5
start %OURO_BIN_PATH%/cellappmgr.exe --cid=6000 --gus=6
start %OURO_BIN_PATH%/baseapp.exe --cid=7001 --gus=7
@rem start %OURO_BIN_PATH%/baseapp.exe --cid=7002 --gus=8 --hide=1
start %OURO_BIN_PATH%/cellapp.exe --cid=8001 --gus=9
@rem start %OURO_BIN_PATH%/cellapp.exe --cid=8002  --gus=10 --hide=1
start %OURO_BIN_PATH%/loginapp.exe --cid=9000 --gus=11