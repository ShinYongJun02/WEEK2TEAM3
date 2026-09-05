#pragma once

#include "UPrimitiveComponent.h"
#include "FVertex.h"

class UPlaneComp : public UPrimitiveComponent
{
	GENERATED_BODY(UPlaneComp, UPrimitiveComponent)

public:
	UPlaneComp() = default;

	void Initialize(UResourceManager& ResourceManager);

	bool CheckIntersection(const FRay& Ray) const override;
};