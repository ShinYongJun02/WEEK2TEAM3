#pragma once

#include "USceneComponent.h"
#include "UResourceManager.h"

class FRenderer;

class UPrimitiveComponent : public USceneComponent
{
	GENERATED_BODY(UPrimitiveComponent, USceneComponent)

public:
	virtual ~UPrimitiveComponent() = default;

	virtual void Initialize(UResourceManager& ResourceManager) = 0;
	virtual void Render(FRenderer& Renderer);
	virtual float CheckIntersection(const FRay& Ray) const = 0;

	TSharedPtr<FStaticMesh> GetStaticMesh() const;
	FString GetName();

protected:
	TSharedPtr<FStaticMesh> StaticMesh = nullptr;
	FString Name;
};
