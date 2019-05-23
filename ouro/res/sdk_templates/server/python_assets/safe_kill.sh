#!/bin/sh

currPath=$(pwd)
keyStr="/ouroboros/"

bcontain=`echo $currPath|grep $keyStr|wc -l`


if [ $bcontain = 0 ]
then
	export OURO_ROOT="$(cd ../; pwd)"
else
	export OURO_ROOT="$(pwd | awk -F "/ouroboros/" '{print $1}')/ouroboros"
fi



export OURO_RES_PATH="$OURO_ROOT/kbe/res/:$(pwd):$(pwd)/res:$(pwd)/scripts/"
export OURO_BIN_PATH="$OURO_ROOT/kbe/bin/server/"

userid=`id -u $1 &>/dev/null`
XUID=$?

if [ $XUID = 0 ]
then
	OBCMD_PATH="$OURO_BIN_PATH/obcmd"
	XUID=`"$OBCMD_PATH" --getuid`
	echo UID=$XUID
fi

python "$OURO_ROOT/kbe/tools/server/pycluster/cluster_controller.py" stop $XUID
