// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_ENTITY_APP_H
#define OURO_ENTITY_APP_H

// common include
#include "pyscript/py_gc.h"
#include "pyscript/script.h"
#include "pyscript/pyprofile.h"
#include "pyscript/pyprofile_handler.h"
#include "common/common.h"
#include "common/timer.h"
#include "common/smartpointer.h"
#include "pyscript/pyobject_pointer.h"
#include "helper/debug_helper.h"
#include "helper/script_loglevel.h"
#include "helper/profile.h"
#include "server/serverconfig.h"
#include "network/message_handler.h"
#include "resmgr/resmgr.h"
#include "helper/console_helper.h"
#include "server/serverapp.h"
#include "server/script_timers.h"

#if OURO_PLATFORM == PLATFORM_WIN32
#pragma warning (disable : 4996)
#endif

	
namespace Ouroboros{

class PythonApp : public ServerApp
{
public:
	enum TimeOutType
	{
		TIMEOUT_GAME_TICK = TIMEOUT_SERVERAPP_MAX + 1,

		// This must be placed at the end, indicating the current maximum enumeration value is how much
		TIMEOUT_PYTHONAPP_MAX = TIMEOUT_GAME_TICK
	};

	PythonApp(Network::EventDispatcher& dispatcher,
		Network::NetworkInterface& ninterface, 
		COMPONENT_TYPE componentType,
		COMPONENT_ID componentID);

	~PythonApp();
	
	Ouroboros::script::Script& getScript(){ return script_; }
	PyObjectPtr getEntryScript(){ return entryScript_; }

	int registerPyObjectToScript(const char* attrName, PyObject* pyObj);
	int unregisterPyObjectToScript(const char* attrName);

	bool installPyScript();
	virtual bool installPyModules();
	virtual void onInstallPyModules() {};
	virtual bool uninstallPyModules();
	bool uninstallPyScript();

	virtual void finalise();
	virtual bool inInitialize();
	virtual bool initializeEnd();
	virtual void onShutdownBegin();
	virtual void onShutdownEnd();

	virtual void handleTimeout(TimerHandle, void * arg);

		/** Network Interface
		Request to execute a python instruction
	*/
	void onExecScriptCommand(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** 
		Console request start profile
	*/
	virtual void startProfile_(Network::Channel* pChannel, std::string profileName, int8 profileType, uint32 timelen);

	/**
		Get the app release status, which can be obtained in the script
	*/
	static PyObject* __py_getAppPublish(PyObject* self, PyObject* args);

	/**
		Set the script output type prefix
	*/
	static PyObject* __py_setScriptLogType(PyObject* self, PyObject* args);

	/**
		Re-import all scripts
	*/
	virtual void reloadScript(bool fullReload);
	virtual void onReloadScript(bool fullReload);

	/**
		Get the full path of the resource through the relative path
	*/
	static PyObject* __py_getResFullPath(PyObject* self, PyObject* args);

	/**
		Determine whether a resource exists through a relative path
	*/
	static PyObject* __py_hasRes(PyObject* self, PyObject* args);

	/**
		Open file
	*/
	static PyObject* __py_ouroOpen(PyObject* self, PyObject* args);

	/**
		List all files in the directory
	*/
	static PyObject* __py_listPathRes(PyObject* self, PyObject* args);

	/**
		Match the relative path to get the full path
	*/
	static PyObject* __py_matchPath(PyObject* self, PyObject* args);

	/** Timer operation
	*/
	static PyObject* __py_addTimer(PyObject* self, PyObject* args);
	static PyObject* __py_delTimer(PyObject* self, PyObject* args);

	static ScriptTimers &scriptTimers() { return scriptTimers_; }


protected:
	static ScriptTimers										scriptTimers_;

	TimerHandle												gameTickTimerHandle_;

	Ouroboros::script::Script								script_;

	PyObjectPtr												entryScript_;

};


}

#endif // OURO_ENTITY_APP_H
