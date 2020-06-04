// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapSelectableActor.h"
#include "RouteEdgeActor.generated.h"

class UParticleSystem;
class UCapsuleComponent;
class ARouteNodeActor;
class UParticleSystemComponent;

UCLASS()
class AIRSIMROUTEPLANNER_API ARouteEdgeActor : public AMapSelectableActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARouteEdgeActor();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Route Edge")
	USceneComponent*			RootSceneComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Collision")
	UCapsuleComponent*			BeamCollision;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Route Edge")
	UParticleSystemComponent*	BeamComponent;

	// The particle system used for the beam
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Laser FX")
	UParticleSystem*			LaserParticleSystem;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Nodes")
	ARouteNodeActor*			ParentNode;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Nodes")
	ARouteNodeActor*			TargetNode;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Nodes")
	float						CurrentScaling = 1.0f;

	// When the nodes move or the scale changes, update the scale, position and rotation of the collision as well as beam start and end points
	UFUNCTION(BlueprintCallable, Category = "Collision")
	void						UpdateTransform();
protected:
	void BeginPlay() override;

};
