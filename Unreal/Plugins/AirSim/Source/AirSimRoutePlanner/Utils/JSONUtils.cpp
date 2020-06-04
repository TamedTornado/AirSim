#include "JSONUtils.h"
#include "Dom/JsonObject.h"



TSharedPtr<FJsonObject> JSONUtils::GetJSONForVector(FVector InVector)
{
	TSharedPtr<FJsonObject> vectorObj = MakeShared<FJsonObject>();

	vectorObj->SetNumberField("X", InVector.X);
	vectorObj->SetNumberField("Y", InVector.Y);
	vectorObj->SetNumberField("Z", InVector.Z);

	return vectorObj;
}

FVector JSONUtils::GetVectorFromJSON(TSharedPtr<FJsonObject> InJSONObj)
{
	FVector result;

	result.X = InJSONObj->GetNumberField("X");
	result.Y = InJSONObj->GetNumberField("Y");
	result.Z = InJSONObj->GetNumberField("Z");

	return result;
}

TSharedPtr<FJsonObject> JSONUtils::GetJSONForRotator(FRotator InRot)
{
	TSharedPtr<FJsonObject> rotObj = MakeShared<FJsonObject>();

	rotObj->SetNumberField("Pitch", InRot.Pitch);
	rotObj->SetNumberField("Yaw", InRot.Yaw);
	rotObj->SetNumberField("Roll", InRot.Roll);

	return rotObj;
}

FRotator JSONUtils::GetRotatorFromJSON(TSharedPtr<FJsonObject> InJSONObj)
{
	FRotator result;

	result.Pitch = InJSONObj->GetNumberField("Pitch");
	result.Yaw = InJSONObj->GetNumberField("Yaw");
	result.Roll = InJSONObj->GetNumberField("Roll");

	return result;
}
