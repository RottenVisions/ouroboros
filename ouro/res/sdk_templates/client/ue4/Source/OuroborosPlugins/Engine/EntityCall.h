// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "OUROCommon.h"

/*
	EntityCall of the entity
	For EntityCall, please refer to the description in the API manual.
	https://github.com/ouroboros/ouroboros/tree/master/docs/api
*/
namespace Ouroboros
{

class Bundle;

class OUROBOROSPLUGINS_API EntityCall
{
public:
	EntityCall(int32 eid, const FString& ename);
	virtual ~EntityCall();

	enum ENTITYCALL_TYPE
	{
		ENTITYCALL_TYPE_CELL = 0,		// CELL_ENTITYCALL
		ENTITYCALL_TYPE_BASE = 1		// BASE_ENTITYCALL
	};

public:
	bool isBase()
	{
		return type == ENTITYCALL_TYPE_BASE;
	}

	bool isCell()
	{
		return type == ENTITYCALL_TYPE_CELL;
	}

	/*
		Create a new call
	*/
	Bundle* newCall();
	Bundle* newCall(const FString& methodName, uint16 entitycomponentPropertyID = 0);

	/*
		Send this call to the server
	*/
	void sendCall(Bundle* inBundle);

public:
	ENTITY_ID id;
	FString className;
	ENTITYCALL_TYPE type;
	Bundle* pBundle;
};

}