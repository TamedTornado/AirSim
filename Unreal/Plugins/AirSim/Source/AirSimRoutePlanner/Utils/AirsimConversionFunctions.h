// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AirsimConversionFunctions.generated.h"

struct FVector3D;

namespace msr { namespace airlib { class GeodeticConverter; } }

/**
 * 
 */
UCLASS()
class AIRSIMROUTEPLANNER_API UAirsimConversionFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintCallable, Category = "AirSim Conversion")
	static FText						FormatSpeedWithMeasurement(float speedInMPS);

	// Returns speed in m/sec or feet/sec depending on metric or SI switch
	UFUNCTION(BlueprintCallable, Category = "AirSim Conversion")
	static FText						FormatSpeed(float speedInMPS);

	// Returns m/sec or feet/sec depending on metric or SI switch
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AirSim Conversion")
	static FText						GetSpeedMeasurement();

	UFUNCTION(BlueprintCallable, Category = "AirSim Conversion")
	static FText						FormatAltitudeWithMeasurement(float altInCM);

	UFUNCTION(BlueprintCallable, Category = "AirSim Conversion")
	static FText						FormatAltitude(float altInCM);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AirSim Conversion")
	static FText						GetAltitudeMeasurement();

	UFUNCTION(BlueprintCallable, Category = "AirSim Conversion")
	static FText						FormatAltitudeMeters(float altInM);

	// Convert a distance in meters to text in the style '100 m' to '1.1 km' or '100 ft' to '1.1 mi.'
	UFUNCTION(BlueprintCallable, Category = "AirSim Conversion")
	static FText						FormatDistance(float distanceInM);

	UFUNCTION(BlueprintCallable, Category = "AirSim Conversion")
	static bool							IsAirsimUsingMetric();

	// Takes input either in mps or fps and converts it to the MPS of the route plan
	UFUNCTION(BlueprintCallable, Category = "AirSim Conversion")
	static float						ConvertSpeedInput(float speed);

	// Takes input either in meters or feet and converts it to meters
	UFUNCTION(BlueprintCallable, Category = "AirSim Conversion")
	static float						ConvertHeightInput(float height);

	UFUNCTION(BlueprintCallable, Category = "AirSm coordinates")
	static FVector						UUToNed(FVector inUUCoord);

	static FVector3D					UUToNedDouble(FVector3D inUUCoord);


	UFUNCTION(BlueprintCallable, Category = "AirSm coordinates")
	static FVector						NedToUU(FVector inNedCoord);
	
	static FVector3D					NedToUUDouble(FVector3D InNedCoord);

	UFUNCTION(BlueprintCallable, Category = "AirSm coordinates")
	static FVector						GetOriginGeoPoint();

	/*
	 Converts NED coordinate to Geodetic, taking home geopoint into account. Uses floats and UE4 math instead of doubles and the AirSim types

	 Returns a FVector with lat, long and altitude
	*/
	UFUNCTION(BlueprintCallable, Category = "AirSim coordinates")
	static FVector						NedToGeodetic(FVector inNed);

	UFUNCTION(BlueprintCallable, Category = "AirSim coordinates")
	static FVector						GeodeticToNed(FVector inGeodetic);

	static FVector3D					GeodeticToNedDouble(FVector3D inGeodetic);

	UFUNCTION(BlueprintCallable, Category = "AirSim coordinates")
	static FVector						GeodeticToUU(FVector inGeodetic);

	static FVector3D					GeodeticToUUDouble(FVector3D inGeodetic);

	UFUNCTION(BlueprintCallable, Category = "AirSim coordinates")
	static FVector						UUToGeodetic(FVector inUU);

	static FVector3D					UUToGeodeticDouble(FVector3D inUU);

	static TSharedPtr<msr::airlib::GeodeticConverter>	GetGeoConverter();

protected:

	// Make sure to use the accessor method GetGeoConverter() to assure that it's created.
	static TSharedPtr<msr::airlib::GeodeticConverter>	GeoConverter;
};
