#include "UPrimitvieComponent.h"
#include "URenderer.h"

void UPrimitiveComponent::Render(URenderer& renderer)
{
	renderer.UpdateModelConstant(GetModelMatrix());
	renderer.RenderPrimitive(StaticMesh->VertexBuffer, StaticMesh->VertexCount);
}

UCubeComp::UCubeComp(UResourceManager& resourceManager)
{
	StaticMesh = resourceManager.GetStaticMesh("Cube");
}

USphereComp::USphereComp(UResourceManager& resourceManager)
{
	StaticMesh = resourceManager.GetStaticMesh("Sphere");
}

UPlaneComp::UPlaneComp(UResourceManager& resourceManager)
{
	StaticMesh = resourceManager.GetStaticMesh("Plane");
}

