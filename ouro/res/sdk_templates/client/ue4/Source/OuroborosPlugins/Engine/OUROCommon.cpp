// Fill out your copyright notice in the Description page of Project Settings.

#include "OUROCommon.h"
#include "KBDebug.h"

namespace Ouroboros
{

DEFINE_LOG_CATEGORY(LogOuroboros);

}

double getTimeSeconds()
{
	return FPlatformTime::Seconds();
}


// Sets default values
AOUROCommon::AOUROCommon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

