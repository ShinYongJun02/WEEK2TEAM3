#pragma once

#include <Core.h>
#include <cmath>

constexpr float PI = 3.14159265358979323846f;
constexpr float Epsilon = 1e-6f;

static float DegreeToRadian(float degree)
{
	return degree * PI / 180.0f;
}

static bool EpsilonEqual(float a, float b, float epsilon = Epsilon)
{
	return std::fabs(a - b) < epsilon;
}

static float Remap(float value, float inMin, float inMax, float outMin, float outMax)
{
	return (value - inMin) / (inMax - inMin) * (outMax - outMin) + outMin;
}

static FMatrix Translate(float x, float y, float z)
{
	return FMatrix(
		FVector4(1.0f, 0.0f, 0.0f, 0.0f),
		FVector4(0.0f, 1.0f, 0.0f, 0.0f),
		FVector4(0.0f, 0.0f, 1.0f, 0.0f),
		FVector4(x, y, z, 1.0f)
	);
}

static FMatrix Scale(float x, float y, float z)
{
	return FMatrix(
		FVector4(x, 0.0f, 0.0f, 0.0f),
		FVector4(0.0f, y, 0.0f, 0.0f),
		FVector4(0.0f, 0.0f, z, 0.0f),
		FVector4(0.0f, 0.0f, 0.0f, 1.0f)
	);
}

static FMatrix RotateX(float angle)
{
	float radian = DegreeToRadian(angle);
	float cosA = cos(radian);
	float sinA = sin(radian);
	return FMatrix(
		FVector4(1.0f, 0.0f, 0.0f, 0.0f),
		FVector4(0.0f, cosA, sinA, 0.0f),
		FVector4(0.0f, -sinA, cosA, 0.0f),
		FVector4(0.0f, 0.0f, 0.0f, 1.0f)
	);
}

static FMatrix RotateY(float angle)
{
	float radian = DegreeToRadian(angle);
	float cosA = cos(radian);
	float sinA = sin(radian);
	return FMatrix(
		FVector4(cosA, 0.0f, -sinA, 0.0f),
		FVector4(0.0f, 1.0f, 0.0f, 0.0f),
		FVector4(sinA, 0.0f, cosA, 0.0f),
		FVector4(0.0f, 0.0f, 0.0f, 1.0f)
	);
}

static FMatrix RotateZ(float angle)
{
	float radian = DegreeToRadian(angle);
	float cosA = cos(radian);
	float sinA = sin(radian);
	return FMatrix(
		FVector4(cosA, sinA, 0.0f, 0.0f),
		FVector4(-sinA, cosA, 0.0f, 0.0f),
		FVector4(0.0f, 0.0f, 1.0f, 0.0f),
		FVector4(0.0f, 0.0f, 0.0f, 1.0f)
	);
}

static FMatrix Rotate(float angleX, float angleY, float angleZ)
{
	return RotateX(angleX) * RotateY(angleY) * RotateZ(angleZ);
}

static FMatrix Ortho(float left, float right, float bottom, float top, float nearZ, float farZ)
{
	return FMatrix{
		FVector4(2.0f / (right - left), 0.0f, 0.0f, 0.0f),
		FVector4(0.0f, 2.0f / (top - bottom), 0.0f, 0.0f),
		FVector4(0.0f, 0.0f, 1.0f / (farZ - nearZ), 0.0f),
		FVector4(-(right + left) / (right - left), -(top + bottom) / (top - bottom), -nearZ / (farZ - nearZ), 1.0f)
	};
}

static float DistanceSquared(const FVector2& a, const FVector2& b)
{
	return (a - b).LengthSquared();
}

static bool Raycast(const FRay& ray, const FTriangle& triangle, FVector& outPoint)
{
	FVector E1 = triangle.P1 - triangle.P0;
	FVector E2 = triangle.P2 - triangle.P0;

	FVector H = Cross(ray.Direction, E2);

	float det = Dot(E1, H);
	if (EpsilonEqual(det, 0.0f))
	{
		return false; // Ray is parallel to the triangle
	}

	FVector S = ray.Origin - triangle.P0;

	float u = Dot(S, H) / det;
	if (u < 0.0f || u > 1.0f)
	{
		return false; // Intersection point is outside the triangle
	}

	FVector Q = Cross(S, E1);

	float v = Dot(ray.Direction, Q) / det;
	if (v < 0.0f || u + v > 1.0f)
	{
		return false; // Intersection point is outside the triangle
	}

	float t = Dot(E2, Q) / det;
	if (t < Epsilon)
	{
		return false; // Intersection point is behind the ray origin
	}

	outPoint = ray.Origin + ray.Direction * t;
	return true;
}

static FVector2 WorldToScreen(const FVector& worldPos, const FMatrix& viewProjection, int screenWidth, int screenHeight)
{
	FVector4 clipSpacePos = FVector4(worldPos.x, worldPos.y, worldPos.z, 1.0f) * viewProjection;

	FVector2 ndcPos(clipSpacePos.x / clipSpacePos.w, clipSpacePos.y / clipSpacePos.w);
	FVector2 screenPos(
		(ndcPos.x + 1.0f) * 0.5f * screenWidth,
		(1.0f - (ndcPos.y + 1.0f) * 0.5f) * screenHeight
	);

	return screenPos;
}