#include "RouteNode.h"
#include "RoutePlan.h"
#include "Utils/AirsimConversionFunctions.h"
#include "JsonUtilities/Public/JsonObjectConverter.h"
#include "Utils/JSONUtils.h"
#include "Vector3D.h"




void URouteNode::SetCoord(FVector NewCoord)
{
	Coord = NewCoord;

	OnNodeDetailsChanged.Broadcast(this);
}

void URouteNode::SetCoordGeodetic(FVector NewCoord)
{
	FVector convertedCoords = UAirsimConversionFunctions::GeodeticToUU(NewCoord);

	// Ignore altitude
	Coord.X = convertedCoords.X;
	Coord.Y = convertedCoords.Y;

	OnNodeDetailsChanged.Broadcast(this);
}

void URouteNode::SetLongitude(float NewLongitude)
{
	UE_LOG(LogAirSimRP, Log, TEXT("New longitude: %f"), NewLongitude);

	FVector geodeticCoord = UAirsimConversionFunctions::UUToGeodetic(Coord);

	FVector NewCoord(geodeticCoord.X, NewLongitude, 0);
	FVector convertedCoords = UAirsimConversionFunctions::GeodeticToUU(NewCoord);

	Coord.Y = convertedCoords.Y;

	OnNodeDetailsChanged.Broadcast(this);
}

void URouteNode::SetLatitude(float NewLatitude)
{
	UE_LOG(LogAirSimRP, Log, TEXT("New latitude: %f"), NewLatitude);

// 	// Test the RT
// 	FVector3D coordDouble(Coord);
// 
 	FVector3D geodeticCoord = UAirsimConversionFunctions::UUToGeodetic(Coord);
// 	FVector3D uuCoord = UAirsimConversionFunctions::GeodeticToUUDouble(geodeticCoord);
// 
// 	UE_LOG(LogAirSimRP, Log, TEXT("RT: %s vs %s"), *Coord.ToString(), *uuCoord.ToString());

	FVector NewCoord(NewLatitude, geodeticCoord.Y, geodeticCoord.Z);
	UE_LOG(LogAirSimRP, Log, TEXT("NewCoord %s"), *NewCoord.ToString());

	FVector convertedCoords = UAirsimConversionFunctions::GeodeticToUU(NewCoord);

	Coord.X = convertedCoords.X;

	OnNodeDetailsChanged.Broadcast(this);
}

void URouteNode::SetAltitude(float NewAltitude)
{
	Coord.Z = OwningPlan->RootPosition.Z + NewAltitude * 100.0f;

	OnNodeDetailsChanged.Broadcast(this);
}

float URouteNode::GetRelativeAltitude()
{
	return Coord.Z - OwningPlan->RootPosition.Z;
}

void URouteNode::SetOverrideVelocity(bool bNewOverrideVelocity)
{
	bOverrideVelocity = bNewOverrideVelocity;

	OnNodeDetailsChanged.Broadcast(this);
}

void URouteNode::SetVelocity(float NewVelocity)
{
	Velocity = NewVelocity;

	OnNodeDetailsChanged.Broadcast(this);
}

void URouteNode::SetOverrideGimbalOrientation(FString cameraKey, bool bNewOverride)
{
	if (!OverrideGimbalOrientations.Contains(cameraKey))
	{
		OverrideGimbalOrientations.Add(cameraKey, bNewOverride);
	} else
		OverrideGimbalOrientations[cameraKey] = bNewOverride;

	OnNodeDetailsChanged.Broadcast(this);
}

void URouteNode::SetGimbalOrientation(FString cameraKey, FRotator newGimbalOrientation)
{
	if (!GimbalOrientations.Contains(cameraKey))
	{
		GimbalOrientations.Add(cameraKey, newGimbalOrientation);
	} else
		GimbalOrientations[cameraKey] = newGimbalOrientation;

	OnNodeDetailsChanged.Broadcast(this);
}

int32 URouteNode::GetNodeNumber()
{
	int32 index = OwningPlan->RouteNodes.Find(this);

	if (index == INDEX_NONE)
	{
		return 0;
	}

	return index+1;
}

bool URouteNode::IsLastNode()
{
	if (OwningPlan->RouteNodes.Num() <= 1)
		return false;

	return OwningPlan->RouteNodes[OwningPlan->RouteNodes.Num() - 1] == this;
}

bool URouteNode::IsFirstNode()
{
	if (OwningPlan->RouteNodes.Num() == 0)
		return false;

	return OwningPlan->RouteNodes[0] == this;
}

void URouteNode::InitWithJSON(TSharedPtr<FJsonObject> jsonObject)
{
	// Read relative coord and then convert it to absolute

	FVector relativeCoord = UAirsimConversionFunctions::NedToUU(JSONUtils::GetVectorFromJSON(jsonObject->GetObjectField("Coord")));

	Coord = relativeCoord + OwningPlan->RootPosition;

	bOverrideVelocity = jsonObject->GetBoolField("bOverrideVelocity");
	Velocity = jsonObject->GetNumberField("Velocity");

	TSharedPtr<FJsonObject> gimbalsObj = jsonObject->GetObjectField("Gimbals");

	if (!gimbalsObj.IsValid())
	{
		UE_LOG(LogAirSimRP, Warning, TEXT("No Gimbals object in node"));
		return;
	}

	for (auto pair : gimbalsObj->Values)
	{
		FString cameraName = pair.Key;

		TSharedPtr<FJsonObject> cameraObj = pair.Value->AsObject();
		check(cameraObj.IsValid());

		bool bOverride = cameraObj->GetBoolField("bOverrideOrientation");

		FRotator rot = JSONUtils::GetRotatorFromJSON(cameraObj->GetObjectField("Orientation"));

		OverrideGimbalOrientations.Add(cameraName, bOverride);
		GimbalOrientations.Add(cameraName, rot);
	}
}

TSharedPtr<FJsonValueObject> URouteNode::GetJSONObject()
{
	TSharedPtr<FJsonObject> result = MakeShared<FJsonObject>();

	// When saving, convert coordinates to relative

	FVector relativeCoord = Coord - OwningPlan->RootPosition;

	result->SetObjectField("Coord", JSONUtils::GetJSONForVector(UAirsimConversionFunctions::UUToNed(relativeCoord)));

	result->SetBoolField("bOverrideVelocity", bOverrideVelocity);
	result->SetNumberField("Velocity", Velocity);

	TSharedPtr<FJsonObject> gimbalsObj = MakeShared<FJsonObject>();

	for (auto pair : GimbalOrientations)
	{
		TSharedPtr<FJsonObject> gimbalObj = MakeShared<FJsonObject>();

		gimbalObj->SetBoolField("bOverrideOrientation", OverrideGimbalOrientations[pair.Key]);
		gimbalObj->SetObjectField("Orientation", JSONUtils::GetJSONForRotator(GimbalOrientations[pair.Key]));
		
		gimbalsObj->SetObjectField(pair.Key, gimbalObj);
	}

	result->SetObjectField("Gimbals", gimbalsObj);

	return MakeShared<FJsonValueObject>(result);
}
