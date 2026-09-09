#pragma once

#include "UObject.h"
#include "Helper.h"

class USceneComponent : public UObject
{
	GENERATED_BODY(USceneComponent, UObject)

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
			FVector4(RelativeLocation.X, RelativeLocation.Y, RelativeLocation.Z, 1.0f));

		return T;
	}

	FMatrix GetRollMatrix() const
	{
		// 언리얼 회전 보정 (-)
		float Radian = -DegreeToRadian(RelativeRotation.X);
		return FMatrix(
			FVector4(1.0f, 0.0f, 0.0f, 0.0f),
			FVector4(0.0f, cos(Radian), sin(Radian), 0.0f),
			FVector4(0.0f, -sin(Radian), cos(Radian), 0.0f),
			FVector4(0.0f, 0.0f, 0.0f, 1.0f));
	}

	FMatrix GetPitchMatrix() const
	{
		// 언리얼 회전 보정 (-)
		float Radian = -DegreeToRadian(RelativeRotation.Y);
		return FMatrix(
			FVector4(cos(Radian), 0.0f, -sin(Radian), 0.0f),
			FVector4(0.0f, 1.0f, 0.0f, 0.0f),
			FVector4(sin(Radian), 0.0f, cos(Radian), 0.0f),
			FVector4(0.0f, 0.0f, 0.0f, 1.0f));
	}

	FMatrix GetYawMatrix() const
	{
		float Radian = DegreeToRadian(RelativeRotation.Z);
		return FMatrix(
			FVector4(cos(Radian), sin(Radian), 0.0f, 0.0f),
			FVector4(-sin(Radian), cos(Radian), 0.0f, 0.0f),
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
			FVector4(RelativeScale3D.X, 0.0f, 0.0f, 0.0f),
			FVector4(0.0f, RelativeScale3D.Y, 0.0f, 0.0f),
			FVector4(0.0f, 0.0f, RelativeScale3D.Z, 0.0f),
			FVector4(0.0f, 0.0f, 0.0f, 1.0f));

		return S;
	}

	FMatrix GetModelMatrix() const
	{
		return GetScaleMatrix() * GetRotationMatrix() * GetTranslationMatrix();
	}

	FVector GetForward() const
	{
		FMatrix R = GetRotationMatrix();
		return FVector(R.M[0][0], R.M[0][1], R.M[0][2]);
	}

	FVector GetRight() const
	{
		FMatrix R = GetRotationMatrix();
		return FVector(R.M[1][0], R.M[1][1], R.M[1][2]);
	}

	FVector GetUp() const
	{
		FMatrix R = GetRotationMatrix();
		return FVector(R.M[2][0], R.M[2][1], R.M[2][2]);
	}
};
