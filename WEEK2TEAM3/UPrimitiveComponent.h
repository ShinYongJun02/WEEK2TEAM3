#pragma once

#include "Core.h"
#include "USceneComponent.h"
#include "UResourceManager.h"
#include "FRenderer.h"
#include <wrl.h>

class UPrimitiveComponent : public USceneComponent
{
	GENERATED_BODY(UPrimitiveComponent, USceneComponent)

public:
	virtual ~UPrimitiveComponent() = default;

	virtual void Initialize(UResourceManager& ResourceManager) = 0;
	virtual void Render(FRenderer& Renderer);
	virtual float CheckIntersection(const FRay& Ray) const = 0;

	inline TSharedPtr<FStaticMesh> GetStaticMesh() const { return StaticMesh; }
	FString GetName();

protected:
	TSharedPtr<FStaticMesh> StaticMesh = nullptr;
	FString Name;
};
