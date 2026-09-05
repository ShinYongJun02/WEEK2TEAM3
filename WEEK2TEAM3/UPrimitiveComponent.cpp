#include "UPrimitiveComponent.h"

void UPrimitiveComponent::Render(URenderer& Renderer)
{
	if (!StaticMesh || StaticMesh->VertexBuffer == nullptr)
	{
		return;
	}

	FMatrix model = GetModelMatrix();
	Renderer.UpdateModelConstant(model);
	Renderer.RenderPrimitive(StaticMesh->VertexBuffer, StaticMesh->VertexCount);
}
