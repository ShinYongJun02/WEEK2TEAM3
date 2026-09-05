#pragma once
#include "UPrimitiveComponent.h"
#include "FVertex.h"

class USphereComp : public UPrimitiveComponent
{
	GENERATED_BODY(USphereComp, UPrimitiveComponent)

public:
	USphereComp() = default;

	void Initialize(UResourceManager& ResourceManager);

	bool CheckIntersection(const FRay& Ray) const override;
};