// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object.h"
#include "OUROCommon.h"
#include "OBEvent.h"
#include "Tickable.h"
#include "OUROTicker.generated.h"
/**
 * 
 */
UCLASS()
class OUROBOROSPLUGINS_API UOUROTicker : public UObject, public FTickableGameObject
{
	GENERATED_BODY()
public:
	UOUROTicker();
	~UOUROTicker();

	UWorld* GetWorld() const override;

	virtual bool IsTickableWhenPaused() const override;	
	virtual bool IsTickableInEditor() const override;	
	virtual UWorld* GetTickableGameObjectWorld() const override;	

	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;

	void OnEndPIE(const bool);
};
