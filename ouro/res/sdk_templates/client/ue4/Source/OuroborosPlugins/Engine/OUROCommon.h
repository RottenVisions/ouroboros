// Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include "KBVar.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"
#include "KBECommon.generated.h"

namespace Ouroboros
{

DECLARE_LOG_CATEGORY_EXTERN(LogOuroboros, Log, All);

}
#define OURO_ASSERT check

typedef uint16 MessageID;
typedef uint16 MessageLength;
typedef uint32 MessageLengthEx;
typedef int32 ENTITY_ID;
typedef uint32 SPACE_ID;
typedef uint64 DBID;
typedef TArray<uint8> ByteArray;
typedef TMap<FString, Ouroboros::KBVar> KB_FIXED_DICT;
typedef TArray<Ouroboros::KBVar> KB_ARRAY;

#define OURO_FLT_MAX FLT_MAX

/** Securely release a pointer memory*/
#define OURO_SAFE_RELEASE(i)									\
	if (i)													\
		{													\
			delete i;										\
			i = NULL;										\
		}

/** Safely release a pointer array memory*/
#define OURO_SAFE_RELEASE_ARRAY(i)							\
	if (i)													\
		{													\
			delete[] i;										\
			i = NULL;										\
		}

USTRUCT(BlueprintType)
struct FKServerErr
{
	GENERATED_USTRUCT_BODY()

	FKServerErr()
	: name()
	, descr()
	, id(0)
	{
	}

	UPROPERTY(Category = ServerErr, BlueprintReadWrite, EditAnywhere)
	FString name;

	UPROPERTY(Category = ServerErr, BlueprintReadWrite, EditAnywhere)
	FString descr;

	UPROPERTY(Category = ServerErr, BlueprintReadWrite, EditAnywhere)
	int32 id;
};

// client category
// http://www.ouroboros.org/docs/programming/clientsdkprogramming.html
// http://www.ouroboros.org/cn/docs/programming/clientsdkprogramming.html
UENUM(BlueprintType)
enum class EKCLIENT_TYPE : uint8
{
	CLIENT_TYPE_UNKNOWN		UMETA(DisplayName = "unknown"),
	CLIENT_TYPE_MOBILE		UMETA(DisplayName = "Mobile"),
	CLIENT_TYPE_WIN			UMETA(DisplayName = "Windows"),
	CLIENT_TYPE_LINUX		UMETA(DisplayName = "Linux"),
	CLIENT_TYPE_MAC			UMETA(DisplayName = "Mac"),
	CLIENT_TYPE_BROWSER		UMETA(DisplayName = "Browser"),
	CLIENT_TYPE_BOTS		UMETA(DisplayName = "Bots"),
	CLIENT_TYPE_MINI		UMETA(DisplayName = "Mini"),
};

// Encrypted communication category
UENUM(BlueprintType)
enum class NETWORK_ENCRYPT_TYPE : uint8
{
	ENCRYPT_TYPE_NONE			UMETA(DisplayName = "None"),
	ENCRYPT_TYPE_BLOWFISH		UMETA(DisplayName = "Blowfish"),
};

// network message category
enum NETWORK_MESSAGE_TYPE
{
	NETWORK_MESSAGE_TYPE_COMPONENT = 0, // component message
		NETWORK_MESSAGE_TYPE_ENTITY = 1,		// entityæ¶ˆæ?¯
};

enum ProtocolType
{
	PROTOCOL_TCP = 0,
	PROTOCOL_UDP = 1,
};

enum EntityDataFlags
{
	ED_FLAG_UNKOWN = 0x00000000, // undefined
	ED_FLAG_CELL_PUBLIC = 0x00000001, // related all cell broadcasts
	ED_FLAG_CELL_PRIVATE = 0x00000002, // current cell
	ED_FLAG_ALL_CLIENTS = 0x00000004, // cell broadcasts with all clients
	ED_FLAG_CELL_PUBLIC_AND_OWN = 0x00000008, // cell broadcasts with its own client
	ED_FLAG_OWN_CLIENT = 0x00000010, // current cell and client
	ED_FLAG_BASE_AND_CLIENT = 0x00000020, // base and client
	ED_FLAG_BASE = 0x00000040, // current base
	ED_FLAG_OTHER_CLIENTS = 0x00000080, // cell broadcast and other clients
};

// Encrypt additional stored information to occupy bytes (length + padding)
#define ENCRYPTTION_WASTAGE_SIZE			(1 + 7)

#define PACKET_MAX_SIZE						1500
#ifndef PACKET_MAX_SIZE_TCP
#define PACKET_MAX_SIZE_TCP					1460
#endif
#define PACKET_MAX_SIZE_UDP					1472

typedef uint16								PacketLength;				// æœ€å¤§65535
#define PACKET_LENGTH_SIZE					sizeof(PacketLength)

#define NETWORK_MESSAGE_ID_SIZE				sizeof(MessageID)
#define NETWORK_MESSAGE_LENGTH_SIZE			sizeof(MessageLength)
#define NETWORK_MESSAGE_LENGTH1_SIZE		sizeof(MessageLengthEx)
#define NETWORK_MESSAGE_MAX_SIZE			65535
#define NETWORK_MESSAGE_MAX_SIZE1			4294967295

// Game content available package size
#define GAME_PACKET_MAX_SIZE_TCP			PACKET_MAX_SIZE_TCP - NETWORK_MESSAGE_ID_SIZE - \
											NETWORK_MESSAGE_LENGTH_SIZE - ENCRYPTTION_WASTAGE_SIZE

/*
	Network message type, fixed length or variable length.
	if you need a custom length, fill in the length when you declare it in NETWORK_INTERFACE_DECLARE_BEGIN.
*/
#ifndef NETWORK_FIXED_MESSAGE
#define NETWORK_FIXED_MESSAGE 0
#endif

#ifndef NETWORK_VARIABLE_MESSAGE
#define NETWORK_VARIABLE_MESSAGE -1
#endif

/*
	Network MTU maximum
*/

#define TCP_PACKET_MAX 1460

double getTimeSeconds();

inline float int82angle(int8 angle, bool half)
{
	float halfv = 128.f;
	if (half == true)
		halfv = 254.f;

	halfv = ((float)angle) * ((float)PI / halfv);
	return halfv;
}

inline bool almostEqual(float f1, float f2, float epsilon)
{
	return FMath::Abs(f1 - f2) < epsilon;
}

inline bool isNumeric(Ouroboros::KBVar& v)
{
	return v.GetType() == EKBVarTypes::Bool || 
		v.GetType() == EKBVarTypes::Double ||
		v.GetType() == EKBVarTypes::Float ||
		v.GetType() == EKBVarTypes::Int8 ||
		v.GetType() == EKBVarTypes::Int16 ||
		v.GetType() == EKBVarTypes::Int32 ||
		v.GetType() == EKBVarTypes::Int64 ||
		v.GetType() == EKBVarTypes::UInt8 ||
		v.GetType() == EKBVarTypes::UInt16 ||
		v.GetType() == EKBVarTypes::UInt32 ||
		v.GetType() == EKBVarTypes::UInt64;
}

// UE4's scale unit is converted to meters
#define UE4_SCALE_UNIT_TO_METER 100.f

// Convert the position (Vector3) of the KBE coordinate system to the position of the UE4 coordinate system
inline void KBPos2UE4Pos(FVector& UE4_POSITION, const FVector& OURO_POSITION)
{	
	// UE4 coordinates are in centimeters, KBE is in meters, so conversions require constants
	UE4_POSITION.Y = OURO_POSITION.X * UE4_SCALE_UNIT_TO_METER;
	UE4_POSITION.Z = OURO_POSITION.Y * UE4_SCALE_UNIT_TO_METER;
	UE4_POSITION.X = OURO_POSITION.Z * UE4_SCALE_UNIT_TO_METER;
}	

// Convert the position (Vector3) of the UE4 coordinate system to the position of the KBE coordinate system
inline void UE4Pos2KBPos(FVector& OURO_POSITION, const FVector& UE4_POSITION)
{
	// UE4 coordinates are in centimeters, KBE is in meters, so conversions require constants
	OURO_POSITION.X = UE4_POSITION.Y / UE4_SCALE_UNIT_TO_METER;
	OURO_POSITION.Y = UE4_POSITION.Z / UE4_SCALE_UNIT_TO_METER;
	OURO_POSITION.Z = UE4_POSITION.X / UE4_SCALE_UNIT_TO_METER;
}

// Convert KBE direction to UE4 direction
inline void KBDir2UE4Dir(FRotator& UE4_DIRECTION, const FVector& OURO_DIRECTION)
{
	UE4_DIRECTION.Pitch = FMath::RadiansToDegrees<float>(OURO_DIRECTION.Y);
	UE4_DIRECTION.Yaw = FMath::RadiansToDegrees<float>(OURO_DIRECTION.Z);
	UE4_DIRECTION.Roll = FMath::RadiansToDegrees<float>(OURO_DIRECTION.X);
}

// Convert UE4 direction to KBE direction
inline void UE4Dir2KBDir(FVector& OURO_DIRECTION, const FRotator& UE4_DIRECTION)
{
	OURO_DIRECTION.Y = FMath::DegreesToRadians<float>(UE4_DIRECTION.Pitch);
	OURO_DIRECTION.Z = FMath::DegreesToRadians<float>(UE4_DIRECTION.Yaw);
	OURO_DIRECTION.X = FMath::DegreesToRadians<float>(UE4_DIRECTION.Roll);
}

UCLASS()
class OUROBOROSPLUGINS_API AKBECommon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKBECommon();
	
};


