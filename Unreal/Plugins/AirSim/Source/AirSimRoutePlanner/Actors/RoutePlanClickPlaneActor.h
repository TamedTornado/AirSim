// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoutePlanClickPlaneActor.generated.h"

class UBoxComponent;

UCLASS()
class AIRSIMROUTEPLANNER_API ARoutePlanClickPlaneActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoutePlanClickPlaneActor();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Collision")
	UBoxComponent*			ClickPlaneVolume;

	// What click trace channel are we using? Taken from AirSim Plugin Settings
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Transient, Category = "Collision Handling")
	TEnumAsByte<ECollisionChannel>				ClickTraceChannel;


};
