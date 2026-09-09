#include "UCubeComp.h"

#include "FVertex.h"
#include "Helper.h"

void UCubeComp::Initialize(UResourceManager& ResourceManager)
{
	StaticMesh = ResourceManager.GetStaticMesh("Cube");
	Name = "Cube";
}

float UCubeComp::CheckIntersection(const FRay& Ray) const
{
	FMatrix ModelMatrix = GetModelMatrix();
	FMatrix InvModelMatrix = ModelMatrix.GetInverse();

	FVector4 LocalRayOrigin = FVector4(Ray.Origin, 1.0f) * InvModelMatrix;
	FVector4 LocalRayDirection = FVector4(Ray.Direction, 0.0f) * InvModelMatrix;

	FRay LocalRay;
	LocalRay.Origin = FVector(LocalRayOrigin.X, LocalRayOrigin.Y, LocalRayOrigin.Z);
	LocalRay.Direction = FVector(LocalRayDirection.X, LocalRayDirection.Y, LocalRayDirection.Z);

	if (!(FRay::CheckAABB(LocalRay, StaticMesh.get()->MinVertex, StaticMesh.get()->MaxVertex)))
	{
		return -1.0f;
	}

	float ClosestT = -1.0f;
	for (uint32 Index = 0; Index < StaticMesh->VertexCount; Index += 3)
	{
		FTriangle Triangle{
			FVector(CubeVertices[Index].X, CubeVertices[Index].Y, CubeVertices[Index].Z),
			FVector(CubeVertices[Index + 1].X, CubeVertices[Index + 1].Y, CubeVertices[Index + 1].Z),
			FVector(CubeVertices[Index + 2].X, CubeVertices[Index + 2].Y, CubeVertices[Index + 2].Z)
		};

		float T = Raycast(LocalRay, Triangle);
		if (T >= 0.0f)
		{
			if (ClosestT < 0.0f || T < ClosestT)
			{
				ClosestT = T;
			}
		}
	}

	return ClosestT;
}
