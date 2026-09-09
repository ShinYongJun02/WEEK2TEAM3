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

enum class AxisNumber
{
	None,
	X,
	Y,
	Z,
	Cameara
};

class UGizmo
{
public:
	UGizmo(URenderer& renderer, UInputContext& inputCtx) 
		: Renderer(renderer)
		, InputContext(inputCtx)
		, SelectedAxis(AxisNumber::None)
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

	void Draw(USceneComponent& sceneComp, const FVector& cameraPosition, const FMatrix& viewProjection)
	{
		FVector centerToCamera = cameraPosition - sceneComp.RelativeLocation;
		float Distance = centerToCamera.Length();
		const float axisLength = 0.1f * Distance;
		const float screenWidth = static_cast<float>(Renderer.GetWidth());
		const float screenHeight = static_cast<float>(Renderer.GetHeight());

		FVector2 mousePos = FVector2(InputContext.GetMouseX(), InputContext.GetMouseY());

		// ImGui 창이 마우스를 가져간 프레임에는 호버/잡기를 하지 않는다 (드래그 중에는 계속 유지)
		const bool bHoverEnabled = !ImGui::GetIO().WantCaptureMouse;
		const bool bGrabAxis = bHoverEnabled && InputContext.IsMouseButtonDown(0);

		// 화면 밖에 나감
		FVector4 clip = FVector4(sceneComp.RelativeLocation, 1.f) * viewProjection;
		bool drawGizmo = !(clip.w <= 0.00001f || clip.z < 0.f || clip.z > clip.w || clip.x < -clip.w || clip.x > clip.w || clip.y < -clip.w || clip.y > clip.w);

		FVector2 center = WorldToScreen(sceneComp.RelativeLocation, viewProjection, screenWidth, screenHeight);

		enum class AxisEndPointStyle
		{
			None,
			Arrow,
			Circle
		};

		auto DrawLineAxis = [&](const FVector& drawAxis, const FVector& applyAxis, const FVector4& color, AxisEndPointStyle style, AxisNumber Axis)
		{
			FVector2 closestPoint = WorldToScreen(sceneComp.RelativeLocation + drawAxis * axisLength, viewProjection, screenWidth, screenHeight);

			FVector4 selectColor(1.0f, 1.0f, 0.0f, 1.0f);

			FVector4 axisColor = (Axis == SelectedAxis) ? selectColor : color;

			if (!IsSelected && bHoverEnabled && PointToLineSegmentDistanceSquared(mousePos, center, closestPoint) < 5.f * 5.f)
			{
				if (bGrabAxis)
				{
					PrevMousePos = mousePos;
					AxisDirection = applyAxis;
					HandleScreenDirection = closestPoint - center;
					HandleScreenDirection.Normalize();
					IsSelected = true;
					SelectedAxis = Axis;
				}
				axisColor = FVector4(1.f, 1.f, 0.f, 1.f);
			}

			Renderer.RenderLine2D(center, closestPoint, axisColor, 5.f);

			if (style == AxisEndPointStyle::Arrow)
			{
				FVector2 screenAxis = closestPoint - center;
				screenAxis.Normalize();
				float angle = atan2(screenAxis.y, screenAxis.x);
				Renderer.RenderTriangle2D(closestPoint, axisColor, 20.f, angle);
			}
			else if (style == AxisEndPointStyle::Circle)
			{
				Renderer.RenderCircle2D(closestPoint, axisColor, 8.f);
			}

			Renderer.RenderCircle2D(center, FVector4(0.8f, 0.8f, 0.8f, 1.f), 5.f);
		};

		auto DrawCircleAxis = [&](const FVector& u, const FVector& v, const FVector4& color, bool noclipping = false, AxisNumber Axis = AxisNumber::None)
		{
			const int32 numSegments = 32;

			FVector2 circleScreenPoints[numSegments];
			FVector circleWorldPoints[numSegments];

			FVector4 selectColor(1.0f, 1.0f, 0.0f, 1.0f);

			FVector4 axisColor = color;

			if (Axis != AxisNumber::None)
			{
				axisColor = (Axis == SelectedAxis) ? selectColor : color;
			}
			GenerateCircleVertices([&](int32 index, const FVector2& point) {
				FVector offset = u * point.x + v * point.y;
				FVector4 worldPoint = FVector4(offset.x, offset.y, offset.z, 1.f) + FVector4(sceneComp.RelativeLocation, 1.f);
				FVector2 screenPoint = WorldToScreen(FVector(worldPoint.x, worldPoint.y, worldPoint.z), viewProjection, screenWidth, screenHeight);

				circleScreenPoints[index] = screenPoint;
				circleWorldPoints[index] = FVector(worldPoint.x, worldPoint.y, worldPoint.z);

				if (index > 0)
				{
					bool checkInteraction = true;
					if (!noclipping)
					{
						FVector worldLineCenterPoint = Lerp(circleWorldPoints[index - 1], circleWorldPoints[index], 0.5f);
						FVector centerToLineCenterPoint = worldLineCenterPoint - sceneComp.RelativeLocation;
						checkInteraction = Dot(centerToLineCenterPoint, centerToCamera) >= 0.f;
					}

					if (checkInteraction && bHoverEnabled && PointToLineSegmentDistanceSquared(mousePos, circleScreenPoints[index - 1], screenPoint) < 5.f * 5.f)
					{
						if (bGrabAxis)
						{
							PrevMousePos = mousePos;
							AxisDirection = u.Cross(v);
							HandleScreenDirection = screenPoint - circleScreenPoints[index - 1];
							HandleScreenDirection.Normalize();
							IsSelected = true;
							SelectedAxis = Axis;
						}
						if (!IsSelected)
							axisColor = FVector4(1.f, 1.f, 0.f, 1.f);
					}
				}

				if (index == numSegments - 1)
				{
					bool checkInteraction = true;
					if (!noclipping)
					{
						FVector worldLineCenterPoint = Lerp(circleWorldPoints[index], circleWorldPoints[0], 0.5f);
						FVector centerToLineCenterPoint = worldLineCenterPoint - sceneComp.RelativeLocation;
						checkInteraction = Dot(centerToLineCenterPoint, centerToCamera) >= 0.f;
					}

					if (checkInteraction && bHoverEnabled && PointToLineSegmentDistanceSquared(mousePos, screenPoint, circleScreenPoints[0]) < 5.f * 5.f)
					{
						if (bGrabAxis)
						{
							PrevMousePos = mousePos;
							AxisDirection = u.Cross(v);
							HandleScreenDirection = circleScreenPoints[0] - screenPoint;
							HandleScreenDirection.Normalize();
							IsSelected = true;
							SelectedAxis = Axis;
						}
						if (!IsSelected)
							axisColor = FVector4(1.f, 1.f, 0.f, 1.f);
					}
				}

			}, axisLength, numSegments);

			for (int32 i = 0; i < numSegments; ++i)
			{
				int32 nextIndex = (i + 1) % numSegments;

				if (!noclipping)
				{
					FVector worldLineCenterPoint = Lerp(circleWorldPoints[i], circleWorldPoints[nextIndex], 0.5f);
					FVector centerToLineCenterPoint = worldLineCenterPoint - sceneComp.RelativeLocation;
					if (Dot(centerToLineCenterPoint, centerToCamera) < 0.f)
					{
						continue;
					}
				}

				Renderer.RenderLine2D(circleScreenPoints[i], circleScreenPoints[nextIndex], axisColor, 2.f);
			}

			Renderer.RenderCircle2D(center, FVector4(0.8f, 0.8f, 0.8f, 1.f), 5.f);
		};

		if (drawGizmo &&CurrentOperation == EGizmoOperation::Translate)
		{
			FVector front = WorldMode ? Front : sceneComp.GetForward();
			FVector right = WorldMode ? Right : sceneComp.GetRight();
			FVector up = WorldMode ? Up : sceneComp.GetUp();

			DrawLineAxis(front, front, FVector4(1.f, 0.f, 0.f, 1.f), AxisEndPointStyle::Arrow, AxisNumber::X);
			DrawLineAxis(right, right, FVector4(0.f, 1.f, 0.f, 1.f), AxisEndPointStyle::Arrow, AxisNumber::Y);
			DrawLineAxis(up, up, FVector4(0.f, 0.f, 1.f, 1.f), AxisEndPointStyle::Arrow, AxisNumber::Z);
		}
		else if (drawGizmo && CurrentOperation == EGizmoOperation::Rotate)
		{
			FVector front = WorldMode ? Front : sceneComp.GetForward();
			FVector right = WorldMode ? Right : sceneComp.GetRight();
			FVector up = WorldMode ? Up : sceneComp.GetUp();

			DrawCircleAxis(right, up, FVector4(1.f, 0.f, 0.f, 1.f),false, AxisNumber::X);
			DrawCircleAxis(front, up, FVector4(0.f, 1.f, 0.f, 1.f), false, AxisNumber::Y);
			DrawCircleAxis(right, front, FVector4(0.f, 0.f, 1.f, 1.f),false , AxisNumber::Z);

			FVector u = Cross(centerToCamera, Up);
			u.Normalize();

			FVector v = Cross(u, centerToCamera);
			v.Normalize();

			DrawCircleAxis(u, v, FVector4(1.f, 1.f, 1.f, 1.f), true, AxisNumber::Cameara);
		}
		else if (drawGizmo && CurrentOperation == EGizmoOperation::Scale)
		{
			FVector front = sceneComp.GetForward();
			FVector right = sceneComp.GetRight();
			FVector up = sceneComp.GetUp();

			DrawLineAxis(front, Front, FVector4(0.f, 1.f, 0.f, 1.f), AxisEndPointStyle::Circle, AxisNumber::X);
			DrawLineAxis(right, Right, FVector4(1.f, 0.f, 0.f, 1.f), AxisEndPointStyle::Circle , AxisNumber::Y);
			DrawLineAxis(up, Up, FVector4(0.f, 0.f, 1.f, 1.f), AxisEndPointStyle::Circle, AxisNumber::Z);
		}

		if (IsSelected)
		{
			if (InputContext.IsMouseButtonPressed(0))
			{
				const float sensitivity = 0.01f;

				FVector2 mouseDelta = mousePos - PrevMousePos;
				float amount = Dot(mouseDelta, HandleScreenDirection);

				if (CurrentOperation == EGizmoOperation::Translate)
				{
					sceneComp.RelativeLocation += AxisDirection * amount * sensitivity;
				}
				else if (CurrentOperation == EGizmoOperation::Rotate)
				{
					FMatrix rotationMatrix = sceneComp.GetRotationMatrix();

					FQuaternion rotationQ = ToQuaternion(rotationMatrix);
					FQuaternion deltaQ(AxisDirection, amount * sensitivity);
					FQuaternion finalQ = deltaQ * rotationQ;
					finalQ.Normalize();

					sceneComp.RelativeRotation = ToEulerAngles(finalQ) * Rad2Deg;
				}
				else if (CurrentOperation == EGizmoOperation::Scale)
				{
					sceneComp.RelativeScale3D += AxisDirection * amount * sensitivity;
				}

				PrevMousePos = mousePos;
			}

			if (InputContext.IsMouseButtonUp(0))
			{
				IsSelected = false;
				SelectedAxis = AxisNumber::None;
			}
		}
	}

private:
	URenderer& Renderer;
	UInputContext& InputContext;

	bool WorldMode = true;
	EGizmoOperation CurrentOperation = EGizmoOperation::Translate;

	FVector2 PrevMousePos;
	bool IsSelected = false;	
	FVector AxisDirection; // World +
	FVector2 HandleScreenDirection;
	AxisNumber SelectedAxis;
};
