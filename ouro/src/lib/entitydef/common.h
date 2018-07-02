// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OUROBOROS_DEF_COMMON_H
#define OUROBOROS_DEF_COMMON_H

#include "common/common.h"
#if OURO_PLATFORM == PLATFORM_WIN32
#pragma warning (disable : 4910)
#pragma warning (disable : 4251)
#endif


namespace Ouroboros{

/** Entity's data transfer feature flag */
enum EntityDataFlags
{
	ED_FLAG_UNKOWN													= 0x00000000, // Undefined
	ED_FLAG_CELL_PUBLIC												= 0x00000001, // Related to all cell broadcasts
	ED_FLAG_CELL_PRIVATE											= 0x00000002, // Current cell
	ED_FLAG_ALL_CLIENTS												= 0x00000004, // Cell broadcast with all clients
	ED_FLAG_CELL_PUBLIC_AND_OWN										= 0x00000008, // Cell broadcasts with its own client
	ED_FLAG_OWN_CLIENT												= 0x00000010, // Current cell and client
	ED_FLAG_BASE_AND_CLIENT											= 0x00000020, // Base and client
	ED_FLAG_BASE													= 0x00000040, // Current base
	ED_FLAG_OTHER_CLIENTS											= 0x00000080, // Cell broadcasts and other clients
};

std::string entityDataFlagsToString(uint32 flags);

#define ED_FLAG_ALL  ED_FLAG_CELL_PUBLIC | ED_FLAG_CELL_PRIVATE | ED_FLAG_ALL_CLIENTS \
	| ED_FLAG_CELL_PUBLIC_AND_OWN | ED_FLAG_OWN_CLIENT |	\
	ED_FLAG_BASE_AND_CLIENT | ED_FLAG_BASE | ED_FLAG_OTHER_CLIENTS

/** Equivalent to an overall definition of the entity data transmission category */
enum EntityDataFlagRelation
{
	// All flags related to baseapp
	ENTITY_BASE_DATA_FLAGS											= ED_FLAG_BASE | ED_FLAG_BASE_AND_CLIENT,
	// All flags related to cellapp
	ENTITY_CELL_DATA_FLAGS											= ED_FLAG_CELL_PUBLIC | ED_FLAG_CELL_PRIVATE | ED_FLAG_ALL_CLIENTS | ED_FLAG_CELL_PUBLIC_AND_OWN | ED_FLAG_OTHER_CLIENTS | ED_FLAG_OWN_CLIENT,
	// All the flags related to the client
	ENTITY_CLIENT_DATA_FLAGS										= ED_FLAG_BASE_AND_CLIENT | ED_FLAG_ALL_CLIENTS | ED_FLAG_CELL_PUBLIC_AND_OWN | ED_FLAG_OTHER_CLIENTS | ED_FLAG_OWN_CLIENT,
	// All flags that need to be broadcast to other cellapps
	ENTITY_BROADCAST_CELL_FLAGS										= ED_FLAG_CELL_PUBLIC | ED_FLAG_ALL_CLIENTS | ED_FLAG_CELL_PUBLIC_AND_OWN | ED_FLAG_OTHER_CLIENTS,
	// All flags that need to be broadcast to other clients (excluding their own)
	ENTITY_BROADCAST_OTHER_CLIENT_FLAGS								= ED_FLAG_OTHER_CLIENTS | ED_FLAG_ALL_CLIENTS,
	// All the flags that need to be broadcast to their own clients
	ENTITY_BROADCAST_OWN_CLIENT_FLAGS								= ED_FLAG_ALL_CLIENTS | ED_FLAG_CELL_PUBLIC_AND_OWN | ED_FLAG_OWN_CLIENT | ED_FLAG_BASE_AND_CLIENT,
	// All baseapp client related flags
	ENTITY_BASEAPP_ANDA_CLIENT_DATA_FLAGS							= ED_FLAG_BASE_AND_CLIENT,
	// All cellapp-related client flags
	ENTITY_CELLAPP_ANDA_CLIENT_DATA_FLAGS							= ED_FLAG_ALL_CLIENTS | ED_FLAG_CELL_PUBLIC_AND_OWN | ED_FLAG_OTHER_CLIENTS | ED_FLAG_OWN_CLIENT,
};

/** EntityCall class corresponding to the component category mapping, the index of this table exactly matches the value of ENTITYCALL_TYPE */
const COMPONENT_TYPE ENTITYCALL_COMPONENT_TYPE_MAPPING[] =
{
	CELLAPP_TYPE,
	BASEAPP_TYPE,
	CLIENT_TYPE,
	BASEAPP_TYPE,
	CELLAPP_TYPE,
	CELLAPP_TYPE,
	BASEAPP_TYPE,
};

/** Definition of lod broadcast level range of attributes */
typedef uint8 DETAIL_TYPE;
#define DETAIL_LEVEL_NEAR													0	// Lod level: near
#define DETAIL_LEVEL_MEDIUM													1	// Lod level: medium
#define DETAIL_LEVEL_FAR													2	// Lod level: far

typedef std::map<std::string, EntityDataFlags> ENTITYFLAGMAP;
extern ENTITYFLAGMAP g_entityFlagMapping;										// entity flag string mapping table

// UID category of attributes and methods
typedef uint16 ENTITY_PROPERTY_UID;
typedef uint16 ENTITY_METHOD_UID;
typedef uint16 ENTITY_SCRIPT_UID;
typedef uint16 ENTITY_COMPONENT_UID;
typedef uint16 DATATYPE_UID;
typedef uint8  DATATYPE;
typedef uint8  ENTITY_DEF_ALIASID;
typedef uint8  ENTITY_COMPONENT_ALIASID;

#define DATA_TYPE_UNKONWN				0
#define DATA_TYPE_FIXEDARRAY			1
#define DATA_TYPE_FIXEDDICT				2
#define DATA_TYPE_STRING				3
#define DATA_TYPE_DIGIT					4
#define DATA_TYPE_BLOB					5
#define DATA_TYPE_PYTHON				6
#define DATA_TYPE_VECTOR2				7
#define DATA_TYPE_VECTOR3				8
#define DATA_TYPE_VECTOR4				9
#define DATA_TYPE_UNICODE				10
#define DATA_TYPE_ENTITYCALL			11
#define DATA_TYPE_PYDICT				12
#define DATA_TYPE_PYTUPLE				13
#define DATA_TYPE_PYLIST				14
#define DATA_TYPE_ENTITY_COMPONENT		15

// Several system-level variable attributes of the entity are numbered so that they can be identified when the network is transmitting
enum ENTITY_BASE_PROPERTY_UTYPE
{
	ENTITY_BASE_PROPERTY_UTYPE_POSITION_XYZ					= 1,
	ENTITY_BASE_PROPERTY_UTYPE_DIRECTION_ROLL_PITCH_YAW		= 2,
	ENTITY_BASE_PROPERTY_UTYPE_SPACEID						= 3,
};

// Several system-level variable attributes of the entity are numbered so that they can be identified when the network is transmitting
enum ENTITY_BASE_PROPERTY_ALIASID
{
	ENTITY_BASE_PROPERTY_ALIASID_NULL						= 0,
	ENTITY_BASE_PROPERTY_ALIASID_POSITION_XYZ				= 1,
	ENTITY_BASE_PROPERTY_ALIASID_DIRECTION_ROLL_PITCH_YAW	= 2,
	ENTITY_BASE_PROPERTY_ALIASID_SPACEID					= 3,
	ENTITY_BASE_PROPERTY_ALIASID_MAX						= 4,
};

// System properties that are restricted, are not allowed in def
const char ENTITY_LIMITED_PROPERTYS[][34] =
{
	"id",
	"position",
	"direction",
	"spaceID",
	"autoLoad",
	"cell",
	"base",
	"client",
	"cellData",
	"className",
	"component"
	"databaseID",
	"isDestroyed",
	"shouldAutoArchive",
	"shouldAutoBackup",
	"__ACCOUNT_NAME__",
	"__ACCOUNT_PASSWORD__",
	"clientAddr",
	"clientEnabled",
	"hasClient",
	"roundTripTime",
	"timeSinceHeardFromClient",
	"allClients",
	"hasWitness",
	"isWitnessed",
	"otherClients",
	"topSpeed",
	"topSpeedY",
	"interface"
	"",
};

}
#endif // OUROBOROS_DEF_COMMON_H
