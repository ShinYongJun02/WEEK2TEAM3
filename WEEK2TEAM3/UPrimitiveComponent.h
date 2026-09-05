#pragma once

#include "Core.h"
#include "USceneComponent.h"
#include "UResourceManager.h"
#include "URenderer.h"
#include <wrl.h>

class UPrimitiveComponent : public USceneComponent
{
	GENERATED_BODY(UPrimitiveComponent, USceneComponent)

public:
	virtual ~UPrimitiveComponent() = default;

	virtual void Render(URenderer& Renderer);
	virtual bool CheckIntersection(const FRay& Ray) const = 0;

protected:
	TSharedPtr<FStaticMesh> StaticMesh = nullptr;
};
