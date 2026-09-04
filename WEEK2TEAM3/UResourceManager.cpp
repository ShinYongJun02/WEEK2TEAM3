#include "UResourceManager.h"
#include "URenderer.h"

extern FVertexSimple plane_vertices[];
extern FVertexSimple cube_vertices[];
extern FVertexSimple sphere_vertices[];

void UResourceManager::Initialize(URenderer& renderer)
{
	Renderer = &renderer;

	BuiltInStaticMeshes();
}

void UResourceManager::BuiltInStaticMeshes()
{
	TSharedPtr<FStaticMesh> CubeMesh = MakeShared<FStaticMesh>();
	CubeMesh->VertexCount = 36;
	CubeMesh->VertexBuffer = Renderer->CreateVertexBuffer(cube_vertices, sizeof(FVertexSimple) * CubeMesh->VertexCount);
	StaticMeshMap["Cube"] = CubeMesh;

	TSharedPtr<FStaticMesh> SphereMesh = MakeShared<FStaticMesh>();
	SphereMesh->VertexCount = 2400;
	SphereMesh->VertexBuffer = Renderer->CreateVertexBuffer(sphere_vertices, sizeof(FVertexSimple) * SphereMesh->VertexCount);
	StaticMeshMap["Sphere"] = SphereMesh;

	TSharedPtr<FStaticMesh> PlaneMesh = MakeShared<FStaticMesh>();
	PlaneMesh->VertexCount = 6;
	PlaneMesh->VertexBuffer = Renderer->CreateVertexBuffer(plane_vertices, sizeof(FVertexSimple) * PlaneMesh->VertexCount);
	StaticMeshMap["Plane"] = PlaneMesh;
}

void UResourceManager::Release()
{
	for (auto& [key, value] : StaticMeshMap)
	{
		if (value && value->VertexBuffer)
		{
			value->VertexBuffer->Release();
			value->VertexBuffer = nullptr;
		}
	}
	StaticMeshMap.clear();
}

TSharedPtr<FStaticMesh> UResourceManager::GetStaticMesh(const FString& NameKey)
{
	auto it = StaticMeshMap.find(NameKey);
	if (it != StaticMeshMap.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}