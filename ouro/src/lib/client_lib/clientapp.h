// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_CLIENT_APP_H
#define OURO_CLIENT_APP_H

#include "clientobjectbase.h"
#include "common/common.h"
#include "helper/debug_helper.h"
#include "helper/script_loglevel.h"
#include "xml/xml.h"	
#include "common/singleton.h"
#include "common/smartpointer.h"
#include "common/timer.h"
#include "network/interfaces.h"
#include "network/encryption_filter.h"
#include "network/event_dispatcher.h"
#include "network/network_interface.h"
#include "thread/threadpool.h"
#include "pyscript/script.h"
#include "resmgr/resmgr.h"
	
namespace Ouroboros{

namespace Network
{
class Channel;
class TCPPacketSender;
class TCPPacketReceiver;
}

class ClientApp : 
	public Singleton<ClientApp>,
	public ClientObjectBase,
	public TimerHandler, 
	public Network::ChannelTimeOutHandler,
	public Network::ChannelDeregisterHandler
{
public:
	enum TimeOutType
	{
		TIMEOUT_GAME_TICK
	};

	enum C_STATE
	{
		C_STATE_INIT = 0,
		C_STATE_INITLOGINAPP_CHANNEL = 1,
		C_STATE_LOGIN = 2,
		C_STATE_LOGIN_BASEAPP_CHANNEL = 3,
		C_STATE_LOGIN_BASEAPP = 4,
		C_STATE_PLAY = 5,
	};
public:
	ClientApp(Network::EventDispatcher& dispatcher, 
			Network::NetworkInterface& ninterface, 
			COMPONENT_TYPE componentType,
			COMPONENT_ID componentID);

	~ClientApp();

	virtual bool initialize();
	virtual bool initializeBegin();
	virtual bool inInitialize(){ return true; }
	virtual bool initializeEnd();
	virtual void finalise();
	virtual bool run();
	
	virtual void reset(void);

	virtual int processOnce(bool shouldIdle = false);

	virtual bool installPyModules();
	virtual void onInstallPyModules(){};
	virtual bool uninstallPyModules();
	virtual bool uninstallPyScript();
	virtual bool installEntityDef();

	void registerScript(PyTypeObject*);
	int registerPyObjectToScript(const char* attrName, PyObject* pyObj);
	int unregisterPyObjectToScript(const char* attrName);

	PyObjectPtr getEntryScript(){ return entryScript_; }

	const char* name(){return COMPONENT_NAME_EX(componentType_);}
	
	virtual void handleTimeout(TimerHandle, void * pUser);
	virtual void handleGameTick();

	void shutDown();

	bool createAccount(std::string accountName, std::string passwd, std::string datas,
		std::string ip, Ouroboros::uint32 port);

	bool login(std::string accountName, std::string passwd, std::string datas,
		std::string ip, Ouroboros::uint32 port);

	bool updateChannel(bool loginapp, std::string accountName, std::string passwd, 
								   std::string ip, Ouroboros::uint32 port);

	GAME_TIME time() const { return g_ourotime; }
	double gameTimeInSeconds() const;

	Network::EventDispatcher & dispatcher()				{ return dispatcher_; }
	Network::NetworkInterface & networkInterface()		{ return networkInterface_; }

	COMPONENT_ID componentID() const	{ return componentID_; }
	COMPONENT_TYPE componentType() const	{ return componentType_; }
		
	Ouroboros::script::Script& getScript(){ return *pScript_; }
	void setScript(Ouroboros::script::Script* p){ pScript_ = p; }

	static PyObject* __py_getAppPublish(PyObject* self, PyObject* args);
	static PyObject* __py_getPlayer(PyObject* self, PyObject* args);
	static PyObject* __py_fireEvent(PyObject* self, PyObject* args);

	virtual void onServerClosed();

	/**
		Set the script output type prefix
	*/
	static PyObject* __py_setScriptLogType(PyObject* self, PyObject* args);

	virtual void onChannelTimeOut(Network::Channel * pChannel);
	virtual void onChannelDeregister(Network::Channel * pChannel);

	virtual void onHelloCB_(Network::Channel* pChannel, const std::string& verInfo,
		const std::string& scriptVerInfo, const std::string& protocolMD5, 
		const std::string& entityDefMD5, COMPONENT_TYPE componentType);

		/** Network Interface
		Does not match the version of the server
	*/
	virtual void onVersionNotMatch(Network::Channel* pChannel, MemoryStream& s);

		/** Network Interface
		Does not match the script layer version of the server
	*/
	virtual void onScriptVersionNotMatch(Network::Channel* pChannel, MemoryStream& s);

		/** Network Interface
	   	   login successful
	   @ip: Server ip address
	   @port: server port
	*/
	virtual void onLoginSuccessfully(Network::Channel * pChannel, MemoryStream& s);

		/** Network Interface
	   Login failure callback
	   @failedcode: Failure return code NETWORK_ERR_SRV_NO_READY: The server is not ready,
									NETWORK_ERR_SRV_OVERLOAD: The server is overloaded,
									NETWORK_ERR_NAME_PASSWORD: Username or password is incorrect
	*/
	virtual void onLoginFailed(Network::Channel * pChannel, MemoryStream& s);

		/** Network Interface
	   Login failure callback
	   @failedcode: Failure return code NETWORK_ERR_SRV_NO_READY: The server is not ready,
									NETWORK_ERR_ILLEGAL_LOGIN: illegal login,
									NETWORK_ERR_NAME_PASSWORD: Username or password is incorrect
	*/
	virtual void onLoginBaseappFailed(Network::Channel * pChannel, SERVER_ERROR_CODE failedcode);
	virtual void onReloginBaseappFailed(Network::Channel * pChannel, SERVER_ERROR_CODE failedcode);

		/** Network Interface
	   Re-login to baseapp successfully
	*/
	virtual void onReloginBaseappSuccessfully(Network::Channel * pChannel, MemoryStream& s);

	virtual void onTargetChanged();

	/** 
		The server adds a geometric map of a space
	*/
	virtual void onAddSpaceGeometryMapping(SPACE_ID spaceID, std::string& respath);

	static PyObject* __py_GetSpaceData(PyObject *self, PyObject* args)
	{
		return ClientObjectBase::__py_GetSpaceData(&ClientApp::getSingleton(), args);	
	}

	static PyObject* __py_callback(PyObject *self, PyObject* args)
	{
		return ClientObjectBase::__py_callback(&ClientApp::getSingleton(), args);	
	}

	static PyObject* __py_cancelCallback(PyObject *self, PyObject* args)
	{
		return ClientObjectBase::__py_cancelCallback(&ClientApp::getSingleton(), args);	
	}

	static PyObject* __py_getWatcher(PyObject *self, PyObject* args)
	{
		return ClientObjectBase::__py_getWatcher(&ClientApp::getSingleton(), args);	
	}

	static PyObject* __py_getWatcherDir(PyObject *self, PyObject* args)
	{
		return ClientObjectBase::__py_getWatcherDir(&ClientApp::getSingleton(), args);	
	}

	static PyObject* __py_disconnect(PyObject *self, PyObject* args)
	{
		return ClientObjectBase::__py_disconnect(&ClientApp::getSingleton(), args);	
	}

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

protected:
	Ouroboros::script::Script*								pScript_;
	std::vector<PyTypeObject*>								scriptBaseTypes_;

	TimerHandle												gameTimer_;

	COMPONENT_TYPE											componentType_;

	// the ID of this component
	COMPONENT_ID											componentID_;									

	Network::EventDispatcher& 								dispatcher_;
	Network::NetworkInterface&								networkInterface_;
	
	Network::TCPPacketSender*								pTCPPacketSender_;
	Network::TCPPacketReceiver*								pTCPPacketReceiver_;
	Network::BlowfishFilter*								pBlowfishFilter_;

		// Thread Pool
	thread::ThreadPool										threadPool_;

	PyObjectPtr												entryScript_;

	C_STATE													state_;
};

}
#endif
