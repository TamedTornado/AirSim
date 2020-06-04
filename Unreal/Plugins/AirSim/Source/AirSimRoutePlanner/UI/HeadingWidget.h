// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGaugeWidget.h"
#include "HeadingWidget.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class AIRSIMROUTEPLANNER_API UHeadingWidget : public UBaseGaugeWidget
{
	GENERATED_BODY()
public:


protected:

	int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	void PaintNeedle(FPaintContext& Context) const;
	void PaintCone(FPaintContext& Context) const;
};
