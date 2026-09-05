#pragma once
#include "UPrimitiveComponent.h"
#include "FVertex.h"

class UCubeComp : public UPrimitiveComponent
{
	GENERATED_BODY(UCubeComp, UPrimitiveComponent)

public:
	UCubeComp() = default;

	void Initialize(UResourceManager& ResourceManager);

	bool CheckIntersection(const FRay& Ray) const override;
};