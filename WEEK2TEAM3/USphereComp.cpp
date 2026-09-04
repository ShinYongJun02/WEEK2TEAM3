#pragma once
#include "USphereComp.h"


USphereComp::USphereComp()
{
	NumVertices = SphereVerticesSize;
	VertexBuffer = nullptr;
}

USphereComp::USphereComp(URenderer& Renderer)
{
	NumVertices = SphereVerticesSize;
	VertexBuffer = Renderer.CreateVertexBuffer(SphereVertices, SphereVerticesSize * sizeof(FVertex));
}

USphereComp::~USphereComp()
{
	;
}

void USphereComp::setVertexBuffer(URenderer& Renderer)
{
	VertexBuffer = Renderer.CreateVertexBuffer(SphereVertices, SphereVerticesSize * sizeof(FVertex));
}