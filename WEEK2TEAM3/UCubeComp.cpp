#pragma once
#include "UCubeComp.h"

UCubeComp::UCubeComp()
{
	NumVertices = CubeVerticesSize;
	VertexBuffer = nullptr;
}

UCubeComp::UCubeComp(URenderer& Renderer)
{
	NumVertices = CubeVerticesSize;
	VertexBuffer = Renderer.CreateVertexBuffer(CubeVertices, CubeVerticesSize * sizeof(FVertex));
}

UCubeComp::~UCubeComp()
{
	;
}

void UCubeComp::setVertexBuffer(URenderer& Renderer)
{
	VertexBuffer = Renderer.CreateVertexBuffer(CubeVertices, CubeVerticesSize * sizeof(FVertex));
}
