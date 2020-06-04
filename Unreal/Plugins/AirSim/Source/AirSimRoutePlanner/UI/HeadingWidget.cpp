// Fill out your copyright notice in the Description page of Project Settings.


#include "HeadingWidget.h"
#include "Components/Widget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"

int32 UHeadingWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	FPaintContext Context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	PaintConeVerts(Context);

	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

void UHeadingWidget::PaintNeedle(FPaintContext& Context) const
{
	if (!BackgroundImage)
		return;

	FVector2D absSize = BackgroundImage->GetPaintSpaceGeometry().GetLocalSize();

	// Determine origin point
	FVector2D origin(absSize.X / 2.0f, absSize.Y / 2.0f);

	// Determine length of line
	float lineLength = GaugeNeedleLength * absSize.X;

	FVector2D baseVector(0, -1.0f);

	// Adjust base vector by orientation and pitch
	float rotAngle = Value;

	FVector2D rotatedVec = baseVector.GetRotated(rotAngle);

	FVector2D needleEndPoint = origin + rotatedVec * lineLength;

	float lineWidth = GaugeNeedleWidth * absSize.X;

	UWidgetBlueprintLibrary::DrawLine(Context, origin, needleEndPoint, OutsideConeColor, true, lineWidth);
}

void UHeadingWidget::PaintCone(FPaintContext& Context) const
{
	if (!BackgroundImage)
		return;

	FVector2D absSize = BackgroundImage->GetPaintSpaceGeometry().GetLocalSize();

	// Determine origin point
	FVector2D origin(absSize.X / 2.0f, absSize.Y / 2.0f);

	// Determine length of line
	float lineLength = GaugeNeedleLength * absSize.X;

	float halfFOV = FOV / 2.0f;

	FVector2D baseVector(0, -1.0f);

	float leastAngle = (Value - halfFOV);

	TArray<FVector2D> Points;

	// Walk the angle toward the other side of the fov, drawing lines as we go

	for (float angle = 0; angle < FOV; angle += 1.0f)
	{
		FVector2D rotatedVec = baseVector.GetRotated(leastAngle + angle);
		FVector2D needleEndPoint = origin + rotatedVec * lineLength;

		Points.Add(origin);
		Points.Add(needleEndPoint);
	}

	UWidgetBlueprintLibrary::DrawLines(Context, Points, OutsideConeColor, true, GaugeNeedleWidth);

}
