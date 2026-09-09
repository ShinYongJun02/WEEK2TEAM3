#pragma once
#include "UPrimitiveComponent.h"

void UPrimitiveComponent::Render(FRenderer& Renderer)
{
	FMatrix Model = GetModelMatrix();
	Renderer.RenderPrimitive(StaticMesh->VertexBuffer, StaticMesh->VertexCount, Model);
}

FString UPrimitiveComponent::GetName()
{
	return (Name);
}
