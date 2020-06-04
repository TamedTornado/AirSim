// Fill out your copyright notice in the Description page of Project Settings.


#include "RoutePlanCameraActor.h"
#include "Math/UnrealMathUtility.h"
#include "Camera/CameraComponent.h"
#include "AirSimPluginSettings.h"
#include "AirSimRoutePlanner.h"

// Sets default values
ARoutePlanCameraActor::ARoutePlanCameraActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	// Make the scene component the root component
	RootComponent = SceneRoot;

	// Setup camera defaults
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	Camera->FieldOfView = 90.0f;
	Camera->bConstrainAspectRatio = true;
	Camera->AspectRatio = 1.777778f;
	Camera->PostProcessBlendWeight = 1.0f;

	Camera->SetupAttachment(SceneRoot);
}

void ARoutePlanCameraActor::SetTargetViewPoint(FVector newTargetViewPoint)
{
	TargetViewPoint = newTargetViewPoint;
	CalcNewCameraPosition();
}

void ARoutePlanCameraActor::OffsetTargetViewPoint(float DeltaX, float DeltaY)
{
	// Make sure these deltas are properly rotated.

	FVector deltaVec(DeltaX, DeltaY, 0);

	FRotator deltaRot(0, TopDownCameraOrientation, 0);

	FVector rotatedVec = deltaRot.RotateVector(deltaVec);

	TargetViewPoint.X += rotatedVec.X;
	TargetViewPoint.Y += rotatedVec.Y;

//	UE_LOG(LogAirSimRP, Log, TEXT("Rotated mov vec %s"), *rotatedVec.ToString());

	CalcNewCameraPosition();
}

void ARoutePlanCameraActor::SetZoom(float newZoom)
{
	Zoom = newZoom;
//	bTransitioningToNewHeight = true;
	CalcNewCameraPosition();
}

void ARoutePlanCameraActor::SetCameraViewMode(EPlanningCameraViewMode newViewMode)
{
	CameraViewMode = newViewMode;
	CalcNewCameraPosition();
}

void ARoutePlanCameraActor::SetOrbitPosition(float newOrbitPosition)
{
	OrbitPosition = newOrbitPosition;
	CalcNewCameraPosition();
}

void ARoutePlanCameraActor::SetTopDownCameraOrientation(float newTopDownCameraOrientation)
{
	TopDownCameraOrientation = newTopDownCameraOrientation;
	
	FRotator rot = GetActorRotation();
	rot.Yaw = FRotator::NormalizeAxis(TopDownCameraOrientation);
	rot.Pitch = -90.0f;
	rot.Roll = 0;


	UE_LOG(LogAirSimRP, Log, TEXT("TDCO: %f New rot: %s"), TopDownCameraOrientation, *rot.ToString());


	SetActorRotation(rot);
}

void ARoutePlanCameraActor::OffsetTopDownCameraOrientation(float deltaRot)
{
	TopDownCameraOrientation += deltaRot;

	FRotator rot = GetActorRotation();
	rot.Yaw = FRotator::NormalizeAxis(TopDownCameraOrientation);
	rot.Pitch = -90.0f;
	rot.Roll = 0;

	UE_LOG(LogAirSimRP, Log, TEXT("TDCO: %f New rot: %s"), TopDownCameraOrientation, *rot.ToString());

	SetActorRotation(rot);
}

// Called every frame
void ARoutePlanCameraActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

// 	if (bTransitioningToNewHeight)
// 	{
// 		UpdateCameraHeight(DeltaTime);
// 	}
}


float ARoutePlanCameraActor::CalcOverheadCameraAltitude()
{
	float HFOV = FMath::DegreesToRadians(Camera->FieldOfView);

	// We need to use this data and our Zoom value to calculate the height of the camera

	float halfZoom = Zoom/2.0f * 100.0f; // Meters!
	
	// Get half the HFOV as the bottom side of our right angle triangle
	float halfFov = HFOV/2.0f;

	float c = halfZoom / FMath::Sin(halfFov);

	// Calc height side
	return FMath::Sqrt(c*c - halfZoom*halfZoom);
}


void ARoutePlanCameraActor::CalcNewCameraPosition()
{
	if (CameraViewMode == EPlanningCameraViewMode::TopDown3D)
	{
		float height = CalcOverheadCameraAltitude();

		// NOTE: Should I be adjusting Z here for each map?
		TargetCameraPosition = FVector(TargetViewPoint.X, TargetViewPoint.Y, height);

		SetActorLocation(TargetCameraPosition);

	}
	else if (CameraViewMode==EPlanningCameraViewMode::TopDownOrtho)
	{
		TargetCameraPosition = FVector(TargetViewPoint.X, TargetViewPoint.Y, 25000.0f);

		SetActorLocation(TargetCameraPosition);

		Camera->SetOrthoWidth(Zoom*100.0f);

	}
	else
	{

	}
}

void ARoutePlanCameraActor::BeginPlay()
{
	Super::BeginPlay();

	ClickTraceChannel = GetDefault<UAirSimPluginSettings>()->ClickTraceChannel;
}

void ARoutePlanCameraActor::UpdateCameraHeight(float DeltaTime)
{
	float targetHeight = CalcOverheadCameraAltitude();

	float currentHeight = GetActorLocation().Z; // TODO: Handle Altitude fudge!

	if (!FMath::IsNearlyEqual(targetHeight, currentHeight))
	{
		FVector currLoc = GetActorLocation();

		float newHeight = FMath::FInterpConstantTo(currentHeight, targetHeight, DeltaTime, CameraZoomSpeed);

		currLoc.Z = newHeight;

		SetActorLocation(currLoc);

		if (FMath::IsNearlyEqual(targetHeight, currLoc.Z))
		{
			bTransitioningToNewHeight = false;
		}
	}
}

