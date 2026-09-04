#pragma once

#include <cmath>

static float DegreeToRadian(float degree)
{
	float radian = degree * (float)acos(-1) / 180.0f;
	return radian;
}