#pragma once

#include "MapSelectableActor.generated.h"

/*
	A simple abstract actor class that gives us selected and hovered for map 3d objects
*/
UCLASS(BlueprintType, Blueprintable, Abstract)
class AIRSIMROUTEPLANNER_API AMapSelectableActor : public AActor
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Route Node")
	bool									bSelected = false;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Selectable Object")
	void									SetHoveredOver(bool bNewHoveredOver);
	virtual void							SetHoveredOver_Implementation(bool bNewHoveredOver);

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Route Node")
	bool									bHoveredOver = false;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Selectable Object")
	void									SetSelected(bool bNewSelected);
	virtual void							SetSelected_Implementation(bool bNewSelected);

	// Blueprint implements this event to update this nodes visuals when something important changes (starting node, selected, etc)
	UFUNCTION(BlueprintImplementableEvent, Category = "Node Visuals")
	void									HandleVisualsUpdate();

protected:

	virtual void UpdateVisuals();

};