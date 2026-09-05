#pragma once
#include "USceneComponent.h"
#include "URenderer.h"
#include "UResourceManager.h"

class UPrimitiveComponent : public USceneComponent
{
public:
	DECLARE_CLASS(UPrimitiveComponent, USceneComponent)

	// UResourceManager 가 소유하는 메시를 참조만 한다.
	// 같은 종류의 프리미티브는 하나의 버텍스 버퍼를 공유하므로
	// 여기서 버퍼를 만들거나 해제하지 않는다.
	TSharedPtr<FStaticMesh> StaticMesh = nullptr;

	virtual void Render(URenderer& Renderer);

	virtual ~UPrimitiveComponent() = default;

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
