// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OUROBOROS_DEF_COMMON_H
#define OUROBOROS_DEF_COMMON_H

#include "common/common.h"
#if OURO_PLATFORM == PLATFORM_WIN32
#pragma warning (disable : 4910)
#pragma warning (disable : 4251)
#endif


namespace Ouroboros{

/** entity data transfer property tag*/
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

std::string entityDataFlagsToString(uint32 flags);
EntityDataFlags stringToEntityDataFlags(const std::string& strFlags);

#define ED_FLAG_ALL  ED_FLAG_CELL_PUBLIC | ED_FLAG_CELL_PRIVATE | ED_FLAG_ALL_CLIENTS \
	| ED_FLAG_CELL_PUBLIC_AND_OWN | ED_FLAG_OWN_CLIENT |	\
	ED_FLAG_BASE_AND_CLIENT | ED_FLAG_BASE | ED_FLAG_OTHER_CLIENTS

/** Equivalent to a general definition of the entity data transfer category*/
enum EntityDataFlagRelation
{
	// all flags that are related to baseapp
	ENTITY_BASE_DATA_FLAGS											= ED_FLAG_BASE | ED_FLAG_BASE_AND_CLIENT,
	// all flags that are related to cellapp
	ENTITY_CELL_DATA_FLAGS											= ED_FLAG_CELL_PUBLIC | ED_FLAG_CELL_PRIVATE | ED_FLAG_ALL_CLIENTS | ED_FLAG_CELL_PUBLIC_AND_OWN | ED_FLAG_OTHER_CLIENTS | ED_FLAG_OWN_CLIENT,
	// all flags that are related to the client
	ENTITY_CLIENT_DATA_FLAGS										= ED_FLAG_BASE_AND_CLIENT | ED_FLAG_ALL_CLIENTS | ED_FLAG_CELL_PUBLIC_AND_OWN | ED_FLAG_OTHER_CLIENTS | ED_FLAG_OWN_CLIENT,
	// all flags that need to be broadcast to other cellapps
	ENTITY_BROADCAST_CELL_FLAGS										= ED_FLAG_CELL_PUBLIC | ED_FLAG_ALL_CLIENTS | ED_FLAG_CELL_PUBLIC_AND_OWN | ED_FLAG_OTHER_CLIENTS,
	// all flags that need to be broadcast to other clients (not including their own)
	ENTITY_BROADCAST_OTHER_CLIENT_FLAGS								= ED_FLAG_OTHER_CLIENTS | ED_FLAG_ALL_CLIENTS,
	// all flags that need to be broadcast to their clients
	ENTITY_BROADCAST_OWN_CLIENT_FLAGS								= ED_FLAG_ALL_CLIENTS | ED_FLAG_CELL_PUBLIC_AND_OWN | ED_FLAG_OWN_CLIENT | ED_FLAG_BASE_AND_CLIENT,
	// all baseapp and client related flags
	ENTITY_BASEAPP_ANDA_CLIENT_DATA_FLAGS							= ED_FLAG_BASE_AND_CLIENT,
	// all cellapp and client related flags
	ENTITY_CELLAPP_ANDA_CLIENT_DATA_FLAGS							= ED_FLAG_ALL_CLIENTS | ED_FLAG_CELL_PUBLIC_AND_OWN | ED_FLAG_OTHER_CLIENTS | ED_FLAG_OWN_CLIENT,
};

/** The component class mapping corresponding to the entityCall class, the index of this table strictly matches the value of ENTITYCALL_TYPE*/
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

/** Definition of attribute lod broadcast level range*/
typedef uint8 DETAIL_TYPE;
#define DETAIL_LEVEL_NEAR 0 // lod level: near
#define DETAIL_LEVEL_MEDIUM 1 // lod level: medium
#define DETAIL_LEVEL_FAR 2 // lod level: far

typedef std::map<std::string, EntityDataFlags> ENTITYFLAGMAP;
extern ENTITYFLAGMAP g_entityFlagMapping; // entity flag string mapping table

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

// number some system-level variable attributes of the entity to identify when transmitting over the network
enum ENTITY_BASE_PROPERTY_UTYPE
{
	ENTITY_BASE_PROPERTY_UTYPE_POSITION_XYZ					= 1,
	ENTITY_BASE_PROPERTY_UTYPE_DIRECTION_ROLL_PITCH_YAW		= 2,
	ENTITY_BASE_PROPERTY_UTYPE_SPACEID						= 3,
};

// number some system-level variable attributes of the entity to identify when transmitting over the network
enum ENTITY_BASE_PROPERTY_ALIASID
{
	ENTITY_BASE_PROPERTY_ALIASID_NULL						= 0,
	ENTITY_BASE_PROPERTY_ALIASID_POSITION_XYZ				= 1,
	ENTITY_BASE_PROPERTY_ALIASID_DIRECTION_ROLL_PITCH_YAW	= 2,
	ENTITY_BASE_PROPERTY_ALIASID_SPACEID					= 3,
	ENTITY_BASE_PROPERTY_ALIASID_MAX						= 4,
};

// restricted system property, not allowed in def
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

// Get the python environment directory of the process
std::pair<std::wstring, std::wstring> getComponentPythonPaths(COMPONENT_TYPE componentType);

}
#endif // OUROBOROS_DEF_COMMON_H

