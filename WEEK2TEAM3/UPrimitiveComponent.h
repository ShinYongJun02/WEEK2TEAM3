#pragma once
#include "USceneComponent.h"
#include "URenderer.h"
#include <wrl.h>

class UPrimitiveComponent : public USceneComponent
{
public:
	DECLARE_CLASS(USceneComponent, UObject)
	Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer = nullptr;
	uint32 NumVertices = 0;

	virtual void Render(URenderer& Renderer);

	virtual ~UPrimitiveComponent();

	bool RayIntersects(const FRay& worldRay, float& outT) const
	{
		FMatrix inv = GetModelInverseMatrix();

		FRay local;
		local.Origin = (FVector4(worldRay.Origin, 1.0f) * inv).Vector3();
		local.Direction = (FVector4(worldRay.Direction, 0.0f) * inv).Vector3();

		return IntersectLocal(local, outT);
	}

	// Möller–Trumbore 방식으로 변환할 때 여기만 수정
	virtual bool IntersectLocal(const FRay& localRay, float& outT) const { return false; };
};
