// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MapScaleWidget.generated.h"

/**
 * 
 */
UCLASS()
class AIRSIMROUTEPLANNER_API UMapScaleWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "Map Scale Appearance")
	float						LineThickness = 5.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Map Scale Appearance")
	FLinearColor				LineColor = FLinearColor::White;

	// What is the scale depicted in meters?
	UPROPERTY(BlueprintReadWrite, Category = "Map Scale Appearance")
	float						ScaleDepicted = 1000.0f;

protected:
	int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

};
