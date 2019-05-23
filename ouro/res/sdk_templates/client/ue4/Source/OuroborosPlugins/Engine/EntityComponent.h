// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "OUROCommon.h"

namespace Ouroboros
{

class Entity;
class MemoryStream;
class ScriptModule;

class OUROBOROSPLUGINS_API EntityComponent
{
public:
	EntityComponent();
	virtual ~EntityComponent();

public:
	virtual void onAttached(Entity* pOwnerEntity)
	{
	}
		
	virtual void onDetached(Entity* pOwnerEntity)
	{
	}

	virtual void onEnterworld()
	{
	}

	virtual void onLeaveworld()
	{
	}

	virtual ScriptModule* getScriptModule()
	{
		return NULL;
	}

	virtual void onRemoteMethodCall(uint16 methodUtype, MemoryStream& stream)
	{
		// Dynamic generation
	}

	virtual void onUpdatePropertys(uint16 propUtype, MemoryStream& stream, int maxCount)
	{
		// Dynamic generation
	}

	virtual void callPropertysSetMethods()
	{
		// Dynamic generation
	}

	virtual void createFromStream(MemoryStream& stream);

public:
	uint16 entityComponentPropertyID;
	uint16 componentType;
	ENTITY_ID ownerID;
	Entity* pOwner;
	FString name_;
};

}