#pragma once
#include "UEngineStatics.h"
#include "Types.h"

struct FVertex
{
	float x, y, z;    // Position
	float r, g, b, a; // Color
};

extern FVertex CubeVertices[];
extern int32 CubeVerticesSize;
extern FVertex TriangleVertices[];
extern int32 TriangleVerticesSize;
extern FVertex SphereVertices[];
extern int32 SphereVerticesSize;
extern FVertex PlaneVertices[];
extern int32 PlaneVerticesSize;
