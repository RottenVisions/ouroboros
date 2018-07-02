// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_ENTITYDEF_H
#define OURO_ENTITYDEF_H

#include "common/common.h"
#include "common/md5.h"
#if OURO_PLATFORM == PLATFORM_WIN32
#pragma warning (disable : 4910)
#pragma warning (disable : 4251)
#endif

#include "method.h"
#include "property.h"
#include "entity_call.h"
#include "math/math.h"
#include "pyscript/scriptobject.h"
#include "xml/xml.h"
#include "common/smartpointer.h"


namespace Ouroboros{

class ScriptDefModule;
typedef SmartPointer<ScriptDefModule> ScriptDefModulePtr;

class EntityDef
{
public:
	typedef std::vector<ScriptDefModulePtr> SCRIPT_MODULES;
	typedef std::map<std::string, ENTITY_SCRIPT_UID> SCRIPT_MODULE_UID_MAP;

	typedef std::tr1::function<PyObject* (COMPONENT_ID componentID, ENTITY_ID& eid)> GetEntityFunc;
	typedef std::tr1::function<Network::Channel* (EntityCall&)> FindChannelFunc;

	EntityDef();
	~EntityDef();

	/**
		initialization
	*/
	static bool initialize(std::vector<PyTypeObject*>& scriptBaseTypes,
		COMPONENT_TYPE loadComponentType);

	static bool finalise(bool isReload = false);

	static void reload(bool fullReload);

	/**
		Trying to find an instance of it by the ID of the entity
	*/
	static PyObject* tryGetEntity(COMPONENT_ID componentID, ENTITY_ID entityID);

	/**
		Set entityCall's __getEntityFunc function address
	*/
	static void setGetEntityFunc(GetEntityFunc func) {
		__getEntityFunc = func;
	};

	/**
		Loading related description
	*/
	static bool loadAllEntityScriptModules(std::string entitiesPath,
		std::vector<PyTypeObject*>& scriptBaseTypes);

	static bool loadAllComponentScriptModules(std::string entitiesPath,
		std::vector<PyTypeObject*>& scriptBaseTypes);

	static bool loadAllDefDescriptions(const std::string& moduleName,
		XML* defxml,
		TiXmlNode* defNode,
		ScriptDefModule* pScriptModule);

	static bool loadDefPropertys(const std::string& moduleName,
		XML* xml,
		TiXmlNode* defPropertyNode,
		ScriptDefModule* pScriptModule);

	static bool calcDefPropertyUType(const std::string& moduleName,
		const std::string& name, int iUtype, ScriptDefModule* pScriptModule, ENTITY_PROPERTY_UID& outUtype);

	static bool loadDefCellMethods(const std::string& moduleName,
		XML* xml,
		TiXmlNode* defMethodNode,
		ScriptDefModule* pScriptModule);

	static bool loadDefBaseMethods(const std::string& moduleName,
		XML* xml,
		TiXmlNode* defMethodNode,
		ScriptDefModule* pScriptModule);

	static bool loadDefClientMethods(const std::string& moduleName,
		XML* xml,
		TiXmlNode* defMethodNode,
		ScriptDefModule* pScriptModule);

	static bool loadInterfaces(const std::string& defFilePath,
		const std::string& moduleName,
		XML* defxml,
		TiXmlNode* defNode,
		ScriptDefModule* pScriptModule, bool ignoreComponents = false);

	static bool loadComponents(const std::string& defFilePath,
		const std::string& moduleName,
		XML* defxml,
		TiXmlNode* defNode,
		ScriptDefModule* pScriptModule);

	static PropertyDescription* addComponentProperty(ENTITY_PROPERTY_UID utype,
		const std::string& componentTypeName,
		const std::string& componentName,
		uint32 flags,
		bool isPersistent,
		bool isIdentifier,
		std::string indexType,
		uint32 databaseLength,
		const std::string& defaultStr,
		DETAIL_TYPE detailLevel,
		ScriptDefModule* pScriptModule,
		ScriptDefModule* pCompScriptDefModule);

	static bool loadParentClass(const std::string& defFilePath,
		const std::string& moduleName,
		XML* defxml,
		TiXmlNode* defNode,
		ScriptDefModule* pScriptModule);

	static bool loadDefInfo(const std::string& defFilePath,
		const std::string& moduleName,
		XML* defxml,
		TiXmlNode* defNode,
		ScriptDefModule* pScriptModule);

	static bool loadDetailLevelInfo(const std::string& defFilePath,
		const std::string& moduleName,
		XML* defxml,
		TiXmlNode* defNode,
		ScriptDefModule* pScriptModule);

	static bool loadVolatileInfo(const std::string& defFilePath,
		const std::string& moduleName,
		XML* defxml,
		TiXmlNode* defNode,
		ScriptDefModule* pScriptModule);

	static PyObject* loadScriptModule(std::string moduleName);

	/**
		Whether to load this script module
	*/
	static bool isLoadScriptModule(ScriptDefModule* pScriptModule);

	/**
		Set whether there is cell or base according to the current component category
	*/
	static void setScriptModuleHasComponentEntity(ScriptDefModule* pScriptModule, bool has);

	/**
		Check if the method defined in the script module exists
	*/
	static bool checkDefMethod(ScriptDefModule* pScriptModule, PyObject* moduleObj,
		const std::string& moduleName);

	/**
		Check whether the defined properties in the script module are legal
	*/
	static bool validDefPropertyName(ScriptDefModule* pScriptModule, const std::string& name);

	/**
		Finding the corresponding script module object by tag
	*/
	static ScriptDefModule* findScriptModule(ENTITY_SCRIPT_UID utype, bool notFoundOutErr = true);
	static ScriptDefModule* findScriptModule(const char* scriptName, bool notFoundOutErr = true);
	static ScriptDefModule* findOldScriptModule(const char* scriptName, bool notFoundOutErr = true);

	static bool installScript(PyObject* mod);
	static bool uninstallScript();

	static const SCRIPT_MODULES& getScriptModules() {
		return EntityDef::__scriptModules;
	}

	static OURO_MD5& md5(){ return __md5; }

	static bool initializeWatcher();

	static void entitydefAliasID(bool v)
	{
		__entitydefAliasID = v;
	}

	static bool entitydefAliasID()
	{
		return __entitydefAliasID;
	}

	static void entityAliasID(bool v)
	{
		__entityAliasID = v;
	}

	static bool entityAliasID()
	{
		return __entityAliasID;
	}

	static bool scriptModuleAliasID()
	{
		return __entitydefAliasID && __scriptModules.size() <= 255;
	}

	struct Context
	{
		Context()
		{
			currEntityID = 0;
			currClientappID = 0;
			currComponentType = UNKNOWN_COMPONENT_TYPE;
		}

		ENTITY_ID currEntityID;
		COMPONENT_TYPE currComponentType;
		int32 currClientappID;
	};

	static Context& context() {
		return __context;
	}

private:
	static SCRIPT_MODULES __scriptModules;										// All extension script modules are stored here
	static SCRIPT_MODULES __oldScriptModules;									// The old module will be put here for judgment when reload

	static SCRIPT_MODULE_UID_MAP __scriptTypeMappingUType;						// Script category mapping utype
	static SCRIPT_MODULE_UID_MAP __oldScriptTypeMappingUType;					// Old script class mapping utype when reload

	static COMPONENT_TYPE __loadComponentType;									// Relational component data for the desired relationship
	static std::vector<PyTypeObject*> __scriptBaseTypes;
	static std::string __entitiesPath;

	static OURO_MD5 __md5;														// defs-md5

	static bool _isInit;

	static bool __entityAliasID;												// Optimize EntityID, less than 255 EntityIDs in view scope, 1 byte pseudo ID when transferring to client
	static bool __entitydefAliasID;												// Optimize the bandwidth and bandwidth occupied by the entity attribute and method. When the entity client attribute or the client does not exceed 255, the method uid and the attribute uid are transmitted to the client.Use 1-byte alias ID

	static GetEntityFunc __getEntityFunc;										// Get an entity's function address

	// Set some context for the current operation
	static Context __context;
};

}

#ifdef CODE_INLINE
#include "entitydef.inl"
#endif
#endif // OURO_ENTITYDEF_H
