// Fill out your copyright notice in the Description page of Project Settings.

#include "KBETicker.h"
#include "Engine.h"
#include "KBDebug.h"
#include "Entity.h"
#include "Ouroboros.h"

#if WITH_EDITOR
#include "Editor.h"
#endif

UKBETicker::UKBETicker()
{
#if WITH_EDITOR
	FEditorDelegates::EndPIE.AddUObject(this, &UKBETicker::OnEndPIE);
#endif
}

UKBETicker::~UKBETicker()
{
}

void UKBETicker::Tick(float DeltaTime)
{
	OBEvent::processOutEvents();

	APawn* ue4_player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	Ouroboros::Entity* ouro_player = Ouroboros::OuroborosApp::getSingleton().player();

	// Each tick writes the player coordinates of UE4 to the player entity coordinates in the KBE plugin, and the plugin will periodically synchronize to the server.
	if (ouro_player && ue4_player)
	{
		UE4Pos2KBPos(ouro_player->position, ue4_player->GetActorLocation());
		UE4Dir2KBDir(ouro_player->direction, ue4_player->GetActorRotation());

		ouro_player->isOnGround(ue4_player->GetMovementComponent() && ue4_player->GetMovementComponent()->IsMovingOnGround());
	}

	Ouroboros::OuroborosApp::getSingleton().process();
}

bool UKBETicker::IsTickable() const
{
	return true;
}

TStatId UKBETicker::GetStatId() const
{
	return TStatId();
}

UWorld* UKBETicker::GetWorld() const
{ 
	UWorld* World = (GetOuter() != nullptr) ? GetOuter()->GetWorld() : GWorld;	
	if (World == nullptr)
	{
		World = GWorld;
	}
	return World; 
}

bool UKBETicker::IsTickableWhenPaused() const
{
	return false;
}

bool UKBETicker::IsTickableInEditor() const
{
	return false;
}

UWorld* UKBETicker::GetTickableGameObjectWorld() const
{
	return GetWorld();
}

void UKBETicker::OnEndPIE(const bool data)
{
#if WITH_EDITOR
	Ouroboros::OuroborosApp::destroyOuroborosApp();
#endif
}