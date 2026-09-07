#include "UResourceManager.h"
#include "URenderer.h"

extern FVertex PlaneVertices[];
extern FVertex CubeVertices[];
extern FVertex SphereVertices[];

void UResourceManager::Initialize(URenderer& renderer)
{
	Renderer = &renderer;

	BuiltInStaticMeshes();
}

void SetMinMaxVertex(TSharedPtr<FStaticMesh>& StaticMesh, FVertex Vertex[])
{
	StaticMesh->MinVertex = FVector(Vertex[0].x, Vertex[0].y, Vertex[0].z);
	StaticMesh->MaxVertex = FVector(Vertex[0].x, Vertex[0].y, Vertex[0].z);



	for (int i = 1; i < StaticMesh->VertexCount; i++)
	{
		if (StaticMesh->MinVertex.x > Vertex[i].y)
		{
			StaticMesh->MinVertex.x = Vertex[i].x;
		}
		if (StaticMesh->MaxVertex.x < Vertex[i].x)
		{
			StaticMesh->MaxVertex.x = Vertex[i].x;
		}

		if (StaticMesh->MinVertex.y > Vertex[i].y)
		{
			StaticMesh->MinVertex.y = Vertex[i].y;
		}
		if (StaticMesh->MaxVertex.y < Vertex[i].y)
		{
			StaticMesh->MaxVertex.y = Vertex[i].y;
		}

		if (StaticMesh->MinVertex.z > Vertex[i].z)
		{
			StaticMesh->MinVertex.z = Vertex[i].z;
		}
		if (StaticMesh->MaxVertex.z < Vertex[i].z)
		{
			StaticMesh->MaxVertex.z = Vertex[i].z;
		}
	}
}

void UResourceManager::BuiltInStaticMeshes()
{
	TSharedPtr<FStaticMesh> CubeMesh = MakeShared<FStaticMesh>();
	CubeMesh->VertexCount = 36;
	CubeMesh->VertexBuffer = Renderer->CreateVertexBuffer(CubeVertices, sizeof(FVertexSimple) * CubeMesh->VertexCount);
	SetMinMaxVertex(CubeMesh, CubeVertices);
	StaticMeshMap["Cube"] = CubeMesh;

	TSharedPtr<FStaticMesh> SphereMesh = MakeShared<FStaticMesh>();
	SphereMesh->VertexCount = 2400;
	SphereMesh->VertexBuffer = Renderer->CreateVertexBuffer(SphereVertices, sizeof(FVertexSimple) * SphereMesh->VertexCount);
	SetMinMaxVertex(SphereMesh, SphereVertices);
	StaticMeshMap["Sphere"] = SphereMesh;

	TSharedPtr<FStaticMesh> PlaneMesh = MakeShared<FStaticMesh>();
	PlaneMesh->VertexCount = 6;
	PlaneMesh->VertexBuffer = Renderer->CreateVertexBuffer(PlaneVertices, sizeof(FVertexSimple) * PlaneMesh->VertexCount);
	SetMinMaxVertex(PlaneMesh, PlaneVertices);
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