// Fill out your copyright notice in the Description page of Project Settings.


#include "AirsimConversionFunctions.h"
#include "AirSimPluginSettings.h"
#include "Internationalization/Text.h"
#include "common/AirSimSettings.hpp"
#include "common/CommonStructs.hpp"
#include "common/GeodeticConverter.hpp"

#define EARTH_RADIUS (6378137.0f)


TSharedPtr<msr::airlib::GeodeticConverter> UAirsimConversionFunctions::GeoConverter;


FText UAirsimConversionFunctions::FormatSpeedWithMeasurement(float speedInMPS)
{
	bool bUseMetric = GetDefault<UAirSimPluginSettings>()->bUseMetricMeasurements;

	FNumberFormattingOptions NumberFormatOptions;
	NumberFormatOptions.AlwaysSign = false;
	NumberFormatOptions.UseGrouping = true;
	NumberFormatOptions.RoundingMode = ERoundingMode::HalfToEven;
	NumberFormatOptions.MinimumIntegralDigits = 1;
	NumberFormatOptions.MaximumIntegralDigits = 10;
	NumberFormatOptions.MinimumFractionalDigits = 0;
	NumberFormatOptions.MaximumFractionalDigits = 1;

	if (bUseMetric)
	{
		return FText::Format(NSLOCTEXT("AirSim", "SpeedInMPS", "{0} m/s"), FText::AsNumber(speedInMPS, &NumberFormatOptions));
	}

	float fps = speedInMPS * 3.281f;

	return FText::Format(NSLOCTEXT("AirSim", "SpeedInFPS", "{0} ft/s"), FText::AsNumber(fps, &NumberFormatOptions));
}

FText UAirsimConversionFunctions::FormatSpeed(float speedInMPS)
{
	bool bUseMetric = GetDefault<UAirSimPluginSettings>()->bUseMetricMeasurements;

	FNumberFormattingOptions NumberFormatOptions;
	NumberFormatOptions.AlwaysSign = false;
	NumberFormatOptions.UseGrouping = true;
	NumberFormatOptions.RoundingMode = ERoundingMode::HalfToEven;
	NumberFormatOptions.MinimumIntegralDigits = 1;
	NumberFormatOptions.MaximumIntegralDigits = 10;
	NumberFormatOptions.MinimumFractionalDigits = 0;
	NumberFormatOptions.MaximumFractionalDigits = 1;

	if (bUseMetric)
	{
//		float kph = speedInMPS * 3.6f;

		return FText::AsNumber(speedInMPS, &NumberFormatOptions);
	}

	float fps = speedInMPS * 3.281f;

	return FText::AsNumber(fps, &NumberFormatOptions);
}

FText UAirsimConversionFunctions::GetSpeedMeasurement()
{
	bool bUseMetric = GetDefault<UAirSimPluginSettings>()->bUseMetricMeasurements;

	return bUseMetric ? NSLOCTEXT("AirSim", "MPS", "m/s") : NSLOCTEXT("AirSim", "FPS", "ft/s");
}

FText UAirsimConversionFunctions::FormatAltitudeWithMeasurement(float altIntCM)
{
	bool bUseMetric = GetDefault<UAirSimPluginSettings>()->bUseMetricMeasurements;

	FNumberFormattingOptions NumberFormatOptions;
	NumberFormatOptions.AlwaysSign = false;
	NumberFormatOptions.UseGrouping = true;
	NumberFormatOptions.RoundingMode = ERoundingMode::HalfToEven;
	NumberFormatOptions.MinimumIntegralDigits = 1;
	NumberFormatOptions.MaximumIntegralDigits = 10;
	NumberFormatOptions.MinimumFractionalDigits = 0;
	NumberFormatOptions.MaximumFractionalDigits = 1;

	float altInMeters = altIntCM / 100.0f;

	if (bUseMetric)
	{
		return FText::Format(NSLOCTEXT("AirSim", "AltInMeters", "{0} m"), FText::AsNumber(altInMeters, &NumberFormatOptions));
	}

	float feet = altIntCM / 30.48f;

	return FText::Format(NSLOCTEXT("AirSim", "AltInFeet", "{0} ft"), FText::AsNumber(feet, &NumberFormatOptions));
}

FText UAirsimConversionFunctions::FormatAltitude(float altInCM)
{
	bool bUseMetric = GetDefault<UAirSimPluginSettings>()->bUseMetricMeasurements;

	FNumberFormattingOptions NumberFormatOptions;
	NumberFormatOptions.AlwaysSign = false;
	NumberFormatOptions.UseGrouping = true;
	NumberFormatOptions.RoundingMode = ERoundingMode::HalfToEven;
	NumberFormatOptions.MinimumIntegralDigits = 1;
	NumberFormatOptions.MaximumIntegralDigits = 10;
	NumberFormatOptions.MinimumFractionalDigits = 0;
	NumberFormatOptions.MaximumFractionalDigits = 1;

	float altInMeters = altInCM / 100.0f;

	if (bUseMetric)
	{
		return FText::AsNumber(altInMeters, &NumberFormatOptions);
	}

	float feet = altInCM / 30.48f;

	return FText::AsNumber(feet, &NumberFormatOptions);
}

FText UAirsimConversionFunctions::GetAltitudeMeasurement()
{
	bool bUseMetric = GetDefault<UAirSimPluginSettings>()->bUseMetricMeasurements;

	return bUseMetric ? NSLOCTEXT("AirSim", "Meters", "m") : NSLOCTEXT("AirSim", "Feet", "ft");
}

FText UAirsimConversionFunctions::FormatAltitudeMeters(float altInM)
{
	return FormatAltitudeWithMeasurement(altInM*100.0f);
}


FText UAirsimConversionFunctions::FormatDistance(float distanceInM)
{
	bool bUseMetric = GetDefault<UAirSimPluginSettings>()->bUseMetricMeasurements;

	FNumberFormattingOptions NumberFormatOptions;
	NumberFormatOptions.AlwaysSign = false;
	NumberFormatOptions.UseGrouping = true;
	NumberFormatOptions.RoundingMode = ERoundingMode::HalfToEven;
	NumberFormatOptions.MinimumIntegralDigits = 1;
	NumberFormatOptions.MaximumIntegralDigits = 10;
	NumberFormatOptions.MinimumFractionalDigits = 0;
	NumberFormatOptions.MaximumFractionalDigits = 1;

	if (bUseMetric)
	{
		if (distanceInM >= 1000.0f)
		{
			// Convert to KM

			return FText::Format(NSLOCTEXT("AirSim", "DistanceInKM", "{0} km"), FText::AsNumber(distanceInM / 1000.0f, &NumberFormatOptions));
		}

		return FText::Format(NSLOCTEXT("AirSim", "DistanceInM", "{0} m"), FText::AsNumber(distanceInM, &NumberFormatOptions));
	}

	// Convert to feet
	float distanceInFeet = distanceInM * 3.281f;

	if (distanceInFeet >= 5280) // Mile or longer
	{
		return FText::Format(NSLOCTEXT("AirSim", "DistanceInMiles", "{0} mi."), FText::AsNumber(distanceInFeet / 5280.0f, &NumberFormatOptions));
	}

	return FText::Format(NSLOCTEXT("AirSim", "DistanceInFeet", "{0} ft"), FText::AsNumber(distanceInFeet, &NumberFormatOptions));
}

bool UAirsimConversionFunctions::IsAirsimUsingMetric()
{
	return GetDefault<UAirSimPluginSettings>()->bUseMetricMeasurements;
}

float UAirsimConversionFunctions::ConvertSpeedInput(float speed)
{
	bool bUseMetric = GetDefault<UAirSimPluginSettings>()->bUseMetricMeasurements;

	if (bUseMetric)
	{
		return speed;
	}

	return speed * 3.281f;
}

float UAirsimConversionFunctions::ConvertHeightInput(float height)
{
	bool bUseMetric = GetDefault<UAirSimPluginSettings>()->bUseMetricMeasurements;

	if (bUseMetric)
	{
		return height;
	}

	return height * 0.3048f;
}

FVector UAirsimConversionFunctions::UUToNed(FVector inUUCoord)
{
	float scale = 0.01f; // Convert cm to meters

	return FVector(inUUCoord.X * scale, inUUCoord.Y * scale, (-inUUCoord.Z) * scale);
// 
// 	return Vector3r(vec.X * scale, vec.Y * scale,
// 		(convert_to_ned ? -vec.Z : vec.Z)  * scale);
}

FVector UAirsimConversionFunctions::NedToUU(FVector inNedCoord)
{
	float scale = 100.0f; // Convert meters to cm

	return FVector(inNedCoord.X * scale, inNedCoord.Y * scale, (-inNedCoord.Z) * scale);
}


FVector UAirsimConversionFunctions::GetOriginGeoPoint()
{
	msr::airlib::HomeGeoPoint hgp = msr::airlib::AirSimSettings::singleton().origin_geopoint;

	return FVector(hgp.home_geo_point.latitude, hgp.home_geo_point.longitude, hgp.home_geo_point.altitude);
}

FVector UAirsimConversionFunctions::NedToGeodetic(FVector inNed)
{
	FVector inOriginGeoPoint = GetOriginGeoPoint();

	float x_rad = inNed.X / EARTH_RADIUS;
	float y_rad = inNed.Y / EARTH_RADIUS;
	float c = FMath::Sqrt(x_rad*x_rad + y_rad * y_rad);

	float sin_c = FMath::Sin(c), cos_c = FMath::Cos(c);
	float lat_rad, lon_rad;

	if (!FMath::IsNearlyZero(c)) { //avoids large changes?

		float hgp_lat_rad = FMath::DegreesToRadians(inOriginGeoPoint.X);
		float hgp_lon_rad = FMath::DegreesToRadians(inOriginGeoPoint.Y);
		float hgp_cos_lat = FMath::Cos(hgp_lat_rad);
		float hgp_sin_lat = FMath::Sin(hgp_lat_rad);

		lat_rad = FMath::Asin(cos_c * hgp_sin_lat + (x_rad * sin_c * hgp_cos_lat) / c);
		lon_rad = (hgp_lon_rad +
			FMath::Atan2(y_rad * sin_c, c * hgp_cos_lat * cos_c - x_rad * hgp_sin_lat * sin_c));

		return FVector(FMath::RadiansToDegrees(lat_rad), FMath::RadiansToDegrees(lon_rad), inNed.Z);
	}

	return FVector(inOriginGeoPoint.X, inOriginGeoPoint.Y, inNed.Z);
}

FVector UAirsimConversionFunctions::GeodeticToNed(FVector inGeodetic)
{
	auto geoCon = GetGeoConverter();

	double outX, outY, outZ;

	geoCon->geodetic2Ned(inGeodetic.X, inGeodetic.Y, inGeodetic.Z, &outX, &outY, &outZ);

	// Loss of precision here, but need floats for UE4
	return FVector(outX, outY, outZ);
}

FVector UAirsimConversionFunctions::GeodeticToUU(FVector inGeodetic)
{
	return NedToUU(GeodeticToNed(inGeodetic));
}

// static GeoPoint nedToGeodetic(const Vector3r& v, const HomeGeoPoint& home_geo_point)
// {
// 	double x_rad = v.x() / EARTH_RADIUS;
// 	double y_rad = v.y() / EARTH_RADIUS;
// 	double c = sqrt(x_rad*x_rad + y_rad * y_rad);
// 	double sin_c = sin(c), cos_c = cos(c);
// 	double lat_rad, lon_rad;
// 	if (!Utils::isApproximatelyZero(c)) { //avoids large changes?
// 		lat_rad = asin(cos_c * home_geo_point.sin_lat + (x_rad * sin_c * home_geo_point.cos_lat) / c);
// 		lon_rad = (home_geo_point.lon_rad +
// 			atan2(y_rad * sin_c, c * home_geo_point.cos_lat * cos_c - x_rad * home_geo_point.sin_lat * sin_c));
// 
// 		return GeoPoint(Utils::radiansToDegrees(lat_rad), Utils::radiansToDegrees(lon_rad),
// 			home_geo_point.home_geo_point.altitude - v.z());
// 	}
// 	else
// 		return GeoPoint(home_geo_point.home_geo_point.latitude, home_geo_point.home_geo_point.longitude, home_geo_point.home_geo_point.altitude - v.z());
// }
// 

FVector UAirsimConversionFunctions::UUToGeodetic(FVector inUU)
{
	return UAirsimConversionFunctions::NedToGeodetic(UAirsimConversionFunctions::UUToNed(inUU));
}

TSharedPtr<msr::airlib::GeodeticConverter> UAirsimConversionFunctions::GetGeoConverter()
{
	if (!GeoConverter.IsValid())
	{
		FVector inOriginGeoPoint = GetOriginGeoPoint();

		GeoConverter = MakeShared<msr::airlib::GeodeticConverter>(inOriginGeoPoint.X, inOriginGeoPoint.Y, inOriginGeoPoint.Z);
	}

	return GeoConverter;
}

