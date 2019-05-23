// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "KBVar.h"
#include "OUROCommon.h"

namespace Ouroboros
{

class DATATYPE_BASE;

/*
	Entity defined method module
	Abstract a method defined in a def file, and the modified module class provides related description information about the method.
	For example: the parameters of the method, the id of the method, and the handler of the method corresponding to the script
*/
class OUROBOROSPLUGINS_API Method
{
public:
	Method();
	virtual ~Method();

public:
	FString name;
	uint16 methodUtype;
	int16 aliasID;

	TArray<DATATYPE_BASE*> args;
};

}