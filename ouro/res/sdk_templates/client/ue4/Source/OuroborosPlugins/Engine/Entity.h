// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "KBVar.h"
#include "OUROCommon.h"

namespace Ouroboros
{

class Method;
class Property;
class EntityCall;
class MemoryStream;
class EntityComponent;

/*
	Entity base class of the Ouroboros logical layer
	All extended game entities should inherit from this module

	https://github.com/ouroboros/ouroboros/blob/master/ouro/res/sdk_templates/client/ue4/README.md
*/
class OUROBOROSPLUGINS_API Entity
{
public:
	Entity();
	virtual ~Entity();

	virtual void __init__();

	static void clear();

public:
	const FString& className() const {
		return className_;
	}

	void className(const FString& v) {
		className_ = v;
	}

	bool inWorld() const {
		return inWorld_;
	}

	void inWorld(bool v) {
		inWorld_ = v;
	}

	bool isControlled() const {
		return isControlled_;
	}

	void isControlled(bool v) {
		isControlled_ = v;
	}

	bool inited() const {
		return inited_;
	}

	void inited(bool v) {
		inited_ = v;
	}

	ENTITY_ID id() const {
		return id_;
	}

	void id(ENTITY_ID v) {
		id_ = v;
	}

	float velocity() const {
		return velocity_;
	}

	void velocity(float v) {
		velocity_ = v;
	}

	bool isPlayer();

	void isOnGround(bool v) {
		isOnGround_ = v;
	}

	bool isOnGround() const {
		return isOnGround_;
	}

public:
	void destroy()
	{
		detachComponents();
		onDestroy();
	}

	virtual void onDestroy()
	{
	}

	void enterWorld();
	virtual void onEnterWorld();
	void leaveWorld();
	virtual void onLeaveWorld();

	virtual void enterSpace();
	virtual void onEnterSpace();
	virtual void leaveSpace();
	virtual void onLeaveSpace();

	// This callback method is called when the local entity control by the client has been enabled or disabled. 
	// See the Entity.controlledBy() method in the CellApp server code for more infomation.
	// For the player itself, it indicates whether it is controlled by other players;
	// For other entities, indicates whether my machine controls this entity
	virtual void onControlled(bool isControlled)
	{

	}

	virtual void onUpdateVolatileData()
	{
	}

	virtual void onPositionChanged(const FVector& oldValue);
	virtual void onDirectionChanged(const FVector& oldValue);

	virtual EntityCall* getBaseEntityCall()
	{
		return NULL;
	}

	virtual EntityCall* getCellEntityCall()
	{
		return NULL;
	}

	virtual void onRemoteMethodCall(MemoryStream& stream)
	{
		// Dynamic generation
	}

	virtual void onUpdatePropertys(MemoryStream& stream)
	{
		// Dynamic generation
	}

	virtual void onGetBase()
	{
		// Dynamic generation
	}

	virtual void onGetCell()
	{
		// Dynamic generation
	}

	virtual void onLoseCell()
	{
		// Dynamic generation
	}

	virtual void onComponentsEnterworld()
	{
		// dynamically generated, notification component onEnterworld
	}

	virtual void onComponentsLeaveworld()
	{
		// dynamically generated, notification component onLeaveworld
	}

	virtual void callPropertysSetMethods()
	{
		// Dynamic generation
	}

	virtual void attachComponents()
	{
		// Dynamic generation
	}

	virtual void detachComponents()
	{
		// Dynamic generation
	}

	virtual TArray<EntityComponent*> getComponents(FString componentName, bool all)
	{
		TArray<EntityComponent*> founds;

		return founds;
	}

protected:
	ENTITY_ID id_;
	FString className_;

	bool isOnGround_;

	// set to true after enterworld
	bool inWorld_;

	// For the player itself, it indicates whether it is controlled by other players;
	// For other entities, indicates whether my machine controls this entity
	bool isControlled_;

	bool inited_;

	float velocity_;

public:
	FVector position;
	FVector direction;
	uint32 spaceID;

	// The position and orientation of the current player's last sync to the server
	// These two properties are for the engine Ouroboros.cs, don't modify it elsewhere.
	FVector entityLastLocalPos;
	FVector entityLastLocalDir;

	//EntityCall* baseEntityCall = null;
	//EntityCall* cellEntityCall = null;
};

}







