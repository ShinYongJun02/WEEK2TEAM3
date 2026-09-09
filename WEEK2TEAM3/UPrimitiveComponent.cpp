#pragma once
#include "UPrimitiveComponent.h"

void UPrimitiveComponent::Render(URenderer& Renderer)
{
	FMatrix model = GetModelMatrix();
	Renderer.RenderPrimitive(StaticMesh->VertexBuffer, StaticMesh->VertexCount, model);
}

FString UPrimitiveComponent::GetName()
{
	return (Name);
}
