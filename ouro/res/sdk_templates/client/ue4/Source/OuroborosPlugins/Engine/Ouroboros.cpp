
#include "Ouroboros.h"
#include "OuroborosArgs.h"
#include "Entity.h"
#include "EntityDef.h"
#include "Messages.h"
#include "NetworkInterfaceTcp.h"
#include "NetworkInterfaceKcp.h"
#include "Bundle.h"
#include "MemoryStream.h"
#include "DataTypes.h"
#include "ScriptModule.h"
#include "Property.h"
#include "Method.h"
#include "EntityCall.h"
#include "Regex.h"
#include "KBDebug.h"
#include "OBEvent.h"
#include "EncryptionFilter.h"

namespace Ouroboros
{

ServerErrorDescrs OuroborosApp::serverErrs_;

OuroborosApp::OuroborosApp() :
	pArgs_(NULL),
	pNetworkInterface_(NULL),
	username_(TEXT("")),
	password_(TEXT("")),
	baseappIP_(TEXT("")),
	baseappTcpPort_(0),
	baseappUdpPort_(0),
	currserver_(TEXT("")),
	currstate_(TEXT("")),
	serverdatas_(),
	clientdatas_(),
	encryptedKey_(),
	serverVersion_(TEXT("")),
	clientVersion_(TEXT("")),
	serverScriptVersion_(TEXT("")),
	clientScriptVersion_(TEXT("")),
	serverProtocolMD5_(TEXT("@{OURO_SERVER_PROTO_MD5}")),
	serverEntitydefMD5_(TEXT("@{OURO_SERVER_ENTITYDEF_MD5}")),
	entity_uuid_(0),
	entity_id_(0),
	entity_type_(TEXT("")),
	useAliasEntityID_(@{OURO_USE_ALIAS_ENTITYID}),
	controlledEntities_(),
	entityServerPos_(),
	spacedatas_(),
	entities_(),
	entityIDAliasIDList_(),
	bufferedCreateEntityMessages_(),
	lastTickTime_(0.0),
	lastTickCBTime_(0.0),
	lastUpdateToServerTime_(0.0),
	spaceID_(0),
	spaceResPath_(TEXT("")),
	isLoadedGeometry_(false),
	component_(TEXT("client")),
	pFilter_(NULL),
	pUKBETicker_(nullptr)
{
	INFO_MSG("OuroborosApp::OuroborosApp(): hello!");
	installUKBETicker();
}

OuroborosApp::OuroborosApp(OuroborosArgs* pArgs):
	pArgs_(NULL),
	pNetworkInterface_(NULL),
	username_(TEXT("")),
	password_(TEXT("")),
	baseappIP_(TEXT("")),
	baseappTcpPort_(0),
	baseappUdpPort_(0),
	currserver_(TEXT("")),
	currstate_(TEXT("")),
	serverdatas_(),
	clientdatas_(),
	encryptedKey_(),
	serverVersion_(TEXT("")),
	clientVersion_(TEXT("")),
	serverScriptVersion_(TEXT("")),
	clientScriptVersion_(TEXT("")),
	serverProtocolMD5_(TEXT("@{OURO_SERVER_PROTO_MD5}")),
	serverEntitydefMD5_(TEXT("@{OURO_SERVER_ENTITYDEF_MD5}")),
	entity_uuid_(0),
	entity_id_(0),
	entity_type_(TEXT("")),
	useAliasEntityID_(@{OURO_USE_ALIAS_ENTITYID}),
	controlledEntities_(),
	entityServerPos_(),
	spacedatas_(),
	entities_(),
	entityIDAliasIDList_(),
	bufferedCreateEntityMessages_(),
	lastTickTime_(0.0),
	lastTickCBTime_(0.0),
	lastUpdateToServerTime_(0.0),
	spaceID_(0),
	spaceResPath_(TEXT("")),
	isLoadedGeometry_(false),
	component_(TEXT("client")),
	pFilter_(NULL),
	pUKBETicker_(nullptr)
{
	INFO_MSG("OuroborosApp::OuroborosApp(): hello!");
	initialize(pArgs);
	installUKBETicker();
}

OuroborosApp::~OuroborosApp()
{
	destroy();
	INFO_MSG("OuroborosApp::~OuroborosApp(): destructed!");
}

OuroborosApp* pOuroborosApp = nullptr;

OuroborosApp& OuroborosApp::getSingleton() 
{
	if(!pOuroborosApp)
		pOuroborosApp = new OuroborosApp();

	return *pOuroborosApp;
}

void OuroborosApp::destroyOuroborosApp() 
{
	if(pOuroborosApp)
	{
		delete pOuroborosApp;
		pOuroborosApp = nullptr;
		OBEvent::clear();
	}
}

bool OuroborosApp::initialize(OuroborosArgs* pArgs)
{
	if (isInitialized())
		return false;

	EntityDef::initialize();

		// Registration issue
	installEvents();

	pArgs_ = pArgs;
	reset();
	return true;
}

void OuroborosApp::installEvents()
{
	OUROBOROS_REGISTER_EVENT_OVERRIDE_FUNC(OBEventTypes::login, OBEventTypes::login, [this](const UOBEventData* pEventData)
	{
		const UOBEventData_login& data = static_cast<const UOBEventData_login&>(*pEventData);
		login(data.username, data.password, data.datas);
	});

	OUROBOROS_REGISTER_EVENT_OVERRIDE_FUNC(OBEventTypes::logout, OBEventTypes::logout, [this](const UOBEventData* pEventData)
	{
		logout();
	});

	OUROBOROS_REGISTER_EVENT_OVERRIDE_FUNC(OBEventTypes::createAccount, OBEventTypes::createAccount, [this](const UOBEventData* pEventData)
	{
		const UOBEventData_createAccount& data = static_cast<const UOBEventData_createAccount&>(*pEventData);
		createAccount(data.username, data.password, data.datas);
	});

	OUROBOROS_REGISTER_EVENT_OVERRIDE_FUNC(OBEventTypes::reloginBaseapp, OBEventTypes::reloginBaseapp, [this](const UOBEventData* pEventData)
	{
		reloginBaseapp();
	});

	OUROBOROS_REGISTER_EVENT_OVERRIDE_FUNC(OBEventTypes::resetPassword, OBEventTypes::resetPassword, [this](const UOBEventData* pEventData)
	{
		const UOBEventData_resetPassword& data = static_cast<const UOBEventData_resetPassword&>(*pEventData);
		resetPassword(data.username);
	});

	OUROBOROS_REGISTER_EVENT_OVERRIDE_FUNC(OBEventTypes::bindAccountEmail, OBEventTypes::bindAccountEmail, [this](const UOBEventData* pEventData)
	{
		const UOBEventData_bindAccountEmail& data = static_cast<const UOBEventData_bindAccountEmail&>(*pEventData);
		bindAccountEmail(data.email);
	});

	OUROBOROS_REGISTER_EVENT_OVERRIDE_FUNC(OBEventTypes::newPassword, OBEventTypes::newPassword, [this](const UOBEventData* pEventData)
	{
		const UOBEventData_newPassword& data = static_cast<const UOBEventData_newPassword&>(*pEventData);
		newPassword(data.old_password, data.new_password);
	});

	// internal events
	OUROBOROS_REGISTER_EVENT_OVERRIDE_FUNC("_closeNetwork", "_closeNetwork", [this](const UOBEventData* pEventData)
	{
		_closeNetwork();
	});
}

void OuroborosApp::destroy()
{
	reset();
	OUROBOROS_DEREGISTER_ALL_EVENT();
	resetMessages();

	OURO_SAFE_RELEASE(pArgs_);
	OURO_SAFE_RELEASE(pNetworkInterface_);
	OURO_SAFE_RELEASE(pFilter_);
	uninstallUKBETicker();
}

void OuroborosApp::resetMessages()
{
	serverErrs_.Clear();

	Messages::clear();
	EntityDef::clear();
	Entity::clear();

	INFO_MSG("OuroborosApp::resetMessages(): done!");
}

void OuroborosApp::reset()
{
	OBEvent::clearFiredEvents();

	clearEntities(true);

	currserver_ = TEXT("");
	currstate_ = TEXT("");

	serverdatas_.Empty();

	serverVersion_ = TEXT("");
	clientVersion_ = TEXT("@{OURO_VERSION}");
	serverScriptVersion_ = TEXT("");
	clientScriptVersion_ = TEXT("@{OURO_SCRIPT_VERSION}");

	entity_uuid_ = 0;
	entity_id_ = 0;
	entity_type_ = TEXT("");

	entityIDAliasIDList_.Empty();
	bufferedCreateEntityMessages_.Empty();

	lastTickTime_ = getTimeSeconds();
	lastTickCBTime_ = getTimeSeconds();
	lastUpdateToServerTime_ = getTimeSeconds();

	spacedatas_.Empty();

	spaceID_ = 0;
	spaceResPath_ = TEXT("");
	isLoadedGeometry_ = false;
	
	baseappUdpPort_ = 0;

	initNetwork();
}

void OuroborosApp::installUKBETicker()
{
	if (pUKBETicker_ == nullptr)
	{
		pUKBETicker_ = NewObject<UKBETicker>();
		pUKBETicker_->AddToRoot();
	}
}

void OuroborosApp::uninstallUKBETicker()
{
	if (pUKBETicker_)
	{
		pUKBETicker_->RemoveFromRoot();
		pUKBETicker_->ConditionalBeginDestroy();
		pUKBETicker_ = nullptr;
	}
}

bool OuroborosApp::initNetwork()
{
	OURO_SAFE_RELEASE(pFilter_);

	if (pNetworkInterface_)
		delete pNetworkInterface_;

	Messages::initialize();

	if (pArgs_)
	{
		if (pArgs_->forceDisableUDP || baseappUdpPort_ == 0)
			pNetworkInterface_ = new NetworkInterfaceTCP();
		else
			pNetworkInterface_ = new NetworkInterfaceKCP();
	}

	return true;
}

void OuroborosApp::_closeNetwork()
{
	if (pNetworkInterface_)
		pNetworkInterface_->close();
}

bool OuroborosApp::validEmail(const FString& strEmail)
{
	const FRegexPattern spattern(FString(TEXT("[a-z0-9._%+-]+@[a-z0-9.-]+\\.[a-z]{2,4}")));

	FRegexMatcher fMatcher(spattern, strEmail);

	if (fMatcher.FindNext()) {
		return true;
	}

	return false;
}

void OuroborosApp::process()
{
	// Processing network
	if (pNetworkInterface_)
		pNetworkInterface_->process();

	// handle the event thrown by the outer layer
	OBEvent::processInEvents();

	// Send heartbeat to the server and synchronize role information to the server
	sendTick();
}

void OuroborosApp::sendTick()
{
	if (!pNetworkInterface_ || !pNetworkInterface_->valid())
		return;

	double span = getTimeSeconds() - lastTickTime_;

	// Update the player's position and orientation to the server
	updatePlayerToServer();

	if (pArgs_->serverHeartbeatTick > 0 && span > pArgs_->serverHeartbeatTick)
	{
		span = lastTickCBTime_ - lastTickTime_;

		// If the heartbeat callback reception time is less than the heartbeat send time, the callback is not received.
		// At this point, the client should be notified that it is offline.
		if (span < 0)
		{
			SCREEN_ERROR_MSG("OuroborosApp::sendTick(): Receive appTick timeout!");
			pNetworkInterface_->close();
			return;
		}

		Message** Loginapp_onClientActiveTickMsgFind = Messages::messages.Find("Loginapp_onClientActiveTick");
		Message** Baseapp_onClientActiveTickMsgFind = Messages::messages.Find("Baseapp_onClientActiveTick");

		if (currserver_ == TEXT("loginapp"))
		{
			if (Loginapp_onClientActiveTickMsgFind)
			{
				Bundle* pBundle = Bundle::createObject();
				pBundle->newMessage(*Loginapp_onClientActiveTickMsgFind);
				pBundle->send(pNetworkInterface_);
			}
		}
		else
		{
			if (Baseapp_onClientActiveTickMsgFind)
			{
				Bundle* pBundle = Bundle::createObject();
				pBundle->newMessage(*Baseapp_onClientActiveTickMsgFind);
				pBundle->send(pNetworkInterface_);
			}
		}

		lastTickTime_ = getTimeSeconds();
	}
}

Entity* OuroborosApp::player()
{
	return findEntity(entity_id_);
}

Entity* OuroborosApp::findEntity(int32 entityID)
{
	Entity** pEntity = entities_.Find(entityID);
	if (pEntity == nullptr)
		return NULL;

	return *pEntity;
}

FString OuroborosApp::serverErr(uint16 id)
{
	return serverErrs_.ServerErrStr(id);
}

void OuroborosApp::updatePlayerToServer()
{
	if (pArgs_->syncPlayerMS <= 0 || spaceID_ == 0)
		return;

	double tnow = getTimeSeconds();
	double span = tnow - lastUpdateToServerTime_;

	if (span < ((double)pArgs_->syncPlayerMS / 1000.0))
		return;

	Entity* pPlayerEntity = player();
	if (!pPlayerEntity || !pPlayerEntity->inWorld()  || pPlayerEntity->isControlled())
		return;

	lastUpdateToServerTime_ = tnow;
	const FVector& position = pPlayerEntity->position;
	const FVector& direction = pPlayerEntity->direction;

	bool posHasChanged = (pPlayerEntity->entityLastLocalPos - position).Size() > 0.001f;
	bool dirHasChanged = (pPlayerEntity->entityLastLocalDir - direction).Size() > 0.001f;

	if (posHasChanged || dirHasChanged)
	{
		pPlayerEntity->entityLastLocalPos = position;
		pPlayerEntity->entityLastLocalDir = direction;

		Bundle* pBundle = Bundle::createObject();
		pBundle->newMessage(Messages::messages[TEXT("Baseapp_onUpdateDataFromClient"]));
		(*pBundle) << position.X;
		(*pBundle) << position.Y;
		(*pBundle) << position.Z;

		(*pBundle) << direction.X;
		(*pBundle) << direction.Y;
		(*pBundle) << direction.Z;

		(*pBundle) << (uint8)pPlayerEntity->isOnGround();
		(*pBundle) << spaceID_;

		pBundle->send(pNetworkInterface_);
	}

	// Start syncing the location of all controlled entities
	for(auto& item : controlledEntities_)
	{
		Entity* pEntity = item;
		const FVector& e_position = pEntity->position;
		const FVector& e_direction = pEntity->direction;

		posHasChanged = (pEntity->entityLastLocalPos - e_position).Size() > 0.001f;
		dirHasChanged = (pEntity->entityLastLocalDir - e_direction).Size() > 0.001f;

		if (posHasChanged || dirHasChanged)
		{
			pEntity->entityLastLocalPos = e_position;
			pEntity->entityLastLocalDir = e_direction;

			Bundle* pBundle = Bundle::createObject();
			pBundle->newMessage(Messages::messages[TEXT("Baseapp_onUpdateDataFromClientForControlledEntity"]));
			(*pBundle) << pEntity->id();
			(*pBundle) << e_position.X;
			(*pBundle) << e_position.Y;
			(*pBundle) << e_position.Z;

			(*pBundle) << e_direction.X;
			(*pBundle) << e_direction.Y;
			(*pBundle) << e_direction.Z;

			(*pBundle) << (uint8)pEntity->isOnGround();
			(*pBundle) << spaceID_;

			pBundle->send(pNetworkInterface_);
		}
	}
}

void OuroborosApp::Client_onAppActiveTickCB()
{
	lastTickCBTime_ = getTimeSeconds();
}

void OuroborosApp::hello()
{
	Bundle* pBundle = Bundle::createObject();
	if (currserver_ == TEXT("loginapp"))
		pBundle->newMessage(Messages::messages[TEXT("Loginapp_hello")]);
	else
		pBundle->newMessage(Messages::messages[TEXT("Baseapp_hello")]);

	OURO_SAFE_RELEASE(pFilter_);

	if (pArgs_->networkEncryptType == NETWORK_ENCRYPT_TYPE::ENCRYPT_TYPE_BLOWFISH)
	{
		pFilter_ = new BlowfishFilter();
		encryptedKey_ = ((BlowfishFilter*)pFilter_)->key();
		pNetworkInterface_->setFilter(NULL);
	}

	(*pBundle) << clientVersion_;
	(*pBundle) << clientScriptVersion_;
	pBundle->appendBlob(encryptedKey_);
	pBundle->send(pNetworkInterface_);
}

void OuroborosApp::Client_onHelloCB(MemoryStream& stream)
{
	FString str_serverVersion;
	stream >> str_serverVersion;
	stream >> serverScriptVersion_;

	FString serverProtocolMD5;
	stream >> serverProtocolMD5;

	FString serverEntitydefMD5;
	stream >> serverEntitydefMD5;

	int32 ctype;
	stream >> ctype;

	INFO_MSG("OuroborosApp::Client_onHelloCB(): verInfo(%s), scriptVersion(%s), srvProtocolMD5(%s), srvEntitydefMD5(%s), ctype(%d)!", 
		*str_serverVersion, *serverScriptVersion_, *serverProtocolMD5_, *serverEntitydefMD5_, ctype);

	if(str_serverVersion != "Getting")
	{
		serverVersion_ = str_serverVersion;

		/*
		if(serverProtocolMD5_ != serverProtocolMD5)
		{
			ERROR_MSG("OuroborosApp::Client_onHelloCB():  digest not match! serverProtocolMD5=%s(server: %s)", *serverProtocolMD5_, *serverProtocolMD5);

			UOBEventData_onVersionNotMatch* pEventData = NewObject<UOBEventData_onVersionNotMatch>();
			pEventData->clientVersion = clientVersion_;
			pEventData->serverVersion = serverVersion_;
			OUROBOROS_EVENT_FIRE(OBEventTypes::onVersionNotMatch, pEventData);
			return;
		}
		*/

		if(serverEntitydefMD5_ != serverEntitydefMD5)
		{
			ERROR_MSG("OuroborosApp::Client_onHelloCB():  digest not match! serverEntitydefMD5=%s(server: %s)", *serverEntitydefMD5_, *serverEntitydefMD5);

			UOBEventData_onVersionNotMatch* pEventData = NewObject<UOBEventData_onVersionNotMatch>();
			pEventData->clientVersion = clientVersion_;
			pEventData->serverVersion = serverVersion_;
			OUROBOROS_EVENT_FIRE(OBEventTypes::onVersionNotMatch, pEventData);
			return;
		}
	}

	if (pArgs_->networkEncryptType == NETWORK_ENCRYPT_TYPE::ENCRYPT_TYPE_BLOWFISH)
	{
		pNetworkInterface_->setFilter(pFilter_);
		pFilter_ = NULL;
	}

	onServerDigest();

	if (currserver_ == TEXT("baseapp"))
	{
		onLogin_baseapp();
	}
	else
	{
		onLogin_loginapp();
	}
}

void OuroborosApp::Client_onVersionNotMatch(MemoryStream& stream)
{
	stream >> serverVersion_;

	ERROR_MSG("OuroborosApp::Client_onVersionNotMatch(): verInfo=%s(server: %s)", *clientVersion_, *serverVersion_);

	UOBEventData_onVersionNotMatch* pEventData = NewObject<UOBEventData_onVersionNotMatch>();
	pEventData->clientVersion = clientVersion_;
	pEventData->serverVersion = serverVersion_;
	OUROBOROS_EVENT_FIRE(OBEventTypes::onVersionNotMatch, pEventData);
}

void OuroborosApp::Client_onScriptVersionNotMatch(MemoryStream& stream)
{
	stream >> serverScriptVersion_;

	ERROR_MSG("OuroborosApp::Client_onScriptVersionNotMatch(): verInfo=%s(server: %s)", *clientScriptVersion_, *serverScriptVersion_);

	UOBEventData_onScriptVersionNotMatch* pEventData = NewObject<UOBEventData_onScriptVersionNotMatch>();
	pEventData->clientScriptVersion = clientScriptVersion_;
	pEventData->serverScriptVersion = serverScriptVersion_;
	OUROBOROS_EVENT_FIRE(OBEventTypes::onScriptVersionNotMatch, pEventData);
}

void OuroborosApp::Client_onImportClientSDK(MemoryStream& stream)
{
	UOBEventData_onImportClientSDK* pEventData = NewObject<UOBEventData_onImportClientSDK>();

	pEventData->remainingFiles = stream.readInt32();
	pEventData->fileName = stream.readString();
	pEventData->fileSize = stream.readInt32();
	stream.readBlob(pEventData->fileDatas);

	OUROBOROS_EVENT_FIRE("onImportClientSDK", pEventData);
}

void OuroborosApp::Client_onKicked(uint16 failedcode)
{
	DEBUG_MSG("OuroborosApp::Client_onKicked(): failedcode=%d, %s", failedcode, *serverErr(failedcode));

	UOBEventData_onKicked* pEventData = NewObject<UOBEventData_onKicked>();
	pEventData->failedcode = failedcode;
	pEventData->errorStr = serverErr(failedcode);
	OUROBOROS_EVENT_FIRE(OBEventTypes::onKicked, pEventData);
}

void OuroborosApp::onServerDigest()
{
}

void OuroborosApp::onConnectCallback(FString ip, uint16 port, bool success, int userdata)
{
	if (userdata == 0)
	{
		onConnectTo_loginapp_login_callback(ip, port, success);
	}
	else if (userdata == 1)
	{
		onConnectTo_loginapp_create_callback(ip, port, success);
	}
	else if (userdata == 2)
	{
		onConnectTo_baseapp_callback(ip, port, success);
	}
	else if (userdata == 3)
	{
		onReloginTo_baseapp_callback(ip, port, success);
	}
	else if (userdata == 4)
	{
		onConnectTo_resetpassword_callback(ip, port, success);
	}
	else if (userdata == 5)
	{
		//onConnectTo_resetpassword_callback(ip, port, success);
	}
	else
	{
		check(false);
	}
}

bool OuroborosApp::login(const FString& username, const FString& password, const TArray<uint8>& datas)
{
	if (username.Len() == 0)
	{
		ERROR_MSG("OuroborosApp::login(): username is empty!");
		return false;
	}

	if (password.Len() == 0)
	{
		ERROR_MSG("OuroborosApp::login(): password is empty!");
		return false;
	}

	username_ = username;
	password_ = password;
	clientdatas_ = datas;

	login_loginapp(true);
	return true;
}

bool OuroborosApp::logout()
{
	if (currserver_ != TEXT("baseapp"))
		return false;

	INFO_MSG("OuroborosApp::logout()");
	Bundle* pBundle = Bundle::createObject();
	pBundle->newMessage(Messages::messages[TEXT("Baseapp_logoutBaseapp"]));
	(*pBundle) << entity_uuid_;
	(*pBundle) << entity_id_;
	pBundle->send(pNetworkInterface_);
	return true;
}

void OuroborosApp::login_loginapp(bool noconnect)
{
	if (noconnect)
	{
		reset();
		pNetworkInterface_->connectTo(pArgs_->ip, pArgs_->port, this, 0);
	}
	else
	{
		INFO_MSG("OuroborosApp::login_loginapp(): send login! username=%s", *username_);
		Bundle* pBundle = Bundle::createObject();
		pBundle->newMessage(Messages::messages[TEXT("Loginapp_login"]));
		(*pBundle) << (uint8)pArgs_->clientType;
		pBundle->appendBlob(clientdatas_);
		(*pBundle) << username_;
		(*pBundle) << password_;
		pBundle->send(pNetworkInterface_);
	}
}

void OuroborosApp::onConnectTo_loginapp_login_callback(FString ip, uint16 port, bool success)
{
	if (!success)
	{
		ERROR_MSG("OuroborosApp::onConnectTo_loginapp_login_callback(): connect %s:%d is error!", *ip, port);
		return;
	}

	currserver_ = TEXT("loginapp");
	currstate_ = TEXT("login");

	INFO_MSG("OuroborosApp::onConnectTo_loginapp_login_callback(): connect %s:%d is success!", *ip, port);

	hello();
}

void OuroborosApp::onLogin_loginapp()
{
	lastTickCBTime_ = getTimeSeconds();
	login_loginapp(false);
}

void OuroborosApp::Client_onLoginFailed(MemoryStream& stream)
{
	uint16 failedcode = 0;
	stream >> failedcode;
	stream.readBlob(serverdatas_);
	ERROR_MSG("OuroborosApp::Client_onLoginFailed(): failedcode(%d:%s), datas(%d)!", failedcode, *serverErr(failedcode), serverdatas_.Num());

	UOBEventData_onLoginFailed* pEventData = NewObject<UOBEventData_onLoginFailed>();
	pEventData->failedcode = failedcode;
	pEventData->errorStr = serverErr(failedcode);
	pEventData->serverdatas = serverdatas_;
	OUROBOROS_EVENT_FIRE(OBEventTypes::onLoginFailed, pEventData);
}

void OuroborosApp::Client_onLoginSuccessfully(MemoryStream& stream)
{
	FString accountName;
	stream >> accountName;
	username_ = accountName;
	stream >> baseappIP_;
	stream >> baseappTcpPort_;
	stream >> baseappUdpPort_;
	stream.readBlob(serverdatas_);

	DEBUG_MSG("OuroborosApp::Client_onLoginSuccessfully(): accountName(%s), addr("
		 "%s:%d:%d), datas(%d)!", *accountName, *baseappIP_, baseappTcpPort_, baseappUdpPort_, serverdatas_.Num());
	
	login_baseapp(true);
}

void OuroborosApp::login_baseapp(bool noconnect)
{
	if (noconnect)
	{
		OUROBOROS_EVENT_FIRE(OBEventTypes::onLoginBaseapp, NewObject<UOBEventData_onLoginBaseapp>());

		pNetworkInterface_->destroy();
		pNetworkInterface_ = NULL;
		initNetwork();
		pNetworkInterface_->connectTo(baseappIP_, (!pArgs_->forceDisableUDP && baseappUdpPort_ > 0) ? baseappUdpPort_ : baseappTcpPort_, this, 2);
	}
	else
	{
		Bundle* pBundle = Bundle::createObject();
		pBundle->newMessage(Messages::messages[TEXT("Baseapp_loginBaseapp"]));
		(*pBundle) << username_;
		(*pBundle) << password_;
		pBundle->send(pNetworkInterface_);
	}
}

void OuroborosApp::onConnectTo_baseapp_callback(FString ip, uint16 port, bool success)
{
	lastTickCBTime_ = getTimeSeconds();

	if (!success)
	{
		ERROR_MSG("OuroborosApp::onConnectTo_baseapp_callback(): connect %s:%d is error!", *ip, port);
		return;
	}

	currserver_ = TEXT("baseapp");
	currstate_ = TEXT("");

	DEBUG_MSG("OuroborosApp::onConnectTo_baseapp_callback(): connect %s:%d is successfully!", *ip, port);

	hello();
}

void OuroborosApp::onLogin_baseapp()
{
	lastTickCBTime_ = getTimeSeconds();
	login_baseapp(false);
}

void OuroborosApp::reloginBaseapp()
{
	lastTickTime_ = getTimeSeconds();
	lastTickCBTime_ = getTimeSeconds();

	if(pNetworkInterface_->valid())
		return;

	UOBEventData_onReloginBaseapp* pEventData = NewObject<UOBEventData_onReloginBaseapp>();
	OUROBOROS_EVENT_FIRE(OBEventTypes::onReloginBaseapp, pEventData);

	pNetworkInterface_->connectTo(baseappIP_, (!pArgs_->forceDisableUDP && baseappUdpPort_ > 0) ? baseappUdpPort_ : baseappTcpPort_, this, 3);
}

void OuroborosApp::onReloginTo_baseapp_callback(FString ip, uint16 port, bool success)
{
	if (!success)
	{
		ERROR_MSG("OuroborosApp::onReloginTo_baseapp_callback(): connect %s:%d is error!", *ip, port);
		return;
	}

	INFO_MSG("OuroborosApp::onReloginTo_baseapp_callback(): connect %s:%d is success!", *ip, port);

	Bundle* pBundle = Bundle::createObject();
	pBundle->newMessage(Messages::messages[TEXT("Baseapp_reloginBaseapp"]));
	(*pBundle) << username_;
	(*pBundle) << password_;
	(*pBundle) << entity_uuid_;
	(*pBundle) << entity_id_;
	pBundle->send(pNetworkInterface_);

	lastTickCBTime_ = getTimeSeconds();
}

void OuroborosApp::Client_onLoginBaseappFailed(uint16 failedcode)
{
	ERROR_MSG("OuroborosApp::Client_onLoginBaseappFailed(): failedcode(%d:%s)!", failedcode, *serverErr(failedcode));

	UOBEventData_onLoginBaseappFailed* pEventData = NewObject<UOBEventData_onLoginBaseappFailed>();
	pEventData->failedcode = failedcode;
	pEventData->errorStr = serverErr(failedcode);
	OUROBOROS_EVENT_FIRE(OBEventTypes::onLoginBaseappFailed, pEventData);
}

void OuroborosApp::Client_onReloginBaseappFailed(uint16 failedcode)
{
	ERROR_MSG("OuroborosApp::Client_onReloginBaseappFailed(): failedcode(%d:%s)!", failedcode, *serverErr(failedcode));

	UOBEventData_onReloginBaseappFailed* pEventData = NewObject<UOBEventData_onReloginBaseappFailed>();
	pEventData->failedcode = failedcode;
	pEventData->errorStr = serverErr(failedcode);
	OUROBOROS_EVENT_FIRE(OBEventTypes::onReloginBaseappFailed, pEventData);
}

void OuroborosApp::Client_onReloginBaseappSuccessfully(MemoryStream& stream)
{
	stream >> entity_uuid_;
	ERROR_MSG("OuroborosApp::Client_onReloginBaseappSuccessfully(): name(%s)!", *username_);
	UOBEventData_onReloginBaseappSuccessfully* pEventData = NewObject<UOBEventData_onReloginBaseappSuccessfully>();
	OUROBOROS_EVENT_FIRE(OBEventTypes::onReloginBaseappSuccessfully, pEventData);
}

void OuroborosApp::Client_onCreatedProxies(uint64 rndUUID, int32 eid, FString& entityType)
{
	DEBUG_MSG("OuroborosApp::Client_onCreatedProxies(): eid(%d), entityType(%s)!", eid, *entityType);

	entity_uuid_ = rndUUID;
	entity_id_ = eid;
	entity_type_ = entityType;

	if (!entities_.Contains(eid))
	{
		ScriptModule** pModuleFind = EntityDef::moduledefs.Find(entityType);
		if (!pModuleFind)
		{
			SCREEN_ERROR_MSG("OuroborosApp::Client_onCreatedProxies(): not found ScriptModule(%s)!", *entityType);
			return;
		}

		ScriptModule* pModule = *pModuleFind;

		Entity* pEntity = pModule->createEntity();
		pEntity->id(eid);
		pEntity->className(entityType);
		pEntity->onGetBase();

		entities_.Add(eid, pEntity);

		MemoryStream** entityMessageFind = bufferedCreateEntityMessages_.Find(eid);
		if (entityMessageFind)
		{
			MemoryStream* entityMessage = *entityMessageFind;
			Client_onUpdatePropertys(*entityMessage);
			bufferedCreateEntityMessages_.Remove(eid);
			MemoryStream::reclaimObject(entityMessage);
		}

		pEntity->__init__();
		pEntity->attachComponents();
		pEntity->inited(true);

		if (pArgs_->isOnInitCallPropertysSetMethods)
			pEntity->callPropertysSetMethods();
	}
	else
	{
		MemoryStream** entityMessageFind = bufferedCreateEntityMessages_.Find(eid);
		if (entityMessageFind)
		{
			MemoryStream* entityMessage = *entityMessageFind;
			Client_onUpdatePropertys(*entityMessage);
			bufferedCreateEntityMessages_.Remove(eid);
			MemoryStream::reclaimObject(entityMessage);
		}
	}
}

ENTITY_ID OuroborosApp::getViewEntityIDFromStream(MemoryStream& stream)
{
	ENTITY_ID id = 0;

	if (!pArgs_->useAliasEntityID)
	{
		stream >> id;
		return id;
	}

	if (entityIDAliasIDList_.Num()> 255)
	{
		stream >> id;
	}
	else
	{
		uint8 aliasID = 0;
		stream >> aliasID;

		// If 0 and the client's previous step is to re-login or reconnect and the server entity is online during the disconnection
		// This error can be ignored, because cellapp may always send synchronization messages to baseapp, not waiting when the client reconnects
		// When the server initialization step starts, it receives the synchronization information, and an error occurs here.
		if (entityIDAliasIDList_.Num() <= aliasID)
			return 0;

		id = entityIDAliasIDList_[aliasID];
	}

	return id;
}

void OuroborosApp::Client_onUpdatePropertysOptimized(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);
	onUpdatePropertys_(eid, stream);
}

void OuroborosApp::Client_onUpdatePropertys(MemoryStream& stream)
{
	ENTITY_ID eid;
	stream >> eid;
	onUpdatePropertys_(eid, stream);
}

void OuroborosApp::onUpdatePropertys_(ENTITY_ID eid, MemoryStream& stream)
{
	Entity** pEntityFind = entities_.Find(eid);

	if (!pEntityFind)
	{
		MemoryStream** entityMessageFind = bufferedCreateEntityMessages_.Find(eid);
		if (entityMessageFind)
		{
			ERROR_MSG("OuroborosApp::onUpdatePropertys_(): entity(%d) not found!", eid);
			return;
		}

		MemoryStream* stream1 = MemoryStream::createObject();
		stream1->append(stream);
		stream1->rpos(stream.rpos() - 4);
		bufferedCreateEntityMessages_.Add(eid, stream1);
		return;
	}
	
	Entity* pEntity = *pEntityFind;

	pEntity->onUpdatePropertys(stream);
}

void OuroborosApp::Client_onEntityDestroyed(int32 eid)
{
	DEBUG_MSG("OuroborosApp::Client_onEntityDestroyed(): entity(%d)!", eid);

	Entity** pEntityFind = entities_.Find(eid);

	if (!pEntityFind)
	{
		ERROR_MSG("OuroborosApp::Client_onEntityDestroyed(): entity(%d) not found!", eid);
		return;
	}

	Entity* pEntity = (*pEntityFind);

	if (pEntity->inWorld())
	{
		if (entity_id_ == eid)
			clearSpace(false);

		(*pEntityFind)->leaveWorld();
	}

	if (controlledEntities_.Contains(pEntity))
	{
		controlledEntities_.Remove(pEntity);

		UOBEventData_onLoseControlledEntity* pEventData = NewObject<UOBEventData_onLoseControlledEntity>();
		pEventData->entityID = pEntity->id();
		OUROBOROS_EVENT_FIRE(OBEventTypes::onLoseControlledEntity, pEventData);
	}

	entities_.Remove(eid);
	pEntity->destroy();
}

void OuroborosApp::clearSpace(bool isall)
{
	entityIDAliasIDList_.Empty();
	spacedatas_.Empty();
	clearEntities(isall);
	isLoadedGeometry_ = false;
	spaceID_ = 0;
}

void OuroborosApp::clearEntities(bool isall)
{
	controlledEntities_.Empty();

	if (!isall)
	{
		Entity* pEntity = player();

		for(auto& item : entities_)
		{
			if (item.Key == pEntity->id())
				continue;

			if (item.Value->inWorld())
				item.Value->leaveWorld();

			item.Value->destroy();
		}

		entities_.Empty();
		entities_.Add(pEntity->id(), pEntity);
	}
	else
	{
		for (auto& item : entities_)
		{
			if (item.Value->inWorld())
				item.Value->leaveWorld();

			item.Value->destroy();
		}

		entities_.Empty();
	}
}

void OuroborosApp::Client_initSpaceData(MemoryStream& stream)
{
	clearSpace(false);
	stream >> spaceID_;

	while (stream.length() > 0)
	{
		FString key;
		FString val;

		stream >> key >> val;
		Client_setSpaceData(spaceID_, key, val);
	}

	DEBUG_MSG("OuroborosApp::Client_initSpaceData(): spaceID(%d), size(%d)!", spaceID_, spacedatas_.Num());
}

void OuroborosApp::Client_setSpaceData(uint32 spaceID, const FString& key, const FString& value)
{
	DEBUG_MSG("OuroborosApp::Client_setSpaceData(): spaceID(%d), key(%s), value(%s)!", spaceID_, *key, *value);
	spacedatas_.Add(key, value);

	if (key == TEXT("_mapping"))
		addSpaceGeometryMapping(spaceID, value);

	UOBEventData_onSetSpaceData* pEventData = NewObject<UOBEventData_onSetSpaceData>();
	pEventData->spaceID = spaceID_;
	pEventData->key = key;
	pEventData->value = value;
	OUROBOROS_EVENT_FIRE(OBEventTypes::onSetSpaceData, pEventData);
}

void OuroborosApp::Client_delSpaceData(uint32 spaceID, const FString& key)
{
	DEBUG_MSG("OuroborosApp::Client_delSpaceData(): spaceID(%d), key(%s)!", spaceID_, *key);

	spacedatas_.Remove(key);

	UOBEventData_onDelSpaceData* pEventData = NewObject<UOBEventData_onDelSpaceData>();
	pEventData->spaceID = spaceID_;
	pEventData->key = key;
	OUROBOROS_EVENT_FIRE(OBEventTypes::onDelSpaceData, pEventData);
}

void OuroborosApp::addSpaceGeometryMapping(uint32 uspaceID, const FString& respath)
{
	DEBUG_MSG("OuroborosApp::addSpaceGeometryMapping(): spaceID(%d), respath(%s)!", spaceID_, *respath);

	isLoadedGeometry_ = true;
	spaceID_ = uspaceID;
	spaceResPath_ = respath;

	UOBEventData_addSpaceGeometryMapping* pEventData = NewObject<UOBEventData_addSpaceGeometryMapping>();
	pEventData->spaceResPath = spaceResPath_;
	OUROBOROS_EVENT_FIRE(OBEventTypes::addSpaceGeometryMapping, pEventData);
}

FString OuroborosApp::getSpaceData(const FString& key)
{
	FString* valFind = spacedatas_.Find(key);

	if(!valFind)
		return FString();

	return (*valFind);
}

void OuroborosApp::resetPassword(const FString& username)
{
	username_ = username;
	resetpassword_loginapp(true);
}

void OuroborosApp::resetpassword_loginapp(bool noconnect)
{
	if (noconnect)
	{
		reset();
		pNetworkInterface_->connectTo(pArgs_->ip, pArgs_->port, this, 4);
	}
	else
	{
		INFO_MSG("OuroborosApp::resetpassword_loginapp(): send resetpassword! username=%s", *username_);
		Bundle* pBundle = Bundle::createObject();
		pBundle->newMessage(Messages::messages[TEXT("Loginapp_reqAccountResetPassword"]));
		(*pBundle) << username_;
		pBundle->send(pNetworkInterface_);
	}
}

void OuroborosApp::onOpenLoginapp_resetpassword()
{
	DEBUG_MSG("OuroborosApp::onOpenLoginapp_resetpassword(): successfully!");
	currserver_ = "loginapp";
	currstate_ = "resetpassword";
	lastTickCBTime_ = getTimeSeconds();

	resetpassword_loginapp(false);
}

void OuroborosApp::onConnectTo_resetpassword_callback(FString ip, uint16 port, bool success)
{
	lastTickCBTime_ = getTimeSeconds();

	if (!success)
	{
		ERROR_MSG("OuroborosApp::onConnectTo_resetpassword_callback(): connect %s:%d is error!", *ip, port);
		return;
	}

	INFO_MSG("OuroborosApp::onConnectTo_resetpassword_callback(): connect %s:%d is success!", *ip, port);

	onOpenLoginapp_resetpassword();
}

void OuroborosApp::Client_onReqAccountResetPasswordCB(uint16 failcode)
{
	if (failcode != 0)
	{
		ERROR_MSG("OuroborosApp::Client_onReqAccountResetPasswordCB(): reset failed! code=%d, error=%s! username=%s", failcode, *serverErr(failcode), *username_);
		return;
	}

	DEBUG_MSG("OuroborosApp::Client_onReqAccountResetPasswordCB(): successfully! username=%s", *username_);
}

bool OuroborosApp::createAccount(const FString& username, const FString& password, const TArray<uint8>& datas)
{
	if (username.Len() == 0)
	{
		ERROR_MSG("OuroborosApp::createAccount(): username is empty!");
		return false;
	}

	if (password.Len() == 0)
	{
		ERROR_MSG("OuroborosApp::createAccount(): password is empty!");
		return false;
	}

	username_ = username;
	password_ = password;
	clientdatas_ = datas;

	createAccount_loginapp(true);
	return true;
}

void OuroborosApp::createAccount_loginapp(bool noconnect)
{
	if (noconnect)
	{
		reset();
		pNetworkInterface_->connectTo(pArgs_->ip, pArgs_->port, this, 1);
	}
	else
	{
		INFO_MSG("OuroborosApp::createAccount_loginapp(): send create! username=%s", *username_);
		Bundle* pBundle = Bundle::createObject();
		pBundle->newMessage(Messages::messages[TEXT("Loginapp_reqCreateAccount"]));
		(*pBundle) << username_;
		(*pBundle) << password_;
		pBundle->appendBlob(clientdatas_);
		pBundle->send(pNetworkInterface_);
	}
}

void OuroborosApp::onOpenLoginapp_createAccount()
{
	DEBUG_MSG("OuroborosApp::onOpenLoginapp_createAccount(): successfully!");

	currserver_ = TEXT("loginapp");
	currstate_ = TEXT("createAccount");
	lastTickCBTime_ = getTimeSeconds();

	createAccount_loginapp(false);
}

void OuroborosApp::onConnectTo_loginapp_create_callback(FString ip, uint16 port, bool success)
{
	lastTickCBTime_ = getTimeSeconds();

	if (!success)
	{
		ERROR_MSG("OuroborosApp::onConnectTo_loginapp_create_callback(): connect %s:%d is error!", *ip, port);
		return;
	}

	INFO_MSG("OuroborosApp::onConnectTo_loginapp_create_callback(): connect %s:%d is success!", *ip, port);

	onOpenLoginapp_createAccount();
}

void OuroborosApp::Client_onCreateAccountResult(MemoryStream& stream)
{
	uint16 retcode;
	stream >> retcode;

	TArray<uint8> datas;
	stream.readBlob(datas);

	UOBEventData_onCreateAccountResult* pEventData = NewObject<UOBEventData_onCreateAccountResult>();
	pEventData->errorCode = retcode;
	pEventData->errorStr = serverErr(retcode);
	pEventData->datas = datas;
	OUROBOROS_EVENT_FIRE(OBEventTypes::onCreateAccountResult, pEventData);

	if (retcode != 0)
	{
		WARNING_MSG("OuroborosApp::Client_onCreateAccountResult(): create(%s) failed! error=%d(%s)!", *username_, retcode, *serverErr(retcode));
		return;
	}

	DEBUG_MSG("OuroborosApp::Client_onCreateAccountResult(): create(%s) is successfully!", *username_);
}

void OuroborosApp::bindAccountEmail(const FString& emailAddress)
{
	INFO_MSG("OuroborosApp::bindAccountEmail(): send bindAccountEmail! username=%s", *username_);
	Bundle* pBundle = Bundle::createObject();
	pBundle->newMessage(Messages::messages[TEXT("Baseapp_reqAccountBindEmail"]));
	(*pBundle) << entity_id_;
	(*pBundle) << password_;
	(*pBundle) << emailAddress;
	pBundle->send(pNetworkInterface_);
}

void OuroborosApp::Client_onReqAccountBindEmailCB(uint16 failcode)
{
	if (failcode != 0)
	{
		ERROR_MSG("OuroborosApp::Client_onReqAccountBindEmailCB(): bind failed! code=%d, error=%s! username=%s", failcode, *serverErr(failcode), *username_);
		return;
	}

	DEBUG_MSG("OuroborosApp::Client_onReqAccountBindEmailCB(): successfully! username=%s", *username_);
}

void OuroborosApp::newPassword(const FString& old_password, const FString& new_password)
{
	INFO_MSG("OuroborosApp::newPassword(): send newPassword! username=%s", *username_);
	Bundle* pBundle = Bundle::createObject();
	pBundle->newMessage(Messages::messages[TEXT("Baseapp_reqAccountNewPassword"]));
	(*pBundle) << entity_id_;
	(*pBundle) << old_password;
	(*pBundle) << new_password;
	pBundle->send(pNetworkInterface_);
}

void OuroborosApp::Client_onReqAccountNewPasswordCB(uint16 failcode)
{
	if (failcode != 0)
	{
		ERROR_MSG("OuroborosApp::Client_onReqAccountNewPasswordCB(): newPassword failed! code=%d, error=%s! username=%s", failcode, *serverErr(failcode), *username_);
		return;
	}

	DEBUG_MSG("OuroborosApp::Client_onReqAccountNewPasswordCB(): successfully! username=%s", *username_);
}

void OuroborosApp::Client_onRemoteMethodCallOptimized(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);
	onRemoteMethodCall_(eid, stream);
}

void OuroborosApp::Client_onRemoteMethodCall(MemoryStream& stream)
{
	ENTITY_ID eid = 0;
	stream >> eid;
	onRemoteMethodCall_(eid, stream);
}

void OuroborosApp::onRemoteMethodCall_(ENTITY_ID eid, MemoryStream& stream)
{
	Entity** pEntityFind = entities_.Find(eid);

	if (!pEntityFind)
	{
		ERROR_MSG("OuroborosApp::onRemoteMethodCall_(): entity(%d) not found!", eid);
		return;
	}

	Entity* pEntity = *pEntityFind;
	pEntity->onRemoteMethodCall(stream);
}

void OuroborosApp::Client_onControlEntity(ENTITY_ID eid, int8 isControlled)
{
	Entity** pEntityFind = entities_.Find(eid);

	if (!pEntityFind)
	{
		ERROR_MSG("OuroborosApp::Client_onControlEntity(): entity(%d) not found!", eid);
		return;
	}

	bool isCont = isControlled != 0;

	if (isCont)
	{
		// If the controlled person is the player himself, it means that the player himself is controlled by others.
		// So the player should not enter this controlled list.
		if (entity_id_ != (*pEntityFind)->id())
		{
			controlledEntities_.Add((*pEntityFind));
		}
	}
	else
	{
		controlledEntities_.Remove((*pEntityFind));
	}

	(*pEntityFind)->isControlled(isCont);
	(*pEntityFind)->onControlled(isCont);

	UOBEventData_onControlled* pEventData = NewObject<UOBEventData_onControlled>();
	pEventData->entityID = (*pEntityFind)->id();
	pEventData->isControlled = isCont;
	OUROBOROS_EVENT_FIRE(OBEventTypes::onControlled, pEventData);
}

void OuroborosApp::Client_onStreamDataStarted(int16 id, uint32 datasize, FString descr)
{
	UOBEventData_onStreamDataStarted* pEventData = NewObject<UOBEventData_onStreamDataStarted>();
	pEventData->resID = id;
	pEventData->dataSize = datasize;
	pEventData->dataDescr = descr;
	OUROBOROS_EVENT_FIRE(OBEventTypes::onStreamDataStarted, pEventData);
}

void OuroborosApp::Client_onStreamDataRecv(MemoryStream& stream)
{
	UOBEventData_onStreamDataRecv* pEventData = NewObject<UOBEventData_onStreamDataRecv>();

	uint16 id = stream.read<uint16>();
	pEventData->resID = id;
	stream.readBlob(pEventData->data);

	OUROBOROS_EVENT_FIRE(OBEventTypes::onStreamDataRecv, pEventData);
}

void OuroborosApp::Client_onStreamDataCompleted(int16 id)
{
	UOBEventData_onStreamDataCompleted* pEventData = NewObject<UOBEventData_onStreamDataCompleted>();
	pEventData->resID = id;
	OUROBOROS_EVENT_FIRE(OBEventTypes::onStreamDataCompleted, pEventData);
}

void OuroborosApp::Client_onEntityEnterWorld(MemoryStream& stream)
{
	ENTITY_ID eid;
	stream >> eid;

	if (entity_id_ > 0 && entity_id_ != eid)
		entityIDAliasIDList_.Add(eid);

	uint16 uEntityType;

	if (EntityDef::idmoduledefs.Num() > 255)
		uEntityType = stream.read<uint16>();
	else
		uEntityType = stream.read<uint8>();

	int8 isOnGround = 1;

	if (stream.length() > 0)
		isOnGround = stream.read<int8>();

	ScriptModule** pScriptModuleFind = EntityDef::idmoduledefs.Find(uEntityType);
	if (!pScriptModuleFind)
	{
		SCREEN_ERROR_MSG("OuroborosApp::Client_onEntityEnterWorld(): not found ScriptModule(utype = %d)!", uEntityType);
		return;
	}

	ScriptModule* pScriptModule = *pScriptModuleFind;
	FString entityType = pScriptModule->name;
	// DEBUG_MSG("OuroborosApp::Client_onEntityEnterWorld(): %s(%d), spaceID(%d)!", *entityType, eid, spaceID_);

	Entity** pEntityFind = entities_.Find(eid);

	if (!pEntityFind)
	{
		MemoryStream** entityMessageFind = bufferedCreateEntityMessages_.Find(eid);
		if (!entityMessageFind)
		{
			ERROR_MSG("OuroborosApp::Client_onEntityEnterWorld(): entity(%d) not found!", eid);
			return;
		}

		pScriptModuleFind = EntityDef::moduledefs.Find(entityType);
		if (!pScriptModuleFind)
		{
			SCREEN_ERROR_MSG("OuroborosApp::Client_onEntityEnterWorld(): not found ScriptModule(%s)!", *entityType);
			return;
		}

		ScriptModule* pModule = *pScriptModuleFind;

		Entity* pEntity = pModule->createEntity();
		pEntity->id(eid);
		pEntity->className(entityType);
		pEntity->onGetCell();

		entities_.Add(eid, pEntity);

		Client_onUpdatePropertys(*(*entityMessageFind));
		MemoryStream::reclaimObject((*entityMessageFind));
		bufferedCreateEntityMessages_.Remove(eid);

		pEntity->isOnGround(isOnGround > 0);
		pEntity->onDirectionChanged(pEntity->direction);
		pEntity->onPositionChanged(pEntity->position);

		pEntity->__init__();
		pEntity->attachComponents();
		pEntity->inited(true);
		pEntity->inWorld(true);
		pEntity->enterWorld();

		if (pArgs_->isOnInitCallPropertysSetMethods)
			pEntity->callPropertysSetMethods();
	}
	else
	{
		Entity* pEntity = (*pEntityFind);

		if (!pEntity->inWorld())
		{
			// For security reasons, clear it here.
			// If you use giveClientTo to switch control on the server
			// The previous entity has entered the world, and the switched entity also enters the world. Here, the information of the previous entity entering the world may remain.
			entityIDAliasIDList_.Empty();
			clearEntities(false);
			entities_.Add(pEntity->id(), pEntity);

			pEntity->onGetCell();

			pEntity->onDirectionChanged(pEntity->direction);
			pEntity->onPositionChanged(pEntity->position);

			entityServerPos_ = pEntity->position;
			pEntity->isOnGround(isOnGround > 0);
			pEntity->inWorld(true);
			pEntity->enterWorld();

			if (pArgs_->isOnInitCallPropertysSetMethods)
				pEntity->callPropertysSetMethods();
		}
	}
}

void OuroborosApp::Client_onEntityLeaveWorldOptimized(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);
	Client_onEntityLeaveWorld(eid);
}

void OuroborosApp::Client_onEntityLeaveWorld(ENTITY_ID eid)
{
	Entity** pEntityFind = entities_.Find(eid);
	
	if (!pEntityFind)
	{
		ERROR_MSG("OuroborosApp::Client_onEntityLeaveWorld(): entity(%d) not found!", eid);
		return;
	}

	Entity* pEntity = *pEntityFind;

	if (pEntity->inWorld())
		pEntity->leaveWorld();

	if (entity_id_ == eid)
	{
		clearSpace(false);
		pEntity->onLoseCell();
	}
	else
	{
		if (controlledEntities_.Contains(pEntity))
		{
			controlledEntities_.Remove(pEntity);

			UOBEventData_onLoseControlledEntity* pEventData = NewObject<UOBEventData_onLoseControlledEntity>();
			pEventData->entityID = pEntity->id();
			OUROBOROS_EVENT_FIRE(OBEventTypes::onLoseControlledEntity, pEventData);
		}

		entities_.Remove(eid);
		pEntity->destroy();
		entityIDAliasIDList_.Remove(eid);
	}
}

void OuroborosApp::Client_onEntityEnterSpace(MemoryStream& stream)
{
	ENTITY_ID eid = stream.read<int32>();
	spaceID_ = stream.read<uint32>();

	int8 isOnGround = 1;

	if (stream.length() > 0)
		isOnGround = stream.read<int8>();

	Entity** pEntityFind = entities_.Find(eid);

	if (!pEntityFind)
	{
		ERROR_MSG("OuroborosApp::Client_onEntityEnterSpace(): entity(%d) not found!", eid);
		return;
	}

	Entity* pEntity = *pEntityFind;
	pEntity->isOnGround(isOnGround > 0);
	entityServerPos_ = pEntity->position;
	pEntity->enterSpace();
}

void OuroborosApp::Client_onEntityLeaveSpace(ENTITY_ID eid)
{
	Entity** pEntityFind = entities_.Find(eid);

	if (!pEntityFind)
	{
		ERROR_MSG("OuroborosApp::Client_onEntityLeaveSpace(): entity(%d) not found!", eid);
		return;
	}

	Entity* pEntity = *pEntityFind;
	pEntity->leaveSpace();

	clearSpace(false);
}

void OuroborosApp::Client_onUpdateBasePos(float x, float y, float z)
{
	entityServerPos_.X = x;
	entityServerPos_.Y = y;
	entityServerPos_.Z = z;

	Entity* pEntity = player();
	if (pEntity && pEntity->isControlled())
	{
		pEntity->position.Set(entityServerPos_.X, entityServerPos_.Y, entityServerPos_.Z);

		UOBEventData_updatePosition* pEventData = NewObject<UOBEventData_updatePosition>();
		KBPos2UE4Pos(pEventData->position, entityServerPos_);
		KBDir2UE4Dir(pEventData->direction, pEntity->direction);
		pEventData->entityID = pEntity->id();
		pEventData->moveSpeed = pEntity->velocity();
		OUROBOROS_EVENT_FIRE(OBEventTypes::updatePosition, pEventData);

		pEntity->onUpdateVolatileData();
	}
}

void OuroborosApp::Client_onUpdateBasePosXZ(float x, float z)
{
	entityServerPos_.X = x;
	entityServerPos_.Z = z;

	Entity* pEntity = player();
	if (pEntity && pEntity->isControlled())
	{
		pEntity->position.X = entityServerPos_.X;
		pEntity->position.Z = entityServerPos_.Z;

		UOBEventData_updatePosition* pEventData = NewObject<UOBEventData_updatePosition>();
		KBPos2UE4Pos(pEventData->position, entityServerPos_);
		KBDir2UE4Dir(pEventData->direction, pEntity->direction);
		pEventData->entityID = pEntity->id();
		pEventData->moveSpeed = pEntity->velocity();
		OUROBOROS_EVENT_FIRE(OBEventTypes::updatePosition, pEventData);

		pEntity->onUpdateVolatileData();
	}
}

void OuroborosApp::Client_onUpdateBaseDir(MemoryStream& stream)
{
	float yaw, pitch, roll;
	stream >> yaw >> pitch >> roll;

	Entity* pEntity = player();
	if (pEntity && pEntity->isControlled())
	{
		pEntity->direction.Set(roll, pitch, yaw);

		UOBEventData_set_direction* pEventData = NewObject<UOBEventData_set_direction>();
		KBDir2UE4Dir(pEventData->direction, pEntity->direction);
		pEventData->entityID = pEntity->id();
		OUROBOROS_EVENT_FIRE(OBEventTypes::set_direction, pEventData);

		pEntity->onUpdateVolatileData();
	}
}

void OuroborosApp::Client_onUpdateData(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	Entity** pEntityFind = entities_.Find(eid);

	if (!pEntityFind)
	{
		ERROR_MSG("OuroborosApp::Client_onUpdateData(): entity(%d) not found!", eid);
		return;
	}
}

void OuroborosApp::Client_onSetEntityPosAndDir(MemoryStream& stream)
{
	ENTITY_ID eid;
	stream >> eid;

	Entity** pEntityFind = entities_.Find(eid);

	if (!pEntityFind)
	{
		ERROR_MSG("OuroborosApp::Client_onSetEntityPosAndDir(): entity(%d) not found!", eid);
		return;
	}
	
	Entity& entity = *(*pEntityFind);

	FVector old_position = entity.position;
	FVector old_direction = entity.direction;

	entity.position.X = stream.read<float>();
	entity.position.Y = stream.read<float>();
	entity.position.Z = stream.read<float>();

	entity.direction.X = stream.read<float>();
	entity.direction.Y = stream.read<float>();
	entity.direction.Z = stream.read<float>();

	entity.entityLastLocalPos = entity.position;
	entity.entityLastLocalDir = entity.direction;

	entity.onDirectionChanged(old_direction);
	entity.onPositionChanged(old_position);	
}

void OuroborosApp::Client_onUpdateData_ypr(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	float y = stream.read<float>();
	float p = stream.read<float>();
	float r = stream.read<float>();

	_updateVolatileData(eid, OURO_FLT_MAX, OURO_FLT_MAX, OURO_FLT_MAX, y, p, r, -1, false);
}

void OuroborosApp::Client_onUpdateData_yp(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	float y = stream.read<float>();
	float p = stream.read<float>();

	_updateVolatileData(eid, OURO_FLT_MAX, OURO_FLT_MAX, OURO_FLT_MAX, y, p, OURO_FLT_MAX, -1, false);
}

void OuroborosApp::Client_onUpdateData_yr(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	float y = stream.read<float>();
	float r = stream.read<float>();

	_updateVolatileData(eid, OURO_FLT_MAX, OURO_FLT_MAX, OURO_FLT_MAX, y, OURO_FLT_MAX, r, -1, false);
}

void OuroborosApp::Client_onUpdateData_pr(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	float p = stream.read<float>();
	float r = stream.read<float>();

	_updateVolatileData(eid, OURO_FLT_MAX, OURO_FLT_MAX, OURO_FLT_MAX, OURO_FLT_MAX, p, r, -1, false);
}

void OuroborosApp::Client_onUpdateData_y(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	float y = stream.read<float>();

	_updateVolatileData(eid, OURO_FLT_MAX, OURO_FLT_MAX, OURO_FLT_MAX, y, OURO_FLT_MAX, OURO_FLT_MAX, -1, false);
}

void OuroborosApp::Client_onUpdateData_p(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	float p = stream.read<float>();

	_updateVolatileData(eid, OURO_FLT_MAX, OURO_FLT_MAX, OURO_FLT_MAX, OURO_FLT_MAX, p, OURO_FLT_MAX, -1, false);
}

void OuroborosApp::Client_onUpdateData_r(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	float r = stream.read<float>();

	_updateVolatileData(eid, OURO_FLT_MAX, OURO_FLT_MAX, OURO_FLT_MAX, OURO_FLT_MAX, OURO_FLT_MAX, r, -1, false);
}

void OuroborosApp::Client_onUpdateData_xz(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	float x = stream.read<float>();
	float z = stream.read<float>();

	_updateVolatileData(eid, x, OURO_FLT_MAX, z, OURO_FLT_MAX, OURO_FLT_MAX, OURO_FLT_MAX, 1, false);
}

void OuroborosApp::Client_onUpdateData_xz_ypr(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	float x = stream.read<float>();
	float z = stream.read<float>();

	float y = stream.read<float>();
	float p = stream.read<float>();
	float r = stream.read<float>();

	_updateVolatileData(eid, x, OURO_FLT_MAX, z, y, p, r, 1, false);
}

void OuroborosApp::Client_onUpdateData_xz_yp(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	float x = stream.read<float>();
	float z = stream.read<float>();

	float y = stream.read<float>();
	float p = stream.read<float>();

	_updateVolatileData(eid, x, OURO_FLT_MAX, z, y, p, OURO_FLT_MAX, 1, false);
}

void OuroborosApp::Client_onUpdateData_xz_yr(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	float x = stream.read<float>();
	float z = stream.read<float>();

	float y = stream.read<float>();
	float r = stream.read<float>();

	_updateVolatileData(eid, x, OURO_FLT_MAX, z, y, OURO_FLT_MAX, r, 1, false);
}

void OuroborosApp::Client_onUpdateData_xz_pr(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	float x = stream.read<float>();
	float z = stream.read<float>();

	float p = stream.read<float>();
	float r = stream.read<float>();

	_updateVolatileData(eid, x, OURO_FLT_MAX, z, OURO_FLT_MAX, p, r, 1, false);
}

void OuroborosApp::Client_onUpdateData_xz_y(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	float x = stream.read<float>();
	float z = stream.read<float>();

	float y = stream.read<float>();

	_updateVolatileData(eid, x, OURO_FLT_MAX, z, y, OURO_FLT_MAX, OURO_FLT_MAX, 1, false);
}

void OuroborosApp::Client_onUpdateData_xz_p(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	float x = stream.read<float>();
	float z = stream.read<float>();

	float p = stream.read<float>();

	_updateVolatileData(eid, x, OURO_FLT_MAX, z, OURO_FLT_MAX, p, OURO_FLT_MAX, 1, false);
}

void OuroborosApp::Client_onUpdateData_xz_r(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	float x = stream.read<float>();
	float z = stream.read<float>();

	float r = stream.read<float>();

	_updateVolatileData(eid, x, OURO_FLT_MAX, z, OURO_FLT_MAX, OURO_FLT_MAX, r, 1, false);
}

void OuroborosApp::Client_onUpdateData_xyz(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	float x = stream.read<float>();
	float y = stream.read<float>();
	float z = stream.read<float>();

	_updateVolatileData(eid, x, y, z, OURO_FLT_MAX, OURO_FLT_MAX, OURO_FLT_MAX, 0, false);
}

void OuroborosApp::Client_onUpdateData_xyz_ypr(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	float x = stream.read<float>();
	float y = stream.read<float>();
	float z = stream.read<float>();

	float yaw = stream.read<float>();
	float p = stream.read<float>();
	float r = stream.read<float>();

	_updateVolatileData(eid, x, y, z, yaw, p, r, 0, false);
}

void OuroborosApp::Client_onUpdateData_xyz_yp(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	float x = stream.read<float>();
	float y = stream.read<float>();
	float z = stream.read<float>();

	float yaw = stream.read<float>();
	float p = stream.read<float>();

	_updateVolatileData(eid, x, y, z, yaw, p, OURO_FLT_MAX, 0, false);
}

void OuroborosApp::Client_onUpdateData_xyz_yr(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	float x = stream.read<float>();
	float y = stream.read<float>();
	float z = stream.read<float>();

	float yaw = stream.read<float>();
	float r = stream.read<float>();

	_updateVolatileData(eid, x, y, z, yaw, OURO_FLT_MAX, r, 0, false);
}

void OuroborosApp::Client_onUpdateData_xyz_pr(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	float x = stream.read<float>();
	float y = stream.read<float>();
	float z = stream.read<float>();

	float p = stream.read<float>();
	float r = stream.read<float>();

	_updateVolatileData(eid, x, y, z, OURO_FLT_MAX, p, r, 0, false);
}

void OuroborosApp::Client_onUpdateData_xyz_y(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	float x = stream.read<float>();
	float y = stream.read<float>();
	float z = stream.read<float>();

	float yaw = stream.read<float>();

	_updateVolatileData(eid, x, y, z, yaw, OURO_FLT_MAX, OURO_FLT_MAX, 0, false);
}

void OuroborosApp::Client_onUpdateData_xyz_p(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	float x = stream.read<float>();
	float y = stream.read<float>();
	float z = stream.read<float>();

	float p = stream.read<float>();

	_updateVolatileData(eid, x, y, z, OURO_FLT_MAX, p, OURO_FLT_MAX, 0, false);
}

void OuroborosApp::Client_onUpdateData_xyz_r(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	float x = stream.read<float>();
	float y = stream.read<float>();
	float z = stream.read<float>();

	float r = stream.read<float>();

	_updateVolatileData(eid, x, y, z, OURO_FLT_MAX, OURO_FLT_MAX, r, 0, false);
}

void OuroborosApp::Client_onUpdateData_ypr_optimized(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	int8 y = stream.read<int8>();
	int8 p = stream.read<int8>();
	int8 r = stream.read<int8>();

	_updateVolatileData(eid, OURO_FLT_MAX, OURO_FLT_MAX, OURO_FLT_MAX, y, p, r, -1, true);
}

void OuroborosApp::Client_onUpdateData_yp_optimized(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	int8 y = stream.read<int8>();
	int8 p = stream.read<int8>();

	_updateVolatileData(eid, OURO_FLT_MAX, OURO_FLT_MAX, OURO_FLT_MAX, y, p, OURO_FLT_MAX, -1, true);
}

void OuroborosApp::Client_onUpdateData_yr_optimized(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	int8 y = stream.read<int8>();
	int8 r = stream.read<int8>();

	_updateVolatileData(eid, OURO_FLT_MAX, OURO_FLT_MAX, OURO_FLT_MAX, y, OURO_FLT_MAX, r, -1, true);
}

void OuroborosApp::Client_onUpdateData_pr_optimized(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	int8 p = stream.read<int8>();
	int8 r = stream.read<int8>();

	_updateVolatileData(eid, OURO_FLT_MAX, OURO_FLT_MAX, OURO_FLT_MAX, OURO_FLT_MAX, p, r, -1, true);
}

void OuroborosApp::Client_onUpdateData_y_optimized(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	int8 y = stream.read<int8>();

	_updateVolatileData(eid, OURO_FLT_MAX, OURO_FLT_MAX, OURO_FLT_MAX, y, OURO_FLT_MAX, OURO_FLT_MAX, -1, true);
}

void OuroborosApp::Client_onUpdateData_p_optimized(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	int8 p = stream.read<int8>();

	_updateVolatileData(eid, OURO_FLT_MAX, OURO_FLT_MAX, OURO_FLT_MAX, OURO_FLT_MAX, p, OURO_FLT_MAX, -1, true);
}

void OuroborosApp::Client_onUpdateData_r_optimized(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	int8 r = stream.read<int8>();

	_updateVolatileData(eid, OURO_FLT_MAX, OURO_FLT_MAX, OURO_FLT_MAX, OURO_FLT_MAX, OURO_FLT_MAX, r, -1, true);
}

void OuroborosApp::Client_onUpdateData_xz_optimized(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	FVector xz;
	stream.readPackXZ(xz.X, xz.Z);

	_updateVolatileData(eid, xz.X, OURO_FLT_MAX, xz.Z, OURO_FLT_MAX, OURO_FLT_MAX, OURO_FLT_MAX, 1, true);
}

void OuroborosApp::Client_onUpdateData_xz_ypr_optimized(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	FVector xz;
	stream.readPackXZ(xz.X, xz.Z);

	int8 y = stream.read<int8>();
	int8 p = stream.read<int8>();
	int8 r = stream.read<int8>();

	_updateVolatileData(eid, xz.X, OURO_FLT_MAX, xz.Z, y, p, r, 1, true);
}

void OuroborosApp::Client_onUpdateData_xz_yp_optimized(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	FVector xz;
	stream.readPackXZ(xz.X, xz.Z);

	int8 y = stream.read<int8>();
	int8 p = stream.read<int8>();

	_updateVolatileData(eid, xz.X, OURO_FLT_MAX, xz.Z, y, p, OURO_FLT_MAX, 1, true);
}

void OuroborosApp::Client_onUpdateData_xz_yr_optimized(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	FVector xz;
	stream.readPackXZ(xz.X, xz.Z);

	int8 y = stream.read<int8>();
	int8 r = stream.read<int8>();

	_updateVolatileData(eid, xz.X, OURO_FLT_MAX, xz.Z, y, OURO_FLT_MAX, r, 1, true);
}

void OuroborosApp::Client_onUpdateData_xz_pr_optimized(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	FVector xz;
	stream.readPackXZ(xz.X, xz.Z);

	int8 p = stream.read<int8>();
	int8 r = stream.read<int8>();

	_updateVolatileData(eid, xz.X, OURO_FLT_MAX, xz.Z, OURO_FLT_MAX, p, r, 1, true);
}

void OuroborosApp::Client_onUpdateData_xz_y_optimized(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	FVector xz;
	stream.readPackXZ(xz.X, xz.Z);

	int8 y = stream.read<int8>();

	_updateVolatileData(eid, xz.X, OURO_FLT_MAX, xz.Z, y, OURO_FLT_MAX, OURO_FLT_MAX, 1, true);
}

void OuroborosApp::Client_onUpdateData_xz_p_optimized(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	FVector xz;
	stream.readPackXZ(xz.X, xz.Z);

	int8 p = stream.read<int8>();

	_updateVolatileData(eid, xz.X, OURO_FLT_MAX, xz.Z, OURO_FLT_MAX, p, OURO_FLT_MAX, 1, true);
}

void OuroborosApp::Client_onUpdateData_xz_r_optimized(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	FVector xz;
	stream.readPackXZ(xz.X, xz.Z);

	int8 r = stream.read<int8>();

	_updateVolatileData(eid, xz.X, OURO_FLT_MAX, xz.Z, OURO_FLT_MAX, OURO_FLT_MAX, r, 1, true);
}

void OuroborosApp::Client_onUpdateData_xyz_optimized(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	FVector xz;
	stream.readPackXZ(xz.X, xz.Z);
	stream.readPackY(xz.Y);

	_updateVolatileData(eid, xz.X, xz.Y, xz.Z, OURO_FLT_MAX, OURO_FLT_MAX, OURO_FLT_MAX, 0, true);
}

void OuroborosApp::Client_onUpdateData_xyz_ypr_optimized(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	FVector xz;
	stream.readPackXZ(xz.X, xz.Z);
	stream.readPackY(xz.Y);

	int8 y = stream.read<int8>();
	int8 p = stream.read<int8>();
	int8 r = stream.read<int8>();

	_updateVolatileData(eid, xz.X, xz.Y, xz.Z, y, p, r, 0, true);
}

void OuroborosApp::Client_onUpdateData_xyz_yp_optimized(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	FVector xz;
	stream.readPackXZ(xz.X, xz.Z);
	stream.readPackY(xz.Y);

	int8 y = stream.read<int8>();
	int8 p = stream.read<int8>();

	_updateVolatileData(eid, xz.X, xz.Y, xz.Z, y, p, OURO_FLT_MAX, 0, true);
}

void OuroborosApp::Client_onUpdateData_xyz_yr_optimized(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	FVector xz;
	stream.readPackXZ(xz.X, xz.Z);
	stream.readPackY(xz.Y);

	int8 y = stream.read<int8>();
	int8 r = stream.read<int8>();

	_updateVolatileData(eid, xz.X, xz.Y, xz.Z, y, OURO_FLT_MAX, r, 0, true);
}

void OuroborosApp::Client_onUpdateData_xyz_pr_optimized(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	FVector xz;
	stream.readPackXZ(xz.X, xz.Z);
	stream.readPackY(xz.Y);

	int8 p = stream.read<int8>();
	int8 r = stream.read<int8>();

	_updateVolatileData(eid, xz.X, xz.Y, xz.Z, OURO_FLT_MAX, p, r, 0, true);
}

void OuroborosApp::Client_onUpdateData_xyz_y_optimized(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	FVector xz;
	stream.readPackXZ(xz.X, xz.Z);
	stream.readPackY(xz.Y);

	int8 y = stream.read<int8>();

	_updateVolatileData(eid, xz.X, xz.Y, xz.Z, y, OURO_FLT_MAX, OURO_FLT_MAX, 0, true);
}

void OuroborosApp::Client_onUpdateData_xyz_p_optimized(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	FVector xz;
	stream.readPackXZ(xz.X, xz.Z);
	stream.readPackY(xz.Y);

	int8 p = stream.read<int8>();

	_updateVolatileData(eid, xz.X, xz.Y, xz.Z, OURO_FLT_MAX, p, OURO_FLT_MAX, 0, true);
}

void OuroborosApp::Client_onUpdateData_xyz_r_optimized(MemoryStream& stream)
{
	ENTITY_ID eid = getViewEntityIDFromStream(stream);

	FVector xz;
	stream.readPackXZ(xz.X, xz.Z);
	stream.readPackY(xz.Y);

	int8 r = stream.read<int8>();

	_updateVolatileData(eid, xz.X, xz.Y, xz.Z, OURO_FLT_MAX, OURO_FLT_MAX, r, 0, true);
}

void OuroborosApp::_updateVolatileData(ENTITY_ID entityID, float x, float y, float z, float yaw, float pitch, float roll, int8 isOnGround, bool isOptimized)
{
	Entity** pEntityFind = entities_.Find(entityID);

	if (!pEntityFind)
	{
		// If 0 and the client's previous step is to re-login or reconnect and the server entity is online during the disconnection
		// This error can be ignored, because cellapp may always send synchronization messages to baseapp, not waiting when the client reconnects
		// When the server initialization step starts, it receives the synchronization information, and an error occurs here.
		ERROR_MSG("OuroborosApp::_updateVolatileData(): entity(%d) not found!", entityID);
		return;
	}

	Entity& entity = *(*pEntityFind);

	// Less than 0 is not set
	if (isOnGround >= 0)
	{
		entity.isOnGround(isOnGround > 0);
	}

	bool changeDirection = false;

	if (roll != OURO_FLT_MAX)
	{
		changeDirection = true;
		entity.direction.X = isOptimized ? int82angle((int8)roll, false) : roll;
	}

	if (pitch != OURO_FLT_MAX)
	{
		changeDirection = true;
		entity.direction.Y = isOptimized ? int82angle((int8)pitch, false) : pitch;
	}

	if (yaw != OURO_FLT_MAX)
	{
		changeDirection = true;
		entity.direction.Z = isOptimized ? int82angle((int8)yaw, false) : yaw;
	}

	bool done = false;
	if (changeDirection == true)
	{
		UOBEventData_set_direction* pEventData = NewObject<UOBEventData_set_direction>();
		KBDir2UE4Dir(pEventData->direction, entity.direction);
		pEventData->entityID = entity.id();
		OUROBOROS_EVENT_FIRE(OBEventTypes::set_direction, pEventData);

		done = true;
	}

	bool positionChanged = x != OURO_FLT_MAX || y != OURO_FLT_MAX || z != OURO_FLT_MAX;
	if (x == OURO_FLT_MAX) x = isOptimized ? 0.0f : entity.position.X;
	if (y == OURO_FLT_MAX) y = isOptimized ? 0.0f : entity.position.Y;
	if (z == OURO_FLT_MAX) z = isOptimized ? 0.0f : entity.position.Z;
	            
	if (positionChanged)
	{
		entity.position = isOptimized ? FVector(x + entityServerPos_.X, y + entityServerPos_.Y, z + entityServerPos_.Z) : FVector(x, y, z);
		done = true;

		UOBEventData_updatePosition* pEventData = NewObject<UOBEventData_updatePosition>();
		KBPos2UE4Pos(pEventData->position, entity.position);
		KBDir2UE4Dir(pEventData->direction, entity.direction);
		pEventData->entityID = entity.id();
		pEventData->moveSpeed = entity.velocity();
		pEventData->isOnGround = entity.isOnGround();
		OUROBOROS_EVENT_FIRE(OBEventTypes::updatePosition, pEventData);
	}

	if (done)
		entity.onUpdateVolatileData();
}

}