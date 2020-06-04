// Fill out your copyright notice in the Description page of Project Settings.


#include "MapScaleWidget.h"
#include "UObject/NoExportTypes.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

int32 UMapScaleWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	
	FPaintContext Context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);


	FVector2D absSize = AllottedGeometry.GetLocalSize();

	// Draw the bottom line
	UWidgetBlueprintLibrary::DrawLine(Context, FVector2D(0, absSize.Y), FVector2D(absSize.X, absSize.Y), LineColor, true, LineThickness);

	// The left line
	UWidgetBlueprintLibrary::DrawLine(Context, FVector2D(0, 0), FVector2D(0, absSize.Y), LineColor, true, LineThickness);

	// The right line
	UWidgetBlueprintLibrary::DrawLine(Context, FVector2D(absSize.X, 0), FVector2D(absSize.X, absSize.Y), LineColor, true, LineThickness);

	float centerX = absSize.X / 2.0f;

	// The center line (half height)
	float lineHeight = absSize.Y / 2.0f;
	UWidgetBlueprintLibrary::DrawLine(Context, FVector2D(centerX, absSize.Y), FVector2D(centerX, absSize.Y - lineHeight), LineColor, true, LineThickness);

	// The subdivisions 
	lineHeight/=2.0f;

	float halfWidth = centerX;
	float quarterWidth = halfWidth / 2.0f;
	float eighthWidth = quarterWidth / 2.0f;

	// First level
	float leftCenter = centerX - quarterWidth;
	float rightCenter = centerX + quarterWidth;
	UWidgetBlueprintLibrary::DrawLine(Context, FVector2D(leftCenter, absSize.Y), FVector2D(leftCenter, absSize.Y - lineHeight), LineColor, true, LineThickness);
	UWidgetBlueprintLibrary::DrawLine(Context, FVector2D(rightCenter, absSize.Y), FVector2D(rightCenter, absSize.Y - lineHeight), LineColor, true, LineThickness);

	// Second level
	lineHeight/=2.0f;

	// left
	UWidgetBlueprintLibrary::DrawLine(Context, FVector2D(leftCenter-eighthWidth, absSize.Y), FVector2D(leftCenter-eighthWidth, absSize.Y - lineHeight), LineColor, true, LineThickness);
	UWidgetBlueprintLibrary::DrawLine(Context, FVector2D(leftCenter+eighthWidth, absSize.Y), FVector2D(leftCenter+eighthWidth, absSize.Y - lineHeight), LineColor, true, LineThickness);

	// Right
	UWidgetBlueprintLibrary::DrawLine(Context, FVector2D(rightCenter - eighthWidth, absSize.Y), FVector2D(rightCenter - eighthWidth, absSize.Y - lineHeight), LineColor, true, LineThickness);
	UWidgetBlueprintLibrary::DrawLine(Context, FVector2D(rightCenter + eighthWidth, absSize.Y), FVector2D(rightCenter + eighthWidth, absSize.Y - lineHeight), LineColor, true, LineThickness);

	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}
