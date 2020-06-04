#pragma once

#include "Blueprint/UserWidget.h"
#include "BaseGaugeWidget.generated.h"

class UImage;
class UMaterialInterface;
class UTexture2D;

UCLASS(Blueprintable, BlueprintType, Abstract)
class AIRSIMROUTEPLANNER_API UBaseGaugeWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Widgets")
	UImage*								BackgroundImage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Appearance")
	UMaterialInterface*					BackgroundMaterial;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Appearance", Transient)
	UMaterialInstanceDynamic*			BackgroundMaterialInst;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Appearance")
	UTexture2D*							GaugeTexture;

	// What color does the white in the texture convert to in the material?
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Appearance")
	FLinearColor						GaugeColor = FLinearColor::White;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Appearance")
	FLinearColor						BackgroundColor = FLinearColor::Black;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Appearance")
	FLinearColor						OutsideConeColor = FLinearColor::Yellow;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Appearance")
	FLinearColor						InsideConeColor = FLinearColor::Red;

	// Needle/cone length as percentage of width of gauge
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Gauge Data")
	float								GaugeNeedleLength = 0.42f;

	// With of the needle in percentage of texture size
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Gauge Data")
	float								GaugeNeedleWidth = 0.034f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Appearance")
	float								ConeStepAngle = 5.0f;

	// How much FOV in degrees does this gimbal have?
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Appearance")
	float								FOV = 24.0f;

	UPROPERTY()
	FSlateBrush							NeedleBrush;


	// Creates the MID and sets the params at run time
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Appearance")
	void								InitBackgroundMaterial();
	virtual void						InitBackgroundMaterial_Implementation();

	// Assigns the background material instance to the background image
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Appearance")
	void								InitBackgroundImage();
	virtual void						InitBackgroundImage_Implementation();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gauge widget")
	float	Value;

	UFUNCTION(BlueprintCallable, Category = "Gauge widget")
	float GetValue();

	UPROPERTY()
	FGetFloat ValueDelegate;

	TAttribute<float> ValueBinding;

	virtual void SynchronizeProperties() override;


protected:
	PROPERTY_BINDING_IMPLEMENTATION(float, Value);

	virtual FVector GetBaseVectorAndMult() const;

	void NativeConstruct() override;
	void NativePreConstruct() override;

	void PaintConeVerts(FPaintContext& Context) const;
};