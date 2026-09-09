#include "UPrimitiveComponent.h"

#include "FRenderer.h"

void UPrimitiveComponent::Render(FRenderer& Renderer)
{
	FMatrix Model = GetModelMatrix();
	Renderer.RenderPrimitive(StaticMesh->VertexBuffer, StaticMesh->VertexCount, Model);
}

TSharedPtr<FStaticMesh> UPrimitiveComponent::GetStaticMesh() const
{
	return StaticMesh;
}

FString UPrimitiveComponent::GetName()
{
	return (Name);
}
