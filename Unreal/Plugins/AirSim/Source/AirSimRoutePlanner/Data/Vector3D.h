#pragma once
#include "Math/Vector.h"
#include "Containers/UnrealString.h"

// A simple way to store vectors in double. Doesn't attempt to implement any functionality.
struct FVector3D
{
	double X = 0;
	double Y = 0;
	double Z = 0;

	FVector3D();

	FVector3D(double InX, double InY, double InZ) : X(InX), Y(InY), Z(InZ) {}
	FVector3D(FVector source) : X(source.X), Y(source.Y), Z(source.Z) {}

	FString ToString() const
	{
		return FString::Printf(TEXT("X=%3.3f Y=%3.3f Z=%3.3f"), X, Y, Z);
	}

};
