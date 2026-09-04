#pragma once

#include "Core.h"

class UObject
{
public:
	FVector Translation;
	FVector Rotation;
	FVector Scale;

	UObject() : Scale(FVector(1.0f, 1.0f, 1.0f)) {}

	static float DegreeToRadian(float degree)
	{
		float radian = degree * (float)acos(-1) / 180.0f;
		return radian;
	}

	FMatrix GetTranslationMatrix() const
	{
		FMatrix T(
			FVector4(1.0f, 0.0f, 0.0f, 0.0f),
			FVector4(0.0f, 1.0f, 0.0f, 0.0f),
			FVector4(0.0f, 0.0f, 1.0f, 0.0f),
			FVector4(Translation.x, Translation.y, Translation.z, 1.0f));

		return T;
	}

	FMatrix GetRollMatrix() const
	{
		// 언리얼 회전 보정
		float radian = -DegreeToRadian(Rotation.x);
		return FMatrix(
			FVector4(1.0f, 0.0f, 0.0f, 0.0f),
			FVector4(0.0f, cos(radian), sin(radian), 0.0f),
			FVector4(0.0f, -sin(radian), cos(radian), 0.0f),
			FVector4(0.0f, 0.0f, 0.0f, 1.0f));
	}

	FMatrix GetPitchMatrix() const
	{
		// 언리얼 회전 보정
		float radian = -DegreeToRadian(Rotation.y);
		return FMatrix(
			FVector4(cos(radian), 0.0f, -sin(radian), 0.0f),
			FVector4(0.0f, 1.0f, 0.0f, 0.0f),
			FVector4(sin(radian), 0.0f, cos(radian), 0.0f),
			FVector4(0.0f, 0.0f, 0.0f, 1.0f));
	}

	FMatrix GetYawMatrix() const
	{
		float radian = DegreeToRadian(Rotation.z);
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
			FVector4(Scale.x, 0.0f, 0.0f, 0.0f),
			FVector4(0.0f, Scale.y, 0.0f, 0.0f),
			FVector4(0.0f, 0.0f, Scale.z, 0.0f),
			FVector4(0.0f, 0.0f, 0.0f, 1.0f));

		return S;
	}

	FMatrix GetModelMatrix() const
	{
		return GetScaleMatrix() * GetRotationMatrix() * GetTranslationMatrix();
	}
};
