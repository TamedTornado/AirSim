#pragma once

#include "RoutePlan.generated.h"

class URouteNode;


UENUM(BlueprintType)
enum class ELastNodeBehaviour : uint8
{
	LandAtLastNode = 0,
	LandAtStartNode = 1,
	ReturnToStartAndLoop = 2
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRouteNodeEventSig, URouteNode*, RouteNode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLastNodeBehaviourEventSig);

/*
	Data representation of a route plan for a drone or drones
*/
UCLASS(Blueprintable, BlueprintType)
class AIRSIMROUTEPLANNER_API URoutePlan : public UObject
{
	GENERATED_BODY()
public:

	// Friendly name of the route plan, saved with the plan
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Route Node", BlueprintSetter=SetRoutePlanName)
	FString					RoutePlanName = "New Route Plan";

	UFUNCTION(BlueprintCallable, Category = "Route Node")
	void					SetRoutePlanName(FString newName);

	// Filename of the route plan set on save and load - blank for a new plan
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Route Node", Transient)
	FString					Filename;
	
	// This flag is true if any details have been changed (and the plan needs to be saved)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Route Node", Transient)
	bool					bDetailsChanged = false;

	// The saved "Starting point" of this route in world coordinates. If used on a different map, it will be whatever the new root point is. All node coords are relative to this one
//	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Route Node")
	//FVector					StartingPointWorld = FVector::ZeroVector;

	// When a new point is added, what altitude does it default to in meters?
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Route Node", BlueprintSetter=SetDefaultAltitude)
	float					DefaultAltitude = 10.0f;

	UFUNCTION(BlueprintCallable, Category = "Route Node")
	void					SetDefaultAltitude(float newAltitude);

	// What velocity is this plan flown at?
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Route Node", BlueprintSetter=SetDefaultVelocity)
	float					DefaultVelocity = 1.0f;

	UFUNCTION(BlueprintCallable, Category = "Route Node")
	void					SetDefaultVelocity(float newVelocity);

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Route Node")
	TMap<FString, FRotator>	DefaultGimbalOrientations;

	UFUNCTION(BlueprintCallable, Category = "Route Node")
	void					SetDefaultGimbalOrientation(FString cameraKey, FRotator newOrientation);

	// The list of nodes in this route, where 0 is the root - WARNING: don't manually add or remove from this unless you know what you are doing.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Route Node")
	TArray<URouteNode*>		RouteNodes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Route Node")
	ELastNodeBehaviour		LastNodeBehaviour = ELastNodeBehaviour::LandAtLastNode;

	UFUNCTION(BlueprintCallable, Category = "Route Node")
	void					SetLastNodeBehaviour(ELastNodeBehaviour NewLastNodeBehaviour);

	// Constructs and adds a new node, firing the OnNodeAdded event when done
	UFUNCTION(BlueprintCallable, Category = "Route Node")
	URouteNode*				AddNewNode(FVector InCoord);

	UFUNCTION(BlueprintCallable, Category = "Route Node")
	URouteNode*				InsertNodeAfter(URouteNode* RouteNodeData, FVector InCoord);

	// Removes a node from the array and fires the OnNodeRemoved event when done
	UFUNCTION(BlueprintCallable, Category = "Route Node")
	void					DeleteRouteNode(URouteNode* nodeData);

	UPROPERTY(BlueprintAssignable, Category = "Route Node")
	FLastNodeBehaviourEventSig					OnLastNodeBehaviourChanged;

	// Called when a node is added
	UPROPERTY(BlueprintAssignable, Category = "Route Plan Data Events")
	FRouteNodeEventSig							OnNodeAdded;

	// Called when a node has been deleted
	UPROPERTY(BlueprintAssignable, Category = "Route Plan Data Events")
	FRouteNodeEventSig							OnNodeRemoved;

	UFUNCTION(BlueprintCallable, Category = "Route Plan IO")
	bool										LoadFromJSONString(FString jsonString);

	// Loads a json file from the AirSim directory in My Documents. Automatically suffixed with .json
	UFUNCTION(BlueprintCallable, Category = "Route Plan IO")
	bool										LoadFromJSONFile(FString jsonFullPath);

	UFUNCTION(BlueprintCallable, Category = "Route Plan IO")
	bool										SaveToJSONString(FString &OutputString);

	UFUNCTION(BlueprintCallable, Category = "Route Plan IO")
	bool										SaveAs(FString jsonFullPath);

	// Save to the already existing file name
	UFUNCTION(BlueprintCallable, Category = "Route Plan IO")
	bool										Save();
	
	// The root position of this plan, in absolute coordinates. Is set when saving and loading
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Route Node", Transient)
	FVector										RootPosition;

	// A copy of the gimbal camera data to avoid a reference back to the owner
	UPROPERTY(BlueprintReadOnly, Category = "Route Node")
	TArray<FGimbalCameraData>					GimbalCameraData;

protected:

	UFUNCTION()
	void HandleNodeDetailsChanged(URouteNode* nodeChanged);

};
