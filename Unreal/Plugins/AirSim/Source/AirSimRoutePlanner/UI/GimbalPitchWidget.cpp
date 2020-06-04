// Fill out your copyright notice in the Description page of Project Settings.


#include "GimbalPitchWidget.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Components/Image.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Styling/CoreStyle.h"
#include "Styling/SlateBrush.h"
#include "Textures/SlateShaderResource.h"
#include "Rendering/RenderingCommon.h"
#include "Rendering/DrawElements.h"
#include "AirSimRoutePlanner.h"

void UGimbalPitchWidget::SetGaugeOrientation(EGaugeOrientation newOrientation)
{
	GaugeOrientation = newOrientation;

	if (!BackgroundMaterialInst)
	{
		BackgroundMaterialInst->SetScalarParameterValue("GaugeRotation", (float)GaugeOrientation * 0.25f);
	}
}

void UGimbalPitchWidget::InitBackgroundMaterial_Implementation()
{
	Super::InitBackgroundMaterial_Implementation();
	
	if (!BackgroundMaterial)
	{
		UE_LOG(LogAirSimRP, Error, TEXT("Unable to init background material instance, no BackgroundMaterial set"));
		return;
	}

	BackgroundMaterialInst->SetScalarParameterValue("GaugeRotation", (float)GaugeOrientation * 0.25f);
}


#if WITH_EDITOR
void UGimbalPitchWidget::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UGimbalPitchWidget, GaugeOrientation))
	{
		if (BackgroundMaterialInst)
		{
			BackgroundMaterialInst->SetScalarParameterValue("GaugeRotation", (float)GaugeOrientation * 0.25f);
		}
	}

}
#endif

int32 UGimbalPitchWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	FPaintContext Context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	PaintConeVerts(Context);


	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

FVector UGimbalPitchWidget::GetBaseVectorAndMult() const
{
	FVector result(ForceInitToZero);

	switch (GaugeOrientation)
	{
	case EGaugeOrientation::BottomLeft: // 0-90 is negative
		result.X = -1.0f;
		result.Y = 0;
		result.Z = -1.0f; // Mult
		break;
	case EGaugeOrientation::BottomRight: // 0-90 is positive
		result.X = 1.0f;
		result.Y = 0;
		result.Z = 1.0f;
		break;
	case EGaugeOrientation::TopRight:// 0=90 is negative
		result.X = 1.0f;
		result.Y = 0;
		result.Z = -1.0f;
		break;
	case EGaugeOrientation::TopLeft: // 0-90 is positive
		result.X = -1.0f;
		result.Y = 0;
		result.Z = 1.0f;
		break;
	default:
		break;
	}

	return result;
}

void UGimbalPitchWidget::PaintNeedle(FPaintContext& Context) const
{
	if (!BackgroundImage)
		return;

	FVector2D absSize = BackgroundImage->GetPaintSpaceGeometry().GetLocalSize();

	// Determine origin point
	FVector2D origin(absSize.X / 2.0f, absSize.Y / 2.0f);

	// Determine length of line
	float lineLength = GaugeNeedleLength * absSize.X;

	FVector baseVectorAndMult = GetBaseVectorAndMult();
	float angleMult = baseVectorAndMult.Z;
	FVector2D baseVector(baseVectorAndMult);

	// Adjust base vector by orientation and pitch
	float rotAngle = Value * angleMult;

	FVector2D rotatedVec = baseVector.GetRotated(rotAngle);

	FVector2D needleEndPoint = origin + rotatedVec * lineLength;

	float lineWidth = GaugeNeedleWidth * absSize.X;

	UWidgetBlueprintLibrary::DrawLine(Context, origin, needleEndPoint, OutsideConeColor, true, lineWidth);
}

