// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RoutePlanCameraActor.h"
#include "RouteNodeActor.h"
#include "RouteEdgeActor.h"
#include "RoutePlanClickPlaneActor.h"
#include "GameFramework/PlayerInput.h"
#include "UI/RoutePlanningWidget.h"
#include "UI/ConfirmationDialog.h"
#include "UI/FileDialogUI.h"
#include "Data/GimbalCameraData.h"
#include "RoutePlanningOwner.generated.h"

class ARouteEdgeActor;
class ARouteNodeActor;
class URoutePlan;
class URouteNode;
class ASimModeBase;

UENUM(BlueprintType)
enum class EPlanInteractionMode : uint8
{
	Viewing,
	EditingPoints,
	AddingPoints

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRouteNodeActorEventSig, ARouteNodeActor*, RouteNodeActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRouteEdgeActorEventSig, ARouteEdgeActor*, RouteNodeActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeselectedActorEventSig);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRoutePlanEventSig, URoutePlan*, RoutePlan);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCameraZoomEventSig, float, ZoomValue);

UCLASS(BlueprintType, Blueprintable)
class AIRSIMROUTEPLANNER_API ARoutePlanningOwnerPawn : public APawn
{
	GENERATED_BODY()
	
public:	
	ARoutePlanningOwnerPawn();

/*
	Names used for axis and action mappings specific to this pawn
*/
	// Move the view left or right
	static FName	AxisLeft;

	// The default key mappings
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Input controls")
	TArray<FInputAxisKeyMapping>				AxisLeftMappings;

	// Move the view up or down
	static FName	AxisUp;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Input controls")
	TArray<FInputAxisKeyMapping>				AxisUpMappings;

	// Used for the top down camera to rotate the axis
	static FName	AxisRotate;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Input controls")
	TArray<FInputAxisKeyMapping>				AxisRotateMappings;

	// An axis-based zoom
	static FName	AxisZoomIn;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Input controls")
	TArray<FInputAxisKeyMapping>				AxisZoomInMappings;


	// Action-based zoom in/out
	static FName	ZoomIn;
	static FName	ZoomOut;

	// Click to select or place a point
	static FName		PlaceOrSelectPoint;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Input controls")
	TArray<FInputActionKeyMapping>				PlaceOrSelectMappings;

	static FName		DragMap;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Input controls")
	TArray<FInputActionKeyMapping>				DragMapMappings;

	static FName		DeleteNode;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Input controls")
	TArray<FInputActionKeyMapping>				DeleteNodeMappings;

	UPROPERTY(BlueprintReadOnly, Category = "Subordinate Actors")
	ARoutePlanCameraActor*						CameraActor;

	UPROPERTY(BlueprintReadOnly, Category = "Subordinate Actors")
	ARoutePlanClickPlaneActor*					ClickPlaneActor;

	UPROPERTY(BlueprintReadOnly, Category = "Planning UI")
	URoutePlanningWidget*						UIWidget = nullptr;

	// if a confirm dialog is visible, this is it.
	UPROPERTY(BlueprintReadOnly, Category = "Planning UI")
	UConfirmationDialog*						ConfirmDialog = nullptr;

	// If a file dialog is visible, this is it.
	UPROPERTY(BlueprintReadOnly, Category = "Planning UI")
	UFileDialogUI*								FileDialog = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Route Plan Data")
	URoutePlan*									CurrentRoutePlan = nullptr;

	/*
		Classes of the actors that need to be spawned. Not hardcoded so users can override them easily in BP
	*/

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Route Node")
	TSubclassOf<ARoutePlanCameraActor>			CameraActorClass = ARoutePlanCameraActor::StaticClass();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Route Node")
	TSubclassOf<ARoutePlanClickPlaneActor>		ClickPlaneClass = ARoutePlanClickPlaneActor::StaticClass();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Route Node")
	TSubclassOf<ARouteEdgeActor>				EdgeActorClass = ARouteEdgeActor::StaticClass();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Route Node")
	TSubclassOf<ARouteNodeActor>				NodeActorClass = ARouteNodeActor::StaticClass();

	// Top level route-planning widget UI
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Route Node")
	TSubclassOf<URoutePlanningWidget>			UIWidgetClass = URoutePlanningWidget::StaticClass();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Route Node")
	TSubclassOf<UConfirmationDialog>			ConfirmDialogClass = UConfirmationDialog::StaticClass();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Route Node")
	TSubclassOf<UFileDialogUI>					FileDialogClass = UFileDialogUI::StaticClass();

	// Color used for a normal node or edge
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Route Node")
	FLinearColor								NormalColor = FLinearColor::White;

	// Color used for a hovered-over node or edge
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Route Node")
	FLinearColor								HoveredColor = FLinearColor::Blue;

	// Color used for a selected node or edge
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Route Node")
	FLinearColor								SelectedColor = FLinearColor::Red;

	// What click trace channel are we using? Taken from AirSim Plugin Settings
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Transient, Category = "Collision Handling")
	TEnumAsByte<ECollisionChannel>				ClickTraceChannel;

	UFUNCTION(BlueprintPure, Category = "Camera")
	float										GetCameraZoom();

	// Speed of camera movement in percentage of zoom per second
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
	float										CameraMovementPercentage = 0.1f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
	float										CameraRotationSpeed = 250.0f;

	// How many meters per second does the zoom change
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
	float										CameraZoomSpeed = 400.0f;

	// What is the lowest the camera can go?
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
	float										MinZoom = 50.0f;

	// Desired node width as percentage of screen width
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera", BlueprintSetter=SetDesiredNodeWidth)
	float										DesiredNodeWidth = 0.05f;

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void										SetDesiredNodeWidth(float NewNodeWidth);


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Selection")
	AMapSelectableActor*						SelectedMapObject = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Selection")
	void										SetSelectedRouteNodeData(URouteNode* RouteNodeData);

	UFUNCTION(BlueprintCallable, Category = "Selection")
	void										SetSelectedObject(AMapSelectableActor* NewSelectedObject);

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Selection")
	AMapSelectableActor*						HoveredMapObject = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Selection")
	void										SetHoveredObject(AMapSelectableActor* NewHoveredObject);


	UFUNCTION(BlueprintCallable, Category = "Camera Control")
	void										ActivateOverheadCamera();

	UFUNCTION(BlueprintCallable, Category = "Camera Control")
	void										CenterOnMapObject(AMapSelectableActor* NewCenterTarget);

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Route Plan Data")
	TArray<ARouteNodeActor*>					RouteNodes;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Route Plan Data")
	TArray<ARouteEdgeActor*>					RouteEdges;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Route Plan Data")
	URoutePlan*									CurrentPlan;

	// Save the route plan, either saving with it's current file name or prompting for one
	UFUNCTION(BlueprintCallable, Category = "Route Plan Data")
	void										Save(bool bSaveAs);

	// Load a new route plan, prompting if there has been changes
	UFUNCTION(BlueprintCallable, Category = "Route Plan Data")
	void										Load();

	// Create a new route plan, prompting if there has been changes
	UFUNCTION(BlueprintCallable, Category = "Route Plan Data")
	void										NewRoutePlan();

	UFUNCTION(BlueprintCallable, Category = "Route Plan Data")
	bool										SaveRoutePlan(FString filename);


	UFUNCTION(BlueprintCallable, Category = "Route Plan Data")
	bool										LoadRoutePlan(FString routePlanFileName);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Route Plan Data")
	EPlanInteractionMode						InteractionMode = EPlanInteractionMode::Viewing;


	// Adds a new node at the specific location, adding it to the route plan. Doesn't work if there's no route plan!
	UFUNCTION(BlueprintCallable, Category = "Route Plan Data")
	void										AddNodeAt(FVector newNodeLoc);

	UFUNCTION(BlueprintCallable, Category = "Route Plan Data")
	void										InsertNodeAfter(ARouteNodeActor* nodeActor);

	UFUNCTION(BlueprintCallable, Category = "Route Plan Data")
	void										DeleteRouteNode(URouteNode* NodeData);

	UFUNCTION(BlueprintCallable, Category = "Route Plan Data")
	void										DeleteRouteNodeByActor(ARouteNodeActor* nodeToDelete);


	UPROPERTY(BlueprintAssignable, Category = "Route Plan Data Events")
	FRouteNodeActorEventSig							OnNodeSelected;

	UPROPERTY(BlueprintAssignable, Category = "Route Plan Data Events")
	FRouteNodeActorEventSig							OnNodeHovered;

	UPROPERTY(BlueprintAssignable, Category = "Route Plan Data Events")
	FRouteEdgeActorEventSig							OnEdgeSelected;

	UPROPERTY(BlueprintAssignable, Category = "Route Plan Data Events")
	FDeselectedActorEventSig						OnSelectionCleared;

	UPROPERTY(BlueprintAssignable, Category = "Route Plan Data Events")
	FRoutePlanEventSig								OnNewRoutePlan;

	UPROPERTY(BlueprintAssignable, Category = "Route Plan Data Events")
	FCameraZoomEventSig								OnCameraZoomChanged;

	UPROPERTY(BlueprintReadOnly, Category = "Gimbal Camera Data")
	TArray<FGimbalCameraData>						GimbalCameraData;


	void PossessedBy(AController* NewController) override;
	void UnPossessed() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;


protected:

	// The current AirSim simmode
	UPROPERTY()
	ASimModeBase*						SimMode;

	// Updated by the key bindings, the X and Y axis are used to move the camera in Tick()
	FVector								CameraMovementVector = FVector::ZeroVector;

	float								CameraRotateAxis = 0;

	float								CameraZoomAxis = 0;

	bool								bSwitchToCamera = false;

	bool								bMapDragButtonDown = false;
	bool								bSelectButtonDown = false;

	FVector2D							MousePositionDragStart = FVector2D::ZeroVector;

	FVector								StartClickPlaneDragPos = FVector::ZeroVector;
	FVector								StartMapViewPos = FVector::ZeroVector;
	FVector								StartActorDragPos = FVector::ZeroVector;
	FVector								DragOffset = FVector::ZeroVector;

	// The action and axis mappings added to the PC->PlayerInput on possess
	TArray<FInputActionKeyMapping>		ActionMappings;
	TArray<FInputAxisKeyMapping>		AxisMappings;

	// The mappings removed to be restored upon exit
	TArray<FInputActionKeyMapping>		PreviousActionMappings;
	TArray<FInputAxisKeyMapping>		PreviousAxisMappings;


	// Add this actor's input mappings to the possessing player controller's PlayerInput
	void	AddInputMappings();

	// Remove the added mappings from the PC's PlayerInput
	void	RemoveInputMappings();

	void	UpdateClickPlaneSize();


	void GetGimbalCameraData();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Internal Helper to set/fire events on change of route plan
	void SetRoutePlan(URoutePlan* NewRoutePlan);


	// Trace for IMapSelectableObjects if we're not currently dragging a node around
	void TraceForObjects();

	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void UpdateNodeVisuals();
	ARouteEdgeActor* SpawnEdge(ARouteNodeActor* parent, ARouteNodeActor* target);
	
	UFUNCTION()
	void HandleLastNodeBehaviourChanged();

	// Adds a new node actor and a new edge actor for the data. Assumes it hasn't already been added!
	void AddActorsForNodeData(URouteNode* routeNodeData);

	void HandlePlaceOrSelectPointPressed();
	void HandlePlaceOrSelectPointReleased();

	void HandleNodeDelete();

	void HandleDragMapPressed();
	void HandleDragMapReleased();

	void HandleAxisLeft(float Value);
	void HandleAxisUp(float Value);
	void HandleAxisRotate(float Value);
	void HandleAxisZoomIn(float Value);

	void SetInputModeGameplay();
	void SetInputModeUI();
	void ShowUI();
	void HideUI();

	void UpdateCameraMove(float DeltaTime);
	void UpdateCameraRotation(float DeltaTime);
	void UpdateCameraZoom(float DeltaTime);

	void SetClickPlaneHeight(float newHeight);

	// Reset click plane to either the default altitude or zero
	void ResetClickPlaneHeight();

	// Get a hit result ignoring nodes and edges
	bool GetHitResultOnClickPlane(APlayerController* PC, ECollisionChannel TraceChannel, FHitResult& HitResult) const;

	// A version of this method that allows for ignored actors
	bool GetHitResultUnderCursor(APlayerController* PC, ECollisionChannel TraceChannel, bool bTraceComplex, TArray<AActor*> IgnoreActors, FHitResult& HitResult) const;

	// Creates and shows the confirmation dialog with the specified header and body text
	UConfirmationDialog* ShowConfirmationDialog(FText headerText, FText bodyText);
	
	// Hides the confirm dialog if one is active
	void HideConfirmationDialog();

	UFileDialogUI*	ShowFileDialog(FString directory, bool bSaveDialog);

	void HideFileDialog();
};
