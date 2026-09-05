#include "UCubeComp.h"
#include "Helper.h"

void UCubeComp::Initialize(UResourceManager& ResourceManager)
{
	StaticMesh = ResourceManager.GetStaticMesh("Cube");
}

bool UCubeComp::CheckIntersection(const FRay& Ray) const
{
	FMatrix ModelMatrix = GetModelMatrix();
	FMatrix InvModelMatrix = ModelMatrix.GetInverse();

	FVector4 LocalRayOrigin = FVector4(Ray.Origin, 1.0f) * InvModelMatrix;
	FVector4 LocalRayDirection = FVector4(Ray.Direction, 0.0f) * InvModelMatrix;

	FRay LocalRay;
	LocalRay.Origin = FVector(LocalRayOrigin.x, LocalRayOrigin.y, LocalRayOrigin.z);
	LocalRay.Direction = FVector(LocalRayDirection.x, LocalRayDirection.y, LocalRayDirection.z);

	for (uint32 i = 0; i < StaticMesh->VertexCount; i += 3)
	{
		FTriangle Triangle{
			FVector(CubeVertices[i].x, CubeVertices[i].y, CubeVertices[i].z),
			FVector(CubeVertices[i + 1].x, CubeVertices[i + 1].y, CubeVertices[i + 1].z),
			FVector(CubeVertices[i + 2].x, CubeVertices[i + 2].y, CubeVertices[i + 2].z)
		};

		FVector crossPoint;
		if (Raycast(LocalRay, Triangle, crossPoint))
		{
			return true;
		}
	}

	return false;
}
