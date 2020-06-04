// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapSelectableActor.h"
#include "RouteNodeActor.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class ARouteEdgeActor;
class ARoutePlanningOwnerPawn;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class URouteNode;

UENUM(BlueprintType)
enum class ERouteNodeType : uint8
{
	Normal, // Normal nodes are either blank or numbered
	Takeoff,
	Landing,
	TakeoffAndLanding,
	TakeoffAndLoop
};

UCLASS()
class AIRSIMROUTEPLANNER_API ARouteNodeActor : public AMapSelectableActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARouteNodeActor();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Route Node")
	USceneComponent*						RootSceneComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Route Node")
	UStaticMeshComponent*					NodeCoin;

	UPROPERTY(BlueprintReadOnly, Category = "Route Node")
	URouteNode*								RouteNodeData;

	UFUNCTION(BlueprintCallable, Category = "Route Node Data")
	void									SetRouteNode(URouteNode* NewRouteNode);

	// TODO: More components for pointers, etc

	// The material instance that we'll turn into a dynamic material instance for this node
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Materials")
	UMaterialInterface*						CoinFaceMaterial;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Materials")
	UMaterialInterface*						CoinEdgeMaterial;

	UPROPERTY(BlueprintReadOnly, Category = "Route Node")
	UMaterialInstanceDynamic*				CoinFaceMaterialInst;

	UPROPERTY(BlueprintReadOnly, Category = "Route Node")
	UMaterialInstanceDynamic*				CoinEdgeMaterialInst;

	// The blank, black texture when there's no icon on the node
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Icons")
	UTexture2D*								BlankIcon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Icons")
	UTexture2D*								LandIcon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Icons")
	UTexture2D*								TakeOffIcon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Icons")
	UTexture2D*								TakeOffAndLandIcon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Icons")
	UTexture2D*								TakeOffAndLoopIcon;


	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Colors")
	FLinearColor							NormalColor = FLinearColor::White;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Colors")
	FLinearColor							HoveredColor = FLinearColor::Blue;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Colors")
	FLinearColor							SelectedColor = FLinearColor::Green;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Route Node")
	ERouteNodeType							NodeType = ERouteNodeType::Normal;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Route Node")
	ARouteEdgeActor*						EdgeToParent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Route Node")
	ARouteEdgeActor*						EdgeToChild;

	// The owner of this node
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Route Node")
	ARoutePlanningOwnerPawn*				OwningRoutePlanner;

	UFUNCTION(BlueprintCallable, Category = "Route Node")
	void									UpdateEdges();

public:

	void OnConstruction(const FTransform& Transform) override;


	void UpdateVisuals() override;

protected:
	void BeginPlay() override;



	UFUNCTION()
	void HandleRouteNodeDetailsChanged(URouteNode* nodeChanged);
};
