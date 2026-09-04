#pragma once
#include "UPlaneComp.h"


UPlaneComp::UPlaneComp()
{
	NumVertices = PlaneVerticesSize;
	VertexBuffer = nullptr;
}

UPlaneComp::UPlaneComp(URenderer& Renderer)
{
	NumVertices = PlaneVerticesSize;
	VertexBuffer = Renderer.CreateVertexBuffer(PlaneVertices, PlaneVerticesSize * sizeof(FVertex));
}

UPlaneComp::~UPlaneComp()
{
	;
}

void UPlaneComp::setVertexBuffer(URenderer& Renderer)
{
	VertexBuffer = Renderer.CreateVertexBuffer(PlaneVertices, PlaneVerticesSize * sizeof(FVertex));
}