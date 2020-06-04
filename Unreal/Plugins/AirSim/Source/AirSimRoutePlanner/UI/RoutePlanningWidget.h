#pragma once
#include "Blueprint/UserWidget.h"

#include "RoutePlanningWidget.generated.h"

class ARouteNodeActor;
class ARouteEdgeActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRoutePlanningWidgetButtonSig);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRoutePlanningWidgetFloatSig, float, FloatVal);

UCLASS(Blueprintable, BlueprintType)
class AIRSIMROUTEPLANNER_API URoutePlanningWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Route Planning Widget")
	class ARoutePlanningOwnerPawn*			OwningRoutePlanner;

	UPROPERTY(BlueprintAssignable, Category = "UI Events")
	FRoutePlanningWidgetButtonSig			OnZoomInClicked;

	UFUNCTION(BlueprintCallable, Category = "UI Events")
	void									CallZoomInClicked();

	UPROPERTY(BlueprintAssignable, Category = "UI Events")
	FRoutePlanningWidgetButtonSig			OnZoomOutClicked;

	UFUNCTION(BlueprintCallable, Category = "UI Events")
	void									CallZoomOutClicked();

	UPROPERTY(BlueprintAssignable, Category = "UI Events")
	FRoutePlanningWidgetFloatSig			OnZoomValueChanged;

	UFUNCTION(BlueprintCallable, Category = "UI Events")
	void									CallZoomValueChanged(float NewZoomValue);

	UPROPERTY(BlueprintReadWrite, Category = "Route Plan Data")
	ARouteNodeActor*						SelectedNode = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Route Plan Data")
	ARouteEdgeActor*						SelectedEdge = nullptr;

};
