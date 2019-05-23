// Fill out your copyright notice in the Description page of Project Settings.

#include "OUROMain.h"
#include "Ouroboros.h"
#include "OuroborosArgs.h"
#include "MemoryStream.h"
#include "Bundle.h"
#include "Engine.h"
#include "KBDebug.h"
#include "Entity.h"

bool UOUROMain::hasUpdateSDK = false;

// Sets default values for this component's properties
UOUROMain::UOUROMain(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;

	// ...

	ip = TEXT("127.0.0.1");
	port = @{OURO_LOGIN_PORT};
	syncPlayerMS = 1000 / @{OURO_UPDATEHZ};
	useAliasEntityID = @{OURO_USE_ALIAS_ENTITYID};
	isOnInitCallPropertysSetMethods = true;
	forceDisableUDP = false;
	clientType = EKCLIENT_TYPE::CLIENT_TYPE_WIN;
	networkEncryptType = NETWORK_ENCRYPT_TYPE::ENCRYPT_TYPE_NONE;
	serverHeartbeatTick = @{OURO_SERVER_EXTERNAL_TIMEOUT};
	TCP_SEND_BUFFER_MAX = TCP_PACKET_MAX;
	TCP_RECV_BUFFER_MAX = TCP_PACKET_MAX;
	UDP_SEND_BUFFER_MAX = 128;
	UDP_RECV_BUFFER_MAX = 128;

	pUpdaterObj = NULL;
	automaticallyUpdateSDK = true;
}

void UOUROMain::InitializeComponent()
{
	Super::InitializeComponent();
}

void UOUROMain::UninitializeComponent()
{
	Super::UninitializeComponent();
}

// Called when the game starts
void UOUROMain::BeginPlay()
{
	Super::BeginPlay();
	Ouroboros::OuroborosArgs* pArgs = new Ouroboros::OuroborosArgs();
	pArgs->ip = ip;
	pArgs->port = port;
	pArgs->syncPlayerMS = syncPlayerMS;
	pArgs->useAliasEntityID = useAliasEntityID;
	pArgs->isOnInitCallPropertysSetMethods = isOnInitCallPropertysSetMethods;
	pArgs->forceDisableUDP = forceDisableUDP;
	pArgs->clientType = clientType;
	pArgs->networkEncryptType = networkEncryptType;
	pArgs->serverHeartbeatTick = serverHeartbeatTick / 2;
	pArgs->TCP_SEND_BUFFER_MAX = TCP_SEND_BUFFER_MAX;
	pArgs->TCP_RECV_BUFFER_MAX = TCP_RECV_BUFFER_MAX;
	pArgs->UDP_SEND_BUFFER_MAX = UDP_SEND_BUFFER_MAX;
	pArgs->UDP_RECV_BUFFER_MAX = UDP_RECV_BUFFER_MAX;

	if(!Ouroboros::OuroborosApp::getSingleton().initialize(pArgs))
		delete pArgs;

	installEvents();
}

void UOUROMain::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (pUpdaterObj != nullptr)
	{
		delete pUpdaterObj;
		pUpdaterObj = nullptr;
	}

	ClientSDKUpdateUI.Reset();
	deregisterEvents();
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void UOUROMain::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	
}

void UOUROMain::installEvents()
{
	OUROBOROS_REGISTER_EVENT(Ouroboros::OBEventTypes::onScriptVersionNotMatch, onScriptVersionNotMatch);
	OUROBOROS_REGISTER_EVENT(Ouroboros::OBEventTypes::onVersionNotMatch, onVersionNotMatch);
	OUROBOROS_REGISTER_EVENT(Ouroboros::OBEventTypes::onImportClientSDKSuccessfully, onImportClientSDKSuccessfully);
	OUROBOROS_REGISTER_EVENT(Ouroboros::OBEventTypes::onDownloadSDK, onDownloadSDK);
}

void UOUROMain::deregisterEvents()
{
	OUROBOROS_DEREGISTER_EVENT(Ouroboros::OBEventTypes::onScriptVersionNotMatch);
	OUROBOROS_DEREGISTER_EVENT(Ouroboros::OBEventTypes::onVersionNotMatch);
	OUROBOROS_DEREGISTER_EVENT(Ouroboros::OBEventTypes::onImportClientSDKSuccessfully);
	OUROBOROS_DEREGISTER_EVENT(Ouroboros::OBEventTypes::onDownloadSDK);
}

void UOUROMain::onVersionNotMatch(const UOBEventData* pEventData)
{
	handVersionNotMatch();
}

void UOUROMain::onScriptVersionNotMatch(const UOBEventData* pEventData)
{
	handVersionNotMatch();
}

bool UOUROMain::isUpdateSDK()
{
#if WITH_EDITOR
	return automaticallyUpdateSDK;
#endif

	return false;
}

void UOUROMain::downloadSDKFromServer()
{
	if (GEngine->IsValidLowLevel())
	{
		GEngine->GameViewport->RemoveAllViewportWidgets();
	}

	if (isUpdateSDK())
	{
		ClientSDKUpdateUI = SNew(SClientSDKUpdateUI);

		if (GEngine->IsValidLowLevel())
		{
			GEngine->GameViewport->AddViewportWidgetContent(SNew(SWeakWidget).PossiblyNullContent(ClientSDKUpdateUI.ToSharedRef()));
		}

		if (ClientSDKUpdateUI.IsValid())
		{
			ClientSDKUpdateUI->SetVisibility(EVisibility::Visible);
		}

		hasUpdateSDK = true;
	}
}

void UOUROMain::onImportClientSDKSuccessfully(const UOBEventData* pEventData)
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true);
}

void UOUROMain::onDownloadSDK(const UOBEventData* pEventData)
{
	ClientSDKUpdateUI.Reset();
	if (GEngine->IsValidLowLevel())
	{
		GEngine->GameViewport->RemoveAllViewportWidgets();
	}

	const UOBEventData_onDownloadSDK* pData = Cast<UOBEventData_onDownloadSDK>(pEventData);
	if(pData->isDownload)
	{
		if (pUpdaterObj == nullptr)
		{
			pUpdaterObj = new Ouroboros::ClientSDKUpdater();
		}

		pUpdaterObj->downloadSDKFromServer();
	}
	else
	{
		if (pUpdaterObj != nullptr)
		{
			delete pUpdaterObj;
			pUpdaterObj = nullptr;
		}
	}
}

FString UOUROMain::getClientVersion()
{
	if (!Ouroboros::OuroborosApp::getSingleton().isInitialized())
		return TEXT("");

	return Ouroboros::OuroborosApp::getSingleton().clientVersion();
}

FString UOUROMain::getClientScriptVersion()
{
	if (!Ouroboros::OuroborosApp::getSingleton().isInitialized())
		return TEXT("");

	return Ouroboros::OuroborosApp::getSingleton().clientScriptVersion();
}

FString UOUROMain::getServerVersion()
{
	if (!Ouroboros::OuroborosApp::getSingleton().isInitialized())
		return TEXT("");

	return Ouroboros::OuroborosApp::getSingleton().serverVersion();
}

FString UOUROMain::getServerScriptVersion()
{
	if (!Ouroboros::OuroborosApp::getSingleton().isInitialized())
		return TEXT("");

	return Ouroboros::OuroborosApp::getSingleton().serverScriptVersion();
}

FString UOUROMain::getComponentName()
{
	if (!Ouroboros::OuroborosApp::getSingleton().isInitialized())
		return TEXT("");

	return Ouroboros::OuroborosApp::getSingleton().component();
}

bool UOUROMain::destroyOuroboros()
{
	if (!Ouroboros::OuroborosApp::getSingleton().isInitialized())
		return false;

	Ouroboros::OuroborosApp::getSingleton().destroy();
	OUROBOROS_EVENT_CLEAR();
	return true;
}

bool UOUROMain::login(FString username, FString password, TArray<uint8> datas)
{
	if (!Ouroboros::OuroborosApp::getSingleton().isInitialized())
	{
		return false;
	}

	Ouroboros::OuroborosApp::getSingleton().reset();

	UOBEventData_login* pEventData = NewObject<UOBEventData_login>();
	pEventData->username = username;
	pEventData->password = password;
	pEventData->datas = datas;
	OUROBOROS_EVENT_FIRE(Ouroboros::OBEventTypes::login, pEventData);
	return true;
}

bool UOUROMain::createAccount(FString username, FString password, const TArray<uint8>& datas)
{
	if (!Ouroboros::OuroborosApp::getSingleton().isInitialized())
	{
		return false;
	}

	Ouroboros::OuroborosApp::getSingleton().reset();

	UOBEventData_createAccount* pEventData = NewObject<UOBEventData_createAccount>();
	pEventData->username = username;
	pEventData->password = password;
	pEventData->datas = datas;
	OUROBOROS_EVENT_FIRE(Ouroboros::OBEventTypes::createAccount, pEventData);
	return true;
}

void UOUROMain::handVersionNotMatch()
{
	if (hasUpdateSDK) 
	{
		showPromptMessageOfCompile();
	}
	else
	{
		downloadSDKFromServer();
	}
}

void UOUROMain::showPromptMessageOfCompile()
{
	if (GEngine->IsValidLowLevel())
	{
		GEngine->GameViewport->RemoveAllViewportWidgets();
	}
	
	if (hasUpdateSDK) 
	{
		ShowPromptMessageUI = SNew(SShowPromptMessageUI);

		if (GEngine->IsValidLowLevel())
		{
			GEngine->GameViewport->AddViewportWidgetContent(SNew(SWeakWidget).PossiblyNullContent(ShowPromptMessageUI.ToSharedRef()));
		}

		if (ShowPromptMessageUI.IsValid())
		{
			ShowPromptMessageUI->SetVisibility(EVisibility::Visible);
		}
	}
}