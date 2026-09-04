#pragma once
#include "UPrimitiveComponent.h"

void UPrimitiveComponent::Render(URenderer& Renderer)
{
	FMatrix model = GetModelMatrix();
	Renderer.UpdateModelConstant(model);
	Renderer.RenderPrimitive(VertexBuffer, NumVertices);
}

UPrimitiveComponent::~UPrimitiveComponent()
{
	if (VertexBuffer) VertexBuffer->Release();
}