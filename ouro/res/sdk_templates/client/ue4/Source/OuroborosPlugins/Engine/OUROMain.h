// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "OUROCommon.h"
#include "OBEvent.h"
#include "ClientSDKUpdater.h"
#include "ClientSDKUpdateUI.h"
#include "ShowPromptMessageUI.h"
#include "Components/ActorComponent.h"
#include "OUROMain.generated.h"

/*
Can be understood as the plugin's entry module
Install the event (installEvents) that needs to be listened to in this entry, and initialize Ouroboros (initOuroboros)
*/
class OuroborosApp;


UCLASS(ClassGroup = "Ouroboros", blueprintable, editinlinenew, hidecategories = (Object, LOD, Lighting, TextureStreaming), meta = (DisplayName = "Ouroboros Main", BlueprintSpawnableComponent))
class OUROBOROSPLUGINS_API UOUROMain : public UActorComponent
{
	GENERATED_UCLASS_BODY()

public:	
	// Sets default values for this component's properties
	UOUROMain();

	/**
	* Initializes the component.  Occurs at level startup. This is before BeginPlay (Actor or Component).
	* All Components in the level will be Initialized on load before any Actor/Component gets BeginPlay
	* Requires component to be registered, and bWantsInitializeComponent to be true.
	*/
	virtual void InitializeComponent() override;

	// Called when the game starts
	virtual void BeginPlay() override;

	/**
	* Ends gameplay for this component.
	* Called from AActor::EndPlay only if bHasBegunPlay is true
	*/
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	* Handle this component being Uninitialized.
	* Called from AActor::EndPlay only if bHasBeenInitialized is true
	*/
	virtual void UninitializeComponent() override;

	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;


	void installEvents();
	void deregisterEvents();

	void onVersionNotMatch(const UOBEventData* pEventData);
	void onScriptVersionNotMatch(const UOBEventData* pEventData);

	bool isUpdateSDK();
	void downloadSDKFromServer();
	void onDownloadSDK(const UOBEventData* pEventData);
	void onImportClientSDKSuccessfully(const UOBEventData* pEventData);

	void handVersionNotMatch();
	void showPromptMessageOfCompile();

	UFUNCTION(BlueprintCallable, Category = "Ouroboros")
	static FString getClientVersion();

	UFUNCTION(BlueprintCallable, Category = "Ouroboros")
	static FString getClientScriptVersion();

	UFUNCTION(BlueprintCallable, Category = "Ouroboros")
	static FString getServerVersion();

	UFUNCTION(BlueprintCallable, Category = "Ouroboros")
	static FString getServerScriptVersion();

	/*
		The client belongs to a functional component in the OURO framework, where the get will be fixed back to the client
	*/
	UFUNCTION(BlueprintCallable, Category = "Ouroboros")
	static FString getComponentName();

	/**
		Active call is required when the program is closed, and Ouroboros is completely destroyed.
	*/
	UFUNCTION(BlueprintCallable, Category = "Ouroboros")
	bool destroyOuroboros();

	UFUNCTION(BlueprintCallable, Category = "Ouroboros")
	bool login(FString username, FString password, TArray<uint8> datas);

	/*
				Create an account
	*/
	UFUNCTION(BlueprintCallable, Category = "Ouroboros")
	bool createAccount(FString username, FString password, const TArray<uint8>& datas);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	FString ip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int port;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	EKCLIENT_TYPE clientType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	NETWORK_ENCRYPT_TYPE networkEncryptType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int syncPlayerMS;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	bool useAliasEntityID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	bool isOnInitCallPropertysSetMethods;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	bool forceDisableUDP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int serverHeartbeatTick;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int TCP_SEND_BUFFER_MAX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int TCP_RECV_BUFFER_MAX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int UDP_SEND_BUFFER_MAX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	int UDP_RECV_BUFFER_MAX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ouroboros)
	bool automaticallyUpdateSDK;

	Ouroboros::ClientSDKUpdater* pUpdaterObj;

	static bool hasUpdateSDK;
	
	TSharedPtr<class SClientSDKUpdateUI> ClientSDKUpdateUI;
	TSharedPtr<class SShowPromptMessageUI> ShowPromptMessageUI;

};
