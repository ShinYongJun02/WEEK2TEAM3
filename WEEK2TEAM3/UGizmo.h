#pragma once

#include "Core.h"
#include "UInputContext.h"
#include "USceneComponent.h"
#include "URenderer.h"
#include "Helper.h"

enum class EGizmoOperation
{
	Translate,
	Rotate,
	Scale
};

class UGizmo
{
public:
	UGizmo(UInputContext& inputCtx) 
		: InputContext(inputCtx) 
	{
	}

	inline void SetWorldMode(bool worldMode)
	{
		WorldMode = worldMode;
	}

	inline void SetOperation(EGizmoOperation operation)
	{
		CurrentOperation = operation;
	}

	void Draw(USceneComponent& sceneComp, const FMatrix& viewProjection, const URenderer& renderer)
	{
		const float axisLength = 1.f;

		FVector2 mousePos = FVector2(InputContext.GetMouseX(), InputContext.GetMouseY());
		FVector2 center = WorldToScreen(sceneComp.RelativeLocation, viewProjection, renderer.GetWidth(), renderer.GetHeight());

		auto DrawLineAxis = [&](const FVector& axis, const FVector4& color)
		{
			FVector2 a = WorldToScreen(sceneComp.RelativeLocation + axis * axisLength, viewProjection, renderer.GetWidth(), renderer.GetHeight());
			FVector2 b = WorldToScreen(sceneComp.RelativeLocation - axis * axisLength, viewProjection, renderer.GetWidth(), renderer.GetHeight());

			float plusLength = DistanceSquared(a, center);
			float minusLength = DistanceSquared(b, center);

			FVector2 closestPoint = minusLength < plusLength ? a : b;
			FVector4 axisColor = color;
			if (PointToLineSegmentDistanceSquared(mousePos, center, closestPoint) < 5.f * 5.f)
			{
				if (InputContext.IsMouseButtonDown(0))
				{
					PrevMousePos = mousePos;
					CurrentAxis = axis;
					CurrentScreenAxis = a - center;
					CurrentScreenAxis.Normalize();
					IsDragging = true;
				}
				axisColor = FVector4(1.f, 1.f, 0.f, 1.f);
			}

			renderer.RenderLine2D(center, closestPoint, axisColor, 5.f);
		};

		auto DrawCircleAxis = [&](const FVector& u, const FVector& v, const FVector4& color)
		{
			const int32 numSegments = 32;

			FVector2 circlePoints[numSegments];
			FVector4 axisColor = color;
			FVector2 firstPoint;
			FVector2 prevPoint;

			GenerateCircleVertices([&](int32 index, const FVector2& point) {
				FVector offset = u * point.x + v * point.y;
				FVector4 worldPoint = FVector4(offset.x, offset.y, offset.z, 1.f) * sceneComp.GetRotationMatrix() + FVector4(sceneComp.RelativeLocation, 1.f);
				FVector2 screenPoint = WorldToScreen(FVector(worldPoint.x, worldPoint.y, worldPoint.z), viewProjection, renderer.GetWidth(), renderer.GetHeight());

				if (index > 0)
				{
					if (PointToLineSegmentDistanceSquared(mousePos, prevPoint, screenPoint) < 5.f * 5.f)
					{
						if (InputContext.IsMouseButtonDown(0))
						{
							PrevMousePos = mousePos;
							CurrentAxis = u.Cross(v);
							CurrentScreenAxis = screenPoint - prevPoint;
							CurrentScreenAxis.Normalize();
							IsDragging = true;
						}
						axisColor = FVector4(1.f, 1.f, 0.f, 1.f);
					}
				}
				else
				{
					firstPoint = screenPoint;
				}

				if (index == numSegments - 1)
				{
					if (PointToLineSegmentDistanceSquared(mousePos, screenPoint, firstPoint) < 5.f * 5.f)
					{
						if (InputContext.IsMouseButtonDown(0))
						{
							PrevMousePos = mousePos;
							CurrentAxis = u.Cross(v);
							CurrentScreenAxis = firstPoint - screenPoint;
							CurrentScreenAxis.Normalize();
							IsDragging = true;
						}
						axisColor = FVector4(1.f, 1.f, 0.f, 1.f);
					}
				}

				circlePoints[index] = screenPoint;
				prevPoint = screenPoint;
			}, FVector2(0.f), axisLength, numSegments);

			for (int32 i = 0; i < numSegments; ++i)
			{
				int32 nextIndex = (i + 1) % numSegments;
				renderer.RenderLine2D(circlePoints[i], circlePoints[nextIndex], axisColor, 2.f);
			}
		};

		if (CurrentOperation == EGizmoOperation::Translate)
		{
			DrawLineAxis(WorldMode ? Right : sceneComp.GetRight(), FVector4(0.f, 1.f, 0.f, 1.f));
			DrawLineAxis(WorldMode ? Front : sceneComp.GetForward(), FVector4(1.f, 0.f, 0.f, 1.f));
			DrawLineAxis(WorldMode ? Up : sceneComp.GetUp(), FVector4(0.f, 0.f, 1.f, 1.f));
		}
		else if (CurrentOperation == EGizmoOperation::Rotate)
		{
			DrawCircleAxis(WorldMode ? Right : sceneComp.GetRight(), WorldMode ? Up : sceneComp.GetUp(), FVector4(1.f, 0.f, 0.f, 1.f));
			DrawCircleAxis(WorldMode ? Front : sceneComp.GetForward(), WorldMode ? Up : sceneComp.GetUp(), FVector4(0.f, 1.f, 0.f, 1.f));
			DrawCircleAxis(WorldMode ? Right : sceneComp.GetRight(), WorldMode ? Front : sceneComp.GetForward(), FVector4(0.f, 0.f, 1.f, 1.f));
		}
		else if (CurrentOperation == EGizmoOperation::Scale)
		{
			DrawLineAxis(WorldMode ? Right : sceneComp.GetRight(), FVector4(0.f, 1.f, 0.f, 1.f));
			DrawLineAxis(WorldMode ? Front : sceneComp.GetForward(), FVector4(1.f, 0.f, 0.f, 1.f));
			DrawLineAxis(WorldMode ? Up : sceneComp.GetUp(), FVector4(0.f, 0.f, 1.f, 1.f));
		}

		if (IsDragging)
		{
			if (InputContext.IsMouseButtonPressed(0))
			{
				const float sensitivity = 0.01f;

				FVector2 mouseDelta = mousePos - PrevMousePos;
				float amount = Dot(mouseDelta, CurrentScreenAxis);

				if (CurrentOperation == EGizmoOperation::Translate)
				{
					sceneComp.RelativeLocation += CurrentAxis * amount * sensitivity;
				}
				else if (CurrentOperation == EGizmoOperation::Rotate)
				{
					sceneComp.RelativeRotation += CurrentAxis * amount * sensitivity * 10.f;
				}
				else if (CurrentOperation == EGizmoOperation::Scale)
				{
					sceneComp.RelativeScale3D += CurrentAxis * -amount * sensitivity;
				}

				PrevMousePos = mousePos;
			}

			if (InputContext.IsMouseButtonUp(0))
			{
				CurrentAxis = FVector(0.f, 0.f, 0.f);
				IsDragging = false;
			}
		}
	}

private:
	UInputContext& InputContext;

	bool WorldMode = true;
	EGizmoOperation CurrentOperation = EGizmoOperation::Translate;

	FVector2 PrevMousePos;
	bool IsDragging = false;
	FVector CurrentAxis = FVector(0.f, 0.f, 0.f);
	FVector2 CurrentScreenAxis = FVector2(0.f, 0.f);
};