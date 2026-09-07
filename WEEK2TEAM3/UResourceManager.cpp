#include "UResourceManager.h"
#include "URenderer.h"
#include "FVertex.h"

void UResourceManager::Initialize(URenderer& renderer)
{
	Renderer = &renderer;

	BuiltInStaticMeshes();
}

void UResourceManager::BuiltInStaticMeshes()
{
	TSharedPtr<FStaticMesh> CubeMesh = MakeShared<FStaticMesh>();
	CubeMesh->VertexCount = CubeVerticesSize;
	CubeMesh->VertexBuffer = Renderer->CreateVertexBuffer(CubeVertices, sizeof(FVertex) * CubeMesh->VertexCount);
	CubeMesh->Vertices = CubeVertices;
	StaticMeshMap["Cube"] = CubeMesh;

	TSharedPtr<FStaticMesh> SphereMesh = MakeShared<FStaticMesh>();
	SphereMesh->VertexCount = SphereVerticesSize;
	SphereMesh->VertexBuffer = Renderer->CreateVertexBuffer(SphereVertices, sizeof(FVertex) * SphereMesh->VertexCount);
	SphereMesh->Vertices = SphereVertices;
	StaticMeshMap["Sphere"] = SphereMesh;

	TSharedPtr<FStaticMesh> PlaneMesh = MakeShared<FStaticMesh>();
	PlaneMesh->VertexCount = PlaneVerticesSize;
	PlaneMesh->VertexBuffer = Renderer->CreateVertexBuffer(PlaneVertices, sizeof(FVertex) * PlaneMesh->VertexCount);
	PlaneMesh->Vertices = PlaneVertices;
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