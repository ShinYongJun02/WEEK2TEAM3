#include "FGizmo.h"

#include <cmath>

#include "ImGui/imgui.h"

#include "FInputContext.h"
#include "FRenderer.h"
#include "Helper.h"
#include "USceneComponent.h"

namespace
{
	enum class AxisEndPointStyle
	{
		None,
		Arrow,
		Circle
	};
}

FGizmo::FGizmo(FRenderer& InRenderer, FInputContext& InInputContext)
	: Renderer(InRenderer)
	, InputContext(InInputContext)
	, SelectedAxis(AxisNumber::None)
{
}

void FGizmo::SetWorldMode(bool bInWorldMode)
{
	bWorldMode = bInWorldMode;
}

void FGizmo::SetOperation(EGizmoOperation Operation)
{
	CurrentOperation = Operation;
}

void FGizmo::Draw(USceneComponent& SceneComp, const FVector& CameraPosition, const FMatrix& ViewProjection)
{
	FVector CenterToCamera = CameraPosition - SceneComp.RelativeLocation;
	float Distance = CenterToCamera.Length();
	const float AxisLength = 0.1f * Distance;
	const float ScreenWidth = static_cast<float>(Renderer.GetWidth());
	const float ScreenHeight = static_cast<float>(Renderer.GetHeight());

	FVector2 MousePos = FVector2(InputContext.GetMouseX(), InputContext.GetMouseY());

	// ImGui 창이 마우스를 가져간 프레임에는 호버/잡기를 하지 않는다 (드래그 중에는 계속 유지)
	const bool bHoverEnabled = !ImGui::GetIO().WantCaptureMouse;
	const bool bGrabAxis = bHoverEnabled && InputContext.IsMouseButtonDown(0);

	// 화면 밖에 나감
	FVector4 Clip = FVector4(SceneComp.RelativeLocation, 1.f) * ViewProjection;
	bool bDrawGizmo = !(Clip.W <= 0.00001f || Clip.Z < 0.f || Clip.Z > Clip.W || Clip.X < -Clip.W || Clip.X > Clip.W || Clip.Y < -Clip.W || Clip.Y > Clip.W);

	HandleScreenSegments.clear();

	FVector2 Center = WorldToScreen(SceneComp.RelativeLocation, ViewProjection, ScreenWidth, ScreenHeight);

	auto DrawLineAxis = [&](const FVector& DrawAxis, const FVector& ApplyAxis, const FVector4& Color, AxisEndPointStyle Style, AxisNumber Axis)
	{
		FVector2 ClosestPoint = WorldToScreen(SceneComp.RelativeLocation + DrawAxis * AxisLength, ViewProjection, ScreenWidth, ScreenHeight);

		FVector4 SelectColor(1.0f, 1.0f, 0.0f, 1.0f);

		FVector4 AxisColor = (Axis == SelectedAxis) ? SelectColor : Color;

		if (!bIsSelected && bHoverEnabled && PointToLineSegmentDistanceSquared(MousePos, Center, ClosestPoint) < HandleHitRadius * HandleHitRadius)
		{
			if (bGrabAxis)
			{
				PrevMousePos = MousePos;
				AxisDirection = ApplyAxis;
				HandleScreenDirection = ClosestPoint - Center;
				HandleScreenDirection.Normalize();
				bIsSelected = true;
				SelectedAxis = Axis;
			}
			if (bIsHoveredAxis == false)
			{
				bIsHoveredAxis = true;
				AxisColor = FVector4(1.f, 1.f, 0.f, 1.f);
			}
		}

		HandleScreenSegments.emplace_back(Center, ClosestPoint);

		Renderer.RenderLine2D(Center, ClosestPoint, AxisColor, 5.f);

		if (Style == AxisEndPointStyle::Arrow)
		{
			FVector2 ScreenAxis = ClosestPoint - Center;
			ScreenAxis.Normalize();
			float Angle = atan2(ScreenAxis.Y, ScreenAxis.X);
			Renderer.RenderTriangle2D(ClosestPoint, AxisColor, 20.f, Angle);
		}
		else if (Style == AxisEndPointStyle::Circle)
		{
			Renderer.RenderCircle2D(ClosestPoint, AxisColor, 8.f);
		}

		Renderer.RenderCircle2D(Center, FVector4(0.8f, 0.8f, 0.8f, 1.f), 5.f);
	};

	auto DrawCircleAxis = [&](const FVector& U, const FVector& V, const FVector4& Color, bool bNoClipping = false, AxisNumber Axis = AxisNumber::None)
	{
		const int32 NumSegments = 32;

		FVector2 CircleScreenPoints[NumSegments];
		FVector CircleWorldPoints[NumSegments];

		FVector4 SelectColor(1.0f, 1.0f, 0.0f, 1.0f);

		FVector4 AxisColor = Color;

		if (Axis != AxisNumber::None)
		{
			AxisColor = (Axis == SelectedAxis) ? SelectColor : Color;
		}
		GenerateCircleVertices([&](int32 Index, const FVector2& Point) {
			FVector Offset = U * Point.X + V * Point.Y;
			FVector4 WorldPoint = FVector4(Offset.X, Offset.Y, Offset.Z, 1.f) + FVector4(SceneComp.RelativeLocation, 1.f);
			FVector2 ScreenPoint = WorldToScreen(FVector(WorldPoint.X, WorldPoint.Y, WorldPoint.Z), ViewProjection, ScreenWidth, ScreenHeight);

			CircleScreenPoints[Index] = ScreenPoint;
			CircleWorldPoints[Index] = FVector(WorldPoint.X, WorldPoint.Y, WorldPoint.Z);

			if (Index > 0)
			{
				bool bCheckInteraction = true;
				if (!bNoClipping)
				{
					FVector WorldLineCenterPoint = Lerp(CircleWorldPoints[Index - 1], CircleWorldPoints[Index], 0.5f);
					FVector CenterToLineCenterPoint = WorldLineCenterPoint - SceneComp.RelativeLocation;
					bCheckInteraction = Dot(CenterToLineCenterPoint, CenterToCamera) >= 0.f;
				}

				if ((!bIsSelected) && bCheckInteraction && bHoverEnabled && PointToLineSegmentDistanceSquared(MousePos, CircleScreenPoints[Index - 1], ScreenPoint) < HandleHitRadius * HandleHitRadius)
				{
					if (bGrabAxis)
					{
						PrevMousePos = MousePos;
						AxisDirection = U.Cross(V);
						HandleScreenDirection = ScreenPoint - CircleScreenPoints[Index - 1];
						HandleScreenDirection.Normalize();
						bIsSelected = true;
						SelectedAxis = Axis;
					}

					if (bIsHoveredAxis == false)
					{
						bIsHoveredAxis = true;
						AxisColor = FVector4(1.f, 1.f, 0.f, 1.f);
					}
				}
			}

			if (Index == NumSegments - 1)
			{
				bool bCheckInteraction = true;
				if (!bNoClipping)
				{
					FVector WorldLineCenterPoint = Lerp(CircleWorldPoints[Index], CircleWorldPoints[0], 0.5f);
					FVector CenterToLineCenterPoint = WorldLineCenterPoint - SceneComp.RelativeLocation;
					bCheckInteraction = Dot(CenterToLineCenterPoint, CenterToCamera) >= 0.f;
				}

				if ((!bIsSelected) && bCheckInteraction && bHoverEnabled && PointToLineSegmentDistanceSquared(MousePos, ScreenPoint, CircleScreenPoints[0]) < HandleHitRadius * HandleHitRadius)
				{
					if (bGrabAxis)
					{
						PrevMousePos = MousePos;
						AxisDirection = U.Cross(V);
						HandleScreenDirection = CircleScreenPoints[0] - ScreenPoint;
						HandleScreenDirection.Normalize();
						bIsSelected = true;
						SelectedAxis = Axis;
					}
					if (bIsHoveredAxis == false)
					{
						bIsHoveredAxis = true;
						AxisColor = FVector4(1.f, 1.f, 0.f, 1.f);
					}
										
				}
			}

		}, AxisLength, NumSegments);

		for (int32 Index = 0; Index < NumSegments; ++Index)
		{
			int32 NextIndex = (Index + 1) % NumSegments;

			if (!bNoClipping)
			{
				FVector WorldLineCenterPoint = Lerp(CircleWorldPoints[Index], CircleWorldPoints[NextIndex], 0.5f);
				FVector CenterToLineCenterPoint = WorldLineCenterPoint - SceneComp.RelativeLocation;
				if (Dot(CenterToLineCenterPoint, CenterToCamera) < 0.f)
				{
					continue;
				}
			}

			HandleScreenSegments.emplace_back(CircleScreenPoints[Index], CircleScreenPoints[NextIndex]);

			Renderer.RenderLine2D(CircleScreenPoints[Index], CircleScreenPoints[NextIndex], AxisColor, 2.f);
		}

		Renderer.RenderCircle2D(Center, FVector4(0.8f, 0.8f, 0.8f, 1.f), 5.f);
	};

	bIsHoveredAxis = false;

	if (bDrawGizmo &&CurrentOperation == EGizmoOperation::Translate)
	{
		FVector ForwardAxis = bWorldMode ? Front : SceneComp.GetForward();
		FVector RightAxis = bWorldMode ? Right : SceneComp.GetRight();
		FVector UpAxis = bWorldMode ? Up : SceneComp.GetUp();

		DrawLineAxis(ForwardAxis, ForwardAxis, FVector4(1.f, 0.f, 0.f, 1.f), AxisEndPointStyle::Arrow, AxisNumber::X);
		DrawLineAxis(RightAxis, RightAxis, FVector4(0.f, 1.f, 0.f, 1.f), AxisEndPointStyle::Arrow, AxisNumber::Y);
		DrawLineAxis(UpAxis, UpAxis, FVector4(0.f, 0.f, 1.f, 1.f), AxisEndPointStyle::Arrow, AxisNumber::Z);
	}
	else if (bDrawGizmo && CurrentOperation == EGizmoOperation::Rotate)
	{
		FVector ForwardAxis = bWorldMode ? Front : SceneComp.GetForward();
		FVector RightAxis = bWorldMode ? Right : SceneComp.GetRight();
		FVector UpAxis = bWorldMode ? Up : SceneComp.GetUp();

		DrawCircleAxis(RightAxis, UpAxis, FVector4(1.f, 0.f, 0.f, 1.f),false, AxisNumber::X);
		DrawCircleAxis(ForwardAxis, UpAxis, FVector4(0.f, 1.f, 0.f, 1.f), false, AxisNumber::Y);
		DrawCircleAxis(RightAxis, ForwardAxis, FVector4(0.f, 0.f, 1.f, 1.f),false , AxisNumber::Z);

		FVector CameraAxisU = Cross(CenterToCamera, Up);
		CameraAxisU.Normalize();

		FVector CameraAxisV = Cross(CameraAxisU, CenterToCamera);
		CameraAxisV.Normalize();

		DrawCircleAxis(CameraAxisU, CameraAxisV, FVector4(1.f, 1.f, 1.f, 1.f), true, AxisNumber::Cameara);
	}
	else if (bDrawGizmo && CurrentOperation == EGizmoOperation::Scale)
	{
		FVector ForwardAxis = SceneComp.GetForward();
		FVector RightAxis = SceneComp.GetRight();
		FVector UpAxis = SceneComp.GetUp();

		DrawLineAxis(ForwardAxis, Front, FVector4(0.f, 1.f, 0.f, 1.f), AxisEndPointStyle::Circle, AxisNumber::X);
		DrawLineAxis(RightAxis, Right, FVector4(1.f, 0.f, 0.f, 1.f), AxisEndPointStyle::Circle , AxisNumber::Y);
		DrawLineAxis(UpAxis, Up, FVector4(0.f, 0.f, 1.f, 1.f), AxisEndPointStyle::Circle, AxisNumber::Z);
	}

	if (bIsSelected)
	{
		if (InputContext.IsMouseButtonPressed(0))
		{
			const float Sensitivity = 0.01f;

			FVector2 MouseDelta = MousePos - PrevMousePos;
			float Amount = Dot(MouseDelta, HandleScreenDirection);

			if (CurrentOperation == EGizmoOperation::Translate)
			{
				SceneComp.RelativeLocation += AxisDirection * Amount * Sensitivity;
			}
			else if (CurrentOperation == EGizmoOperation::Rotate)
			{
				FMatrix RotationMatrix = SceneComp.GetRotationMatrix();

				FQuaternion RotationQ = ToQuaternion(RotationMatrix);
				FQuaternion DeltaQ(AxisDirection, Amount * Sensitivity);
				FQuaternion FinalQ = DeltaQ * RotationQ;
				FinalQ.Normalize();

				SceneComp.RelativeRotation = ToEulerAngles(FinalQ) * Rad2Deg;
			}
			else if (CurrentOperation == EGizmoOperation::Scale)
			{
				SceneComp.RelativeScale3D += AxisDirection * Amount * Sensitivity;
			}

			PrevMousePos = MousePos;
		}

		if (InputContext.IsMouseButtonUp(0))
		{
			bIsSelected = false;
			SelectedAxis = AxisNumber::None;
		}
	}
}

bool FGizmo::IsMouseOverHandle() const
{
	FVector2 MousePos = FVector2(InputContext.GetMouseX(), InputContext.GetMouseY());

	for (const TPair<FVector2, FVector2>& Segment : HandleScreenSegments)
	{
		if (PointToLineSegmentDistanceSquared(MousePos, Segment.first, Segment.second) < HandleHitRadius * HandleHitRadius)
		{
			return true;
		}
	}

	return false;
}
