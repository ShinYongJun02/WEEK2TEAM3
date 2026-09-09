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

private:
	FRenderer& Renderer;
	FInputContext& InputContext;

	bool bWorldMode = true;
	EGizmoOperation CurrentOperation = EGizmoOperation::Translate;

	FVector2 PrevMousePos;
	bool bIsSelected = false;
	FVector AxisDirection; // World +
	FVector2 HandleScreenDirection;
	AxisNumber SelectedAxis;
};
