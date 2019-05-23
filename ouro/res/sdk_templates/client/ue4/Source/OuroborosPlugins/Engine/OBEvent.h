// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Object.h"
#include "OUROCommon.h"
#include "OBEventTypes.h"
#include "OBEvent.generated.h"


/*
Event module
The data foundation class of the event, different events need to implement different data classes
*/
UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData : public UObject
{
	GENERATED_BODY()

public:
	// The name of the event, which can be used to determine the type of event, which is populated by the event system when the event is triggered.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString eventName;
};

/*
	Event module
	The Ouroboros plugin layer interacts with the Unity3D presentation layer through events
*/
class OUROBOROSPLUGINS_API OBEvent
{
public:
	OBEvent();
	virtual ~OBEvent();
	
public:
		static bool registerEvent(const FString& eventName, const FString& funcName, TFunction<void(const UOBEventData*)> func, void* objPtr);
		static bool deregister(void* objPtr, const FString& eventName, const FString& funcName);
		static bool deregister(void* objPtr);

		static void fire(const FString& eventName, UOBEventData* pEventData);

		static void clear();
		static void clearFiredEvents();

		static void processInEvents() {}
		static void processOutEvents() {}

		static void pause();
		static void resume();

		static void removeFiredEvent(void* objPtr, const FString& eventName = TEXT(""), const FString& funcName = TEXT(""));

protected:
	struct EventObj
	{
		TFunction<void(const UOBEventData*)> method;
		FString funcName;
		void* objPtr;
	};

	struct FiredEvent
	{
		EventObj evt;
		FString eventName;
		UOBEventData* args;
	};

	static TMap<FString, TArray<EventObj>> events_;
	static TArray<FiredEvent*> firedEvents_;
	static bool isPause_;
};


// Registration issue
#define OUROBOROS_REGISTER_EVENT(EVENT_NAME, EVENT_FUNC) \
	OBEvent::registerEvent(EVENT_NAME, #EVENT_FUNC, [this](const UOBEventData* pEventData) {	EVENT_FUNC(pEventData);	}, (void*)this);

// register event, rewrite event function
#define OUROBOROS_REGISTER_EVENT_OVERRIDE_FUNC(EVENT_NAME, FUNC_NAME, EVENT_FUNC) \
	OBEvent::registerEvent(EVENT_NAME, FUNC_NAME, EVENT_FUNC, (void*)this);

// Unregister an event of this object
#define OUROBOROS_DEREGISTER_EVENT_BY_FUNCNAME(EVENT_NAME, FUNC_NAME) OBEvent::deregister((void*)this, EVENT_NAME, FUNC_NAME);
#define OUROBOROS_DEREGISTER_EVENT(EVENT_NAME) OBEvent::deregister((void*)this, EVENT_NAME, TEXT(""));

// Unregister all events of this object
#define OUROBOROS_DEREGISTER_ALL_EVENT()	OBEvent::deregister((void*)this);

// fire event
#define OUROBOROS_EVENT_FIRE(EVENT_NAME, EVENT_DATA) OBEvent::fire(EVENT_NAME, EVENT_DATA);

// pause event
#define OUROBOROS_EVENT_PAUSE() OBEvent::pause();

// recovery event
#define OUROBOROS_EVENT_RESUME() OBEvent::resume();

// clear all events
#define OUROBOROS_EVENT_CLEAR() OBEvent::clear();

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_Baseapp_importClientMessages : public UOBEventData
{
	GENERATED_BODY()

public:
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_onKicked : public UOBEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int32 failedcode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString errorStr;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_createAccount : public UOBEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString username;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString password;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	TArray<uint8> datas;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_login : public UOBEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString username;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString password;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	TArray<uint8> datas;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_logout : public UOBEventData
{
	GENERATED_BODY()

public:
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_onLoginFailed : public UOBEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int32 failedcode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString errorStr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	TArray<uint8> serverdatas;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_onLoginBaseapp : public UOBEventData
{
	GENERATED_BODY()

public:
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_onLoginSuccessfully : public UOBEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, /*BlueprintReadWrite, No support*/ Category = Ouroboros)
	uint64  entity_uuid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int32 entity_id;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_onReloginBaseapp : public UOBEventData
{
	GENERATED_BODY()

public:
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_onLoginBaseappFailed : public UOBEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int32 failedcode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString errorStr;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_onReloginBaseappFailed : public UOBEventData
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int32 failedcode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString errorStr;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_onReloginBaseappSuccessfully : public UOBEventData
{
	GENERATED_BODY()

public:
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_onVersionNotMatch : public UOBEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString clientVersion;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString serverVersion;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_onScriptVersionNotMatch : public UOBEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString clientScriptVersion;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString serverScriptVersion;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_Loginapp_importClientMessages : public UOBEventData
{
	GENERATED_BODY()

public:
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_Baseapp_importClientEntityDef : public UOBEventData
{
	GENERATED_BODY()

public:
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_onControlled : public UOBEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int entityID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	bool isControlled;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_onLoseControlledEntity : public UOBEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int entityID;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_updatePosition : public UOBEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FVector position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FRotator direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int entityID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	float moveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	bool isOnGround;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_set_position : public UOBEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FVector position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int entityID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	float moveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	bool isOnGround;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_set_direction : public UOBEventData
{
	GENERATED_BODY()

public:
	// roll, pitch, yaw
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FRotator direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int entityID;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_onCreateAccountResult : public UOBEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(Category = Ouroboros, BlueprintReadWrite, EditAnywhere)
	int errorCode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString errorStr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	TArray<uint8> datas;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_addSpaceGeometryMapping : public UOBEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString spaceResPath;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_onSetSpaceData : public UOBEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int spaceID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString value;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_onDelSpaceData : public UOBEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int spaceID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString key;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_onDisconnected : public UOBEventData
{
	GENERATED_BODY()

public:
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_onConnectionState : public UOBEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	bool success;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString address;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_onEnterWorld : public UOBEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString entityClassName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int spaceID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int entityID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString res;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FVector position;

	// roll, pitch, yaw
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FVector direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	float moveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	bool isOnGround;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	bool isPlayer;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_onLeaveWorld : public UOBEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int spaceID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int entityID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	bool isPlayer;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_onEnterSpace : public UOBEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString entityClassName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int spaceID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int entityID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString res;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FVector position;

	// roll, pitch, yaw
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FVector direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	float moveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	bool isOnGround;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	bool isPlayer;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_onLeaveSpace : public UOBEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int spaceID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int entityID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	bool isPlayer;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_resetPassword : public UOBEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString username;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_bindAccountEmail : public UOBEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString email;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_newPassword : public UOBEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString old_password;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString new_password;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_onStreamDataStarted : public UOBEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int resID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int dataSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString dataDescr;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_onStreamDataRecv : public UOBEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int resID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	TArray<uint8> data;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_onStreamDataCompleted : public UOBEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int resID;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_onImportClientSDK : public UOBEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int remainingFiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int fileSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString fileName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	TArray<uint8> fileDatas;
};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_onImportClientSDKSuccessfully : public UOBEventData
{
	GENERATED_BODY()

public:

};

UCLASS(Blueprintable, BlueprintType)
class OUROBOROSPLUGINS_API UOBEventData_onDownloadSDK : public UOBEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	bool isDownload;
};

