#pragma once

#include "UPrimitiveComponent.h"
#include "FVertex.h"

class UPlaneComp : public UPrimitiveComponent
{
	// scale 1.0 기준
	float halfX = 1.0f;
	float halfY = 1.0f;

public:
	UPlaneComp();

	UPlaneComp(URenderer& Renderer);

	~UPlaneComp();

	void setVertexBuffer(URenderer& Renderer);

	virtual bool IntersectLocal(const FRay& localRay, float& outT) const
	{
		if (localRay.Origin.z <= 0.0f || localRay.Direction.z >= 0.0f) return false;

		float t = -localRay.Origin.z / localRay.Direction.z;
		FVector z0 = localRay.Origin + localRay.Direction * t;

		if (z0.x <-halfX || z0.x > halfX || z0.y < -halfY || z0.y > halfY) return false;

		outT = t;
		return true;
	}
};
