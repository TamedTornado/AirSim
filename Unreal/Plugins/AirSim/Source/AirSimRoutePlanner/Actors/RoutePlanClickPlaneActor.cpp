// Fill out your copyright notice in the Description page of Project Settings.


#include "RoutePlanClickPlaneActor.h"
#include "Engine/EngineTypes.h"
#include "Components/BoxComponent.h"
#include "AirSimPluginSettings.h"

// Sets default values
ARoutePlanClickPlaneActor::ARoutePlanClickPlaneActor()
{
 	ClickPlaneVolume = CreateDefaultSubobject<UBoxComponent>("ClickPlaneVolume");
	ClickPlaneVolume->SetBoxExtent(FVector(1000.0f, 1000.0f, 100.0f));
	RootComponent = ClickPlaneVolume;

	ClickTraceChannel = GetDefault<UAirSimPluginSettings>()->ClickTraceChannel;

	//TODO: Change to use plugin settings!
	ClickPlaneVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	ClickPlaneVolume->SetCollisionResponseToChannel(ClickTraceChannel, ECR_Block);
}
