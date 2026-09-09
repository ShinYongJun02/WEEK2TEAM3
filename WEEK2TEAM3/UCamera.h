#pragma once

#include "USceneComponent.h"
#include "Core.h"
#include "Helper.h"

class UCamera : public USceneComponent
{
	GENERATED_BODY(UCamera, USceneComponent)

public:
	float NearZ = 0.1f;
	float FarZ = 1000.0f;
	float Aspect = 1.0f;

	// 월드의 UE기준 좌표를 렌더링 전 DX 기준으로 변환
	FMatrix GetUEtoDXAxisSwap()
	{
		return FMatrix(
			FVector4(0.0f, 0.0f, 1.0f, 0.0f),   // UE.X(전방) -> DX.Z
			FVector4(1.0f, 0.0f, 0.0f, 0.0f),   // UE.Y(우)   -> DX.X
			FVector4(0.0f, 1.0f, 0.0f, 0.0f),   // UE.Z(상)   -> DX.Y
			FVector4(0.0f, 0.0f, 0.0f, 1.0f));
	}

	FMatrix GetViewMatrix()
	{
		// 뷰 행렬
		// = 카메라를 원점으로 이동시키는 행렬(=카메라의 이동행렬의 역행렬) * 카메라를 월드좌표계에 일치시키는 회전행렬(=카메라 회전행렬의 역행렬)
		// = 카메라 좌표계 행렬의 역행렬 (Rotation * Translation) ^ -1 = Translation ^ -1 * Rotation ^ -1
		FMatrix TranslationInv = FMatrix::GetIdentity();
		TranslationInv.M[3][0] = -RelativeLocation.X;
		TranslationInv.M[3][1] = -RelativeLocation.Y;
		TranslationInv.M[3][2] = -RelativeLocation.Z;

		FMatrix ViewMatrix = TranslationInv * GetRotationMatrix().GetTranspose() * GetUEtoDXAxisSwap();
		return ViewMatrix;
	}

	// aspect = width / height
	virtual FMatrix GetProjectionMatrix() = 0;

	/*FMatrix GetInverse()
	{

	}*/
};

class UPerspectiveCamera : public UCamera
{
	GENERATED_BODY(UPerspectiveCamera, UCamera)

public:
	FMatrix GetProjectionMatrix() override
	{
		float Radian = DegreeToRadian(FovY);
		float ScaleY = 1.0f / tan(Radian / 2);
		float ScaleX = ScaleY / Aspect;
		float R = FarZ / (FarZ - NearZ);

		return FMatrix(
			FVector4(ScaleX, 0.0f, 0.0f, 0.0f),
			FVector4(0.0f, ScaleY, 0.0f, 0.0f),
			FVector4(0.0f, 0.0f, R, 1.0f),
			FVector4(0.0f, 0.0f, -NearZ * R, 0.0f));
	}

	float FovY = 60.0f;
};

class UOrthoCamera : public UCamera
{
	GENERATED_BODY(UOrthoCamera, UCamera)

public:
	FMatrix GetProjectionMatrix() override
	{
		float Left = -HalfHeight * Aspect;
		float Right = HalfHeight * Aspect;
		float Top = HalfHeight;
		float Bottom = -HalfHeight;

		return FMatrix{
			FVector4(2.0f / (Right - Left), 0.0f, 0.0f, 0.0f),
			FVector4(0.0f, 2.0f / (Top - Bottom), 0.0f, 0.0f),
			FVector4(0.0f, 0.0f, 1.0f / (FarZ - NearZ), 0.0f),
			FVector4(-(Right + Left) / (Right - Left), -(Top + Bottom) / (Top - Bottom), -NearZ / (FarZ - NearZ), 1.0f)
		};
	}

	float HalfHeight = 1.0f;
};

