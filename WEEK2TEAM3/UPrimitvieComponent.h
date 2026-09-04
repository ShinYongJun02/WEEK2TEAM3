#pragma once

#include "USceneComponent.h"
#include "UResourceManager.h"

class URenderer;

class UPrimitiveComponent : public USceneComponent
{
public:
	void Render(URenderer& renderer);

protected:
	TSharedPtr<FStaticMesh> StaticMesh;
};

class UCubeComp : public UPrimitiveComponent
{
public:
	UCubeComp(UResourceManager& resourceManager);
};

class USphereComp : public UPrimitiveComponent
{
public:
	USphereComp(UResourceManager& resourceManager);
};

class UPlaneComp : public UPrimitiveComponent
{
public:
	UPlaneComp(UResourceManager& resourceManager);
};