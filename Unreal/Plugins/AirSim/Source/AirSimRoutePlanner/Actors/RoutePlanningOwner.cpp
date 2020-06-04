// Fill out your copyright notice in the Description page of Project Settings.


#include "RoutePlanningOwner.h"
#include "UI/RoutePlanningWidget.h"
#include "InputCoreTypes.h"
#include "GameFramework/PlayerInput.h"
#include "AirSimPluginSettings.h"
#include "Components/BoxComponent.h"
#include "Actors/RoutePlanClickPlaneActor.h"
#include "CollisionQueryParams.h"
#include "Actors/RouteEdgeActor.h"
#include "EngineUtils.h"
#include "Actors/MapSelectableActor.h"
#include "Data/RoutePlan.h"
#include "Data/RouteNode.h"
#include "Actors/RouteNodeActor.h"
#include "SimMode/SimModeBase.h"
#include "AirSimRoutePlanner.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/CollisionProfile.h"
#include "common/AirSimSettings.hpp"
#include "api/ApiProvider.hpp"
#include "Data/GimbalCameraData.h"
#include "Math/UnrealMathUtility.h"
#include "AirSimRoutePlanner.h"

FName ARoutePlanningOwnerPawn::AxisLeft("AirSimLeft");
FName ARoutePlanningOwnerPawn::AxisUp("AirSimUp");
FName ARoutePlanningOwnerPawn::AxisRotate("AirSimRotateCW");
FName ARoutePlanningOwnerPawn::AxisZoomIn("AirSimZoomInAxis");
FName ARoutePlanningOwnerPawn::ZoomIn("AirSimZoomIn");
FName ARoutePlanningOwnerPawn::ZoomOut("AirSimZoomOut");
FName ARoutePlanningOwnerPawn::PlaceOrSelectPoint("AirSimPlaceOrSelect");
FName ARoutePlanningOwnerPawn::DragMap("AirSimDragMap");

FName ARoutePlanningOwnerPawn::DeleteNode("AirSimDeleteNodeOrEdge");

#define LOCTEXT_NAMESPACE "AirSim"

// Sets default values
ARoutePlanningOwnerPawn::ARoutePlanningOwnerPawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AxisLeftMappings.Add(FInputAxisKeyMapping(ARoutePlanningOwnerPawn::AxisLeft, EKeys::Left, -1.0f));
	AxisLeftMappings.Add(FInputAxisKeyMapping(ARoutePlanningOwnerPawn::AxisLeft, EKeys::A, -1.0f));
	AxisLeftMappings.Add(FInputAxisKeyMapping(ARoutePlanningOwnerPawn::AxisLeft, EKeys::Right, 1.0f));
	AxisLeftMappings.Add(FInputAxisKeyMapping(ARoutePlanningOwnerPawn::AxisLeft, EKeys::D, 1.0f));

	AxisUpMappings.Add(FInputAxisKeyMapping(ARoutePlanningOwnerPawn::AxisUp, EKeys::Up, 1.0f));
	AxisUpMappings.Add(FInputAxisKeyMapping(ARoutePlanningOwnerPawn::AxisUp, EKeys::W, 1.0f));
	AxisUpMappings.Add(FInputAxisKeyMapping(ARoutePlanningOwnerPawn::AxisUp, EKeys::Down, -1.0f));
	AxisUpMappings.Add(FInputAxisKeyMapping(ARoutePlanningOwnerPawn::AxisUp, EKeys::S, -1.0f));

	AxisRotateMappings.Add(FInputAxisKeyMapping(ARoutePlanningOwnerPawn::AxisRotate, EKeys::Q, 1.0f));
	AxisRotateMappings.Add(FInputAxisKeyMapping(ARoutePlanningOwnerPawn::AxisRotate, EKeys::E, -1.0f));

	AxisZoomInMappings.Add(FInputAxisKeyMapping(ARoutePlanningOwnerPawn::AxisZoomIn, EKeys::MouseWheelAxis, -1.0f));

	PlaceOrSelectMappings.Add(FInputActionKeyMapping(ARoutePlanningOwnerPawn::PlaceOrSelectPoint, EKeys::LeftMouseButton));

	DragMapMappings.Add(FInputActionKeyMapping(ARoutePlanningOwnerPawn::DragMap, EKeys::MiddleMouseButton));

	DeleteNodeMappings.Add(FInputActionKeyMapping(ARoutePlanningOwnerPawn::DeleteNode, EKeys::Delete));
}


void ARoutePlanningOwnerPawn::AddInputMappings()
{
	APlayerController* PC = Cast<APlayerController>(Controller);

	if (PC != nullptr)
	{
		// Remove all the previous action and axis mappings and save them for later

		PreviousActionMappings = PC->PlayerInput->ActionMappings;
		PreviousAxisMappings = PC->PlayerInput->AxisMappings;

		UE_LOG(LogAirSimRP, Log, TEXT("Saved %d action and %d axis mappings"), PreviousActionMappings.Num(), PreviousAxisMappings.Num());

		PC->PlayerInput->ActionMappings.Empty();
		PC->PlayerInput->AxisMappings.Empty();

		for (const FInputActionKeyMapping& actionKeyMapping : PlaceOrSelectMappings)
		{
			PC->PlayerInput->AddActionMapping(actionKeyMapping);
			ActionMappings.Add(actionKeyMapping);
		}

		for (const FInputActionKeyMapping& actionKeyMapping : DragMapMappings)
		{
			PC->PlayerInput->AddActionMapping(actionKeyMapping);
			ActionMappings.Add(actionKeyMapping);
		}

		for (const FInputActionKeyMapping& actionKeyMapping : DeleteNodeMappings)
		{
			PC->PlayerInput->AddActionMapping(actionKeyMapping);
			ActionMappings.Add(actionKeyMapping);
		}

		for (const FInputAxisKeyMapping& axisKeyMapping : AxisLeftMappings)
		{
			PC->PlayerInput->AddAxisMapping(axisKeyMapping);
			AxisMappings.Add(axisKeyMapping);
		}

		for (const FInputAxisKeyMapping& axisKeyMapping : AxisUpMappings)
		{
			PC->PlayerInput->AddAxisMapping(axisKeyMapping);
			AxisMappings.Add(axisKeyMapping);
		}

		for (const FInputAxisKeyMapping& axisKeyMapping : AxisRotateMappings)
		{
			PC->PlayerInput->AddAxisMapping(axisKeyMapping);
			AxisMappings.Add(axisKeyMapping);
		}

		for (const FInputAxisKeyMapping& axisKeyMapping : AxisZoomInMappings)
		{
			PC->PlayerInput->AddAxisMapping(axisKeyMapping);
			AxisMappings.Add(axisKeyMapping);
		}

	}
}

void ARoutePlanningOwnerPawn::RemoveInputMappings()
{
	APlayerController* PC = Cast<APlayerController>(Controller);

	if (PC != nullptr)
	{
		for (FInputActionKeyMapping actionMapping : ActionMappings)
		{
			PC->PlayerInput->RemoveActionMapping(actionMapping);
		}

		ActionMappings.Empty();

		for (FInputAxisKeyMapping axisMapping : AxisMappings)
		{
			PC->PlayerInput->RemoveAxisMapping(axisMapping);
		}

		AxisMappings.Empty();

		// Now add the old ones back

		PreviousActionMappings = PC->PlayerInput->ActionMappings;
		PreviousAxisMappings = PC->PlayerInput->AxisMappings;

		for (FInputActionKeyMapping actionMapping : PreviousActionMappings)
		{
			PC->PlayerInput->AddActionMapping(actionMapping);
		}

		for (FInputAxisKeyMapping axisMapping : PreviousAxisMappings)
		{
			PC->PlayerInput->AddAxisMapping(axisMapping);
		}
	}
}

void ARoutePlanningOwnerPawn::UpdateClickPlaneSize()
{
	if (ClickPlaneActor && CameraActor)
	{
		float zoom = CameraActor->Zoom;

		// This actually makes the box twice as wide as it needs to be! That's fine.
		ClickPlaneActor->ClickPlaneVolume->SetBoxExtent(FVector(zoom*100.0f, zoom*100.0f, 100.0f));
	}
}

void ARoutePlanningOwnerPawn::GetGimbalCameraData() 
{
	if (!SimMode)
		return;

	GimbalCameraData.Empty();

	FGimbalCameraData& cameraData = GimbalCameraData.AddDefaulted_GetRef();
	cameraData.Name = "Default";
	// Happy with default rotation limits

	// FOV is set to a 16:9 aspect ratio
	cameraData.HFOV = 90.0f;
	cameraData.VFOV = 59.0f;

// 	// NOTE: This is bad, we're taking the first one because we don't have a way to be smarter about it
// 	for (const auto& pair : SimMode->getApiProvider()->getVehicleSimApis().getMap())
// 	{
// 		msr::airlib::VehicleSimApiBase* apiBase = pair.second;
// 
// 		for (const auto& camPair : apiBase->getVehicleSetting()->cameras)
// 		{
// 			msr::airlib::AirSimSettings::CameraSetting camera = camPair.second;
// 
// 			if (!camera.rotation.isNaN())
// 			{
// 				FGimbalCameraData& cameraData = GimbalCameraData.AddZeroed_GetRef();
// 				cameraData.Name = pair.first.c_str();
// 				cameraData.RotationLimits = FRotator(camera.rotation.pitch, camera.rotation.yaw, camera.rotation.roll); // PROBABLY NOT RIGHT
// 				
// 				if (camera.capture_settings.size() > 0)
// 				{
// 					// Get the first CaptureSetting and get fovs from it
// 
// 					for (const auto& csPair : camera.capture_settings)
// 					{
// 						msr::airlib::AirSimSettings::CaptureSetting cs = csPair.second;
// 
// // 						V = Vertical FOV
// // 						H = Horizontal FOV
// // 						r = aspect ratio(1920 / 1080 for FHD)
// // 
// // 						V = 2 * arctan(tan(H / 2) / r)
// 
// 						float r = (float) (cs.width / cs.height);
// 						float H = FMath::DegreesToRadians(cs.fov_degrees);
// 						float VFOV = 2.0f * FMath::Atan(FMath::Tan(H/2.0f)/r);
// 
// 						cameraData.HFOV = cs.fov_degrees;
// 						cameraData.VFOV = FMath::RadiansToDegrees(VFOV);
// 					}
// 				}
// 				else
// 				{
// 					UE_LOG(LogAirSimRP, Error, TEXT("Camera %s doesn't have any capture settings defined, so no FOV can be extracted"), *cameraData.Name);
// 					
// 					// Defaulting to 16:9
// 					cameraData.HFOV = 90.0f;
// 					cameraData.HFOV = 59.0f;
// 				}
// 			}
// 
// 			return;
// 		}
// 	}
}

// Called when the game starts or when spawned
void ARoutePlanningOwnerPawn::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<ASimModeBase> Itr(GetWorld()); Itr; ++Itr)
	{
		SimMode = *Itr;

		UE_LOG(LogAirSimRP, Log, TEXT("Found ASimModeBase named %s"), *SimMode->GetName());
		break;
	}

	if (!SimMode)
	{
		UE_LOG(LogAirSimRP, Log, TEXT("Failed to find ASimModeBase in the scene"));
	}

	GetGimbalCameraData();
}

void ARoutePlanningOwnerPawn::SetupPlayerInputComponent(UInputComponent* PIC)
{
	Super::SetupPlayerInputComponent(PIC);

	if (InputComponent)
	{
		InputComponent->BindAction(PlaceOrSelectPoint, IE_Pressed, this, &ARoutePlanningOwnerPawn::HandlePlaceOrSelectPointPressed);
		InputComponent->BindAction(PlaceOrSelectPoint, IE_Released, this, &ARoutePlanningOwnerPawn::HandlePlaceOrSelectPointReleased);

		InputComponent->BindAction(DragMap, IE_Pressed, this, &ARoutePlanningOwnerPawn::HandleDragMapPressed);
		InputComponent->BindAction(DragMap, IE_Released, this, &ARoutePlanningOwnerPawn::HandleDragMapReleased);

		InputComponent->BindAction(DeleteNode, IE_Released, this, &ARoutePlanningOwnerPawn::HandleNodeDelete);

		InputComponent->BindAxis(AxisLeft, this, &ARoutePlanningOwnerPawn::HandleAxisLeft);
		InputComponent->BindAxis(AxisUp, this, &ARoutePlanningOwnerPawn::HandleAxisUp);

		InputComponent->BindAxis(AxisRotate, this, &ARoutePlanningOwnerPawn::HandleAxisRotate);

		InputComponent->BindAxis(AxisZoomIn, this, &ARoutePlanningOwnerPawn::HandleAxisZoomIn);
	}
}

void ARoutePlanningOwnerPawn::HandlePlaceOrSelectPointPressed() 
{
	auto PC = Cast<APlayerController>(Controller);

	if (PC != nullptr)
	{
		FHitResult hitResult;

		//TODO: Use airsim plugin settings channel!
		if (PC->GetHitResultUnderCursor(ClickTraceChannel, false, hitResult))
		{
			AMapSelectableActor* selectableObj = Cast<AMapSelectableActor>(hitResult.Actor.Get());

			// If the object we're clicking is the same as the currently selected, we might be trying to drag!
			if (selectableObj && selectableObj==SelectedMapObject)
			{
				FVector selectableLocation = selectableObj->GetActorLocation();

				// Set the click plane height to the node we are moving
				SetClickPlaneHeight(selectableLocation.Z);

				// Get a new position, ignoring the nodes and edges
				FHitResult clickPlaneHitResult;

				if (!GetHitResultOnClickPlane(PC, ClickTraceChannel, clickPlaneHitResult))
				{
					UE_LOG(LogAirSimRP, Error, TEXT("Failed to get a hit result on the click plane for start drag!"))
					return;
				}

				bSelectButtonDown = true;
				StartClickPlaneDragPos = clickPlaneHitResult.Location;
				StartActorDragPos = selectableLocation;
				DragOffset = FVector(hitResult.Location.X - selectableLocation.X, hitResult.Location.Y - selectableLocation.Y, 0);
				UE_LOG(LogAirSimRP, Log, TEXT("Drag started at %s/%s, drag offset: %s"), *StartClickPlaneDragPos.ToString(), *StartActorDragPos.ToString(), *DragOffset.ToString());
			}

			UE_LOG(LogAirSimRP, Log, TEXT("Hit on %s at %s"), *hitResult.Actor->GetName(), *hitResult.Location.ToString());
		}
	}
}

void ARoutePlanningOwnerPawn::HandlePlaceOrSelectPointReleased()
{
	bSelectButtonDown = false;

	// Set the click plane height back to altitude zero
	ResetClickPlaneHeight();

	auto PC = Cast<APlayerController>(Controller);

	if (PC != nullptr)
	{
		FHitResult hitResult;

		bool bHit = false;
		bool bFirstNode = CurrentRoutePlan->RouteNodes.Num() == 0;

		if (bFirstNode)
		{
			// Placing the first node is special - we don't want to hit the click plane, we want to hit the ground
			//NOTE: Does this channel need to be put into a setting as well? Maybe
			bHit = PC->GetHitResultUnderCursor(ECC_Visibility, false, hitResult);
		}
		else
		{
			bHit = PC->GetHitResultUnderCursor(ClickTraceChannel, false, hitResult);
		}

		//TODO: Use airsim plugin settings channel!
		if (bHit)
		{
			AMapSelectableActor* selectableObj = Cast<AMapSelectableActor>(hitResult.Actor.Get());

			if (selectableObj)
			{
				SetSelectedObject(selectableObj);
			}
			else
			{
				if (SelectedMapObject)
				{
					SetSelectedObject(nullptr);
				}
				else
				{
					// Selected the click plane, so spawn a point if we're in the right mode
					if (CurrentRoutePlan && InteractionMode == EPlanInteractionMode::AddingPoints)
					{
						FVector newLoc = hitResult.Location;

						if (bFirstNode)
						{
							// Set the root position, including altitude in UU
							CurrentRoutePlan->RootPosition = newLoc;
						}

						// Set altitude to the root position plus default altitude
						newLoc.Z = CurrentRoutePlan->RootPosition.Z + CurrentRoutePlan->DefaultAltitude * 100.0f; // meters!

						AddNodeAt(newLoc);
					}
				}
			}

			UE_LOG(LogAirSimRP, Log, TEXT("Hit on %s at %s"), *hitResult.Actor->GetName(), *hitResult.Location.ToString());
		}
	}
}

void ARoutePlanningOwnerPawn::DeleteRouteNode(URouteNode* NodeData)
{
	if (CurrentRoutePlan && NodeData)
	{
		auto findNode = [&](ARouteNodeActor* node)
		{
			return node->RouteNodeData==NodeData;
		};

		ARouteNodeActor** nodeToDeletePtr = RouteNodes.FindByPredicate(findNode);
		if (!nodeToDeletePtr)
		{
			UE_LOG(LogAirSimRP, Error, TEXT("Failed to find node actor to delete by route node data"));
			return;
		}

		ARouteNodeActor* nodeToDelete = *nodeToDeletePtr;

		// Make sure it's not the root/first node
		if (NodeData->IsFirstNode()) // It's the root
		{
			UE_LOG(LogAirSimRP, Warning, TEXT("Can't delete the root node"));
			return;
		}

		if (nodeToDelete->EdgeToParent && nodeToDelete->EdgeToChild) // Node in the middle of two
		{
			// We keep the edge to parent and delete the edge to child

			ARouteNodeActor* previousNode = nodeToDelete->EdgeToParent->ParentNode;
			ARouteNodeActor* nextNode = nodeToDelete->EdgeToChild->TargetNode;

			// Point the edge from parent to the new next node
			nodeToDelete->EdgeToParent->TargetNode = nextNode;

			// Next node inherits deleted node's edge to parent
			nextNode->EdgeToParent = nodeToDelete->EdgeToParent;

			// Delete our now-useless edgeToChild
			nodeToDelete->EdgeToChild->Destroy();

			previousNode->UpdateEdges();
			nextNode->UpdateEdges();
		}
		else if (nodeToDelete->EdgeToParent && !nodeToDelete->EdgeToChild) // Node at end of the chain
		{
			// We delete the edge to parent

			ARouteNodeActor* previousNode = nodeToDelete->EdgeToParent->ParentNode;

			previousNode->EdgeToChild = nullptr;
			nodeToDelete->EdgeToParent->Destroy();
		}
		else // First node or some weird case.
		{
			return;
		}

		// Delete the data		
		CurrentRoutePlan->DeleteRouteNode(NodeData);

		// Remove it from current nodes
		RouteNodes.Remove(nodeToDelete);

		// Destroy the actor
		nodeToDelete->Destroy();

		SetSelectedObject(nullptr);

	}
}

void ARoutePlanningOwnerPawn::DeleteRouteNodeByActor(ARouteNodeActor* nodeToDelete)
{
	if (CurrentRoutePlan && nodeToDelete)
	{
		URouteNode* nodeData = nodeToDelete->RouteNodeData;

		DeleteRouteNode(nodeData);
	}
}

void ARoutePlanningOwnerPawn::HandleNodeDelete()
{
	ARouteNodeActor* nodeToDelete = Cast<ARouteNodeActor>(SelectedMapObject);

	DeleteRouteNodeByActor(nodeToDelete);
}

void ARoutePlanningOwnerPawn::HandleDragMapPressed()
{
	auto PC = Cast<APlayerController>(Controller);

	if (PC != nullptr)
	{
		FHitResult hitResult;

		if (GetHitResultOnClickPlane(PC, ClickTraceChannel, hitResult))
		{
			bMapDragButtonDown = true;

			StartClickPlaneDragPos = hitResult.Location;
			StartMapViewPos = CameraActor->TargetViewPoint;

			PC->GetMousePosition(MousePositionDragStart.X, MousePositionDragStart.Y);

			UE_LOG(LogAirSimRP, Log, TEXT("Map drag started at %s"), *MousePositionDragStart.ToString());
		}
	}
}

void ARoutePlanningOwnerPawn::HandleDragMapReleased()
{
	bMapDragButtonDown = false;
}

void ARoutePlanningOwnerPawn::HandleAxisLeft(float Value)
{
	CameraMovementVector.Y = Value;
}

void ARoutePlanningOwnerPawn::HandleAxisUp(float Value)
{
	CameraMovementVector.X = Value;
}

void ARoutePlanningOwnerPawn::HandleAxisRotate(float Value)
{
	CameraRotateAxis = Value;
}

void ARoutePlanningOwnerPawn::HandleAxisZoomIn(float Value)
{
	CameraZoomAxis = Value;

	UpdateNodeVisuals();
}

float ARoutePlanningOwnerPawn::GetCameraZoom()
{
	if (CameraActor)
	{
		return CameraActor->Zoom;
	}

	return 0;
}

void ARoutePlanningOwnerPawn::SetRoutePlan(URoutePlan* NewRoutePlan)
{
	if (NewRoutePlan != CurrentRoutePlan)
	{
		CurrentRoutePlan = NewRoutePlan;

		OnNewRoutePlan.Broadcast(CurrentRoutePlan);

		CurrentRoutePlan->OnLastNodeBehaviourChanged.AddDynamic(this, &ARoutePlanningOwnerPawn::HandleLastNodeBehaviourChanged);
	}
}

bool ARoutePlanningOwnerPawn::SaveRoutePlan(FString filename)
{
	if (!CurrentRoutePlan)
	{
		UE_LOG(LogAirSimRP, Error, TEXT("Trying to save but no Current Route Plan"));
		return false;
	}

	return CurrentRoutePlan->SaveAs(filename);
}

void ARoutePlanningOwnerPawn::Save(bool bSaveAs)
{
	if (CurrentRoutePlan)
	{
		if (CurrentRoutePlan->Filename.IsEmpty() || bSaveAs) // Never saved
		{
			UFileDialogUI* fileDlg = ShowFileDialog(FString(FPlatformProcess::UserDir()) + "AirSim/", true);

			fileDlg->OnFilenameChosen.AddLambda([&](FString fileFullPath)
			{
				HideFileDialog();

				CurrentRoutePlan->SaveAs(fileFullPath);
			});

			fileDlg->OnCanceled.AddLambda([&]()
			{
				HideFileDialog();
			});
		}
		else
		{
			CurrentRoutePlan->Save();
		}
	}

}

void ARoutePlanningOwnerPawn::Load()
{
	auto doLoad = [&](ARoutePlanningOwnerPawn* owner, FString planFileName)
	{
		URoutePlan* newRoutePlan = NewObject<URoutePlan>(owner, NAME_None);

		if (!newRoutePlan->LoadFromJSONFile(planFileName))
		{
			UE_LOG(LogAirSimRP, Error, TEXT("Failed to load route plan %s"), *planFileName);
			return false;
		}

		// Destroy old nodes and edges
		for (ARouteNodeActor* node : owner->RouteNodes)
		{
			node->Destroy();
		}

		owner->RouteNodes.Empty();

		for (ARouteEdgeActor* edge : owner->RouteEdges)
		{
			edge->Destroy();
		}

		owner->RouteEdges.Empty();

		// Spawn new ones

		for (URouteNode* nodeData : newRoutePlan->RouteNodes)
		{
			owner->AddActorsForNodeData(nodeData);
		}

		owner->UpdateNodeVisuals();

		// Do it last to make sure all the actors and edges are emptied out
		owner->SetRoutePlan(newRoutePlan);

		owner->HandleLastNodeBehaviourChanged();

		return true;
	};

	auto loadUsingDlg = [&](ARoutePlanningOwnerPawn* owner)
	{
		UFileDialogUI* fileDlg = owner->ShowFileDialog(FString(FPlatformProcess::UserDir()) + "AirSim/", false);

		fileDlg->OnFilenameChosen.AddLambda([&, owner](FString fileFullPath)
		{
			owner->HideFileDialog();

			doLoad(owner, fileFullPath);
		});

		fileDlg->OnCanceled.AddLambda([&, owner]()
		{
			owner->HideFileDialog();
		});
	};

	if (CurrentRoutePlan && CurrentRoutePlan->bDetailsChanged)
	{
		// Need a confirmation!
		UConfirmationDialog* dlg = ShowConfirmationDialog(LOCTEXT("AreYouSure", "Are you sure?"), LOCTEXT("RoutePlanModified", "The current route plan has been modified. Changes will be lost."));

		dlg->OnOkButtonClicked.AddLambda([&]()
		{
			HideConfirmationDialog();

			//TODO: Should really show the user it failed!
			loadUsingDlg(this);
		});

		dlg->OnCancelButtonClicked.AddLambda([&]()
		{
			HideConfirmationDialog();
		});
	}
	else
	{
		loadUsingDlg(this);
	}

	UpdateNodeVisuals();
}

void ARoutePlanningOwnerPawn::NewRoutePlan()
{
	// Note: Need to use a reference to the owner pawn here instead of this because of the way it's nested in another lambda below.
	auto doNewRoutePlan = [&](ARoutePlanningOwnerPawn* owner)
	{
		// Dump the current route plan data and make a new one

		URoutePlan* newPlan = NewObject<URoutePlan>(owner);
		newPlan->DefaultAltitude = GetDefault<UAirSimPluginSettings>()->DefaultAltitude;
		newPlan->DefaultVelocity = GetDefault<UAirSimPluginSettings>()->DefaultVelocity;

		newPlan->GimbalCameraData = GimbalCameraData;
		
		// Add the gimbal cameras

		for (FGimbalCameraData cameraData : GimbalCameraData)
		{
			FRotator zeroRot = FRotator::ZeroRotator;

			// Pitch at min, yaw in middle

			zeroRot.Pitch = cameraData.MinPitch; 
			
			float yawRange = FMath::Abs(cameraData.MinYaw - cameraData.MaxYaw);
			zeroRot.Yaw = cameraData.MinYaw + yawRange/2.0f;

			newPlan->DefaultGimbalOrientations.Add(cameraData.Name, zeroRot);
		}

		// Clear all nodes and edges

		for (ARouteNodeActor* node : owner->RouteNodes)
		{
			node->Destroy();
		}

		owner->RouteNodes.Empty();

		for (ARouteEdgeActor* edge : owner->RouteEdges)
		{
			edge->Destroy();
		}

		owner->RouteEdges.Empty();

		owner->ResetClickPlaneHeight();

		// Do it last to make sure all the actors and edges are emptied out
		owner->SetRoutePlan(newPlan);

		owner->InteractionMode = EPlanInteractionMode::AddingPoints;
	};

	if (CurrentRoutePlan && CurrentRoutePlan->bDetailsChanged)
	{
		// Need a confirmation!
		UConfirmationDialog* dlg = ShowConfirmationDialog(LOCTEXT("AreYouSure", "Are you sure?"), LOCTEXT("RoutePlanModified", "The current route plan has been modified. Changes will be lost."));

		dlg->OnOkButtonClicked.AddLambda([&]() 
		{ 
			HideConfirmationDialog(); 
			doNewRoutePlan(this); 
		});

		dlg->OnCancelButtonClicked.AddLambda([&]()
		{
			HideConfirmationDialog();
		});
	}
	else
	{
		doNewRoutePlan(this);
	}
}

bool ARoutePlanningOwnerPawn::LoadRoutePlan(FString routePlanFileName)
{
	auto doLoad = [&](ARoutePlanningOwnerPawn* owner, FString planFileName)
	{
		URoutePlan* newRoutePlan = NewObject<URoutePlan>(owner, NAME_None);

		if (!newRoutePlan->LoadFromJSONFile(planFileName))
		{
			UE_LOG(LogAirSimRP, Error, TEXT("Failed to load route plan %s"), *planFileName);
			return false;
		}

		// Destroy old nodes and edges
		for (ARouteNodeActor* node : owner->RouteNodes)
		{
			node->Destroy();
		}

		owner->RouteNodes.Empty();

		for (ARouteEdgeActor* edge : owner->RouteEdges)
		{
			edge->Destroy();
		}

		owner->RouteEdges.Empty();

		// Spawn new ones

		for (URouteNode* nodeData : newRoutePlan->RouteNodes)
		{
			owner->AddActorsForNodeData(nodeData);
		}

		owner->UpdateNodeVisuals();

		// Do it last to make sure all the actors and edges are emptied out
		owner->SetRoutePlan(newRoutePlan);

		return true;
	};

	auto loadUsingDlg = [&](ARoutePlanningOwnerPawn* owner)
	{

	};

	if (CurrentRoutePlan && CurrentRoutePlan->bDetailsChanged)
	{
		// Need a confirmation!
		UConfirmationDialog* dlg = ShowConfirmationDialog(LOCTEXT("AreYouSure", "Are you sure?"), LOCTEXT("RoutePlanModified", "The current route plan has been modified. Changes will be lost."));

		dlg->OnOkButtonClicked.AddLambda([&, routePlanFileName]()
		{
			HideConfirmationDialog();

			//TODO: Should really show the user it failed!
			doLoad(this, routePlanFileName);
		});

		dlg->OnCancelButtonClicked.AddLambda([&]()
		{
			HideConfirmationDialog();
		});
	}
	else
	{
		doLoad(this, routePlanFileName);
	}

	return true;
}

void ARoutePlanningOwnerPawn::UpdateNodeVisuals()
{
	if (!CurrentRoutePlan)
		return;

	// What should our icon scale be?

	float iconWidthCM = DesiredNodeWidth * GetCameraZoom() * 100.0f;
	float edgeWidthCM = DesiredNodeWidth / 2.0f * GetCameraZoom() * 100.0f;
	float iconScale = 1.0f;
	float edgeScale = 1.0f;

	if (iconWidthCM > 0)
	{
		iconScale = iconWidthCM / 100.0f;
		edgeScale = edgeWidthCM / 100.0f;
	}

	//TODO: Fix me!
	for (int i = 0; i < RouteEdges.Num(); i++)
	{
		ARouteEdgeActor* edgeActor = RouteEdges[i];

		if (edgeActor->IsValidLowLevel())
		{
			FVector currScale = edgeActor->GetActorScale3D();
			currScale.X = edgeScale;
			currScale.Y = edgeScale;

			edgeActor->SetActorScale3D(currScale);
			edgeActor->CurrentScaling = edgeScale;

			edgeActor->UpdateTransform();
		}
	}

	for (int i = 0;i<RouteNodes.Num();i++)
	{
		ARouteNodeActor* nodeActor = RouteNodes[i];

		if (nodeActor->IsValidLowLevel())
		{
			if (i == 0)
			{
				if (CurrentRoutePlan->LastNodeBehaviour == ELastNodeBehaviour::LandAtStartNode)
				{
					nodeActor->NodeType=ERouteNodeType::TakeoffAndLanding;
				}
				else if (CurrentRoutePlan->LastNodeBehaviour == ELastNodeBehaviour::ReturnToStartAndLoop)
				{
					nodeActor->NodeType=ERouteNodeType::TakeoffAndLoop;
				}
				else
				{
					nodeActor->NodeType=ERouteNodeType::Takeoff;
				}
			}
			else if (i == RouteNodes.Num() - 1) // Last node
			{
				if (CurrentRoutePlan->LastNodeBehaviour == ELastNodeBehaviour::LandAtLastNode)
				{
					nodeActor->NodeType=ERouteNodeType::Landing;
				}
				else
				{
					nodeActor->NodeType=ERouteNodeType::Normal;
				}
			}
			else
			{
				nodeActor->NodeType=ERouteNodeType::Normal;
			}

			nodeActor->UpdateVisuals();
			nodeActor->SetActorScale3D(FVector(iconScale));
		}
	}
}

ARouteEdgeActor* ARoutePlanningOwnerPawn::SpawnEdge(ARouteNodeActor* parent, ARouteNodeActor* target)
{
	FTransform spawnTrans = FTransform::Identity;

	FVector newEdgeLoc = parent->GetActorLocation();

	// Put the new edge above the new node to avoid collision while moving it
	newEdgeLoc.Z += 200.0f;
	spawnTrans.SetTranslation(newEdgeLoc);

	ARouteEdgeActor* edgeActor = GetWorld()->SpawnActorDeferred<ARouteEdgeActor>(EdgeActorClass, spawnTrans, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	edgeActor->ParentNode = parent;
	edgeActor->TargetNode = target;

	edgeActor->FinishSpawning(spawnTrans, true);

	// Move the edge actor to it's new place
	edgeActor->UpdateTransform();

	parent->EdgeToChild = edgeActor;
	target->EdgeToParent = edgeActor;

	return edgeActor;
}

void ARoutePlanningOwnerPawn::HandleLastNodeBehaviourChanged()
{
	if (!CurrentRoutePlan)
	{
		UE_LOG(LogAirSimRP, Error, TEXT("Got a last route node behaviour changed event but no CurrentRoutePlan!"));
		return;
	}

	if (RouteNodes.Num()<=1)
		return;

	ARouteNodeActor* lastNodeActor = RouteNodes[RouteNodes.Num()-1];
	if (CurrentRoutePlan->LastNodeBehaviour == ELastNodeBehaviour::LandAtLastNode)
	{
		// Remove ongoing edge from last node if one exists
		if (lastNodeActor->EdgeToChild)
		{
			lastNodeActor->EdgeToChild->Destroy();
			lastNodeActor->EdgeToChild = nullptr;
		}
	}
	else if (	CurrentRoutePlan->LastNodeBehaviour == ELastNodeBehaviour::LandAtStartNode || 
				CurrentRoutePlan->LastNodeBehaviour == ELastNodeBehaviour::ReturnToStartAndLoop)
	{
		RouteEdges.Add(SpawnEdge(lastNodeActor, RouteNodes[0]));
	}

	// Add or remove an edge

	// Update edges of last node

	// Update last and first node visuals

	UpdateNodeVisuals();
}

void ARoutePlanningOwnerPawn::AddActorsForNodeData(URouteNode* routeNodeData)
{
	FTransform spawnTrans;
	spawnTrans.SetTranslation(routeNodeData->Coord);

	ARouteNodeActor* newNodeActor = GetWorld()->SpawnActorDeferred<ARouteNodeActor>(NodeActorClass, spawnTrans, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	newNodeActor->OwningRoutePlanner = this;

	// Add the data
	newNodeActor->SetRouteNode(routeNodeData);

	ARouteNodeActor* previousNode = nullptr;

	if (RouteNodes.Num() == 0) // First node!
	{
		newNodeActor->NodeType = ERouteNodeType::Takeoff;
	}
	else
	{
		previousNode = RouteNodes[RouteNodes.Num() - 1];
	}

	RouteNodes.Add(newNodeActor);

	newNodeActor->FinishSpawning(spawnTrans, true);

	if (previousNode)
	{
		// Spawn the new edge associated with this node

		if (CurrentRoutePlan->LastNodeBehaviour == ELastNodeBehaviour::LandAtLastNode)
		{
			// Spawn a new edge, normal behaviour

			RouteEdges.Add(SpawnEdge(previousNode, newNodeActor));
		}
		else
		{
			ARouteNodeActor* firstNodeActor = RouteNodes[0];
			
			// We assume that if we're here, the new node we're adding is NOT the second node ever added. If it is, there is no edge back to node zero to steal.

			if (firstNodeActor != previousNode)
			{
				// Steal the edge that's linking back to the first node, add a new edge pointing to the first node
				ARouteEdgeActor* lastEdge = previousNode->EdgeToChild;

				lastEdge->ParentNode=previousNode;
				lastEdge->TargetNode=newNodeActor;

				lastEdge->UpdateTransform();
			}
			else
			{
				RouteEdges.Add(SpawnEdge(previousNode, newNodeActor));
			}

			RouteEdges.Add(SpawnEdge(newNodeActor, firstNodeActor));
		}
	}
}

void ARoutePlanningOwnerPawn::AddNodeAt(FVector newNodeLoc)
{
	if (!CurrentRoutePlan)
	{
		UE_LOG(LogAirSimRP, Error, TEXT("Unable to add a new node, there's no route plan!"));
		return;
	}

	if (CurrentRoutePlan->RouteNodes.Num() == 0) // First node!
	{
		// Take the new node loc and remove the Z from it
		//CurrentRoutePlan->StartingPointWorld = FVector(newNodeLoc.X, newNodeLoc.Y, newNodeLoc.Z);
	}

	// Now spawn the actual node actor and edge

	AddActorsForNodeData(CurrentRoutePlan->AddNewNode(newNodeLoc));

	UpdateNodeVisuals();
}

void ARoutePlanningOwnerPawn::InsertNodeAfter(ARouteNodeActor* nodeActor)
{
	if (!CurrentRoutePlan)
	{
		UE_LOG(LogAirSimRP, Error, TEXT("Unable to add a new node, there's no route plan!"));
		return;
	}

	// Sanity check - can't insert if there is no edge to child

	if (!nodeActor->EdgeToChild)
	{
		UE_LOG(LogAirSimRP, Error, TEXT("Can't insert a node since there's no EdgeToChild"));
		return;
	}

	FVector newNodeLoc;

	// Determine the new location halfway between the two nodes

	ARouteNodeActor* parentNode = nodeActor;
	ARouteNodeActor* childNode = nodeActor->EdgeToChild->TargetNode;

	FVector edgeVector = (childNode->GetActorLocation() - parentNode->GetActorLocation());

	newNodeLoc = parentNode->GetActorLocation() + edgeVector * 0.5f;

	FTransform spawnTrans;
	spawnTrans.SetTranslation(newNodeLoc);

	ARouteNodeActor* newNodeActor = GetWorld()->SpawnActorDeferred<ARouteNodeActor>(NodeActorClass, spawnTrans, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	newNodeActor->OwningRoutePlanner = this;

	// Add the data
	URouteNode* routeNodeData = CurrentRoutePlan->InsertNodeAfter(parentNode->RouteNodeData, newNodeLoc);
	newNodeActor->SetRouteNode(routeNodeData);

	// Redirect the parent edge to this new node, and spawn a new edge

	parentNode->EdgeToChild->TargetNode = newNodeActor;
	newNodeActor->EdgeToParent = parentNode->EdgeToChild;
	newNodeActor->FinishSpawning(spawnTrans, true);

	int32 index;
	if (RouteNodes.Find(parentNode, index))
	{
		RouteNodes.Insert(newNodeActor, index+1);
	}

	RouteEdges.Add(SpawnEdge(newNodeActor, childNode));

	UpdateNodeVisuals();

	SetSelectedObject(newNodeActor);
}

// Called every frame
void ARoutePlanningOwnerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bSwitchToCamera) 
	{
		ActivateOverheadCamera();
		bSwitchToCamera = false;
	}

	if (!CameraMovementVector.IsNearlyZero())
	{
		UpdateCameraMove(DeltaTime);
	}

	if (CameraRotateAxis != 0)
	{
		UpdateCameraRotation(DeltaTime);
	}

	if (CameraZoomAxis != 0)
	{
		UpdateCameraZoom(DeltaTime);
	}

	if (bMapDragButtonDown)
	{
		FHitResult clickPlaneHitResult;

		auto PC = Cast<APlayerController>(Controller);

		//TODO: Get this working
// 		if (PC)
// 		{
// 			FVector2D newMousePos = FVector2D::ZeroVector;
// 
// 			PC->GetMousePosition(newMousePos.X, newMousePos.Y);
// 
// 			FVector2D mouseDelta = newMousePos - MousePositionDragStart;
// 
// 			UE_LOG(LogAirSimRP, Log, TEXT("Start: %s End: %s Mouse delta: %s"), *MousePositionDragStart.ToString(), *newMousePos.ToString(), *mouseDelta.ToString());
// 
// 			// Screen width in cm
// 			float screenWidthInCM = CameraActor->Zoom * 100.0f;
// 
// 			FVector2D VPSize;
// 			GEngine->GameViewport->GetViewportSize(VPSize);
// 
// 			// Get delta as percentage of screen
// 			float deltaPerc = mouseDelta.Size() / VPSize.X;
// 
// 			float moveDelta = screenWidthInCM*deltaPerc;
// 
// 			mouseDelta.Normalize();
// 
// 			float DeltaX = mouseDelta.X * moveDelta;
// 			float DeltaY = mouseDelta.Y * moveDelta;
// 
// 			UE_LOG(LogAirSimRP, Log, TEXT("Moving camera actor %f,%f"), DeltaX, DeltaY);
// 
// 			// Now convert into world space
// 
// 			CameraActor->OffsetTargetViewPoint(DeltaX, DeltaY);
// 		}

		if (PC && GetHitResultOnClickPlane(PC, ClickTraceChannel, clickPlaneHitResult))
		{
			// Get the delta between the start click plane pos and the current click plane pos

			FVector clickPlaneDelta = clickPlaneHitResult.Location - StartClickPlaneDragPos;
			clickPlaneDelta.Z = 0;

//  			GEngine->AddOnScreenDebugMessage(177, 1.0f, FColor::Blue, FString::Format(TEXT("Click plane delta: {0}"), { clickPlaneDelta.ToString() } ), true, FVector2D(3.0f));
//  			GEngine->AddOnScreenDebugMessage(178, 1.0f, FColor::Blue, FString::Format(TEXT("CP Hit Result Loc: {0}"), { clickPlaneHitResult.Location.ToString() }), true, FVector2D(3.0f));

			FVector newLoc = FVector(StartMapViewPos.X-clickPlaneDelta.X, StartMapViewPos.Y-clickPlaneDelta.Y, StartMapViewPos.Z);

			CameraActor->SetTargetViewPoint(newLoc);
		}
	} else if (bSelectButtonDown && InteractionMode==EPlanInteractionMode::EditingPoints) // Moving something?
	{
		if (SelectedMapObject)
		{
			ARouteNodeActor* node = Cast<ARouteNodeActor>(SelectedMapObject);

			if (node)
			{
				// Get our new click plane position
				FHitResult clickPlaneHitResult;

				auto PC = Cast<APlayerController>(Controller);

				if (PC && GetHitResultOnClickPlane(PC, ClickTraceChannel, clickPlaneHitResult))
				{
					// Get the delta between the start click plane pos and the current click plane pos

					FVector clickPlaneDelta = clickPlaneHitResult.Location - StartClickPlaneDragPos;
					clickPlaneDelta.Z = 0;

// 					GEngine->AddOnScreenDebugMessage(177, 1.0f, FColor::Blue, FString::Format(TEXT("Click plane delta: {0}"), { clickPlaneDelta.ToString() } ), true, FVector2D(3.0f));
// 					GEngine->AddOnScreenDebugMessage(178, 1.0f, FColor::Blue, FString::Format(TEXT("CP Hit Result Loc: {0}"), { clickPlaneHitResult.Location.ToString() }), true, FVector2D(3.0f));

					// Move the actor to its start location + clickPlaneDelta - DragOffset

					FVector newLoc = FVector(StartActorDragPos.X+clickPlaneDelta.X+DragOffset.X, StartActorDragPos.Y+clickPlaneDelta.Y+DragOffset.Y, StartActorDragPos.Z);

					// If this is the root node, then we're changing the coords in the plan rather than the node itself (since it is always at 0,0,0)

					// Set the location in the data
					node->RouteNodeData->SetCoord(newLoc);

					node->SetActorLocation(newLoc);
					node->UpdateEdges();

					// If we're moving the "root" node then update the root position using a trace
					if (node->RouteNodeData->IsFirstNode())
					{
						FHitResult rootNodeHitResult;

						FVector traceEnd = newLoc;
						traceEnd.Z -= 1000000.0f;

						if (!UKismetSystemLibrary::LineTraceSingle(this, newLoc, traceEnd, UCollisionProfile::Get()->ConvertToTraceType(ECC_Visibility), false, TArray<AActor*>(), EDrawDebugTrace::ForDuration, rootNodeHitResult, true))
						{
							UE_LOG(LogAirSimRP, Error, TEXT("Failed to trace from root node to ground! That's very bad."));
							return;
						}

						CurrentRoutePlan->RootPosition = rootNodeHitResult.Location;
					}
				}
			}
		}
	}
	else
	{
		TraceForObjects();
	}
}

bool ARoutePlanningOwnerPawn::GetHitResultOnClickPlane(APlayerController* PC, ECollisionChannel TraceChannel, FHitResult& HitResult) const
{
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PC->Player);
	bool bHit = false;
	if (LocalPlayer && LocalPlayer->ViewportClient)
	{
		FVector2D MousePosition;
		if (LocalPlayer->ViewportClient->GetMousePosition(MousePosition))
		{
			FCollisionQueryParams CQParams(SCENE_QUERY_STAT(ClickableTrace), false);

			// NOTE: Optimization here would be to maintain an array of nodes and edges cast to AActor
			for (ARouteNodeActor* node : RouteNodes)
			{
				CQParams.AddIgnoredActor(Cast<AActor>(node));
			}

			for (ARouteEdgeActor* edge : RouteEdges)
			{
				CQParams.AddIgnoredActor(Cast<AActor>(edge));
			}

// 			CQParams.AddIgnoredActors(RouteNodes);
// 			CQParams.AddIgnoredActors(RouteEdges);

			bHit = PC->GetHitResultAtScreenPosition(MousePosition, TraceChannel, CQParams, HitResult);
		}
	}

	if (!bHit)	//If there was no hit we reset the results. This is redundant but helps Blueprint users
	{
		HitResult = FHitResult();
	}

	return bHit;
}

bool ARoutePlanningOwnerPawn::GetHitResultUnderCursor(APlayerController* PC,  ECollisionChannel TraceChannel, bool bTraceComplex, TArray<AActor*> IgnoreActors, FHitResult& HitResult) const
{
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PC->Player);
	bool bHit = false;
	if (LocalPlayer && LocalPlayer->ViewportClient)
	{
		FVector2D MousePosition;
		if (LocalPlayer->ViewportClient->GetMousePosition(MousePosition))
		{
			FCollisionQueryParams CQParams(SCENE_QUERY_STAT(ClickableTrace), bTraceComplex);
			CQParams.AddIgnoredActors(IgnoreActors);

			bHit = PC->GetHitResultAtScreenPosition(MousePosition, TraceChannel, CQParams, HitResult);
		}
	}

	if (!bHit)	//If there was no hit we reset the results. This is redundant but helps Blueprint users
	{
		HitResult = FHitResult();
	}

	return bHit;
}


UConfirmationDialog* ARoutePlanningOwnerPawn::ShowConfirmationDialog(FText headerText, FText bodyText)
{
	if (ConfirmDialog != nullptr)
	{
		UE_LOG(LogAirSimRP, Error, TEXT("Confirmation dialog already showing!"));
		return ConfirmDialog;
	}

	ConfirmDialog = CreateWidget<UConfirmationDialog>(GetWorld(), ConfirmDialogClass);
	ConfirmDialog->HeaderText = headerText;
	ConfirmDialog->BodyText = bodyText;

	ConfirmDialog->AddToViewport(100); // Put it above the normal UI

	// Set the input mode appropriately

	auto PC = Cast<APlayerController>(Controller);
	UWidgetBlueprintLibrary::SetInputMode_UIOnly(PC);

	return ConfirmDialog;
}

void ARoutePlanningOwnerPawn::HideConfirmationDialog()
{
	if (ConfirmDialog != nullptr)
	{
		ConfirmDialog->RemoveFromViewport();
		ConfirmDialog = nullptr;
	}

	auto PC = Cast<APlayerController>(Controller);
	UWidgetBlueprintLibrary::SetInputMode_GameAndUI(PC, nullptr, false, false);
}

UFileDialogUI* ARoutePlanningOwnerPawn::ShowFileDialog(FString directory, bool bSaveDialog)
{
	if (FileDialog)
	{
		UE_LOG(LogAirSimRP, Error, TEXT("File dialog already visible!"));
		return FileDialog;
	}

	FileDialog = CreateWidget<UFileDialogUI>(GetWorld(), FileDialogClass);
	FileDialog->Directory = directory;
	FileDialog->bIsSaveDialog = bSaveDialog;
	FileDialog->FileExtension = ".rpjson";

	FileDialog->AddToViewport(100);

	FileDialog->UpdateFilesList();

	// Set the input mode appropriately

	auto PC = Cast<APlayerController>(Controller);
	UWidgetBlueprintLibrary::SetInputMode_UIOnly(PC);

	return FileDialog;
}

void ARoutePlanningOwnerPawn::HideFileDialog()
{
	if (FileDialog != nullptr)
	{
		FileDialog->RemoveFromViewport();
		FileDialog = nullptr;
	}

	auto PC = Cast<APlayerController>(Controller);
	UWidgetBlueprintLibrary::SetInputMode_GameAndUI(PC, nullptr, false, false);
}

void ARoutePlanningOwnerPawn::TraceForObjects()
{
	auto PC = Cast<APlayerController>(Controller);

	FHitResult hitResult;

	if (PC && PC->GetHitResultUnderCursor(ClickTraceChannel, false, hitResult))
	{
		AMapSelectableActor* selectableObj = Cast<AMapSelectableActor>(hitResult.Actor);

		if (selectableObj)
		{
			SetHoveredObject(selectableObj); // Set it here, it's smart, so no need to check here
		}
		else
		{
			SetHoveredObject(nullptr);
		}
	}
}

void ARoutePlanningOwnerPawn::SetInputModeGameplay()
{
	auto PC = Cast<APlayerController>(Controller);

	if (PC != nullptr)
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);

		PC->bShowMouseCursor = false;

// 		if (PC->GetPawn())
// 		{
// 			PC->GetPawn()->EnableInput(PC);
// 		}
	}
}

void ARoutePlanningOwnerPawn::SetInputModeUI()
{
	auto PC = Cast<APlayerController>(Controller);

	if (PC != nullptr)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);
		InputMode.SetHideCursorDuringCapture(false);
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;

// 		if (PC->GetPawn())
// 		{
// 			PC->GetPawn()->DisableInput(PC);
// 		}
// 
	}
}

void ARoutePlanningOwnerPawn::ShowUI() 
{
	if (UIWidgetClass != nullptr && UIWidgetClass->IsValidLowLevel())
	{
		UIWidget = CreateWidget<URoutePlanningWidget>(GetWorld(), UIWidgetClass);
		UIWidget->OwningRoutePlanner = this;

		UIWidget->AddToViewport();

		if (!CurrentRoutePlan)
		{
			NewRoutePlan();
		}
	}
}

void ARoutePlanningOwnerPawn::HideUI()
{
	UIWidget->RemoveFromViewport();
	UIWidget = nullptr;
}


void ARoutePlanningOwnerPawn::UpdateCameraMove(float DeltaTime)
{
	if (CameraActor)
	{
		float moveDelta = GetCameraZoom()*100.0f*CameraMovementPercentage * DeltaTime;

		float DeltaX = CameraMovementVector.X * moveDelta;
		float DeltaY = CameraMovementVector.Y * moveDelta;

		CameraActor->OffsetTargetViewPoint(DeltaX, DeltaY);
	}
}

void ARoutePlanningOwnerPawn::UpdateCameraRotation(float DeltaTime)
{
	if (CameraActor)
	{
		CameraActor->OffsetTopDownCameraOrientation(CameraRotateAxis*CameraRotationSpeed*DeltaTime);
	}
}

void ARoutePlanningOwnerPawn::UpdateCameraZoom(float DeltaTime)
{
	if (CameraActor)
	{
		// Set the zoom, respecting min

		float newZoom = FMath::Max(MinZoom, CameraActor->Zoom + CameraZoomAxis*CameraZoomSpeed*DeltaTime);

		CameraActor->SetZoom(newZoom);

		UpdateClickPlaneSize();

		OnCameraZoomChanged.Broadcast(newZoom);
	}
}

void ARoutePlanningOwnerPawn::SetClickPlaneHeight(float newHeight)
{
	if (ClickPlaneActor)
	{
		float thickness = ClickPlaneActor->ClickPlaneVolume->GetScaledBoxExtent().Z;

		FVector cpLoc = ClickPlaneActor->GetActorLocation();
		cpLoc.Z = newHeight - thickness;

		ClickPlaneActor->SetActorLocation(cpLoc);
	}
}

void ARoutePlanningOwnerPawn::ResetClickPlaneHeight()
{
// 	if (CurrentRoutePlan)
// 	{
// 		SetClickPlaneHeight(CurrentRoutePlan->DefaultAltitude);
// 	}
// 	else
	{
		SetClickPlaneHeight(0);
	}
}

void ARoutePlanningOwnerPawn::SetDesiredNodeWidth(float NewNodeWidth)
{
	if (DesiredNodeWidth != NewNodeWidth)
	{
		DesiredNodeWidth = NewNodeWidth;
	}
}

void ARoutePlanningOwnerPawn::SetSelectedRouteNodeData(URouteNode* RouteNodeData)
{
	if (RouteNodeData == nullptr)
	{
		SetSelectedObject(nullptr);
		return;
	}

	// Find the actor with this data

	for (ARouteNodeActor * node : RouteNodes)
	{
		if (node->RouteNodeData == RouteNodeData)
		{
			SetSelectedObject(node);
			return;
		}
	}

	UE_LOG(LogAirSimRP, Error, TEXT("Trying to set selected route node data, but no actor found with that data!"));
}

void ARoutePlanningOwnerPawn::SetSelectedObject(AMapSelectableActor* NewSelectedObject)
{
	if (SelectedMapObject == NewSelectedObject)
	{
		return;
	}

	if (SelectedMapObject != nullptr)
	{
		SelectedMapObject->SetSelected(false);
	}

	SelectedMapObject = NewSelectedObject;
	CenterOnMapObject(SelectedMapObject);

	if (SelectedMapObject != nullptr)
	{
		SelectedMapObject->SetSelected(true);

		// Fire events
		ARouteNodeActor* node = Cast<ARouteNodeActor>(SelectedMapObject);

		if (node)
		{
			OnNodeSelected.Broadcast(node);

			InteractionMode = EPlanInteractionMode::EditingPoints;
		}

		ARouteEdgeActor* edge = Cast<ARouteEdgeActor>(SelectedMapObject);

		if (edge)
		{
			OnEdgeSelected.Broadcast(edge);

			InteractionMode = EPlanInteractionMode::EditingPoints;
		}
	}
	else
	{
		OnSelectionCleared.Broadcast();
	}
}

void ARoutePlanningOwnerPawn::SetHoveredObject(AMapSelectableActor* NewHoveredObject)
{
	if (HoveredMapObject == NewHoveredObject)
	{
		return;
	}

	if (HoveredMapObject != nullptr)
	{
		HoveredMapObject->SetHoveredOver(false);
	}

	HoveredMapObject=NewHoveredObject;

	if (HoveredMapObject != nullptr)
	{
		HoveredMapObject->SetHoveredOver(true);
	
		// Fire events
		ARouteNodeActor* node = Cast<ARouteNodeActor>(HoveredMapObject);

		if (node)
		{
			OnNodeHovered.Broadcast(node);
		}
	}
}

void ARoutePlanningOwnerPawn::ActivateOverheadCamera()
{
	auto PC = Cast<APlayerController>(Controller);

	if (PC && CameraActor)  
	{
		PC->SetViewTargetWithBlend(CameraActor);
	}
}

void ARoutePlanningOwnerPawn::CenterOnMapObject(AMapSelectableActor* NewCenterTarget)
{
	if (NewCenterTarget)
	{
		CameraActor->SetTargetViewPoint(FVector(NewCenterTarget->GetActorLocation().X, NewCenterTarget->GetActorLocation().Y, 0));
	}
}

void ARoutePlanningOwnerPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	auto PC = Cast<APlayerController>(NewController);

	if (!PC)
	{
		UE_LOG(LogAirSimRP, Error, TEXT("Failed to get player controller in ARoutePlanningOwnerPawn!"));
		return;
	}


	if (PC && CameraActor == nullptr)
	{
		ClickTraceChannel = GetDefault<UAirSimPluginSettings>()->ClickTraceChannel;

		FTransform spawnTrans = GetTransform();

		spawnTrans.SetTranslation(FVector(0,0,1000.0f)); // Spawn ten meters off the ground

		// Spawn camera and click plane
		CameraActor = GetWorld()->SpawnActorDeferred<ARoutePlanCameraActor>(CameraActorClass, spawnTrans, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		CameraActor->SetTargetViewPoint(FVector::ZeroVector);
		CameraActor->SetTopDownCameraOrientation(0);
		CameraActor->SetCameraViewMode(EPlanningCameraViewMode::TopDownOrtho);
		CameraActor->SetZoom(100.0f);
		CameraActor->SetActorRotation(FVector::DownVector.Rotation());
		CameraActor->FinishSpawning(spawnTrans, true);

		ClickPlaneActor = GetWorld()->SpawnActorDeferred<ARoutePlanClickPlaneActor>(ClickPlaneClass, FTransform::Identity, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		ClickPlaneActor->FinishSpawning(spawnTrans, true);

		UpdateClickPlaneSize();

		// Set view target to camera
		bSwitchToCamera = true;


		// Add the input mappings to the game
		AddInputMappings();

		// Turn on mouse input
		SetInputModeUI();

		// Show UI
		ShowUI();

	}
}

void ARoutePlanningOwnerPawn::UnPossessed()
{
	Super::UnPossessed();

	APlayerController* PC = Cast<APlayerController>(Controller);

	if (PC) 
	{
		RemoveInputMappings();
	}

	// NOTE: This will occasionally crash if we're playing in the editor and try to destroy actors the editor has already cleaned up!

	// If we don't have a world or the world is PIE, just skip this cleanup since editor will do it for us and we can crash
	if (!GetWorld() || GetWorld() && GetWorld()->WorldType == EWorldType::PIE)
	{
		return;
	}

	for (ARouteNodeActor* nodeActor : RouteNodes)
	{
		nodeActor->Destroy();
	}

	for (ARouteEdgeActor* edgeActor : RouteEdges) 
	{
		edgeActor->Destroy();
	}

	if (UIWidget)
	{
		UIWidget->RemoveFromViewport();
	}

	if (ClickPlaneActor)
	{
		ClickPlaneActor->Destroy();
	}

	if (CameraActor)
	{
		CameraActor->Destroy();
	}
}

#undef LOCTEXT_NAMESPACE