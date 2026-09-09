#pragma once

#include "Core.h"

class FInputContext;
class FRenderer;
class USceneComponent;

enum class EGizmoOperation
{
	Translate,
	Rotate,
	Scale
};

enum class AxisNumber
{
	None,
	X,
	Y,
	Z,
	Cameara
};

class FGizmo
{
public:
	FGizmo(FRenderer& InRenderer, FInputContext& InInputContext);

	void SetWorldMode(bool bInWorldMode);
	void SetOperation(EGizmoOperation Operation);

	void Draw(USceneComponent& SceneComp, const FVector& CameraPosition, const FMatrix& ViewProjection);

	// 마우스가 기즈모 핸들 위에 있는지. 오브젝트 실루엣 밖으로 나온 핸들을 눌러도
	// 선택이 풀리지 않도록, 피킹보다 먼저 이걸 물어보고 기즈모에 입력 우선권을 준다.
	bool IsMouseOverHandle() const;

private:
	static constexpr float HandleHitRadius = 5.f;

	FRenderer& Renderer;
	FInputContext& InputContext;

	bool bWorldMode = true;
	EGizmoOperation CurrentOperation = EGizmoOperation::Translate;

	FVector2 PrevMousePos;
	bool bIsSelected = false;
	bool bIsHoveredAxis = false;
	FVector AxisDirection; // World +
	FVector2 HandleScreenDirection;
	AxisNumber SelectedAxis;

	// 이번 프레임에 그린 핸들의 화면 공간 선분. IsMouseOverHandle 이 참조한다.
	TArray<TPair<FVector2, FVector2>> HandleScreenSegments;
};
