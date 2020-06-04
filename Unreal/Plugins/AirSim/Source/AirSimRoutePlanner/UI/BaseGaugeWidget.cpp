#include "BaseGaugeWidget.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/Image.h"
#include "AirSimRoutePlanner.h"



void UBaseGaugeWidget::InitBackgroundMaterial_Implementation()
{
	// Create the MID for this image

	if (!BackgroundMaterial)
	{
		UE_LOG(LogAirSimRP, Error, TEXT("Unable to init background material instance, no BackgroundMaterial set"));
		return;
	}

	if (!BackgroundMaterialInst)
		BackgroundMaterialInst = UMaterialInstanceDynamic::Create(BackgroundMaterial, this, "BGMaterialInstance");
	// Set the params

	BackgroundMaterialInst->SetVectorParameterValue("BGColor", BackgroundColor);
	BackgroundMaterialInst->SetVectorParameterValue("GaugeColor", GaugeColor);
	BackgroundMaterialInst->SetTextureParameterValue("GaugeTexture", GaugeTexture);
}

void UBaseGaugeWidget::InitBackgroundImage_Implementation()
{
	if (!BackgroundMaterialInst)
	{
		UE_LOG(LogAirSimRP, Error, TEXT("Unable to init background image because no BackgroundMaterialInst"));
		return;
	}

	BackgroundImage->SetBrushFromMaterial(BackgroundMaterialInst);
}

float UBaseGaugeWidget::GetValue()
{
	return ValueBinding.Get();
}

void UBaseGaugeWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	ValueBinding = PROPERTY_BINDING(float, Value);
	Value = ValueBinding.Get();
}

void UBaseGaugeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// At this point, BackgroundImage should be set!

	if (!BackgroundImage)
	{
		UE_LOG(LogAirSimRP, Error, TEXT("Unable to initialize BaseGaugeWidget - BackgroundImage not set in Construct()"));
		return;
	}

	InitBackgroundMaterial();

	InitBackgroundImage();
}

FVector UBaseGaugeWidget::GetBaseVectorAndMult() const
{
	return FVector(0, -1.0f, 1.0f);
}

void UBaseGaugeWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsDesignTime())
	{
		if (!BackgroundImage)
		{
			UE_LOG(LogAirSimRP, Error, TEXT("Unable to initialize BaseGaugeWidget - BackgroundImage not set in Construct()"));
			return;
		}

		InitBackgroundMaterial();

		InitBackgroundImage();
	}
}

void UBaseGaugeWidget::PaintConeVerts(FPaintContext& Context) const
{
	if (!BackgroundImage)
		return;

	FVector2D absSize = BackgroundImage->GetPaintSpaceGeometry().GetLocalSize();


	// Determine origin point
	FVector2D origin(absSize.X / 2.0f, absSize.Y / 2.0f);

	// Determine length of line
	float lineLength = GaugeNeedleLength * absSize.X;

	float halfFOV = FOV / 2.0f;

	// need NeedleBrush for the Handle but seems dumb. TODO: Get the handle somewhere else
	FSlateResourceHandle Handle = NeedleBrush.GetRenderingResource();
	const FSlateShaderResourceProxy* ResourceProxy = Handle.GetResourceProxy();

	TArray<FSlateVertex> Verts;
	Verts.Reserve(4); // Two tris first
	FColor vertColor = OutsideConeColor.ToFColor(false);

	FVector2D absOrigin = Context.AllottedGeometry.LocalToAbsolute(origin);
	// Add the center vert as vert 0
	{
		FSlateVertex& vert = Verts.AddZeroed_GetRef();

		vert.Position[0] = absOrigin.X;
		vert.Position[1] = absOrigin.Y;

		vert.TexCoords[0] = 0;
		vert.TexCoords[1] = 0;
		vert.TexCoords[2] = vert.TexCoords[3] = 1.0f;
		vert.Color = InsideConeColor.ToFColor(false);
	}

	FVector baseVectorAndMult = GetBaseVectorAndMult();
	FVector2D baseVector(baseVectorAndMult);

	float effAngle = Value * baseVectorAndMult.Z; // Flip the sign if needed

	float leastAngle = (effAngle - halfFOV);
	float highestAngle = leastAngle + FOV;

	TArray<SlateIndex> Indexes;


	// Add the two extreme verts
	{
		FVector2D rotatedVec = baseVector.GetRotated(leastAngle);
		FVector2D needleEndPoint = Context.AllottedGeometry.LocalToAbsolute(origin + rotatedVec * lineLength);

		FSlateVertex& vert = Verts.AddZeroed_GetRef();
		vert.Position[0] = needleEndPoint.X;
		vert.Position[1] = needleEndPoint.Y;

		// 		vert.TexCoords[0] = 1.0f;
		// 		vert.TexCoords[1] = 0;
		// 		vert.TexCoords[2] = vert.TexCoords[3] = 1.0f;
		vert.Color = vertColor;
	}

	// Start by pointing to the vert in the 1 slot, the "least angle" vert
	int32 lastVertIndex = 1;

	// Step through the remaining angle, adding a vertex every ConeStepAngle degrees
	for (float angle = ConeStepAngle; angle < FOV - ConeStepAngle; angle += ConeStepAngle)
	{
		float adjAngle = leastAngle + angle;

		FVector2D rotatedVec = baseVector.GetRotated(adjAngle);
		FVector2D needleEndPoint = Context.AllottedGeometry.LocalToAbsolute(origin + rotatedVec * lineLength);

		FSlateVertex& vert = Verts.AddZeroed_GetRef();
		vert.Position[0] = needleEndPoint.X;
		vert.Position[1] = needleEndPoint.Y;
		vert.Color = vertColor;

		Indexes.Add(0); // Root vert
		Indexes.Add(lastVertIndex); // Previous vert
		Indexes.Add(++lastVertIndex); // Current vert and inc
	}

	{
		FVector2D rotatedVec = baseVector.GetRotated(leastAngle + FOV);
		FVector2D needleEndPoint = Context.AllottedGeometry.LocalToAbsolute(origin + rotatedVec * lineLength);
		//		FVector2D needleEndPoint = absOrigin + rotatedVec * lineLength;

		FSlateVertex& vert = Verts.AddZeroed_GetRef();
		vert.Position[0] = needleEndPoint.X;
		vert.Position[1] = needleEndPoint.Y;

		vert.Color = vertColor;
		// Add final triangle
		Indexes.Add(0);
		Indexes.Add(lastVertIndex);
		Indexes.Add(++lastVertIndex);
	}



	FSlateDrawElement::MakeCustomVerts(Context.OutDrawElements, Context.LayerId, Handle, Verts, Indexes, nullptr, 0, 0);
}
