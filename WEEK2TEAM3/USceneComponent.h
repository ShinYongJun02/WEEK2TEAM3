#pragma once

#include "UObject.h"

class USceneComponent : public UObject
{
	GENERATED_BODY(USceneComponent, UObject)

public:
	FVector RelativeLocation;
	FVector RelativeRotation;
	FVector RelativeScale3D;

	USceneComponent();

	FMatrix GetTranslationMatrix() const;
	FMatrix GetRollMatrix() const;
	FMatrix GetPitchMatrix() const;
	FMatrix GetYawMatrix() const;
	FMatrix GetRotationMatrix() const;
	FMatrix GetScaleMatrix() const;
	FMatrix GetModelMatrix() const;

	FVector GetForward() const;
	FVector GetRight() const;
	FVector GetUp() const;
};
