// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "RoutePlanCameraActor.generated.h"

UENUM(BlueprintType)
enum class EPlanningCameraViewMode : uint8
{
	TopDown3D,
	TopDownOrtho,
	Orbiting
};

UCLASS(BlueprintType, Blueprintable)
class AIRSIMROUTEPLANNER_API ARoutePlanCameraActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ARoutePlanCameraActor();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Camera")
	UCameraComponent*					Camera;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Camera")
	USceneComponent*					SceneRoot;

	// The point the camera is looking at, at the sea level plane
	UPROPERTY(BlueprintReadOnly, Category = "Camera")
	FVector								TargetViewPoint = FVector::ZeroVector;

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void								SetTargetViewPoint(FVector newTargetViewPoint);

	// Move the target view point by deltax and y in cm, respecting the camera rotation
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void								OffsetTargetViewPoint(float DeltaX, float DeltaY);
	
	// The "zoom" of the camera measured in meters, the longest axis of the viewport is this many meters. Should work in all view modes
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Camera")
	float								Zoom = 100.0f;

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void								SetZoom(float newZoom);

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Camera")
	EPlanningCameraViewMode				CameraViewMode = EPlanningCameraViewMode::TopDownOrtho;

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void								SetCameraViewMode(EPlanningCameraViewMode newViewMode);

	// Speed of camera movement
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
	float								CameraZoomSpeed = 3000.0f;


	// When in Orbiting mode, what degree are we at?
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Camera")
	float								OrbitPosition = 0.0f;

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void								SetOrbitPosition(float newOrbitPosition);

	// When in top down mode, where is the top of the screen pointing at? really just a 2d vector in XY
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Camera")
	float								TopDownCameraOrientation = 0;

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void								SetTopDownCameraOrientation(float newTopDownCameraOrientation);

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void								OffsetTopDownCameraOrientation(float deltaRot);

	// This will be true when the camera is currently transitioning to a new position
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera")
	bool								bTransitioningToNewHeight = false;

	virtual void Tick(float DeltaTime) override;

	// Does a bit of trig to calculate how high the camera should be
	float CalcOverheadCameraAltitude();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Tracing")
	TEnumAsByte<ECollisionChannel>				ClickTraceChannel;
protected:

	FVector								TargetCameraPosition;
	

	// When one of the camera params changes, calculate the new position for the camera to transition to
	virtual void CalcNewCameraPosition();

	void BeginPlay() override;

private:
	void UpdateCameraHeight(float DeltaTime);
};
