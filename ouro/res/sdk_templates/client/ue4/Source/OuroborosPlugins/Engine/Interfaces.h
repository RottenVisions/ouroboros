// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "OUROCommon.h"

namespace Ouroboros
{

/*
interface module
Used to declare and implement some callback unified interfaces
*/
class OUROBOROSPLUGINS_API InterfaceConnect
{
public:

	virtual void onConnectCallback(FString ip, uint16 port, bool success, int userdata) = 0;

};


}