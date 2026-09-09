#include "UResourceManager.h"
#include "FRenderer.h"

extern FVertex PlaneVertices[];
extern FVertex CubeVertices[];
extern FVertex SphereVertices[];

void UResourceManager::Initialize(FRenderer& InRenderer)
{
	Renderer = &InRenderer;

	BuiltInStaticMeshes();
}

void SetMinMaxVertex(TSharedPtr<FStaticMesh>& StaticMesh, FVertex Vertex[])
{
	StaticMesh->MinVertex = FVector(Vertex[0].X, Vertex[0].Y, Vertex[0].Z);
	StaticMesh->MaxVertex = FVector(Vertex[0].X, Vertex[0].Y, Vertex[0].Z);



	for (int Index = 1; Index < StaticMesh->VertexCount; Index++)
	{
		if (StaticMesh->MinVertex.X > Vertex[Index].X)
		{
			StaticMesh->MinVertex.X = Vertex[Index].X;
		}
		if (StaticMesh->MaxVertex.X < Vertex[Index].X)
		{
			StaticMesh->MaxVertex.X = Vertex[Index].X;
		}

		if (StaticMesh->MinVertex.Y > Vertex[Index].Y)
		{
			StaticMesh->MinVertex.Y = Vertex[Index].Y;
		}
		if (StaticMesh->MaxVertex.Y < Vertex[Index].Y)
		{
			StaticMesh->MaxVertex.Y = Vertex[Index].Y;
		}

		if (StaticMesh->MinVertex.Z > Vertex[Index].Z)
		{
			StaticMesh->MinVertex.Z = Vertex[Index].Z;
		}
		if (StaticMesh->MaxVertex.Z < Vertex[Index].Z)
		{
			StaticMesh->MaxVertex.Z = Vertex[Index].Z;
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
	for (auto& [Key, Value] : StaticMeshMap)
	{
		if (Value && Value->VertexBuffer)
		{
			Value->VertexBuffer->Release();
			Value->VertexBuffer = nullptr;
		}
	}
	StaticMeshMap.clear();
}

TSharedPtr<FStaticMesh> UResourceManager::GetStaticMesh(const FString& NameKey)
{
	auto It = StaticMeshMap.find(NameKey);
	if (It != StaticMeshMap.end())
	{
		return It->second;
	}
	else
	{
		return nullptr;
	}
}