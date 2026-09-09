#pragma once

#include "Core.h"

struct FVertex
{
	float X, Y, Z;    // Position
	float R, G, B, A; // Color
};

extern FVertex CubeVertices[];
extern FVertex SphereVertices[];
extern FVertex PlaneVertices[];
