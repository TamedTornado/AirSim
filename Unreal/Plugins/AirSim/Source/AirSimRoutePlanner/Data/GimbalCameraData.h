#pragma once

#include "GimbalCameraData.generated.h"

USTRUCT(BlueprintType)
struct FGimbalCameraData
{
	GENERATED_BODY()
public:
	
	UPROPERTY(BlueprintReadOnly, Category = "Gimbal Camera Data")
	FString						Name;

	UPROPERTY(BlueprintReadOnly, Category = "Gimbal Camera Data")
	float						MinPitch = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Gimbal Camera Data")
	float						MaxPitch = 90.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Gimbal Camera Data")
	float						MinYaw = -179.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Gimbal Camera Data")
	float						MaxYaw = 179.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Gimbal Camera Data")
	float						VFOV;

	UPROPERTY(BlueprintReadOnly, Category = "Gimbal Camera Data")
	float						HFOV;

	void					InitWithJSON(TSharedPtr<FJsonObject> jsonObject);

	// Save this node to a FJsonValueObject and return it
	TSharedPtr<FJsonObject> GetJSONObject();

};