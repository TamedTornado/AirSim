// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StringFloatInput.generated.h"

/**
 * This class exists for the lat/long controls, specifically to stop undesired behaviour due to imprecision.

  It will be wrapped around a text input and whatever else and the blueprint code will use the facilities here.

  The string value (used for display) won't change if the input value isn't different enough.

  User inputs new float string
  -Converted to float
  -Displayed Value and String Value both changed to new float
  -Compared for equality with current float value. If equal, no event is fired

 */
UCLASS()
class AIRSIMROUTEPLANNER_API UStringFloatInput : public UUserWidget
{
	GENERATED_BODY()
public:

	// Value of the float in string form with proper amount of digits
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "String Float Input")
	FString								StringValue;

	// The value the string is showing
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "String Float Input")
	float								DisplayedValue;

	// The underlying, actual value
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "String Float Input")
	float								FloatValue;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "String Float Input")
	int32								DecimalPlaces = 6;

	// When comparing the input value with the current value, what do we use for an equality check?
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "String Float Input")
	float								ComparisonTolerance = 0.00001f;

	UFUNCTION(BlueprintCallable, Category = "Gauge widget")
	float GetFloatValue();

	UPROPERTY()
	FGetFloat FloatValueDelegate;

	TAttribute<float> FloatValueBinding;

	virtual void SynchronizeProperties() override;


protected:
	PROPERTY_BINDING_IMPLEMENTATION(float, FloatValue);



};
