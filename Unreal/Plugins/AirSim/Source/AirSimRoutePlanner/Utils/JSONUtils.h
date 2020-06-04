#pragma once

class FJsonObject;

class JSONUtils
{
public:
	static TSharedPtr<FJsonObject>		GetJSONForVector(FVector InVector);

	static FVector						GetVectorFromJSON(TSharedPtr<FJsonObject> InJSONObj);

	static TSharedPtr<FJsonObject>		GetJSONForRotator(FRotator InRot);

	static FRotator						GetRotatorFromJSON(TSharedPtr<FJsonObject> InJSONObj);
};
