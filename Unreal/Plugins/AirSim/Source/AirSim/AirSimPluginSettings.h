#pragma once

#include "Engine/EngineTypes.h"
#include "AirSimPluginSettings.generated.h"

UCLASS(config = AirSim)
class AIRSIM_API UAirSimPluginSettings : public UObject
{
	GENERATED_BODY()
public:

	// This collision channel should be unused by anything else in the scene as it's used by the route planner to choose spots to place nodes
	UPROPERTY(EditAnywhere, config, Category = "Drone Route Planner")
	TEnumAsByte<ECollisionChannel>				ClickTraceChannel = ECC_GameTraceChannel12;

	// If true distances and speeds in the route planner will be shown in metric, otherwise SI
	UPROPERTY(EditAnywhere, config, Category = "Drone Route Planner")
	bool										bUseMetricMeasurements = false;

	// Default altitude in meters
	UPROPERTY(EditAnywhere, config, Category = "Drone Route Planner")
	float										DefaultAltitude = 10.0f;

	// Default velocity in meters/second
	UPROPERTY(EditAnywhere, config, Category = "Drone Route Planner")
	float										DefaultVelocity = 10.0f;

// 	UPROPERTY(EditAnywhere, config, Category = "Drone Route Planner")
// 	TSubclassOf<ARoutePlanningOwnerPawn>		RoutePlannerPawn = ARoutePlanningOwnerPawn::StaticClass();
};