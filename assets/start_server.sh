#!/bin/sh

currPath=$(pwd)
keyStr="/ouroboros/"

bcontain=`echo $currPath|grep $keyStr|wc -l`


if [ $bcontain = 0 ]
then
	export OURO_ROOT=$(cd ../; pwd)
else
	export OURO_ROOT="$(pwd | awk -F "/ouroboros/" '{print $1}')/ouroboros"
fi



export OURO_RES_PATH="$OURO_ROOT/ouro/res/:$(pwd):$(pwd)/res:$(pwd)/scripts/"
export OURO_BIN_PATH="$OURO_ROOT/ouro/bin/server/"

echo OURO_ROOT = \"${OURO_ROOT}\"
echo OURO_RES_PATH = \"${OURO_RES_PATH}\"
echo OURO_BIN_PATH = \"${OURO_BIN_PATH}\"

sh ./kill_server.sh

$OURO_BIN_PATH/machine --cid=2129652375332859700 --gus=1&
$OURO_BIN_PATH/logger --cid=1129653375331859700 --gus=2&
$OURO_BIN_PATH/interfaces --cid=1129652375332859700 --gus=3&
$OURO_BIN_PATH/dbmgr --cid=3129652375332859700 --gus=4&
$OURO_BIN_PATH/baseappmgr --cid=4129652375332859700 --gus=5&
$OURO_BIN_PATH/cellappmgr --cid=5129652375332859700 --gus=6&
$OURO_BIN_PATH/baseapp --cid=6129652375332859700 --gus=7&
$OURO_BIN_PATH/cellapp --cid=7129652375332859700 --gus=8&
$OURO_BIN_PATH/loginapp --cid=8129652375332859700 --gus=9&
