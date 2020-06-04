// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseGaugeWidget.h"
#include "GimbalPitchWidget.generated.h"

UENUM(BlueprintType)
enum class EGaugeOrientation : uint8
{
	BottomLeft = 0,
	BottomRight = 1,
	TopRight = 2,
	TopLeft = 3
};

class UImage;
class UMaterialInterface;
class UTexture2D;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class AIRSIMROUTEPLANNER_API UGimbalPitchWidget : public UBaseGaugeWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gauge Data", BlueprintSetter=SetGaugeOrientation)
	EGaugeOrientation					GaugeOrientation = EGaugeOrientation::BottomLeft;

	UFUNCTION(BlueprintCallable, Category = "Gauge Data")
	void								SetGaugeOrientation(EGaugeOrientation newOrientation);



#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif


	void InitBackgroundMaterial_Implementation() override;

protected:


	int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	// This utility function returns the base (0 pitch) vector and the direction needed to go positive 
	virtual FVector GetBaseVectorAndMult() const override;

	void PaintNeedle(FPaintContext& Context) const;
};
