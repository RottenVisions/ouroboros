// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "OUROCommon.h"


namespace Ouroboros
{

class Property;
class Method;
class Entity;

/*
	a description class for a script module defined in an entitydef
	Contains the attributes and methods defined by an entity and the name and module ID of the entity script module
*/
class OUROBOROSPLUGINS_API ScriptModule
{
public:
	ScriptModule(const FString& moduleName, int type);
	virtual ~ScriptModule();

	Entity* createEntity();

public:
	FString name;
	bool usePropertyDescrAlias;
	bool useMethodDescrAlias;

	TMap<FString, Property*> propertys;
	TMap<uint16, Property*> idpropertys;

	TMap<FString, Method*> methods;
	TMap<FString, Method*> base_methods;
	TMap<FString, Method*> cell_methods;

	TMap<uint16, Method*> idmethods;
	TMap<uint16, Method*> idbase_methods;
	TMap<uint16, Method*> idcell_methods;

	uint16 utype;
};

}
