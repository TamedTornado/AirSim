#include "RoutePlan.h"
#include "RouteNode.h"
#include "AirSimRoutePlanner.h"
#include "Utils/JSONUtils.h"


void URoutePlan::SetRoutePlanName(FString newName)
{
	RoutePlanName = newName;

	bDetailsChanged = true;
}

void URoutePlan::SetDefaultAltitude(float newAltitude)
{
	DefaultAltitude = newAltitude;

	bDetailsChanged = true;
}

void URoutePlan::SetDefaultVelocity(float newVelocity)
{
	DefaultVelocity = newVelocity;

	bDetailsChanged = true;
}

void URoutePlan::SetDefaultGimbalOrientation(FString cameraKey, FRotator newOrientation)
{
	if (!DefaultGimbalOrientations.Contains(cameraKey))
	{
		DefaultGimbalOrientations.Add(cameraKey, newOrientation);
	}

	DefaultGimbalOrientations[cameraKey] = newOrientation;

	bDetailsChanged = true;
}

void URoutePlan::SetLastNodeBehaviour(ELastNodeBehaviour NewLastNodeBehaviour)
{
	if (LastNodeBehaviour != NewLastNodeBehaviour)
	{
		LastNodeBehaviour = NewLastNodeBehaviour;

		OnLastNodeBehaviourChanged.Broadcast();
	}
}

URouteNode* URoutePlan::AddNewNode(FVector InCoord)
{
	URouteNode* newNode = NewObject<URouteNode>(this, NAME_None);
	newNode->OwningPlan = this;
	newNode->Coord = InCoord;
	
	// Grab defaults and set them for overrides
	newNode->Velocity = DefaultVelocity;

	// Add an entry for every gimbal camera!
	for (FGimbalCameraData& gcd : GimbalCameraData)
	{
		newNode->GimbalOrientations.Add(gcd.Name, FRotator::ZeroRotator);
		newNode->OverrideGimbalOrientations.Add(gcd.Name, false);
	}
	
	RouteNodes.Add(newNode);

	OnNodeAdded.Broadcast(newNode);

	bDetailsChanged = true;

	newNode->OnNodeDetailsChanged.AddDynamic(this, &URoutePlan::HandleNodeDetailsChanged);

	return newNode;
}

URouteNode* URoutePlan::InsertNodeAfter(URouteNode* RouteNodeData, FVector InCoord)
{
	URouteNode* newNode = NewObject<URouteNode>(this, NAME_None);
	newNode->OwningPlan = this;
	newNode->Coord = InCoord;

	// Grab defaults and set them for overrides
	newNode->Velocity = DefaultVelocity;

	// Add an entry for every gimbal camera!
	for (FGimbalCameraData& gcd : GimbalCameraData)
	{
		newNode->GimbalOrientations.Add(gcd.Name, FRotator::ZeroRotator);
		newNode->OverrideGimbalOrientations.Add(gcd.Name, false);
	}

	int32 index;
	
	if (RouteNodes.Find(RouteNodeData, index))
	{
		RouteNodes.Insert(newNode, index+1);
	}

	bDetailsChanged = true;

	newNode->OnNodeDetailsChanged.AddDynamic(this, &URoutePlan::HandleNodeDetailsChanged);

	return newNode;
}


void URoutePlan::DeleteRouteNode(URouteNode* nodeData)
{
	if (!RouteNodes.Contains(nodeData))
	{
		UE_LOG(LogAirSimRP, Warning, TEXT("Failed to find route node we're trying to delete in the route plan. Probably a bug!"));
		return;
	}

	RouteNodes.Remove(nodeData);

	OnNodeRemoved.Broadcast(nodeData);

	bDetailsChanged = true;
}

void URoutePlan::HandleNodeDetailsChanged(URouteNode* nodeChanged)
{
	bDetailsChanged = true;
}

bool URoutePlan::LoadFromJSONString(FString jsonString)
{
	RouteNodes.Empty();
	DefaultGimbalOrientations.Empty();

	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(jsonString);
	
	TSharedPtr<FJsonValue> output;

	if (FJsonSerializer::Deserialize(Reader, output))
	{
		TSharedPtr<FJsonObject> jsonObj = output->AsObject();

		RoutePlanName = jsonObj->GetStringField("Name");

		// Get the root position
		RootPosition = UAirsimConversionFunctions::NedToUU(JSONUtils::GetVectorFromJSON(jsonObj->GetObjectField("RootPosition")));

		DefaultVelocity = jsonObj->GetNumberField("DefaultVelocity");
		DefaultAltitude = jsonObj->GetNumberField("DefaultAltitude");

		// Load the default gimbal cam orientations
		TSharedPtr<FJsonObject> gimbalsObj = jsonObj->GetObjectField("Gimbals");

		for (auto pair : gimbalsObj->Values)
		{
			TSharedPtr<FJsonObject> gimbalObj = pair.Value->AsObject();

			DefaultGimbalOrientations.Add(pair.Key, JSONUtils::GetRotatorFromJSON(gimbalObj->GetObjectField("DefaultOrientation")));

			FGimbalCameraData& gcd = GimbalCameraData.AddDefaulted_GetRef();
			gcd.InitWithJSON(gimbalObj->GetObjectField("GimbalCameraData"));
		}

		LastNodeBehaviour = (ELastNodeBehaviour) (uint8) jsonObj->GetNumberField("LastNodeBehaviour");

		auto nodesData = jsonObj->GetArrayField("Nodes");

		for (TSharedPtr<FJsonValue> nodeData : nodesData)
		{
			URouteNode* newNode = NewObject<URouteNode>(this, NAME_None);
			newNode->OwningPlan = this;

			newNode->InitWithJSON(nodeData->AsObject());

			newNode->OnNodeDetailsChanged.AddDynamic(this, &URoutePlan::HandleNodeDetailsChanged);


			RouteNodes.Add(newNode);
		}

		bDetailsChanged = true;

		return true;
	}

	return false;
}

bool URoutePlan::LoadFromJSONFile(FString jsonFullPath)
{
	FString loadedString;

	if (FFileHelper::LoadFileToString(loadedString, *jsonFullPath))
	{
		if (LoadFromJSONString(loadedString))
		{
			Filename = jsonFullPath;
			bDetailsChanged = false;
			return true;
		}
	}

	return false;
}

bool URoutePlan::SaveToJSONString(FString &OutputString)
{
	if (RouteNodes.Num() == 0)
	{
		UE_LOG(LogAirSimRP, Error, TEXT("Can't save route plan that has no nodes!"));
		return false;
	}

	// Make sure we grab the root position
//	RootPosition = RouteNodes[0]->Coord;

	TSharedPtr<FJsonObject> json = MakeShared<FJsonObject>();

	json->SetStringField("Name", RoutePlanName);

	json->SetObjectField("RootPosition", JSONUtils::GetJSONForVector(UAirsimConversionFunctions::UUToNed(RootPosition)));

	json->SetNumberField("DefaultVelocity", DefaultVelocity);
	json->SetNumberField("DefaultAltitude", DefaultAltitude);

	// Save the gimbal camera data
	TSharedPtr<FJsonObject> gimbalsObj = MakeShared<FJsonObject>();

	for (FGimbalCameraData& gcd : GimbalCameraData)
	{
		TSharedPtr<FJsonObject> gimbalObj = MakeShared<FJsonObject>();

		FRotator defaultOrientation = DefaultGimbalOrientations[gcd.Name];

		gimbalObj->SetObjectField("DefaultOrientation", JSONUtils::GetJSONForRotator(defaultOrientation));

		// Save the actual camera data
		gimbalObj->SetObjectField("GimbalCameraData", gcd.GetJSONObject());

		gimbalsObj->SetObjectField(gcd.Name, gimbalObj);
	}

	json->SetObjectField("Gimbals", gimbalsObj);

	json->SetNumberField("LastNodeBehaviour", (uint8) LastNodeBehaviour);

	TArray<TSharedPtr<FJsonValue>> jsonNodes;

	for (URouteNode* node : RouteNodes)
	{
		jsonNodes.Add(node->GetJSONObject());
	}

	json->SetArrayField("Nodes", jsonNodes);


	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
	return FJsonSerializer::Serialize(json.ToSharedRef(), Writer);
}

bool URoutePlan::SaveAs(FString jsonFullPath)
{
	if (RouteNodes.Num() == 0)
	{
		UE_LOG(LogAirSimRP, Error, TEXT("Can't save route plan that has no nodes!"));
		return false;
	}

	FString jsonString;

	if (SaveToJSONString(jsonString))
	{
//		auto Path = FString(FPlatformProcess::UserDir()) + "AirSim/"+jsonFileName+".json";

		if (!FFileHelper::SaveStringToFile(jsonString, *jsonFullPath))
		{
			UE_LOG(LogAirSimRP, Error, TEXT("Failed to save route plan to %s"), *jsonFullPath);
			return false;
		}

		bDetailsChanged = false;

		Filename = jsonFullPath;

		return true;
	}

	return false;
}

bool URoutePlan::Save()
{
	if (Filename.IsEmpty())
	{
		return false;
	}

	return SaveAs(Filename);
}

