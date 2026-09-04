#pragma once

#include "UObject.h"
#include "Helper.h"

class USceneComponent : public UObject
{
public:
	FVector RelativeLocation;
	FVector RelativeRotation;
	FVector RelativeScale3D;

	USceneComponent() : RelativeScale3D(FVector(1.0f, 1.0f, 1.0f)) {}

	FMatrix GetTranslationMatrix() const
	{
		FMatrix T(
			FVector4(1.0f, 0.0f, 0.0f, 0.0f),
			FVector4(0.0f, 1.0f, 0.0f, 0.0f),
			FVector4(0.0f, 0.0f, 1.0f, 0.0f),
			FVector4(RelativeLocation.x, RelativeLocation.y, RelativeLocation.z, 1.0f));

		return T;
	}

	FMatrix GetRollMatrix() const
	{
		// 언리얼 회전 보정 (-)
		float radian = -DegreeToRadian(RelativeRotation.x);
		return FMatrix(
			FVector4(1.0f, 0.0f, 0.0f, 0.0f),
			FVector4(0.0f, cos(radian), sin(radian), 0.0f),
			FVector4(0.0f, -sin(radian), cos(radian), 0.0f),
			FVector4(0.0f, 0.0f, 0.0f, 1.0f));
	}

	FMatrix GetPitchMatrix() const
	{
		// 언리얼 회전 보정 (-)
		float radian = -DegreeToRadian(RelativeRotation.y);
		return FMatrix(
			FVector4(cos(radian), 0.0f, -sin(radian), 0.0f),
			FVector4(0.0f, 1.0f, 0.0f, 0.0f),
			FVector4(sin(radian), 0.0f, cos(radian), 0.0f),
			FVector4(0.0f, 0.0f, 0.0f, 1.0f));
	}

	FMatrix GetYawMatrix() const
	{
		float radian = DegreeToRadian(RelativeRotation.z);
		return FMatrix(
			FVector4(cos(radian), sin(radian), 0.0f, 0.0f),
			FVector4(-sin(radian), cos(radian), 0.0f, 0.0f),
			FVector4(0.0f, 0.0f, 1.0f, 0.0f),
			FVector4(0.0f, 0.0f, 0.0f, 1.0f));
	}

	FMatrix GetRotationMatrix() const
	{
		return GetRollMatrix() * GetPitchMatrix() * GetYawMatrix();
	}

	FMatrix GetScaleMatrix() const
	{
		FMatrix S(
			FVector4(RelativeScale3D.x, 0.0f, 0.0f, 0.0f),
			FVector4(0.0f, RelativeScale3D.y, 0.0f, 0.0f),
			FVector4(0.0f, 0.0f, RelativeScale3D.z, 0.0f),
			FVector4(0.0f, 0.0f, 0.0f, 1.0f));

		return S;
	}

	FMatrix GetModelMatrix() const
	{
		return GetScaleMatrix() * GetRotationMatrix() * GetTranslationMatrix();
	}
};
