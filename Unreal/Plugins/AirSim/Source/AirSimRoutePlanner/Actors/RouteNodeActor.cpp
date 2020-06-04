// Fill out your copyright notice in the Description page of Project Settings.


#include "RouteNodeActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "AirSimPluginSettings.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "RouteEdgeActor.h"
#include "RoutePlanningOwner.h"
#include "Data/RouteNode.h"
#include "Data/RoutePlan.h"

// Sets default values
ARouteNodeActor::ARouteNodeActor()
{
	TEnumAsByte<ECollisionChannel> ClickTraceChannel = GetDefault<UAirSimPluginSettings>()->ClickTraceChannel;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>("RouteNodeRoot");
	RootComponent = RootSceneComponent;

	NodeCoin = CreateDefaultSubobject<UStaticMeshComponent>("NodeCoinMesh");
	NodeCoin->SetCollisionResponseToAllChannels(ECR_Ignore);
	NodeCoin->SetCollisionResponseToChannel(ClickTraceChannel, ECR_Block);
	NodeCoin->SetupAttachment(RootComponent);
}

void ARouteNodeActor::SetRouteNode(URouteNode* NewRouteNode)
{
	RouteNodeData = NewRouteNode;

	if (RouteNodeData)
	{
		RouteNodeData->OnNodeDetailsChanged.AddDynamic(this, &ARouteNodeActor::HandleRouteNodeDetailsChanged);
	}
}

void ARouteNodeActor::HandleRouteNodeDetailsChanged(URouteNode* nodeChanged)
{
	if (RouteNodeData)
	{
		if (GetActorLocation() != RouteNodeData->Coord)
		{
			SetActorLocation(RouteNodeData->Coord);
		}

		//TODO: More!

		UpdateEdges();

	}
}

void ARouteNodeActor::UpdateEdges()
{
	if (EdgeToParent)
		EdgeToParent->UpdateTransform();

	if (EdgeToChild)
		EdgeToChild->UpdateTransform();
}

void ARouteNodeActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

}

void ARouteNodeActor::BeginPlay()
{
	Super::BeginPlay();

	if (CoinFaceMaterial)
	{
		CoinFaceMaterialInst = UMaterialInstanceDynamic::Create(CoinFaceMaterial, this, "CoinFaceMaterialInstance");

		CoinFaceMaterialInst->SetVectorParameterValue("SelectedColor", SelectedColor);
		CoinFaceMaterialInst->SetVectorParameterValue("HoveredColor", HoveredColor);

		CoinFaceMaterialInst->SetScalarParameterValue("Selected",0);
		CoinFaceMaterialInst->SetScalarParameterValue("Hovered",0);
	}

	if (CoinEdgeMaterial)
	{
		CoinEdgeMaterialInst = UMaterialInstanceDynamic::Create(CoinEdgeMaterial, this, "CoinEdgeMaterialInstance");

		CoinEdgeMaterialInst->SetVectorParameterValue("SelectedColor", SelectedColor);
		CoinEdgeMaterialInst->SetVectorParameterValue("HoveredColor", HoveredColor);

		CoinEdgeMaterialInst->SetScalarParameterValue("Selected", 0);
		CoinEdgeMaterialInst->SetScalarParameterValue("Hovered", 0);
	}

	if (NodeCoin)
	{
		if (CoinFaceMaterialInst)
			NodeCoin->SetMaterial(0, CoinFaceMaterialInst);

		if (CoinEdgeMaterialInst)
			NodeCoin->SetMaterial(1, CoinEdgeMaterialInst);
	}

	UpdateVisuals();
}

void ARouteNodeActor::UpdateVisuals()
{
	Super::UpdateVisuals();

	if (CoinFaceMaterialInst)
	{
		CoinFaceMaterialInst->SetScalarParameterValue("Selected", bSelected ? 1.0f : 0.0f);
		CoinFaceMaterialInst->SetScalarParameterValue("Hovered", bHoveredOver ? 1.0f : 0.0f);


		UTexture2D* iconTexture = BlankIcon;

		if (RouteNodeData->IsLastNode())
		{
			switch (OwningRoutePlanner->CurrentRoutePlan->LastNodeBehaviour)
			{
			case ELastNodeBehaviour::LandAtLastNode:
				iconTexture = LandIcon;
				break;
			case ELastNodeBehaviour::LandAtStartNode:
			case ELastNodeBehaviour::ReturnToStartAndLoop:
			default:
				iconTexture = BlankIcon;
				break;
			}
		} else if (RouteNodeData->IsFirstNode())
		{
			switch (OwningRoutePlanner->CurrentRoutePlan->LastNodeBehaviour)
			{
			case ELastNodeBehaviour::LandAtStartNode:
				iconTexture = TakeOffAndLandIcon;
				break;
			case ELastNodeBehaviour::ReturnToStartAndLoop:
				iconTexture = TakeOffAndLoopIcon;
			case ELastNodeBehaviour::LandAtLastNode:
			default:
				iconTexture = TakeOffIcon;
				break;
			}
		}

		//TODO: Numbered!

		CoinFaceMaterialInst->SetTextureParameterValue("IconTexture", iconTexture);
	}

	if (CoinEdgeMaterialInst)
	{
		CoinEdgeMaterialInst->SetScalarParameterValue("Selected", bSelected ? 1.0f : 0.0f);
		CoinEdgeMaterialInst->SetScalarParameterValue("Hovered", bHoveredOver ? 1.0f : 0.0f);
	}
}
