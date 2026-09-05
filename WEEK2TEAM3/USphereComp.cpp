#pragma once
#include "USphereComp.h"

void USphereComp::Initialize(UResourceManager& ResourceManager)
{
	StaticMesh = ResourceManager.GetStaticMesh("Sphere");
}

bool USphereComp::CheckIntersection(const FRay& Ray) const
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
			FVector(SphereVertices[i].x, SphereVertices[i].y, SphereVertices[i].z),
			FVector(SphereVertices[i + 1].x, SphereVertices[i + 1].y, SphereVertices[i + 1].z),
			FVector(SphereVertices[i + 2].x, SphereVertices[i + 2].y, SphereVertices[i + 2].z)
		};

		FVector crossPoint;
		if (Raycast(LocalRay, Triangle, crossPoint))
		{
			return true;
		}
	}

	return false;
}
