#include "UPlaneComp.h"

void UPlaneComp::Initialize(UResourceManager& ResourceManager)
{
	StaticMesh = ResourceManager.GetStaticMesh("Plane");
}

bool UPlaneComp::CheckIntersection(const FRay& Ray) const
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
			FVector(PlaneVertices[i].x, PlaneVertices[i].y, PlaneVertices[i].z),
			FVector(PlaneVertices[i + 1].x, PlaneVertices[i + 1].y, PlaneVertices[i + 1].z),
			FVector(PlaneVertices[i + 2].x, PlaneVertices[i + 2].y, PlaneVertices[i + 2].z)
		};

		FVector crossPoint;
		if (Raycast(LocalRay, Triangle, crossPoint))
		{
			return true;
		}
	}

	return false;
}

