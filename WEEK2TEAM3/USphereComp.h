#pragma once

#include "UPrimitiveComponent.h"

class USphereComp : public UPrimitiveComponent
{
	GENERATED_BODY(USphereComp, UPrimitiveComponent)

public:
	USphereComp() = default;

	void Initialize(UResourceManager& ResourceManager) override;

	float CheckIntersection(const FRay& Ray) const override;
};
