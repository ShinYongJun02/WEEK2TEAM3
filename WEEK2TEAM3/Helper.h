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

static float DistanceSquared(const FVector& a, const FVector& b)
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

static FVector2 WorldToScreen(const FVector& worldPos, const FMatrix& viewProjection, int32 screenWidth, int32 screenHeight)
{
	FVector4 clipSpacePos = FVector4(worldPos.x, worldPos.y, worldPos.z, 1.0f) * viewProjection;

	FVector2 ndcPos(clipSpacePos.x / clipSpacePos.w, clipSpacePos.y / clipSpacePos.w);
	FVector2 screenPos(
		(ndcPos.x + 1.0f) * 0.5f * screenWidth,
		(1.0f - (ndcPos.y + 1.0f) * 0.5f) * screenHeight
	);

	return screenPos;
}

static bool LineSegmentIntersect(const FVector2& p0, const FVector2& p1, const FVector2& p2, const FVector2& p3, FVector2& intersection)
{
	float denominator = (p0.x - p1.x) * (p2.y - p3.y) - (p0.y - p1.y) * (p2.x - p3.x);

	if (EpsilonEqual(denominator, 0.0f))
	{
		return false; // Lines are parallel or coincident
	}

	float t = ((p0.x - p2.x) * (p2.y - p3.y) - (p0.y - p2.y) * (p2.x - p3.x)) / denominator;
	float u = ((p0.x - p2.x) * (p0.y - p1.y) - (p0.y - p2.y) * (p0.x - p1.x)) / denominator;

	if (t < 0.f || t > 1.f || u < 0.f || u > 1.f)
	{
		return false;
	}

	intersection.x = p0.x + t * (p1.x - p0.x);
	intersection.y = p0.y + t * (p1.y - p0.y);

	return true;
}

static float PointToLineSegmentDistanceSquared(const FVector2& point, const FVector2& lineStart, const FVector2& lineEnd)
{
	FVector2 lineVec = lineEnd - lineStart;

	float lineLength = lineVec.Length();
	if (lineLength == 0.f)
	{
		return DistanceSquared(point, lineStart);
	}
	lineVec /= lineLength;

	FVector2 startToPoint = point - lineStart;
	float projectedLength = Dot(startToPoint, lineVec);

	if (projectedLength < 0.f)
	{
		projectedLength = 0.f;
	}
	else if (projectedLength > lineLength)
	{
		projectedLength = lineLength;
	}

	FVector2 closest = lineStart + lineVec * projectedLength;
	return DistanceSquared(point, closest);
}

static float PointToLineSegmentDistanceSquared(const FVector& point, const FVector& lineStart, const FVector& lineEnd)
{
	FVector lineVec = lineEnd - lineStart;

	float lineLength = lineVec.Length();
	if (lineLength == 0.f)
	{
		return DistanceSquared(point, lineStart);
	}

	lineVec /= lineLength;

	FVector startToPoint = point - lineStart;
	float projectedLength = Dot(startToPoint, lineVec);

	if (projectedLength < 0.f)
	{
		projectedLength = 0.f;
	}
	else if (projectedLength > lineLength)
	{
		projectedLength = lineLength;
	}

	FVector closest = lineStart + lineVec * projectedLength;
	return DistanceSquared(point, closest);
}

static void GenerateCircleVertices(const std::function<void(int32 index, const FVector2&)>& handler, const FVector2& center, float radius, int segments)
{
	const float step = 2.0f * PI / static_cast<float>(segments);

	for (int32 i = 0; i < segments; ++i)
	{
		float angle = step * static_cast<float>(i);
		float x = center.x + radius * cos(angle);
		float y = center.y + radius * sin(angle);

		handler(i, FVector2(x, y));
	}
}

