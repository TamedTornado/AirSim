#pragma once

#include "RouteNode.generated.h"

class FJsonObject;
class URouteNode;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRouteNodeChangedSig, URouteNode*, ChangedNode);

// This is a class instead of a struct so that we can pass it by reference and avoid lots of copying and state updating
UCLASS(Blueprintable, BlueprintType)
class AIRSIMROUTEPLANNER_API URouteNode : public UObject
{
	GENERATED_BODY()
public:
	// UU Coordinates, Z = Altitude - while in UE these are always absolute coords
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Route Node", BlueprintSetter = SetCoord)
	FVector					Coord;

	// Sets this node's coord in UU coordinate space
	UFUNCTION(BlueprintCallable, Category = "Route Node")
	void					SetCoord(FVector NewCoord);

	// Sets this node's coord in Geodetic coordinate space
	UFUNCTION(BlueprintCallable, Category = "Route Node")
	void					SetCoordGeodetic(FVector NewCoord);

	// Set the longitude (in geodetic space, obv)
	UFUNCTION(BlueprintCallable, Category = "Route Node")
	void					SetLongitude(float NewLongitude);

	// Set the latitude (in geodetic space, obv)
	UFUNCTION(BlueprintCallable, Category = "Route Node")
	void					SetLatitude(float NewLatitude);

	// Set the RELATIVE altitude, input in meters
	UFUNCTION(BlueprintCallable, Category = "Route Node")
	void					SetAltitude(float NewAltitude);

	// Get the altitude relative to the root position in CM
	UFUNCTION(BlueprintPure, Category = "Route Node")
	float					GetRelativeAltitude();

	// If set, the next edge traversal will use the overridden velocity value of this node
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Route Node", BlueprintSetter = SetOverrideVelocity)
	bool					bOverrideVelocity = false;

	UFUNCTION(BlueprintCallable, Category = "Route Node")
	void					SetOverrideVelocity(bool bNewOverrideVelocity);

	// What velocity will we attempt to traverse to the next node at?
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Route Node", meta = (EditCondition = "bOverrideGimbalOrientation"), BlueprintSetter = SetVelocity)
	float					Velocity;

	UFUNCTION(BlueprintCallable, Category = "Route Node")
	void					SetVelocity(float NewVelocity);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Route Node")
	TMap<FString, bool>		OverrideGimbalOrientations;

	UFUNCTION(BlueprintCallable, Category = "Route Node")
	void					SetOverrideGimbalOrientation(FString cameraKey, bool bNewOverride);

	UFUNCTION(BlueprintCallable, Category = "Route Node")
	void					SetGimbalOrientation(FString cameraKey, FRotator newGimbalOrientation);

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Route Node")
	TMap<FString, FRotator>	GimbalOrientations;

	// A reference to the owning plan so that we can see defaults, etc without having to have that object
	UPROPERTY(BlueprintReadOnly, Transient)
	class URoutePlan*		OwningPlan;

	UFUNCTION(BlueprintPure, Category = "Route Node")
	int32					GetNodeNumber();

	UFUNCTION(BlueprintPure, Category = "Route Node")
	bool					IsLastNode();

	UFUNCTION(BlueprintPure, Category = "Route Node")
	bool					IsFirstNode();

	// used for UI to update itself. Fires when NodeDetailsChanged() is called
	UPROPERTY(BlueprintAssignable, Category = "Route Node")
	FOnRouteNodeChangedSig	OnNodeDetailsChanged;

	void					InitWithJSON(TSharedPtr<FJsonObject> jsonObject);

	// Save this node to a FJsonValueObject and return it
	TSharedPtr<FJsonValueObject> GetJSONObject();
};
