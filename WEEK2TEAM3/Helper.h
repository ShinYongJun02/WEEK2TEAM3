#pragma once

#include <Core.h>
#include <cmath>

constexpr float Epsilon = 1e-6f;

static float DegreeToRadian(float degree)
{
	float radian = degree * (float)acos(-1) / 180.0f;
	return radian;
}

static bool EpsilonEqual(float a, float b, float epsilon = Epsilon)
{
	return std::fabs(a - b) < epsilon;
}

static float Remap(float value, float inMin, float inMax, float outMin, float outMax)
{
	return (value - inMin) / (inMax - inMin) * (outMax - outMin) + outMin;
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