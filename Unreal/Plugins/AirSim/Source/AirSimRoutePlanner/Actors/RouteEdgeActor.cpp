// Fill out your copyright notice in the Description page of Project Settings.


#include "RouteEdgeActor.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "AirSimPluginSettings.h"
#include "RouteNodeActor.h"

// Sets default values
ARouteEdgeActor::ARouteEdgeActor()
{
	TEnumAsByte<ECollisionChannel> ClickTraceChannel = GetDefault<UAirSimPluginSettings>()->ClickTraceChannel;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>("RouteNodeRoot");
	RootComponent = RootSceneComponent;

	BeamCollision = CreateDefaultSubobject<UCapsuleComponent>("BeamCollision");
	BeamCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	BeamCollision->SetCollisionResponseToChannel(ClickTraceChannel, ECR_Block);
	BeamCollision->SetHiddenInGame(false);
	BeamCollision->SetupAttachment(RootComponent);

	BeamComponent = CreateDefaultSubobject<UParticleSystemComponent>("BeamComponent");
	BeamComponent->SetupAttachment(RootComponent);
}

void ARouteEdgeActor::UpdateTransform()
{
	// Get the line between the two route nodes, if they exist

	if (ParentNode && TargetNode)
	{
		FVector edgeVector = TargetNode->GetActorLocation() - ParentNode->GetActorLocation();
		float distance = edgeVector.Size();

		edgeVector.Normalize();

		// Put this actor at the midpoint between the two nodes
		FVector newLoc = ParentNode->GetActorLocation() + (edgeVector * (distance / 2.0f));

		SetActorLocation(newLoc);

		// Point this actor toward the target node
		SetActorRotation(edgeVector.ToOrientationRotator());

		// Scale the capsule height 
		// TODO: Back it off so capsule doesn't overlap with scaled nodes
		BeamCollision->SetCapsuleHalfHeight(distance/2.0f - 50.0f * CurrentScaling);

		// Default to 0.5m, but scale that with the nodes
		BeamCollision->SetCapsuleRadius(25.0f);

		// TODO: Beam stuff
		FVector startLoc = ParentNode->GetActorLocation()+edgeVector * (50.0f * CurrentScaling);
		FVector endLoc = TargetNode->GetActorLocation()-edgeVector * (50.0f * CurrentScaling);


		BeamComponent->SetBeamSourcePoint(0, startLoc,0);
		BeamComponent->SetBeamEndPoint(0, endLoc);
	}
}

void ARouteEdgeActor::BeginPlay()
{
	Super::BeginPlay();
}
