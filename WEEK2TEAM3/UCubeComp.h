#pragma once

#include "UPrimitiveComponent.h"

class UCubeComp : public UPrimitiveComponent
{
	GENERATED_BODY(UCubeComp, UPrimitiveComponent)

public:
	UCubeComp() = default;

	void Initialize(UResourceManager& ResourceManager) override;

	float CheckIntersection(const FRay& Ray) const override;
};
