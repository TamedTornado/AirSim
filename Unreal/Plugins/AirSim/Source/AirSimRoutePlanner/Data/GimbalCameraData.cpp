#include "GimbalCameraData.h"
#include "Dom/JsonObject.h"



void FGimbalCameraData::InitWithJSON(TSharedPtr<FJsonObject> jsonObject)
{
	Name = jsonObject->GetStringField("Name");
	MinPitch = jsonObject->GetNumberField("MinPitch");
	MaxPitch = jsonObject->GetNumberField("MaxPitch");
	MinYaw = jsonObject->GetNumberField("MinYaw");
	MaxYaw = jsonObject->GetNumberField("MaxYaw");
	VFOV = jsonObject->GetNumberField("VFOV");
	HFOV = jsonObject->GetNumberField("HFOV");
}

TSharedPtr<FJsonObject> FGimbalCameraData::GetJSONObject()
{
	TSharedPtr<FJsonObject>	result = MakeShared<FJsonObject>();

	result->SetStringField("Name", Name);
	result->SetNumberField("MinPitch", MinPitch);
	result->SetNumberField("MaxPitch", MaxPitch);
	result->SetNumberField("MinYaw", MinYaw);
	result->SetNumberField("MaxYaw", MaxYaw);
	result->SetNumberField("VFOV", VFOV);
	result->SetNumberField("HFOV", HFOV);

	return result;
}
