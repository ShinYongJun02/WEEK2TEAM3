#pragma once
#include "UPrimitiveComponent.h"
#include "FVertex.h"
#include <cfloat>

class UCubeComp : public UPrimitiveComponent
{
	// scale 1.0 기준
	float halfX = 0.5f;
	float halfY = 0.5f;
	float halfZ = 0.5f;

public:
	DECLARE_CLASS(USceneComponent, UObject)
	UCubeComp();

	UCubeComp(URenderer& Renderer);

	~UCubeComp();

	void setVertexBuffer(URenderer& Renderer);

	virtual bool IntersectLocal(const FRay& localRay, float& outT) const
	{
		const float o[3] = { localRay.Origin.x, localRay.Origin.y, localRay.Origin.z };
		const float d[3] = { localRay.Direction.x, localRay.Direction.y, localRay.Direction.z };
		const float h[3] = { halfX, halfY, halfZ };

		float tMin = 0.0f; // 축의 범위에 들어가는데 걸린 t
		float tMax = FLT_MAX; // 축의 범위에서 나올때까지 걸린 t

		for (int i = 0;i < 3;i++)
		{
			if (fabs(d[i]) <= 1e-8f)
			{
				// 해당 축에 수직이면서 범위에 들어가지 않음
				if (o[i] < -h[i] || o[i] > h[i]) return false;

				// 해당 축에 수직이면서 범위에 들어감
				continue;
			}

			float inv = 1.0f / d[i];
			float t1 = (-h[i] - o[i]) * inv;
			float t2 = (h[i] - o[i]) * inv;
			if (t1 > t2) { float tmp = t1; t1 = t2; t2 = tmp; }

			if (t1 > tMin) tMin = t1;
			if (t2 < tMax) tMax = t2;
			if (tMin > tMax) return false;
		}

		outT = tMin;
		return true;
	}
};
