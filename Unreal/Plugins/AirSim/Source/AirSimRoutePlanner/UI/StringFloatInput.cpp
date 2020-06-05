// Fill out your copyright notice in the Description page of Project Settings.


#include "StringFloatInput.h"

float UStringFloatInput::GetFloatValue()
{
	return FloatValueBinding.Get();
}

void UStringFloatInput::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	FloatValueBinding = PROPERTY_BINDING(float, FloatValue);
	FloatValue = FloatValueBinding.Get();
}
