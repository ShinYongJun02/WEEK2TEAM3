#include "UPlaneComp.h"

void UPlaneComp::Initialize(UResourceManager& ResourceManager)
{
	StaticMesh = ResourceManager.GetStaticMesh("Plane");
	Name = "Plane";
}

float UPlaneComp::CheckIntersection(const FRay& Ray) const
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
			FVector(PlaneVertices[Index].X, PlaneVertices[Index].Y, PlaneVertices[Index].Z),
			FVector(PlaneVertices[Index + 1].X, PlaneVertices[Index + 1].Y, PlaneVertices[Index + 1].Z),
			FVector(PlaneVertices[Index + 2].X, PlaneVertices[Index + 2].Y, PlaneVertices[Index + 2].Z)
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

