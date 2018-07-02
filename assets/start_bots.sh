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

$OURO_BIN_PATH/bots&

