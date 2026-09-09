#pragma once

#include "USceneComponent.h"

class UCamera : public USceneComponent
{
	GENERATED_BODY(UCamera, USceneComponent)

public:
	float NearZ = 0.1f;
	float FarZ = 1000.0f;
	float Aspect = 1.0f;

	// 월드의 UE기준 좌표를 렌더링 전 DX 기준으로 변환
	FMatrix GetUEtoDXAxisSwap();
	FMatrix GetViewMatrix();

	// aspect = width / height
	virtual FMatrix GetProjectionMatrix() = 0;
};

class UPerspectiveCamera : public UCamera
{
	GENERATED_BODY(UPerspectiveCamera, UCamera)

public:
	FMatrix GetProjectionMatrix() override;

	float FovY = 60.0f;
};

class UOrthoCamera : public UCamera
{
	GENERATED_BODY(UOrthoCamera, UCamera)

public:
	FMatrix GetProjectionMatrix() override;

	float HalfHeight = 1.0f;
};
