#pragma once

#include "USceneComponent.h"
#include "Core.h"
#include "Helper.h"

class UCamera : public USceneComponent
{
public:
	float nearZ = 0.1f;
	float farZ = 1000.0f;
	float fovY = 60.0f;

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
		TranslationInv.M[3][0] = -RelativeLocation.x;
		TranslationInv.M[3][1] = -RelativeLocation.y;
		TranslationInv.M[3][2] = -RelativeLocation.z;

		FMatrix ViewMatrix = TranslationInv * GetRotationMatrix().GetTranspose() * GetUEtoDXAxisSwap();
		return ViewMatrix;
	}

	// aspect = width / height
	FMatrix GetProjectionMatrix(float aspect)
	{
		float radian = DegreeToRadian(fovY);
		float scaleY = 1.0f / tan(radian / 2);
		float scaleX = scaleY / aspect;
		float r = farZ / (farZ - nearZ);

		return FMatrix(
			FVector4(scaleX, 0.0f, 0.0f, 0.0f),
			FVector4(0.0f, scaleY, 0.0f, 0.0f),
			FVector4(0.0f, 0.0f, r, 1.0f),
			FVector4(0.0f, 0.0f, -nearZ * r, 0.0f));
	}

	/*FMatrix GetInverse()
	{

	}*/
};
