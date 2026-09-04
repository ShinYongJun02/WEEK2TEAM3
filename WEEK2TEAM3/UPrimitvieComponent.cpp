#include "UPrimitvieComponent.h"
#include "URenderer.h"

extern FVertexSimple plane_vertices[];
extern FVertexSimple cube_vertices[];
extern FVertexSimple sphere_vertices[];

UPrimitiveComponent::UPrimitiveComponent(URenderer& renderer)
	: Renderer(renderer)
	, VertexBuffer(nullptr)
	, VertexCount(0)
{
}

void UPrimitiveComponent::Render()
{
	Renderer.RenderPrimitive(VertexBuffer, VertexCount);
}

UCubeComp::UCubeComp(URenderer& renderer)
	: UPrimitiveComponent(renderer)
{
	VertexCount = 36;
	VertexBuffer = renderer.CreateVertexBuffer(cube_vertices, sizeof(FVertexSimple) * VertexCount);
}

USphereComp::USphereComp(URenderer& renderer)
	: UPrimitiveComponent(renderer)
{
	VertexCount = 2400;
	VertexBuffer = renderer.CreateVertexBuffer(sphere_vertices, sizeof(FVertexSimple) * VertexCount);
}

UPlaneComp::UPlaneComp(URenderer& renderer)
	: UPrimitiveComponent(renderer)
{
	VertexCount = 6;
	VertexBuffer = renderer.CreateVertexBuffer(plane_vertices, sizeof(FVertexSimple) * VertexCount);
}

